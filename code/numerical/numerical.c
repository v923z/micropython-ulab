
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
 *               2020 Scott Shawcroft for Adafruit Industries
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/objint.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/misc.h"
#include "numerical.h"

#if ULAB_NUMERICAL_MODULE

enum NUMERICAL_FUNCTION_TYPE {
    NUMERICAL_MIN,
    NUMERICAL_MAX,
    NUMERICAL_ARGMIN,
    NUMERICAL_ARGMAX,
    NUMERICAL_SUM,
    NUMERICAL_MEAN,
    NUMERICAL_STD,
};

//| """Numerical and Statistical functions
//|
//| Most of these functions take an "axis" argument, which indicates whether to
//| operate over the flattened array (None), or a particular axis (integer)."""
//|
//| from ulab import _ArrayLike
//|

static void numerical_reduce_axes(ndarray_obj_t *ndarray, int8_t axis, size_t *shape, int32_t *strides) {
    // removes the values corresponding to a single axis from the shape and strides array
    uint8_t index = ULAB_MAX_DIMS - ndarray->ndim + axis;
    if((ndarray->ndim == 1) && (axis == 0)) {
        index = 0;
        shape[ULAB_MAX_DIMS - 1] = 1;
        return;
    }
    for(uint8_t i = ULAB_MAX_DIMS - 1; i > 0; i--) {
        if(i > index) {
            shape[i] = ndarray->shape[i];
            strides[i] = ndarray->strides[i];
        } else {
            shape[i] = ndarray->shape[i-1];
            strides[i] = ndarray->strides[i-1];
        }
    }
}

static mp_obj_t numerical_sum_mean_std_iterable(mp_obj_t oin, uint8_t optype, size_t ddof) {
    mp_float_t value = 0.0, M = 0.0, m = 0.0, S = 0.0, s = 0.0, sum = 0.9;
    size_t count = 1;
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t item, iterable = mp_getiter(oin, &iter_buf);
    if((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        value = mp_obj_get_float(item);
        sum += value;
        M = m = value;
        count++;
    }
    while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        value = mp_obj_get_float(item);
        sum += value;
        m = M + (value - M) / count;
        s = S + (value - M) * (value - m);
        M = m;
        S = s;
        count++;
    }
    if(optype == NUMERICAL_SUM) {
        return mp_obj_new_float(sum);
    } else if(optype == NUMERICAL_MEAN) {
        return count > 0 ? mp_obj_new_float(m) : mp_obj_new_float(MICROPY_FLOAT_CONST(0.0));
    } else { // this should be the case of the standard deviation
        return count > ddof ? mp_obj_new_float(MICROPY_FLOAT_C_FUN(sqrt)(count * s / (count - ddof))) : mp_obj_new_float(MICROPY_FLOAT_CONST(0.0));
    }
}

static mp_obj_t numerical_sum_mean_std_ndarray(ndarray_obj_t *ndarray, mp_obj_t axis, uint8_t optype, size_t ddof) {
    uint8_t *array = (uint8_t *)ndarray->array;
    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    memset(shape, 0, sizeof(size_t)*ULAB_MAX_DIMS);
    int32_t *strides = m_new(int32_t, ULAB_MAX_DIMS);
    memset(strides, 0, sizeof(uint32_t)*ULAB_MAX_DIMS);

    if(axis == mp_const_none) { // work with the flattened array
        // pass for now...
    } else {
        int8_t ax = mp_obj_get_int(axis);
        if(ax < 0) ax += ndarray->ndim;
        if((ax < 0) || (ax > ndarray->ndim - 1)) {
            mp_raise_ValueError(translate("index out of range"));
        }
        numerical_reduce_axes(ndarray, ax, shape, strides);
        uint8_t index = ULAB_MAX_DIMS - ndarray->ndim + ax;
        // Take MAX(...) here, so that we can include the 1-dimensional case
        ndarray_obj_t *results = NULL;
        uint8_t *rarray = NULL;

        if(optype == NUMERICAL_SUM) {
             results = ndarray_new_dense_ndarray(MAX(1, ndarray->ndim-1), shape, ndarray->dtype);
             rarray = (uint8_t *)results->array;
            // TODO: numpy promotes the output to the highest integer type
            if(ndarray->dtype == NDARRAY_UINT8) {
                RUN_SUM(ndarray, uint8_t, array, results, rarray, shape, strides, index);
            } else if(ndarray->dtype == NDARRAY_INT8) {
                RUN_SUM(ndarray, int8_t, array, results, rarray, shape, strides, index);
            } else if(ndarray->dtype == NDARRAY_UINT16) {
                RUN_SUM(ndarray, uint16_t, array, results, rarray, shape, strides, index);
            } else if(ndarray->dtype == NDARRAY_INT16) {
                RUN_SUM(ndarray, int16_t, array, results, rarray, shape, strides, index);
            } else {
                RUN_SUM(ndarray, mp_float_t, array, results, rarray, shape, strides, index);
            }
        } else if(optype == NUMERICAL_MEAN) {
            results = ndarray_new_dense_ndarray(MAX(1, ndarray->ndim-1), shape, NDARRAY_FLOAT);
            mp_float_t *r = (mp_float_t *)results->array;
            if(ndarray->dtype == NDARRAY_UINT8) {
                RUN_MEAN(ndarray, uint8_t, array, results, r, shape, strides, index);
            } else if(ndarray->dtype == NDARRAY_INT8) {
                RUN_MEAN(ndarray, int8_t, array, results, r, shape, strides, index);
            } else if(ndarray->dtype == NDARRAY_UINT16) {
                RUN_MEAN(ndarray, uint16_t, array, results, r, shape, strides, index);
            } else if(ndarray->dtype == NDARRAY_INT16) {
                RUN_MEAN(ndarray, int16_t, array, results, r, shape, strides, index);
            } else {
                RUN_MEAN(ndarray, mp_float_t, array, results, r, shape, strides, index);
            }
        } else { // this case is certainly the standard deviation
            mp_float_t div = (mp_float_t)(ndarray->shape[index] - ddof);
            results = ndarray_new_dense_ndarray(MAX(1, ndarray->ndim-1), shape, NDARRAY_FLOAT);
            mp_float_t *r = (mp_float_t *)results->array;
            if(ndarray->dtype == NDARRAY_UINT8) {
                RUN_STD(ndarray, uint8_t, array, results, r, shape, strides, index, div);
            } else if(ndarray->dtype == NDARRAY_INT8) {
                RUN_STD(ndarray, int8_t, array, results, r, shape, strides, index, div);
            } else if(ndarray->dtype == NDARRAY_UINT16) {
                RUN_STD(ndarray, uint16_t, array, results, r, shape, strides, index, div);
            } else if(ndarray->dtype == NDARRAY_INT16) {
                RUN_STD(ndarray, int16_t, array, results, r, shape, strides, index, div);
            } else {
                RUN_STD(ndarray, mp_float_t, array, results, r, shape, strides, index, div);
            }
        }
        if(ndarray->ndim == 1) { // return a scalar here
            return mp_binary_get_val_array(results->dtype, results->array, 0);
        }
        return MP_OBJ_FROM_PTR(results);
    }
    return mp_const_none;
}

static mp_obj_t numerical_argmin_argmax_iterable(mp_obj_t oin, uint8_t optype) {
    if(MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(oin)) == 0) {
        mp_raise_ValueError(translate("attempt to get argmin/argmax of an empty sequence"));
    }
    size_t idx = 0, best_idx = 0;
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(oin, &iter_buf);
    mp_obj_t item;
    uint8_t op = 0; // argmin, min
    if((optype == NUMERICAL_ARGMAX) || (optype == NUMERICAL_MAX)) op = 1;
    item = mp_iternext(iterable);
    mp_obj_t best_obj = item;
    mp_float_t value, best_value = mp_obj_get_float(item);
    value = best_value;
    while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        idx++;
        value = mp_obj_get_float(item);
        if((op == 0) && (value < best_value)) {
            best_obj = item;
            best_idx = idx;
            best_value = value;
        } else if((op == 1) && (value > best_value)) {
            best_obj = item;
            best_idx = idx;
            best_value = value;
        }
    }
    if((optype == NUMERICAL_ARGMIN) || (optype == NUMERICAL_ARGMAX)) {
        return MP_OBJ_NEW_SMALL_INT(best_idx);
    } else {
        return best_obj;
    }
}

static mp_obj_t numerical_argmin_argmax_ndarray(ndarray_obj_t *ndarray, mp_obj_t axis, uint8_t optype) {
    // TODO: treat the flattened array
    if(ndarray->len == 0) {
        mp_raise_ValueError(translate("attempt to get (arg)min/(arg)max of empty sequence"));
    }

    uint8_t *array = (uint8_t *)ndarray->array;
    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    memset(shape, 0, sizeof(size_t)*ULAB_MAX_DIMS);
    int32_t *strides = m_new(int32_t, ULAB_MAX_DIMS);
    memset(strides, 0, sizeof(uint32_t)*ULAB_MAX_DIMS);

    if(axis == mp_const_none) {
        // pass for now
    } else {
        int8_t ax = mp_obj_get_int(axis);
        if(ax < 0) ax += ndarray->ndim;
        if((ax < 0) || (ax > ndarray->ndim - 1)) {
            mp_raise_ValueError(translate("index out of range"));
        }
        numerical_reduce_axes(ndarray, ax, shape, strides);
        uint8_t index = ULAB_MAX_DIMS - ndarray->ndim + ax;

        ndarray_obj_t *results = NULL;

        if((optype == NUMERICAL_ARGMIN) || (optype == NUMERICAL_ARGMAX)) {
            results = ndarray_new_dense_ndarray(MAX(1, ndarray->ndim-1), shape, NDARRAY_INT16);
        } else {
            results = ndarray_new_dense_ndarray(MAX(1, ndarray->ndim-1), shape, ndarray->dtype);
        }

        uint8_t *rarray = (uint8_t *)results->array;

        if(ndarray->dtype == NDARRAY_UINT8) {
            RUN_ARGMIN(ndarray, uint8_t, array, results, rarray, shape, strides, index, optype);
        } else if(ndarray->dtype == NDARRAY_INT8) {
            RUN_ARGMIN(ndarray, int8_t, array, results, rarray, shape, strides, index, optype);
        } else if(ndarray->dtype == NDARRAY_UINT16) {
            RUN_ARGMIN(ndarray, uint16_t, array, results, rarray, shape, strides, index, optype);
        } else if(ndarray->dtype == NDARRAY_INT16) {
            RUN_ARGMIN(ndarray, int16_t, array, results, rarray, shape, strides, index, optype);
        } else {
            RUN_ARGMIN(ndarray, mp_float_t, array, results, rarray, shape, strides, index, optype);
        }
        if(results->len == 1) {
            return mp_binary_get_val_array(results->dtype, results->array, 0);
        }
        return MP_OBJ_FROM_PTR(results);
    }
    return mp_const_none;
}

STATIC mp_obj_t numerical_function(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args, uint8_t optype) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} } ,
        { MP_QSTR_axis, MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t oin = args[0].u_obj;
    mp_obj_t axis = args[1].u_obj;
    if((axis != mp_const_none) && (!MP_OBJ_IS_INT(axis))) {
        mp_raise_TypeError(translate("axis must be None, or an integer"));
    }

    if(MP_OBJ_IS_TYPE(oin, &mp_type_tuple) || MP_OBJ_IS_TYPE(oin, &mp_type_list) ||
        MP_OBJ_IS_TYPE(oin, &mp_type_range)) {
        switch(optype) {
            case NUMERICAL_MIN:
            case NUMERICAL_ARGMIN:
            case NUMERICAL_MAX:
            case NUMERICAL_ARGMAX:
                return numerical_argmin_argmax_iterable(oin, optype);
            case NUMERICAL_SUM:
            case NUMERICAL_MEAN:
                return numerical_sum_mean_std_iterable(oin, optype, 0);
            default: // we should never reach this point, but whatever
                return mp_const_none;
        }
    } else if(MP_OBJ_IS_TYPE(oin, &ulab_ndarray_type)) {
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(oin);
        switch(optype) {
            case NUMERICAL_MIN:
            case NUMERICAL_MAX:
            case NUMERICAL_ARGMIN:
            case NUMERICAL_ARGMAX:
                return numerical_argmin_argmax_ndarray(ndarray, axis, optype);
            case NUMERICAL_SUM:
            case NUMERICAL_MEAN:
                return numerical_sum_mean_std_ndarray(ndarray, axis, optype, 0);
            default:
                mp_raise_NotImplementedError(translate("operation is not implemented on ndarrays"));
        }
    } else {
        mp_raise_TypeError(translate("input must be tuple, list, range, or ndarray"));
    }
    return mp_const_none;
}
/*
static mp_obj_t numerical_sort_helper(mp_obj_t oin, mp_obj_t axis, uint8_t inplace) {
    if(!MP_OBJ_IS_TYPE(oin, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("sort argument must be an ndarray"));
    }

    ndarray_obj_t *ndarray;
    if(inplace == 1) {
        ndarray = MP_OBJ_TO_PTR(oin);
    } else {
        mp_obj_t out = ndarray_copy(oin);
        ndarray = MP_OBJ_TO_PTR(out);
    }
    size_t increment, start_inc, end, N;
    if(axis == mp_const_none) { // flatten the array
        ndarray->m = 1;
        ndarray->n = ndarray->array->len;
        increment = 1;
        start_inc = ndarray->n;
        end = ndarray->n;
        N = ndarray->n;
    } else if((mp_obj_get_int(axis) == -1) ||
              (mp_obj_get_int(axis) == 1)) { // sort along the horizontal axis
        increment = 1;
        start_inc = ndarray->n;
        end = ndarray->array->len;
        N = ndarray->n;
    } else if(mp_obj_get_int(axis) == 0) { // sort along vertical axis
        increment = ndarray->n;
        start_inc = 1;
        end = ndarray->n;
        N = ndarray->m;
    } else {
        mp_raise_ValueError(translate("axis must be -1, 0, None, or 1"));
    }

    size_t q, k, p, c;

    for(size_t start=0; start < end; start+=start_inc) {
        q = N;
        k = (q >> 1);
        if((ndarray->array->typecode == NDARRAY_UINT8) || (ndarray->array->typecode == NDARRAY_INT8)) {
            HEAPSORT(uint8_t, ndarray);
        } else if((ndarray->array->typecode == NDARRAY_INT16) || (ndarray->array->typecode == NDARRAY_INT16)) {
            HEAPSORT(uint16_t, ndarray);
        } else {
            HEAPSORT(mp_float_t, ndarray);
        }
    }
    if(inplace == 1) {
        return mp_const_none;
    } else {
        return MP_OBJ_FROM_PTR(ndarray);
    }
}
*/
//| def argmax(array: _ArrayLike, *, axis: Optional[int] = None) -> int:
//|     """Return the index of the maximum element of the 1D array"""
//|     ...
//|

static mp_obj_t numerical_argmax(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_ARGMAX);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_argmax_obj, 1, numerical_argmax);

//| def argmin(array: _ArrayLike, *, axis: Optional[int] = None) -> int:
//|     """Return the index of the minimum element of the 1D array"""
//|     ...
//|

static mp_obj_t numerical_argmin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_ARGMIN);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_argmin_obj, 1, numerical_argmin);

/*
//| def argsort(array: ulab.array, *, axis: Optional[int] = None) -> ulab.array:
//|     """Returns an array which gives indices into the input array from least to greatest."""
//|     ...
//|

static mp_obj_t numerical_argsort(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_int = -1 } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    if(!MP_OBJ_IS_TYPE(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("argsort argument must be an ndarray"));
    }

    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0].u_obj);
    size_t increment, start_inc, end, N, m, n;
    if(args[1].u_obj == mp_const_none) { // flatten the array
        m = 1;
        n = ndarray->array->len;
        increment = 1;
        start_inc = ndarray->n;
        end = ndarray->n;
        N = n;
    } else if((mp_obj_get_int(args[1].u_obj) == -1) ||
              (mp_obj_get_int(args[1].u_obj) == 1)) { // sort along the horizontal axis
        m = ndarray->m;
        n = ndarray->n;
        increment = 1;
        start_inc = n;
        end = ndarray->array->len;
        N = n;
    } else if(mp_obj_get_int(args[1].u_obj) == 0) { // sort along vertical axis
        m = ndarray->m;
        n = ndarray->n;
        increment = n;
        start_inc = 1;
        end = n;
        N = m;
    } else {
        mp_raise_ValueError(translate("axis must be -1, 0, None, or 1"));
    }

    if((m > 65535) || (n > 65535)) {
        mp_raise_ValueError(translate("sorted axis can't be longer than 65535"));
    }
    // at the expense of flash, we could save RAM by creating
    // an NDARRAY_UINT16 ndarray only, if needed, otherwise, NDARRAY_UINT8
    ndarray_obj_t *indices = create_new_ndarray(m, n, NDARRAY_UINT16);
    uint16_t *index_array = (uint16_t *)indices->array->items;
    // initialise the index array
    // if array is flat: 0 to indices->n
    // if sorting vertically, identical indices are arranged row-wise
    // if sorting horizontally, identical indices are arranged colunn-wise
    for(uint16_t start=0; start < end; start+=start_inc) {
        for(uint16_t s=0; s < N; s++) {
            index_array[start+s*increment] = s;
        }
    }

    size_t q, k, p, c;
    for(size_t start=0; start < end; start+=start_inc) {
        q = N;
        k = (q >> 1);
        if((ndarray->array->typecode == NDARRAY_UINT8) || (ndarray->array->typecode == NDARRAY_INT8)) {
            HEAP_ARGSORT(uint8_t, ndarray, index_array);
        } else if((ndarray->array->typecode == NDARRAY_INT16) || (ndarray->array->typecode == NDARRAY_INT16)) {
            HEAP_ARGSORT(uint16_t, ndarray, index_array);
        } else {
            HEAP_ARGSORT(mp_float_t, ndarray, index_array);
        }
    }
    return MP_OBJ_FROM_PTR(indices);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_argsort_obj, 1, numerical_argsort);

//| def diff(array: ulab.array, *, axis: int = 1) -> ulab.array:
//|     """Return the numerical derivative of successive elements of the array, as
//|        an array.  axis=None is not supported."""
//|     ...
//|

static mp_obj_t numerical_diff(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_n, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1 } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!MP_OBJ_IS_TYPE(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("diff argument must be an ndarray"));
    }

    ndarray_obj_t *in = MP_OBJ_TO_PTR(args[0].u_obj);
    size_t increment, N, M;
    if((args[2].u_int == -1) || (args[2].u_int == 1)) { // differentiate along the horizontal axis
        increment = 1;
    } else if(args[2].u_int == 0) { // differtiate along vertical axis
        increment = in->n;
    } else {
        mp_raise_ValueError(translate("axis must be -1, 0, or 1"));
    }
    if((args[1].u_int < 0) || (args[1].u_int > 9)) {
        mp_raise_ValueError(translate("n must be between 0, and 9"));
    }
    uint8_t n = args[1].u_int;
    int8_t *stencil = m_new(int8_t, n+1);
    stencil[0] = 1;
    for(uint8_t i=1; i < n+1; i++) {
        stencil[i] = -stencil[i-1]*(n-i+1)/i;
    }

    ndarray_obj_t *out;

    if(increment == 1) { // differentiate along the horizontal axis
        if(n >= in->n) {
            out = create_new_ndarray(in->m, 0, in->array->typecode);
            m_del(uint8_t, stencil, n);
            return MP_OBJ_FROM_PTR(out);
        }
        N = in->n - n;
        M = in->m;
    } else { // differentiate along vertical axis
        if(n >= in->m) {
            out = create_new_ndarray(0, in->n, in->array->typecode);
            m_del(uint8_t, stencil, n);
            return MP_OBJ_FROM_PTR(out);
        }
        M = in->m - n;
        N = in->n;
    }
    out = create_new_ndarray(M, N, in->array->typecode);
    if(in->array->typecode == NDARRAY_UINT8) {
        CALCULATE_DIFF(in, out, uint8_t, M, N, in->n, increment);
    } else if(in->array->typecode == NDARRAY_INT8) {
        CALCULATE_DIFF(in, out, int8_t, M, N, in->n, increment);
    }  else if(in->array->typecode == NDARRAY_UINT16) {
        CALCULATE_DIFF(in, out, uint16_t, M, N, in->n, increment);
    } else if(in->array->typecode == NDARRAY_INT16) {
        CALCULATE_DIFF(in, out, int16_t, M, N, in->n, increment);
    } else {
        CALCULATE_DIFF(in, out, mp_float_t, M, N, in->n, increment);
    }
    m_del(int8_t, stencil, n);
    return MP_OBJ_FROM_PTR(out);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_diff_obj, 1, numerical_diff);

//| def flip(array: ulab.array, *, axis: Optional[int] = None) -> ulab.array:
//|     """Returns a new array that reverses the order of the elements along the
//|        given axis, or along all axes if axis is None."""
//|     ...
//|

static mp_obj_t numerical_flip(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!MP_OBJ_IS_TYPE(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("flip argument must be an ndarray"));
    }
    if((args[1].u_obj != mp_const_none) &&
           (mp_obj_get_int(args[1].u_obj) != 0) &&
           (mp_obj_get_int(args[1].u_obj) != 1)) {
        mp_raise_ValueError(translate("axis must be None, 0, or 1"));
    }

    ndarray_obj_t *in = MP_OBJ_TO_PTR(args[0].u_obj);
    mp_obj_t oout = ndarray_copy(args[0].u_obj);
    ndarray_obj_t *out = MP_OBJ_TO_PTR(oout);
    uint8_t _sizeof = mp_binary_get_size('@', in->array->typecode, NULL);
    uint8_t *array_in = (uint8_t *)in->array->items;
    uint8_t *array_out = (uint8_t *)out->array->items;
    size_t len;
    if((args[1].u_obj == mp_const_none) || (mp_obj_get_int(args[1].u_obj) == 1)) { // flip horizontally
        uint16_t M = in->m;
        len = in->n;
        if(args[1].u_obj == mp_const_none) { // flip flattened array
            len = in->array->len;
            M = 1;
        }
        for(size_t m=0; m < M; m++) {
            for(size_t n=0; n < len; n++) {
                memcpy(array_out+_sizeof*(m*len+n), array_in+_sizeof*((m+1)*len-n-1), _sizeof);
            }
        }
    } else { // flip vertically
        for(size_t m=0; m < in->m; m++) {
            for(size_t n=0; n < in->n; n++) {
                memcpy(array_out+_sizeof*(m*in->n+n), array_in+_sizeof*((in->m-m-1)*in->n+n), _sizeof);
            }
        }
    }
    return out;
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_flip_obj, 1, numerical_flip);
*/
//| def max(array: _ArrayLike, *, axis: Optional[int] = None) -> float:
//|     """Return the maximum element of the 1D array"""
//|     ...
//|

static mp_obj_t numerical_max(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_MAX);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_max_obj, 1, numerical_max);

//| def mean(array: _ArrayLike, *, axis: Optional[int] = None) -> float:
//|     """Return the mean element of the 1D array, as a number if axis is None, otherwise as an array."""
//|     ...
//|

static mp_obj_t numerical_mean(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_MEAN);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_mean_obj, 1, numerical_mean);

//| def min(array: _ArrayLike, *, axis: Optional[int] = None) -> float:
//|     """Return the minimum element of the 1D array"""
//|     ...
//|

static mp_obj_t numerical_min(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_MIN);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_min_obj, 1, numerical_min);
/*
//| def roll(array: ulab.array, distance: int, *, axis: Optional[int] = None) -> None:
//|     """Shift the content of a vector by the positions given as the second
//|        argument. If the ``axis`` keyword is supplied, the shift is applied to
//|        the given axis.  The array is modified in place."""
//|     ...
//|

static mp_obj_t numerical_roll(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none  } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t oin = args[0].u_obj;
    int16_t shift = mp_obj_get_int(args[1].u_obj);
    if((args[2].u_obj != mp_const_none) &&
           (mp_obj_get_int(args[2].u_obj) != 0) &&
           (mp_obj_get_int(args[2].u_obj) != 1)) {
        mp_raise_ValueError(translate("axis must be None, 0, or 1"));
    }

    ndarray_obj_t *in = MP_OBJ_TO_PTR(oin);
    uint8_t _sizeof = mp_binary_get_size('@', in->array->typecode, NULL);
    size_t len;
    int16_t _shift;
    uint8_t *array = (uint8_t *)in->array->items;
    // TODO: transpose the matrix, if axis == 0. Though, that is hard on the RAM...
    if(shift < 0) {
        _shift = -shift;
    } else {
        _shift = shift;
    }
    if((args[2].u_obj == mp_const_none) || (mp_obj_get_int(args[2].u_obj) == 1)) { // shift horizontally
        uint16_t M;
        if(args[2].u_obj == mp_const_none) {
            len = in->array->len;
            M = 1;
        } else {
            len = in->n;
            M = in->m;
        }
        _shift = _shift % len;
        if(shift < 0) _shift = len - _shift;
        // TODO: if(shift > len/2), we should move in the opposite direction. That would save RAM
        _shift *= _sizeof;
        uint8_t *tmp = m_new(uint8_t, _shift);
        for(size_t m=0; m < M; m++) {
            memmove(tmp, &array[m*len*_sizeof], _shift);
            memmove(&array[m*len*_sizeof], &array[m*len*_sizeof+_shift], len*_sizeof-_shift);
            memmove(&array[(m+1)*len*_sizeof-_shift], tmp, _shift);
        }
        m_del(uint8_t, tmp, _shift);
        return mp_const_none;
    } else {
        len = in->m;
        // temporary buffer
        uint8_t *_data = m_new(uint8_t, _sizeof*len);

        _shift = _shift % len;
        if(shift < 0) _shift = len - _shift;
        _shift *= _sizeof;
        uint8_t *tmp = m_new(uint8_t, _shift);

        for(size_t n=0; n < in->n; n++) {
            for(size_t m=0; m < len; m++) {
                // this loop should fill up the temporary buffer
                memmove(&_data[m*_sizeof], &array[(m*in->n+n)*_sizeof], _sizeof);
            }
            // now, the actual shift
            memmove(tmp, _data, _shift);
            memmove(_data, &_data[_shift], len*_sizeof-_shift);
            memmove(&_data[len*_sizeof-_shift], tmp, _shift);
            for(size_t m=0; m < len; m++) {
                // this loop should dump the content of the temporary buffer into data
                memmove(&array[(m*in->n+n)*_sizeof], &_data[m*_sizeof], _sizeof);
            }
        }
        m_del(uint8_t, tmp, _shift);
        m_del(uint8_t, _data, _sizeof*len);
        return mp_const_none;
    }
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_roll_obj, 2, numerical_roll);

//| def sort(array: ulab.array, *, axis: Optional[int] = 0) -> ulab.array:
//|     """Sort the array along the given axis, or along all axes if axis is None.
//|        The array is modified in place."""
//|     ...
//|

static mp_obj_t numerical_sort(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    return numerical_sort_helper(args[0].u_obj, args[1].u_obj, 0);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_sort_obj, 1, numerical_sort);

// method of an ndarray
static mp_obj_t numerical_sort_inplace(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    return numerical_sort_helper(args[0].u_obj, args[1].u_obj, 1);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_sort_inplace_obj, 1, numerical_sort_inplace);
*/
//| def std(array: _ArrayLike, *, axis: Optional[int] = None) -> float:
//|     """Return the standard deviation of the array, as a number if axis is None, otherwise as an array."""
//|     ...
//|

static mp_obj_t numerical_std(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } } ,
        { MP_QSTR_axis, MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_ddof, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t oin = args[0].u_obj;
    mp_obj_t axis = args[1].u_obj;
    size_t ddof = args[2].u_int;
    if((axis != mp_const_none) && (mp_obj_get_int(axis) != 0) && (mp_obj_get_int(axis) != 1)) {
        // this seems to pass with False, and True...
        mp_raise_ValueError(translate("axis must be None, or an integer"));
    }
    if(MP_OBJ_IS_TYPE(oin, &mp_type_tuple) || MP_OBJ_IS_TYPE(oin, &mp_type_list) || MP_OBJ_IS_TYPE(oin, &mp_type_range)) {
        return numerical_sum_mean_std_iterable(oin, NUMERICAL_STD, ddof);
    } else if(MP_OBJ_IS_TYPE(oin, &ulab_ndarray_type)) {
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(oin);
        return numerical_sum_mean_std_ndarray(ndarray, axis, NUMERICAL_STD, ddof);
    } else {
        mp_raise_TypeError(translate("input must be tuple, list, range, or ndarray"));
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_std_obj, 1, numerical_std);

//| def sum(array: _ArrayLike, *, axis: Optional[int] = None) -> Union[float, int, ulab.array]:
//|     """Return the sum of the array, as a number if axis is None, otherwise as an array."""
//|     ...
//|

static mp_obj_t numerical_sum(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_SUM);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_sum_obj, 1, numerical_sum);

STATIC const mp_rom_map_elem_t ulab_numerical_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_numerical) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_argmax), (mp_obj_t)&numerical_argmax_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_argmin), (mp_obj_t)&numerical_argmin_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_argsort), (mp_obj_t)&numerical_argsort_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_diff), (mp_obj_t)&numerical_diff_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_flip), (mp_obj_t)&numerical_flip_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_max), (mp_obj_t)&numerical_max_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mean), (mp_obj_t)&numerical_mean_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_min), (mp_obj_t)&numerical_min_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_roll), (mp_obj_t)&numerical_roll_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_sort), (mp_obj_t)&numerical_sort_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_std), (mp_obj_t)&numerical_std_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sum), (mp_obj_t)&numerical_sum_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_numerical_globals, ulab_numerical_globals_table);

mp_obj_module_t ulab_numerical_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_numerical_globals,
};

#endif

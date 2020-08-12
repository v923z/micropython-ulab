/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 * 				 2019-2020 Zoltán Vörös
*/

#include <math.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "ulab_create.h"

mp_obj_t create_zeros_ones(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args, uint8_t kind) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} } ,
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = NDARRAY_FLOAT} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    uint8_t dtype = args[1].u_int;
    if(!MP_OBJ_IS_INT(args[0].u_obj) && !MP_OBJ_IS_TYPE(args[0].u_obj, &mp_type_tuple)) {
        mp_raise_TypeError(translate("input argument must be an integer or a 2-tuple"));
    }
    ndarray_obj_t *ndarray = NULL;
    if(MP_OBJ_IS_INT(args[0].u_obj)) {
        size_t n = mp_obj_get_int(args[0].u_obj);
        ndarray = ndarray_new_linear_array(n, dtype);
    } else if(MP_OBJ_IS_TYPE(args[0].u_obj, &mp_type_tuple)) {
        mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(args[0].u_obj);
        if(tuple->len != 2) {
            mp_raise_TypeError(translate("input argument must be an integer or a 2-tuple"));
        }
        ndarray = ndarray_new_ndarray_from_tuple(tuple, dtype);
    }
    if(kind == 1) {
        mp_obj_t one = mp_obj_new_int(1);
        for(size_t i=0; i < ndarray->len; i++) {
            mp_binary_set_val_array(dtype, ndarray->array, i, one);
        }
    }
    return MP_OBJ_FROM_PTR(ndarray);
}
STATIC ndarray_obj_t *create_linspace_arange(mp_float_t start, mp_float_t step, size_t len, uint8_t dtype) {
    mp_float_t value = start;
    
    ndarray_obj_t *ndarray = ndarray_new_linear_array(len, dtype);
    if(dtype == NDARRAY_UINT8) {
        uint8_t *array = (uint8_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value += step) array[i] = (uint8_t)value;
    } else if(dtype == NDARRAY_INT8) {
        int8_t *array = (int8_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value += step) array[i] = (int8_t)value;
    } else if(dtype == NDARRAY_UINT16) {
        uint16_t *array = (uint16_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value += step) array[i] = (uint16_t)value;
    } else if(dtype == NDARRAY_INT16) {
        int16_t *array = (int16_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value += step) array[i] = (int16_t)value;
    } else {
        mp_float_t *array = (mp_float_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value += step) array[i] = value;
    }
    return ndarray;
}

//| @overload
//| def arange(stop: _float, step: _float = 1, dtype: _DType = float) -> array: ...
//| @overload
//| def arange(start: _float, stop: _float, step: _float = 1, dtype: _DType = float) -> array:
//|     """
//|     .. param: start
//|       First value in the array, optional, defaults to 0
//|     .. param: stop
//|       Final value in the array
//|     .. param: step
//|       Difference between consecutive elements, optional, defaults to 1.0
//|     .. param: dtype
//|       Type of values in the array
//|
//|     Return a new 1-D array with elements ranging from ``start`` to ``stop``, with step size ``step``."""
//|     ...
//|

mp_obj_t create_arange(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED| MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    uint8_t dtype = NDARRAY_FLOAT;
    mp_float_t start, stop, step;
    if(n_args == 1) {
        start = 0.0;
        stop = mp_obj_get_float(args[0].u_obj);
        step = 1.0;
        if(mp_obj_is_int(args[0].u_obj)) dtype = NDARRAY_INT16;
    } else if(n_args == 2) {
        start = mp_obj_get_float(args[0].u_obj);
        stop = mp_obj_get_float(args[1].u_obj);
        step = 1.0;
        if(mp_obj_is_int(args[0].u_obj) && mp_obj_is_int(args[1].u_obj)) dtype = NDARRAY_INT16;
    } else if(n_args == 3) {
        start = mp_obj_get_float(args[0].u_obj);
        stop = mp_obj_get_float(args[1].u_obj);
        step = mp_obj_get_float(args[2].u_obj);
        if(mp_obj_is_int(args[0].u_obj) && mp_obj_is_int(args[1].u_obj) && mp_obj_is_int(args[2].u_obj)) dtype = NDARRAY_INT16;
    } else {
        mp_raise_TypeError(translate("wrong number of arguments"));
    }
    if((MICROPY_FLOAT_C_FUN(fabs)(stop) > 32768) || (MICROPY_FLOAT_C_FUN(fabs)(start) > 32768) || (MICROPY_FLOAT_C_FUN(fabs)(step) > 32768)) {
        dtype = NDARRAY_FLOAT;
    }
    if(args[3].u_obj != mp_const_none) {
        dtype = (uint8_t)mp_obj_get_int(args[3].u_obj);
    }
    size_t len;
    if((stop - start)/step < 0) {
        len = 0;
    } else {
        len = (size_t)(MICROPY_FLOAT_C_FUN(ceil)((stop - start)/step));
    }
    return MP_OBJ_FROM_PTR(ndarray_new_linear_array(len, dtype));
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_arange_obj, 1, create_arange);

#if ULAB_MAX_DIMS > 1
//| def eye(size: int, *, dtype: _DType = float) -> array:
//|     """Return a new square array of size, with the diagonal elements set to 1
//|        and the other elements set to 0."""
//|     ...
//|

mp_obj_t create_eye(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_M, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_k, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },        
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = NDARRAY_FLOAT} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t n = args[0].u_int, m;
    size_t k = args[2].u_int;
    uint8_t dtype = args[3].u_int;
    if(args[1].u_rom_obj == mp_const_none) {
        m = n;
    } else {
        m = mp_obj_get_int(args[1].u_rom_obj);
    }
    
    size_t shape[] = {0, 0, m, n};
    ndarray_obj_t *ndarray = ndarray_new_dense_ndarray(2, shape, dtype);
    mp_obj_t one = mp_obj_new_int(1);
    size_t i = 0;
    if((k >= 0) && (k < n)) {
        while(k < n) {
            mp_binary_set_val_array(dtype, ndarray->array, i*n+k, one);
            k++;
            i++;
        }
    } else if((k < 0) && (-k < m)) {
        k = -k;
        i = 0;
        while(k < m) {
            mp_binary_set_val_array(dtype, ndarray->array, k*n+i, one);
            k++;
            i++;
        }
    }
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_eye_obj, 0, create_eye);

#endif

//| def linspace(
//|     start: _float,
//|     stop: _float,
//|     *,
//|     dtype: _DType = float,
//|     num: int = 50,
//|     endpoint: bool = True
//| ) -> array:
//|     """
//|     .. param: start
//|       First value in the array
//|     .. param: stop
//|       Final value in the array
//|     .. param int: num
//|       Count of values in the array
//|     .. param: dtype
//|       Type of values in the array
//|     .. param bool: endpoint
//|       Whether the ``stop`` value is included.  Note that even when
//|       endpoint=True, the exact ``stop`` value may not be included due to the
//|       inaccuracy of floating point arithmetic.
//|
//|     Return a new 1-D array with ``num`` elements ranging from ``start`` to ``stop`` linearly."""
//|     ...
//|

mp_obj_t create_linspace(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_num, MP_ARG_INT, {.u_int = 50} },
        { MP_QSTR_endpoint, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_true} },
        { MP_QSTR_retstep, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_false} },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = NDARRAY_FLOAT} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(args[2].u_int < 2) {
        mp_raise_ValueError(translate("number of points must be at least 2"));
    }
    size_t len = (size_t)args[2].u_int;
    mp_float_t start, step;
    start = mp_obj_get_float(args[0].u_obj);
    uint8_t typecode = args[5].u_int;
    if(args[3].u_obj == mp_const_true) step = (mp_obj_get_float(args[1].u_obj)-start)/(len-1);
    else step = (mp_obj_get_float(args[1].u_obj)-start)/len;
    ndarray_obj_t *ndarray = create_linspace_arange(start, step, len, typecode);
    if(args[4].u_obj == mp_const_false) {
        return MP_OBJ_FROM_PTR(ndarray);
    } else {
        mp_obj_t tuple[2];
        tuple[0] = ndarray;
        tuple[1] = mp_obj_new_float(step);
        return mp_obj_new_tuple(2, tuple);
    }
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_linspace_obj, 2, create_linspace);

//| def ones(shape: Union[int, Tuple[int, int]], *, dtype: _DType = float) -> array:
//|    """
//|    .. param: shape
//|       Shape of the array, either an integer (for a 1-D array) or a tuple of 2 integers (for a 2-D array)
//|    .. param: dtype
//|       Type of values in the array
//|
//|    Return a new array of the given shape with all elements set to 1."""
//|    ...
//|    

mp_obj_t create_ones(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return create_zeros_ones(n_args, pos_args, kw_args, 1);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_ones_obj, 0, create_ones);

//| def zeros(shape: Union[int, Tuple[int, int]], *, dtype: _DType = float) -> array:
//|    """
//|    .. param: shape
//|       Shape of the array, either an integer (for a 1-D array) or a tuple of 2 integers (for a 2-D array)
//|    .. param: dtype
//|       Type of values in the array
//|
//|    Return a new array of the given shape with all elements set to 0."""
//|    ...
//|

mp_obj_t create_zeros(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return create_zeros_ones(n_args, pos_args, kw_args, 0);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_zeros_obj, 0, create_zeros);

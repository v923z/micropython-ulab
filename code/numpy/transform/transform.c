/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
 *
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"

#include "../../ulab.h"
#include "../../ulab_tools.h"
#include "../carray/carray_tools.h"

#include "transform.h"


#if ULAB_NUMPY_HAS_DOT
//| def dot(m1: ulab.array, m2: ulab.array) -> Union[ulab.array, float]:
//|    """
//|    :param ~ulab.array m1: a matrix, or a vector
//|    :param ~ulab.array m2: a matrix, or a vector
//|
//|    Computes the product of two matrices, or two vectors. In the letter case, the inner product is returned."""
//|    ...
//|

mp_obj_t transform_dot(mp_obj_t _m1, mp_obj_t _m2) {
    // TODO: should the results be upcast?
    // This implements 2D operations only!
    if(!MP_OBJ_IS_TYPE(_m1, &ulab_ndarray_type) || !MP_OBJ_IS_TYPE(_m2, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("arguments must be ndarrays"));
    }
    ndarray_obj_t *m1 = MP_OBJ_TO_PTR(_m1);
    ndarray_obj_t *m2 = MP_OBJ_TO_PTR(_m2);
    COMPLEX_DTYPE_NOT_IMPLEMENTED(m1->dtype)
    COMPLEX_DTYPE_NOT_IMPLEMENTED(m2->dtype)

    uint8_t *array1 = (uint8_t *)m1->array;
    uint8_t *array2 = (uint8_t *)m2->array;

    mp_float_t (*func1)(void *) = ndarray_get_float_function(m1->dtype);
    mp_float_t (*func2)(void *) = ndarray_get_float_function(m2->dtype);

    if(m1->shape[ULAB_MAX_DIMS - 1] != m2->shape[ULAB_MAX_DIMS - m2->ndim]) {
        mp_raise_ValueError(translate("dimensions do not match"));
    }
    uint8_t ndim = MIN(m1->ndim, m2->ndim);
    size_t shape1 = m1->ndim == 2 ? m1->shape[ULAB_MAX_DIMS - m1->ndim] : 1;
    size_t shape2 = m2->ndim == 2 ? m2->shape[ULAB_MAX_DIMS - 1] : 1;

    size_t *shape = NULL;
    if(ndim == 2) { // matrix times matrix -> matrix
        shape = ndarray_shape_vector(0, 0, shape1, shape2);
    } else { // matrix times vector -> vector, vector times vector -> vector (size 1)
        shape = ndarray_shape_vector(0, 0, 0, shape1);
    }
    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
    mp_float_t *rarray = (mp_float_t *)results->array;

    for(size_t i=0; i < shape1; i++) { // rows of m1
        for(size_t j=0; j < shape2; j++) { // columns of m2
            mp_float_t dot = 0.0;
            for(size_t k=0; k < m1->shape[ULAB_MAX_DIMS - 1]; k++) {
                // (i, k) * (k, j)
                dot += func1(array1) * func2(array2);
                array1 += m1->strides[ULAB_MAX_DIMS - 1];
                array2 += m2->strides[ULAB_MAX_DIMS - m2->ndim];
            }
            *rarray++ = dot;
            array1 -= m1->strides[ULAB_MAX_DIMS - 1] * m1->shape[ULAB_MAX_DIMS - 1];
            array2 -= m2->strides[ULAB_MAX_DIMS - m2->ndim] * m2->shape[ULAB_MAX_DIMS - m2->ndim];
            array2 += m2->strides[ULAB_MAX_DIMS - 1];
        }
        array1 += m1->strides[ULAB_MAX_DIMS - m1->ndim];
        array2 = m2->array;
    }
    if((m1->ndim * m2->ndim) == 1) { // return a scalar, if product of two vectors
        return mp_obj_new_float(*(--rarray));
    } else {
        return MP_OBJ_FROM_PTR(results);
    }
}

MP_DEFINE_CONST_FUN_OBJ_2(transform_dot_obj, transform_dot);
#endif

#if ULAB_NUMPY_HAS_FLIP
//| def flip(array: ulab.array, *, axis: Optional[int] = None) -> ulab.array:
//|     """Returns a new array that reverses the order of the elements along the
//|        given axis, or along all axes if axis is None."""
//|     ...
//|

mp_obj_t transform_flip(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!MP_OBJ_IS_TYPE(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("flip argument must be an ndarray"));
    }

    ndarray_obj_t *results = NULL;
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0].u_obj);
    if(args[1].u_obj == mp_const_none) { // flip the flattened array
        results = ndarray_new_linear_array(ndarray->len, ndarray->dtype);
        ndarray_copy_array(ndarray, results, 0);
        uint8_t *rarray = (uint8_t *)results->array;
        rarray += (results->len - 1) * results->itemsize;
        results->array = rarray;
        results->strides[ULAB_MAX_DIMS - 1] = -results->strides[ULAB_MAX_DIMS - 1];
    } else if(MP_OBJ_IS_INT(args[1].u_obj)){
        int8_t ax = mp_obj_get_int(args[1].u_obj);
        if(ax < 0) ax += ndarray->ndim;
        if((ax < 0) || (ax > ndarray->ndim - 1)) {
            mp_raise_ValueError(translate("index out of range"));
        }
        ax = ULAB_MAX_DIMS - ndarray->ndim + ax;
        int32_t offset = (ndarray->shape[ax] - 1) * ndarray->strides[ax];
        results = ndarray_new_view(ndarray, ndarray->ndim, ndarray->shape, ndarray->strides, offset);
        results->strides[ax] = -results->strides[ax];
    } else {
        mp_raise_TypeError(translate("wrong axis index"));
    }
    return results;
}

MP_DEFINE_CONST_FUN_OBJ_KW(transform_flip_obj, 1, transform_flip);
#endif

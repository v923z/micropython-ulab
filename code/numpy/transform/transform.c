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

    #if ULAB_MAX_DIMS > 1
    if ((m1->ndim == 1) && (m2->ndim == 1)) {
    #endif
        // 2 vectors
        if (m1->len != m2->len) {
            mp_raise_ValueError(translate("vectors must have same lengths"));
        }
        mp_float_t dot = 0.0;
        uint8_t *array1 = (uint8_t *)m1->array;
        uint8_t *array2 = (uint8_t *)m2->array;
        for (size_t i=0; i < m1->len; i++) {
            dot += ndarray_get_float_value(array1, m1->dtype)*ndarray_get_float_value(array2, m2->dtype);
            array1 += m1->strides[ULAB_MAX_DIMS - 1];
            array2 += m2->strides[ULAB_MAX_DIMS - 1];
        }
        return mp_obj_new_float(dot);
    #if ULAB_MAX_DIMS > 1
    } else {
        // 2 matrices
        if(m1->shape[ULAB_MAX_DIMS - 1] != m2->shape[ULAB_MAX_DIMS - 2]) {
            mp_raise_ValueError(translate("matrix dimensions do not match"));
        }
        size_t *shape = ndarray_shape_vector(0, 0, m1->shape[ULAB_MAX_DIMS - 2], m2->shape[ULAB_MAX_DIMS - 1]);
        ndarray_obj_t *out = ndarray_new_dense_ndarray(2, shape, NDARRAY_FLOAT);
        mp_float_t *outdata = (mp_float_t *)out->array;
        for(size_t i=0; i < m1->shape[ULAB_MAX_DIMS - 2]; i++) { // rows of m1
            for(size_t j=0; j < m2->shape[ULAB_MAX_DIMS - 1]; j++) { // columns of m2
                mp_float_t sum = 0.0, v1, v2;
                for(size_t k=0; k < m2->shape[ULAB_MAX_DIMS - 2]; k++) {
                    // (i, k) * (k, j)
                    size_t pos1 = i*m1->shape[ULAB_MAX_DIMS - 1]+k;
                    size_t pos2 = k*m2->shape[ULAB_MAX_DIMS - 1]+j;
                    v1 = ndarray_get_float_index(m1->array, m1->dtype, pos1);
                    v2 = ndarray_get_float_index(m2->array, m2->dtype, pos2);
                    sum += v1 * v2;
                }
                *outdata++ = sum;
            }
        }
        return MP_OBJ_FROM_PTR(out);
    }
    #endif
}

MP_DEFINE_CONST_FUN_OBJ_2(transform_dot_obj, transform_dot);
#endif


/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Zoltán Vörös
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/objint.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/misc.h"

#include "../../ulab.h"
#include "../../ndarray.h"
#include "carray.h"

#if ULAB_SUPPORTS_COMPLEX

STATIC mp_obj_t carray_real(mp_obj_t _source) {
    if(mp_obj_is_type(_source, &ulab_ndarray_type)) {
        ndarray_obj_t *source = MP_OBJ_TO_PTR(_source);
        if(source->dtype != NDARRAY_COMPLEX) {
            ndarray_obj_t *target = ndarray_new_dense_ndarray(source->ndim, source->shape, source->dtype);
            ndarray_copy_array(source, target, 0);
            return MP_OBJ_FROM_PTR(target);
        } else { // the input is most definitely a complex array
            ndarray_obj_t *target = ndarray_new_dense_ndarray(source->ndim, source->shape, NDARRAY_FLOAT);
            ndarray_copy_array(source, target, 0);
            return MP_OBJ_FROM_PTR(target);
        }
    } else {
        mp_raise_NotImplementedError(translate("function is implemented for ndarrays only"));
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(carray_real_obj, carray_real);

STATIC mp_obj_t carray_imag(mp_obj_t _source) {
    if(mp_obj_is_type(_source, &ulab_ndarray_type)) {
        ndarray_obj_t *source = MP_OBJ_TO_PTR(_source);
        if(source->dtype != NDARRAY_COMPLEX) { // if not complex, then the imaginary part is zero
            ndarray_obj_t *target = ndarray_new_dense_ndarray(source->ndim, source->shape, source->dtype);
            return MP_OBJ_FROM_PTR(target);
        } else { // the input is most definitely a complex array
            ndarray_obj_t *target = ndarray_new_dense_ndarray(source->ndim, source->shape, NDARRAY_FLOAT);
            ndarray_copy_array(source, target, source->itemsize / 2);
            return MP_OBJ_FROM_PTR(target);
        }
    } else {
        mp_raise_NotImplementedError(translate("function is implemented for ndarrays only"));
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(carray_imag_obj, carray_imag);

mp_obj_t carray_abs(ndarray_obj_t *source, ndarray_obj_t *target) {
    // calculates the absolute value of a complex array and returns a dense array
    uint8_t *sarray = (uint8_t *)source->array;
    mp_float_t *tarray = (mp_float_t *)target->array;
    uint8_t itemsize = mp_binary_get_size('@', NDARRAY_FLOAT, NULL);

    #if ULAB_MAX_DIMS > 3
    size_t i = 0;
    do {
    #endif
        #if ULAB_MAX_DIMS > 2
        size_t j = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 1
            size_t k = 0;
            do {
            #endif
                size_t l = 0;
                do {
                    mp_float_t rvalue = *(mp_float_t *)sarray;
                    mp_float_t ivalue = *(mp_float_t *)(sarray + itemsize);
                    *tarray++ = MICROPY_FLOAT_C_FUN(sqrt)(rvalue * rvalue + ivalue * ivalue);
                    sarray += source->strides[ULAB_MAX_DIMS - 1];
                    l++;
                } while(l < source->shape[ULAB_MAX_DIMS - 1]);
            #if ULAB_MAX_DIMS > 1
                sarray -= source->strides[ULAB_MAX_DIMS - 1] * source->shape[ULAB_MAX_DIMS-1];
                sarray += source->strides[ULAB_MAX_DIMS - 2];
                k++;
            } while(k < source->shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 2
            sarray -= source->strides[ULAB_MAX_DIMS - 2] * source->shape[ULAB_MAX_DIMS-2];
            sarray += source->strides[ULAB_MAX_DIMS - 3];
            j++;
        } while(j < source->shape[ULAB_MAX_DIMS - 3]);
        #endif
    #if ULAB_MAX_DIMS > 3
        sarray -= source->strides[ULAB_MAX_DIMS - 3] * source->shape[ULAB_MAX_DIMS-3];
        sarray += source->strides[ULAB_MAX_DIMS - 4];
        i++;
    } while(i < source->shape[ULAB_MAX_DIMS - 4]);
    #endif
    return MP_OBJ_FROM_PTR(target);
}

void carray_binary_add(ndarray_obj_t *results, mp_float_t *resarray, uint8_t *larray, uint8_t *rarray,
                            int32_t *lstrides, int32_t *rstrides, uint8_t rdtype) {

    if(rdtype == NDARRAY_UINT8) {
        BINARY_LOOP_COMPLEX(results, resarray, uint8_t, larray, lstrides, rarray, rstrides, +);
    } else if(rdtype == NDARRAY_INT8) {
        BINARY_LOOP_COMPLEX(results, resarray, int8_t, larray, lstrides, rarray, rstrides, +);
    } else if(rdtype == NDARRAY_UINT16) {
        BINARY_LOOP_COMPLEX(results, resarray, uint16_t, larray, lstrides, rarray, rstrides, +);
    } else if(rdtype == NDARRAY_INT16) {
        BINARY_LOOP_COMPLEX(results, resarray, int16_t, larray, lstrides, rarray, rstrides, +);
    } else if(rdtype == NDARRAY_FLOAT) {
        BINARY_LOOP_COMPLEX(results, resarray, mp_float_t, larray, lstrides, rarray, rstrides, +);
    }
}

#endif

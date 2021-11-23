
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

#endif

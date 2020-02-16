
/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *               2020 Zoltán Vörös   
*/

#ifndef _NDARRAY_PROPERTIES_
#define _NDARRAY_PROPERTIES_

#include "py/runtime.h"
#include "py/binary.h"
#include "py/obj.h"
#include "py/objarray.h"

#include "ndarray.h"

typedef struct _mp_obj_property_t {
    mp_obj_base_t base;
    mp_obj_t proxy[3]; // getter, setter, deleter
} mp_obj_property_t;

/* v923z: it is not at all clear to me, why this must be declared; it should already be in obj.h */
typedef struct _mp_obj_none_t {
    mp_obj_base_t base;
} mp_obj_none_t;

const mp_obj_type_t mp_type_NoneType;
const mp_obj_none_t mp_const_none_obj = {{&mp_type_NoneType}};

MP_DEFINE_CONST_FUN_OBJ_1(ndarray_get_shape_obj, ndarray_shape);
MP_DEFINE_CONST_FUN_OBJ_1(ndarray_get_size_obj, ndarray_size);
MP_DEFINE_CONST_FUN_OBJ_1(ndarray_get_itemsize_obj, ndarray_itemsize);
MP_DEFINE_CONST_FUN_OBJ_KW(ndarray_flatten_obj, 1, ndarray_flatten);

STATIC const mp_obj_property_t ndarray_shape_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&ndarray_get_shape_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_obj_property_t ndarray_size_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&ndarray_get_size_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_obj_property_t ndarray_itemsize_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&ndarray_get_itemsize_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

#endif

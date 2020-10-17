
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

#include "ulab.h"
#include "ndarray.h"

#if NDARRAY_HAS_ITEMSIZE
MP_DEFINE_CONST_FUN_OBJ_1(ndarray_itemsize_obj, ndarray_itemsize);
#endif
#if NDARRAY_HAS_SHAPE
MP_DEFINE_CONST_FUN_OBJ_1(ndarray_shape_obj, ndarray_shape);
#endif
#if NDARRAY_HAS_SIZE
MP_DEFINE_CONST_FUN_OBJ_1(ndarray_size_obj, ndarray_size);
#endif
#if NDARRAY_HAS_STRIDES
MP_DEFINE_CONST_FUN_OBJ_1(ndarray_strides_obj, ndarray_strides);
#endif

#if CIRCUITPY
typedef struct _mp_obj_property_t {
    mp_obj_base_t base;
    mp_obj_t proxy[3]; // getter, setter, deleter
} mp_obj_property_t;

STATIC const mp_obj_property_t ndarray_shape_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&ndarray_get_shape_obj,
              mp_const_none,
              mp_const_none },
};

STATIC const mp_obj_property_t ndarray_strides_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&ndarray_get_strides_obj,
              mp_const_none,
              mp_const_none },
};

STATIC const mp_obj_property_t ndarray_size_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&ndarray_get_size_obj,
              mp_const_none,
              mp_const_none },
};

STATIC const mp_obj_property_t ndarray_itemsize_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&ndarray_get_itemsize_obj,
              mp_const_none,
              mp_const_none },
};
#endif /* CIRCUITPY */
#endif


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

#if CIRCUITPY
typedef struct _mp_obj_property_t {
    mp_obj_base_t base;
    mp_obj_t proxy[3]; // getter, setter, deleter
} mp_obj_property_t;

MP_DEFINE_CONST_FUN_OBJ_1(ndarray_get_shape_obj, ndarray_shape);
MP_DEFINE_CONST_FUN_OBJ_1(ndarray_get_size_obj, ndarray_size);
MP_DEFINE_CONST_FUN_OBJ_1(ndarray_get_itemsize_obj, ndarray_itemsize);

STATIC const mp_obj_property_t ndarray_shape_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&ndarray_get_shape_obj,
              MP_ROM_NONE,
              MP_ROM_NONE },
};

STATIC const mp_obj_property_t ndarray_size_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&ndarray_get_size_obj,
              MP_ROM_NONE,
              MP_ROM_NONE },
};

STATIC const mp_obj_property_t ndarray_itemsize_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&ndarray_get_itemsize_obj,
              MP_ROM_NONE,
              MP_ROM_NONE },
};
#else

MP_DEFINE_CONST_FUN_OBJ_1(ndarray_size_obj, ndarray_size);
MP_DEFINE_CONST_FUN_OBJ_1(ndarray_itemsize_obj, ndarray_itemsize);
MP_DEFINE_CONST_FUN_OBJ_1(ndarray_shape_obj, ndarray_shape);

#endif

#endif

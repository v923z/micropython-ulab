
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "py/runtime.h"
#include "py/binary.h"
#include "py/obj.h"
#include "py/objarray.h"

#include "ulab.h"
#include "ndarray.h"
#include "ndarray_properties.h"
#include "create.h"
#include "linalg.h"
#include "vectorise.h"
#include "poly.h"
#include "fft.h"
#include "filter.h"
#include "numerical.h"
#include "extras.h"

STATIC MP_DEFINE_STR_OBJ(ulab_version_obj, "0.36.0");

MP_DEFINE_CONST_FUN_OBJ_KW(ndarray_flatten_obj, 1, ndarray_flatten);

STATIC const mp_rom_map_elem_t ulab_ndarray_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reshape), MP_ROM_PTR(&ndarray_reshape_obj) },
    { MP_ROM_QSTR(MP_QSTR_transpose), MP_ROM_PTR(&ndarray_transpose_obj) },
    { MP_ROM_QSTR(MP_QSTR_flatten), MP_ROM_PTR(&ndarray_flatten_obj) },
    { MP_ROM_QSTR(MP_QSTR_shape), MP_ROM_PTR(&ndarray_shape_obj) },
    { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&ndarray_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_itemsize), MP_ROM_PTR(&ndarray_itemsize_obj) },
#if CIRCUITPY
    { MP_ROM_QSTR(MP_QSTR_sort), MP_ROM_PTR(&numerical_sort_inplace_obj) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(ulab_ndarray_locals_dict, ulab_ndarray_locals_dict_table);

const mp_obj_type_t ulab_ndarray_type = {
    { &mp_type_type },
    .name = MP_QSTR_ndarray,
    .print = ndarray_print,
    .make_new = ndarray_make_new,
    .subscr = ndarray_subscr,
    .getiter = ndarray_getiter,
    .unary_op = ndarray_unary_op,
    .binary_op = ndarray_binary_op,
    .buffer_p = { .get_buffer = ndarray_get_buffer, },
    .locals_dict = (mp_obj_dict_t*)&ulab_ndarray_locals_dict,
};

STATIC const mp_map_elem_t ulab_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_ulab) },
    { MP_ROM_QSTR(MP_QSTR___version__), MP_ROM_PTR(&ulab_version_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_array), (mp_obj_t)&ulab_ndarray_type },
    { MP_ROM_QSTR(MP_QSTR_zeros), (mp_obj_t)&create_zeros_obj },
    { MP_ROM_QSTR(MP_QSTR_ones), (mp_obj_t)&create_ones_obj },
    { MP_ROM_QSTR(MP_QSTR_eye), (mp_obj_t)&create_eye_obj },
    { MP_ROM_QSTR(MP_QSTR_linspace), (mp_obj_t)&create_linspace_obj },
    #if ULAB_LINALG_MODULE
    { MP_ROM_QSTR(MP_QSTR_linalg), MP_ROM_PTR(&ulab_linalg_module) },
    #endif
    #if ULAB_VECTORISE_MODULE
    { MP_ROM_QSTR(MP_QSTR_vector), MP_ROM_PTR(&ulab_vectorise_module) },
    #endif
    #if ULAB_NUMERICAL_MODULE
    { MP_ROM_QSTR(MP_QSTR_numerical), MP_ROM_PTR(&ulab_numerical_module) },
    #endif
    #if ULAB_POLY_MODULE
    { MP_ROM_QSTR(MP_QSTR_poly), MP_ROM_PTR(&ulab_poly_module) },
    #endif
    #if ULAB_FFT_MODULE
    { MP_ROM_QSTR(MP_QSTR_fft), MP_ROM_PTR(&ulab_fft_module) },
    #endif
    #if ULAB_FILTER_MODULE
    { MP_ROM_QSTR(MP_QSTR_filter), MP_ROM_PTR(&ulab_filter_module) },
    #endif
    #if ULAB_EXTRAS_MODULE
    { MP_ROM_QSTR(MP_QSTR_extras), MP_ROM_PTR(&ulab_extras_module) },
    #endif
    // class constants
    { MP_ROM_QSTR(MP_QSTR_uint8), MP_ROM_INT(NDARRAY_UINT8) },
    { MP_ROM_QSTR(MP_QSTR_int8), MP_ROM_INT(NDARRAY_INT8) },
    { MP_ROM_QSTR(MP_QSTR_uint16), MP_ROM_INT(NDARRAY_UINT16) },
    { MP_ROM_QSTR(MP_QSTR_int16), MP_ROM_INT(NDARRAY_INT16) },
    { MP_ROM_QSTR(MP_QSTR_float), MP_ROM_INT(NDARRAY_FLOAT) },
};

STATIC MP_DEFINE_CONST_DICT (
    mp_module_ulab_globals,
    ulab_globals_table
);

mp_obj_module_t ulab_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ulab, ulab_user_cmodule, MODULE_ULAB_ENABLED);

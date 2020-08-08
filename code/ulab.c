
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
#include "ulab_create.h"
#include "approx/approx.h"
#include "compare/compare.h"
#include "numerical/numerical.h"
#include "fft/fft.h"
#include "filter/filter.h"
#include "linalg/linalg.h"
#include "poly/poly.h"
#include "user/user.h"
#include "vector/vectorise.h"

#define ULAB_VERSION 0.99.0
#define xstr(s) str(s)
#define str(s) #s
#define ULAB_VERSION_STRING xstr(ULAB_VERSION) xstr(-) xstr(ULAB_MAX_DIMS) xstr(D)

STATIC MP_DEFINE_STR_OBJ(ulab_version_obj, ULAB_VERSION_STRING);

MP_DEFINE_CONST_FUN_OBJ_KW(ndarray_flatten_obj, 1, ndarray_flatten);

STATIC const mp_rom_map_elem_t ulab_ndarray_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reshape), MP_ROM_PTR(&ndarray_reshape_obj) },
    { MP_ROM_QSTR(MP_QSTR_transpose), MP_ROM_PTR(&ndarray_transpose_obj) },
//    { MP_ROM_QSTR(MP_QSTR_flatten), MP_ROM_PTR(&ndarray_flatten_obj) },
    { MP_ROM_QSTR(MP_QSTR_shape), MP_ROM_PTR(&ndarray_shape_obj) },
    { MP_ROM_QSTR(MP_QSTR_strides), MP_ROM_PTR(&ndarray_strides_obj) }, 
    { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&ndarray_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_itemsize), MP_ROM_PTR(&ndarray_itemsize_obj) },
#if CIRCUITPY
    { MP_ROM_QSTR(MP_QSTR_sort), MP_ROM_PTR(&numerical_sort_inplace_obj) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(ulab_ndarray_locals_dict, ulab_ndarray_locals_dict_table);

const mp_obj_type_t ulab_ndarray_type = {
    { &mp_type_type },
#if defined(MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE) && defined(MP_TYPE_FLAG_EQ_HAS_NEQ_TEST)
    .flags = MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE | MP_TYPE_FLAG_EQ_HAS_NEQ_TEST,
#endif
    .name = MP_QSTR_ndarray,
    .print = ndarray_print,
    .make_new = ndarray_make_new,
//    .subscr = ndarray_subscr,
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
    { MP_ROM_QSTR(MP_QSTR_set_printoptions), (mp_obj_t)&ndarray_set_printoptions_obj },
    { MP_ROM_QSTR(MP_QSTR_get_printoptions), (mp_obj_t)&ndarray_get_printoptions_obj },
    { MP_ROM_QSTR(MP_QSTR_ndinfo), (mp_obj_t)&ndarray_info_obj },
    { MP_ROM_QSTR(MP_QSTR_arange), (mp_obj_t)&create_arange_obj },
//    { MP_ROM_QSTR(MP_QSTR_eye), (mp_obj_t)&create_eye_obj },
    { MP_ROM_QSTR(MP_QSTR_linspace), (mp_obj_t)&create_linspace_obj },
    { MP_ROM_QSTR(MP_QSTR_ones), (mp_obj_t)&create_ones_obj },
    { MP_ROM_QSTR(MP_QSTR_zeros), (mp_obj_t)&create_zeros_obj },
    #if ULAB_APPROX_MODULE
    { MP_ROM_QSTR(MP_QSTR_approx), MP_ROM_PTR(&ulab_approx_module) },
    #endif
    #if ULAB_COMPARE_MODULE
    { MP_ROM_QSTR(MP_QSTR_compare), MP_ROM_PTR(&ulab_compare_module) },
    #endif
    #if ULAB_FFT_MODULE
    { MP_ROM_QSTR(MP_QSTR_fft), MP_ROM_PTR(&ulab_fft_module) },
    #endif
    #if ULAB_FILTER_MODULE
    { MP_ROM_QSTR(MP_QSTR_filter), MP_ROM_PTR(&ulab_filter_module) },
    #endif
    #if ULAB_LINALG_MODULE
    { MP_ROM_QSTR(MP_QSTR_linalg), MP_ROM_PTR(&ulab_linalg_module) },
    #endif
    #if ULAB_NUMERICAL_MODULE
    { MP_ROM_QSTR(MP_QSTR_numerical), MP_ROM_PTR(&ulab_numerical_module) },
    #endif
    #if ULAB_POLY_MODULE
    { MP_ROM_QSTR(MP_QSTR_poly), MP_ROM_PTR(&ulab_poly_module) },
    #endif
    #if ULAB_USER_MODULE
    { MP_ROM_QSTR(MP_QSTR_user), MP_ROM_PTR(&ulab_user_module) },
    #endif
    #if ULAB_VECTORISE_MODULE
    { MP_ROM_QSTR(MP_QSTR_vector), MP_ROM_PTR(&ulab_vectorise_module) },
    #endif
    // class constants
    { MP_ROM_QSTR(MP_QSTR_bool), MP_ROM_INT(NDARRAY_BOOL) },
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

#ifdef OPENMV
const struct _mp_obj_module_t ulab_user_cmodule = {
#else
const mp_obj_module_t ulab_user_cmodule = {
#endif
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ulab, ulab_user_cmodule, MODULE_ULAB_ENABLED);

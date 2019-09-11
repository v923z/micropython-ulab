/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Zoltán Vörös
*/
    
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "py/runtime.h"
#include "py/binary.h"
#include "py/obj.h"
#include "py/objarray.h" // this can in the future be dropped

#include "ndarray.h"
#include "linalg.h"
#include "vectorise.h"
#include "poly.h"
#include "fft.h"
#include "numerical.h"

#define ULAB_VERSION 1.0

typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;

mp_obj_float_t ulab_version = {{&mp_type_float}, ULAB_VERSION};

MP_DEFINE_CONST_FUN_OBJ_1(ndarray_shape_obj, ndarray_shape);
MP_DEFINE_CONST_FUN_OBJ_2(ndarray_size_obj, ndarray_size);
MP_DEFINE_CONST_FUN_OBJ_1(ndarray_rawsize_obj, ndarray_rawsize);

MP_DEFINE_CONST_FUN_OBJ_1(linalg_transpose_obj, linalg_transpose);
MP_DEFINE_CONST_FUN_OBJ_2(linalg_reshape_obj, linalg_reshape);
MP_DEFINE_CONST_FUN_OBJ_1(linalg_inv_obj, linalg_inv);

MP_DEFINE_CONST_FUN_OBJ_1(vectorise_acos_obj, vectorise_acos);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_acosh_obj, vectorise_acosh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_asin_obj, vectorise_asin);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_asinh_obj, vectorise_asinh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_atan_obj, vectorise_atan);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_atanh_obj, vectorise_atanh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_ceil_obj, vectorise_ceil);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_cos_obj, vectorise_cos);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_erf_obj, vectorise_erf);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_erfc_obj, vectorise_erfc);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_exp_obj, vectorise_exp);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_expm1_obj, vectorise_expm1);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_floor_obj, vectorise_floor);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_gamma_obj, vectorise_gamma);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_lgamma_obj, vectorise_lgamma);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log_obj, vectorise_log);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log10_obj, vectorise_log10);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log2_obj, vectorise_log2);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sin_obj, vectorise_sin);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sinh_obj, vectorise_sinh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sqrt_obj, vectorise_sqrt);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_tan_obj, vectorise_tan);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_tanh_obj, vectorise_tanh);

MP_DEFINE_CONST_FUN_OBJ_3(numerical_linspace_obj, numerical_linspace);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_sum_obj, 1, numerical_sum);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_mean_obj, 1, numerical_mean);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_std_obj, 1, numerical_std);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_min_obj, 1, numerical_min);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_max_obj, 1, numerical_max);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_argmin_obj, 1, numerical_argmin);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_argmax_obj, 1, numerical_argmax);

STATIC MP_DEFINE_CONST_FUN_OBJ_2(poly_polyval_obj, poly_polyval);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fft_fft_obj, fft_fft);

STATIC const mp_rom_map_elem_t ulab_ndarray_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_shape), MP_ROM_PTR(&ndarray_shape_obj) },
    { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&ndarray_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_rawsize), MP_ROM_PTR(&ndarray_rawsize_obj) },
    { MP_ROM_QSTR(MP_QSTR_transpose), MP_ROM_PTR(&linalg_transpose_obj) },
    { MP_ROM_QSTR(MP_QSTR_reshape), MP_ROM_PTR(&linalg_reshape_obj) },
//    { MP_ROM_QSTR(MP_QSTR_get), MP_ROM_PTR(&ndarray_get_obj) },
//    { MP_ROM_QSTR(MP_QSTR_dot), MP_ROM_PTR(&ndarray_dot_obj) },    
};

STATIC MP_DEFINE_CONST_DICT(ulab_ndarray_locals_dict, ulab_ndarray_locals_dict_table);

const mp_obj_type_t ulab_ndarray_type = {
    { &mp_type_type },
    .name = MP_QSTR_ndarray,
    .print = ndarray_print,
    .make_new = ndarray_make_new,
    .subscr = ndarray_subscr,
    .getiter = ndarray_getiter,
//    .unary_op = ndarray_unary_op,
    .binary_op = ndarray_binary_op,
    .locals_dict = (mp_obj_dict_t*)&ulab_ndarray_locals_dict,
};

STATIC const mp_map_elem_t ulab_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_ulab) },
    { MP_ROM_QSTR(MP_QSTR___version__), MP_ROM_PTR(&ulab_version) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ndarray), (mp_obj_t)&ulab_ndarray_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_inv), (mp_obj_t)&linalg_inv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_acos), (mp_obj_t)&vectorise_acos_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_acosh), (mp_obj_t)&vectorise_acosh_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_asin), (mp_obj_t)&vectorise_asin_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_asinh), (mp_obj_t)&vectorise_asinh_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_atan), (mp_obj_t)&vectorise_atan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_atanh), (mp_obj_t)&vectorise_atanh_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ceil), (mp_obj_t)&vectorise_ceil_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_cos), (mp_obj_t)&vectorise_cos_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_erf), (mp_obj_t)&vectorise_erf_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_erfc), (mp_obj_t)&vectorise_erfc_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_exp), (mp_obj_t)&vectorise_exp_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_expm1), (mp_obj_t)&vectorise_expm1_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_floor), (mp_obj_t)&vectorise_floor_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_gamma), (mp_obj_t)&vectorise_gamma_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_lgamma), (mp_obj_t)&vectorise_lgamma_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_log), (mp_obj_t)&vectorise_log_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_log10), (mp_obj_t)&vectorise_log10_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_log2), (mp_obj_t)&vectorise_log2_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sin), (mp_obj_t)&vectorise_sin_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sinh), (mp_obj_t)&vectorise_sinh_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sqrt), (mp_obj_t)&vectorise_sqrt_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_tan), (mp_obj_t)&vectorise_tan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_tanh), (mp_obj_t)&vectorise_tanh_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_linspace), (mp_obj_t)&numerical_linspace_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sum), (mp_obj_t)&numerical_sum_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mean), (mp_obj_t)&numerical_mean_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_std), (mp_obj_t)&numerical_std_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_min), (mp_obj_t)&numerical_min_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_max), (mp_obj_t)&numerical_max_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_argmin), (mp_obj_t)&numerical_argmin_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_argmax), (mp_obj_t)&numerical_argmax_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_polyval), (mp_obj_t)&poly_polyval_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fft), (mp_obj_t)&fft_fft_obj },
    // class constants. I don't know, whether they are necessary
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

const mp_obj_module_t ulab_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_globals,
};


MP_REGISTER_MODULE(MP_QSTR_ulab, ulab_user_cmodule, MODULE_ULAB_ENABLED);

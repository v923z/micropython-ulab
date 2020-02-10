
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
#include "linalg.h"
#include "vectorise.h"
#include "poly.h"
#include "fft.h"
#include "filter.h"
#include "numerical.h"

STATIC MP_DEFINE_STR_OBJ(ulab_version_obj, "0.30.2");

MP_DEFINE_CONST_FUN_OBJ_1(ndarray_shape_obj, ndarray_shape);
MP_DEFINE_CONST_FUN_OBJ_1(ndarray_rawsize_obj, ndarray_rawsize);
MP_DEFINE_CONST_FUN_OBJ_KW(ndarray_flatten_obj, 1, ndarray_flatten);
MP_DEFINE_CONST_FUN_OBJ_1(ndarray_asbytearray_obj, ndarray_asbytearray);

STATIC const mp_rom_map_elem_t ulab_ndarray_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_shape), MP_ROM_PTR(&ndarray_shape_obj) },
    { MP_ROM_QSTR(MP_QSTR_rawsize), MP_ROM_PTR(&ndarray_rawsize_obj) },
    { MP_ROM_QSTR(MP_QSTR_flatten), MP_ROM_PTR(&ndarray_flatten_obj) },    
    { MP_ROM_QSTR(MP_QSTR_asbytearray), MP_ROM_PTR(&ndarray_asbytearray_obj) },
    #if ULAB_LINALG_TRANSPOSE
    { MP_ROM_QSTR(MP_QSTR_transpose), MP_ROM_PTR(&linalg_transpose_obj) },
    #endif
    #if ULAB_LINALG_RESHAPE
    { MP_ROM_QSTR(MP_QSTR_reshape), MP_ROM_PTR(&linalg_reshape_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_sort), MP_ROM_PTR(&numerical_sort_inplace_obj) },
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
    #if ULAB_LINALG_SIZE
    { MP_OBJ_NEW_QSTR(MP_QSTR_size), (mp_obj_t)&linalg_size_obj },
    #endif
    #if ULAB_LINALG_INV
    { MP_OBJ_NEW_QSTR(MP_QSTR_inv), (mp_obj_t)&linalg_inv_obj },
    #endif
    #if ULAB_LINALG_DOT
    { MP_ROM_QSTR(MP_QSTR_dot), (mp_obj_t)&linalg_dot_obj },
    #endif
    #if ULAB_LINALG_ZEROS
    { MP_ROM_QSTR(MP_QSTR_zeros), (mp_obj_t)&linalg_zeros_obj },
    #endif
    #if ULAB_LINALG_ONES
    { MP_ROM_QSTR(MP_QSTR_ones), (mp_obj_t)&linalg_ones_obj },
    #endif
    #if ULAB_LINALG_EYE
    { MP_ROM_QSTR(MP_QSTR_eye), (mp_obj_t)&linalg_eye_obj },
    #endif
    #if ULAB_LINALG_DET
    { MP_ROM_QSTR(MP_QSTR_det), (mp_obj_t)&linalg_det_obj },
    #endif
    #if ULAB_LINALG_EIG
    { MP_ROM_QSTR(MP_QSTR_eig), (mp_obj_t)&linalg_eig_obj },
    #endif
    #if ULAB_VECTORISE_ACOS
    { MP_OBJ_NEW_QSTR(MP_QSTR_acos), (mp_obj_t)&vectorise_acos_obj },
    #endif
    #if ULAB_VECTORISE_ACOSH
    { MP_OBJ_NEW_QSTR(MP_QSTR_acosh), (mp_obj_t)&vectorise_acosh_obj },
    #endif
    #if ULAB_VECTORISE_ASIN
    { MP_OBJ_NEW_QSTR(MP_QSTR_asin), (mp_obj_t)&vectorise_asin_obj },
    #endif
    #if ULAB_VECTORISE_ASINH
    { MP_OBJ_NEW_QSTR(MP_QSTR_asinh), (mp_obj_t)&vectorise_asinh_obj },
    #endif
    #if ULAB_VECTORISE_ATAN
    { MP_OBJ_NEW_QSTR(MP_QSTR_atan), (mp_obj_t)&vectorise_atan_obj },
    #endif
    #if ULAB_VECTORISE_ATANH
    { MP_OBJ_NEW_QSTR(MP_QSTR_atanh), (mp_obj_t)&vectorise_atanh_obj },
    #endif
    #if ULAB_VECTORISE_CEIL
    { MP_OBJ_NEW_QSTR(MP_QSTR_ceil), (mp_obj_t)&vectorise_ceil_obj },
    #endif
    #if ULAB_VECTORISE_COS
    { MP_OBJ_NEW_QSTR(MP_QSTR_cos), (mp_obj_t)&vectorise_cos_obj },
    #endif
    #if ULAB_VECTORISE_ERF
    { MP_OBJ_NEW_QSTR(MP_QSTR_erf), (mp_obj_t)&vectorise_erf_obj },
    #endif
    #if ULAB_VECTORISE_ERFC
    { MP_OBJ_NEW_QSTR(MP_QSTR_erfc), (mp_obj_t)&vectorise_erfc_obj },
    #endif
    #if ULAB_VECTORISE_EXP
    { MP_OBJ_NEW_QSTR(MP_QSTR_exp), (mp_obj_t)&vectorise_exp_obj },
    #endif
    #if ULAB_VECTORISE_EXPM1
    { MP_OBJ_NEW_QSTR(MP_QSTR_expm1), (mp_obj_t)&vectorise_expm1_obj },
    #endif
    #if ULAB_VECTORISE_FLOOR
    { MP_OBJ_NEW_QSTR(MP_QSTR_floor), (mp_obj_t)&vectorise_floor_obj },
    #endif
    #if ULAB_VECTORISE_GAMMA
    { MP_OBJ_NEW_QSTR(MP_QSTR_gamma), (mp_obj_t)&vectorise_gamma_obj },
    #endif
    #if ULAB_VECTORISE_LGAMMA
    { MP_OBJ_NEW_QSTR(MP_QSTR_lgamma), (mp_obj_t)&vectorise_lgamma_obj },
    #endif
    #if ULAB_VECTORISE_LOG
    { MP_OBJ_NEW_QSTR(MP_QSTR_log), (mp_obj_t)&vectorise_log_obj },
    #endif
    #if ULAB_VECTORISE_LOG10
    { MP_OBJ_NEW_QSTR(MP_QSTR_log10), (mp_obj_t)&vectorise_log10_obj },
    #endif
    #if ULAB_VECTORISE_LOG2
    { MP_OBJ_NEW_QSTR(MP_QSTR_log2), (mp_obj_t)&vectorise_log2_obj },
    #endif
    #if ULAB_VECTORISE_SIN
    { MP_OBJ_NEW_QSTR(MP_QSTR_sin), (mp_obj_t)&vectorise_sin_obj },
    #endif
    #if ULAB_VECTORISE_
    { MP_OBJ_NEW_QSTR(MP_QSTR_sinh), (mp_obj_t)&vectorise_sinh_obj },
    #endif
    #if ULAB_VECTORISE_SQRT
    { MP_OBJ_NEW_QSTR(MP_QSTR_sqrt), (mp_obj_t)&vectorise_sqrt_obj },
    #endif
    #if ULAB_VECTORISE_TAN
    { MP_OBJ_NEW_QSTR(MP_QSTR_tan), (mp_obj_t)&vectorise_tan_obj },
    #endif
    #if ULAB_VECTORISE_TAHN
    { MP_OBJ_NEW_QSTR(MP_QSTR_tanh), (mp_obj_t)&vectorise_tanh_obj },
    #endif
    #if ULAB_NUMERICAL_LINSPACE
    { MP_OBJ_NEW_QSTR(MP_QSTR_linspace), (mp_obj_t)&numerical_linspace_obj },
    #endif
    #if ULAB_NUMERICAL_SUM
    { MP_OBJ_NEW_QSTR(MP_QSTR_sum), (mp_obj_t)&numerical_sum_obj },
    #endif
    #if ULAB_NUMERICAL_MEAN
    { MP_OBJ_NEW_QSTR(MP_QSTR_mean), (mp_obj_t)&numerical_mean_obj },
    #endif
    #if ULAB_NUMERICAL_STD
    { MP_OBJ_NEW_QSTR(MP_QSTR_std), (mp_obj_t)&numerical_std_obj },
    #endif
    #if ULAB_NUMERICAL_MIN
    { MP_OBJ_NEW_QSTR(MP_QSTR_min), (mp_obj_t)&numerical_min_obj },
    #endif
    #if ULAB_NUMERICAL_MAX
    { MP_OBJ_NEW_QSTR(MP_QSTR_max), (mp_obj_t)&numerical_max_obj },
    #endif
    #if ULAB_NUMERICAL_ARGMIN
    { MP_OBJ_NEW_QSTR(MP_QSTR_argmin), (mp_obj_t)&numerical_argmin_obj },
    #endif
    #if ULAB_NUMERICAL_ARGMAX
    { MP_OBJ_NEW_QSTR(MP_QSTR_argmax), (mp_obj_t)&numerical_argmax_obj },
    #endif
    #if ULAB_NUMERICAL_ROLL
    { MP_OBJ_NEW_QSTR(MP_QSTR_roll), (mp_obj_t)&numerical_roll_obj },
    #endif
    #if ULAB_NUMERICAL_FLIP
    { MP_OBJ_NEW_QSTR(MP_QSTR_flip), (mp_obj_t)&numerical_flip_obj },
    #endif
    #if ULAB_NUMERICAL_DIFF
    { MP_OBJ_NEW_QSTR(MP_QSTR_diff), (mp_obj_t)&numerical_diff_obj },
    #endif
    #if ULAB_NUMERICAL_SORT
    { MP_OBJ_NEW_QSTR(MP_QSTR_sort), (mp_obj_t)&numerical_sort_obj },
    #endif
    #if ULAB_NUMERICAL_ARGSORT
    { MP_OBJ_NEW_QSTR(MP_QSTR_argsort), (mp_obj_t)&numerical_argsort_obj },
    #endif
    #if ULAB_POLY_POLYVAL
    { MP_OBJ_NEW_QSTR(MP_QSTR_polyval), (mp_obj_t)&poly_polyval_obj },
    #endif
    #if ULAB_POLY_POLYFIT
    { MP_OBJ_NEW_QSTR(MP_QSTR_polyfit), (mp_obj_t)&poly_polyfit_obj },
    #endif
    #if ULAB_FFT_FFT
    { MP_OBJ_NEW_QSTR(MP_QSTR_fft), (mp_obj_t)&fft_fft_obj },
    #endif
    #if ULAB_FFT_IFFT
    { MP_OBJ_NEW_QSTR(MP_QSTR_ifft), (mp_obj_t)&fft_ifft_obj },
    #endif
    #if ULAB_FFT_SPECTRUM
    { MP_OBJ_NEW_QSTR(MP_QSTR_spectrum), (mp_obj_t)&fft_spectrum_obj },
    #endif
    #if ULAB_FILTER_CONVOLVE
    { MP_OBJ_NEW_QSTR(MP_QSTR_convolve), (mp_obj_t)&filter_convolve_obj },
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

const mp_obj_module_t ulab_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ulab, ulab_user_cmodule, MODULE_ULAB_ENABLED);

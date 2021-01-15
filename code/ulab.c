
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

#define ULAB_VERSION 1.7.1
#define xstr(s) str(s)
#define str(s) #s
#if ULAB_NUMPY_COMPATIBILITY
#define ULAB_VERSION_STRING xstr(ULAB_VERSION) xstr(-) xstr(ULAB_MAX_DIMS) xstr(D) xstr(-numpy)
#else
#define ULAB_VERSION_STRING xstr(ULAB_VERSION) xstr(-) xstr(ULAB_MAX_DIMS) xstr(D) xstr(-cpy)
#endif

STATIC MP_DEFINE_STR_OBJ(ulab_version_obj, ULAB_VERSION_STRING);

#if ULAB_HAS_MATH_CONSTANTS
mp_obj_float_t ulab_const_float_e_obj = {{&mp_type_float}, MP_E};
mp_obj_float_t ulab_const_float_pi_obj = {{&mp_type_float}, MP_PI};
#endif

#if ULAB_HAS_INF
mp_obj_float_t ulab_const_float_inf_obj = {{&mp_type_float}, (mp_float_t)INFINITY};
#endif

#if ULAB_HAS_NAN
mp_obj_float_t ulab_const_float_nan_obj = {{&mp_type_float}, (mp_float_t)NAN};
#endif

STATIC const mp_rom_map_elem_t ulab_ndarray_locals_dict_table[] = {
    // these are the methods and properties of an ndarray
    #if ULAB_MAX_DIMS > 1
        #if NDARRAY_HAS_RESHAPE
            { MP_ROM_QSTR(MP_QSTR_reshape), MP_ROM_PTR(&ndarray_reshape_obj) },
        #endif
        #if NDARRAY_HAS_TRANSPOSE
            { MP_ROM_QSTR(MP_QSTR_transpose), MP_ROM_PTR(&ndarray_transpose_obj) },
        #endif
    #endif
    #if NDARRAY_HAS_COPY
        { MP_ROM_QSTR(MP_QSTR_copy), MP_ROM_PTR(&ndarray_copy_obj) },
    #endif
    #if NDARRAY_HAS_DTYPE
        { MP_ROM_QSTR(MP_QSTR_dtype), MP_ROM_PTR(&ndarray_dtype_obj) },
    #endif
    #if NDARRAY_HAS_FLATTEN
        { MP_ROM_QSTR(MP_QSTR_flatten), MP_ROM_PTR(&ndarray_flatten_obj) },
    #endif
    #if NDARRAY_HAS_ITEMSIZE
        { MP_ROM_QSTR(MP_QSTR_itemsize), MP_ROM_PTR(&ndarray_itemsize_obj) },
    #endif
    #if NDARRAY_HAS_SHAPE
        { MP_ROM_QSTR(MP_QSTR_shape), MP_ROM_PTR(&ndarray_shape_obj) },
    #endif
    #if NDARRAY_HAS_SIZE
        { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&ndarray_size_obj) },
    #endif
    #if NDARRAY_HAS_STRIDES
        { MP_ROM_QSTR(MP_QSTR_strides), MP_ROM_PTR(&ndarray_strides_obj) },
    #endif
    #if NDARRAY_HAS_TOBYTES
        { MP_ROM_QSTR(MP_QSTR_tobytes), MP_ROM_PTR(&ndarray_tobytes_obj) },
    #endif
    #if NDARRAY_HAS_SORT
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
    #if NDARRAY_IS_SLICEABLE
    .subscr = ndarray_subscr,
    #endif
    #if NDARRAY_IS_ITERABLE
    .getiter = ndarray_getiter,
    #endif
    #if NDARRAY_HAS_UNARY_OPS
    .unary_op = ndarray_unary_op,
    #endif
    #if NDARRAY_HAS_BINARY_OPS
    .binary_op = ndarray_binary_op,
    #endif
    .buffer_p = { .get_buffer = ndarray_get_buffer, },
    .locals_dict = (mp_obj_dict_t*)&ulab_ndarray_locals_dict,
};

#if ULAB_HAS_DTYPE_OBJECT
const mp_obj_type_t ulab_dtype_type = {
    { &mp_type_type },
    .name = MP_QSTR_dtype,
    .print = ndarray_dtype_print,
    .make_new = ndarray_dtype_make_new,
};
#endif

STATIC const mp_map_elem_t ulab_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_ulab) },
    { MP_ROM_QSTR(MP_QSTR___version__), MP_ROM_PTR(&ulab_version_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_array), (mp_obj_t)&ulab_ndarray_type },
    #if ULAB_HAS_DTYPE_OBJECT
        { MP_OBJ_NEW_QSTR(MP_QSTR_dtype), (mp_obj_t)&ulab_dtype_type },
    #else
        #if NDARRAY_HAS_DTYPE
        { MP_OBJ_NEW_QSTR(MP_QSTR_dtype), (mp_obj_t)&ndarray_dtype_obj },
        #endif /* NDARRAY_HAS_DTYPE */
    #endif /* ULAB_HAS_DTYPE_OBJECT */
    #if ULAB_HAS_PRINTOPTIONS
        { MP_ROM_QSTR(MP_QSTR_set_printoptions), (mp_obj_t)&ndarray_set_printoptions_obj },
        { MP_ROM_QSTR(MP_QSTR_get_printoptions), (mp_obj_t)&ndarray_get_printoptions_obj },
    #endif
    #if ULAB_HAS_NDINFO
        { MP_ROM_QSTR(MP_QSTR_ndinfo), (mp_obj_t)&ndarray_info_obj },
    #endif
    #if ULAB_CREATE_HAS_ARANGE
        { MP_ROM_QSTR(MP_QSTR_arange), (mp_obj_t)&create_arange_obj },
    #endif
    #if ULAB_CREATE_HAS_CONCATENATE
        { MP_ROM_QSTR(MP_QSTR_concatenate), (mp_obj_t)&create_concatenate_obj },
    #endif
    #if ULAB_CREATE_HAS_DIAG
        { MP_ROM_QSTR(MP_QSTR_diag), (mp_obj_t)&create_diag_obj },
    #endif
    #if ULAB_MAX_DIMS > 1
        #if ULAB_CREATE_HAS_EYE
            { MP_ROM_QSTR(MP_QSTR_eye), (mp_obj_t)&create_eye_obj },
        #endif
    #endif /* ULAB_MAX_DIMS */
    // frombuffer adds 600 bytes to the firmware
    #if ULAB_CREATE_HAS_FROMBUFFER
        { MP_ROM_QSTR(MP_QSTR_frombuffer), (mp_obj_t)&create_frombuffer_obj },
    #endif
    #if ULAB_CREATE_HAS_FULL
        { MP_ROM_QSTR(MP_QSTR_full), (mp_obj_t)&create_full_obj },
    #endif
    #if ULAB_CREATE_HAS_LINSPACE
        { MP_ROM_QSTR(MP_QSTR_linspace), (mp_obj_t)&create_linspace_obj },
    #endif
    #if ULAB_CREATE_HAS_LOGSPACE
        { MP_ROM_QSTR(MP_QSTR_logspace), (mp_obj_t)&create_logspace_obj },
    #endif
    #if ULAB_CREATE_HAS_ONES
        { MP_ROM_QSTR(MP_QSTR_ones), (mp_obj_t)&create_ones_obj },
    #endif
    #if ULAB_CREATE_HAS_ZEROS
        { MP_ROM_QSTR(MP_QSTR_zeros), (mp_obj_t)&create_zeros_obj },
    #endif
    #if ULAB_APPROX_MODULE
        #if !ULAB_NUMPY_COMPATIBILITY
            { MP_ROM_QSTR(MP_QSTR_approx), MP_ROM_PTR(&ulab_approx_module) },
        #else
            #if ULAB_APPROX_HAS_BISECT
                { MP_OBJ_NEW_QSTR(MP_QSTR_bisect), (mp_obj_t)&approx_bisect_obj },
            #endif
            #if ULAB_APPROX_HAS_FMIN
                { MP_OBJ_NEW_QSTR(MP_QSTR_fmin), (mp_obj_t)&approx_fmin_obj },
            #endif
            #if ULAB_APPROX_HAS_CURVE_FIT
                { MP_OBJ_NEW_QSTR(MP_QSTR_curve_fit), (mp_obj_t)&approx_curve_fit_obj },
            #endif
            #if ULAB_APPROX_HAS_INTERP
                { MP_OBJ_NEW_QSTR(MP_QSTR_interp), (mp_obj_t)&approx_interp_obj },
            #endif
            #if ULAB_APPROX_HAS_NEWTON
                { MP_OBJ_NEW_QSTR(MP_QSTR_newton), (mp_obj_t)&approx_newton_obj },
            #endif
            #if ULAB_APPROX_HAS_TRAPZ
                { MP_OBJ_NEW_QSTR(MP_QSTR_trapz), (mp_obj_t)&approx_trapz_obj },
            #endif
        #endif /* ULAB_NUMPY_COMPATIBILITY */
    #endif /* ULAB_APPROX_MODULE */
    #if ULAB_COMPARE_MODULE
        #if !ULAB_NUMPY_COMPATIBILITY
            { MP_ROM_QSTR(MP_QSTR_compare), MP_ROM_PTR(&ulab_compare_module) },
        #else
            #if ULAB_COMPARE_HAS_CLIP
                { MP_OBJ_NEW_QSTR(MP_QSTR_clip), (mp_obj_t)&compare_clip_obj },
            #endif
            #if ULAB_COMPARE_HAS_EQUAL
                { MP_OBJ_NEW_QSTR(MP_QSTR_equal), (mp_obj_t)&compare_equal_obj },
            #endif
            #if ULAB_COMPARE_HAS_NOTEQUAL
                { MP_OBJ_NEW_QSTR(MP_QSTR_not_equal), (mp_obj_t)&compare_not_equal_obj },
            #endif
            #if ULAB_COMPARE_HAS_MAXIMUM
                { MP_OBJ_NEW_QSTR(MP_QSTR_maximum), (mp_obj_t)&compare_maximum_obj },
            #endif
            #if ULAB_COMPARE_HAS_MINIMUM
                { MP_OBJ_NEW_QSTR(MP_QSTR_minimum), (mp_obj_t)&compare_minimum_obj },
            #endif
        #endif /* ULAB_NUMPY_COMPATIBILITY */
    #endif /* ULAB_COMPARE_MODULE */
    #if ULAB_FFT_MODULE
        { MP_ROM_QSTR(MP_QSTR_fft), MP_ROM_PTR(&ulab_fft_module) },
    #endif
    #if ULAB_FILTER_MODULE
        #if !ULAB_NUMPY_COMPATIBILITY
            { MP_ROM_QSTR(MP_QSTR_filter), MP_ROM_PTR(&ulab_filter_module) },
        #else
            #if ULAB_FILTER_HAS_CONVOLVE
                { MP_OBJ_NEW_QSTR(MP_QSTR_convolve), (mp_obj_t)&filter_convolve_obj },
            #endif
            #if ULAB_FILTER_HAS_SOSFILT
                { MP_OBJ_NEW_QSTR(MP_QSTR_sosfilt), (mp_obj_t)&filter_sosfilt_obj },
            #endif
        #endif /* ULAB_NUMPY_COMPATIBILITY */
    #endif /* ULAB_FFT_MODULE */
    #if ULAB_LINALG_MODULE
    { MP_ROM_QSTR(MP_QSTR_linalg), MP_ROM_PTR(&ulab_linalg_module) },
    #endif
    #if ULAB_NUMERICAL_MODULE
        #if !ULAB_NUMPY_COMPATIBILITY
            { MP_ROM_QSTR(MP_QSTR_numerical), MP_ROM_PTR(&ulab_numerical_module) },
        #else
            #if ULAB_NUMERICAL_HAS_ARGMINMAX
                { MP_OBJ_NEW_QSTR(MP_QSTR_argmax), (mp_obj_t)&numerical_argmax_obj },
                { MP_OBJ_NEW_QSTR(MP_QSTR_argmin), (mp_obj_t)&numerical_argmin_obj },
            #endif
            #if ULAB_NUMERICAL_HAS_ARGSORT
                { MP_OBJ_NEW_QSTR(MP_QSTR_argsort), (mp_obj_t)&numerical_argsort_obj },
            #endif
            #if ULAB_NUMERICAL_HAS_CROSS
                { MP_OBJ_NEW_QSTR(MP_QSTR_cross), (mp_obj_t)&numerical_cross_obj },
            #endif
            #if ULAB_NUMERICAL_HAS_DIFF
                { MP_OBJ_NEW_QSTR(MP_QSTR_diff), (mp_obj_t)&numerical_diff_obj },
            #endif
            #if ULAB_NUMERICAL_HAS_FLIP
                { MP_OBJ_NEW_QSTR(MP_QSTR_flip), (mp_obj_t)&numerical_flip_obj },
            #endif
            #if ULAB_NUMERICAL_HAS_MINMAX
                { MP_OBJ_NEW_QSTR(MP_QSTR_max), (mp_obj_t)&numerical_max_obj },
            #endif
            #if ULAB_NUMERICAL_HAS_MEAN
                { MP_OBJ_NEW_QSTR(MP_QSTR_mean), (mp_obj_t)&numerical_mean_obj },
            #endif
            #if ULAB_NUMERICAL_HAS_MEDIAN
                { MP_OBJ_NEW_QSTR(MP_QSTR_median), (mp_obj_t)&numerical_median_obj },
            #endif
            #if ULAB_NUMERICAL_HAS_MINMAX
                { MP_OBJ_NEW_QSTR(MP_QSTR_min), (mp_obj_t)&numerical_min_obj },
            #endif
            #if ULAB_NUMERICAL_HAS_ROLL
                { MP_OBJ_NEW_QSTR(MP_QSTR_roll), (mp_obj_t)&numerical_roll_obj },
            #endif
            #if ULAB_NUMERICAL_HAS_SORT
                { MP_OBJ_NEW_QSTR(MP_QSTR_sort), (mp_obj_t)&numerical_sort_obj },
            #endif
            #if ULAB_NUMERICAL_HAS_STD
                { MP_OBJ_NEW_QSTR(MP_QSTR_std), (mp_obj_t)&numerical_std_obj },
            #endif
            #if ULAB_NUMERICAL_HAS_SUM
                { MP_OBJ_NEW_QSTR(MP_QSTR_sum), (mp_obj_t)&numerical_sum_obj },
            #endif
        #endif /* ULAB_NUMPY_COMPATIBILITY */
    #endif
    #if ULAB_POLY_MODULE
        #if !ULAB_NUMPY_COMPATIBILITY
            { MP_ROM_QSTR(MP_QSTR_poly), MP_ROM_PTR(&ulab_poly_module) },
        #else
            #if ULAB_POLY_HAS_POLYFIT
                { MP_ROM_QSTR(MP_QSTR_polyfit), (mp_obj_t)&poly_polyfit_obj },
            #endif
            #if ULAB_POLY_HAS_POLYVAL
                { MP_ROM_QSTR(MP_QSTR_polyval), (mp_obj_t)&poly_polyval_obj },
            #endif
        #endif /* ULAB_NUMPY_COMPATIBILITY */
    #endif /* ULAB_POLY_MODULE */
    #if ULAB_USER_MODULE
        { MP_ROM_QSTR(MP_QSTR_user), MP_ROM_PTR(&ulab_user_module) },
    #endif
    #if ULAB_VECTORISE_MODULE
        #if !ULAB_NUMPY_COMPATIBILITY
            { MP_ROM_QSTR(MP_QSTR_vector), MP_ROM_PTR(&ulab_vectorise_module) },
        #else
            #if ULAB_VECTORISE_HAS_ACOS
                { MP_OBJ_NEW_QSTR(MP_QSTR_acos), (mp_obj_t)&vectorise_acos_obj },
            #endif
            #if ULAB_VECTORISE_HAS_ACOSH
                { MP_OBJ_NEW_QSTR(MP_QSTR_acosh), (mp_obj_t)&vectorise_acosh_obj },
            #endif
            #if ULAB_VECTORISE_HAS_ARCTAN2
                { MP_OBJ_NEW_QSTR(MP_QSTR_arctan2), (mp_obj_t)&vectorise_arctan2_obj },
            #endif
            #if ULAB_VECTORISE_HAS_AROUND
                { MP_OBJ_NEW_QSTR(MP_QSTR_around), (mp_obj_t)&vectorise_around_obj },
            #endif
            #if ULAB_VECTORISE_HAS_ASIN
                { MP_OBJ_NEW_QSTR(MP_QSTR_asin), (mp_obj_t)&vectorise_asin_obj },
            #endif
            #if ULAB_VECTORISE_HAS_ASINH
                { MP_OBJ_NEW_QSTR(MP_QSTR_asinh), (mp_obj_t)&vectorise_asinh_obj },
            #endif
            #if ULAB_VECTORISE_HAS_ATAN
                { MP_OBJ_NEW_QSTR(MP_QSTR_atan), (mp_obj_t)&vectorise_atan_obj },
            #endif
            #if ULAB_VECTORISE_HAS_ATANH
                { MP_OBJ_NEW_QSTR(MP_QSTR_atanh), (mp_obj_t)&vectorise_atanh_obj },
            #endif
            #if ULAB_VECTORISE_HAS_CEIL
                { MP_OBJ_NEW_QSTR(MP_QSTR_ceil), (mp_obj_t)&vectorise_ceil_obj },
            #endif
            #if ULAB_VECTORISE_HAS_COS
                { MP_OBJ_NEW_QSTR(MP_QSTR_cos), (mp_obj_t)&vectorise_cos_obj },
            #endif
            #if ULAB_VECTORISE_HAS_COSH
                { MP_OBJ_NEW_QSTR(MP_QSTR_cosh), (mp_obj_t)&vectorise_cosh_obj },
            #endif
            #if ULAB_VECTORISE_HAS_DEGREES
                { MP_OBJ_NEW_QSTR(MP_QSTR_degrees), (mp_obj_t)&vectorise_degrees_obj },
            #endif
            #if ULAB_VECTORISE_HAS_ERF
                { MP_OBJ_NEW_QSTR(MP_QSTR_erf), (mp_obj_t)&vectorise_erf_obj },
            #endif
            #if ULAB_VECTORISE_HAS_ERFC
                { MP_OBJ_NEW_QSTR(MP_QSTR_erfc), (mp_obj_t)&vectorise_erfc_obj },
            #endif
            #if ULAB_VECTORISE_HAS_EXP
                { MP_OBJ_NEW_QSTR(MP_QSTR_exp), (mp_obj_t)&vectorise_exp_obj },
            #endif
            #if ULAB_VECTORISE_HAS_EXPM1
                { MP_OBJ_NEW_QSTR(MP_QSTR_expm1), (mp_obj_t)&vectorise_expm1_obj },
            #endif
            #if ULAB_VECTORISE_HAS_FLOOR
                { MP_OBJ_NEW_QSTR(MP_QSTR_floor), (mp_obj_t)&vectorise_floor_obj },
            #endif
            #if ULAB_VECTORISE_HAS_GAMMA
                { MP_OBJ_NEW_QSTR(MP_QSTR_gamma), (mp_obj_t)&vectorise_gamma_obj },
            #endif
            #if ULAB_VECTORISE_HAS_LGAMMA
                { MP_OBJ_NEW_QSTR(MP_QSTR_lgamma), (mp_obj_t)&vectorise_lgamma_obj },
            #endif
            #if ULAB_VECTORISE_HAS_LOG
                { MP_OBJ_NEW_QSTR(MP_QSTR_log), (mp_obj_t)&vectorise_log_obj },
            #endif
            #if ULAB_VECTORISE_HAS_LOG10
                { MP_OBJ_NEW_QSTR(MP_QSTR_log10), (mp_obj_t)&vectorise_log10_obj },
            #endif
            #if ULAB_VECTORISE_HAS_LOG2
                { MP_OBJ_NEW_QSTR(MP_QSTR_log2), (mp_obj_t)&vectorise_log2_obj },
            #endif
            #if ULAB_VECTORISE_HAS_RADIANS
                { MP_OBJ_NEW_QSTR(MP_QSTR_radians), (mp_obj_t)&vectorise_radians_obj },
            #endif
            #if ULAB_VECTORISE_HAS_SIN
                { MP_OBJ_NEW_QSTR(MP_QSTR_sin), (mp_obj_t)&vectorise_sin_obj },
            #endif
            #if ULAB_VECTORISE_HAS_SINH
                { MP_OBJ_NEW_QSTR(MP_QSTR_sinh), (mp_obj_t)&vectorise_sinh_obj },
            #endif
            #if ULAB_VECTORISE_HAS_SQRT
                { MP_OBJ_NEW_QSTR(MP_QSTR_sqrt), (mp_obj_t)&vectorise_sqrt_obj },
            #endif
            #if ULAB_VECTORISE_HAS_TAN
                { MP_OBJ_NEW_QSTR(MP_QSTR_tan), (mp_obj_t)&vectorise_tan_obj },
            #endif
            #if ULAB_VECTORISE_HAS_TANH
                { MP_OBJ_NEW_QSTR(MP_QSTR_tanh), (mp_obj_t)&vectorise_tanh_obj },
            #endif
            #if ULAB_VECTORISE_HAS_VECTORIZE
                { MP_OBJ_NEW_QSTR(MP_QSTR_vectorize), (mp_obj_t)&vectorise_vectorize_obj },
            #endif
        #endif /* ULAB_NUMPY_COMPATIBILITY */
    #endif /* ULAB_VECTORISE_MODULE */
    // math constants
    #if ULAB_HAS_MATH_CONSTANTS
        { MP_ROM_QSTR(MP_QSTR_e), MP_ROM_PTR(&ulab_const_float_e_obj) },
        { MP_ROM_QSTR(MP_QSTR_pi), MP_ROM_PTR(&ulab_const_float_pi_obj) },
    #endif
    #if ULAB_HAS_INF
        { MP_ROM_QSTR(MP_QSTR_inf), MP_ROM_PTR(&ulab_const_float_inf_obj) },
    #endif
    #if ULAB_HAS_NAN
        { MP_ROM_QSTR(MP_QSTR_nan), MP_ROM_PTR(&ulab_const_float_nan_obj) },
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

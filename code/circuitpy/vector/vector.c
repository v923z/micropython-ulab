
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
 *               2020 Jeff Epler for Adafruit Industries
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020 Taku Fukada
*/

#include "py/obj.h"
#include "py/runtime.h"
#include "py/objarray.h"

#include "../ulabcpy.h"
#include "../../numpy/vector/vectorise.h"

STATIC const mp_rom_map_elem_t ulab_vectorise_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_vector) },
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
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_vectorise_globals, ulab_vectorise_globals_table);

mp_obj_module_t ulab_vectorise_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_vectorise_globals,
};

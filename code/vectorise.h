
/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#ifndef _VECTORISE_
#define _VECTORISE_

#include "ulab.h"
#include "ndarray.h"


#if ULAB_VECTORISE_ACOS
mp_obj_t vectorise_acos(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_acos_obj);
#endif

#if ULAB_VECTORISE_ACOSH
mp_obj_t vectorise_acosh(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_acosh_obj);
#endif

#if ULAB_VECTORISE_ASIN
mp_obj_t vectorise_asin(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_asin_obj);
#endif

#if ULAB_VECTORISE_ASINH
mp_obj_t vectorise_asinh(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_asinh_obj);
#endif

#if ULAB_VECTORISE_ATANH
mp_obj_t vectorise_atan(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_atan_obj);
#endif

#if ULAB_VECTORISE_ATANH
mp_obj_t vectorise_atanh(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_atanh_obj);
#endif

#if ULAB_VECTORISE_CEIL
mp_obj_t vectorise_ceil(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_ceil_obj);
#endif

#if ULAB_VECTORISE_COS
mp_obj_t vectorise_cos(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_cos_obj);
#endif

#if ULAB_VECTORISE_ERF
mp_obj_t vectorise_erf(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_erf_obj);
#endif

#if ULAB_VECTORISE_ERFC
mp_obj_t vectorise_erfc(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_erfc_obj);
#endif

#if ULAB_VECTORISE_EXP
mp_obj_t vectorise_exp(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_exp_obj);
#endif

#if ULAB_VECTORISE_EXPM1
mp_obj_t vectorise_expm1(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_expm1_obj);
#endif

#if ULAB_VECTORISE_FLOOR
mp_obj_t vectorise_floor(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_floor_obj);
#endif

#if ULAB_VECTORISE_GAMMA
mp_obj_t vectorise_gamma(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_gamma_obj);
#endif

#if ULAB_VECTORISE_LGAMMA
mp_obj_t vectorise_lgamma(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_lgamma_obj);
#endif

#if ULAB_VECTORISE_LOG
mp_obj_t vectorise_log(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_log_obj);
#endif

#if ULAB_VECTORISE_LOG10
mp_obj_t vectorise_log10(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_log10_obj);
#endif

#if ULAB_VECTORISE_LOG2
mp_obj_t vectorise_log2(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_log2_obj);
#endif

#if ULAB_VECTORISE_SIN
mp_obj_t vectorise_sin(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_sin_obj);
#endif

#if ULAB_VECTORISE_SINH
mp_obj_t vectorise_sinh(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_sinh_obj);
#endif

#if ULAB_VECTORISE_SQRT
mp_obj_t vectorise_sqrt(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_sqrt_obj);
#endif

#if ULAB_VECTORISE_TAN
mp_obj_t vectorise_tan(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_tan_obj);
#endif

#if ULAB_VECTORISE_TANH
mp_obj_t vectorise_tanh(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_tanh_obj);
#endif

#define ITERATE_VECTOR(type, source, out) do {\
    type *input = (type *)(source)->array->items;\
    for(size_t i=0; i < (source)->array->len; i++) {\
                (out)[i] = f(input[i]);\
    }\
} while(0)

#define MATH_FUN_1(py_name, c_name) \
    mp_obj_t vectorise_ ## py_name(mp_obj_t x_obj) { \
        return vectorise_generic_vector(x_obj, MICROPY_FLOAT_C_FUN(c_name)); \
    }
    
#endif

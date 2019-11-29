/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Zoltán Vörös
*/
    
#ifndef _VECTORISE_
#define _VECTORISE_

#include "ndarray.h"

mp_obj_t vectorise_acos(mp_obj_t );
mp_obj_t vectorise_acosh(mp_obj_t );
mp_obj_t vectorise_asin(mp_obj_t );
mp_obj_t vectorise_asinh(mp_obj_t );
mp_obj_t vectorise_atan(mp_obj_t );
mp_obj_t vectorise_atanh(mp_obj_t );
mp_obj_t vectorise_ceil(mp_obj_t );
mp_obj_t vectorise_cos(mp_obj_t );
mp_obj_t vectorise_erf(mp_obj_t );
mp_obj_t vectorise_erfc(mp_obj_t );
mp_obj_t vectorise_exp(mp_obj_t );
mp_obj_t vectorise_expm1(mp_obj_t );
mp_obj_t vectorise_floor(mp_obj_t );
mp_obj_t vectorise_gamma(mp_obj_t );
mp_obj_t vectorise_lgamma(mp_obj_t );
mp_obj_t vectorise_log(mp_obj_t );
mp_obj_t vectorise_log10(mp_obj_t );
mp_obj_t vectorise_log2(mp_obj_t );
mp_obj_t vectorise_sin(mp_obj_t );
mp_obj_t vectorise_sinh(mp_obj_t );
mp_obj_t vectorise_sqrt(mp_obj_t );
mp_obj_t vectorise_tan(mp_obj_t );
mp_obj_t vectorise_tanh(mp_obj_t );

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

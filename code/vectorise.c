
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
#include "py/runtime.h"
#include "py/binary.h"
#include "py/obj.h"
#include "py/objarray.h"
#include "vectorise.h"

#ifndef MP_PI
#define MP_PI MICROPY_FLOAT_CONST(3.14159265358979323846)
#endif
    
mp_obj_t vectorise_generic_vector(mp_obj_t o_in, mp_float_t (*f)(mp_float_t)) {
    // Return a single value, if o_in is not iterable
    if(mp_obj_is_float(o_in) || mp_obj_is_integer(o_in)) {
            return mp_obj_new_float(f(mp_obj_get_float(o_in)));
    }
    mp_float_t x;
    if(mp_obj_is_type(o_in, &ulab_ndarray_type)) {
        ndarray_obj_t *source = MP_OBJ_TO_PTR(o_in);
        ndarray_obj_t *ndarray = create_new_ndarray(source->m, source->n, NDARRAY_FLOAT);
        mp_float_t *dataout = (mp_float_t *)ndarray->array->items;
        if(source->array->typecode == NDARRAY_UINT8) {
            ITERATE_VECTOR(uint8_t, source, dataout);
        } else if(source->array->typecode == NDARRAY_INT8) {
            ITERATE_VECTOR(int8_t, source, dataout);
        } else if(source->array->typecode == NDARRAY_UINT16) {
            ITERATE_VECTOR(uint16_t, source, dataout);
        } else if(source->array->typecode == NDARRAY_INT16) {
            ITERATE_VECTOR(int16_t, source, dataout);
        } else {
            ITERATE_VECTOR(mp_float_t, source, dataout);
        }
        return MP_OBJ_FROM_PTR(ndarray);
    } else if(mp_obj_is_type(o_in, &mp_type_tuple) || mp_obj_is_type(o_in, &mp_type_list) || 
        mp_obj_is_type(o_in, &mp_type_range)) { // i.e., the input is a generic iterable
            mp_obj_array_t *o = MP_OBJ_TO_PTR(o_in);
            ndarray_obj_t *out = create_new_ndarray(1, o->len, NDARRAY_FLOAT);
            mp_float_t *dataout = (mp_float_t *)out->array->items;
            mp_obj_iter_buf_t iter_buf;
            mp_obj_t item, iterable = mp_getiter(o_in, &iter_buf);
            size_t i=0;
            while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
                x = mp_obj_get_float(item);
                dataout[i++] = f(x);
            }
        return MP_OBJ_FROM_PTR(out);
    }
    return mp_const_none;
}


#if ULAB_VECTORISE_ACOS
MATH_FUN_1(acos, acos);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_acos_obj, vectorise_acos);
#endif

#if ULAB_VECTORISE_ACOSH
MATH_FUN_1(acosh, acosh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_acosh_obj, vectorise_acosh);
#endif

#if ULAB_VECTORISE_ASIN
MATH_FUN_1(asin, asin);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_asin_obj, vectorise_asin);
#endif

#if ULAB_VECTORISE_ASINH
MATH_FUN_1(asinh, asinh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_asinh_obj, vectorise_asinh);
#endif

#if ULAB_VECTORISE_ATAN
MATH_FUN_1(atan, atan);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_atan_obj, vectorise_atan);
#endif

#if ULAB_VECTORISE_ATANH
MATH_FUN_1(atanh, atanh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_atanh_obj, vectorise_atanh);
#endif

#if ULAB_VECTORISE_CEIL
MATH_FUN_1(ceil, ceil);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_ceil_obj, vectorise_ceil);
#endif

#if ULAB_VECTORISE_COS
MATH_FUN_1(cos, cos);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_cos_obj, vectorise_cos);
#endif

#if ULAB_VECTORISE_ERF
MATH_FUN_1(erf, erf);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_erf_obj, vectorise_erf);
#endif

#if ULAB_VECTORISE_ERFC
MATH_FUN_1(erfc, erfc);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_erfc_obj, vectorise_erfc);
#endif

#if ULAB_VECTORISE_EXP
MATH_FUN_1(exp, exp);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_exp_obj, vectorise_exp);
#endif

#if ULAB_VECTORISE_EXPM1
MATH_FUN_1(expm1, expm1);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_expm1_obj, vectorise_expm1);
#endif

#if ULAB_VECTORISE_FLOOR
MATH_FUN_1(floor, floor);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_floor_obj, vectorise_floor);
#endif

#if ULAB_VECTORISE_GAMMA
MATH_FUN_1(gamma, tgamma);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_gamma_obj, vectorise_gamma);
#endif

#if ULAB_VECTORISE_LGAMMA
MATH_FUN_1(lgamma, lgamma);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_lgamma_obj, vectorise_lgamma);
#endif

#if ULAB_VECTORISE_LOG
MATH_FUN_1(log, log);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log_obj, vectorise_log);
#endif

#if ULAB_VECTORISE_LOG10
MATH_FUN_1(log10, log10);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log10_obj, vectorise_log10);
#endif

#if ULAB_VECTORISE_LOG2
MATH_FUN_1(log2, log2);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log2_obj, vectorise_log2);
#endif

#if ULAB_VECTORISE_SIN
MATH_FUN_1(sin, sin);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sin_obj, vectorise_sin);
#endif

#if ULAB_VECTORISE_SINH
MATH_FUN_1(sinh, sinh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sinh_obj, vectorise_sinh);
#endif

#if ULAB_VECTORISE_SQRT
MATH_FUN_1(sqrt, sqrt);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sqrt_obj, vectorise_sqrt);
#endif

#if ULAB_VECTORISE_TAN
MATH_FUN_1(tan, tan);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_tan_obj, vectorise_tan);
#endif

#if ULAB_VECTORISE_TANH
MATH_FUN_1(tanh, tanh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_tanh_obj, vectorise_tanh);
#endif

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
#include <complex.h>
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
    if(MP_OBJ_IS_TYPE(o_in, &ulab_ndarray_type)) {
        ndarray_obj_t *o = MP_OBJ_TO_PTR(o_in);
        ndarray_obj_t *out = create_new_ndarray(o->m, o->n, NDARRAY_FLOAT);
        float *datain = (float *)o->data->items;
        float *dataout = (float *)out->data->items;
        for(size_t i=0; i < o->data->len; i++) {
            dataout[i] = f(datain[i]);
        }
        return MP_OBJ_FROM_PTR(out);
    } else if(MP_OBJ_IS_TYPE(o_in, &mp_type_tuple) || MP_OBJ_IS_TYPE(o_in, &mp_type_list) || 
        MP_OBJ_IS_TYPE(o_in, &mp_type_range)) {
            mp_obj_array_t *o = MP_OBJ_TO_PTR(o_in);
            ndarray_obj_t *out = create_new_ndarray(1, o->len, NDARRAY_FLOAT);
            float *dataout = (float *)out->data->items;
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


#define MATH_FUN_1(py_name, c_name) \
    mp_obj_t vectorise_ ## py_name(mp_obj_t x_obj) { \
        return vectorise_generic_vector(x_obj, MICROPY_FLOAT_C_FUN(c_name)); \
    }

// _degrees won't compile for the unix port
/*
mp_float_t _degreesf(mp_float_t x) {
    return(180*x/MP_PI);
}

MATH_FUN_1(degrees, _degrees);

// _radians won't compile for the unix port
mp_float_t _radiansf(mp_float_t x) {
    return(MP_PI*x/180.0);
}

MATH_FUN_1(radians, _radians);

STATIC mp_float_t _fabsf(mp_float_t x) {
    return fabsf(x);
}

MATH_FUN_1(fabs, _fabs);
*/
MATH_FUN_1(acos, acos);
MATH_FUN_1(acosh, acosh);
MATH_FUN_1(asin, asin);
MATH_FUN_1(asinh, asinh);
MATH_FUN_1(atan, atan);	
MATH_FUN_1(atanh, atanh);
MATH_FUN_1(ceil, ceil);
MATH_FUN_1(cos, cos);
MATH_FUN_1(erf, erf);
MATH_FUN_1(erfc, erfc);
MATH_FUN_1(exp, exp);
MATH_FUN_1(expm1, expm1);
MATH_FUN_1(floor, floor);
MATH_FUN_1(gamma, tgamma);
MATH_FUN_1(lgamma, lgamma);
MATH_FUN_1(log, log);
MATH_FUN_1(log10, log10);
MATH_FUN_1(log2, log2);
MATH_FUN_1(sin, sin);
MATH_FUN_1(sinh, sinh);
MATH_FUN_1(sqrt, sqrt);
MATH_FUN_1(tan, tan);
MATH_FUN_1(tanh, tanh);

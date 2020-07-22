
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
    
#if ULAB_VECTORISE_MODULE

//| """Element-by-element functions
//| These functions can operate on numbers, 1-D iterables, 1-D arrays, or 2-D arrays by
//| applying the function to every element in the array.  This is typically
//| much more efficient than expressing the same operation as a Python loop."""
//|
    
static mp_obj_t vectorise_generic_vector(mp_obj_t o_in, mp_float_t (*f)(mp_float_t)) {
    // Return a single value, if o_in is not iterable
    if(mp_obj_is_float(o_in) || MP_OBJ_IS_INT(o_in)) {
            return mp_obj_new_float(f(mp_obj_get_float(o_in)));
    }
    mp_float_t x;
    if(MP_OBJ_IS_TYPE(o_in, &ulab_ndarray_type)) {
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
    } else if(MP_OBJ_IS_TYPE(o_in, &mp_type_tuple) || MP_OBJ_IS_TYPE(o_in, &mp_type_list) || 
        MP_OBJ_IS_TYPE(o_in, &mp_type_range)) { // i.e., the input is a generic iterable
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

//| def acos():
//|    """Computes the inverse cosine function"""
//|    ...
//|

MATH_FUN_1(acos, acos);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_acos_obj, vectorise_acos);

//| def acosh():
//|    """Computes the inverse hyperbolic cosine function"""
//|    ...
//|

MATH_FUN_1(acosh, acosh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_acosh_obj, vectorise_acosh);


//| def asin():
//|    """Computes the inverse sine function"""
//|    ...
//|

MATH_FUN_1(asin, asin);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_asin_obj, vectorise_asin);

//| def asinh():
//|    """Computes the inverse hyperbolic sine function"""
//|    ...
//|

MATH_FUN_1(asinh, asinh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_asinh_obj, vectorise_asinh);


//| def around(a, *, decimals):
//|    """Returns a new float array in which each element is rounded to
//|       ``decimals`` places."""
//|    ...
//|

static mp_obj_t vectorise_around(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_decimals, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0 } }
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
	if(!MP_OBJ_IS_TYPE(args[0].u_obj, &ulab_ndarray_type)) {
		mp_raise_TypeError(translate("first argument must be an ndarray"));
	}
    int8_t n = args[1].u_int;
	mp_float_t mul = MICROPY_FLOAT_C_FUN(pow)(10.0, n);
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0].u_obj);
    ndarray_obj_t *result = create_new_ndarray(ndarray->m, ndarray->n, NDARRAY_FLOAT);
    mp_float_t *array = (mp_float_t *)result->array->items;
    for(size_t i=0; i < ndarray->array->len; i++) {
		mp_float_t f = ndarray_get_float_value(ndarray->array->items, ndarray->array->typecode, i);
		*array++ = MICROPY_FLOAT_C_FUN(round)(f * mul) / mul;
	}
    return MP_OBJ_FROM_PTR(result);
}

MP_DEFINE_CONST_FUN_OBJ_KW(vectorise_around_obj, 1, vectorise_around);

//| def atan():
//|    """Computes the inverse tangent function; the return values are in the
//|       range [-pi/2,pi/2]."""
//|    ...
//|

MATH_FUN_1(atan, atan);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_atan_obj, vectorise_atan);

//| def atan2(y,x):
//|    """Computes the inverse tangent function of y/x; the return values are in
//|       the range [-pi, pi]."""
//|    ...
//|

static mp_obj_t vectorise_arctan2(mp_obj_t x, mp_obj_t y) {
	// the function is implemented for scalars and ndarrays only, with partial 
	// broadcasting: arguments must be either scalars, or ndarrays of equal size/shape
	if(!(MP_OBJ_IS_INT(x) || mp_obj_is_float(x) || MP_OBJ_IS_TYPE(x, &ulab_ndarray_type)) &&
		!(MP_OBJ_IS_INT(y) || mp_obj_is_float(y) || MP_OBJ_IS_TYPE(y, &ulab_ndarray_type))) {
		mp_raise_TypeError(translate("arctan2 is implemented for scalars and ndarrays only"));
	}
	ndarray_obj_t *ndarray_x, *ndarray_y;
	if(MP_OBJ_IS_INT(x) || mp_obj_is_float(x)) {
		ndarray_x = create_new_ndarray(1, 1, NDARRAY_FLOAT);
		mp_float_t *array_x = (mp_float_t *)ndarray_x->array->items;
		*array_x = mp_obj_get_float(x);
	} else {
		ndarray_x = MP_OBJ_TO_PTR(x);
	}
	if(MP_OBJ_IS_INT(y) || mp_obj_is_float(y)) {
		ndarray_y = create_new_ndarray(1, 1, NDARRAY_FLOAT);
		mp_float_t *array_y = (mp_float_t *)ndarray_y->array->items;
		*array_y = mp_obj_get_float(y);
	} else {
		ndarray_y = MP_OBJ_TO_PTR(y);
	}
	// check, whether partial broadcasting is possible here
	if((ndarray_x->m != ndarray_y->m) || (ndarray_x->n != ndarray_y->n)) {
		if((ndarray_x->array->len != 1) && (ndarray_y->array->len != 1)) {
            mp_raise_ValueError(translate("operands could not be broadcast together"));
		}
	}
	size_t xinc = 0, yinc = 0;
	size_t m = MAX(ndarray_x->m, ndarray_y->m);
	size_t n = MAX(ndarray_x->n, ndarray_y->n);
	size_t len = MAX(ndarray_x->array->len, ndarray_y->array->len);
	if(ndarray_x->array->len != 1) {
		xinc = 1;
	}
	if(ndarray_y->array->len != 1) {
		yinc = 1;
	}
	size_t posx = 0, posy = 0;
	ndarray_obj_t *result = create_new_ndarray(m, n, NDARRAY_FLOAT);
	mp_float_t *array_r = (mp_float_t *)result->array->items;
	for(size_t i=0; i < len; i++) {
		mp_float_t value_x = ndarray_get_float_value(ndarray_x->array->items, ndarray_x->array->typecode, posx);
		mp_float_t value_y = ndarray_get_float_value(ndarray_y->array->items, ndarray_y->array->typecode, posy);
		*array_r++ = MICROPY_FLOAT_C_FUN(atan2)(value_x, value_y);
		posx += xinc;
		posy += yinc;
	}
    return MP_OBJ_FROM_PTR(result);
}

MP_DEFINE_CONST_FUN_OBJ_2(vectorise_arctan2_obj, vectorise_arctan2);



//| def atanh():
//|    """Computes the inverse hyperbolic tangent function"""
//|    ...
//|

MATH_FUN_1(atanh, atanh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_atanh_obj, vectorise_atanh);

//| def ceil():
//|    """Rounds numbers up to the next whole number"""
//|    ...
//|

MATH_FUN_1(ceil, ceil);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_ceil_obj, vectorise_ceil);

//| def cos():
//|    """Computes the cosine function"""
//|    ...
//|

MATH_FUN_1(cos, cos);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_cos_obj, vectorise_cos);

//| def cosh():
//|    """Computes the hyperbolic cosine function"""
//|    ...
//|

MATH_FUN_1(cosh, cosh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_cosh_obj, vectorise_cosh);

//| def erf():
//|    """Computes the error function, which has applications in statistics"""
//|    ...
//|

MATH_FUN_1(erf, erf);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_erf_obj, vectorise_erf);

//| def erfc():
//|    """Computes the complementary error function, which has applications in statistics"""
//|    ...
//|

MATH_FUN_1(erfc, erfc);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_erfc_obj, vectorise_erfc);

//| def exp():
//|    """Computes the exponent function."""
//|    ...
//|

MATH_FUN_1(exp, exp);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_exp_obj, vectorise_exp);

//| def expm1():
//|    """Computes $e^x-1$.  In certain applications, using this function preserves numeric accuracy better than the `exp` function."""
//|    ...
//|

MATH_FUN_1(expm1, expm1);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_expm1_obj, vectorise_expm1);

//| def floor():
//|    """Rounds numbers up to the next whole number"""
//|    ...
//|

MATH_FUN_1(floor, floor);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_floor_obj, vectorise_floor);

//| def gamma():
//|    """Computes the gamma function"""
//|    ...
//|

MATH_FUN_1(gamma, tgamma);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_gamma_obj, vectorise_gamma);

//| def lgamma():
//|    """Computes the natural log of the gamma function"""
//|    ...
//|

MATH_FUN_1(lgamma, lgamma);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_lgamma_obj, vectorise_lgamma);

//| def log():
//|    """Computes the natural log"""
//|    ...
//|

MATH_FUN_1(log, log);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log_obj, vectorise_log);

//| def log10():
//|    """Computes the log base 10"""
//|    ...
//|

MATH_FUN_1(log10, log10);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log10_obj, vectorise_log10);

//| def log2():
//|    """Computes the log base 2"""
//|    ...
//|

MATH_FUN_1(log2, log2);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log2_obj, vectorise_log2);

//| def sin():
//|    """Computes the sine function"""
//|    ...
//|

MATH_FUN_1(sin, sin);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sin_obj, vectorise_sin);

//| def sinh():
//|    """Computes the hyperbolic sine"""
//|    ...
//|

MATH_FUN_1(sinh, sinh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sinh_obj, vectorise_sinh);

//| def sqrt():
//|    """Computes the square root"""
//|    ...
//|

MATH_FUN_1(sqrt, sqrt);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sqrt_obj, vectorise_sqrt);

//| def tan():
//|    """Computes the tangent"""
//|    ...
//|

MATH_FUN_1(tan, tan);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_tan_obj, vectorise_tan);

//| def tanh():
//|    """Computes the hyperbolic tangent"""
//|    ...

MATH_FUN_1(tanh, tanh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_tanh_obj, vectorise_tanh);

static mp_obj_t vectorise_vectorized_function_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void) n_args;
    (void) n_kw;
    vectorized_function_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t avalue[1];
    mp_obj_t fvalue;
    if(MP_OBJ_IS_TYPE(args[0], &ulab_ndarray_type)) {
        ndarray_obj_t *source = MP_OBJ_TO_PTR(args[0]);
        ndarray_obj_t *target = create_new_ndarray(source->m, source->n, self->otypes);
        for(size_t i=0; i < source->array->len; i++) {
            avalue[0] = mp_binary_get_val_array(source->array->typecode, source->array->items, i);
            fvalue = self->type->call(self->fun, 1, 0, avalue);
            mp_binary_set_val_array(self->otypes, target->array->items, i, fvalue);
        }
        return MP_OBJ_FROM_PTR(target);
    } else if(MP_OBJ_IS_TYPE(args[0], &mp_type_tuple) || MP_OBJ_IS_TYPE(args[0], &mp_type_list) ||
        MP_OBJ_IS_TYPE(args[0], &mp_type_range)) { // i.e., the input is a generic iterable
        size_t len = (size_t)mp_obj_get_int(mp_obj_len_maybe(args[0]));
        ndarray_obj_t *target = create_new_ndarray(1, len, self->otypes);
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable = mp_getiter(args[0], &iter_buf);
        size_t i=0;
        while ((avalue[0] = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            fvalue = self->type->call(self->fun, 1, 0, avalue);
            mp_binary_set_val_array(self->otypes, target->array->items, i, fvalue);
            i++;
        }
        return MP_OBJ_FROM_PTR(target);
    } else if(mp_obj_is_int(args[0]) || mp_obj_is_float(args[0])) {
        ndarray_obj_t *target = create_new_ndarray(1, 1, self->otypes);
        fvalue = self->type->call(self->fun, 1, 0, args);
        mp_binary_set_val_array(self->otypes, target->array->items, 0, fvalue);
        return MP_OBJ_FROM_PTR(target);
    } else {
        mp_raise_ValueError(translate("wrong input type"));
    }
    return mp_const_none;
}

const mp_obj_type_t vectorise_function_type = {
    { &mp_type_type },
    .name = MP_QSTR_,
    .call = vectorise_vectorized_function_call,
};

//| def vectorize(f, *, otypes=None):
//|    """
//|    :param callable f: The function to wrap
//|    :param otypes: List of array types that may be returned by the function.  None is interpreted to mean the return value is float.
//|    Wrap a Python function ``f`` so that it can be applied to arrays.
//|    The callable must return only values of the types specified by ``otypes``, or the result is undefined."""
//|    ...
//|

static mp_obj_t vectorise_vectorize(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_otypes, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} }
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    const mp_obj_type_t *type = mp_obj_get_type(args[0].u_obj);
    if(type->call == NULL) {
        mp_raise_TypeError(translate("first argument must be a callable"));
    }
    mp_obj_t _otypes = args[1].u_obj;
    uint8_t otypes = NDARRAY_FLOAT;
    if(_otypes == mp_const_none) {
        // TODO: is this what numpy does?
        otypes = NDARRAY_FLOAT;
    } else if(mp_obj_is_int(_otypes)) {
        otypes = mp_obj_get_int(_otypes);
        if(otypes != NDARRAY_FLOAT && otypes != NDARRAY_UINT8 && otypes != NDARRAY_INT8 &&
            otypes != NDARRAY_UINT16 && otypes != NDARRAY_INT16) {
                mp_raise_ValueError(translate("wrong output type"));
        }
    }
    else {
        mp_raise_ValueError(translate("wrong output type"));
    }
    vectorized_function_obj_t *function = m_new_obj(vectorized_function_obj_t);
    function->base.type = &vectorise_function_type;
    function->otypes = otypes;
    function->fun = args[0].u_obj;
    function->type = type;
    return MP_OBJ_FROM_PTR(function);
}

MP_DEFINE_CONST_FUN_OBJ_KW(vectorise_vectorize_obj, 1, vectorise_vectorize);

STATIC const mp_rom_map_elem_t ulab_vectorise_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_vector) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_acos), (mp_obj_t)&vectorise_acos_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_acosh), (mp_obj_t)&vectorise_acosh_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_arctan2), (mp_obj_t)&vectorise_arctan2_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_around), (mp_obj_t)&vectorise_around_obj },
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
    { MP_OBJ_NEW_QSTR(MP_QSTR_vectorize), (mp_obj_t)&vectorise_vectorize_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_vectorise_globals, ulab_vectorise_globals_table);

mp_obj_module_t ulab_vectorise_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_vectorise_globals,
};

#endif

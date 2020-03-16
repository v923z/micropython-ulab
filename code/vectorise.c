
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
    
#if ULAB_VECTORISE_MODULE
mp_obj_t vectorise_generic_vector(mp_obj_t o_in, mp_float_t (*f)(mp_float_t)) {
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


MATH_FUN_1(acos, acos);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_acos_obj, vectorise_acos);

MATH_FUN_1(acosh, acosh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_acosh_obj, vectorise_acosh);

MATH_FUN_1(asin, asin);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_asin_obj, vectorise_asin);

MATH_FUN_1(asinh, asinh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_asinh_obj, vectorise_asinh);

MATH_FUN_1(atan, atan);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_atan_obj, vectorise_atan);

MATH_FUN_1(atanh, atanh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_atanh_obj, vectorise_atanh);

MATH_FUN_1(ceil, ceil);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_ceil_obj, vectorise_ceil);

MATH_FUN_1(cos, cos);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_cos_obj, vectorise_cos);

MATH_FUN_1(cosh, cosh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_cosh_obj, vectorise_cosh);

MATH_FUN_1(erf, erf);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_erf_obj, vectorise_erf);

MATH_FUN_1(erfc, erfc);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_erfc_obj, vectorise_erfc);

MATH_FUN_1(exp, exp);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_exp_obj, vectorise_exp);

MATH_FUN_1(expm1, expm1);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_expm1_obj, vectorise_expm1);

MATH_FUN_1(floor, floor);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_floor_obj, vectorise_floor);

MATH_FUN_1(gamma, tgamma);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_gamma_obj, vectorise_gamma);

MATH_FUN_1(lgamma, lgamma);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_lgamma_obj, vectorise_lgamma);

MATH_FUN_1(log, log);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log_obj, vectorise_log);

MATH_FUN_1(log10, log10);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log10_obj, vectorise_log10);

MATH_FUN_1(log2, log2);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log2_obj, vectorise_log2);

MATH_FUN_1(sin, sin);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sin_obj, vectorise_sin);

MATH_FUN_1(sinh, sinh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sinh_obj, vectorise_sinh);

MATH_FUN_1(sqrt, sqrt);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sqrt_obj, vectorise_sqrt);

MATH_FUN_1(tan, tan);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_tan_obj, vectorise_tan);

MATH_FUN_1(tanh, tanh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_tanh_obj, vectorise_tanh);

mp_obj_t vectorise_around(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

mp_obj_t vectorise_arctan2(mp_obj_t x, mp_obj_t y) {
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
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_vectorise_globals, ulab_vectorise_globals_table);

mp_obj_module_t ulab_vectorise_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_vectorise_globals,
};

#endif

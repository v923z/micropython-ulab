/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Zoltán Vörös
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "approx.h"

#if ULAB_APPROX_MODULE

const mp_obj_float_t xtolerance = {{&mp_type_float}, 2.4e-7};
const mp_obj_float_t rtolerance = {{&mp_type_float}, 0.0};

mp_float_t approx_python_call(const mp_obj_type_t *type, mp_obj_t fun, mp_float_t x, mp_obj_t *fargs) {
	// Helper function for calculating the value of f(x, a, b, c, ...), 
	// where f is defined in python. Takes a float, returns a float.
	fargs[0] = mp_obj_new_float(x);
	return mp_obj_get_float(type->call(fun, 1, 0, fargs));	
}

mp_obj_t approx_bisect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
	// Simple bisection routine
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_xtol, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&xtolerance)} },
        { MP_QSTR_maxiter, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

	mp_obj_t fun = args[0].u_obj;
	const mp_obj_type_t *type = mp_obj_get_type(fun);
	if(type->call == NULL) {
		mp_raise_TypeError(translate("first argument must be a function"));
	}
	mp_float_t xtol = mp_obj_get_float(args[3].u_obj);
	mp_obj_t *fargs = (mp_obj_t *)malloc(sizeof(mp_obj_t));
	mp_float_t left, right;
	mp_float_t x_mid;
	mp_float_t a = mp_obj_get_float(args[1].u_obj);
	mp_float_t b = mp_obj_get_float(args[2].u_obj);
	left = approx_python_call(type, fun, a, fargs);
	right = approx_python_call(type, fun, b, fargs);
	if(left * right > 0) {
		mp_raise_ValueError(translate("function has the same sign at the ends of interval"));
	}
	mp_float_t rtb = left < 0.0 ? a : b;
	mp_float_t dx = left < 0.0 ? b - a : a - b;
	
	for(uint16_t i=0; i < args[4].u_int; i++) {
		dx *= 0.5;
		x_mid = rtb + dx;
		if(approx_python_call(type, fun, x_mid, fargs) < 0.0) {
			rtb = x_mid;
		}
		if(MICROPY_FLOAT_C_FUN(fabs)(dx) < xtol) break;
	}
	return mp_obj_new_float(rtb);
}

MP_DEFINE_CONST_FUN_OBJ_KW(approx_bisect_obj, 3, approx_bisect);

mp_obj_t approx_newton(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
	// this is actually the secant method, as the first derivative of the function 
	// is not accepted as an argument. The function whose root we want to solve for 
	// must depend on a single variable without parameters, i.e., f(x)
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_tol, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&xtolerance)} },
        { MP_QSTR_rtol, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&rtolerance)} },
        { MP_QSTR_maxiter, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 50} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

	mp_obj_t fun = args[0].u_obj;
	const mp_obj_type_t *type = mp_obj_get_type(fun);
	if(type->call == NULL) {
		mp_raise_TypeError(translate("first argument must be a function"));
	}
	mp_float_t x = mp_obj_get_float(args[1].u_obj);
	mp_float_t tol = mp_obj_get_float(args[2].u_obj);
	mp_float_t rtol = mp_obj_get_float(args[3].u_obj);
	mp_float_t dx, df, fx;
	dx = x > 0.0 ? APPROX_EPS * x : -APPROX_EPS * x;
	mp_obj_t *fargs = (mp_obj_t *)malloc(sizeof(mp_obj_t));
	for(uint16_t i=0; i < args[4].u_int; i++) {
		fx = approx_python_call(type, fun, x, fargs);
		df = (approx_python_call(type, fun, x + dx, fargs) - fx) / dx;
		dx = fx / df;
		x -= dx;
		if(MICROPY_FLOAT_C_FUN(fabs)(dx) < (tol + rtol * MICROPY_FLOAT_C_FUN(fabs)(x))) break;
	}
	return mp_obj_new_float(x);
}

MP_DEFINE_CONST_FUN_OBJ_KW(approx_newton_obj, 2, approx_newton);

mp_obj_t approx_fmin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
	// downhill simplex method in 1D
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_xatol, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&xtolerance)} },
        { MP_QSTR_fatol, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&xtolerance)} },
        { MP_QSTR_maxiter, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

	mp_obj_t fun = args[0].u_obj;
	const mp_obj_type_t *type = mp_obj_get_type(fun);
	if(type->call == NULL) {
		mp_raise_TypeError(translate("first argument must be a function"));
	}
	
	// parameters controlling convergence conditions
	mp_float_t xatol = mp_obj_get_float(args[2].u_obj);
	mp_float_t fatol = mp_obj_get_float(args[3].u_obj);	
	uint16_t maxiter = args[4].u_obj == mp_const_none ? 200 : mp_obj_get_int(args[4].u_obj);
	
	mp_float_t x0 = mp_obj_get_float(args[1].u_obj);
	mp_float_t x1 = x0 != 0.0 ? (1.0 + APPROX_NONZDELTA) * x0 : APPROX_ZDELTA;
	mp_obj_t *fargs = (mp_obj_t *)malloc(sizeof(mp_obj_t));
	mp_float_t f0 = approx_python_call(type, fun, x0, fargs);
	mp_float_t f1 = approx_python_call(type, fun, x1, fargs);
	if(f1 < f0) {
		SWAP(mp_float_t, x0, x1);
		SWAP(mp_float_t, f0, f1);
	}
	for(uint16_t i=0; i < maxiter; i++) {
		uint8_t shrink = 0;
		f0 = approx_python_call(type, fun, x0, fargs);
		f1 = approx_python_call(type, fun, x1, fargs);
		
		// reflection
		mp_float_t xr = (1.0 + APPROX_ALPHA) * x0 - APPROX_ALPHA * x1;
		mp_float_t fr = approx_python_call(type, fun, xr, fargs);
		if(fr < f0) { // expansion
			mp_float_t xe = (1 + APPROX_ALPHA * APPROX_BETA) * x0 - APPROX_ALPHA * APPROX_BETA * x1;
			mp_float_t fe = approx_python_call(type, fun, xe, fargs);
			if(fe < fr) {
				x1 = xe;
				f1 = fe;
			} else {
				x1 = xr;
				f1 = fr;
			}
		} else {
			if(fr < f1) { // contraction
				mp_float_t xc = (1 + APPROX_GAMMA * APPROX_ALPHA) * x0 - APPROX_GAMMA * APPROX_ALPHA * x1;
				mp_float_t fc = approx_python_call(type, fun, xc, fargs);
				if(fc < fr) {
					x1 = xc;
					f1 = fc;
				} else {
					shrink = 1;
				}
			} else { // inside contraction
				mp_float_t xc = (1.0 - APPROX_GAMMA) * x0 + APPROX_GAMMA * x1;
				mp_float_t fc = approx_python_call(type, fun, xc, fargs);
				if(fc < f1) {
					x1 = xc;
					f1 = fc;
				} else {
					shrink = 1;
				}
			}
			if(shrink == 1) {
				x1 = x0 + APPROX_DELTA * (x1 - x0);
				f1 = approx_python_call(type, fun, x1, fargs);
			}
			if((MICROPY_FLOAT_C_FUN(fabs)(f1 - f0) < fatol) || 
				(MICROPY_FLOAT_C_FUN(fabs)(x1 - x0) < xatol)) {
				break;
			}
			if(f1 < f0) {
				SWAP(mp_float_t, x0, x1);
				SWAP(mp_float_t, f0, f1);
			}
		}
	}
	return mp_obj_new_float(x0);
}

MP_DEFINE_CONST_FUN_OBJ_KW(approx_fmin_obj, 2, approx_fmin);


STATIC const mp_rom_map_elem_t ulab_approx_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_approx) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_bisect), (mp_obj_t)&approx_bisect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_newton), (mp_obj_t)&approx_newton_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fmin), (mp_obj_t)&approx_fmin_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_approx_globals, ulab_approx_globals_table);

mp_obj_module_t ulab_approx_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_approx_globals,
};

#endif

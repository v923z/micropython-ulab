/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Zoltán Vörös
 *               2020 Diego Elio Pettenò
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "../linalg/linalg.h"
#include "approx.h"

#if ULAB_APPROX_MODULE

//| """Numerical approximation methods"""
//|


const mp_obj_float_t xtolerance = {{&mp_type_float}, MICROPY_FLOAT_CONST(2.4e-7)};
const mp_obj_float_t rtolerance = {{&mp_type_float}, MICROPY_FLOAT_CONST(0.0)};
const mp_obj_float_t approx_trapz_dx = {{&mp_type_float}, MICROPY_FLOAT_CONST(1.0)};

STATIC mp_float_t approx_python_call(const mp_obj_type_t *type, mp_obj_t fun, mp_float_t x, mp_obj_t *fargs, uint8_t nparams) {
    // Helper function for calculating the value of f(x, a, b, c, ...),
    // where f is defined in python. Takes a float, returns a float.
    // The array of mp_obj_t type must be supplied, as must the number of parameters (a, b, c...) in nparams
    fargs[0] = mp_obj_new_float(x);
    return mp_obj_get_float(type->call(fun, nparams+1, 0, fargs));
}

#if ULAB_APPROX_HAS_BISECT
//| def bisect(
//|     fun: Callable[[float], float],
//|     a: float,
//|     b: float,
//|     *,
//|     xtol: float = 2.4e-7,
//|     maxiter: int = 100
//| ) -> float:
//|     """
//|     :param callable f: The function to bisect
//|     :param float a: The left side of the interval
//|     :param float b: The right side of the interval
//|     :param float xtol: The tolerance value
//|     :param float maxiter: The maximum number of iterations to perform
//|
//|     Find a solution (zero) of the function ``f(x)`` on the interval
//|     (``a``..``b``) using the bisection method.  The result is accurate to within
//|     ``xtol`` unless more than ``maxiter`` steps are required."""
//|     ...
//|

STATIC mp_obj_t approx_bisect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // Simple bisection routine
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
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
    mp_obj_t *fargs = m_new(mp_obj_t, 1);
    mp_float_t left, right;
    mp_float_t x_mid;
    mp_float_t a = mp_obj_get_float(args[1].u_obj);
    mp_float_t b = mp_obj_get_float(args[2].u_obj);
    left = approx_python_call(type, fun, a, fargs, 0);
    right = approx_python_call(type, fun, b, fargs, 0);
    if(left * right > 0) {
        mp_raise_ValueError(translate("function has the same sign at the ends of interval"));
    }
    mp_float_t rtb = left < MICROPY_FLOAT_CONST(0.0) ? a : b;
    mp_float_t dx = left < MICROPY_FLOAT_CONST(0.0) ? b - a : a - b;
    if(args[4].u_int < 0) {
        mp_raise_ValueError(translate("maxiter should be > 0"));
    }
    for(uint16_t i=0; i < args[4].u_int; i++) {
        dx *= MICROPY_FLOAT_CONST(0.5);
        x_mid = rtb + dx;
        if(approx_python_call(type, fun, x_mid, fargs, 0) < MICROPY_FLOAT_CONST(0.0)) {
            rtb = x_mid;
        }
        if(MICROPY_FLOAT_C_FUN(fabs)(dx) < xtol) break;
    }
    return mp_obj_new_float(rtb);
}

MP_DEFINE_CONST_FUN_OBJ_KW(approx_bisect_obj, 3, approx_bisect);
#endif

#if ULAB_APPROX_HAS_FMIN
//| def fmin(
//|     fun: Callable[[float], float],
//|     x0: float,
//|     *,
//|     xatol: float = 2.4e-7,
//|     fatol: float = 2.4e-7,
//|     maxiter: int = 200
//| ) -> float:
//|     """
//|     :param callable f: The function to bisect
//|     :param float x0: The initial x value
//|     :param float xatol: The absolute tolerance value
//|     :param float fatol: The relative tolerance value
//|
//|     Find a minimum of the function ``f(x)`` using the downhill simplex method.
//|     The located ``x`` is within ``fxtol`` of the actual minimum, and ``f(x)``
//|     is within ``fatol`` of the actual minimum unless more than ``maxiter``
//|     steps are requried."""
//|     ...
//|

STATIC mp_obj_t approx_fmin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
    uint16_t maxiter;
    if(args[4].u_obj == mp_const_none) {
        maxiter = 200;
    } else {
        if(args[4].u_obj < 0) {
            mp_raise_TypeError(translate("maxiter must be > 0"));
        }
        maxiter = (uint16_t)mp_obj_get_int(args[4].u_obj);
    }

    mp_float_t x0 = mp_obj_get_float(args[1].u_obj);
    mp_float_t x1 = x0 != MICROPY_FLOAT_CONST(0.0) ? (MICROPY_FLOAT_CONST(1.0) + APPROX_NONZDELTA) * x0 : APPROX_ZDELTA;
    mp_obj_t *fargs = m_new(mp_obj_t, 1);
    mp_float_t f0 = approx_python_call(type, fun, x0, fargs, 0);
    mp_float_t f1 = approx_python_call(type, fun, x1, fargs, 0);
    if(f1 < f0) {
        SWAP(mp_float_t, x0, x1);
        SWAP(mp_float_t, f0, f1);
    }
    for(uint16_t i=0; i < maxiter; i++) {
        uint8_t shrink = 0;
        f0 = approx_python_call(type, fun, x0, fargs, 0);
        f1 = approx_python_call(type, fun, x1, fargs, 0);

        // reflection
        mp_float_t xr = (MICROPY_FLOAT_CONST(1.0) + APPROX_ALPHA) * x0 - APPROX_ALPHA * x1;
        mp_float_t fr = approx_python_call(type, fun, xr, fargs, 0);
        if(fr < f0) { // expansion
            mp_float_t xe = (1 + APPROX_ALPHA * APPROX_BETA) * x0 - APPROX_ALPHA * APPROX_BETA * x1;
            mp_float_t fe = approx_python_call(type, fun, xe, fargs, 0);
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
                mp_float_t fc = approx_python_call(type, fun, xc, fargs, 0);
                if(fc < fr) {
                    x1 = xc;
                    f1 = fc;
                } else {
                    shrink = 1;
                }
            } else { // inside contraction
                mp_float_t xc = (MICROPY_FLOAT_CONST(1.0) - APPROX_GAMMA) * x0 + APPROX_GAMMA * x1;
                mp_float_t fc = approx_python_call(type, fun, xc, fargs, 0);
                if(fc < f1) {
                    x1 = xc;
                    f1 = fc;
                } else {
                    shrink = 1;
                }
            }
            if(shrink == 1) {
                x1 = x0 + APPROX_DELTA * (x1 - x0);
                f1 = approx_python_call(type, fun, x1, fargs, 0);
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
#endif

#if ULAB_APPROX_HAS_CURVE_FIT
static void approx_jacobi(const mp_obj_type_t *type, mp_obj_t fun, mp_float_t *x, mp_float_t *y, uint16_t len, mp_float_t *params, uint8_t nparams, mp_float_t *jacobi, mp_float_t *grad) {
    /* Calculates the Jacobian and the gradient of the cost function
     *
     * The entries in the Jacobian are
     * J(m, n) = de_m/da_n,
     *
     * where
     *
     * e_m = (f(x_m, a1, a2, ...) - y_m)/sigma_m is the error at x_m,
     *
     * and
     *
     * a1, a2, ..., a_n are the free parameters
     */
    mp_obj_t *fargs0 = m_new(mp_obj_t, lenp+1);
    mp_obj_t *fargs1 = m_new(mp_obj_t, lenp+1);
    for(uint8_t p=0; p < nparams; p++) {
        fargs0[p+1] = mp_obj_new_float(params[p]);
        fargs1[p+1] = mp_obj_new_float(params[p]);
    }
    for(uint8_t p=0; p < nparams; p++) {
        mp_float_t da = params[p] != MICROPY_FLOAT_CONST(0.0) ? (MICROPY_FLOAT_CONST(1.0) + APPROX_NONZDELTA) * params[p] : APPROX_ZDELTA;
        fargs1[p+1] = mp_obj_new_float(params[p] + da);
        grad[p] = MICROPY_FLOAT_CONST(0.0);
        for(uint16_t i=0; i < len; i++) {
            mp_float_t f0 = approx_python_call(type, fun, x[i], fargs0, nparams);
            mp_float_t f1 = approx_python_call(type, fun, x[i], fargs1, nparams);
            jacobi[i*nparamp+p] = (f1 - f0) / da;
            grad[p] += (f0 - y[i]) * jacobi[i*nparamp+p];
        }
        fargs1[p+1] = fargs0[p+1]; // set back to the original value
    }
}

static void approx_delta(mp_float_t *jacobi, mp_float_t *grad, uint16_t len, uint8_t nparams, mp_float_t lambda) {
    //
}

mp_obj_t approx_curve_fit(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // Levenberg-Marquardt non-linear fit
    // The implementation follows the introductory discussion in Mark Tanstrum's paper, https://arxiv.org/abs/1201.5885
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_p0, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
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

    mp_obj_t x_obj = args[1].u_obj;
    mp_obj_t y_obj = args[2].u_obj;
    mp_obj_t p0_obj = args[3].u_obj;
    if(!ndarray_object_is_array_like(x_obj) || !ndarray_object_is_array_like(y_obj)) {
        mp_raise_TypeError(translate("data must be iterable"));
    }
    if(!ndarray_object_is_nditerable(p0_obj)) {
        mp_raise_TypeError(translate("initial values must be iterable"));
    }
    size_t len = (size_t)mp_obj_get_int(mp_obj_len_maybe(x_obj));
    uint8_t lenp = (uint8_t)mp_obj_get_int(mp_obj_len_maybe(p0_obj));
    if(len != (uint16_t)mp_obj_get_int(mp_obj_len_maybe(y_obj))) {
        mp_raise_ValueError(translate("data must be of equal length"));
    }

    mp_float_t *x = m_new(mp_float_t, len);
    fill_array_iterable(x, x_obj);
    mp_float_t *y = m_new(mp_float_t, len);
    fill_array_iterable(y, y_obj);
    mp_float_t *p0 = m_new(mp_float_t, lenp);
    fill_array_iterable(p0, p0_obj);
    mp_float_t *grad = m_new(mp_float_t, len);
    mp_float_t *jacobi = m_new(mp_float_t, len*len);
    mp_obj_t *fargs = m_new(mp_obj_t, lenp+1);

    m_del(mp_float_t, p0, lenp);
    // parameters controlling convergence conditions
    //mp_float_t xatol = mp_obj_get_float(args[2].u_obj);
    //mp_float_t fatol = mp_obj_get_float(args[3].u_obj);

    // this has finite binary representation; we will multiply/divide by 4
    //mp_float_t lambda = 0.0078125;

    //linalg_invert_matrix(mp_float_t *data, size_t N)

    m_del(mp_float_t, x, len);
    m_del(mp_float_t, y, len);
    m_del(mp_float_t, grad, len);
    m_del(mp_float_t, jacobi, len*len);
    m_del(mp_obj_t, fargs, lenp+1);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(approx_curve_fit_obj, 2, approx_curve_fit);
#endif

#if ULAB_APPROX_HAS_INTERP
//| def interp(
//|     x: ulab.array,
//|     xp: ulab.array,
//|     fp: ulab.array,
//|     *,
//|     left: Optional[float] = None,
//|     right: Optional[float] = None
//| ) -> ulab.array:
//|     """
//|     :param ulab.array x: The x-coordinates at which to evaluate the interpolated values.
//|     :param ulab.array xp: The x-coordinates of the data points, must be increasing
//|     :param ulab.array fp: The y-coordinates of the data points, same length as xp
//|     :param left: Value to return for ``x < xp[0]``, default is ``fp[0]``.
//|     :param right: Value to return for ``x > xp[-1]``, default is ``fp[-1]``.
//|
//|     Returns the one-dimensional piecewise linear interpolant to a function with given discrete data points (xp, fp), evaluated at x."""
//|     ...
//|

STATIC mp_obj_t approx_interp(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_left, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_right, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ndarray_obj_t *x = ndarray_from_mp_obj(args[0].u_obj);
    ndarray_obj_t *xp = ndarray_from_mp_obj(args[1].u_obj); // xp must hold an increasing sequence of independent values
    ndarray_obj_t *fp = ndarray_from_mp_obj(args[2].u_obj);
    if((xp->ndim != 1) || (fp->ndim != 1) || (xp->len < 2) || (fp->len < 2) || (xp->len != fp->len)) {
        mp_raise_ValueError(translate("interp is defined for 1D arrays of equal length"));
    }

    ndarray_obj_t *y = ndarray_new_linear_array(x->len, NDARRAY_FLOAT);
    mp_float_t left_value, right_value;
    uint8_t *xparray = (uint8_t *)xp->array;
    mp_float_t xp_left = ndarray_get_float_value(xparray, xp->dtype);
    xparray += (xp->len-1) * xp->strides[ULAB_MAX_DIMS - 1];
    mp_float_t xp_right = ndarray_get_float_value(xparray, xp->dtype);

    uint8_t *fparray = (uint8_t *)fp->array;
    if(args[3].u_obj == mp_const_none) {
        left_value = ndarray_get_float_value(fparray, fp->dtype);
    } else {
        left_value = mp_obj_get_float(args[3].u_obj);
    }
    if(args[4].u_obj == mp_const_none) {
        fparray += (fp->len-1) * fp->strides[ULAB_MAX_DIMS - 1];
        right_value = ndarray_get_float_value(fp->array, fp->dtype);
    } else {
        right_value = mp_obj_get_float(args[4].u_obj);
    }

    uint8_t *xarray = (uint8_t *)x->array;
    mp_float_t *yarray = (mp_float_t *)y->array;
    for(size_t i=0; i < x->len; i++, yarray++) {
        mp_float_t x_value = ndarray_get_float_value(xarray, x->dtype);
        xarray += x->strides[ULAB_MAX_DIMS - 1];
        if(x_value <= xp_left) {
            *yarray = left_value;
        } else if(x_value >= xp_right) {
            *yarray = right_value;
        } else { // do the binary search here
            mp_float_t xp_left_, xp_right_;
            mp_float_t fp_left, fp_right;
            size_t left_index = 0, right_index = xp->len - 1, middle_index;
            while(right_index - left_index > 1) {
                middle_index = left_index + (right_index - left_index) / 2;
                mp_float_t xp_middle = ndarray_get_float_index(xparray, xp->dtype, middle_index);
                if(x_value <= xp_middle) {
                    right_index = middle_index;
                } else {
                    left_index = middle_index;
                }
            }
            xp_left_ = ndarray_get_float_index(xp->array, xp->dtype, left_index);
            xp_right_ = ndarray_get_float_index(xp->array, xp->dtype, right_index);
            fp_left = ndarray_get_float_index(fp->array, fp->dtype, left_index);
            fp_right = ndarray_get_float_index(fp->array, fp->dtype, right_index);
            *yarray = fp_left + (x_value - xp_left_) * (fp_right - fp_left) / (xp_right_ - xp_left_);
        }
    }
    return MP_OBJ_FROM_PTR(y);
}

MP_DEFINE_CONST_FUN_OBJ_KW(approx_interp_obj, 2, approx_interp);
#endif

#if ULAB_APPROX_HAS_NEWTON
//| def newton(
//|     fun: Callable[[float], float],
//|     x0: float,
//|     *,
//|     xtol: float = 2.4e-7,
//|     rtol: float = 0.0,
//|     maxiter: int = 50
//| ) -> float:
//|     """
//|     :param callable f: The function to bisect
//|     :param float x0: The initial x value
//|     :param float xtol: The absolute tolerance value
//|     :param float rtol: The relative tolerance value
//|     :param float maxiter: The maximum number of iterations to perform
//|
//|     Find a solution (zero) of the function ``f(x)`` using Newton's Method.
//|     The result is accurate to within ``xtol * rtol * |f(x)|`` unless more than
//|     ``maxiter`` steps are requried."""
//|     ...
//|

STATIC mp_obj_t approx_newton(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
    dx = x > MICROPY_FLOAT_CONST(0.0) ? APPROX_EPS * x : -APPROX_EPS * x;
    mp_obj_t *fargs = m_new(mp_obj_t, 1);
    if(args[4].u_int < 0) {
        mp_raise_ValueError(translate("maxiter must be > 0"));
    }
    for(uint16_t i=0; i < args[4].u_int; i++) {
        fx = approx_python_call(type, fun, x, fargs, 0);
        df = (approx_python_call(type, fun, x + dx, fargs, 0) - fx) / dx;
        dx = fx / df;
        x -= dx;
        if(MICROPY_FLOAT_C_FUN(fabs)(dx) < (tol + rtol * MICROPY_FLOAT_C_FUN(fabs)(x))) break;
    }
    return mp_obj_new_float(x);
}

MP_DEFINE_CONST_FUN_OBJ_KW(approx_newton_obj, 2, approx_newton);
#endif

#if ULAB_APPROX_HAS_TRAPZ
//| def trapz(y: ulab.array, x: Optional[ulab.array] = None, dx: float = 1.0) -> float:
//|     """
//|     :param 1D ulab.array y: the values of the dependent variable
//|     :param 1D ulab.array x: optional, the coordinates of the independent variable. Defaults to uniformly spaced values.
//|     :param float dx: the spacing between sample points, if x=None
//|
//|     Returns the integral of y(x) using the trapezoidal rule.
//|     """
//|     ...
//|

STATIC mp_obj_t approx_trapz(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_x, MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_dx, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&approx_trapz_dx)} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ndarray_obj_t *y = ndarray_from_mp_obj(args[0].u_obj);
    ndarray_obj_t *x;
    mp_float_t sum = MICROPY_FLOAT_CONST(0.0);
    if(y->len < 2) {
        return mp_obj_new_float(sum);
    }
    if(args[1].u_obj != mp_const_none) {
        x = ndarray_from_mp_obj(args[1].u_obj); // x must hold an increasing sequence of independent values
        if((y->ndim != 1) || (x->ndim != 1) || (y->len != x->len)) {
            mp_raise_ValueError(translate("trapz is defined for 1D arrays of equal length"));
        }
        mp_float_t x1, x2, y1, y2;
        y1 = ndarray_get_float_index(y->array, y->dtype, 0);
        x1 = ndarray_get_float_index(x->array, x->dtype, 0);
        for(size_t i=1; i < y->len; i++) {
            y2 = ndarray_get_float_index(y->array, y->dtype, i);
            x2 = ndarray_get_float_index(x->array, x->dtype, i);
            sum += (x2 - x1) * (y2 + y1);
            x1 = x2;
            y1 = y2;
        }
    } else {
        mp_float_t y1, y2;
        mp_float_t dx = mp_obj_get_float(args[2].u_obj);
        y1 = ndarray_get_float_index(y->array, y->dtype, 0);
        for(size_t i=1; i < y->len; i++) {
            y2 = ndarray_get_float_index(y->array, y->dtype, i);
            sum += (y2 + y1);
            y1 = y2;
        }
        sum *= dx;
    }
    return mp_obj_new_float(MICROPY_FLOAT_CONST(0.5)*sum);
}

MP_DEFINE_CONST_FUN_OBJ_KW(approx_trapz_obj, 1, approx_trapz);
#endif

#if !ULAB_NUMPY_COMPATIBILITY
STATIC const mp_rom_map_elem_t ulab_approx_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_approx) },
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
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_approx_globals, ulab_approx_globals_table);

mp_obj_module_t ulab_approx_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_approx_globals,
};
#endif /* ULAB_NUMPY_COMPATIBILITY */
#endif

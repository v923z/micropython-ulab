
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
 *               2020 Jeff Epler for Adafruit Industries
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020 Taku Fukada
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "py/runtime.h"
#include "py/binary.h"
#include "py/obj.h"
#include "py/objarray.h"

#include "../../ulab.h"
#include "../../ulab_tools.h"
#include "vector.h"

//| """Element-by-element functions
//|
//| These functions can operate on numbers, 1-D iterables, and arrays of 1 to 4 dimensions by
//| applying the function to every element in the array.  This is typically
//| much more efficient than expressing the same operation as a Python loop."""
//|
//| from ulab import _DType, _ArrayLike
//|

static mp_obj_t vectorise_generic_vector(mp_obj_t o_in, mp_float_t (*f)(mp_float_t)) {
    // Return a single value, if o_in is not iterable
    if(mp_obj_is_float(o_in) || MP_OBJ_IS_INT(o_in)) {
        return mp_obj_new_float(f(mp_obj_get_float(o_in)));
    }
    if(MP_OBJ_IS_TYPE(o_in, &ulab_ndarray_type)) {
        ndarray_obj_t *source = MP_OBJ_TO_PTR(o_in);
        uint8_t *sarray = (uint8_t *)source->array;
        ndarray_obj_t *ndarray = ndarray_new_dense_ndarray(source->ndim, source->shape, NDARRAY_FLOAT);
        mp_float_t *array = (mp_float_t *)ndarray->array;
        
        #if ULAB_VECTORISE_USES_FUN_POINTER
        
            mp_float_t (*func)(void *) = ndarray_get_float_function(source->dtype);
            
            #if ULAB_MAX_DIMS > 3
            size_t i = 0;
            do {
            #endif
                #if ULAB_MAX_DIMS > 2
                size_t j = 0;
                do {
                #endif
                    #if ULAB_MAX_DIMS > 1
                    size_t k = 0;
                    do {
                    #endif
                        size_t l = 0;
                        do {
                            mp_float_t value = func(sarray);
                            *array++ = f(value);
                            sarray += source->strides[ULAB_MAX_DIMS - 1];
                            l++;
                        } while(l < source->shape[ULAB_MAX_DIMS - 1]);
                    #if ULAB_MAX_DIMS > 1
                        sarray -= source->strides[ULAB_MAX_DIMS - 1] * source->shape[ULAB_MAX_DIMS-1];
                        sarray += source->strides[ULAB_MAX_DIMS - 2];
                        k++;
                    } while(k < source->shape[ULAB_MAX_DIMS - 2]);
                    #endif /* ULAB_MAX_DIMS > 1 */
                #if ULAB_MAX_DIMS > 2
                    sarray -= source->strides[ULAB_MAX_DIMS - 2] * source->shape[ULAB_MAX_DIMS-2];
                    sarray += source->strides[ULAB_MAX_DIMS - 3];
                    j++;
                } while(j < source->shape[ULAB_MAX_DIMS - 3]);
                #endif /* ULAB_MAX_DIMS > 2 */
            #if ULAB_MAX_DIMS > 3
                sarray -= source->strides[ULAB_MAX_DIMS - 3] * source->shape[ULAB_MAX_DIMS-3];
                sarray += source->strides[ULAB_MAX_DIMS - 4];
                i++;
            } while(i < source->shape[ULAB_MAX_DIMS - 4]);
            #endif /* ULAB_MAX_DIMS > 3 */
        #else
        if(source->dtype == NDARRAY_UINT8) {
            ITERATE_VECTOR(uint8_t, array, source, sarray);
        } else if(source->dtype == NDARRAY_INT8) {
            ITERATE_VECTOR(int8_t, array, source, sarray);
        } else if(source->dtype == NDARRAY_UINT16) {
            ITERATE_VECTOR(uint16_t, array, source, sarray);
        } else if(source->dtype == NDARRAY_INT16) {
            ITERATE_VECTOR(int16_t, array, source, sarray);
        } else {
            ITERATE_VECTOR(mp_float_t, array, source, sarray);
        }
        #endif /* ULAB_VECTORISE_USES_FUN_POINTER */
        
        return MP_OBJ_FROM_PTR(ndarray);
    } else if(MP_OBJ_IS_TYPE(o_in, &mp_type_tuple) || MP_OBJ_IS_TYPE(o_in, &mp_type_list) ||
        MP_OBJ_IS_TYPE(o_in, &mp_type_range)) { // i.e., the input is a generic iterable
            mp_obj_array_t *o = MP_OBJ_TO_PTR(o_in);
            ndarray_obj_t *out = ndarray_new_linear_array(o->len, NDARRAY_FLOAT);
            mp_float_t *array = (mp_float_t *)out->array;
            mp_obj_iter_buf_t iter_buf;
            mp_obj_t item, iterable = mp_getiter(o_in, &iter_buf);
            size_t i=0;
            while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
                mp_float_t x = mp_obj_get_float(item);
                *array++ = f(x);
                i++;
            }
        return MP_OBJ_FROM_PTR(out);
    }
    return mp_const_none;
}

#if ULAB_NUMPY_HAS_ACOS
//| def acos(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the inverse cosine function"""
//|    ...
//|

MATH_FUN_1(acos, acos);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_acos_obj, vectorise_acos);
#endif

#if ULAB_NUMPY_HAS_ACOSH
//| def acosh(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the inverse hyperbolic cosine function"""
//|    ...
//|

MATH_FUN_1(acosh, acosh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_acosh_obj, vectorise_acosh);
#endif

#if ULAB_NUMPY_HAS_ASIN
//| def asin(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the inverse sine function"""
//|    ...
//|

MATH_FUN_1(asin, asin);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_asin_obj, vectorise_asin);
#endif

#if ULAB_NUMPY_HAS_ASINH
//| def asinh(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the inverse hyperbolic sine function"""
//|    ...
//|

MATH_FUN_1(asinh, asinh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_asinh_obj, vectorise_asinh);
#endif

#if ULAB_NUMPY_HAS_AROUND
//| def around(a: _ArrayLike, *, decimals: int = 0) -> ulab.ndarray:
//|    """Returns a new float array in which each element is rounded to
//|       ``decimals`` places."""
//|    ...
//|

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
    ndarray_obj_t *source = MP_OBJ_TO_PTR(args[0].u_obj);
    ndarray_obj_t *ndarray = ndarray_new_dense_ndarray(source->ndim, source->shape, NDARRAY_FLOAT);
    mp_float_t *narray = (mp_float_t *)ndarray->array;
    uint8_t *sarray = (uint8_t *)source->array;

    mp_float_t (*func)(void *) = ndarray_get_float_function(source->dtype);

    #if ULAB_MAX_DIMS > 3
    size_t i = 0;
    do {
    #endif
        #if ULAB_MAX_DIMS > 2
        size_t j = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 1
            size_t k = 0;
            do {
            #endif
                size_t l = 0;
                do {
                    mp_float_t f = func(sarray);
                    *narray++ = MICROPY_FLOAT_C_FUN(round)(f * mul) / mul;
                    sarray += source->strides[ULAB_MAX_DIMS - 1];
                    l++;
                } while(l < source->shape[ULAB_MAX_DIMS - 1]);
            #if ULAB_MAX_DIMS > 1
                sarray -= source->strides[ULAB_MAX_DIMS - 1] * source->shape[ULAB_MAX_DIMS-1];
                sarray += source->strides[ULAB_MAX_DIMS - 2];
                k++;
            } while(k < source->shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 2
            sarray -= source->strides[ULAB_MAX_DIMS - 2] * source->shape[ULAB_MAX_DIMS-2];
            sarray += source->strides[ULAB_MAX_DIMS - 3];
            j++;
        } while(j < source->shape[ULAB_MAX_DIMS - 3]);
        #endif
    #if ULAB_MAX_DIMS > 3
        sarray -= source->strides[ULAB_MAX_DIMS - 3] * source->shape[ULAB_MAX_DIMS-3];
        sarray += source->strides[ULAB_MAX_DIMS - 4];
        i++;
    } while(i < source->shape[ULAB_MAX_DIMS - 4]);
    #endif
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_KW(vectorise_around_obj, 1, vectorise_around);
#endif

#if ULAB_NUMPY_HAS_ATAN
//| def atan(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the inverse tangent function; the return values are in the
//|       range [-pi/2,pi/2]."""
//|    ...
//|

MATH_FUN_1(atan, atan);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_atan_obj, vectorise_atan);
#endif

#if ULAB_NUMPY_HAS_ARCTAN2
//| def arctan2(ya: _ArrayLike, xa: _ArrayLike) -> ulab.ndarray:
//|    """Computes the inverse tangent function of y/x; the return values are in
//|       the range [-pi, pi]."""
//|    ...
//|

mp_obj_t vectorise_arctan2(mp_obj_t y, mp_obj_t x) {
    ndarray_obj_t *ndarray_x = ndarray_from_mp_obj(x);
    ndarray_obj_t *ndarray_y = ndarray_from_mp_obj(y);

    uint8_t ndim = 0;
    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    int32_t *xstrides = m_new(int32_t, ULAB_MAX_DIMS);
    int32_t *ystrides = m_new(int32_t, ULAB_MAX_DIMS);
    if(!ndarray_can_broadcast(ndarray_x, ndarray_y, &ndim, shape, xstrides, ystrides)) {
        mp_raise_ValueError(translate("operands could not be broadcast together"));
        m_del(size_t, shape, ULAB_MAX_DIMS);
        m_del(int32_t, xstrides, ULAB_MAX_DIMS);
        m_del(int32_t, ystrides, ULAB_MAX_DIMS);
    }

    uint8_t *xarray = (uint8_t *)ndarray_x->array;
    uint8_t *yarray = (uint8_t *)ndarray_y->array;

    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
    mp_float_t *rarray = (mp_float_t *)results->array;

    mp_float_t (*funcx)(void *) = ndarray_get_float_function(ndarray_x->dtype);
    mp_float_t (*funcy)(void *) = ndarray_get_float_function(ndarray_y->dtype);

    #if ULAB_MAX_DIMS > 3
    size_t i = 0;
    do {
    #endif
        #if ULAB_MAX_DIMS > 2
        size_t j = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 1
            size_t k = 0;
            do {
            #endif
                size_t l = 0;
                do {
                    mp_float_t _x = funcx(xarray);
                    mp_float_t _y = funcy(yarray);
                    *rarray++ = MICROPY_FLOAT_C_FUN(atan2)(_y, _x);
                    xarray += xstrides[ULAB_MAX_DIMS - 1];
                    yarray += ystrides[ULAB_MAX_DIMS - 1];
                    l++;
                } while(l < results->shape[ULAB_MAX_DIMS - 1]);
            #if ULAB_MAX_DIMS > 1
                xarray -= xstrides[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];
                xarray += xstrides[ULAB_MAX_DIMS - 2];
                yarray -= ystrides[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];
                yarray += ystrides[ULAB_MAX_DIMS - 2];
                k++;
            } while(k < results->shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 2
            xarray -= xstrides[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];
            xarray += xstrides[ULAB_MAX_DIMS - 3];
            yarray -= ystrides[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];
            yarray += ystrides[ULAB_MAX_DIMS - 3];
            j++;
        } while(j < results->shape[ULAB_MAX_DIMS - 3]);
        #endif
    #if ULAB_MAX_DIMS > 3
        xarray -= xstrides[ULAB_MAX_DIMS - 3] * results->shape[ULAB_MAX_DIMS-3];
        xarray += xstrides[ULAB_MAX_DIMS - 4];
        yarray -= ystrides[ULAB_MAX_DIMS - 3] * results->shape[ULAB_MAX_DIMS-3];
        yarray += ystrides[ULAB_MAX_DIMS - 4];
        i++;
    } while(i < results->shape[ULAB_MAX_DIMS - 4]);
    #endif

    return MP_OBJ_FROM_PTR(results);
}

MP_DEFINE_CONST_FUN_OBJ_2(vectorise_arctan2_obj, vectorise_arctan2);
#endif /* ULAB_VECTORISE_HAS_ARCTAN2 */

#if ULAB_NUMPY_HAS_ATANH
//| def atanh(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the inverse hyperbolic tangent function"""
//|    ...
//|

MATH_FUN_1(atanh, atanh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_atanh_obj, vectorise_atanh);
#endif

#if ULAB_NUMPY_HAS_CEIL
//| def ceil(a: _ArrayLike) -> ulab.ndarray:
//|    """Rounds numbers up to the next whole number"""
//|    ...
//|

MATH_FUN_1(ceil, ceil);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_ceil_obj, vectorise_ceil);
#endif

#if ULAB_NUMPY_HAS_COS
//| def cos(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the cosine function"""
//|    ...
//|

MATH_FUN_1(cos, cos);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_cos_obj, vectorise_cos);
#endif

#if ULAB_NUMPY_HAS_COSH
//| def cosh(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the hyperbolic cosine function"""
//|    ...
//|

MATH_FUN_1(cosh, cosh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_cosh_obj, vectorise_cosh);
#endif

#if ULAB_NUMPY_HAS_DEGREES
//| def degrees(a: _ArrayLike) -> ulab.ndarray:
//|    """Converts angles from radians to degrees"""
//|    ...
//|

static mp_float_t vectorise_degrees_(mp_float_t value) {
    return value * MICROPY_FLOAT_CONST(180.0) / MP_PI;
}

static mp_obj_t vectorise_degrees(mp_obj_t x_obj) {
    return vectorise_generic_vector(x_obj, vectorise_degrees_);
}

MP_DEFINE_CONST_FUN_OBJ_1(vectorise_degrees_obj, vectorise_degrees);
#endif

#if ULAB_SCIPY_SPECIAL_HAS_ERF
//| def erf(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the error function, which has applications in statistics"""
//|    ...
//|

MATH_FUN_1(erf, erf);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_erf_obj, vectorise_erf);
#endif

#if ULAB_SCIPY_SPECIAL_HAS_ERFC
//| def erfc(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the complementary error function, which has applications in statistics"""
//|    ...
//|

MATH_FUN_1(erfc, erfc);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_erfc_obj, vectorise_erfc);
#endif

#if ULAB_NUMPY_HAS_EXP
//| def exp(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the exponent function."""
//|    ...
//|

MATH_FUN_1(exp, exp);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_exp_obj, vectorise_exp);
#endif

#if ULAB_NUMPY_HAS_EXPM1
//| def expm1(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes $e^x-1$.  In certain applications, using this function preserves numeric accuracy better than the `exp` function."""
//|    ...
//|

MATH_FUN_1(expm1, expm1);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_expm1_obj, vectorise_expm1);
#endif

#if ULAB_NUMPY_HAS_FLOOR
//| def floor(a: _ArrayLike) -> ulab.ndarray:
//|    """Rounds numbers up to the next whole number"""
//|    ...
//|

MATH_FUN_1(floor, floor);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_floor_obj, vectorise_floor);
#endif

#if ULAB_SCIPY_SPECIAL_HAS_GAMMA
//| def gamma(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the gamma function"""
//|    ...
//|

MATH_FUN_1(gamma, tgamma);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_gamma_obj, vectorise_gamma);
#endif

#if ULAB_SCIPY_SPECIAL_HAS_GAMMALN
//| def lgamma(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the natural log of the gamma function"""
//|    ...
//|

MATH_FUN_1(lgamma, lgamma);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_lgamma_obj, vectorise_lgamma);
#endif

#if ULAB_NUMPY_HAS_LOG
//| def log(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the natural log"""
//|    ...
//|

MATH_FUN_1(log, log);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log_obj, vectorise_log);
#endif

#if ULAB_NUMPY_HAS_LOG10
//| def log10(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the log base 10"""
//|    ...
//|

MATH_FUN_1(log10, log10);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log10_obj, vectorise_log10);
#endif

#if ULAB_NUMPY_HAS_LOG2
//| def log2(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the log base 2"""
//|    ...
//|

MATH_FUN_1(log2, log2);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log2_obj, vectorise_log2);
#endif

#if ULAB_NUMPY_HAS_RADIANS
//| def radians(a: _ArrayLike) -> ulab.ndarray:
//|    """Converts angles from degrees to radians"""
//|    ...
//|

static mp_float_t vectorise_radians_(mp_float_t value) {
    return value * MP_PI / MICROPY_FLOAT_CONST(180.0);
}

static mp_obj_t vectorise_radians(mp_obj_t x_obj) {
    return vectorise_generic_vector(x_obj, vectorise_radians_);
}

MP_DEFINE_CONST_FUN_OBJ_1(vectorise_radians_obj, vectorise_radians);
#endif

#if ULAB_NUMPY_HAS_SIN
//| def sin(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the sine function"""
//|    ...
//|

MATH_FUN_1(sin, sin);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sin_obj, vectorise_sin);
#endif

#if ULAB_NUMPY_HAS_SINH
//| def sinh(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the hyperbolic sine"""
//|    ...
//|

MATH_FUN_1(sinh, sinh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sinh_obj, vectorise_sinh);
#endif

#if ULAB_NUMPY_HAS_SQRT
//| def sqrt(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the square root"""
//|    ...
//|

MATH_FUN_1(sqrt, sqrt);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sqrt_obj, vectorise_sqrt);
#endif

#if ULAB_NUMPY_HAS_TAN
//| def tan(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the tangent"""
//|    ...
//|

MATH_FUN_1(tan, tan);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_tan_obj, vectorise_tan);
#endif

#if ULAB_NUMPY_HAS_TANH
//| def tanh(a: _ArrayLike) -> ulab.ndarray:
//|    """Computes the hyperbolic tangent"""
//|    ...

MATH_FUN_1(tanh, tanh);
MP_DEFINE_CONST_FUN_OBJ_1(vectorise_tanh_obj, vectorise_tanh);
#endif

#if ULAB_NUMPY_HAS_VECTORIZE
static mp_obj_t vectorise_vectorized_function_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void) n_args;
    (void) n_kw;
    vectorized_function_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t avalue[1];
    mp_obj_t fvalue;
    if(MP_OBJ_IS_TYPE(args[0], &ulab_ndarray_type)) {
        ndarray_obj_t *source = MP_OBJ_TO_PTR(args[0]);
        ndarray_obj_t *ndarray = ndarray_new_dense_ndarray(source->ndim, source->shape, self->otypes);
        for(size_t i=0; i < source->len; i++) {
            avalue[0] = mp_binary_get_val_array(source->dtype, source->array, i);
            fvalue = self->type->call(self->fun, 1, 0, avalue);
            mp_binary_set_val_array(self->otypes, ndarray->array, i, fvalue);
        }
        return MP_OBJ_FROM_PTR(ndarray);
    } else if(MP_OBJ_IS_TYPE(args[0], &mp_type_tuple) || MP_OBJ_IS_TYPE(args[0], &mp_type_list) ||
        MP_OBJ_IS_TYPE(args[0], &mp_type_range)) { // i.e., the input is a generic iterable
        size_t len = (size_t)mp_obj_get_int(mp_obj_len_maybe(args[0]));
        ndarray_obj_t *ndarray = ndarray_new_linear_array(len, self->otypes);
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable = mp_getiter(args[0], &iter_buf);
        size_t i=0;
        while ((avalue[0] = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            fvalue = self->type->call(self->fun, 1, 0, avalue);
            mp_binary_set_val_array(self->otypes, ndarray->array, i, fvalue);
            i++;
        }
        return MP_OBJ_FROM_PTR(ndarray);
    } else if(mp_obj_is_int(args[0]) || mp_obj_is_float(args[0])) {
        ndarray_obj_t *ndarray = ndarray_new_linear_array(1, self->otypes);
        fvalue = self->type->call(self->fun, 1, 0, args);
        mp_binary_set_val_array(self->otypes, ndarray->array, 0, fvalue);
        return MP_OBJ_FROM_PTR(ndarray);
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

//| def vectorize(
//|     f: Union[Callable[[int], float], Callable[[float], float]],
//|     *,
//|     otypes: Optional[_DType] = None
//| ) -> Callable[[_ArrayLike], ulab.ndarray]:
//|    """
//|    :param callable f: The function to wrap
//|    :param otypes: List of array types that may be returned by the function.  None is interpreted to mean the return value is float.
//|
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
#endif

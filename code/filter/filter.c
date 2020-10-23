
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020 Zoltán Vörös
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "filter.h"

#if ULAB_FILTER_MODULE

#if ULAB_FILTER_HAS_CONVOLVE

//| """Filtering functions"""
//|
//| from ulab import _ArrayLike
//|

//| def convolve(a: ulab.array, v: ulab.array) -> ulab.array:
//|     """
//|     :param ulab.array a:
//|     :param ulab.array v:
//|
//|     Returns the discrete, linear convolution of two one-dimensional sequences.
//|     The result is always an array of float.  Only the ``full`` mode is supported,
//|     and the ``mode`` named parameter of numpy is not accepted. Note that all other
//|     modes can be had by slicing a ``full`` result.
//|
//|     Convolution filters can implement high pass, low pass, band pass, etc.,
//|     filtering operations.  Convolution filters are typically constructed ahead
//|     of time.  This can be done using desktop python with scipy, or on web pages
//|     such as https://fiiir.com/
//|
//|     Convolution is most time-efficient when both inputs are of float type."""
//|     ...
//|

mp_obj_t filter_convolve(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_a, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_v, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!MP_OBJ_IS_TYPE(args[0].u_obj, &ulab_ndarray_type) || !MP_OBJ_IS_TYPE(args[1].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("convolve arguments must be ndarrays"));
    }

    ndarray_obj_t *a = MP_OBJ_TO_PTR(args[0].u_obj);
    ndarray_obj_t *c = MP_OBJ_TO_PTR(args[1].u_obj);
    // deal with linear arrays only
    #if ULAB_MAX_DIMS > 1
    if((a->ndim != 1) || (c->ndim != 1)) {
        mp_raise_TypeError(translate("convolve arguments must be linear arrays"));
    }
    #endif
    size_t len_a = a->len;
    size_t len_c = c->len;
    if(len_a == 0 || len_c == 0) {
        mp_raise_TypeError(translate("convolve arguments must not be empty"));
    }

    int len = len_a + len_c - 1; // convolve mode "full"
    ndarray_obj_t *out = ndarray_new_linear_array(len, NDARRAY_FLOAT);
    mp_float_t *outptr = (mp_float_t *)out->array;
    uint8_t *aarray = (uint8_t *)a->array;
    uint8_t *carray = (uint8_t *)c->array;
    
    int off = len_c-1;
    for(int k=-off; k<len-off; k++) {
        mp_float_t accum = (mp_float_t)0;
        int top_n = MIN(len_c, len_a - k);
        int bot_n = MAX(-k, 0);
        for(int n=bot_n; n<top_n; n++) {
            int idx_c = (len_c - n - 1) * c->strides[ULAB_MAX_DIMS - 1];
            int idx_a = (n + k) * a->strides[ULAB_MAX_DIMS - 1];
            mp_float_t ai = ndarray_get_float_index(aarray, a->dtype, idx_a);
            mp_float_t ci = ndarray_get_float_index(carray, c->dtype, idx_c);
            accum += ai * ci;
        }
        *outptr++ = accum;
    }

    return out;
}

MP_DEFINE_CONST_FUN_OBJ_KW(filter_convolve_obj, 2, filter_convolve);

#endif

#if ULAB_FILTER_HAS_SOSFILT

static void filter_sosfilt_array(mp_float_t *x, const mp_float_t *coeffs, mp_float_t *zf, const size_t len) {
    for(size_t i=0; i < len; i++) {
        mp_float_t xn = *x;
        *x = coeffs[0] * xn + zf[0];
        zf[0] = zf[1] + coeffs[1] * xn - coeffs[4] * *x;
        zf[1] = coeffs[2] * xn - coeffs[5] * *x;
        x++;
    }
    x -= len;
}

//| @overload
//| def sosfilt(sos: _ArrayLike, x: _ArrayLike) -> ulab.array: ...
//| @overload
//| def sosfilt(sos: _ArrayLike, x: _ArrayLike, *, zi: ulab.array) -> Tuple[ulab.array, ulab.array]:
//|     """
//|     :param ulab.array sos: Array of second-order filter coefficients, must have shape (n_sections, 6). Each row corresponds to a second-order section, with the first three columns providing the numerator coefficients and the last three providing the denominator coefficients.
//|     :param ulab.array x: The data to be filtered
//|     :param ulab.array zi: Optional initial conditions for the filter
//|     :return: If ``zi`` is not specified, the filter result alone is returned.  If ``zi`` is specified, the return value is a 2-tuple of the filter result and the final filter conditions.
//|
//|     Filter data along one dimension using cascaded second-order sections.
//|
//|     Filter a data sequence, x, using a digital IIR filter defined by sos.
//|
//|     The filter function is implemented as a series of second-order filters with direct-form II transposed structure. It is designed to minimize numerical precision errors for high-order filters.
//|
//|     Filter coefficients can be generated by using scipy's filter generators such as ``signal.ellip(..., output='sos')``."""
//|     ...
//|

mp_obj_t filter_sosfilt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sos, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_zi, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!ndarray_object_is_array_like(args[0].u_obj) || !ndarray_object_is_array_like(args[1].u_obj)) {
        mp_raise_TypeError(translate("sosfilt requires iterable arguments"));
    }
    size_t lenx = (size_t)mp_obj_get_int(mp_obj_len_maybe(args[1].u_obj));
    ndarray_obj_t *y = ndarray_new_linear_array(lenx, NDARRAY_FLOAT);
    mp_float_t *yarray = (mp_float_t *)y->array;
    mp_float_t coeffs[6];
    if(MP_OBJ_IS_TYPE(args[1].u_obj, &ulab_ndarray_type)) {
        ndarray_obj_t *inarray = MP_OBJ_TO_PTR(args[1].u_obj);
        #if ULAB_MAX_DIMS > 1
        if(inarray->ndim > 1) {
            mp_raise_ValueError(translate("input must be one-dimensional"));
        }
        #endif
        uint8_t *iarray = (uint8_t *)inarray->array;
        for(size_t i=0; i < lenx; i++) {
            *yarray++ = ndarray_get_float_value(iarray, inarray->dtype);
            iarray += inarray->strides[ULAB_MAX_DIMS - 1];
        }
        yarray -= lenx;
    } else {
        fill_array_iterable(yarray, args[1].u_obj);
    }

    mp_obj_iter_buf_t iter_buf;
    mp_obj_t item, iterable = mp_getiter(args[0].u_obj, &iter_buf);
    size_t lensos = (size_t)mp_obj_get_int(mp_obj_len_maybe(args[0].u_obj));

    size_t shape[] = {0, 0, lensos, 2};
    ndarray_obj_t *zf = ndarray_new_dense_ndarray(2, shape, NDARRAY_FLOAT);
    mp_float_t *zf_array = (mp_float_t *)zf->array;

    if(args[2].u_obj != mp_const_none) {
        if(!MP_OBJ_IS_TYPE(args[2].u_obj, &ulab_ndarray_type)) {
            mp_raise_TypeError(translate("zi must be an ndarray"));
        } else {
            ndarray_obj_t *zi = MP_OBJ_TO_PTR(args[2].u_obj);
            if((zi->shape[ULAB_MAX_DIMS - 1] != lensos) || (zi->shape[ULAB_MAX_DIMS - 1] != 2)) {
                mp_raise_ValueError(translate("zi must be of shape (n_section, 2)"));
            }
            if(zi->dtype != NDARRAY_FLOAT) {
                mp_raise_ValueError(translate("zi must be of float type"));
            }
            // TODO: this won't work with sparse arrays
            memcpy(zf_array, zi->array, 2*lensos*sizeof(mp_float_t));
        }
    }
    while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        if(mp_obj_get_int(mp_obj_len_maybe(item)) != 6) {
            mp_raise_ValueError(translate("sos array must be of shape (n_section, 6)"));
        } else {
            fill_array_iterable(coeffs, item);
            if(coeffs[3] != MICROPY_FLOAT_CONST(1.0)) {
                mp_raise_ValueError(translate("sos[:, 3] should be all ones"));
            }
            filter_sosfilt_array(yarray, coeffs, zf_array, lenx);
            zf_array += 2;
        }
    }
    if(args[2].u_obj == mp_const_none) {
        return MP_OBJ_FROM_PTR(y);
    } else {
        mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
        tuple->items[0] = MP_OBJ_FROM_PTR(y);
        tuple->items[1] = MP_OBJ_FROM_PTR(zf);
        return tuple;
    }
}

MP_DEFINE_CONST_FUN_OBJ_KW(filter_sosfilt_obj, 2, filter_sosfilt);
#endif

#if !ULAB_NUMPY_COMPATIBILITY
STATIC const mp_rom_map_elem_t ulab_filter_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_filter) },
    #if ULAB_FILTER_HAS_CONVOLVE
    { MP_OBJ_NEW_QSTR(MP_QSTR_convolve), (mp_obj_t)&filter_convolve_obj },
    #endif
    #if ULAB_FILTER_HAS_SOSFILT
    { MP_OBJ_NEW_QSTR(MP_QSTR_sosfilt), (mp_obj_t)&filter_sosfilt_obj },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_filter_globals, ulab_filter_globals_table);

mp_obj_module_t ulab_filter_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_filter_globals,
};
#endif /* ULAB_NUMPY_COMPATIBILITY */
#endif

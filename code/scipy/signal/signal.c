
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020-2021 Zoltán Vörös
 *               2020 Taku Fukada
*/

#include <math.h>
#include <string.h>
#include "py/runtime.h"

#include "../../ulab.h"
#include "../../ndarray.h"
#include "../../numpy/fft/fft_tools.h"

#if ULAB_SCIPY_SIGNAL_HAS_SPECTROGRAM
//| def spectrogram(r: ulab.ndarray) -> ulab.ndarray:
//|     """
//|     :param ulab.ndarray r: A 1-dimension array of values whose size is a power of 2
//|
//|     Computes the spectrum of the input signal.  This is the absolute value of the (complex-valued) fft of the signal.
//|     This function is similar to scipy's ``scipy.signal.spectrogram``."""
//|     ...
//|

mp_obj_t signal_spectrogram(size_t n_args, const mp_obj_t *args) {
    if(n_args == 2) {
        return fft_fft_ifft_spectrogram(n_args, args[0], args[1], FFT_SPECTROGRAM);
    } else {
        return fft_fft_ifft_spectrogram(n_args, args[0], mp_const_none, FFT_SPECTROGRAM);
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(signal_spectrogram_obj, 1, 2, signal_spectrogram);
#endif /* ULAB_SCIPY_SIGNAL_HAS_SPECTROGRAM */

#if ULAB_SCIPY_SIGNAL_HAS_SOSFILT
static void signal_sosfilt_array(mp_float_t *x, const mp_float_t *coeffs, mp_float_t *zf, const size_t len) {
    for(size_t i=0; i < len; i++) {
        mp_float_t xn = *x;
        *x = coeffs[0] * xn + zf[0];
        zf[0] = zf[1] + coeffs[1] * xn - coeffs[4] * *x;
        zf[1] = coeffs[2] * xn - coeffs[5] * *x;
        x++;
    }
    x -= len;
}

mp_obj_t signal_sosfilt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

    size_t *shape = ndarray_shape_vector(0, 0, lensos, 2);
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
            signal_sosfilt_array(yarray, coeffs, zf_array, lenx);
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

MP_DEFINE_CONST_FUN_OBJ_KW(signal_sosfilt_obj, 2, signal_sosfilt);
#endif /* ULAB_SCIPY_SIGNAL_HAS_SOSFILT */

static const mp_rom_map_elem_t ulab_scipy_signal_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_signal) },
    #if ULAB_SCIPY_SIGNAL_HAS_SPECTROGRAM
        { MP_OBJ_NEW_QSTR(MP_QSTR_spectrogram), (mp_obj_t)&signal_spectrogram_obj },
    #endif
    #if ULAB_SCIPY_SIGNAL_HAS_SOSFILT
        { MP_OBJ_NEW_QSTR(MP_QSTR_sosfilt), (mp_obj_t)&signal_sosfilt_obj },
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_scipy_signal_globals, ulab_scipy_signal_globals_table);

mp_obj_module_t ulab_scipy_signal_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_scipy_signal_globals,
};

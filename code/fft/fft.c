
/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
 *               2020 Scott Shawcroft for Adafruit Industries
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/binary.h"
#include "py/obj.h"
#include "py/objarray.h"
#include "fft.h"

#if ULAB_FFT_MODULE

//| """Frequency-domain functions"""
//|

static void fft_kernel(mp_float_t *real, mp_float_t *imag, int n, int isign) {
    // This is basically a modification of four1 from Numerical Recipes
    // The main difference is that this function takes two arrays, one 
    // for the real, and one for the imaginary parts. 
    int j, m, mmax, istep;
    mp_float_t tempr, tempi;
    mp_float_t wtemp, wr, wpr, wpi, wi, theta;

    j = 0;
    for(int i = 0; i < n; i++) {
        if (j > i) {
            SWAP(mp_float_t, real[i], real[j]);
            SWAP(mp_float_t, imag[i], imag[j]);
        }
        m = n >> 1;
        while (j >= m && m > 0) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }

    mmax = 1;
    while (n > mmax) {
        istep = mmax << 1;
        theta = -2.0*isign*MP_PI/istep;
        wtemp = MICROPY_FLOAT_C_FUN(sin)(0.5 * theta);
        wpr = -2.0 * wtemp * wtemp;
        wpi = MICROPY_FLOAT_C_FUN(sin)(theta);
        wr = 1.0;
        wi = 0.0;
        for(m = 0; m < mmax; m++) {
            for(int i = m; i < n; i += istep) {
                j = i + mmax;
                tempr = wr * real[j] - wi * imag[j];
                tempi = wr * imag[j] + wi * real[j];
                real[j] = real[i] - tempr;
                imag[j] = imag[i] - tempi;
                real[i] += tempr;
                imag[i] += tempi;
            }
            wtemp = wr;
            wr = wr*wpr - wi*wpi + wr;
            wi = wi*wpr + wtemp*wpi + wi;
        }
        mmax = istep;
    }
}

mp_obj_t fft_fft_ifft_spectrum(size_t n_args, mp_obj_t arg_re, mp_obj_t arg_im, uint8_t type) {
    if(!MP_OBJ_IS_TYPE(arg_re, &ulab_ndarray_type)) {
        mp_raise_NotImplementedError(translate("FFT is defined for ndarrays only"));
    } 
    if(n_args == 2) {
        if(!MP_OBJ_IS_TYPE(arg_im, &ulab_ndarray_type)) {
            mp_raise_NotImplementedError(translate("FFT is defined for ndarrays only"));
        }
    }
    // Check if input is of length of power of 2
    ndarray_obj_t *re = MP_OBJ_TO_PTR(arg_re);
    uint16_t len = re->array->len;
    if((len & (len-1)) != 0) {
        mp_raise_ValueError(translate("input array length must be power of 2"));
    }
    
    ndarray_obj_t *out_re = create_new_ndarray(1, len, NDARRAY_FLOAT);
    mp_float_t *data_re = (mp_float_t *)out_re->array->items;
    
    if(re->array->typecode == NDARRAY_FLOAT) { 
        // By treating this case separately, we can save a bit of time.
        // I don't know if it is worthwhile, though...
        memcpy((mp_float_t *)out_re->array->items, (mp_float_t *)re->array->items, re->bytes);
    } else {
        for(size_t i=0; i < len; i++) {
            *data_re++ = ndarray_get_float_value(re->array->items, re->array->typecode, i);
        }
        data_re -= len;
    }
    ndarray_obj_t *out_im = create_new_ndarray(1, len, NDARRAY_FLOAT);
    mp_float_t *data_im = (mp_float_t *)out_im->array->items;

    if(n_args == 2) {
        ndarray_obj_t *im = MP_OBJ_TO_PTR(arg_im);
        if (re->array->len != im->array->len) {
            mp_raise_ValueError(translate("real and imaginary parts must be of equal length"));
        }
        if(im->array->typecode == NDARRAY_FLOAT) {
            memcpy((mp_float_t *)out_im->array->items, (mp_float_t *)im->array->items, im->bytes);
        } else {
            for(size_t i=0; i < len; i++) {
               *data_im++ = ndarray_get_float_value(im->array->items, im->array->typecode, i);
            }
            data_im -= len;
        }
    }

    if((type == FFT_FFT) || (type == FFT_SPECTROGRAM)) {
        fft_kernel(data_re, data_im, len, 1);
        if(type == FFT_SPECTROGRAM) {
            for(size_t i=0; i < len; i++) {
                *data_re = MICROPY_FLOAT_C_FUN(sqrt)(*data_re * *data_re + *data_im * *data_im);
                data_re++;
                data_im++;
            }
        }
    } else { // inverse transform
        fft_kernel(data_re, data_im, len, -1);
        // TODO: numpy accepts the norm keyword argument
        for(size_t i=0; i < len; i++) {
            *data_re++ /= len;
            *data_im++ /= len;
        }
    }
    if(type == FFT_SPECTROGRAM) {
        return MP_OBJ_TO_PTR(out_re);
    } else {
        mp_obj_t tuple[2];
        tuple[0] = out_re;
        tuple[1] = out_im;
        return mp_obj_new_tuple(2, tuple);
    }
}

//| def fft(r, c=None):
//|     """
//|     :param ulab.array r: A 1-dimension array of values whose size is a power of 2
//|     :param ulab.array c: An optional 1-dimension array of values whose size is a power of 2, giving the complex part of the value
//|     :return tuple (r, c): The real and complex parts of the FFT
//|
//|     Perform a Fast Fourier Transform from the time domain into the frequency domain
//|     See also ~ulab.extras.spectrum, which computes the magnitude of the fft,
//|     rather than separately returning its real and imaginary parts."""
//|     ...
//|

static mp_obj_t fft_fft(size_t n_args, const mp_obj_t *args) {
    if(n_args == 2) {
        return fft_fft_ifft_spectrum(n_args, args[0], args[1], FFT_FFT);
    } else {
        return fft_fft_ifft_spectrum(n_args, args[0], mp_const_none, FFT_FFT);        
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fft_fft_obj, 1, 2, fft_fft);

//| def ifft(r, c=None):
//|     """
//|     :param ulab.array r: A 1-dimension array of values whose size is a power of 2
//|     :param ulab.array c: An optional 1-dimension array of values whose size is a power of 2, giving the complex part of the value
//|     :return tuple (r, c): The real and complex parts of the inverse FFT
//|
//|     Perform an Inverse Fast Fourier Transform from the frequeny domain into the time domain"""
//|     ...
//| 

static mp_obj_t fft_ifft(size_t n_args, const mp_obj_t *args) {
    if(n_args == 2) {
        return fft_fft_ifft_spectrum(n_args, args[0], args[1], FFT_IFFT);
    } else {
        return fft_fft_ifft_spectrum(n_args, args[0], mp_const_none, FFT_IFFT);
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fft_ifft_obj, 1, 2, fft_ifft);

//| def spectrogram(r):
//|     """
//|     :param ulab.array r: A 1-dimension array of values whose size is a power of 2
//|
//|     Computes the spectrum of the input signal.  This is the absolute value of the (complex-valued) fft of the signal.
//|     This function is similar to scipy's ``scipy.signal.spectrogram``."""
//|     ...
//|

static mp_obj_t fft_spectrogram(size_t n_args, const mp_obj_t *args) {
    if(n_args == 2) {
        return fft_fft_ifft_spectrum(n_args, args[0], args[1], FFT_SPECTROGRAM);
    } else {
        return fft_fft_ifft_spectrum(n_args, args[0], mp_const_none, FFT_SPECTROGRAM);
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fft_spectrogram_obj, 1, 2, fft_spectrogram);

STATIC const mp_rom_map_elem_t ulab_fft_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_fft) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fft), (mp_obj_t)&fft_fft_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ifft), (mp_obj_t)&fft_ifft_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_spectrogram), (mp_obj_t)&fft_spectrogram_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_fft_globals, ulab_fft_globals_table);

mp_obj_module_t ulab_fft_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_fft_globals,
};

#endif

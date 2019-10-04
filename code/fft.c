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
#include <string.h>
#include "py/runtime.h"
#include "py/binary.h"
#include "py/obj.h"
#include "py/objarray.h"
#include "ndarray.h"
#include "fft.h"

void fft_kernel(float *real, float *imag, int n, int isign) {
    // This is basically a modification of four1 from Numerical Recipes
    // The main difference is that this function takes two arrays, one 
    // for the real, and one for the imaginary parts. 
    int j, m, mmax, istep;
    float tempr, tempi;
    float wtemp, wr, wpr, wpi, wi, theta;

    j = 0;
    for(int i = 0; i < n; i++) {
        if (j > i) {
            SWAP(float, real[i], real[j]);
            SWAP(float, imag[i], imag[j]);            
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
        theta = -1.0*isign*6.28318530717959/istep;
        wtemp = sinf(0.5 * theta);
        wpr = -2.0 * wtemp * wtemp;
        wpi = sinf(theta);
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

mp_obj_t fft_fft(size_t n_args, const mp_obj_t *args) {
    // TODO: return the absolute value, if keyword argument is specified
    // TODO: transform the data in place, if keyword argument is specified
    if(!MP_OBJ_IS_TYPE(args[0], &ulab_ndarray_type)) {
        mp_raise_NotImplementedError("FFT is defined for ndarrays only");
    } 
    if(n_args == 2) {
        if(!MP_OBJ_IS_TYPE(args[1], &ulab_ndarray_type)) {
            mp_raise_NotImplementedError("FFT is defined for ndarrays only");
        }
    }
    // Check if input is of length of power of 2
    ndarray_obj_t *re = MP_OBJ_TO_PTR(args[0]);
    uint16_t len = re->array->len;
    if((len & (len-1)) != 0) {
        mp_raise_ValueError("input array length must be power of 2");
    }
    
    ndarray_obj_t *out_re = create_new_ndarray(1, len, NDARRAY_FLOAT);
    float *data_re = (float *)out_re->array->items;
    
    if(re->array->typecode == NDARRAY_FLOAT) {
        memcpy((float *)out_re->array->items, (float *)re->array->items, re->bytes);
    } else {
        for(size_t i=0; i < len; i++) {
            data_re[i] = ndarray_get_float_value(re->array->items, re->array->typecode, i);
        }
    }
    ndarray_obj_t *out_im = create_new_ndarray(1, len, NDARRAY_FLOAT);
    float *data_im = (float *)out_im->array->items;

    if(n_args == 2) {
        ndarray_obj_t *im = MP_OBJ_TO_PTR(args[1]);
        if (re->array->len != im->array->len) {
            mp_raise_ValueError("real and imaginary parts must be of equal length");
        }
        if(im->array->typecode == NDARRAY_FLOAT) {
            memcpy((float *)out_im->array->items, (float *)im->array->items, im->bytes);
        } else {
            for(size_t i=0; i < len; i++) {
                data_im[i] = ndarray_get_float_value(im->array->items, im->array->typecode, i);
            }
        }
    }    
    fft_kernel(data_re, data_im, len, 1);
    mp_obj_t tuple[2];
    tuple[0] = out_re;
    tuple[1] = out_im;
    return mp_obj_new_tuple(2, tuple);
}

mp_obj_t fft_spectrum(mp_obj_t oin) {
    // calculates the the spectrum of a single real ndarray in place
    if(!MP_OBJ_IS_TYPE(oin, &ulab_ndarray_type)) {
        mp_raise_NotImplementedError("FFT is defined for ndarrays only");
    }
    ndarray_obj_t *re = MP_OBJ_TO_PTR(oin);
    uint16_t len = re->array->len;
    if((re->m > 1) && (re->n > 1)) {
        mp_raise_ValueError("input data must be an array");
    }
    if((len & (len-1)) != 0) {
        mp_raise_ValueError("input array length must be power of 2");
    }
    if(re->array->typecode != NDARRAY_FLOAT) {
        mp_raise_TypeError("input array must be of type float");
    }
    float *data_re = (float *)re->array->items;
    float *data_im = m_new(float, len);
    fft_kernel(data_re, data_im, len, 1);
    for(size_t i=0; i < len; i++) {
        data_re[i] = sqrtf(data_re[i]*data_re[i] + data_im[i]*data_im[i]);
    }
    m_del(float, data_im, len);
    return mp_const_none;
}

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
#include <stdlib.h>
#include "py/runtime.h"
#include "ndarray.h"
#include "fft.h"
    
STATIC void fft_kernel(complex_t *buffer, complex_t *out, int n, int step) {
    // this is the actual FFT kernel that we call recursively
    if (step < n) {
        // split the input buffer in two
        fft_kernel(out, buffer, n, step*2);
        fft_kernel(out+step, buffer+step, n, step*2);

        for (int i=0; i<n; i+=2*step) {
            complex_t twiddle = (cosf(MP_PI*i/n)-I*sinf(MP_PI*i/n)) * out[i+step];
            buffer[i/2] = out[i] + twiddle * out[i+step];
            buffer[(i+n)/2] = out[i] - twiddle * out[i+step];
        }
    }
}

mp_obj_t fft_fft(mp_obj_t real, mp_obj_t imag) {
    // TODO: return the absolute value, if keyword argument is specified
    // TODO: let go of the complex type, for the assignment of values is through reals and imags, anyway
    // Check if input is of length of power of 2
    mp_obj_array_t *re = MP_OBJ_TO_PTR(real);
    if ((re->len & (re->len-1)) != 0) {
        mp_raise_ValueError("input array length must be power of 2");
    }
    mp_obj_array_t *im = MP_OBJ_TO_PTR(imag);
    if (re->len != im->len) {
        mp_raise_ValueError("real and imaginary parts must be of equal length");
    }

    complex_t *buffer, *out;
    buffer = (complex_t *)malloc(sizeof(complex_t)*re->len);
    out = (complex_t *)malloc(sizeof(complex_t)*re->len);
    size_t i=0;

    if(MP_OBJ_IS_TYPE(re, &ulab_ndarray_type) && MP_OBJ_IS_TYPE(im, &ulab_ndarray_type)) {
        ndarray_obj_t *_re = MP_OBJ_TO_PTR(real);
        ndarray_obj_t *_im = MP_OBJ_TO_PTR(imag);
        for(size_t j=0; j < re->len; j++) {
            buffer[i] = out[i] = ndarray_get_float_value(_re->data->items, _re->data->typecode, j) + 
            I*ndarray_get_float_value(_im->data->items, _im->data->typecode, j);
        }
    } else {
        mp_obj_iter_buf_t re_buf;
        mp_obj_t re_item, re_iterable = mp_getiter(real, &re_buf);

        mp_obj_iter_buf_t im_buf;
        mp_obj_t im_item, im_iterable = mp_getiter(imag, &im_buf);

        while ((re_item = mp_iternext(re_iterable)) != MP_OBJ_STOP_ITERATION) {
            im_item = mp_iternext(im_iterable);
            buffer[i] = out[i] = mp_obj_get_float(re_item) + I*mp_obj_get_float(im_item);
            i++;
        }
    }
    fft_kernel(buffer, out, re->len, 1);

    complex_t z;
    if(1) {
        ndarray_obj_t *re_out = create_new_ndarray(1, re->len, NDARRAY_FLOAT);
        ndarray_obj_t *im_out = create_new_ndarray(1, im->len, NDARRAY_FLOAT);
        float *_re_out = (float *)re_out->data->items;
        float *_im_out = (float *)im_out->data->items;        
        for(i=0; i<re->len; i++) {
            z = buffer[i];
            _re_out[i] = creal(z);
            _im_out[i] = cimag(z);
        }
        free(buffer);
        mp_obj_t tuple[2];
        tuple[0] = re_out;
        tuple[1] = im_out;
        return mp_obj_new_tuple(2, tuple);
    } 
}

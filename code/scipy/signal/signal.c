
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
#include "../../numpy/carray/carray_tools.h"
#include "../../numpy/fft/fft_tools.h" // For fft
#include "../../ulab_tools.h"

#if ULAB_SCIPY_SIGNAL_HAS_SOSFILT & ULAB_MAX_DIMS > 1
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
        { MP_QSTR_sos, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_zi, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!ndarray_object_is_array_like(args[0].u_obj) || !ndarray_object_is_array_like(args[1].u_obj)) {
        mp_raise_TypeError(MP_ERROR_TEXT("sosfilt requires iterable arguments"));
    }
    #if ULAB_SUPPORTS_COMPLEX
    if(mp_obj_is_type(args[1].u_obj, &ulab_ndarray_type)) {
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[1].u_obj);
        COMPLEX_DTYPE_NOT_IMPLEMENTED(ndarray->dtype)
    }
    #endif
    size_t lenx = (size_t)mp_obj_get_int(mp_obj_len_maybe(args[1].u_obj));
    ndarray_obj_t *y = ndarray_new_linear_array(lenx, NDARRAY_FLOAT);
    mp_float_t *yarray = (mp_float_t *)y->array;
    mp_float_t coeffs[6];
    if(mp_obj_is_type(args[1].u_obj, &ulab_ndarray_type)) {
        ndarray_obj_t *inarray = MP_OBJ_TO_PTR(args[1].u_obj);
        #if ULAB_MAX_DIMS > 1
        if(inarray->ndim > 1) {
            mp_raise_ValueError(MP_ERROR_TEXT("input must be one-dimensional"));
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
        if(!mp_obj_is_type(args[2].u_obj, &ulab_ndarray_type)) {
            mp_raise_TypeError(MP_ERROR_TEXT("zi must be an ndarray"));
        } else {
            ndarray_obj_t *zi = MP_OBJ_TO_PTR(args[2].u_obj);
            if((zi->shape[ULAB_MAX_DIMS - 2] != lensos) || (zi->shape[ULAB_MAX_DIMS - 1] != 2)) {
                mp_raise_ValueError(MP_ERROR_TEXT("zi must be of shape (n_section, 2)"));
            }
            if(zi->dtype != NDARRAY_FLOAT) {
                mp_raise_ValueError(MP_ERROR_TEXT("zi must be of float type"));
            }
            // TODO: this won't work with sparse arrays
            memcpy(zf_array, zi->array, 2*lensos*sizeof(mp_float_t));
        }
    }
    while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        if(mp_obj_get_int(mp_obj_len_maybe(item)) != 6) {
            mp_raise_ValueError(MP_ERROR_TEXT("sos array must be of shape (n_section, 6)"));
        } else {
            fill_array_iterable(coeffs, item);
            if(coeffs[3] != MICROPY_FLOAT_CONST(1.0)) {
                mp_raise_ValueError(MP_ERROR_TEXT("sos[:, 3] should be all ones"));
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
        return MP_OBJ_FROM_PTR(tuple);
    }
}

MP_DEFINE_CONST_FUN_OBJ_KW(signal_sosfilt_obj, 2, signal_sosfilt);
#endif /* ULAB_SCIPY_SIGNAL_HAS_SOSFILT */

#if ULAB_SCIPY_SIGNAL_HAS_OACONVOLVE

mp_obj_t signal_oaconvolve(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_a, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        {MP_QSTR_v, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type) || !mp_obj_is_type(args[1].u_obj, &ulab_ndarray_type))
    {
        mp_raise_TypeError(MP_ERROR_TEXT("oa convolve arguments must be ndarrays"));
    }

    ndarray_obj_t *a = MP_OBJ_TO_PTR(args[0].u_obj);
    ndarray_obj_t *c = MP_OBJ_TO_PTR(args[1].u_obj);
// deal with linear arrays only
#if ULAB_MAX_DIMS > 1
    if ((a->ndim != 1) || (c->ndim != 1))
    {
        mp_raise_TypeError(MP_ERROR_TEXT("convolve arguments must be linear arrays"));
    }
#endif
    size_t signal_len = a->len;
    size_t kernel_len = c->len;
    if (signal_len == 0 || kernel_len == 0)
    {
        mp_raise_TypeError(MP_ERROR_TEXT("convolve arguments must not be empty"));
    }

    

    size_t segment_len = kernel_len; // Min segment size, at least size of kernel
    size_t fft_size = 1;
    while (fft_size < segment_len + kernel_len - 1)
    {
        fft_size *= 2; // Find smallest power of 2 for fft size
    }
    segment_len = fft_size - kernel_len + 1; // Final segment size
    size_t output_len = signal_len + kernel_len - 1;
    uint8_t dtype = NDARRAY_FLOAT;
#if ULAB_SUPPORTS_COMPLEX
    if ((a->dtype == NDARRAY_COMPLEX) || (c->dtype == NDARRAY_COMPLEX))
    {
        dtype = NDARRAY_COMPLEX;
    }
#endif

    uint8_t sz = 2 * sizeof(mp_float_t);

    // Buffer allocation
    mp_float_t *kernel_fft_array = m_new0(mp_float_t, fft_size * 2);
    mp_float_t *segment_fft_array = m_new0(mp_float_t, fft_size * 2);
    ndarray_obj_t *output = ndarray_new_linear_array(output_len, dtype);
    mp_float_t *output_array = (mp_float_t *)output->array;
    uint8_t *kernel_array = (uint8_t *)c->array;
    uint8_t *signal_array = (uint8_t *)a->array;

    // Copy kernel data
    if (c->dtype == NDARRAY_COMPLEX)
    {
        for (size_t i = 0; i < kernel_len; i++)
        {
            memcpy(kernel_fft_array, kernel_array, sz);
            kernel_fft_array += 2;
            kernel_array += c->strides[ULAB_MAX_DIMS - 1];
        }
    }
    else
    {
        mp_float_t (*func)(void *) = ndarray_get_float_function(c->dtype);
        for (size_t i = 0; i < kernel_len; i++)
        {
            // real part; the imaginary part is 0, no need to assign
            *kernel_fft_array = func(kernel_array);
            kernel_fft_array += 2;
            kernel_array += c->strides[ULAB_MAX_DIMS - 1];
        }
    }
    kernel_fft_array -= 2 * kernel_len;

    // Compute kernel fft in place
    fft_kernel(kernel_fft_array, fft_size, 1);

    mp_float_t real, imag; // For complex multiplication
    size_t current_segment_size = segment_len;
    mp_float_t (*funca)(void *) = ndarray_get_float_function(a->dtype);

    for (size_t i = 0; i < signal_len; i += segment_len)
    {
        // Check if remaining data is less than segment length
        if (i + segment_len > signal_len)
        {
            current_segment_size = signal_len - i;
        }
        // Load segment in buffer
        memset(segment_fft_array, 0, fft_size * sz); // Reset buffer to zero
        if (a->dtype == NDARRAY_COMPLEX)
        {
            for (size_t k = 0; k < current_segment_size; k++)
            {
                memcpy(segment_fft_array, signal_array, sz);
                segment_fft_array += 2;
                signal_array += a->strides[ULAB_MAX_DIMS - 1];
            }
        }
        else
        {
            for (size_t k = 0; k < current_segment_size; k++)
            {
                // real part; the imaginary part is 0, no need to assign
                *segment_fft_array = funca(signal_array);
                segment_fft_array += 2;
                signal_array += a->strides[ULAB_MAX_DIMS - 1];
            }
        }
        segment_fft_array -= 2 * current_segment_size;

        // Compute segment fft in place
        fft_kernel(segment_fft_array, fft_size, 1);

        // Product of kernel and segment fft (complex multiplication)
        for (size_t j = 0; j < fft_size; j++)
        {
            real = segment_fft_array[j * 2] * kernel_fft_array[j * 2] - segment_fft_array[j * 2 + 1] * kernel_fft_array[j * 2 + 1];
            imag = segment_fft_array[j * 2] * kernel_fft_array[j * 2 + 1] + segment_fft_array[j * 2 + 1] * kernel_fft_array[j * 2];
            segment_fft_array[j * 2] = real;
            segment_fft_array[j * 2 + 1] = imag;
        }
        // Inverse FFT in place
        fft_kernel(segment_fft_array, fft_size, -1);

        #if ULAB_SUPPORTS_COMPLEX
        if (dtype == NDARRAY_COMPLEX)
        {
            // Overlap, Add and normalized inverse fft
            for (size_t j = 0; j < fft_size * 2; j++)
            {
                if ((i * 2 + j) < (output_len * 2))
                {
                    output_array[i * 2 + j] += (segment_fft_array[j] / fft_size);
                }
            }
            continue;
        }
        #endif
        
        
        for (size_t j = 0; j < fft_size; j++)
        {
            if ((i + j) < (output_len)) // adds only real part
            {
                output_array[i + j] += (segment_fft_array[j * 2] / fft_size);
            }
        }
        
    }
    return MP_OBJ_FROM_PTR(output);
}
MP_DEFINE_CONST_FUN_OBJ_KW(signal_oaconvolve_obj, 2, signal_oaconvolve);
#endif /* ULAB_SCIPY_SIGNAL_HAS_OACONVOLVE */


static const mp_rom_map_elem_t ulab_scipy_signal_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_signal) },
    #if ULAB_SCIPY_SIGNAL_HAS_SOSFILT & ULAB_MAX_DIMS > 1
        { MP_ROM_QSTR(MP_QSTR_sosfilt), MP_ROM_PTR(&signal_sosfilt_obj) },
    #endif
    #if ULAB_SCIPY_SIGNAL_HAS_OACONVOLVE
        { MP_ROM_QSTR(MP_QSTR_oaconvolve), MP_ROM_PTR(&signal_oaconvolve_obj) },
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_scipy_signal_globals, ulab_scipy_signal_globals_table);

const mp_obj_module_t ulab_scipy_signal_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_scipy_signal_globals,
};
#if CIRCUITPY_ULAB
MP_REGISTER_MODULE(MP_QSTR_ulab_dot_scipy_dot_signal, ulab_scipy_signal_module);
#endif

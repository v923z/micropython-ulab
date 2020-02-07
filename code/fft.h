
/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#ifndef _FFT_
#define _FFT_
#include "ulab.h"

#ifndef MP_PI
#define MP_PI MICROPY_FLOAT_CONST(3.14159265358979323846)
#endif

#define SWAP(t, a, b) { t tmp = a; a = b; b = tmp; }

#if ULAB_FFT_FFT
mp_obj_t fft_fft(size_t , const mp_obj_t *);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(fft_fft_obj);
#endif

#if ULAB_FFT_IFFT
mp_obj_t fft_ifft(size_t , const mp_obj_t *);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(fft_ifft_obj);
#endif

#if ULAB_FFT_SPECTRUM
mp_obj_t fft_spectrum(size_t , const mp_obj_t *);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(fft_spectrum_obj);
#endif

#endif

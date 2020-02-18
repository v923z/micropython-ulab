
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

#if ULAB_FFT_MODULE

mp_obj_module_t ulab_fft_module;

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(fft_fft_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(fft_ifft_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(fft_spectrum_obj);

#endif
#endif

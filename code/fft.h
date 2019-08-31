/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Zoltán Vörös
*/
    
#ifndef _FFT_
#define _FFT_

#ifndef MP_PI
#define MP_PI MICROPY_FLOAT_CONST(3.14159265358979323846)
#endif

#include <complex.h>
typedef float complex complex_t;

mp_obj_t fft_fft(mp_obj_t, mp_obj_t);
#endif

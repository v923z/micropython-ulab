
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

#include "../ulab.h"
#include "../ulab_tools.h"
#include "../ndarray.h"
#include "fft_tools.h"

extern mp_obj_module_t ulab_fft_module;

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(fft_fft_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(fft_ifft_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(fft_spectrogram_obj);

mp_obj_t fft_fft_ifft_spectrogram(size_t , mp_obj_t , mp_obj_t , uint8_t );

#endif

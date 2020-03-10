
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Zoltán Vörös
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "extras.h"

#if ULAB_EXTRAS_MODULE

mp_obj_t extras_spectrogram(size_t n_args, const mp_obj_t *args) {
    if(n_args == 2) {
        return fft_fft_ifft_spectrum(n_args, args[0], args[1], FFT_SPECTRUM);
    } else {
        return fft_fft_ifft_spectrum(n_args, args[0], mp_const_none, FFT_SPECTRUM);
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(extras_spectrogram_obj, 1, 2, extras_spectrogram);

STATIC const mp_rom_map_elem_t ulab_extras_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_extras) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_spectrogram), (mp_obj_t)&extras_spectrogram_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_extras_globals, ulab_extras_globals_table);

mp_obj_module_t ulab_extras_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_extras_globals,
};

#endif


/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020 Zoltán Vörös
 *               2020 Taku Fukada
*/

#include <math.h>
#include "py/runtime.h"

#include "scipy_defs.h"
#include "../fft/fft.h"
#include "../filter/filter.h"

static const mp_rom_map_elem_t ulab_scipy_signal_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_signal) },
	#if ULAB_SCIPY_SIGNAL_HAS_SPECTROGRAM
        { MP_OBJ_NEW_QSTR(MP_QSTR_spectrogram), (mp_obj_t)&fft_spectrogram_obj },
	#endif
    #if ULAB_SCIPY_SIGNAL_HAS_SOSFILT
		{ MP_OBJ_NEW_QSTR(MP_QSTR_sosfilt), (mp_obj_t)&filter_sosfilt_obj },
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_scipy_signal_globals, ulab_scipy_signal_globals_table);

mp_obj_module_t ulab_scipy_signal_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_scipy_signal_globals,
};

/*
        #else
            #if ULAB_APPROX_HAS_BISECT
                { MP_OBJ_NEW_QSTR(MP_QSTR_bisect), (mp_obj_t)&approx_bisect_obj },
            #endif
            #if ULAB_APPROX_HAS_FMIN
                { MP_OBJ_NEW_QSTR(MP_QSTR_fmin), (mp_obj_t)&approx_fmin_obj },
            #endif
            #if ULAB_APPROX_HAS_CURVE_FIT
                { MP_OBJ_NEW_QSTR(MP_QSTR_curve_fit), (mp_obj_t)&approx_curve_fit_obj },
            #endif
            #if ULAB_APPROX_HAS_INTERP
                { MP_OBJ_NEW_QSTR(MP_QSTR_interp), (mp_obj_t)&approx_interp_obj },
            #endif
            #if ULAB_APPROX_HAS_NEWTON
                { MP_OBJ_NEW_QSTR(MP_QSTR_newton), (mp_obj_t)&approx_newton_obj },
            #endif
            #if ULAB_APPROX_HAS_TRAPZ
                { MP_OBJ_NEW_QSTR(MP_QSTR_trapz), (mp_obj_t)&approx_trapz_obj },
            #endif
           
            #if ULAB_FILTER_HAS_CONVOLVE
                { MP_OBJ_NEW_QSTR(MP_QSTR_convolve), (mp_obj_t)&filter_convolve_obj },
            #endif
            #if ULAB_FILTER_HAS_SOSFILT
                { MP_OBJ_NEW_QSTR(MP_QSTR_sosfilt), (mp_obj_t)&filter_sosfilt_obj },
            #endif
        #endif  
*/

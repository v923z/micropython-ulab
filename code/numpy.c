
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

#include "numpy.h"
#include "ulab_create.h"
#include "compare/compare.h"
#include "fft/fft.h"
#include "filter/filter.h"
#include "linalg/linalg.h"
#include "vector/vectorise.h"

#if ULAB_NUMPY_COMPATIBILITY

// math constants
#if ULAB_HAS_E
mp_obj_float_t ulab_const_float_e_obj = {{&mp_type_float}, MP_E};
#endif

#if ULAB_HAS_INF
mp_obj_float_t numpy_const_float_inf_obj = {{&mp_type_float}, INFINITY};
#endif

#if ULAB_HAS_NAN
mp_obj_float_t numpy_const_float_nan_obj = {{&mp_type_float}, NAN};
#endif

#if ULAB_HAS_PI
mp_obj_float_t ulab_const_float_pi_obj = {{&mp_type_float}, MP_PI};
#endif

static const mp_rom_map_elem_t ulab_numpy_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_numpy) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_array), (mp_obj_t)&ulab_ndarray_type },
    // math constants
    #if ULAB_HAS_E
        { MP_ROM_QSTR(MP_QSTR_e), MP_ROM_PTR(&ulab_const_float_e_obj) },
    #endif
    #if ULAB_HAS_INF
        { MP_ROM_QSTR(MP_QSTR_inf), MP_ROM_PTR(&numpy_const_float_inf_obj) },
    #endif
    #if ULAB_HAS_NAN
        { MP_ROM_QSTR(MP_QSTR_nan), MP_ROM_PTR(&numpy_const_float_nan_obj) },
    #endif
    #if ULAB_HAS_PI
        { MP_ROM_QSTR(MP_QSTR_pi), MP_ROM_PTR(&ulab_const_float_pi_obj) },
    #endif
	// class constants, always included
    { MP_ROM_QSTR(MP_QSTR_bool), MP_ROM_INT(NDARRAY_BOOL) },
    { MP_ROM_QSTR(MP_QSTR_uint8), MP_ROM_INT(NDARRAY_UINT8) },
    { MP_ROM_QSTR(MP_QSTR_int8), MP_ROM_INT(NDARRAY_INT8) },
    { MP_ROM_QSTR(MP_QSTR_uint16), MP_ROM_INT(NDARRAY_UINT16) },
    { MP_ROM_QSTR(MP_QSTR_int16), MP_ROM_INT(NDARRAY_INT16) },
    { MP_ROM_QSTR(MP_QSTR_float), MP_ROM_INT(NDARRAY_FLOAT) },
    // modules of numpy
	#if ULAB_FFT_MODULE
        { MP_ROM_QSTR(MP_QSTR_fft), MP_ROM_PTR(&ulab_fft_module) },
    #endif
    #if ULAB_HAS_PRINTOPTIONS
        { MP_ROM_QSTR(MP_QSTR_set_printoptions), (mp_obj_t)&ndarray_set_printoptions_obj },
        { MP_ROM_QSTR(MP_QSTR_get_printoptions), (mp_obj_t)&ndarray_get_printoptions_obj },
    #endif
    #if ULAB_HAS_NDINFO
        { MP_ROM_QSTR(MP_QSTR_ndinfo), (mp_obj_t)&ndarray_info_obj },
    #endif
    #if ULAB_CREATE_HAS_ARANGE
        { MP_ROM_QSTR(MP_QSTR_arange), (mp_obj_t)&create_arange_obj },
    #endif
    #if ULAB_CREATE_HAS_CONCATENATE
        { MP_ROM_QSTR(MP_QSTR_concatenate), (mp_obj_t)&create_concatenate_obj },
    #endif
    #if ULAB_CREATE_HAS_DIAG
        { MP_ROM_QSTR(MP_QSTR_diag), (mp_obj_t)&create_diag_obj },
    #endif
    #if ULAB_MAX_DIMS > 1
        #if ULAB_CREATE_HAS_EYE
            { MP_ROM_QSTR(MP_QSTR_eye), (mp_obj_t)&create_eye_obj },
        #endif
    #endif /* ULAB_MAX_DIMS */
    // functions of the create sub-module
    #if ULAB_CREATE_HAS_FULL
        { MP_ROM_QSTR(MP_QSTR_full), (mp_obj_t)&create_full_obj },
    #endif
    #if ULAB_CREATE_HAS_LINSPACE
        { MP_ROM_QSTR(MP_QSTR_linspace), (mp_obj_t)&create_linspace_obj },
    #endif
    #if ULAB_CREATE_HAS_LOGSPACE
        { MP_ROM_QSTR(MP_QSTR_logspace), (mp_obj_t)&create_logspace_obj },
    #endif
    #if ULAB_CREATE_HAS_ONES
        { MP_ROM_QSTR(MP_QSTR_ones), (mp_obj_t)&create_ones_obj },
    #endif
    #if ULAB_CREATE_HAS_ZEROS
        { MP_ROM_QSTR(MP_QSTR_zeros), (mp_obj_t)&create_zeros_obj },
    #endif
    // functions of the compare sub-module
	#if ULAB_COMPARE_HAS_CLIP
		{ MP_OBJ_NEW_QSTR(MP_QSTR_clip), (mp_obj_t)&compare_clip_obj },
	#endif
	#if ULAB_COMPARE_HAS_EQUAL
		{ MP_OBJ_NEW_QSTR(MP_QSTR_equal), (mp_obj_t)&compare_equal_obj },
	#endif
	#if ULAB_COMPARE_HAS_NOTEQUAL
		{ MP_OBJ_NEW_QSTR(MP_QSTR_not_equal), (mp_obj_t)&compare_not_equal_obj },
	#endif
	#if ULAB_COMPARE_HAS_MAXIMUM
		{ MP_OBJ_NEW_QSTR(MP_QSTR_maximum), (mp_obj_t)&compare_maximum_obj },
	#endif
	#if ULAB_COMPARE_HAS_MINIMUM
		{ MP_OBJ_NEW_QSTR(MP_QSTR_minimum), (mp_obj_t)&compare_minimum_obj },
	#endif
    #if ULAB_VECTORISE_HAS_VECTORIZE
		{ MP_OBJ_NEW_QSTR(MP_QSTR_vectorize), (mp_obj_t)&vectorise_vectorize_obj },
	#endif
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_numpy_globals, ulab_numpy_globals_table);

mp_obj_module_t ulab_numpy_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_numpy_globals,
};
#endif /* ULAB_NUMPY_COMPATIBILITY */

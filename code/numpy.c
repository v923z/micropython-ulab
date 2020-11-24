
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
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_numpy_globals, ulab_numpy_globals_table);

mp_obj_module_t ulab_numpy_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_numpy_globals,
};
#endif /* ULAB_NUMPY_COMPATIBILITY */

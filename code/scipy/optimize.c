
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
#include "../approx/approx.h"

static const mp_rom_map_elem_t ulab_scipy_optimize_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_optimize) },
	#if ULAB_SCIPY_OPTIMIZE_HAS_BISECT
		{ MP_OBJ_NEW_QSTR(MP_QSTR_bisect), (mp_obj_t)&approx_bisect_obj },
	#endif
	#if ULAB_SCIPY_OPTIMIZE_HAS_CURVE_FIT
		{ MP_OBJ_NEW_QSTR(MP_QSTR_curve_fit), (mp_obj_t)&approx_curve_fit_obj },
	#endif
	#if ULAB_SCIPY_OPTIMIZE_HAS_FMIN
		{ MP_OBJ_NEW_QSTR(MP_QSTR_fmin), (mp_obj_t)&approx_fmin_obj },
	#endif
	#if ULAB_SCIPY_OPTIMIZE_HAS_NEWTON
		{ MP_OBJ_NEW_QSTR(MP_QSTR_newton), (mp_obj_t)&approx_newton_obj },
	#endif
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_scipy_optimize_globals, ulab_scipy_optimize_globals_table);

mp_obj_module_t ulab_scipy_optimize_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_scipy_optimize_globals,
};

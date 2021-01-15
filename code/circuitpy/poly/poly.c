
/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
 *               2020 Jeff Epler for Adafruit Industries
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020 Taku Fukada
*/

#include "py/obj.h"
#include "py/runtime.h"
#include "py/objarray.h"

#include "../../ulab.h"
#include "../linalg/linalg_tools.h"
#include "../../ulab_tools.h"
#include "poly.h"

//| """Polynomial functions"""
//|
//| from ulab import _ArrayLike
//|

//| @overload
//| def polyfit(y: _ArrayLike, degree: int) -> ulab.array: ...
//| @overload
//| def polyfit(x: _ArrayLike, y: _ArrayLike, degree: int) -> ulab.array:
//|     """Return a polynomial of given degree that approximates the function
//|        f(x)=y.  If x is not supplied, it is the range(len(y))."""
//|     ...
//|

//| def polyval(p: _ArrayLike, x: _ArrayLike) -> ulab.array:
//|     """Evaluate the polynomial p at the points x.  x must be an array."""
//|     ...
//|

STATIC const mp_rom_map_elem_t ulab_poly_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_poly) },
    #if ULAB_NUMPY_HAS_POLYFIT
    { MP_OBJ_NEW_QSTR(MP_QSTR_polyfit), (mp_obj_t)&poly_polyfit_obj },
    #endif
    #if ULAB_NUMPY_HAS_POLYVAL
    { MP_OBJ_NEW_QSTR(MP_QSTR_polyval), (mp_obj_t)&poly_polyval_obj },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_poly_globals, ulab_poly_globals_table);

mp_obj_module_t ulab_poly_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_poly_globals,
};


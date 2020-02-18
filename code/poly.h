
/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#ifndef _POLY_
#define _POLY_

#include "ulab.h"

#if ULAB_POLY_MODULE

extern const mp_obj_module_t ulab_poly_module;

MP_DECLARE_CONST_FUN_OBJ_2(poly_polyval_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(poly_polyfit_obj);

#endif
#endif

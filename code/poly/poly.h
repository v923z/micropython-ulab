
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

#include "../ulab.h"
#include "../ndarray.h"

#if ULAB_POLY_MODULE

#if !ULAB_NUMPY_COMPATIBILITY
extern mp_obj_module_t ulab_poly_module;
#endif

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(poly_polyfit_obj);
MP_DECLARE_CONST_FUN_OBJ_2(poly_polyval_obj);

#endif /* ULAB_POLY_MODULE */
#endif

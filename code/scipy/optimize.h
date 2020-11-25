
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Zoltán Vörös
 *               
*/

#ifndef _SCIPY_OPTIMIZE_
#define _SCIPY_OPTIMIZE_

#include "../ulab_tools.h"

extern mp_obj_module_t ulab_scipy_optimize_module;

MP_DECLARE_CONST_FUN_OBJ_KW(optimize_bisect_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(optimize_curve_fit_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(optimize_fmin_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(optimize_newton_obj);

#endif /* _SCIPY_OPTIMIZE_ */

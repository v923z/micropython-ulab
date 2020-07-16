
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Zoltán Vörös
*/

#ifndef _APPROX_
#define _APPROX_

#include "ulab.h"
#include "ndarray.h"

#if ULAB_APPROX_MODULE

// this typedef is lifted from objfloat.c, because mp_obj_float_t is not exposed
typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;

#define     APPROX_EPS          1.0e-4
#define     APPROX_NONZDELTA    0.05
#define     APPROX_ZDELTA       0.00025
#define     APPROX_ALPHA        1.0
#define     APPROX_BETA         2.0
#define     APPROX_GAMMA        0.5
#define     APPROX_DELTA        0.5

extern mp_obj_module_t ulab_approx_module;

MP_DECLARE_CONST_FUN_OBJ_KW(approx_bisect_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(approx_newton_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(approx_fmin_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(approx_interp_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(approx_trapz_obj);

#endif
#endif

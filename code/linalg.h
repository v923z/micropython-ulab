
/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#ifndef _LINALG_
#define _LINALG_

#include "ulab.h"
#include "ndarray.h"

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define epsilon        1.2e-7
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define epsilon        2.3e-16
#endif

#define JACOBI_MAX     20

#if ULAB_LINALG_MODULE || ULAB_POLY_MODULE
bool linalg_invert_matrix(mp_float_t *, size_t );
#endif

#if ULAB_LINALG_MODULE

extern mp_obj_module_t ulab_linalg_module;

#endif
#endif

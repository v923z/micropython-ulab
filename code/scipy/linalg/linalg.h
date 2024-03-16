
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Vikas Udupa
 * 
*/

#ifndef _SCIPY_LINALG_
#define _SCIPY_LINALG_


#define SCIPY_LINALG_SVD_MAXITERATIONS                  1UL << 10

#ifndef SCIPY_LINALG_EPSILON
#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define SCIPY_LINALG_EPSILON                            MICROPY_FLOAT_CONST(1.2e-7)
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define SCIPY_LINALG_EPSILON                            MICROPY_FLOAT_CONST(2.3e-16)
#endif
#endif /* SCIPY_LINALG_EPSILON */

extern const mp_obj_module_t ulab_scipy_linalg_module;

MP_DECLARE_CONST_FUN_OBJ_KW(linalg_solve_triangular_obj);
MP_DECLARE_CONST_FUN_OBJ_2(linalg_cho_solve_obj);
MP_DECLARE_CONST_FUN_OBJ_1(linalg_svd_obj);

#endif /* _SCIPY_LINALG_ */

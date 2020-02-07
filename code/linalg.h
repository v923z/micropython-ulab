
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

#define SWAP(t, a, b) { t tmp = a; a = b; b = tmp; }

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define epsilon        1.2e-7
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define epsilon        2.3e-16
#endif

#define JACOBI_MAX     20

// TODO: transpose, reshape and size should probably be part of ndarray.c
#if ULAB_LINALG_TRANSPOSE
mp_obj_t linalg_transpose(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(linalg_transpose_obj);
#endif

#if ULAB_LINALG_RESHAPE
mp_obj_t linalg_reshape(mp_obj_t , mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_2(linalg_reshape_obj);
#endif

#if ULAB_LINALG_SIZE
mp_obj_t linalg_size(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(linalg_size_obj);
#endif

#if ULAB_LINALG_INV || ULAB_POLY_POLYFIT
bool linalg_invert_matrix(mp_float_t *, size_t );
#endif

#if ULAB_LINALG_INV
mp_obj_t linalg_inv(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(linalg_inv_obj);
#endif

#if ULAB_LINALG_DOT
mp_obj_t linalg_dot(mp_obj_t , mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_2(linalg_dot_obj);
#endif

#if ULAB_LINALG_ZEROS
mp_obj_t linalg_zeros(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(linalg_zeros_obj);
#endif

#if ULAB_LINALG_ONES
mp_obj_t linalg_ones(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(linalg_ones_obj);
#endif

#if ULAB_LINALG_EYE
mp_obj_t linalg_eye(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(linalg_eye_obj);
#endif

#if ULAB_LINALG_DET
mp_obj_t linalg_det(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(linalg_det_obj);
#endif

#if ULAB_LINALG_EIG
mp_obj_t linalg_eig(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(linalg_eig_obj);
#endif

#endif

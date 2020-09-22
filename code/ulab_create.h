/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *               2019-2020 Zoltán Vörös
*/

#ifndef _CREATE_
#define _CREATE_

#include "ulab.h"
#include "ndarray.h"

#if ULAB_CREATE_HAS_ARANGE
mp_obj_t create_arange(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_arange_obj);
#endif

#if ULAB_MAX_DIMS > 1
#if ULAB_CREATE_HAS_EYE
mp_obj_t create_eye(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_eye_obj);
#endif
#endif

#if ULAB_CREATE_HAS_LINSPACE
mp_obj_t create_linspace(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_linspace_obj);
#endif

#if ULAB_CREATE_HAS_ONES
mp_obj_t create_ones(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_ones_obj);
#endif

#if ULAB_CREATE_HAS_ZEROS
mp_obj_t create_zeros(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_zeros_obj);
#endif

#endif

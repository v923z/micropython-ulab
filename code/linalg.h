/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Zoltán Vörös
*/
    
#ifndef _LINALG_
#define _LINALG_

#include "ndarray.h"

#define SWAP(t, a, b) { t tmp = a; a = b; b = tmp; }
#define epsilon        1e-6

mp_obj_t linalg_transpose(mp_obj_t );
mp_obj_t linalg_reshape(mp_obj_t , mp_obj_t );
bool linalg_invert_matrix(float *, size_t );
mp_obj_t linalg_inv(mp_obj_t );
mp_obj_t linalg_dot(mp_obj_t , mp_obj_t );

#endif

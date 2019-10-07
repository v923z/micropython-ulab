/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Zoltán Vörös
*/
    
#ifndef _NUMERICAL_
#define _NUMERICAL_

#include "ndarray.h"

mp_obj_t numerical_linspace(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_sum(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_mean(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_std(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_min(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_max(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_argmin(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_argmax(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_roll(size_t , const mp_obj_t *, mp_map_t *);

#endif

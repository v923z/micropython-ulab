
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
*/

#ifndef _FILTER_
#define _FILTER_

#include "ulab.h"
#include "ndarray.h"

#if ULAB_FILTER_CONVOLVE
mp_obj_t filter_convolve(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(filter_convolve_obj);
#endif

#endif

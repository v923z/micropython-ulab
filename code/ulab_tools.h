/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Zoltán Vörös
*/

#ifndef _TOOLS_
#define _TOOLS_

#define SWAP(t, a, b) { t tmp = a; a = b; b = tmp; }

mp_float_t ndarray_get_float_uint8(void *);
mp_float_t ndarray_get_float_int8(void *);
mp_float_t ndarray_get_float_uint16(void *);
mp_float_t ndarray_get_float_int16(void *);
mp_float_t ndarray_get_float_float(void *);
void *ndarray_get_float_function(uint8_t );

uint8_t ndarray_upcast_dtype(uint8_t , uint8_t );
void *ndarray_set_float_function(uint8_t );

#endif

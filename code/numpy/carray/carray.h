
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Zoltán Vörös
*/

#ifndef _CARRAY_
#define _CARRAY_

MP_DECLARE_CONST_FUN_OBJ_1(carray_real_obj);
MP_DECLARE_CONST_FUN_OBJ_1(carray_imag_obj);

mp_obj_t carray_abs(ndarray_obj_t *, ndarray_obj_t *);

#endif

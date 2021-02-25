
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Zoltán Vörös
*/

#ifndef _BLOCKS_
#define _BLOCKS_

#include "ulab.h"
#include "ndarray.h"

#define BLOCK_NO_FLAG               0x00
#define BLOCK_IS_READ_ONLY          0x01
#define BLOCK_IS_READ_WRITE         0x02

typedef struct _blocks_function_obj_t {
    mp_obj_base_t base;
    void *arrfunc;
} blocks_function_obj_t;

extern const mp_obj_type_t blocks_block_type;
extern mp_obj_module_t ulab_blocks_module;

size_t *blocks_coords_from_pointer(void *, ndarray_obj_t *);

#endif

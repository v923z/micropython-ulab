
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Zoltán Vörös
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"

#include "../ulab_tools.h"
#include "blocks.h"

#if ULAB_HAS_BLOCKS

static void blocks_imreader(ndarray_obj_t *ndarray, void *array, int32_t *strides, size_t count) {
    uint16_t *subarray = (uint16_t *)ndarray->dtype.subarray;
    // if necessary, get the coordinates in the original reference frame, i.e.,
    // in the coordinates used at the time of the creation of the object
    // size_t *coords = tools_coords_from_pointer(array, ndarray);
    for(size_t i = 0; i < count; i += *strides/ndarray->itemsize) {
        // fill up the array with dummy data
        *subarray++ = i*i;
    }
    // since strides is going to be used in computation loops, and subarray is
    // meant to be a dense array, simply overwrite strides with the itemsize
    *strides = ndarray->itemsize;
}

ndarray_obj_t *ndarray_ndarray_header(size_t len, void *arrfunc, uint8_t dtype) {
    ndarray_obj_t *ndarray = ndarray_new_linear_array(len, dtype);
    ndarray->dtype.flags = 1;
    ndarray->dtype.arrfunc = arrfunc;
    uint8_t *subarray = m_new(uint8_t, ndarray->itemsize * ndarray->shape[ULAB_MAX_DIMS - 1]);
    ndarray->dtype.subarray = subarray;
    memcpy(&(ndarray->dtype.shape), &(ndarray->shape), sizeof(size_t) *ULAB_MAX_DIMS);
    return ndarray;
}

static mp_obj_t blocks_imread(mp_obj_t shape, mp_obj_t dtype) {

    size_t len = mp_obj_get_int(shape);
    uint8_t _dtype = MP_OBJ_SMALL_INT_VALUE(dtype);

    void (*imreader)(ndarray_obj_t *, void *, int32_t *, size_t) = blocks_imreader;

    ndarray_obj_t *ndarray = ndarray_ndarray_header(len, imreader, _dtype);
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_2(blocks_imread_obj, blocks_imread);

static const mp_rom_map_elem_t ulab_blocks_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_blocks) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_imread), (mp_obj_t)&blocks_imread_obj },
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_blocks_globals, ulab_blocks_globals_table);

mp_obj_module_t ulab_blocks_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_blocks_globals,
};

#endif


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

typedef struct _blocks_function_obj_t {
    void *arrfunc;
} blocks_function_obj_t;

static void blocks_imreader(ndarray_obj_t *ndarray, void *array, int32_t *strides, size_t count) {
    printf("imreader: %p\n", ndarray->dtype.subarray);
    uint8_t *subarray = (uint8_t *)ndarray->dtype.subarray;
    // if necessary, get the coordinates in the original reference frame, i.e.,
    // in the coordinates used at the time of the creation of the object
    // size_t *coords = tools_coords_from_pointer(array, ndarray);
    for(size_t i = 0; i < count; i++) {
        // fill up the array with dummy data
        *subarray++ = (uint8_t)i*i;
         // array += *strides/ndarray->itemsize
    }
    // since strides is going to be used in computation loops, and subarray is
    // meant to be a dense array, simply overwrite strides with the itemsize
    *strides = ndarray->itemsize;
}

static mp_obj_t blocks_imreader_function(void) {
    blocks_function_obj_t *func = m_new(blocks_function_obj_t, 1);
    void (*imreader)(ndarray_obj_t *, void *, int32_t *, size_t) = blocks_imreader;
    func->arrfunc = imreader;
    return MP_OBJ_FROM_PTR(func);
}

MP_DEFINE_CONST_FUN_OBJ_0(blocks_imreader_function_obj, blocks_imreader_function);

ndarray_obj_t *blocks_new_ndarray(mp_obj_t _shape, uint8_t dtype, void *arrfunc) {
    if(!MP_OBJ_IS_TYPE(_shape, &mp_type_tuple)) {
        mp_raise_TypeError(translate("shape must be a tuple"));
    }
    mp_obj_tuple_t *shape_tuple = MP_OBJ_TO_PTR(_shape);
    if(shape_tuple->len > ULAB_MAX_DIMS) {
        mp_raise_ValueError(translate("too many dimensions"));
    }

    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    memset(shape, 0, sizeof(size_t) * ULAB_MAX_DIMS);
    for(uint8_t i=0; i < shape_tuple->len; i++) {
        shape[ULAB_MAX_DIMS - i - 1] = mp_obj_get_int(shape_tuple->items[shape_tuple->len - i - 1]);
    }

    ndarray_obj_t *ndarray = ndarray_new_ndarray_header(shape_tuple->len, shape, NULL, dtype);
    ndarray->dtype.flags = 1;
//    ndarray->dtype.arrfunc = arrfunc;
//    void (*imreader)(ndarray_obj_t *, void *, int32_t *, size_t) = blocks_imreader;
    ndarray->dtype.arrfunc = blocks_imreader;
    // reserve so much space that data for the longest array can still be accommodated
    size_t len = 0;
    for(uint8_t i = 0; i < ULAB_MAX_DIMS; i++) {
        if(ndarray->shape[i] > len) {
            len = ndarray->shape[i];
        }
    }
    uint8_t *subarray = m_new(uint8_t, ndarray->itemsize * len);
    ndarray->dtype.subarray = subarray;
    // store the original array dimensions; dtype.shape is immuateble
    memcpy(&(ndarray->dtype.shape), &(ndarray->shape), sizeof(size_t) * ULAB_MAX_DIMS);
    printf("%p\n", ndarray->dtype.subarray);
    return ndarray;
}

mp_obj_t blocks_block(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_shape, MP_ARG_OBJ | MP_ARG_REQUIRED, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_transformer, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_ROM_INT(NDARRAY_FLOAT) } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_tuple_t *shape_tuple = MP_OBJ_TO_PTR(args[0].u_obj);
    if(shape_tuple->len > ULAB_MAX_DIMS) {
        mp_raise_ValueError(translate("too many dimensions"));
    }
    uint8_t _dtype;
    #if ULAB_HAS_DTYPE_OBJECT
    if(MP_OBJ_IS_TYPE(args[1].u_obj, &ulab_dtype_type)) {
        dtype_obj_t *dtype = MP_OBJ_TO_PTR(args[2].u_obj);
        _dtype = dtype->dtype.type;
    } else {
        _dtype = mp_obj_get_int(args[2].u_obj);
    }
    #else
    _dtype = mp_obj_get_int(args[2].u_obj);
    #endif

    blocks_function_obj_t *transformer = MP_OBJ_TO_PTR(args[1].u_obj);
    (void)transformer;
    void (*arrfunc)(ndarray_obj_t *, void *, int32_t *, size_t) = blocks_imreader;
    ndarray_obj_t *ndarray = blocks_new_ndarray(args[0].u_obj, _dtype, arrfunc);
    ndarray->dtype.arrfunc = arrfunc;
    printf("%p\n", ndarray->dtype.subarray);
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_KW(blocks_block_obj, 0, blocks_block);

static const mp_rom_map_elem_t ulab_blocks_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_blocks) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_block), (mp_obj_t)&blocks_block_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_imreader), (mp_obj_t)&blocks_imreader_function_obj },
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_blocks_globals, ulab_blocks_globals_table);

mp_obj_module_t ulab_blocks_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_blocks_globals,
};

#endif

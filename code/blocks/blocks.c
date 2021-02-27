
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

#include "ndarray.h"
#include "../ulab_tools.h"
#include "blocks.h"

#if ULAB_HAS_BLOCKS

extern const mp_obj_type_t blocks_transformer_type;

const mp_obj_type_t blocks_transformer_type = {
    { &mp_type_type },
    .name = MP_QSTR_transformer,
};

extern const mp_obj_type_t imreader_type;

void imreader_imreader(ndarray_obj_t *ndarray, void *array, int32_t *strides, size_t count) {
    blocks_block_obj_t *block = (blocks_block_obj_t *)ndarray->block;
    uint8_t *barray = (uint8_t *)block->subarray;
    // if necessary, get the coordinates in the original reference frame, i.e.,
    // in the coordinates used at the time of the creation of the object
    // size_t *coords = blocks_coords_from_pointer(array, ndarray);
    for(size_t i = 0; i < count; i++) {
        // fill up the array with dummy data
        *barray++ = (uint8_t)i*i;
    }
    // The subarray is a forward propagating dense array, so set the strides to the itemsize
    *strides = ndarray->itemsize;
}

mp_obj_t imreader_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void) type;
    mp_arg_check_num(n_args, n_kw, 0, 1, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    mp_arg_val_t _args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, _args);

    blocks_transformer_obj_t *transformer = m_new_obj(blocks_transformer_obj_t);
    transformer->base.type = &blocks_transformer_type;
    transformer->arrfunc = imreader_imreader;
    transformer->array = NULL;
    return MP_OBJ_FROM_PTR(transformer);
}

const mp_obj_type_t imreader_type = {
    { &mp_type_type },
    .name = MP_QSTR_imreader,
    .make_new = imreader_make_new,
};

size_t *blocks_coords_from_pointer(void *p1, ndarray_obj_t *ndarray) {
    // calculates the coordinates in the original tensor from the position of the pointer
    // The original view is assumed to be dense, i.e., the strides can be computed from the shape
    blocks_block_obj_t *block = ndarray->block;
    size_t diff = (uint8_t *)p1 - (uint8_t *)block->origin;
    size_t accumulator = 1;
    size_t *coords = m_new(size_t, ULAB_MAX_DIMS);
    for(uint8_t i = 1; i < ndarray->ndim + 1; i++) {
        accumulator *= block->shape[ULAB_MAX_DIMS - i];
        coords[ULAB_MAX_DIMS - i] = diff % accumulator;
    }
    return coords;
}

void blocks_block_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    blocks_block_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ndarray_obj_t *ndarray = (ndarray_obj_t *)self->ndarray;
    mp_print_str(print, "block(shape=(");
    for(uint8_t i = 0; i < ndarray->ndim - 1; i++) {
        mp_printf(print, "%ld, ", ndarray->shape[ULAB_MAX_DIMS - ndarray->ndim + i]);
    }
    mp_printf(print, "%ld), ", ndarray->shape[ULAB_MAX_DIMS - 1]);
    // mp_printf(print, "transformer=%s, ", self->);
    // this is duplicate from ndarray.c:ndarray_print, but allows complete decoupling
    mp_print_str(print, "dtype=");
    ndarray_print_dtype(print, ndarray);
    mp_print_str(print, ")");
}

const mp_obj_type_t blocks_block_type = {
    { &mp_type_type },
    .name = MP_QSTR_block,
    .print = blocks_block_print,
};

mp_obj_t blocks_new_ndarray(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_shape, MP_ARG_OBJ | MP_ARG_REQUIRED, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_transformer, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_ROM_INT(NDARRAY_FLOAT) } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!MP_OBJ_IS_TYPE(args[0].u_obj, &mp_type_tuple)) {
        mp_raise_TypeError(translate("shape must be a tuple"));
    }
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

    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    memset(shape, 0, sizeof(size_t) * ULAB_MAX_DIMS);
    size_t len = 0;
    for(uint8_t i=0; i < shape_tuple->len; i++) {
        shape[ULAB_MAX_DIMS - i - 1] = mp_obj_get_int(shape_tuple->items[shape_tuple->len - i - 1]);
        // reserve as much space that data for the longest array can still be accommodated
        if(shape[ULAB_MAX_DIMS - i - 1] > len) {
            len = shape[ULAB_MAX_DIMS - i - 1];
        }
    }
    ndarray_obj_t *ndarray = ndarray_new_ndarray_header(shape_tuple->len, shape, NULL, _dtype);
    ndarray->flags = BLOCK_IS_READ_ONLY;
    blocks_block_obj_t *block = m_new_obj(blocks_block_obj_t);
    block->base.type = &blocks_block_type;
    // store a pointer to the ndarray
    block->ndarray = ndarray;

    uint8_t *barray = m_new(uint8_t, ndarray->itemsize * len);
    block->subarray = barray;
    // store the original array dimensions; block->shape should never be touched
    memcpy(&(block->shape), &(ndarray->shape), sizeof(size_t) * ULAB_MAX_DIMS);
    // store the original address of the array; block->origin should never be touched
    block->origin = ndarray->array;

    // get the pointer to the reader function
    blocks_transformer_obj_t *transformer = MP_OBJ_TO_PTR(args[1].u_obj);
    block->arrfunc = transformer->arrfunc;
    ndarray->array = transformer->array;
    ndarray->block = block;
    return ndarray;
}

MP_DEFINE_CONST_FUN_OBJ_KW(blocks_new_ndarray_obj, 0, blocks_new_ndarray);

static const mp_rom_map_elem_t ulab_blocks_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_blocks) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ndarray), (mp_obj_t)&blocks_new_ndarray_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_imreader), (mp_obj_t)&imreader_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_block), (mp_obj_t)&blocks_block_type },
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_blocks_globals, ulab_blocks_globals_table);

mp_obj_module_t ulab_blocks_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_blocks_globals,
};

#endif

// typedef struct _imreader_obj_t {
//     mp_obj_base_t base;
//     void *arrfunc;
// } imreader_obj_t;

/*
mp_obj_t blocks_block_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void) type;
    mp_arg_check_num(n_args, n_kw, 0, 1, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    mp_arg_val_t _args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, _args);

    blocks_function_obj_t *function = m_new_obj(blocks_obj_t);
    function->base.type = &blocks_block_type;

    return MP_OBJ_FROM_PTR(function);
}
*/

/*
mp_obj_t blocks_block_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void) type;
    mp_arg_check_num(n_args, n_kw, 0, 1, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    mp_arg_val_t _args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, _args);

    blocks_function_obj_t *function = m_new_obj(blocks_obj_t);
    function->base.type = &blocks_block_type;

    return MP_OBJ_FROM_PTR(function);
}
*/

#if 0
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
#endif

#if 0
mp_obj_t blocks_ndarray(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

    if(!MP_OBJ_IS_TYPE(args[1].u_obj, &blocks_block_function_type)) {
        mp_raise_TypeError(translate("transformer must be a block function object"));
    }
    blocks_block_function_obj_t *transformer = MP_OBJ_TO_PTR(args[1].u_obj);
    (void)transformer;
    void (*arrfunc)(ndarray_obj_t *, void *, int32_t *, size_t) = blocks_imreader;
    ndarray_obj_t *ndarray = blocks_new_ndarray(args[0].u_obj, _dtype, arrfunc);
    ndarray->dtype.arrfunc = arrfunc;
    printf("%p\n", ndarray->dtype.subarray);
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_KW(blocks_ndarray_obj, 0, blocks_ndarray);
#endif

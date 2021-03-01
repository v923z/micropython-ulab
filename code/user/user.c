
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Zoltán Vörös
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"

#include "../ulab_tools.h"
#include "../blocks/blocks.h"
#include "user.h"

#if ULAB_HAS_USER_MODULE

//| """This module should hold arbitrary user-defined functions."""
//|

static mp_obj_t user_square(mp_obj_t arg) {
    // the function takes a single dense ndarray, and calculates the
    // element-wise square of its entries

    // raise a TypeError exception, if the input is not an ndarray
    if(!MP_OBJ_IS_TYPE(arg, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("input must be an ndarray"));
    }
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(arg);

    // make sure that the input is a dense array
    if(!ndarray_is_dense(ndarray)) {
        mp_raise_TypeError(translate("input must be a dense ndarray"));
    }

    // if the input is a dense array, create `results` with the same number of
    // dimensions, shape, and dtype
    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndarray->ndim, ndarray->shape, ndarray->dtype.type);

    // since in a dense array the iteration over the elements is trivial, we
    // can cast the data arrays ndarray->array and results->array to the actual type
    if(ndarray->dtype.type == NDARRAY_UINT8) {
        uint8_t *array = (uint8_t *)ndarray->array;
        uint8_t *rarray = (uint8_t *)results->array;
        for(size_t i=0; i < ndarray->len; i++, array++) {
            *rarray++ = (*array) * (*array);
        }
    } else if(ndarray->dtype.type == NDARRAY_INT8) {
        int8_t *array = (int8_t *)ndarray->array;
        int8_t *rarray = (int8_t *)results->array;
        for(size_t i=0; i < ndarray->len; i++, array++) {
            *rarray++ = (*array) * (*array);
        }
    } else if(ndarray->dtype.type == NDARRAY_UINT16) {
        uint16_t *array = (uint16_t *)ndarray->array;
        uint16_t *rarray = (uint16_t *)results->array;
        for(size_t i=0; i < ndarray->len; i++, array++) {
            *rarray++ = (*array) * (*array);
        }
    } else if(ndarray->dtype.type == NDARRAY_INT16) {
        int16_t *array = (int16_t *)ndarray->array;
        int16_t *rarray = (int16_t *)results->array;
        for(size_t i=0; i < ndarray->len; i++, array++) {
            *rarray++ = (*array) * (*array);
        }
    } else { // if we end up here, the dtype is NDARRAY_FLOAT
        mp_float_t *array = (mp_float_t *)ndarray->array;
        mp_float_t *rarray = (mp_float_t *)results->array;
        for(size_t i=0; i < ndarray->len; i++, array++) {
            *rarray++ = (*array) * (*array);
        }
    }
    // at the end, return a micrppython object
    return MP_OBJ_FROM_PTR(results);
}

MP_DEFINE_CONST_FUN_OBJ_1(user_square_obj, user_square);

extern const mp_obj_type_t imreader_type;

void imreader_imreader(ndarray_obj_t *ndarray, void *array, int32_t *strides, size_t count) {
    blocks_block_obj_t *block = (blocks_block_obj_t *)ndarray->block;
    uint8_t *barray = (uint8_t *)block->subarray;
    // if necessary, get the coordinates in the original reference frame, i.e.,
    // in the coordinates used at the time of the creation of the object
    // size_t *coords = blocks_coords_from_pointer(array, ndarray);
    for(size_t i = 0; i < count; i++) {
        // fill up the array with dummy data
        *barray++ = (uint8_t)i * i;
    }
    // The subarray is a forward propagating dense array, so set the strides to the itemsize
    *strides = ndarray->itemsize;
}

mp_obj_t imreader_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type;
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

static const mp_rom_map_elem_t ulab_user_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_user) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_square), (mp_obj_t)&user_square_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_imreader), (mp_obj_t)&imreader_type },
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_user_globals, ulab_user_globals_table);

mp_obj_module_t ulab_user_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_user_globals,
};

#endif

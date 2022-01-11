/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Zoltán Vörös
*/

#include <string.h>
#include <stdio.h>

#include "py/builtin.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "../../ndarray.h"
#include "io.h"

#define ULAB_IO_BUFFER_SIZE         128


static mp_obj_t io_save(mp_obj_t fname, mp_obj_t ndarray_) {
    if(!mp_obj_is_str(fname) || !mp_obj_is_type(ndarray_, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("wrong input type"));
    }

    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(ndarray_);
    int error;
    char *buffer = m_new(char, ULAB_IO_BUFFER_SIZE);
    uint8_t offset = 0;

    // test for endianness
    uint16_t x = 1;
    int8_t endian = (x >> 8) == 1 ? '>' : '<';

    mp_obj_t open_args[2] = {
        fname,
        MP_OBJ_NEW_QSTR(MP_QSTR_wb)
    };

    mp_obj_t npy = mp_builtin_open(2, open_args, (mp_map_t *)&mp_const_empty_map);
    const mp_stream_p_t *fout = mp_get_stream(npy);

    // write header;
    // magic string + header length, which is always 128 - 10 = 118, represented as a little endian uint16 (0x76, 0x00)
    // + beginning of the dictionary describing the array
    memcpy(buffer, "\x93NUMPY\x01\x00\x76\x00{'descr': '", 21);
    offset += 21;

    buffer[offset] = endian;
    if((ndarray->dtype == NDARRAY_UINT8) || (ndarray->dtype == NDARRAY_INT8)) {
        // for single-byte data, the endianness doesn't matter
        buffer[offset] = '|';
    }
    offset++;
    switch(ndarray->dtype) {
        case NDARRAY_UINT8:
            memcpy(buffer+offset, "u1", 2);
            break;
        case NDARRAY_INT8:
            memcpy(buffer+offset, "i1", 2);
            break;
        case NDARRAY_UINT16:
            memcpy(buffer+offset, "u2", 2);
            break;
        case NDARRAY_INT16:
            memcpy(buffer+offset, "i2", 2);
            break;
        case NDARRAY_FLOAT:
            #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
            memcpy(buffer+offset, "f4", 2);
            #else
            memcpy(buffer+offset, "f8", 2);
            #endif
            break;
        #if ULAB_SUPPORTS_COMPLEX
        case NDARRAY_COMPLEX:
            #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
            memcpy(buffer+offset, "c8", 2);
            #else
            memcpy(buffer+offset, "c16", 2);
            #endif
            offset++;
            break;
        #endif
    }

    offset += 2;
    memcpy(buffer+offset, "', 'fortran_order': False, 'shape': (", 37);
    offset += 37;

    if(ndarray->ndim == 1) {
        offset += sprintf(buffer+offset, "%ld,", ndarray->shape[ULAB_MAX_DIMS - 1]);
    } else {
        for(uint8_t i = ndarray->ndim; i > 1; i--) {
            offset += sprintf(buffer+offset, "%ld, ", ndarray->shape[ULAB_MAX_DIMS - i]);
        }
        offset += sprintf(buffer+offset, "%ld", ndarray->shape[ULAB_MAX_DIMS - 1]);
    }
    memcpy(buffer+offset, "), }", 4);
    offset += 4;
    // pad with space till the very end
    memset(buffer+offset, 32, ULAB_IO_BUFFER_SIZE - offset - 1);
    buffer[ULAB_IO_BUFFER_SIZE - 1] = '\n';
    fout->write(npy, buffer, ULAB_IO_BUFFER_SIZE, &error);

    // write the array data
    uint8_t sz = ndarray->itemsize;
    offset = 0;

    uint8_t *array = (uint8_t *)ndarray->array;

    #if ULAB_MAX_DIMS > 3
    size_t i = 0;
    do {
    #endif
        #if ULAB_MAX_DIMS > 2
        size_t j = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 1
            size_t k = 0;
            do {
            #endif
                size_t l = 0;
                do {
                    memcpy(buffer+offset, array, sz);
                    offset += sz;
                    if(offset == ULAB_IO_BUFFER_SIZE) {
                        fout->write(npy, buffer, offset, &error);
                        offset = 0;
                    }
                    array += ndarray->strides[ULAB_MAX_DIMS - 1];
                    l++;
                } while(l <  ndarray->shape[ULAB_MAX_DIMS - 1]);
            #if ULAB_MAX_DIMS > 1
                array -= ndarray->strides[ULAB_MAX_DIMS - 1] * ndarray->shape[ULAB_MAX_DIMS-1];
                array += ndarray->strides[ULAB_MAX_DIMS - 2];
                k++;
            } while(k <  ndarray->shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 2
            array -= ndarray->strides[ULAB_MAX_DIMS - 2] * ndarray->shape[ULAB_MAX_DIMS-2];
            array += ndarray->strides[ULAB_MAX_DIMS - 3];
            j++;
        } while(j <  ndarray->shape[ULAB_MAX_DIMS - 3]);
        #endif
    #if ULAB_MAX_DIMS > 3
        array -= ndarray->strides[ULAB_MAX_DIMS - 3] * ndarray->shape[ULAB_MAX_DIMS-3];
        array += ndarray->strides[ULAB_MAX_DIMS - 4];
        i++;
    } while(i <  ndarray->shape[ULAB_MAX_DIMS - 4]);
    #endif

    fout->write(npy, buffer, offset, &error);
    fout->ioctl(npy, MP_STREAM_CLOSE, 0, &error);

    m_del(char, buffer, ULAB_IO_BUFFER_SIZE);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(io_save_obj, io_save);


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
#include "extmod/vfs.h"

#include "../../ndarray.h"
#include "io.h"

#define ULAB_IO_BUFFER_SIZE         128

#define ULAB_IO_NULL_ENDIAN         0
#define ULAB_IO_LITTLE_ENDIAN       1
#define ULAB_IO_BIG_ENDIAN          2

#if ULAB_NUMPY_HAS_LOAD
static void io_read_(mp_obj_t stream, const mp_stream_p_t *stream_p, char *buffer, char *string, uint16_t len, int *error) {
    size_t read = stream_p->read(stream, buffer, len, error);
    bool fail = false;
    if(read == len) {
        if(string != NULL) {
            if(memcmp(buffer, string, len) != 0) {
                fail = true;
            }
        }
    } else {
        fail = true;
    }
    if(fail) {
        stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, error);
        mp_raise_msg(&mp_type_RuntimeError, translate("corrupted file"));
    }
}

static mp_obj_t io_load(mp_obj_t file) {
    if(!mp_obj_is_str(file)) {
        mp_raise_TypeError(translate("wrong input type"));
    }

    int error;
    char *buffer = m_new(char, ULAB_IO_BUFFER_SIZE);

    // test for endianness
    uint16_t x = 1;
    int8_t native_endianness = (x >> 8) == 1 ? ULAB_IO_BIG_ENDIAN : ULAB_IO_LITTLE_ENDIAN;

    mp_obj_t open_args[2] = {
        file,
        MP_OBJ_NEW_QSTR(MP_QSTR_rb)
    };

    mp_obj_t stream = io_open_stream(2, open_args, (mp_map_t *)&mp_const_empty_map);
    const mp_stream_p_t *stream_p = mp_get_stream(stream);

    // read header
    // magic string
    io_read_(stream, stream_p, buffer, "\x93NUMPY", 6, &error);
    // simply discard the version number
    io_read_(stream, stream_p, buffer, NULL, 2, &error);
    // header length, represented as a little endian uint16 (0x76, 0x00)
    io_read_(stream, stream_p, buffer, NULL, 2, &error);

    uint16_t header_length = buffer[1];
    header_length <<= 8;
    header_length += buffer[0];

    // beginning of the dictionary describing the array
    io_read_(stream, stream_p, buffer, "{'descr': '", 11, &error);
    uint8_t dtype;

    io_read_(stream, stream_p, buffer, NULL, 1, &error);
    uint8_t endianness = ULAB_IO_NULL_ENDIAN;
    if(*buffer == '<') {
        endianness = ULAB_IO_LITTLE_ENDIAN;
    } else if(*buffer == '>') {
        endianness = ULAB_IO_BIG_ENDIAN;
    }

    io_read_(stream, stream_p, buffer, NULL, 2, &error);
    if(memcmp(buffer, "u1", 2) == 0) {
        dtype = NDARRAY_UINT8;
    } else if(memcmp(buffer, "i1", 2) == 0) {
        dtype = NDARRAY_INT8;
    } else if(memcmp(buffer, "u2", 2) == 0) {
        dtype = NDARRAY_UINT16;
    } else if(memcmp(buffer, "i2", 2) == 0) {
        dtype = NDARRAY_INT16;
    }
    #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
    else if(memcmp(buffer, "f4", 2) == 0) {
        dtype = NDARRAY_FLOAT;
    }
    #else
    else if(memcmp(buffer, "f8", 2) == 0) {
        dtype = NDARRAY_FLOAT;
    }
    #endif
    #if ULAB_SUPPORTS_COMPLEX
    #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
    else if(memcmp(buffer, "c8", 2) == 0) {
        dtype = NDARRAY_COMPLEX;
    }
    #else
    else if(memcmp(buffer, "c16", 3) == 0) {
        dtype = NDARRAY_COMPLEX;
    }
    #endif
    #endif /* ULAB_SUPPORT_COPMLEX */
    else {
        stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, &error);
        mp_raise_TypeError(translate("wrong dtype"));
    }

    io_read_(stream, stream_p, buffer, "', 'fortran_order': False, 'shape': (", 37, &error);

    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    memset(shape, 0, sizeof(size_t) * ULAB_MAX_DIMS);

    uint16_t bytes_to_read = MIN(ULAB_IO_BUFFER_SIZE, header_length - 51);
    // bytes_to_read is 128 at most. This should be enough to contain a
    // maximum of 4 size_t numbers plus the delimiters
    io_read_(stream, stream_p, buffer, NULL, bytes_to_read, &error);
    char *needle = buffer;
    uint8_t ndim = 0;

    // find out the number of dimensions by counting the commas in the string
    while(1) {
        if(*needle == ',') {
            ndim++;
            if(needle[1] == ')') {
                break;
            }
        } else if((*needle == ')') && (ndim > 0)) {
            ndim++;
            break;
        }
        needle++;
    }

    needle = buffer;
    for(uint8_t i = 0; i < ndim; i++) {
        size_t number = 0;
        // trivial number parsing here
        while(1) {
            if((*needle == ' ') || (*needle == '\t')) {
                needle++;
            }
            if((*needle > 47) && (*needle < 58)) {
                number = number * 10 + (*needle - 48);
            } else if((*needle == ',') || (*needle == ')')) {
                break;
            }
            else {
                stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, &error);
                mp_raise_msg(&mp_type_RuntimeError, translate("corrupted file"));
            }
            needle++;
        }
        needle++;
        shape[ULAB_MAX_DIMS - ndim + i] = number;
    }

    // strip the rest of the header
    if((bytes_to_read + 51) < header_length) {
        io_read_(stream, stream_p, buffer, NULL, header_length - (bytes_to_read + 51), &error);
    }

    ndarray_obj_t *ndarray = ndarray_new_dense_ndarray(ndim, shape, dtype);
    char *array = (char *)ndarray->array;

    size_t read = stream_p->read(stream, array, ndarray->len * ndarray->itemsize, &error);
    if(read != ndarray->len * ndarray->itemsize) {
        stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, &error);
        mp_raise_msg(&mp_type_RuntimeError, translate("corrupted file"));
    }

    stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, &error);
    m_del(char, buffer, ULAB_IO_BUFFER_SIZE);

    // swap the bytes, if necessary
    if((native_endianness != endianness) && (dtype != NDARRAY_UINT8) && (dtype != NDARRAY_INT8)) {
        uint8_t sz = ndarray->itemsize;
        char *tmpbuff = NULL;

        #if ULAB_SUPPORTS_COMPLEX
        if(dtype == NDARRAY_COMPLEX) {
            // work with the floating point real and imaginary parts
            sz /= 2;
            tmpbuff = m_new(char, sz);
            for(size_t i = 0; i < ndarray->len; i++) {
                for(uint8_t k = 0; k < 2; k++) {
                    tmpbuff += sz;
                    for(uint8_t j = 0; j < sz; j++) {
                        memcpy(--tmpbuff, array++, 1);
                    }
                    memcpy(array-sz, tmpbuff, sz);
                }
            }
        } else {
        #endif
            tmpbuff = m_new(char, sz);
            for(size_t i = 0; i < ndarray->len; i++) {
                tmpbuff += sz;
                for(uint8_t j = 0; j < sz; j++) {
                    memcpy(--tmpbuff, array++, 1);
                }
                memcpy(array-sz, tmpbuff, sz);
            }
        #if ULAB_SUPPORTS_COMPLEX
        }
        #endif
        m_del(char, tmpbuff, sz);
    }

    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_1(io_load_obj, io_load);
#endif /* ULAB_NUMPY_HAS_LOAD */

#if ULAB_NUMPY_HAS_SAVE
static mp_obj_t io_save(mp_obj_t file, mp_obj_t ndarray_) {
    if(!mp_obj_is_str(file) || !mp_obj_is_type(ndarray_, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("wrong input type"));
    }

    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(ndarray_);
    int error;
    char *buffer = m_new(char, ULAB_IO_BUFFER_SIZE);
    uint8_t offset = 0;

    // test for endianness
    uint16_t x = 1;
    int8_t native_endiannes = (x >> 8) == 1 ? '>' : '<';

    mp_obj_t open_args[2] = {
        file,
        MP_OBJ_NEW_QSTR(MP_QSTR_wb)
    };

    mp_obj_t stream = io_open_stream(2, open_args, (mp_map_t *)&mp_const_empty_map);
    const mp_stream_p_t *stream_p = mp_get_stream(stream);

    // write header;
    // magic string + header length, which is always 128 - 10 = 118, represented as a little endian uint16 (0x76, 0x00)
    // + beginning of the dictionary describing the array
    memcpy(buffer, "\x93NUMPY\x01\x00\x76\x00{'descr': '", 21);
    offset += 21;

    buffer[offset] = native_endiannes;
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
            memcpy(buffer+offset, "c16", 3);
            offset++;
            #endif
            break;
        #endif
    }

    offset += 2;
    memcpy(buffer+offset, "', 'fortran_order': False, 'shape': (", 37);
    offset += 37;

    if(ndarray->ndim == 1) {
        offset += sprintf(buffer+offset, "%zu,", ndarray->shape[ULAB_MAX_DIMS - 1]);
    } else {
        for(uint8_t i = ndarray->ndim; i > 1; i--) {
            offset += sprintf(buffer+offset, "%zu, ", ndarray->shape[ULAB_MAX_DIMS - i]);
        }
        offset += sprintf(buffer+offset, "%zu", ndarray->shape[ULAB_MAX_DIMS - 1]);
    }
    memcpy(buffer+offset, "), }", 4);
    offset += 4;
    // pad with space till the very end
    memset(buffer+offset, 32, ULAB_IO_BUFFER_SIZE - offset - 1);
    buffer[ULAB_IO_BUFFER_SIZE - 1] = '\n';
    stream_p->write(stream, buffer, ULAB_IO_BUFFER_SIZE, &error);

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
                        stream_p->write(stream, buffer, offset, &error);
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

    stream_p->write(stream, buffer, offset, &error);
    stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, &error);

    m_del(char, buffer, ULAB_IO_BUFFER_SIZE);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(io_save_obj, io_save);
#endif /* ULAB_NUMPY_HAS_SAVE */

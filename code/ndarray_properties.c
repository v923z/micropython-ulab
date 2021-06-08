
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Zoltán Vörös
 *
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"

#include "ulab.h"
#include "ndarray.h"

#if !CIRCUITPY

// a somewhat hackish implementation of property getters/setters;
// this functions is hooked into the attr member of ndarray
void ndarray_properties_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL) {
        switch(attr) {
            #if NDARRAY_HAS_DTYPE
            case MP_QSTR_dtype:
                dest[0] = ndarray_dtype(self_in);
                break;
            #endif
            #if NDARRAY_HAS_ITEMSIZE
            case MP_QSTR_itemsize:
                dest[0] = ndarray_itemsize(self_in);
                break;
            #endif
            #if NDARRAY_HAS_ITEMSIZE
            case MP_QSTR_shape:
                dest[0] = ndarray_shape(self_in);
                break;
            #endif
            #if NDARRAY_HAS_SIZE
            case MP_QSTR_size:
                dest[0] = ndarray_size(self_in);
                break;
            #endif
            #if NDARRAY_HAS_STRIDES
            case MP_QSTR_strides:
                dest[0] = ndarray_strides(self_in);
                break;
            #endif
            default:
                return;
        }
    } else {
        return;
        // if (dest[1]) {
        //     switch(attr) {
        //         default:
        //             return;
        //     }

        //     dest[0] = MP_OBJ_NULL;
        // }
    }
}

#endif /* CIRCUITPY */
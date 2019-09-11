/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Zoltán Vörös
*/
    
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "py/runtime.h"
#include "py/binary.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "ndarray.h"

// This function is copied verbatim from objarray.c
STATIC mp_obj_array_t *array_new(char typecode, size_t n) {
    int typecode_size = mp_binary_get_size('@', typecode, NULL);
    mp_obj_array_t *o = m_new_obj(mp_obj_array_t);
    // this step could probably be skipped: we are never going to store a bytearray per se
    #if MICROPY_PY_BUILTINS_BYTEARRAY && MICROPY_PY_ARRAY
    o->base.type = (typecode == BYTEARRAY_TYPECODE) ? &mp_type_bytearray : &mp_type_array;
    #elif MICROPY_PY_BUILTINS_BYTEARRAY
    o->base.type = &mp_type_bytearray;
    #else
    o->base.type = &mp_type_array;
    #endif
    o->typecode = typecode;
    o->free = 0;
    o->len = n;
    o->items = m_new(byte, typecode_size * o->len);
    return o;
}

float ndarray_get_float_value(void *data, uint8_t typecode, size_t index) {
    if(typecode == NDARRAY_UINT8) {
        return (float)((uint8_t *)data)[index];
    } else if(typecode == NDARRAY_INT8) {
        return (float)((int8_t *)data)[index];
    } else if(typecode == NDARRAY_UINT16) {
        return (float)((uint16_t *)data)[index];
    } else if(typecode == NDARRAY_INT16) {
        return (float)((int16_t *)data)[index];
    } else {
        return (float)((float_t *)data)[index];
    }
}

void ndarray_print_row(const mp_print_t *print, mp_obj_array_t *data, size_t n0, size_t n) {
    mp_print_str(print, "[");
    size_t i;
    if(n < PRINT_MAX) { // if the array is short, print everything
        mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0), PRINT_REPR);
        for(i=1; i<n; i++) {
            mp_print_str(print, ", ");
            mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0+i), PRINT_REPR);
        }
    } else {
        mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0), PRINT_REPR);
        for(i=1; i<3; i++) {
            mp_print_str(print, ", ");
            mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0+i), PRINT_REPR);
        }
        mp_printf(print, ", ..., ");
        mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0+n-3), PRINT_REPR);
        for(size_t i=1; i<3; i++) {
            mp_print_str(print, ", ");
            mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0+n-3+i), PRINT_REPR);
        }
    }
    mp_print_str(print, "]");
}

void ndarray_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_print_str(print, "ndarray(");
    if((self->m == 1) || (self->n == 1)) {
        ndarray_print_row(print, self->data, 0, self->data->len);
    } else {
        // TODO: add vertical ellipses for the case, when self->m > PRINT_MAX
        mp_print_str(print, "[");
        ndarray_print_row(print, self->data, 0, self->n);
        for(size_t i=1; i < self->m; i++) {
            mp_print_str(print, ",\n\t ");
            ndarray_print_row(print, self->data, i*self->n, self->n);
        }
        mp_print_str(print, "]");
    }
    // TODO: print typecode
    if(self->data->typecode == NDARRAY_UINT8) {
        printf(", dtype='uint8')");
    } else if(self->data->typecode == NDARRAY_INT8) {
        printf(", dtype='int8')");
    } if(self->data->typecode == NDARRAY_UINT16) {
        printf(", dtype='uint16')");
    } if(self->data->typecode == NDARRAY_INT16) {
        printf(", dtype='int16')");
    } if(self->data->typecode == NDARRAY_FLOAT) {
        printf(", dtype='float')");
    } 
}

void ndarray_assign_elements(mp_obj_array_t *data, mp_obj_t iterable, uint8_t typecode, size_t *idx) {
    // assigns a single row in the matrix
    mp_obj_t item;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        mp_binary_set_val_array(typecode, data->items, (*idx)++, item);
    }
}

ndarray_obj_t *create_new_ndarray(size_t m, size_t n, uint8_t typecode) {
    // Creates the base ndarray with shape (m, n), and initialises the values to straight 0s
    ndarray_obj_t *ndarray = m_new_obj(ndarray_obj_t);
    ndarray->base.type = &ulab_ndarray_type;
    ndarray->m = m;
    ndarray->n = n;
    mp_obj_array_t *data = array_new(typecode, m*n);
    ndarray->bytes = m * n * mp_binary_get_size('@', typecode, NULL);
    // this should set all elements to 0, irrespective of the of the typecode (all bits are zero)
    // we could, perhaps, leave this step out, and initialise the array, only, when needed
    memset(data->items, 0, ndarray->bytes); 
    ndarray->data = data;
    return ndarray;
}

mp_obj_t ndarray_copy(mp_obj_t self_in) {
    // returns a verbatim (shape and typecode) copy of self_in
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ndarray_obj_t *out = create_new_ndarray(self->m, self->n, self->data->typecode);
    int typecode_size = mp_binary_get_size('@', self->data->typecode, NULL);
    memcpy(out->data->items, self->data->items, self->data->len*typecode_size);
    return MP_OBJ_FROM_PTR(out);
}

STATIC uint8_t ndarray_init_helper(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR__array_arr, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)}},
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_QSTR(MP_QSTR_float)} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);    
    GET_STR_DATA_LEN(args[1].u_rom_obj, str, str_len);
    if(memcmp(str, "uint8", 5) == 0) {
        return NDARRAY_UINT8;
    } else if(memcmp(str, "uint16", 6) == 0) {
        return NDARRAY_UINT16;
    } else if(memcmp(str, "int8", 4) == 0) {
        return NDARRAY_INT8;
    } else if(memcmp(str, "int16", 5) == 0) {
        return NDARRAY_INT16;
    }
    return NDARRAY_FLOAT;
}

mp_obj_t ndarray_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    uint8_t dtype = ndarray_init_helper(n_args, args, &kw_args);
    
    size_t len1, len2=0, i=0;
    mp_obj_t len_in = mp_obj_len_maybe(args[0]);
    if (len_in == MP_OBJ_NULL) {
        mp_raise_ValueError("first argument must be an iterable");
    } else {
        len1 = MP_OBJ_SMALL_INT_VALUE(len_in);
    }

    // We have to figure out, whether the first element of the iterable is an iterable itself
    // Perhaps, there is a more elegant way of handling this
    mp_obj_iter_buf_t iter_buf1;
    mp_obj_t item1, iterable1 = mp_getiter(args[0], &iter_buf1);
    while ((item1 = mp_iternext(iterable1)) != MP_OBJ_STOP_ITERATION) {
        len_in = mp_obj_len_maybe(item1);
        if(len_in != MP_OBJ_NULL) { // indeed, this seems to be an iterable
            // Next, we have to check, whether all elements in the outer loop have the same length
            if(i > 0) {
                if(len2 != MP_OBJ_SMALL_INT_VALUE(len_in)) {
                    mp_raise_ValueError("iterables are not of the same length");
                }
            }
            len2 = MP_OBJ_SMALL_INT_VALUE(len_in);
            i++;
        }
    }
    // By this time, it should be established, what the shape is, so we can now create the array
    // set the typecode to float, if the format specifier is missing
    ndarray_obj_t *self = create_new_ndarray(len1, (len2 == 0) ? 1 : len2, dtype);
    iterable1 = mp_getiter(args[0], &iter_buf1);
    i = 0;
    if(len2 == 0) { // the first argument is a single iterable
        ndarray_assign_elements(self->data, iterable1, dtype, &i);
    } else {
        mp_obj_iter_buf_t iter_buf2;
        mp_obj_t iterable2; 

        while ((item1 = mp_iternext(iterable1)) != MP_OBJ_STOP_ITERATION) {
            iterable2 = mp_getiter(item1, &iter_buf2);
            ndarray_assign_elements(self->data, iterable2, dtype, &i);
        }
    }
    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t ndarray_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    // NOTE: this will work only on the flattened array!
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (value == MP_OBJ_SENTINEL) { 
        // simply return the values at index, no assignment
#if MICROPY_PY_BUILTINS_SLICE
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            mp_seq_get_fast_slice_indexes(self->data->len, index, &slice);
            // TODO: this won't work with index reversion!!!
            size_t len = (slice.stop - slice.start) / slice.step;
            ndarray_obj_t *out = create_new_ndarray(1, len, self->data->typecode);
            int _sizeof = mp_binary_get_size('@', self->data->typecode, NULL);
            uint8_t *indata = (uint8_t *)self->data->items;
            uint8_t *outdata = (uint8_t *)out->data->items;
            for(size_t i=0; i < len; i++) {
                memcpy(outdata+(i*_sizeof), indata+(slice.start+i*slice.step)*_sizeof, _sizeof);
            }
            return MP_OBJ_FROM_PTR(out);
        }
#endif
        // we have a single index, return a single number
        size_t idx = mp_obj_get_int(index);
        switch(self->data->typecode) {
            case NDARRAY_UINT8:
                return MP_OBJ_NEW_SMALL_INT(((uint8_t *)self->data->items)[idx]);
            case NDARRAY_INT8:
                return MP_OBJ_NEW_SMALL_INT(((int8_t *)self->data->items)[idx]);
            case NDARRAY_UINT16:
                return MP_OBJ_NEW_SMALL_INT(((uint16_t *)self->data->items)[idx]);
            case NDARRAY_INT16:
                return MP_OBJ_NEW_SMALL_INT(((int16_t *)self->data->items)[idx]);
            case NDARRAY_FLOAT:
                return mp_obj_new_float(((float_t *)self->data->items)[idx]);
        }
    } else { // do not deal with assignment, bail out
        mp_raise_NotImplementedError("subcript assignment is not implemented for ndarrays");
    }
    return mp_const_none;
}

// itarray iterator

mp_obj_t ndarray_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf) {
    return mp_obj_new_ndarray_iterator(o_in, 0, iter_buf);
}

typedef struct _mp_obj_ndarray_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_t ndarray;
    size_t cur;
} mp_obj_ndarray_it_t;

mp_obj_t ndarray_iternext(mp_obj_t self_in) {
    mp_obj_ndarray_it_t *self = MP_OBJ_TO_PTR(self_in);
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(self->ndarray);
    // TODO: in numpy, ndarrays are iterated with respect to the first axis. 
    size_t iter_end = 0;
    if((ndarray->m == 1) || (ndarray->n ==1)) {
        iter_end = ndarray->data->len;
    } else {
        iter_end = ndarray->m;
    }
    if(self->cur < iter_end) {
        if(ndarray->m == ndarray->data->len) { // we are have a linear array
            // read the current value
            mp_obj_t value;
            value = mp_binary_get_val_array(ndarray->data->typecode, ndarray->data->items, self->cur);
            self->cur++;
            return value;
        } else { // we have a matrix, return the 
            ndarray_obj_t *value = create_new_ndarray(1, ndarray->n, ndarray->data->typecode);
            // copy the memory content here
            uint8_t *tmp = (uint8_t *)ndarray->data->items;
            size_t strip_size = ndarray->n * mp_binary_get_size('@', ndarray->data->typecode, NULL);
            memcpy(value->data->items, &tmp[self->cur*strip_size], strip_size);
            self->cur++;
            return value;
        }
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

mp_obj_t mp_obj_new_ndarray_iterator(mp_obj_t ndarray, size_t cur, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(mp_obj_ndarray_it_t) <= sizeof(mp_obj_iter_buf_t));
    mp_obj_ndarray_it_t *o = (mp_obj_ndarray_it_t*)iter_buf;
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = ndarray_iternext;
    o->ndarray = ndarray;
    o->cur = cur;
    return MP_OBJ_FROM_PTR(o);
}

mp_obj_t ndarray_shape(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t tuple[2] = {
        mp_obj_new_int(self->m),
        mp_obj_new_int(self->n)
    };
    return mp_obj_new_tuple(2, tuple);
}

mp_obj_t ndarray_size(mp_obj_t self_in, mp_obj_t axis) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t ax = mp_obj_get_int(axis);
    if(ax == 0) {
        return mp_obj_new_int(self->data->len);
    } else if(ax == 1) {
        return mp_obj_new_int(self->m);
    } else if(ax == 2) {
        return mp_obj_new_int(self->n);
    } else {
        return mp_const_none;
    }
}

mp_obj_t ndarray_rawsize(mp_obj_t self_in) {
    // returns a 5-tuple with the 
    // 
    // 1. number of rows
    // 2. number of columns
    // 3. length of the storage (should be equal to the product of 1. and 2.)
    // 4. length of the data storage in bytes
    // 5. datum size in bytes
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(5, NULL));
    tuple->items[0] = MP_OBJ_NEW_SMALL_INT(self->m);
    tuple->items[1] = MP_OBJ_NEW_SMALL_INT(self->n);
    tuple->items[2] = MP_OBJ_NEW_SMALL_INT(self->bytes);
    tuple->items[3] = MP_OBJ_NEW_SMALL_INT(self->data->len);
    tuple->items[4] = MP_OBJ_NEW_SMALL_INT(mp_binary_get_size('@', self->data->typecode, NULL));
    return tuple;
}

// Binary operations
STATIC uint8_t upcasting(uint8_t type_left, uint8_t type_right) {
    // returns the upcast typecode
    // Now we have to collect 25 cases. Perhaps there is a more elegant solution for this    
    if(type_left == type_right) { 
        // 5 cases
        return type_left;
    } else if((type_left == NDARRAY_FLOAT) || (type_right == NDARRAY_FLOAT)) { 
        // 8 cases ('f' AND 'f' has already been accounted for) 
        return NDARRAY_FLOAT;
    } else if(((type_left == NDARRAY_UINT8) && (type_right == NDARRAY_INT8)) || 
              ((type_left == NDARRAY_INT8) && (type_right == NDARRAY_UINT8)) || 
              ((type_left == NDARRAY_UINT8) && (type_right == NDARRAY_INT16)) || 
              ((type_left == NDARRAY_INT16) && (type_right == NDARRAY_UINT8)) || 
              ((type_left == NDARRAY_UINT8) && (type_right == NDARRAY_UINT16)) ||
              ((type_left == NDARRAY_UINT16) && (type_right == NDARRAY_UINT8)) || 
              ((type_left == NDARRAY_INT8) && (type_right == NDARRAY_UINT16)) ||
              ((type_left == NDARRAY_UINT16) && (type_right == NDARRAY_INT8)) ) {
        // 8 cases
        return NDARRAY_UINT16;
    } else if ( ((type_left == NDARRAY_INT8) && (type_right == NDARRAY_INT16)) ||
               ((type_left == NDARRAY_INT16) && (type_right == NDARRAY_INT8)) ) {
        // 2 cases
        return NDARRAY_INT16;
    } else if ( ((type_left == NDARRAY_INT16) && (type_right == NDARRAY_UINT16)) ||
               ((type_left == NDARRAY_UINT16) && (type_right == NDARRAY_INT16)) ) {
        // 2 cases
        return NDARRAY_FLOAT;
    }
    return NDARRAY_FLOAT; // we are never going to reach this statement, but we have to make the compiler happy
}

mp_obj_t ndarray_binary_op(mp_binary_op_t op, mp_obj_t lhs, mp_obj_t rhs) {
    ndarray_obj_t *ol = MP_OBJ_TO_PTR(lhs);
    uint8_t typecode;
    float value;
    // First, the right hand side is a native micropython object, i.e, an integer, or a float
    if (MP_OBJ_IS_TYPE(rhs, &mp_type_int) || MP_OBJ_IS_TYPE(rhs, &mp_type_float)) {
        // we have to split the two cases here...
        if(MP_OBJ_IS_TYPE(rhs, &mp_type_int)) {
            typecode = upcasting(ol->data->typecode, NDARRAY_INT16);
        } else {
            typecode = upcasting(ol->data->typecode, NDARRAY_FLOAT);    
        }
        if(MP_OBJ_IS_TYPE(rhs, &mp_type_int)) {
            value = (float)mp_obj_get_int(rhs);
        } else {
            value = mp_obj_get_float(rhs);
        }
        if((op == MP_BINARY_OP_ADD) || (op == MP_BINARY_OP_MULTIPLY) || 
            (op == MP_BINARY_OP_SUBTRACT) || (op == MP_BINARY_OP_TRUE_DIVIDE)) {
            ndarray_obj_t *out = create_new_ndarray(ol->m, ol->n, typecode);
            if(op == MP_BINARY_OP_SUBTRACT) value *= -1.0;
            if(op == MP_BINARY_OP_TRUE_DIVIDE) value = 1.0/value;
            if(typecode == NDARRAY_INT16) {
                int16_t *outdata = (int16_t *)out->data->items;
                if((op == MP_BINARY_OP_ADD) || (op == MP_BINARY_OP_SUBTRACT)) {
                    for(size_t i=0; i < ol->data->len; i++) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) + value;
                    }
                } else if((op == MP_BINARY_OP_MULTIPLY) || (op == MP_BINARY_OP_TRUE_DIVIDE)) {
                    for(size_t i=0; i < ol->data->len; i++) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) * value;
                    }
                }
            } else if(typecode == NDARRAY_FLOAT) {
                float *outdata = (float *)out->data->items;
                if((op == MP_BINARY_OP_ADD) || (op == MP_BINARY_OP_SUBTRACT)) {
                    for(size_t i=0; i < ol->data->len; i++) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) + value;
                    }
                } else if((op == MP_BINARY_OP_MULTIPLY) || (op == MP_BINARY_OP_TRUE_DIVIDE)) {
                    for(size_t i=0; i < ol->data->len; i++) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) * value;
                    }
                }                    
            }
            return MP_OBJ_FROM_PTR(out);
        } else {
            return MP_OBJ_NULL; // op not supported
        }
    } else if(MP_OBJ_IS_TYPE(rhs, &ulab_ndarray_type)) { // next, the ndarray stuff
        ndarray_obj_t *or = MP_OBJ_TO_PTR(rhs);
        if((ol->m != or->m) || (ol->n != or->n)) {
            mp_raise_ValueError("operands could not be broadcast together");
        }
        // At this point, the operands should have the same shape
        typecode = upcasting(or->data->typecode, ol->data->typecode);
        if(op == MP_BINARY_OP_EQUAL) {
            // Two arrays are equal, if their shape, typecode, and elements are equal
            if((ol->m != or->m) || (ol->n != or->n) || (ol->data->typecode != or->data->typecode)) {
                return mp_const_false;
            } else {
                size_t i = ol->bytes;
                uint8_t *l = (uint8_t *)ol->data->items;
                uint8_t *r = (uint8_t *)or->data->items;
                while(i) { // At this point, we can simply compare the bytes, the type is irrelevant
                    if(*l++ != *r++) {
                        return mp_const_false;
                    }
                    i--;
                }
                return mp_const_true;
            }
        } else if((op == MP_BINARY_OP_ADD) || (op == MP_BINARY_OP_SUBTRACT) || 
            (op == MP_BINARY_OP_TRUE_DIVIDE) || (op == MP_BINARY_OP_MULTIPLY)) {
            // for in-place operations, we won't need this!!!
            typecode = upcasting(or->data->typecode, ol->data->typecode);
            ndarray_obj_t *out = create_new_ndarray(ol->m, ol->n, typecode);
            if(typecode == NDARRAY_UINT8) {
                uint8_t *outdata = (uint8_t *)out->data->items;
                for(size_t i=0; i < ol->data->len; i++) {
                    value = ndarray_get_float_value(or->data->items, or->data->typecode, i);
                    if(op == MP_BINARY_OP_ADD) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) + value;
                    } else if(op == MP_BINARY_OP_SUBTRACT) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) - value;                            
                    } else if(op == MP_BINARY_OP_MULTIPLY) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) * value;
                    } else if(op == MP_BINARY_OP_TRUE_DIVIDE) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) / value;
                    }
                }
            } else if(typecode == NDARRAY_INT8) {
                int8_t *outdata = (int8_t *)out->data->items;
                for(size_t i=0; i < ol->data->len; i++) {
                    value = ndarray_get_float_value(or->data->items, or->data->typecode, i);
                    if(op == MP_BINARY_OP_ADD) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) + value;
                    } else if(op == MP_BINARY_OP_SUBTRACT) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) - value;                            
                    } else if(op == MP_BINARY_OP_MULTIPLY) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) * value;
                    } else if(op == MP_BINARY_OP_TRUE_DIVIDE) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) / value;
                    }
                }                    
            } else if(typecode == NDARRAY_UINT16) {
                uint16_t *outdata = (uint16_t *)out->data->items;
                for(size_t i=0; i < ol->data->len; i++) {
                    value = ndarray_get_float_value(or->data->items, or->data->typecode, i);
                    if(op == MP_BINARY_OP_ADD) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) + value;
                    } else if(op == MP_BINARY_OP_SUBTRACT) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) - value;                            
                    } else if(op == MP_BINARY_OP_MULTIPLY) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) * value;
                    } else if(op == MP_BINARY_OP_TRUE_DIVIDE) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) / value;
                    }
                }
            } else if(typecode == NDARRAY_INT16) {
                int16_t *outdata = (int16_t *)out->data->items;
                for(size_t i=0; i < ol->data->len; i++) {
                    value = ndarray_get_float_value(or->data->items, or->data->typecode, i);
                    if(op == MP_BINARY_OP_ADD) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) + value;
                    } else if(op == MP_BINARY_OP_SUBTRACT) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) - value;                            
                    } else if(op == MP_BINARY_OP_MULTIPLY) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) * value;
                    } else if(op == MP_BINARY_OP_TRUE_DIVIDE) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) / value;
                    }
                } 
            } else if(typecode == NDARRAY_FLOAT) {
                float *outdata = (float *)out->data->items;
                for(size_t i=0; i < ol->data->len; i++) {
                    value = ndarray_get_float_value(or->data->items, or->data->typecode, i);
                    if(op == MP_BINARY_OP_ADD) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) + value;
                    } else if(op == MP_BINARY_OP_SUBTRACT) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) - value;                            
                    } else if(op == MP_BINARY_OP_MULTIPLY) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) * value;
                    } else if(op == MP_BINARY_OP_TRUE_DIVIDE) {
                        outdata[i] = ndarray_get_float_value(ol->data->items, ol->data->typecode, i) / value;
                    }
                }
            }
            return MP_OBJ_FROM_PTR(out);
        } else {
            return MP_OBJ_NULL; // op not supported                                                        
        }
    } else {
        mp_raise_TypeError("wrong operand type on the right hand side");
    }
}

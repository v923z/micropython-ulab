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

void fill_array_iterable(float *array, mp_obj_t iterable) {
    mp_obj_iter_buf_t x_buf;
    mp_obj_t x_item, x_iterable = mp_getiter(iterable, &x_buf);
    size_t i=0;
    while ((x_item = mp_iternext(x_iterable)) != MP_OBJ_STOP_ITERATION) {
        array[i] = (float)mp_obj_get_float(x_item);
        i++;
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
    mp_print_str(print, "array(");
    if((self->m == 1) || (self->n == 1)) {
        ndarray_print_row(print, self->array, 0, self->array->len);
    } else {
        // TODO: add vertical ellipses for the case, when self->m > PRINT_MAX
        mp_print_str(print, "[");
        ndarray_print_row(print, self->array, 0, self->n);
        for(size_t i=1; i < self->m; i++) {
            mp_print_str(print, ",\n\t ");
            ndarray_print_row(print, self->array, i*self->n, self->n);
        }
        mp_print_str(print, "]");
    }
    if(self->array->typecode == NDARRAY_UINT8) {
        mp_print_str(print, ", dtype=uint8)");
    } else if(self->array->typecode == NDARRAY_INT8) {
        mp_print_str(print, ", dtype=int8)");
    } else if(self->array->typecode == NDARRAY_UINT16) {
        mp_print_str(print, ", dtype=uint16)");
    } else if(self->array->typecode == NDARRAY_INT16) {
        mp_print_str(print, ", dtype=int16)");
    } else if(self->array->typecode == NDARRAY_FLOAT) {
        mp_print_str(print, ", dtype=float)");
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
    mp_obj_array_t *array = array_new(typecode, m*n);
    ndarray->bytes = m * n * mp_binary_get_size('@', typecode, NULL);
    // this should set all elements to 0, irrespective of the of the typecode (all bits are zero)
    // we could, perhaps, leave this step out, and initialise the array only, when needed
    memset(array->items, 0, ndarray->bytes); 
    ndarray->array = array;
    return ndarray;
}

mp_obj_t ndarray_copy(mp_obj_t self_in) {
    // returns a verbatim (shape and typecode) copy of self_in
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ndarray_obj_t *out = create_new_ndarray(self->m, self->n, self->array->typecode);
    memcpy(out->array->items, self->array->items, self->bytes);
    return MP_OBJ_FROM_PTR(out);
}

STATIC uint8_t ndarray_init_helper(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = NDARRAY_FLOAT } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(1, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    uint8_t dtype = args[1].u_int;
    return dtype;
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
        // len1 is either the number of rows (for matrices), or the number of elements (row vectors)
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
    ndarray_obj_t *self = create_new_ndarray((len2 == 0) ? 1 : len1, (len2 == 0) ? len1 : len2, dtype);
    iterable1 = mp_getiter(args[0], &iter_buf1);
    i = 0;
    if(len2 == 0) { // the first argument is a single iterable
        ndarray_assign_elements(self->array, iterable1, dtype, &i);
    } else {
        mp_obj_iter_buf_t iter_buf2;
        mp_obj_t iterable2; 

        while ((item1 = mp_iternext(iterable1)) != MP_OBJ_STOP_ITERATION) {
            iterable2 = mp_getiter(item1, &iter_buf2);
            ndarray_assign_elements(self->array, iterable2, dtype, &i);
        }
    }
    return MP_OBJ_FROM_PTR(self);
}

void ndarray_copy_slice(mp_bound_slice_t slice, ndarray_obj_t *source, ndarray_obj_t *target, 
                        size_t soff, size_t toff, size_t len) {
    uint8_t *sarray = (uint8_t *)source->array->items;
    uint8_t *tarray = (uint8_t *)target->array->items;
    uint8_t _sizeof = mp_binary_get_size('@', source->array->typecode, NULL);
    for(uint16_t i=0; i < len; i++) {
        memcpy(tarray+toff+(i*_sizeof), sarray+soff+(slice.start+i*slice.step)*_sizeof, _sizeof);
    }
}

void ndarray_assign_slice(mp_bound_slice_t slice, mp_obj_t source, ndarray_obj_t *target, size_t offset, size_t len) {
    if(mp_obj_is_int(source) || mp_obj_is_float(source)) {
        for(size_t i=0; i < len; i++) {
        mp_binary_set_val_array(target->array->typecode, target->array->items, 
                                offset+(slice.start+i*slice.step), source);
        }
    } else if(mp_obj_is_type(source, &ulab_ndarray_type)) {
        // at this point, source should be a row vector of length len
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(source);
        if(ndarray->array->len != len) {
            mp_raise_ValueError("array lengths are not compatible");
        } else {
            mp_obj_t value;
            for(size_t i=0; i < len; i++) {
                value = mp_binary_get_val_array(ndarray->array->typecode, ndarray->array->items, i);
                mp_binary_set_val_array(target->array->typecode, target->array->items, 
                                offset+(slice.start+i*slice.step), value);
            }
        }
    } else {
        mp_raise_TypeError("right hand side of assignment must be an integer or an ndarray");
    }
}

size_t slice_length(mp_bound_slice_t slice) {
    if(slice.stop > slice.start) {
      return (slice.stop - slice.start) / slice.step;
    } else {
      return (slice.start - slice.stop) / slice.step;
    }
}

mp_obj_t fill_in_trues(ndarray_obj_t *source, mp_obj_t index) {
    if(source->array->len != mp_obj_get_int(mp_obj_len(index))) {
        // the index vector should be exactly as long as self
        mp_raise_ValueError("boolean index did not match array length");  
    }
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t item, iterable = mp_getiter(index, &iter_buf);
    size_t trues = 0;
    while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        if(!mp_obj_is_type(item, &mp_type_bool)) {
            // numpy seems to be a little bit inconsistent in when an index is considered 
            // to be True/False. Bail out immediately, if the items is not True/False
            return mp_const_none;
        }
        if(mp_obj_is_true(item)) {
            trues++;
        }
    }
    if(trues == 0) { // there is not too much we can do with an empty array...
        return mp_const_none;
    }
    
    ndarray_obj_t *ndarray = create_new_ndarray(1, trues, source->array->typecode);
    size_t i = 0;
    
    // reset trues counter, and iterator
    trues = 0;
    iterable = mp_getiter(index, &iter_buf);
    
    while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        if(mp_obj_is_true(item)) {
             mp_binary_set_val_array(ndarray->array->typecode, 
                                     ndarray->array->items, trues++, 
                                     mp_binary_get_val_array(source->array->typecode, 
                                                             source->array->items, i));
        }
        i++;
    }
    return MP_OBJ_FROM_PTR(ndarray);
}

mp_obj_t ndarray_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    // NOTE: this will work only on the flattened array!
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (value == MP_OBJ_SENTINEL) {
        // simply return the values at index, no assignment
        if(MP_OBJ_IS_TYPE(index, &mp_type_tuple)) {
            mp_raise_NotImplementedError("slicing is not implemented for 2D arrays");            
        }
        if(MP_OBJ_IS_TYPE(index, &mp_type_list)) {
            if(self->n != self->array->len) {
                mp_raise_ValueError("list indices are implemented for row vectors only");
            } else { 
                return fill_in_trues(self, index);
            }
        }
        if(MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            mp_seq_get_fast_slice_indexes(self->array->len, index, &slice);
            size_t len = slice_length(slice);
            ndarray_obj_t *target = create_new_ndarray(1, len, self->array->typecode);
            ndarray_copy_slice(slice, self, target, 0, 0, len);
            return MP_OBJ_FROM_PTR(target);
        }
        // we have a single index, return either a single number (arrays), or an array (matrices)
        int16_t idx = mp_obj_get_int(index);
        if(idx < 0) {
            idx = self->m > 1 ? self->m + idx : self->n + idx;
        }
        if(self->m > 1) { // we do have a matrix
            if(idx >= self->m) {
                mp_raise_ValueError("index is out of range");
            }
            if(self->n == 1) { // the matrix is actually a column vector
                return mp_binary_get_val_array(self->array->typecode, self->array->items, idx);
            }
            // return an array
            ndarray_obj_t *out = create_new_ndarray(1, self->n, self->array->typecode);
            int _sizeof = mp_binary_get_size('@', self->array->typecode, NULL);
            uint8_t *indata = (uint8_t *)self->array->items;
            uint8_t *outdata = (uint8_t *)out->array->items;
            memcpy(outdata, &indata[idx*self->n*_sizeof], self->n*_sizeof);
            return MP_OBJ_FROM_PTR(out);            
        }
        // since self->m == 1, we have a flat array, hence, we've got to return a single number
        if(idx >= self->n) {
            mp_raise_ValueError("index is out of range");
        }
        return mp_binary_get_val_array(self->array->typecode, self->array->items, idx);
    } else { 
        // assignment
        if(MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            mp_seq_get_fast_slice_indexes(self->array->len, index, &slice);
            size_t len = slice_length(slice);
            if(self->array->len == self->n) { // the left hand side is a row vector
                ndarray_assign_slice(slice, value, self, 0, len);
            } else {
                mp_bound_slice_t full_slice;
                full_slice.start = 0; 
                full_slice.stop = self->n;
                full_slice.step = 1;
                for(size_t i=0; i < len; i++) {
                    ndarray_assign_slice(full_slice, value, self, (slice.start+i*slice.step)*self->n, self->n);
                }
            }
            return mp_const_none;
        } else if(MP_OBJ_IS_TYPE(index, &mp_type_tuple)) {
            mp_raise_NotImplementedError("tuple assignment is not implemented");
            //mp_bound_slice_t slice;
            //mp_seq_get_fast_slice_indexes(self->array->len, index, &slice);
            //size_t len = slice_length(slice);
        } else if(mp_obj_is_int(index)) {
            size_t pos;
            int32_t idx = mp_obj_get_int(index);
            size_t index_range;
            if(self->n == self->array->len) { // row vector
                index_range = self->n;
            } else {
                index_range = self->m;
            }
            if((idx > 0) && (idx < index_range)) {
                pos = idx;
            }
            else if((idx < 0) && (idx > -index_range)) {
                pos = index_range + mp_obj_get_int(index);
            } else {
                mp_raise_ValueError("index is out of range");
            }
            if(self->n == self->array->len) { // row vector
                mp_binary_set_val_array(self->array->typecode, self->array->items, pos, value);
            } else {
                mp_bound_slice_t full_slice;
                full_slice.start = 0; 
                full_slice.stop = self->n;
                full_slice.step = 1;
                ndarray_assign_slice(full_slice, value, self, pos*self->n, self->n);
            }
            return mp_const_none;
        }
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
    if((ndarray->m == 1)) {
        iter_end = ndarray->array->len;
    } else {
        iter_end = ndarray->m;
    }
    if(self->cur < iter_end) {
        if(ndarray->n == ndarray->array->len) { // we have a linear array
            // read the current value
            mp_obj_t value;
            value = mp_binary_get_val_array(ndarray->array->typecode, ndarray->array->items, self->cur);
            self->cur++;
            return value;
        } else { // we have a matrix, return the number of rows
            ndarray_obj_t *value = create_new_ndarray(1, ndarray->n, ndarray->array->typecode);
            // copy the memory content here
            uint8_t *tmp = (uint8_t *)ndarray->array->items;
            size_t strip_size = ndarray->n * mp_binary_get_size('@', ndarray->array->typecode, NULL);
            memcpy(value->array->items, &tmp[self->cur*strip_size], strip_size);
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
        return mp_obj_new_int(self->array->len);
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
    tuple->items[3] = MP_OBJ_NEW_SMALL_INT(self->array->len);
    tuple->items[4] = MP_OBJ_NEW_SMALL_INT(mp_binary_get_size('@', self->array->typecode, NULL));
    return tuple;
}

mp_obj_t ndarray_flatten(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_order, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_QSTR(MP_QSTR_C)} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_obj_t self_copy = ndarray_copy(pos_args[0]);
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(self_copy);
    
    GET_STR_DATA_LEN(args[0].u_obj, order, len);    
    if((len != 1) || ((memcmp(order, "C", 1) != 0) && (memcmp(order, "F", 1) != 0))) {
        mp_raise_ValueError("flattening order must be either 'C', or 'F'");        
    }

    // if order == 'C', we simply have to set m, and n, there is nothing else to do
    if(memcmp(order, "F", 1) == 0) {
        ndarray_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
        uint8_t _sizeof = mp_binary_get_size('@', self->array->typecode, NULL);
        // get the data of self_in: we won't need a temporary buffer for the transposition
        uint8_t *self_array = (uint8_t *)self->array->items;
        uint8_t *array = (uint8_t *)ndarray->array->items;
        size_t i=0;
        for(size_t n=0; n < self->n; n++) {
            for(size_t m=0; m < self->m; m++) {
                memcpy(array+_sizeof*i, self_array+_sizeof*(m*self->n + n), _sizeof);
                i++;
            }
        }        
    }
    ndarray->n = ndarray->array->len;
    ndarray->m = 1;
    return self_copy;
}

// Binary operations

mp_obj_t ndarray_binary_op(mp_binary_op_t op, mp_obj_t lhs, mp_obj_t rhs) {
    // One of the operands is a scalar
    // TODO: conform to numpy with the upcasting
    // TODO: implement in-place operators
    mp_obj_t RHS = MP_OBJ_NULL;
    bool rhs_is_scalar = true;
    if(mp_obj_is_int(rhs)) {
        int32_t ivalue = mp_obj_get_int(rhs);
        if((ivalue > 0) && (ivalue < 256)) {
            CREATE_SINGLE_ITEM(RHS, uint8_t, NDARRAY_UINT8, ivalue);
        }
        else if((ivalue > 0) && (ivalue < 65535)) {
            CREATE_SINGLE_ITEM(RHS, uint16_t, NDARRAY_UINT16, ivalue);
        }
        else if((ivalue < 0) && (ivalue > -128)) {
            CREATE_SINGLE_ITEM(RHS, int8_t, NDARRAY_INT8, ivalue);
        }
        else if((ivalue < 0) && (ivalue > -32767)) {
            CREATE_SINGLE_ITEM(RHS, int16_t, NDARRAY_INT16, ivalue);
        }
    } else if(mp_obj_is_float(rhs)) {
        float fvalue = mp_obj_get_float(rhs);        
        CREATE_SINGLE_ITEM(RHS, float, NDARRAY_FLOAT, fvalue);
    } else {
        RHS = rhs;
        rhs_is_scalar = false;
    }
    //else 
    if(mp_obj_is_type(lhs, &ulab_ndarray_type) && mp_obj_is_type(RHS, &ulab_ndarray_type)) { 
        // next, the ndarray stuff
        ndarray_obj_t *ol = MP_OBJ_TO_PTR(lhs);
        ndarray_obj_t *or = MP_OBJ_TO_PTR(RHS);
        if(!rhs_is_scalar && ((ol->m != or->m) || (ol->n != or->n))) {
            mp_raise_ValueError("operands could not be broadcast together");
        }
        // At this point, the operands should have the same shape
        switch(op) {
            case MP_BINARY_OP_EQUAL:
                // Two arrays are equal, if their shape, typecode, and elements are equal
                if((ol->m != or->m) || (ol->n != or->n) || (ol->array->typecode != or->array->typecode)) {
                    return mp_const_false;
                } else {
                    size_t i = ol->bytes;
                    uint8_t *l = (uint8_t *)ol->array->items;
                    uint8_t *r = (uint8_t *)or->array->items;
                    while(i) { // At this point, we can simply compare the bytes, the type is irrelevant
                        if(*l++ != *r++) {
                            return mp_const_false;
                        }
                        i--;
                    }
                    return mp_const_true;
                }
                break;
            case MP_BINARY_OP_LESS:
            case MP_BINARY_OP_LESS_EQUAL:
            case MP_BINARY_OP_MORE:
            case MP_BINARY_OP_MORE_EQUAL:
            case MP_BINARY_OP_ADD:
            case MP_BINARY_OP_SUBTRACT:
            case MP_BINARY_OP_TRUE_DIVIDE:
            case MP_BINARY_OP_MULTIPLY:
                // TODO: I believe, this part can be made significantly smaller (compiled size)
                // by doing only the typecasting in the large ifs, and moving the loops outside
                // These are the upcasting rules
                // float always becomes float
                // operation on identical types preserves type
                // uint8 + int8 => int16
                // uint8 + int16 => int16
                // uint8 + uint16 => uint16
                // int8 + int16 => int16
                // int8 + uint16 => uint16
                // uint16 + int16 => float
                // The parameters of RUN_BINARY_LOOP are 
                // typecode of result, type_out, type_left, type_right, lhs operand, rhs operand, operator
                if(ol->array->typecode == NDARRAY_UINT8) {
                    if(or->array->typecode == NDARRAY_UINT8) {
                        RUN_BINARY_LOOP(NDARRAY_UINT8, uint8_t, uint8_t, uint8_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_INT8) {
                        RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, uint8_t, int8_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_UINT16) {
                        RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint8_t, uint16_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_INT16) {
                        RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, uint8_t, int16_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_FLOAT) {
                        RUN_BINARY_LOOP(NDARRAY_FLOAT, float, uint8_t, float, ol, or, op);
                    }
                } else if(ol->array->typecode == NDARRAY_INT8) {
                    if(or->array->typecode == NDARRAY_UINT8) {
                        RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int8_t, uint8_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_INT8) {
                        RUN_BINARY_LOOP(NDARRAY_INT8, int8_t, int8_t, int8_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_UINT16) {
                        RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int8_t, uint16_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_INT16) {
                        RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int8_t, int16_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_FLOAT) {
                        RUN_BINARY_LOOP(NDARRAY_FLOAT, float, int8_t, float, ol, or, op);
                    }                
                } else if(ol->array->typecode == NDARRAY_UINT16) {
                    if(or->array->typecode == NDARRAY_UINT8) {
                        RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, uint8_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_INT8) {
                        RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, int8_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_UINT16) {
                        RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, uint16_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_INT16) {
                        RUN_BINARY_LOOP(NDARRAY_FLOAT, float, uint16_t, int16_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_FLOAT) {
                        RUN_BINARY_LOOP(NDARRAY_FLOAT, float, uint8_t, float, ol, or, op);
                    }
                } else if(ol->array->typecode == NDARRAY_INT16) {
                    if(or->array->typecode == NDARRAY_UINT8) {
                        RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int16_t, uint8_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_INT8) {
                        RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int16_t, int8_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_UINT16) {
                        RUN_BINARY_LOOP(NDARRAY_FLOAT, float, int16_t, uint16_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_INT16) {
                        RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int16_t, int16_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_FLOAT) {
                        RUN_BINARY_LOOP(NDARRAY_FLOAT, float, uint16_t, float, ol, or, op);
                    }
                } else if(ol->array->typecode == NDARRAY_FLOAT) {
                    if(or->array->typecode == NDARRAY_UINT8) {
                        RUN_BINARY_LOOP(NDARRAY_FLOAT, float, float, uint8_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_INT8) {
                        RUN_BINARY_LOOP(NDARRAY_FLOAT, float, float, int8_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_UINT16) {
                        RUN_BINARY_LOOP(NDARRAY_FLOAT, float, float, uint16_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_INT16) {
                        RUN_BINARY_LOOP(NDARRAY_FLOAT, float, float, int16_t, ol, or, op);
                    } else if(or->array->typecode == NDARRAY_FLOAT) {
                        RUN_BINARY_LOOP(NDARRAY_FLOAT, float, float, float, ol, or, op);
                    }
                } else { // this should never happen
                    mp_raise_TypeError("wrong input type");
                }
                // this instruction should never be reached, but we have to make the compiler happy
                return MP_OBJ_NULL; 
            default:
                return MP_OBJ_NULL; // op not supported                                                        
        }
    } else {
        mp_raise_TypeError("wrong operand type on the right hand side");
    }
}

mp_obj_t ndarray_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ndarray_obj_t *ndarray = NULL;
    switch (op) {
        case MP_UNARY_OP_LEN: 
            if(self->m > 1) {
                return mp_obj_new_int(self->m);
            } else {
                return mp_obj_new_int(self->n);
            }
            break;
        
        case MP_UNARY_OP_INVERT:
            if(self->array->typecode == NDARRAY_FLOAT) {
                mp_raise_ValueError("operation is not supported for given type");
            }
            // we can invert the content byte by byte, there is no need to distinguish 
            // between different typecodes
            ndarray = MP_OBJ_TO_PTR(ndarray_copy(self_in));
            uint8_t *array = (uint8_t *)ndarray->array->items;
            for(size_t i=0; i < self->bytes; i++) array[i] = ~array[i];
            return MP_OBJ_FROM_PTR(ndarray);
            break;
        
        case MP_UNARY_OP_NEGATIVE:
            ndarray = MP_OBJ_TO_PTR(ndarray_copy(self_in));
            if(self->array->typecode == NDARRAY_UINT8) {
                uint8_t *array = (uint8_t *)ndarray->array->items;
                for(size_t i=0; i < self->array->len; i++) array[i] = -array[i];
            } else if(self->array->typecode == NDARRAY_INT8) {
                int8_t *array = (int8_t *)ndarray->array->items;
                for(size_t i=0; i < self->array->len; i++) array[i] = -array[i];
            } else if(self->array->typecode == NDARRAY_UINT16) {                
                uint16_t *array = (uint16_t *)ndarray->array->items;
                for(size_t i=0; i < self->array->len; i++) array[i] = -array[i];
            } else if(self->array->typecode == NDARRAY_INT16) {
                int16_t *array = (int16_t *)ndarray->array->items;
                for(size_t i=0; i < self->array->len; i++) array[i] = -array[i];
            } else {
                float *array = (float *)ndarray->array->items;
                for(size_t i=0; i < self->array->len; i++) array[i] = -array[i];
            }
            return MP_OBJ_FROM_PTR(ndarray);
            break;

        case MP_UNARY_OP_POSITIVE:
            return ndarray_copy(self_in);

        case MP_UNARY_OP_ABS:
            if((self->array->typecode == NDARRAY_UINT8) || (self->array->typecode == NDARRAY_UINT16)) {
                return ndarray_copy(self_in);
            }
            ndarray = MP_OBJ_TO_PTR(ndarray_copy(self_in));
            if((self->array->typecode == NDARRAY_INT8)) {
                int8_t *array = (int8_t *)ndarray->array->items;
                for(size_t i=0; i < self->array->len; i++) {
                    if(array[i] < 0) array[i] = -array[i];
                }
            } else if((self->array->typecode == NDARRAY_INT16)) {
                int16_t *array = (int16_t *)ndarray->array->items;
                for(size_t i=0; i < self->array->len; i++) {
                    if(array[i] < 0) array[i] = -array[i];
                }
            } else {
                float *array = (float *)ndarray->array->items;
                for(size_t i=0; i < self->array->len; i++) {
                    if(array[i] < 0) array[i] = -array[i];
                }                
            }
            return MP_OBJ_FROM_PTR(ndarray);
            break;
        default: return MP_OBJ_NULL; // operator not supported
    }
}

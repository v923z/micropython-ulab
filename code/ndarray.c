
/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#include <unistd.h>
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

mp_float_t ndarray_get_float_value(void *data, uint8_t typecode, size_t index) {
    if(typecode == NDARRAY_UINT8) {
        return (mp_float_t)((uint8_t *)data)[index];
    } else if(typecode == NDARRAY_INT8) {
        return (mp_float_t)((int8_t *)data)[index];
    } else if(typecode == NDARRAY_UINT16) {
        return (mp_float_t)((uint16_t *)data)[index];
    } else if(typecode == NDARRAY_INT16) {
        return (mp_float_t)((int16_t *)data)[index];
    } else {
        return (mp_float_t)((mp_float_t *)data)[index];
    }
}

void fill_array_iterable(mp_float_t *array, mp_obj_t iterable) {
    mp_obj_iter_buf_t x_buf;
    mp_obj_t x_item, x_iterable = mp_getiter(iterable, &x_buf);
    size_t i=0;
    while ((x_item = mp_iternext(x_iterable)) != MP_OBJ_STOP_ITERATION) {
        array[i] = (mp_float_t)mp_obj_get_float(x_item);
        i++;
    }
}

void ndarray_print_row(const mp_print_t *print, mp_obj_array_t *data, size_t n0, size_t n) {
    mp_print_str(print, "[");
    if(n < PRINT_MAX) { // if the array is short, print everything
        mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0), PRINT_REPR);
        for(size_t i=1; i<n; i++) {
            mp_print_str(print, ", ");
            mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0+i), PRINT_REPR);
        }
    } else {
        mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0), PRINT_REPR);
        for(size_t i=1; i<3; i++) {
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
    
    if(self->array->len == 0) {
        mp_print_str(print, "[]");
    } else {
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
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = NDARRAY_FLOAT } },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    uint8_t dtype = args[1].u_int;
    return dtype;
}

STATIC mp_obj_t ndarray_make_new_core(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    uint8_t dtype = ndarray_init_helper(n_args, args, kw_args);
    
	if(MP_OBJ_IS_TYPE(args[0], &ulab_ndarray_type)) {
		ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0]);
		if(dtype == ndarray->array->typecode) {
			return ndarray_copy(args[0]);
		}
		ndarray_obj_t *ndarray_new = create_new_ndarray(ndarray->m, ndarray->n, dtype);
		mp_obj_t item;
		if((ndarray->array->typecode == NDARRAY_FLOAT) &&(dtype != NDARRAY_FLOAT)) {
			for(size_t i=0; i < ndarray->array->len; i++) {
				mp_float_t f = ndarray_get_float_value(ndarray->array->items, ndarray->array->typecode, i);
				item = mp_obj_new_int((int32_t)MICROPY_FLOAT_C_FUN(floor)(f));
				mp_binary_set_val_array(dtype, ndarray_new->array->items, i, item);
			}
		} else {
			for(size_t i=0; i < ndarray->array->len; i++) {
				item = mp_binary_get_val_array(ndarray->array->typecode, ndarray->array->items, i);
				mp_binary_set_val_array(dtype, ndarray_new->array->items, i, item);
			}
		}
		return MP_OBJ_FROM_PTR(ndarray_new);
	}

    size_t len1, len2 = 0, i = 0;
    mp_obj_t len_in = mp_obj_len_maybe(args[0]);
    if (len_in == MP_OBJ_NULL) {
        mp_raise_ValueError(translate("first argument must be an iterable"));
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
                if(len2 != (size_t)MP_OBJ_SMALL_INT_VALUE(len_in)) {
                    mp_raise_ValueError(translate("iterables are not of the same length"));
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

#ifdef CIRCUITPY
mp_obj_t ndarray_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    (void) type;
    mp_arg_check_num(n_args, kw_args, 1, 2, true);
    size_t n_kw = 0;
    if (kw_args != 0) {
        n_kw = kw_args->used;
    }
    mp_map_init_fixed_table(kw_args, n_kw, args + n_args);
    return ndarray_make_new_core(n_args, args, kw_args);
}
#else
mp_obj_t ndarray_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void) type;
    mp_arg_check_num(n_args, n_kw, 1, 2, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    return ndarray_make_new_core(n_args, args, &kw_args);
}
#endif

static size_t slice_length(mp_bound_slice_t slice) {
    ssize_t len, correction = 1;
    if(slice.step > 0) correction = -1;
    len = (ssize_t)(slice.stop - slice.start + (slice.step + correction)) / slice.step;
    if(len < 0) return 0;
    return (size_t)len;
}

static size_t true_length(mp_obj_t bool_list) {
    // returns the number of Trues in a Boolean list
    // I wonder, wouldn't this be faster, if we looped through bool_list->items instead?
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t item, iterable = mp_getiter(bool_list, &iter_buf);
    size_t trues = 0;
    while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        if(!mp_obj_is_bool(item)) {
                // numpy seems to be a little bit inconsistent in when an index is considered
                // to be True/False. Bail out immediately, if the items are not True/False
                mp_raise_TypeError(translate("wrong index type"));
        }
        if(mp_obj_is_true(item)) {
            trues++;
        }
    }
    return trues;
}

static mp_bound_slice_t generate_slice(mp_int_t n, mp_obj_t index) {
    // micropython seems to have difficulties with negative steps
    mp_bound_slice_t slice;
    if(MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
        mp_obj_slice_indices(index, n, &slice);
    } else if(MP_OBJ_IS_INT(index)) {
        mp_int_t _index = mp_obj_get_int(index);
        if(_index < 0) {
            _index += n;
        } 
        if((_index >= n) || (_index < 0)) {
            mp_raise_msg(&mp_type_IndexError, translate("index is out of bounds"));
        }
        slice.start = _index;
        slice.stop = _index + 1;
        slice.step = 1;
    } else {
        mp_raise_msg(&mp_type_IndexError, translate("indices must be integers, slices, or Boolean lists"));
    }
    return slice;
}

static mp_bound_slice_t simple_slice(int16_t start, int16_t stop, int16_t step) {
    mp_bound_slice_t slice;
    slice.start = start;
    slice.stop = stop;
    slice.step = step;
    return slice;
}

static void insert_binary_value(ndarray_obj_t *ndarray, size_t nd_index, ndarray_obj_t *values, size_t value_index) {
    // there is probably a more elegant implementation...
    mp_obj_t tmp = mp_binary_get_val_array(values->array->typecode, values->array->items, value_index);
    if((values->array->typecode == NDARRAY_FLOAT) && (ndarray->array->typecode != NDARRAY_FLOAT)) {
        // workaround: rounding seems not to work in the arm compiler
        int32_t x = (int32_t)floorf(mp_obj_get_float(tmp)+0.5);
        tmp = mp_obj_new_int(x);
    }
    mp_binary_set_val_array(ndarray->array->typecode, ndarray->array->items, nd_index, tmp); 
}

static mp_obj_t insert_slice_list(ndarray_obj_t *ndarray, size_t m, size_t n, 
                            mp_bound_slice_t row, mp_bound_slice_t column, 
                            mp_obj_t row_list, mp_obj_t column_list, 
                            ndarray_obj_t *values) {
    if((m != values->m) && (n != values->n)) {
        if(values->array->len != 1) { // not a single item
            mp_raise_ValueError(translate("could not broadast input array from shape"));
        }
    }
    size_t cindex, rindex;
    // M, and N are used to manipulate how the source index is incremented in the loop
    uint8_t M = 1, N = 1;
    if(values->m == 1) {
        M = 0;
    }
    if(values->n == 1) {
        N = 0;
    }
    
    if(row_list == mp_const_none) { // rows are indexed by a slice
        rindex = row.start;
        if(column_list == mp_const_none) { // columns are indexed by a slice
            for(size_t i=0; i < m; i++) {
                cindex = column.start;
                for(size_t j=0; j < n; j++) {
                    insert_binary_value(ndarray, rindex*ndarray->n+cindex, values, i*M*n+j*N);
                    cindex += column.step;
                }
                rindex += row.step;
            }
        } else { // columns are indexed by a Boolean list
            mp_obj_iter_buf_t column_iter_buf;
            mp_obj_t column_item, column_iterable;
            for(size_t i=0; i < m; i++) {
                column_iterable = mp_getiter(column_list, &column_iter_buf);
                size_t j = 0;
                cindex = 0;
                while((column_item = mp_iternext(column_iterable)) != MP_OBJ_STOP_ITERATION) {
                    if(mp_obj_is_true(column_item)) {
                        insert_binary_value(ndarray, rindex*ndarray->n+cindex, values, i*M*n+j*N);
                        j++;
                    }
                    cindex++;
                }
                rindex += row.step;
            }
        }
    } else { // rows are indexed by a Boolean list
        mp_obj_iter_buf_t row_iter_buf;
        mp_obj_t row_item, row_iterable;
        row_iterable = mp_getiter(row_list, &row_iter_buf);
        size_t i = 0;
        rindex = 0;
        if(column_list == mp_const_none) { // columns are indexed by a slice
            while((row_item = mp_iternext(row_iterable)) != MP_OBJ_STOP_ITERATION) {
                if(mp_obj_is_true(row_item)) {
                    cindex = column.start;
                    for(size_t j=0; j < n; j++) {
                        insert_binary_value(ndarray, rindex*ndarray->n+cindex, values, i*M*n+j*N);
                        cindex += column.step;
                    }
                    i++;
                }
                rindex++;
            } 
        } else { // columns are indexed by a list
            mp_obj_iter_buf_t column_iter_buf;
            mp_obj_t column_item, column_iterable;
            size_t j = 0;
            cindex = 0;
            while((row_item = mp_iternext(row_iterable)) != MP_OBJ_STOP_ITERATION) {
                if(mp_obj_is_true(row_item)) {
                    column_iterable = mp_getiter(column_list, &column_iter_buf);                   
                    while((column_item = mp_iternext(column_iterable)) != MP_OBJ_STOP_ITERATION) {
                        if(mp_obj_is_true(column_item)) {
                            insert_binary_value(ndarray, rindex*ndarray->n+cindex, values, i*M*n+j*N);
                            j++;
                        }
                        cindex++;
                    }
                    i++;
                }
                rindex++;
            }
        }
    }
    return mp_const_none;
}

static mp_obj_t iterate_slice_list(ndarray_obj_t *ndarray, size_t m, size_t n, 
                            mp_bound_slice_t row, mp_bound_slice_t column, 
                            mp_obj_t row_list, mp_obj_t column_list, 
                            ndarray_obj_t *values) {
    if(values != NULL) {
        return insert_slice_list(ndarray, m, n, row, column, row_list, column_list, values);
    }
    uint8_t _sizeof = mp_binary_get_size('@', ndarray->array->typecode, NULL);
    ndarray_obj_t *out = create_new_ndarray(m, n, ndarray->array->typecode);
    uint8_t *target = (uint8_t *)out->array->items;
    uint8_t *source = (uint8_t *)ndarray->array->items;
    size_t cindex, rindex;    
    if(row_list == mp_const_none) { // rows are indexed by a slice
        rindex = row.start;
        if(column_list == mp_const_none) { // columns are indexed by a slice
            for(size_t i=0; i < m; i++) {
                cindex = column.start;
                for(size_t j=0; j < n; j++) {
                    memcpy(target+(i*n+j)*_sizeof, source+(rindex*ndarray->n+cindex)*_sizeof, _sizeof);
                    cindex += column.step;
                }
                rindex += row.step;
            }
        } else { // columns are indexed by a Boolean list
            // TODO: the list must be exactly as long as the axis
            mp_obj_iter_buf_t column_iter_buf;
            mp_obj_t column_item, column_iterable;
            for(size_t i=0; i < m; i++) {
                column_iterable = mp_getiter(column_list, &column_iter_buf);
                size_t j = 0;
                cindex = 0;
                while((column_item = mp_iternext(column_iterable)) != MP_OBJ_STOP_ITERATION) {
                    if(mp_obj_is_true(column_item)) {
                        memcpy(target+(i*n+j)*_sizeof, source+(rindex*ndarray->n+cindex)*_sizeof, _sizeof);
                        j++;
                    }
                    cindex++;
                }
                rindex += row.step;
            }
        }
    } else { // rows are indexed by a Boolean list
        mp_obj_iter_buf_t row_iter_buf;
        mp_obj_t row_item, row_iterable;
        row_iterable = mp_getiter(row_list, &row_iter_buf);
        size_t i = 0;
        rindex = 0;
        if(column_list == mp_const_none) { // columns are indexed by a slice
            while((row_item = mp_iternext(row_iterable)) != MP_OBJ_STOP_ITERATION) {
                if(mp_obj_is_true(row_item)) {
                    cindex = column.start;
                    for(size_t j=0; j < n; j++) {
                        memcpy(target+(i*n+j)*_sizeof, source+(rindex*ndarray->n+cindex)*_sizeof, _sizeof);
                        cindex += column.step;
                    }
                    i++;
                }
                rindex++;
            } 
        } else { // columns are indexed by a list
            mp_obj_iter_buf_t column_iter_buf;
            mp_obj_t column_item, column_iterable;
            size_t j = 0;
            cindex = 0;
            while((row_item = mp_iternext(row_iterable)) != MP_OBJ_STOP_ITERATION) {
                if(mp_obj_is_true(row_item)) {
                    column_iterable = mp_getiter(column_list, &column_iter_buf);                   
                    while((column_item = mp_iternext(column_iterable)) != MP_OBJ_STOP_ITERATION) {
                        if(mp_obj_is_true(column_item)) {
                            memcpy(target+(i*n+j)*_sizeof, source+(rindex*ndarray->n+cindex)*_sizeof, _sizeof);
                            j++;
                        }
                        cindex++;
                    }
                    i++;
                }
                rindex++;
            }
        }
    }
    return MP_OBJ_FROM_PTR(out);
}

static mp_obj_t ndarray_get_slice(ndarray_obj_t *ndarray, mp_obj_t index, ndarray_obj_t *values) {
    mp_bound_slice_t row_slice = simple_slice(0, 0, 1), column_slice = simple_slice(0, 0, 1);

    size_t m = 0, n = 0;
    if(MP_OBJ_IS_INT(index) && (ndarray->m == 1) && (values == NULL)) { 
        // we have a row vector, and don't want to assign
        column_slice = generate_slice(ndarray->n, index);
        if(slice_length(column_slice) == 1) { // we were asked for a single item
            // subscribe returns an mp_obj_t, if and only, if the index is an integer, and we have a row vector
            return mp_binary_get_val_array(ndarray->array->typecode, ndarray->array->items, column_slice.start);
        }
    }
    
    if(MP_OBJ_IS_INT(index) || MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
        if(ndarray->m == 1) { // we have a row vector
            column_slice = generate_slice(ndarray->n, index);
            row_slice = simple_slice(0, 1, 1);
        } else { // we have a matrix
            row_slice = generate_slice(ndarray->m, index);
            column_slice = simple_slice(0, ndarray->n, 1); // take all columns
        }
        m = slice_length(row_slice);
        n = slice_length(column_slice);
        return iterate_slice_list(ndarray, m, n, row_slice, column_slice, mp_const_none, mp_const_none, values);
    } else if(MP_OBJ_IS_TYPE(index, &mp_type_list)) {
        n = true_length(index);
        if(ndarray->m == 1) { // we have a flat array
            // we might have to separate the n == 1 case
            row_slice = simple_slice(0, 1, 1);
            return iterate_slice_list(ndarray, 1, n, row_slice, column_slice, mp_const_none, index, values);
        } else { // we have a matrix
            return iterate_slice_list(ndarray, 1, n, row_slice, column_slice, mp_const_none, index, values);
        }
    }
    else { // we certainly have a tuple, so let us deal with it
        mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(index);
        if(tuple->len != 2) {
            mp_raise_msg(&mp_type_IndexError, translate("too many indices"));
        }
        if(!(MP_OBJ_IS_TYPE(tuple->items[0], &mp_type_list) || 
            MP_OBJ_IS_TYPE(tuple->items[0], &mp_type_slice) || 
            MP_OBJ_IS_INT(tuple->items[0])) || 
           !(MP_OBJ_IS_TYPE(tuple->items[1], &mp_type_list) || 
            MP_OBJ_IS_TYPE(tuple->items[1], &mp_type_slice) || 
            MP_OBJ_IS_INT(tuple->items[1]))) {
                mp_raise_msg(&mp_type_IndexError, translate("indices must be integers, slices, or Boolean lists"));
        }
        if(MP_OBJ_IS_TYPE(tuple->items[0], &mp_type_list)) { // rows are indexed by Boolean list
            m = true_length(tuple->items[0]);
            if(MP_OBJ_IS_TYPE(tuple->items[1], &mp_type_list)) {
                n = true_length(tuple->items[1]);
                return iterate_slice_list(ndarray, m, n, row_slice, column_slice, 
                                          tuple->items[0], tuple->items[1], values);
            } else { // the column is indexed by an integer, or a slice
                column_slice = generate_slice(ndarray->n, tuple->items[1]);
                n = slice_length(column_slice);
                return iterate_slice_list(ndarray, m, n, row_slice, column_slice, 
                                          tuple->items[0], mp_const_none, values);
            }
            
        } else { // rows are indexed by a slice, or an integer
            row_slice = generate_slice(ndarray->m, tuple->items[0]);
            m = slice_length(row_slice);
            if(MP_OBJ_IS_TYPE(tuple->items[1], &mp_type_list)) { // columns are indexed by a Boolean list
                n = true_length(tuple->items[1]);
                return iterate_slice_list(ndarray, m, n, row_slice, column_slice, 
                                         mp_const_none, tuple->items[1], values);
            } else { // columns are indexed by an integer, or a slice
                column_slice = generate_slice(ndarray->n, tuple->items[1]);
                n = slice_length(column_slice);
                return iterate_slice_list(ndarray, m, n, row_slice, column_slice, 
                                          mp_const_none, mp_const_none, values);             
                
            }
        }
    }
}

mp_obj_t ndarray_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    
    if (value == MP_OBJ_SENTINEL) { // return value(s)
        return ndarray_get_slice(self, index, NULL);    
    } else { // assignment to slices; the value must be an ndarray, or a scalar
        if(!MP_OBJ_IS_TYPE(value, &ulab_ndarray_type) && 
          !MP_OBJ_IS_INT(value) && !mp_obj_is_float(value)) {
            mp_raise_ValueError(translate("right hand side must be an ndarray, or a scalar"));
        } else {
            ndarray_obj_t *values = NULL;
            if(MP_OBJ_IS_INT(value)) {
                values = create_new_ndarray(1, 1, self->array->typecode);
                mp_binary_set_val_array(values->array->typecode, values->array->items, 0, value);   
            } else if(mp_obj_is_float(value)) {
                values = create_new_ndarray(1, 1, NDARRAY_FLOAT);
                mp_binary_set_val_array(NDARRAY_FLOAT, values->array->items, 0, value);
            } else {
                values = MP_OBJ_TO_PTR(value);
            }
            return ndarray_get_slice(self, index, values);
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

static mp_obj_t ndarray_iternext(mp_obj_t self_in) {
    mp_obj_ndarray_it_t *self = MP_OBJ_TO_PTR(self_in);
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(self->ndarray);
    // TODO: in numpy, ndarrays are iterated with respect to the first axis. 
    size_t iter_end = 0;
    if(ndarray->m == 1) {
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

mp_obj_t ndarray_size(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->array->len);
}

mp_obj_t ndarray_itemsize(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(mp_binary_get_size('@', self->array->typecode, NULL));
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
        mp_raise_ValueError(translate("flattening order must be either 'C', or 'F'"));
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
static ndarray_obj_t *ndarray_from_mp_obj(mp_obj_t obj) {
	// creates an ndarray from an micropython int or float
	// if the input is an ndarray, it is returned
	ndarray_obj_t *ndarray;
	if(MP_OBJ_IS_INT(obj)) {
		int32_t ivalue = mp_obj_get_int(obj);
		if((ivalue > 0) && (ivalue < 256)) {
			CREATE_SINGLE_ITEM(ndarray, uint8_t, NDARRAY_UINT8, ivalue);
		} else if((ivalue > 255) && (ivalue < 65535)) {
			CREATE_SINGLE_ITEM(ndarray, uint16_t, NDARRAY_UINT16, ivalue);
		} else if((ivalue < 0) && (ivalue > -128)) {
			CREATE_SINGLE_ITEM(ndarray, int8_t, NDARRAY_INT8, ivalue);
		} else if((ivalue < -127) && (ivalue > -32767)) {
			CREATE_SINGLE_ITEM(ndarray, int16_t, NDARRAY_INT16, ivalue);
		} else { // the integer value clearly does not fit the ulab types, so move on to float
			CREATE_SINGLE_ITEM(ndarray, mp_float_t, NDARRAY_FLOAT, ivalue);
		}
	} else if(mp_obj_is_float(obj)) {
		mp_float_t fvalue = mp_obj_get_float(obj);
		CREATE_SINGLE_ITEM(ndarray, mp_float_t, NDARRAY_FLOAT, fvalue);
	} else if(MP_OBJ_IS_TYPE(obj, &ulab_ndarray_type)){
		ndarray = MP_OBJ_TO_PTR(obj);
	} else {
		mp_raise_TypeError(translate("wrong operand type"));
	}
	return ndarray;
}

mp_obj_t ndarray_binary_op(mp_binary_op_t _op, mp_obj_t lhs, mp_obj_t rhs) {
	// if the ndarray stands on the right hand side of the expression, simply swap the operands
	ndarray_obj_t *ol, *or;
	mp_binary_op_t op = _op;
	if((op == MP_BINARY_OP_REVERSE_ADD) || (op == MP_BINARY_OP_REVERSE_MULTIPLY) || 
		(op == MP_BINARY_OP_REVERSE_POWER) || (op == MP_BINARY_OP_REVERSE_SUBTRACT) || 
		(op == MP_BINARY_OP_REVERSE_TRUE_DIVIDE)) {
		ol = ndarray_from_mp_obj(rhs);
		or = ndarray_from_mp_obj(lhs);		
	} else {
		ol = ndarray_from_mp_obj(lhs);
		or = ndarray_from_mp_obj(rhs);
	}
	if(op == MP_BINARY_OP_REVERSE_ADD) {
		op = MP_BINARY_OP_ADD;
	} else if(op == MP_BINARY_OP_REVERSE_MULTIPLY) {
		op = MP_BINARY_OP_MULTIPLY;
	} else if(op == MP_BINARY_OP_REVERSE_POWER) {
		op = MP_BINARY_OP_POWER;
	} else if(op == MP_BINARY_OP_REVERSE_SUBTRACT) {
		op = MP_BINARY_OP_SUBTRACT;
	} else if(op == MP_BINARY_OP_REVERSE_TRUE_DIVIDE) {
		op = MP_BINARY_OP_TRUE_DIVIDE;
	}
    // One of the operands is a scalar
    // TODO: conform to numpy with the upcasting
    // TODO: implement in-place operators
	// these are partial broadcasting rules: either the two arrays 
	// are of the same shape, or one of them is of length 1
	if(((ol->m != or->m) || (ol->n != or->n))) {
		if((ol->array->len != 1) && (or->array->len != 1)) {
			if(op == MP_BINARY_OP_EQUAL) {
				return mp_const_false;
			} else if(op == MP_BINARY_OP_NOT_EQUAL) {
				return mp_const_true;
			}
            mp_raise_ValueError(translate("operands could not be broadcast together"));
		}
	}
	uint8_t linc = ol->array->len == 1 ? 0 : 1;
	uint8_t rinc = or->array->len == 1 ? 0 : 1;
	// do the partial broadcasting here
	size_t m = MAX(ol->m, or->m);
	size_t n = MAX(ol->n, or->n);
	size_t len = MAX(ol->array->len, or->array->len);
	if((ol->array->len == 0) || (or->array->len == 0)) {
		len = 0;
	}
	switch(op) {
		case MP_BINARY_OP_EQUAL:
		case MP_BINARY_OP_NOT_EQUAL:
		case MP_BINARY_OP_LESS:
		case MP_BINARY_OP_LESS_EQUAL:
		case MP_BINARY_OP_MORE:
		case MP_BINARY_OP_MORE_EQUAL:
		case MP_BINARY_OP_ADD:
		case MP_BINARY_OP_SUBTRACT:
		case MP_BINARY_OP_TRUE_DIVIDE:
		case MP_BINARY_OP_MULTIPLY:
		case MP_BINARY_OP_POWER:       
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
					RUN_BINARY_LOOP(NDARRAY_UINT8, uint8_t, uint8_t, uint8_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_INT8) {
					RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, uint8_t, int8_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_UINT16) {
					RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint8_t, uint16_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_INT16) {
					RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, uint8_t, int16_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_FLOAT) {
					RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, uint8_t, mp_float_t, ol, or, op, m, n, len, linc, rinc);
				}
			} else if(ol->array->typecode == NDARRAY_INT8) {
				if(or->array->typecode == NDARRAY_UINT8) {
					RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int8_t, uint8_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_INT8) {
					RUN_BINARY_LOOP(NDARRAY_INT8, int8_t, int8_t, int8_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_UINT16) {
					RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int8_t, uint16_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_INT16) {
					RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int8_t, int16_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_FLOAT) {
					RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, int8_t, mp_float_t, ol, or, op, m, n, len, linc, rinc);
				}                
			} else if(ol->array->typecode == NDARRAY_UINT16) {
				if(or->array->typecode == NDARRAY_UINT8) {
					RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, uint8_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_INT8) {
					RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, int8_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_UINT16) {
					RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, uint16_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_INT16) {
					RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, uint16_t, int16_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_FLOAT) {
					RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, uint8_t, mp_float_t, ol, or, op, m, n, len, linc, rinc);
				}
			} else if(ol->array->typecode == NDARRAY_INT16) {
				if(or->array->typecode == NDARRAY_UINT8) {
					RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int16_t, uint8_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_INT8) {
					RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int16_t, int8_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_UINT16) {
					RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, int16_t, uint16_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_INT16) {
					RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int16_t, int16_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_FLOAT) {
					RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, uint16_t, mp_float_t, ol, or, op, m, n, len, linc, rinc);
				}
			} else if(ol->array->typecode == NDARRAY_FLOAT) {
				if(or->array->typecode == NDARRAY_UINT8) {
					RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, uint8_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_INT8) {
					RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, int8_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_UINT16) {
					RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, uint16_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_INT16) {
					RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, int16_t, ol, or, op, m, n, len, linc, rinc);
				} else if(or->array->typecode == NDARRAY_FLOAT) {
					RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, mp_float_t, ol, or, op, m, n, len, linc, rinc);
				}
			}
			// this instruction should never be reached, but we have to make the compiler happy
			return MP_OBJ_NULL;
			break;
		default:
			return MP_OBJ_NULL; // op not supported
			break;
	}
	return MP_OBJ_NULL;
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
                mp_raise_ValueError(translate("operation is not supported for given type"));
            }
            // we can invert the content byte by byte, there is no need to distinguish 
            // between different typecodes
            ndarray = MP_OBJ_TO_PTR(ndarray_copy(self_in));
            {
                uint8_t *array = (uint8_t *)ndarray->array->items;
                for(size_t i=0; i < self->bytes; i++) array[i] = ~array[i];
                return MP_OBJ_FROM_PTR(ndarray);
            }
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
                mp_float_t *array = (mp_float_t *)ndarray->array->items;
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
            if(self->array->typecode == NDARRAY_INT8) {
                int8_t *array = (int8_t *)ndarray->array->items;
                for(size_t i=0; i < self->array->len; i++) {
                    if(array[i] < 0) array[i] = -array[i];
                }
            } else if(self->array->typecode == NDARRAY_INT16) {
                int16_t *array = (int16_t *)ndarray->array->items;
                for(size_t i=0; i < self->array->len; i++) {
                    if(array[i] < 0) array[i] = -array[i];
                }
            } else {
                mp_float_t *array = (mp_float_t *)ndarray->array->items;
                for(size_t i=0; i < self->array->len; i++) {
                    if(array[i] < 0) array[i] = -array[i];
                }                
            }
            return MP_OBJ_FROM_PTR(ndarray);
            break;
        default: return MP_OBJ_NULL; // operator not supported
    }
}

mp_obj_t ndarray_transpose(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // the size of a single item in the array
    uint8_t _sizeof = mp_binary_get_size('@', self->array->typecode, NULL);
    
    // NOTE: 
    // if the matrices are square, we can simply swap items, but 
    // generic matrices can't be transposed in place, so we have to 
    // declare a temporary variable
    
    // NOTE: 
    //  In the old matrix, the coordinate (m, n) is m*self->n + n
    //  We have to assign this to the coordinate (n, m) in the new 
    //  matrix, i.e., to n*self->m + m (since the new matrix has self->m columns)
    
    // one-dimensional arrays can be transposed by simply swapping the dimensions
    if((self->m != 1) && (self->n != 1)) {
        uint8_t *c = (uint8_t *)self->array->items;
        // self->bytes is the size of the bytearray, irrespective of the typecode
        uint8_t *tmp = m_new(uint8_t, self->bytes);
        for(size_t m=0; m < self->m; m++) {
            for(size_t n=0; n < self->n; n++) {
                memcpy(tmp+_sizeof*(n*self->m + m), c+_sizeof*(m*self->n + n), _sizeof);
            }
        }
        memcpy(self->array->items, tmp, self->bytes);
        m_del(uint8_t, tmp, self->bytes);
    } 
    SWAP(size_t, self->m, self->n);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(ndarray_transpose_obj, ndarray_transpose);

mp_obj_t ndarray_reshape(mp_obj_t self_in, mp_obj_t shape) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if(!MP_OBJ_IS_TYPE(shape, &mp_type_tuple) || (MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(shape)) != 2)) {
        mp_raise_ValueError(translate("shape must be a 2-tuple"));
    }

    mp_obj_iter_buf_t iter_buf;
    mp_obj_t item, iterable = mp_getiter(shape, &iter_buf);
    uint16_t m, n;
    item = mp_iternext(iterable);
    m = mp_obj_get_int(item);
    item = mp_iternext(iterable);
    n = mp_obj_get_int(item);
    if(m*n != self->m*self->n) {
        // TODO: the proper error message would be "cannot reshape array of size %d into shape (%d, %d)"
        mp_raise_ValueError(translate("cannot reshape array (incompatible input/output shape)"));
    }
    self->m = m;
    self->n = n;
    return MP_OBJ_FROM_PTR(self);
}

MP_DEFINE_CONST_FUN_OBJ_2(ndarray_reshape_obj, ndarray_reshape);

mp_int_t ndarray_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // buffer_p.get_buffer() returns zero for success, while mp_get_buffer returns true for success
    return !mp_get_buffer(self->array, bufinfo, flags);
}

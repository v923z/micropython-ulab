
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

mp_uint_t ndarray_print_threshold = NDARRAY_PRINT_THRESHOLD;
mp_uint_t ndarray_print_edgeitems = NDARRAY_PRINT_EDGEITEMS;

//| """Manipulate numeric data similar to numpy
//| `ulab` is a numpy-like module for micropython, meant to simplify and
//| speed up common mathematical operations on arrays. The primary goal was to
//| implement a small subset of numpy that might be useful in the context of a
//| microcontroller. This means low-level data processing of linear (array) and
//| two-dimensional (matrix) data.
//| `ulab` is adapted from micropython-ulab, and the original project's
//| documentation can be found at
//| https://micropython-ulab.readthedocs.io/en/latest/
//| `ulab` is modeled after numpy, and aims to be a compatible subset where
//| possible.  Numpy's documentation can be found at
//| https://docs.scipy.org/doc/numpy/index.html"""
//|
//| class array:
//|     """1- and 2- dimensional array"""
//|     def __init__(self, values, *, dtype=float):
//|         """:param sequence values: Sequence giving the initial content of the array.
//|           :param dtype: The type of array values, ``int8``, ``uint8``, ``int16``, ``uint16``, or ``float``
//|
//|           The `values` sequence can either be another ~ulab.array, sequence of numbers
//|           (in which case a 1-dimensional array is created), or a sequence where each
//|           subsequence has the same length (in which case a 2-dimensional array is
//|           created).
//|           Passing a ~ulab.array and a different dtype can be used to convert an array
//|           from one dtype to another.
//|           In many cases, it is more convenient to create an array from a function
//|           like `zeros` or `linspace`.
//|           `ulab.array` implements the buffer protocol, so it can be used in many
//|           places an `array.array` can be used."""
//|         ...
//|
//|     shape: tuple = ...
//|     """The size of the array, a tuple of length 1 or 2"""
//|
//|     size: int = ...
//|     """The number of elements in the array"""
//|
//|     itemsize: int = ...
//|     """The number of elements in the array"""
//|
//|     def flatten(self, *, order='C'):
//|         """:param order: Whether to flatten by rows ('C') or columns ('F')
//|
//|            Returns a new `ulab.array` object which is always 1 dimensional.
//|            If order is 'C' (the default", then the data is ordered in rows;
//|            If it is 'F', then the data is ordered in columns.  "C" and "F" refer
//|            to the typical storage organization of the C and Fortran languages."""
//|         ...
//|
//|     def sort(self, *, axis=1):
//|         """:param axis: Whether to sort elements within rows (0), columns (1), or elements (None)"""
//|         ...
//|
//|     def transpose(self):
//|         """Swap the rows and columns of a 2-dimensional array"""
//|         ...
//|
//|     def  __add__(self):
//|         """Adds corresponding elements of the two arrays, or adds a number to all
//|            elements of the array.  If both arguments are arrays, their sizes must match."""
//|         ...
//|
//|     def  __sub__(self):
//|         """Subtracts corresponding elements of the two arrays, or adds a number to all
//|            elements of the array.  If both arguments are arrays, their sizes must match."""
//|         ...
//|
//|     def  __mul__(self):
//|         """Multiplies corresponding elements of the two arrays, or multiplies
//|            all elements of the array by a number.  If both arguments are arrays,
//|            their sizes must match."""
//|         ...
//|
//|     def __div__(self):
//|         """Multiplies corresponding elements of the two arrays, or divides
//|            all elements of the array by a number.  If both arguments are arrays,
//|            their sizes must match."""
//|         ...
//|
//|     def __pow__():
//|         """Computes the power (x**y) of corresponding elements of the the two arrays,
//|            or one number and one array.  If both arguments are arrays, their sizes
//|            must match."""
//|         ...
//|
//|     def __getitem__():
//|         """Retrieve an element of the array."""
//|         ...
//|
//|     def __setitem__():
//|         """Set an element of the array."""
//|         ...
//|
//| int8 = ...
//| """Type code for signed integers in the range -128 .. 127 inclusive, like the 'b' typecode of `array.array`"""
//|
//| int16 = ...
//| """Type code for signed integers in the range -32768 .. 32767 inclusive, like the 'h' typecode of `array.array`"""
//|
//| float = ...
//| """Type code for floating point values, like the 'f' typecode of `array.array`"""
//|
//| uint8 = ...
//| """Type code for unsigned integers in the range 0 .. 255 inclusive, like the 'H' typecode of `array.array`"""
//|
//| uint16 = ...
//| """Type code for unsigned integers in the range 0 .. 65535 inclusive, like the 'h' typecode of `array.array`"""
//|

#ifdef OPENMV
void mp_obj_slice_indices(mp_obj_t self_in, mp_int_t length, mp_bound_slice_t *result) {
    mp_obj_slice_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t start, stop, step;

    if (self->step == mp_const_none) {
        step = 1;
    } else {
        step = mp_obj_get_int(self->step);
        if (step == 0) {
            mp_raise_ValueError(translate("slice step can't be zero"));
        }
    }

    if (step > 0) {
        // Positive step
        if (self->start == mp_const_none) {
            start = 0;
        } else {
            start = mp_obj_get_int(self->start);
            if (start < 0) {
                start += length;
            }
            start = MIN(length, MAX(start, 0));
        }

        if (self->stop == mp_const_none) {
            stop = length;
        } else {
            stop = mp_obj_get_int(self->stop);
            if (stop < 0) {
                stop += length;
            }
            stop = MIN(length, MAX(stop, 0));
        }
    } else {
        // Negative step
        if (self->start == mp_const_none) {
            start = length - 1;
        } else {
            start = mp_obj_get_int(self->start);
            if (start < 0) {
                start += length;
            }
            start = MIN(length - 1, MAX(start, -1));
        }

        if (self->stop == mp_const_none) {
            stop = -1;
        } else {
            stop = mp_obj_get_int(self->stop);
            if (stop < 0) {
                stop += length;
            }
            stop = MIN(length - 1, MAX(stop, -1));
        }
    }

    result->start = start;
    result->stop = stop;
    result->step = step;
}
#endif

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

void ndarray_fill_array_iterable(mp_float_t *array, mp_obj_t iterable) {
    mp_obj_iter_buf_t x_buf;
    mp_obj_t x_item, x_iterable = mp_getiter(iterable, &x_buf);
    size_t i=0;
    while ((x_item = mp_iternext(x_iterable)) != MP_OBJ_STOP_ITERATION) {
        *array++ = (mp_float_t)mp_obj_get_float(x_item);
        i++;
    }
}

int32_t *strides_from_shape(size_t *shape, size_t n) {
    // returns a strides array that corresponds to a dense array with the prescribed shape
    int32_t *strides = m_new(int32_t, n);
    strides[n-1] = 1;
    for(uint8_t i=n-1; i > 0; i--) {
        strides[i-1] = strides[i] * shape[i];
    }
    return strides;
}

size_t *ndarray_new_coords(uint8_t ndim) {
    size_t *coords = m_new(size_t, ndim);
    memset(coords, 0, ndim*sizeof(size_t));
    return coords;
}

bool ndarray_object_is_nditerable(mp_obj_t o_in) {
    if(MP_OBJ_IS_TYPE(o_in, &ulab_ndarray_type) ||
      MP_OBJ_IS_TYPE(o_in, &mp_type_tuple) ||
      MP_OBJ_IS_TYPE(o_in, &mp_type_list) ||
      MP_OBJ_IS_TYPE(o_in, &mp_type_range)) {
        return true;
    }
    return false;
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

mp_obj_t ndarray_set_printoptions(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
	static const mp_arg_t allowed_args[] = {
		{ MP_QSTR_threshold, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
		{ MP_QSTR_edgeitems, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
	};

	mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
	mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
	if(args[0].u_rom_obj != mp_const_none) {
		ndarray_print_threshold = mp_obj_get_int(args[0].u_rom_obj);
	}
	if(args[1].u_rom_obj != mp_const_none) {
		ndarray_print_edgeitems = mp_obj_get_int(args[1].u_rom_obj);
	}
	return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(ndarray_set_printoptions_obj, 0, ndarray_set_printoptions);

mp_obj_t ndarray_get_printoptions(void) {
	mp_obj_t dict = mp_obj_new_dict(2);
	mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), MP_OBJ_NEW_QSTR(MP_QSTR_threshold), mp_obj_new_int(ndarray_print_threshold));
	mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), MP_OBJ_NEW_QSTR(MP_QSTR_edgeitems), mp_obj_new_int(ndarray_print_edgeitems));
    return dict;
}

MP_DEFINE_CONST_FUN_OBJ_0(ndarray_get_printoptions_obj, ndarray_get_printoptions);

void ndarray_print_row(const mp_print_t *print, mp_obj_array_t *data, size_t n0, size_t n) {
    mp_print_str(print, "[");
    if((n <= ndarray_print_threshold) || (n <= 2*ndarray_print_edgeitems)) { // if the array is short, print everything
        mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0), PRINT_REPR);
        for(size_t i=1; i < n; i++) {
            mp_print_str(print, ", ");
            mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0+i), PRINT_REPR);
        }
    } else {
        mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0), PRINT_REPR);
        for(size_t i=1; i < ndarray_print_edgeitems; i++) {
            mp_print_str(print, ", ");
            mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0+i), PRINT_REPR);
        }
        mp_printf(print, ", ..., ");
        mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0+n-ndarray_print_edgeitems), PRINT_REPR);
        for(size_t i=1; i < ndarray_print_edgeitems; i++) {
            mp_print_str(print, ", ");
            mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0+n-ndarray_print_edgeitems+i), PRINT_REPR);
        }
    }
    mp_print_str(print, "]");
}

void ndarray_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t print_extra = self->ndim;
    size_t *coords = ndarray_new_coords(self->ndim);
    int32_t last_stride = self->strides[self->ndim-1];
    
    size_t offset = 0;
    mp_print_str(print, "array(");
    if(self->len == 0) mp_print_str(print, "[");
    for(size_t i=0; i < self->len; i++) {
        for(uint8_t j=0; j < print_extra; j++) {
            mp_print_str(print, "[");
        }
        print_extra = 0;
        if(!self->boolean) {
            mp_obj_print_helper(print, mp_binary_get_val_array(self->dtype, self->array, offset), PRINT_REPR);
        } else {
            if(((uint8_t *)self->array)[offset]) {
                mp_print_str(print, "True");
            } else {
                mp_print_str(print, "False");
            }
        }
		offset += last_stride;
        coords[self->ndim-1] += 1;
        if(coords[self->ndim-1] != self->shape[self->ndim-1]) {
            mp_print_str(print, ", ");
        }
        for(uint8_t j=self->ndim-1; j > 0; j--) {
            if(coords[j] == self->shape[j]) {
                offset -= self->shape[j] * self->strides[j];
                offset += self->strides[j-1];
                print_extra += 1;
                coords[j] = 0;
                coords[j-1] += 1;
                mp_print_str(print, "]");
            } else { // coordinates can change only, if the last coordinate changes
                break;
            }
        }
        if(print_extra && (i != self->len-1)) {
            mp_print_str(print, "\n");
            if(print_extra > 1) {
                mp_print_str(print, "\n");
            }
        }
    }
    m_del(size_t, coords, self->ndim);
	mp_print_str(print, "]");
	if(self->boolean) {
		mp_print_str(print, ", dtype=bool)");
	} else if(self->dtype == NDARRAY_UINT8) {
		mp_print_str(print, ", dtype=uint8)");
	} else if(self->dtype == NDARRAY_INT8) {
		mp_print_str(print, ", dtype=int8)");
	} else if(self->dtype == NDARRAY_UINT16) {
		mp_print_str(print, ", dtype=uint16)");
	} else if(self->dtype == NDARRAY_INT16) {
		mp_print_str(print, ", dtype=int16)");
	} else if(self->dtype == NDARRAY_FLOAT) {
		mp_print_str(print, ", dtype=float)");
	}
}

void ndarray_assign_elements(ndarray_obj_t *ndarray, mp_obj_t iterable, uint8_t dtype, size_t *idx) {
    // assigns a single row in the matrix
    mp_obj_t item;
    uint8_t *array = (uint8_t *)ndarray->array;
    array += *idx;
	if(ndarray->boolean) {
	    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            // TODO: this might be wrong here: we have to check for the trueness of item
            if(mp_obj_is_true(item)) {
                *array = 1;
            }
            array++;
            (*idx)++;
        }
    } else {
        while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            mp_binary_set_val_array(dtype, ndarray->array, (*idx)++, item);
        }
    }
}

bool ndarray_is_dense(ndarray_obj_t *ndarray) {
	// returns true, if the array is dense, false otherwise
	// the array should dense, if the very first stride can be calculated from shape
	int32_t stride = 1;
	for(uint8_t i=0; i < ndarray->ndim; i++) {
        stride *= ndarray->shape[i];
    }
	return stride == ndarray->strides[0] ? true : false;
}

ndarray_obj_t *ndarray_new_ndarray(uint8_t ndim, size_t *shape, int32_t *strides, uint8_t dtype) {
    // Creates the base ndarray with shape, and initialises the values to straight 0s
    // the function should work in the general n-dimensional case
    ndarray_obj_t *ndarray = m_new_obj(ndarray_obj_t);
    ndarray->base.type = &ulab_ndarray_type;
    ndarray->dtype = dtype;
    ndarray->ndim = ndim;
	ndarray->len = 1;
    for(uint8_t i=0; i < ndim; i++) {
		ndarray->shape[i] = shape[i];
		ndarray->strides[i] = strides[i];
		ndarray->len *= shape[i];
	}
    if(dtype == NDARRAY_BOOL) {
        dtype = NDARRAY_UINT8;
        ndarray->boolean = NDARRAY_BOOLEAN;
    } else {
        ndarray->boolean = NDARRAY_NUMERIC;
    }
	uint8_t itemsize = mp_binary_get_size('@', dtype, NULL);
	uint8_t *array = m_new(byte, itemsize*ndarray->len);
    // this should set all elements to 0, irrespective of the of the dtype (all bits are zero)
    // we could, perhaps, leave this step out, and initialise the array only, when needed
    memset(array, 0, ndarray->len*itemsize);
    ndarray->array = array;
    return ndarray;
}

ndarray_obj_t *ndarray_new_dense_ndarray(uint8_t ndim, size_t *shape, uint8_t dtype) {
    // creates a dense array, i.e., one, where the strides are derived directly from the shapes
    // the function should work in the general n-dimensional case
    int32_t *strides = m_new(int32_t, ndim);
    strides[ndim-1] = 1;
    for(size_t i=ndim-1; i > 0; i--) {
        strides[i-1] = strides[i] * shape[i-1];
    }
    return ndarray_new_ndarray(ndim, shape, strides, dtype);
}

ndarray_obj_t *ndarray_new_ndarray_from_tuple(mp_obj_tuple_t *_shape, uint8_t dtype) {
    // creates a dense array from a tuple
    // the function should work in the general n-dimensional case
    uint8_t ndim = _shape->len;
    size_t *shape = m_new(size_t, ndim);
    for(size_t i=0; i < ndim; i++) {
		shape[i] = mp_obj_get_int(_shape->items[i]);
    }
    return ndarray_new_dense_ndarray(ndim, shape, dtype);
}

void ndarray_copy_array(ndarray_obj_t *source, ndarray_obj_t *target) {
	// copies the content of source->array into a new dense void pointer
	// it is assumed that the dtypes in source and target are the same
	size_t *coords = ndarray_new_coords(source->ndim);
	int32_t last_stride = source->strides[source->ndim-1];
	uint8_t itemsize = mp_binary_get_size('@', source->dtype, NULL);

	uint8_t *array = (uint8_t *)source->array;
	uint8_t *new_array = (uint8_t *)target->array;

	for(size_t i=0; i < source->len; i++) {
		memcpy(new_array, array, itemsize);
		new_array += itemsize;        
		array += last_stride*itemsize;
		coords[source->ndim-1] += 1;
		for(uint8_t j=source->ndim-1; j > 0; j--) {
			if(coords[j] == source->shape[j]) {
				array -= source->shape[j] * source->strides[j] * itemsize;
				array += source->strides[j-1] * itemsize;
				coords[j] = 0;
				coords[j-1] += 1;
			} else { // coordinates can change only, if the last coordinate changes
				break;
			}
		}
	}
	m_del(size_t, coords, source->ndim);
}

ndarray_obj_t *ndarray_new_view(ndarray_obj_t *source, uint8_t ndim, size_t *shape, int32_t *strides, int32_t offset) {
    // creates a new view from the input arguments
    // the function should work in the n-dimensional case
    ndarray_obj_t *ndarray = m_new_obj(ndarray_obj_t);
    ndarray->base.type = &ulab_ndarray_type;
    ndarray->boolean = source->boolean;
    ndarray->dtype = source->dtype;
    ndarray->ndim = ndim;
    ndarray->len = 1;
    for(uint8_t i=0; i < ndim; i++) {
		ndarray->shape[i] = shape[i];
		ndarray->strides[i] = strides[i];
        ndarray->len *= shape[i];
    }
	uint8_t itemsize = mp_binary_get_size('@', source->dtype, NULL);
    ndarray->array = (uint8_t *)source->array + offset * itemsize;
    return ndarray;
}

ndarray_obj_t *ndarray_copy_view(ndarray_obj_t *source) {
    // creates a one-to-one deep copy of the input ndarray or its view
    // the function should work in the general n-dimensional case
    // In order to make it dtype-agnostic, we copy the memory content 
    // instead of reading out the values
    
    int32_t *strides = strides_from_shape(source->shape, source->ndim);

    uint8_t dtype = source->dtype;
    if(source->boolean) {
        dtype = NDARRAY_BOOLEAN;
    }
    ndarray_obj_t *ndarray = ndarray_new_ndarray(source->ndim, source->shape, strides, dtype);
	ndarray_copy_array(source, ndarray);
    return ndarray;
}

ndarray_obj_t *ndarray_new_linear_array(size_t len, uint8_t dtype) {
    size_t *shape = m_new(size_t, 1);
    int32_t *strides = m_new(int32_t, 1);
    shape[0] = len;
    strides[0] = 1;
    return ndarray_new_ndarray(1, shape, strides, dtype);
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

STATIC mp_obj_t ndarray_make_new_core(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args, mp_map_t *kw_args) {
    uint8_t dtype = ndarray_init_helper(n_args, args, kw_args);

    mp_obj_t len_in = mp_obj_len_maybe(args[0]);
	size_t i = 0, len1 = 0, len2 = 0;
    if (len_in == MP_OBJ_NULL) {
        mp_raise_ValueError(translate("first argument must be an iterable"));
    } else {
		// len1 is either the number of rows (for matrices), or the number of elements (row vectors)
		len1 = MP_OBJ_SMALL_INT_VALUE(len_in);
    }
    
    ndarray_obj_t *self;

	// TODO: this doesn't allow dtype conversion. 
    if(MP_OBJ_IS_TYPE(args[0], &ulab_ndarray_type)) {
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0]);
        self = ndarray_copy_view(ndarray);
        return MP_OBJ_FROM_PTR(self);
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
    if(len2 == 0) {
		self = ndarray_new_linear_array(len1, dtype);
	} else {
		size_t shape[2] = {len1, len2};
		self = ndarray_new_dense_ndarray(2, shape, dtype);
	}
    
    size_t idx = 0;
    iterable1 = mp_getiter(args[0], &iter_buf1);
    if(len2 == 0) { // the first argument is a single iterable
        ndarray_assign_elements(self, iterable1, dtype, &idx);
    } else {
        mp_obj_iter_buf_t iter_buf2;
        mp_obj_t iterable2;
        while ((item1 = mp_iternext(iterable1)) != MP_OBJ_STOP_ITERATION) {
            iterable2 = mp_getiter(item1, &iter_buf2);
            ndarray_assign_elements(self, iterable2, dtype, &idx);
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
    return ndarray_make_new_core(type, n_args, n_kw, args, kw_args);
}
#else
mp_obj_t ndarray_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void) type;
    mp_arg_check_num(n_args, n_kw, 1, 2, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    return ndarray_make_new_core(type, n_args, n_kw, args, &kw_args);
}
#endif

#if 0
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
        int32_t x = (int32_t)MICROPY_FLOAT_C_FUN(floor)(mp_obj_get_float(tmp)+MICROPY_FLOAT_CONST(0.5));
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
#endif

// itarray iterator
mp_obj_t ndarray_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf) {
    return ndarray_new_ndarray_iterator(o_in, 0, iter_buf);
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
    size_t iter_end = ndarray->shape[0];
    if(self->cur < iter_end) {
        if(ndarray->ndim == 1) { // we have a linear array
            // read the current value
            self->cur++;
            return mp_binary_get_val_array(ndarray->dtype, ndarray->array, self->cur-1);
        } else { // we have a tensor, return the reduced view
            int32_t offset = self->cur * ndarray->strides[0];
            self->cur++;
            ndarray_obj_t *value = ndarray_new_view(ndarray, ndarray->ndim-1, ndarray->shape+1, ndarray->strides+1, offset);
            return MP_OBJ_FROM_PTR(value);
        }
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

mp_obj_t ndarray_new_ndarray_iterator(mp_obj_t ndarray, size_t cur, mp_obj_iter_buf_t *iter_buf) {
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
    mp_obj_t *items = m_new(mp_obj_t, self->ndim);
    for(uint8_t i=0; i < self->ndim; i++) {
        items[i] = mp_obj_new_int(self->shape[i]);
    }
    mp_obj_t tuple = mp_obj_new_tuple(self->ndim, items);
    m_del(mp_obj_t, items, self->ndim);
    return tuple;
}

mp_obj_t ndarray_size(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->len);
}

mp_obj_t ndarray_itemsize(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(mp_binary_get_size('@', self->dtype, NULL));
}

/*
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
*/
#if 0
// Binary operations
ndarray_obj_t *ndarray_from_mp_obj(mp_obj_t obj) {
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
					RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, uint16_t, mp_float_t, ol, or, op, m, n, len, linc, rinc);
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
#endif

mp_obj_t ndarray_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t itemsize = mp_binary_get_size('@', self->dtype, NULL);
    ndarray_obj_t *ndarray = NULL;
    switch (op) {
        case MP_UNARY_OP_LEN: 
            if(self->ndim > 1) {
                return mp_obj_new_int(self->ndim);
            } else {
                return mp_obj_new_int(self->len);
            }
            break;
        
        case MP_UNARY_OP_INVERT:
            if(self->dtype == NDARRAY_FLOAT) {
                mp_raise_ValueError(translate("operation is not supported for given type"));
            }
            // we can invert the content byte by byte, no need to distinguish between different dtypes
            ndarray = ndarray_copy_view(self); // from this point, this is a dense copy
            uint8_t *array = (uint8_t *)ndarray->array;
            if(ndarray->boolean) {
                for(size_t i=0; i < ndarray->len; i++, array++) *array = *array ^ 0x01;
            } else {
                for(size_t i=0; i < ndarray->len*itemsize; i++, array++) *array ^= 0xFF;
            }
            return MP_OBJ_FROM_PTR(ndarray);
            break;
        
        case MP_UNARY_OP_NEGATIVE:
            ndarray = ndarray_copy_view(self); // from this point, this is a dense copy
            if(self->dtype == NDARRAY_UINT8) {
                uint8_t *array = (uint8_t *)ndarray->array;
                for(size_t i=0; i < self->len; i++, array++) *array = -(*array);
            } else if(self->dtype == NDARRAY_INT8) {
                int8_t *array = (int8_t *)ndarray->array;
                for(size_t i=0; i < self->len; i++, array++) *array = -(*array);
            } else if(self->dtype == NDARRAY_UINT16) {                
                uint16_t *array = (uint16_t *)ndarray->array;
                for(size_t i=0; i < self->len; i++, array++) *array = -(*array);
            } else if(self->dtype == NDARRAY_INT16) {
                int16_t *array = (int16_t *)ndarray->array;
                for(size_t i=0; i < self->len; i++, array++) *array = -(*array);
            } else {
                mp_float_t *array = (mp_float_t *)ndarray->array;
                for(size_t i=0; i < self->len; i++, array++) *array = -(*array);
            }
            return MP_OBJ_FROM_PTR(ndarray);
            break;

        case MP_UNARY_OP_POSITIVE:
            return MP_OBJ_FROM_PTR(ndarray_copy_view(self));

        case MP_UNARY_OP_ABS:
            ndarray = ndarray_copy_view(self);
            // if Booleam, NDARRAY_UINT8, or NDARRAY_UINT16, there is nothing to do
            if(self->dtype == NDARRAY_INT8) {
                int8_t *array = (int8_t *)ndarray->array;
                for(size_t i=0; i < self->len; i++, array++) {
                    if(*array < 0) *array = -(*array);
                }
            } else if(self->dtype == NDARRAY_INT16) {
                int16_t *array = (int16_t *)ndarray->array;
                for(size_t i=0; i < self->len; i++, array++) {
                    if(*array < 0) *array = -(*array);
                }
            } else {
                mp_float_t *array = (mp_float_t *)ndarray->array;
                for(size_t i=0; i < self->len; i++, array++) {
                    if(*array < 0) *array = -(*array);
                }                
            }
            return MP_OBJ_FROM_PTR(ndarray);
            break;
        default: return MP_OBJ_NULL; // operator not supported
    }
}

mp_obj_t ndarray_transpose(mp_obj_t self_in) {
    // TODO: check, what happens to the offset here, if we have a view
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t *shape = m_new(size_t, self->ndim);
    int32_t *strides = m_new(int32_t, self->ndim);
    for(uint8_t i=0; i < self->ndim; i++) {
        shape[i] = self->shape[self->ndim-1-i];
        strides[i] = self->strides[self->ndim-1-i];
    }
    // TODO: I am not sure ndarray_new_view is OK here...
    // should be deep copy...
    ndarray_obj_t *ndarray = ndarray_new_view(self, self->ndim, shape, strides, 0);
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_1(ndarray_transpose_obj, ndarray_transpose);

mp_obj_t ndarray_reshape(mp_obj_t oin, mp_obj_t _shape) {
    // There is an error somewhere here: a = ulab.array(range(10)).reshape((2, 5)) won't work
    ndarray_obj_t *ndarray_in = MP_OBJ_TO_PTR(oin);
	if(!MP_OBJ_IS_TYPE(_shape, &mp_type_tuple)) {
		mp_raise_TypeError(translate("shape must be a tuple"));
    }

    mp_obj_tuple_t *shape = MP_OBJ_TO_PTR(_shape);
    if(shape->len > ULAB_MAX_DIMS) {
        mp_raise_ValueError(translate("maximum number of dimensions is 4"));
	}
    size_t *new_shape = m_new(size_t, shape->len);
    size_t new_length = 1;
    for(uint8_t i=0; i < shape->len; i++) {
        new_shape[i] = mp_obj_get_int(shape->items[i]);
        new_length *= new_shape[i];
    }
    
    if(ndarray_in->len != new_length) {
        mp_raise_ValueError(translate("input and output shapes are not compatible"));
    }
    ndarray_obj_t *ndarray;
    if(ndarray_is_dense(ndarray_in)) {
        // TODO: check if this is what numpy does
        int32_t *new_strides = strides_from_shape(new_shape, shape->len);
        ndarray = ndarray_new_view(ndarray_in, shape->len, new_shape, new_strides, 0);
    } else {
        ndarray = ndarray_new_ndarray_from_tuple(shape, ndarray_in->dtype);
        ndarray_copy_array(ndarray_in, ndarray);
    }
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_2(ndarray_reshape_obj, ndarray_reshape);

mp_int_t ndarray_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // buffer_p.get_buffer() returns zero for success, while mp_get_buffer returns true for success
    return !mp_get_buffer(self->array, bufinfo, flags);
}


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
//| from typing import List
//|
//| _DType = int
//| """`ulab.int8`, `ulab.uint8`, `ulab.int16`, `ulab.uint16`, or `ulab.float`"""
//|
//| _Index = Union[int, slice, List[bool], Tuple[Union[int, slice, List[bool]], Union[int, slice, List[bool]]]]
//| _float = float
//|

//| class array:
//|     """1- and 2- dimensional array"""
//|
//|     def __init__(
//|         self,
//|         values: Union[array, Iterable[_float], Iterable[Iterable[_float]]],
//|         *,
//|         dtype: _DType = float
//|     ) -> None:
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
//|     shape: Union[Tuple[int], Tuple[int, int]]
//|     """The size of the array, a tuple of length 1 or 2"""
//|
//|     size: int
//|     """The number of elements in the array"""
//|
//|     itemsize: int
//|     """The size of a single item in the array"""
//|
//|     def flatten(self, *, order: str = "C") -> array:
//|         """:param order: Whether to flatten by rows ('C') or columns ('F')
//|
//|            Returns a new `ulab.array` object which is always 1 dimensional.
//|            If order is 'C' (the default", then the data is ordered in rows;
//|            If it is 'F', then the data is ordered in columns.  "C" and "F" refer
//|            to the typical storage organization of the C and Fortran languages."""
//|         ...
//|
//|     def reshape(self, shape: Tuple[int, int]) -> array:
//|         """Returns an array containing the same data with a new shape."""
//|         ...
//|
//|     def sort(self, *, axis: Optional[int] = 1) -> None:
//|         """:param axis: Whether to sort elements within rows (0), columns (1), or elements (None)"""
//|         ...
//|
//|     def transpose(self) -> array:
//|         """Swap the rows and columns of a 2-dimensional array"""
//|         ...
//|
//|     def __add__(self, other: Union[array, _float]) -> array:
//|         """Adds corresponding elements of the two arrays, or adds a number to all
//|            elements of the array.  If both arguments are arrays, their sizes must match."""
//|         ...
//|     def __radd__(self, other: _float) -> array: ...
//|
//|     def __sub__(self, other: Union[array, _float]) -> array:
//|         """Subtracts corresponding elements of the two arrays, or subtracts a number from all
//|            elements of the array.  If both arguments are arrays, their sizes must match."""
//|         ...
//|     def __rsub__(self, other: _float) -> array: ...
//|
//|     def __mul__(self, other: Union[array, _float]) -> array:
//|         """Multiplies corresponding elements of the two arrays, or multiplies
//|            all elements of the array by a number.  If both arguments are arrays,
//|            their sizes must match."""
//|         ...
//|     def __rmul__(self, other: _float) -> array: ...
//|
//|     def __div__(self, other: Union[array, _float]) -> array:
//|         """Multiplies corresponding elements of the two arrays, or divides
//|            all elements of the array by a number.  If both arguments are arrays,
//|            their sizes must match."""
//|         ...
//|     def __rdiv__(self, other: _float) -> array: ...
//|
//|     def __pow__(self, other: Union[array, _float]) -> array:
//|         """Computes the power (x**y) of corresponding elements of the the two arrays,
//|            or one number and one array.  If both arguments are arrays, their sizes
//|            must match."""
//|         ...
//|     def __rpow__(self, other: _float) -> array: ...
//|
//|     def __inv__(self) -> array:
//|         ...
//|     def __neg__(self) -> array:
//|         ...
//|     def __pos__(self) -> array:
//|         ...
//|     def __abs__(self) -> array:
//|         ...
//|     def __len__(self) -> array:
//|         ...
//|     def __lt__(self, other: Union[array, _float]) -> List[bool]:
//|         ...
//|     def __le__(self, other: Union[array, _float]) -> List[bool]:
//|         ...
//|     def __gt__(self, other: Union[array, _float]) -> List[bool]:
//|         ...
//|     def __ge__(self, other: Union[array, _float]) -> List[bool]:
//|         ...
//|
//|     def __iter__(self) -> Union[Iterator[array], Iterator[_float]]:
//|         ...
//|
//|     def __getitem__(self, index: _Index) -> Union[array, _float]:
//|         """Retrieve an element of the array."""
//|         ...
//|
//|     def __setitem__(self, index: _Index, value: Union[array, _float]) -> None:
//|         """Set an element of the array."""
//|         ...
//|
//| _ArrayLike = Union[array, List[_float], Tuple[_float], range]
//|
//| int8: _DType
//| """Type code for signed integers in the range -128 .. 127 inclusive, like the 'b' typecode of `array.array`"""
//|
//| int16: _DType
//| """Type code for signed integers in the range -32768 .. 32767 inclusive, like the 'h' typecode of `array.array`"""
//|
//| float: _DType
//| """Type code for floating point values, like the 'f' typecode of `array.array`"""
//|
//| uint8: _DType
//| """Type code for unsigned integers in the range 0 .. 255 inclusive, like the 'H' typecode of `array.array`"""
//|
//| uint16: _DType
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

mp_float_t ndarray_get_float_index(void *data, uint8_t typecode, size_t index) {
    // Returns a float value from an arbitrary data type
    // The value in question is supposed to be located at the head of the pointer
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

mp_float_t ndarray_get_float_value(void *data, uint8_t typecode) {
    // Returns a float value from an arbitrary data type
    // The value in question is supposed to be located at the head of the pointer
    if(typecode == NDARRAY_UINT8) {
        return *(mp_float_t *)((uint8_t *)data);
    } else if(typecode == NDARRAY_INT8) {
        return *(mp_float_t *)((int8_t *)data);
    } else if(typecode == NDARRAY_UINT16) {
        return *(mp_float_t *)((uint16_t *)data);
    } else if(typecode == NDARRAY_INT16) {
        return *(mp_float_t *)((int16_t *)data);
    } else {
        return *(mp_float_t *)((mp_float_t *)data);
    }
}

void ndarray_fill_array_iterable(mp_float_t *array, mp_obj_t iterable) {
    mp_obj_iter_buf_t x_buf;
    mp_obj_t x_item, x_iterable = mp_getiter(iterable, &x_buf);
    while ((x_item = mp_iternext(x_iterable)) != MP_OBJ_STOP_ITERATION) {
        *array++ = (mp_float_t)mp_obj_get_float(x_item);
    }
}

static int32_t *strides_from_shape(size_t *shape, uint8_t dtype) {
    // returns a strides array that corresponds to a dense array with the prescribed shape
    int32_t *strides = m_new(int32_t, ULAB_MAX_DIMS);
    strides[ULAB_MAX_DIMS-1] = 	(int32_t)mp_binary_get_size('@', dtype, NULL);;
    for(uint8_t i=ULAB_MAX_DIMS; i > 1; i--) {
        strides[i-2] = strides[i-1] * shape[i-1];
    }
    return strides;
}

size_t *ndarray_shape_vector(size_t a, size_t b, size_t c, size_t d) {
    // returns a ULAB_MAX_DIMS-aware array of shapes
    // WARNING: this assumes that the maximum possible dimension is 4!
    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    shape[ULAB_MAX_DIMS - 1] = d;
    #if ULAB_MAX_DIMS > 1
    shape[ULAB_MAX_DIMS - 2] = c;
    #endif
    #if ULAB_MAX_DIMS > 2
    shape[ULAB_MAX_DIMS - 3] = b;
    #endif
    #if ULAB_MAX_DIMS > 3
    shape[ULAB_MAX_DIMS - 4] = a;
    #endif
    return shape;
}

// TODO: should be re-named as array_like
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


void ndarray_print_row(const mp_print_t *print, uint8_t dtype, uint8_t *array, size_t stride, size_t n) {
    mp_print_str(print, "[");
    if((n <= ndarray_print_threshold) || (n <= 2*ndarray_print_edgeitems)) { // if the array is short, print everything
        mp_obj_print_helper(print, mp_binary_get_val_array(dtype, array, 0), PRINT_REPR);
        array += stride;
        for(size_t i=1; i < n; i++, array += stride) {
            mp_print_str(print, ", ");
            mp_obj_print_helper(print, mp_binary_get_val_array(dtype, array, 0), PRINT_REPR);
        }
    } else {
        mp_obj_print_helper(print, mp_binary_get_val_array(dtype, array, 0), PRINT_REPR);
        array += stride;
        for(size_t i=1; i < ndarray_print_edgeitems; i++, array += stride) {
            mp_print_str(print, ", ");
            mp_obj_print_helper(print, mp_binary_get_val_array(dtype, array, 0), PRINT_REPR);
        }
        mp_printf(print, ", ..., ");
        array += stride * (n - 2 *  ndarray_print_edgeitems);
        mp_obj_print_helper(print, mp_binary_get_val_array(dtype, array, 0), PRINT_REPR);
        array += stride;
        for(size_t i=1; i < ndarray_print_edgeitems; i++, array += stride) {
            mp_print_str(print, ", ");
            mp_obj_print_helper(print, mp_binary_get_val_array(dtype, array, 0), PRINT_REPR);
        }
    }
    mp_print_str(print, "]");
}

void ndarray_print_bracket(const mp_print_t *print, const size_t condition, const size_t shape, const char *string) {
    if(condition < shape) {
        mp_print_str(print, string);
    }
}

void ndarray_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t *array = (uint8_t *)self->array;
    mp_print_str(print, "array(");

    #if ULAB_MAX_DIMS > 3
    size_t i=0;
    ndarray_print_bracket(print, 0, self->shape[ULAB_MAX_DIMS-4], "[");
    do {
    #endif
        #if ULAB_MAX_DIMS > 2
        size_t j = 0;
        ndarray_print_bracket(print, 0, self->shape[ULAB_MAX_DIMS-3], "[");
        do {
        #endif
            #if ULAB_MAX_DIMS > 1
            size_t k = 0;
            ndarray_print_bracket(print, 0, self->shape[ULAB_MAX_DIMS-2], "[");
            do {
            #endif
                ndarray_print_row(print, self->dtype, array, self->strides[ULAB_MAX_DIMS-1], self->shape[ULAB_MAX_DIMS-1]);
                #if ULAB_MAX_DIMS > 1
                array += self->strides[ULAB_MAX_DIMS-2];
                k++;
                ndarray_print_bracket(print, k, self->shape[ULAB_MAX_DIMS-2], ",\n\t");
            } while(k < self->shape[ULAB_MAX_DIMS-2]);
                #endif
        #if ULAB_MAX_DIMS > 2
            ndarray_print_bracket(print, 0, self->shape[ULAB_MAX_DIMS-2], "]");
            j++;
            ndarray_print_bracket(print, j, self->shape[ULAB_MAX_DIMS-3], ",\n\n\t");
            array -= self->strides[ULAB_MAX_DIMS-2] * self->shape[ULAB_MAX_DIMS-2];
            array += self->strides[ULAB_MAX_DIMS-3];
        } while(j < self->shape[ULAB_MAX_DIMS-3]);
        #endif
    #if ULAB_MAX_DIMS > 3
        ndarray_print_bracket(print, 0, self->shape[ULAB_MAX_DIMS-3], "]");
        array -= self->strides[ULAB_MAX_DIMS-3] * self->shape[ULAB_MAX_DIMS-3];
        array += self->strides[ULAB_MAX_DIMS-4];
        i++;
        ndarray_print_bracket(print, i, self->shape[ULAB_MAX_DIMS-4], ",\n\n\t");
    } while(i < self->shape[ULAB_MAX_DIMS-4]);
    #endif
    
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
	if(ndarray->boolean) {
        uint8_t *array = (uint8_t *)ndarray->array;
        array += *idx;
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
	int32_t stride = ndarray->itemsize;
	for(uint8_t i=ULAB_MAX_DIMS; i > ULAB_MAX_DIMS-ndarray->ndim; i--) {
        stride *= ndarray->shape[i];
    }
	return stride == ndarray->strides[ULAB_MAX_DIMS-ndarray->ndim-1] ? true : false;
}

ndarray_obj_t *ndarray_new_ndarray(uint8_t ndim, size_t *shape, int32_t *strides, uint8_t dtype) {
    // Creates the base ndarray with shape, and initialises the values to straight 0s
    // the function should work in the general n-dimensional case
    ndarray_obj_t *ndarray = m_new_obj(ndarray_obj_t);
    ndarray->base.type = &ulab_ndarray_type;
    ndarray->dense = 1;
    ndarray->dtype = dtype;
    ndarray->ndim = ndim;
	ndarray->len = 1;
    ndarray->itemsize = mp_binary_get_size('@', dtype, NULL);
    for(uint8_t i=ULAB_MAX_DIMS; i > ULAB_MAX_DIMS-ndim; i--) {
		ndarray->shape[i-1] = shape[i-1];
		ndarray->strides[i-1] = strides[i-1];
		ndarray->len *= shape[i-1];
	}
    if(dtype == NDARRAY_BOOL) {
        dtype = NDARRAY_UINT8;
        ndarray->boolean = NDARRAY_BOOLEAN;
    } else {
        ndarray->boolean = NDARRAY_NUMERIC;
    }
    uint8_t *array = m_new(byte, ndarray->itemsize * ndarray->len);
    // this should set all elements to 0, irrespective of the of the dtype (all bits are zero)
    // we could, perhaps, leave this step out, and initialise the array only, when needed
    memset(array, 0, ndarray->len * ndarray->itemsize);
    ndarray->array = array;
    return ndarray;
}

ndarray_obj_t *ndarray_new_dense_ndarray(uint8_t ndim, size_t *shape, uint8_t dtype) {
    // creates a dense array, i.e., one, where the strides are derived directly from the shapes
    // the function should work in the general n-dimensional case
    int32_t *strides = m_new(int32_t, ULAB_MAX_DIMS);
    strides[ULAB_MAX_DIMS-1] = mp_binary_get_size('@', dtype, NULL);
    for(size_t i=ULAB_MAX_DIMS; i > 1; i--) {
        strides[i-2] = strides[i-1] * shape[i-1];
    }
    return ndarray_new_ndarray(ndim, shape, strides, dtype);
}

ndarray_obj_t *ndarray_new_ndarray_from_tuple(mp_obj_tuple_t *_shape, uint8_t dtype) {
    // creates a dense array from a tuple
    // the function should work in the general n-dimensional case
    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    for(size_t i=0; i < ULAB_MAX_DIMS; i++) {
        if(i < ULAB_MAX_DIMS - _shape->len) {
            shape[i] = 0;
        } else {
            shape[i] = mp_obj_get_int(_shape->items[i]);
        }
    }
    return ndarray_new_dense_ndarray(_shape->len, shape, dtype);
}

void ndarray_copy_array(ndarray_obj_t *source, ndarray_obj_t *target) {
    // TODO: if the array is dense, the content could be copied in a single pass
    // copies the content of source->array into a new dense void pointer
	// it is assumed that the dtypes in source and target are the same
    // Since the target is a new array, it is supposed to be dense
    uint8_t *sarray = (uint8_t *)source->array;
    uint8_t *tarray = (uint8_t *)target->array;
    
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
                    memcpy(tarray, sarray, source->itemsize);
                    tarray += target->itemsize;
                    sarray += source->strides[ULAB_MAX_DIMS - 1];
                    l++;
                } while(l <  source->shape[ULAB_MAX_DIMS - 1]);
            #if ULAB_MAX_DIMS > 1
                sarray -= source->strides[ULAB_MAX_DIMS - 1] * source->shape[ULAB_MAX_DIMS-1];
                sarray += source->strides[ULAB_MAX_DIMS - 2];
                k++;
            } while(k <  source->shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 2
            sarray -= source->strides[ULAB_MAX_DIMS - 2] * source->shape[ULAB_MAX_DIMS-2];
            sarray += source->strides[ULAB_MAX_DIMS - 3];
            j++;
        } while(j <  source->shape[ULAB_MAX_DIMS - 3]);
        #endif
    #if ULAB_MAX_DIMS > 3
        sarray -= source->strides[ULAB_MAX_DIMS - 3] * source->shape[ULAB_MAX_DIMS-3];
        sarray += source->strides[ULAB_MAX_DIMS - 4];
        i++;
    } while(i <  source->shape[ULAB_MAX_DIMS - 4]);
    #endif
}

ndarray_obj_t *ndarray_new_view(ndarray_obj_t *source, uint8_t ndim, size_t *shape, int32_t *strides, int32_t offset) {
    // creates a new view from the input arguments
    // the function should work in the n-dimensional case
    ndarray_obj_t *ndarray = m_new_obj(ndarray_obj_t);
    ndarray->base.type = &ulab_ndarray_type;
    ndarray->boolean = source->boolean;
    ndarray->dtype = source->dtype;
    ndarray->ndim = ndim;
    ndarray->itemsize = source->itemsize;
    ndarray->len = 1;
    for(uint8_t i=ULAB_MAX_DIMS; i > ULAB_MAX_DIMS-ndim; i--) {
		ndarray->shape[i-1] = shape[i-1];
		ndarray->strides[i-1] = strides[i-1];
		ndarray->len *= shape[i-1];
	}
    uint8_t *pointer = (uint8_t *)source->array;
    pointer += offset;
    ndarray->array = pointer;
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
    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    shape[ULAB_MAX_DIMS-1] = len;
    return ndarray_new_dense_ndarray(1, shape, dtype);
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
	size_t len1 = 0, len2 = 0;
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
    mp_obj_t iterable1 = mp_getiter(args[0], &iter_buf1);
    #if ULAB_MAX_DIMS > 1
    mp_obj_t item1;
    size_t i = 0;
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
    #endif
    // By this time, it should be established, what the shape is, so we can now create the array
    if(len2 == 0) {
		self = ndarray_new_linear_array(len1, dtype);
	}
    #if ULAB_MAX_DIMS > 1
    else {
		size_t shape[2] = {len1, len2};
		self = ndarray_new_dense_ndarray(2, shape, dtype);
	}
    #endif
    size_t idx = 0;
    iterable1 = mp_getiter(args[0], &iter_buf1);
    if(len2 == 0) { // the first argument is a single iterable
        ndarray_assign_elements(self, iterable1, dtype, &idx);
    }
    #if ULAB_MAX_DIMS > 1
    else {
        mp_obj_iter_buf_t iter_buf2;
        mp_obj_t iterable2;
        while ((item1 = mp_iternext(iterable1)) != MP_OBJ_STOP_ITERATION) {
            iterable2 = mp_getiter(item1, &iter_buf2);
            ndarray_assign_elements(self, iterable2, dtype, &idx);
        }
    }
    #endif
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

static size_t slice_length(mp_bound_slice_t slice) {
    ssize_t len, correction = 1;
    if(slice.step > 0) correction = -1;
    len = (ssize_t)(slice.stop - slice.start + (slice.step + correction)) / slice.step;
    if(len < 0) return 0;
    return (size_t)len;
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

ndarray_obj_t *ndarray_view_from_slices(ndarray_obj_t *ndarray, mp_obj_tuple_t *tuple) {
    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    memset(shape, 0, sizeof(size_t)*ULAB_MAX_DIMS);
    int32_t *strides = m_new(int32_t, ULAB_MAX_DIMS);
    memset(strides, 0, sizeof(size_t)*ULAB_MAX_DIMS);
    uint8_t ndim = ndarray->ndim;
    for(uint8_t i=0; i < ndim; i++) {
        // copy from the end
        shape[ULAB_MAX_DIMS - 1 - i] = ndarray->shape[ULAB_MAX_DIMS  - 1 - i];
        strides[ULAB_MAX_DIMS - 1 - i] = ndarray->strides[ULAB_MAX_DIMS  - 1 - i];
    }
    int32_t offset = 0;
    for(uint8_t i=0; i  < tuple->len; i++) {
        mp_bound_slice_t slice = generate_slice(shape[ULAB_MAX_DIMS - ndim + i], tuple->items[i]);
        shape[ULAB_MAX_DIMS - ndim + i] = slice_length(slice);
        offset += ndarray->strides[ULAB_MAX_DIMS - ndim + i] * (int32_t)slice.start;
        strides[ULAB_MAX_DIMS - ndim + i] = (int32_t)slice.step * ndarray->strides[ULAB_MAX_DIMS - ndim + i];
    }
    return ndarray_new_view(ndarray, ndim, shape, strides, offset);
}

bool ndarray_view_broadcast(ndarray_obj_t *view, ndarray_obj_t *values, int32_t *strides) {
    // returns True or False, depending on, whether the `values` array
    //  can be broadcast to the shape of the `view` array
    memset(strides, 0, sizeof(size_t)*ULAB_MAX_DIMS);
    for(uint8_t i=ULAB_MAX_DIMS; i > 0; i--) {
        if((values->shape[i-1] == view->shape[i-1])) {
            strides[i-1] = values->strides[i-1];
        } else if(values->shape[i-1] == 1) {
            strides[i-1] = 0;
        } else {
            return false;
        }
    }
    return true;
}

void ndarray_assign_view(ndarray_obj_t *view, ndarray_obj_t *values) {
    int32_t *strides = m_new(int32_t, ULAB_MAX_DIMS);
    if(!ndarray_view_broadcast(view, values, strides)) {
        mp_raise_ValueError(translate("operands could not be broadcast together"));
        m_del(int32_t, strides, ULAB_MAX_DIMS);
    }

    uint8_t *viarray = (uint8_t *)view->array;
    uint8_t *vaarray = (uint8_t *)values->array;

    if(view->dtype == NDARRAY_UINT8) {
        if(values->dtype == NDARRAY_UINT8) {
            ASSIGNMENT_LOOP(uint8_t, uint8_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_INT8) {
            ASSIGNMENT_LOOP(uint8_t, int8_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_UINT16) {
            ASSIGNMENT_LOOP(uint8_t, uint16_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_INT16) {
            ASSIGNMENT_LOOP(uint8_t, int16_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_FLOAT) {
            ASSIGNMENT_LOOP(uint8_t, mp_float_t, view, viarray, values, vaarray, strides);
        }
    } else if(view->dtype == NDARRAY_INT8) {
        if(values->dtype == NDARRAY_UINT8) {
            ASSIGNMENT_LOOP(int8_t, uint8_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_INT8) {
            ASSIGNMENT_LOOP(int8_t, int8_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_UINT16) {
            ASSIGNMENT_LOOP(int8_t, uint16_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_INT16) {
            ASSIGNMENT_LOOP(int8_t, int16_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_FLOAT) {
            ASSIGNMENT_LOOP(int8_t, mp_float_t, view, viarray, values, vaarray, strides);
        }
    } else if(view->dtype == NDARRAY_UINT16) {
        if(values->dtype == NDARRAY_UINT8) {
            ASSIGNMENT_LOOP(uint16_t, uint8_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_INT8) {
            ASSIGNMENT_LOOP(uint16_t, int8_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_UINT16) {
            ASSIGNMENT_LOOP(uint16_t, uint16_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_INT16) {
            ASSIGNMENT_LOOP(uint16_t, int16_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_FLOAT) {
            ASSIGNMENT_LOOP(uint16_t, mp_float_t, view, viarray, values, vaarray, strides);
        }
    } else if(view->dtype == NDARRAY_INT16) {
        if(values->dtype == NDARRAY_UINT8) {
            ASSIGNMENT_LOOP(int16_t, uint8_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_INT8) {
            ASSIGNMENT_LOOP(int16_t, int8_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_UINT16) {
            ASSIGNMENT_LOOP(int16_t, uint16_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_INT16) {
            ASSIGNMENT_LOOP(int16_t, int16_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_FLOAT) {
            ASSIGNMENT_LOOP(int16_t, mp_float_t, view, viarray, values, vaarray, strides);
        }
    } else {
        if(values->dtype == NDARRAY_UINT8) {
            ASSIGNMENT_LOOP(mp_float_t, uint8_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_INT8) {
            ASSIGNMENT_LOOP(mp_float_t, int8_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_UINT16) {
            ASSIGNMENT_LOOP(mp_float_t, uint16_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_INT16) {
            ASSIGNMENT_LOOP(mp_float_t, int16_t, view, viarray, values, vaarray, strides);
        } else if(values->dtype == NDARRAY_FLOAT) {
            ASSIGNMENT_LOOP(mp_float_t, mp_float_t, view, viarray, values, vaarray, strides);
        }
    }
}

static mp_obj_t ndarray_get_slice(ndarray_obj_t *ndarray, mp_obj_t index, ndarray_obj_t *values) {
    if(MP_OBJ_IS_TYPE(index, &mp_type_tuple) || MP_OBJ_IS_INT(index) || MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
        mp_obj_tuple_t *tuple;
        if(MP_OBJ_IS_TYPE(index, &mp_type_tuple)) {
            tuple = MP_OBJ_TO_PTR(index);
            if(tuple->len > ndarray->ndim) {
                mp_raise_msg(&mp_type_IndexError, translate("too many indices"));
            }
        } else {
            mp_obj_t *items = m_new(mp_obj_t, 1);
            items[0] = index;
            tuple = mp_obj_new_tuple(1, items);
        }
        ndarray_obj_t *view = ndarray_view_from_slices(ndarray, tuple);
        if(values == NULL) { // return value(s)
            if(view->len == 1) {
                return mp_binary_get_val_array(view->dtype, view->array, 0);
            } else {
                return MP_OBJ_FROM_PTR(view);
            }
        } else { // assign value(s)
            ndarray_assign_view(view, values);
        }
    }
    return mp_const_none;
}

mp_obj_t ndarray_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (value == MP_OBJ_SENTINEL) { // return value(s)
        return ndarray_get_slice(self, index, NULL);
    } else { // assignment to slices; the value must be an ndarray, or a scalar
        ndarray_obj_t *values = ndarray_from_mp_obj(value);
        return ndarray_get_slice(self, index, values);
    }
    return mp_const_none;
}

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
    size_t iter_end = ndarray->shape[ULAB_MAX_DIMS-ndarray->ndim];
    if(self->cur < iter_end) {
        // separating this case out saves 50 bytes for 1D arrays
        #if ULAB_MAX_DIMS == 1
        int32_t pos = self->cur * ndarray->strides[ULAB_MAX_DIMS - 1] / ndarray->itemsize;
        return mp_binary_get_val_array(ndarray->dtype, ndarray->array, pos);
        #else
        if(ndarray->ndim == 1) { // we have a linear array
            int32_t pos = self->cur * ndarray->strides[ULAB_MAX_DIMS - 1] / ndarray->itemsize;
            self->cur++;
            return mp_binary_get_val_array(ndarray->dtype, ndarray->array, pos);
        } else { // we have a tensor, return the reduced view
            size_t offset = self->cur * ndarray->strides[ULAB_MAX_DIMS - ndarray->ndim];
            self->cur++;
            return MP_OBJ_FROM_PTR(ndarray_new_view(ndarray, ndarray->ndim-1, ndarray->shape, ndarray->strides, offset));
        }
        #endif
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

mp_obj_t ndarray_strides(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t *items = m_new(mp_obj_t, self->ndim);
    for(int8_t i=0; i < self->ndim; i++) {
        items[i] = mp_obj_new_int(self->strides[ULAB_MAX_DIMS - self->ndim + i]);
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
    return MP_OBJ_NEW_SMALL_INT(self->itemsize);
}

mp_obj_t ndarray_flatten(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_order, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_QSTR(MP_QSTR_C)} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    ndarray_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    GET_STR_DATA_LEN(args[0].u_obj, order, len);
    if((len != 1) || ((memcmp(order, "C", 1) != 0) && (memcmp(order, "F", 1) != 0))) {
        mp_raise_ValueError(translate("flattening order must be either 'C', or 'F'"));
    }

    uint8_t *sarray = (uint8_t *)self->array;
    ndarray_obj_t *ndarray = ndarray_new_linear_array(self->len, self->dtype);
    uint8_t *array = (uint8_t *)ndarray->array;

    if(memcmp(order, "C", 1) == 0) {
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
                        memcpy(array, sarray, self->itemsize);
                        array += ndarray->strides[ULAB_MAX_DIMS - 1];
                        sarray += self->strides[ULAB_MAX_DIMS - 1];
                        l++;
                    } while(l <  self->shape[ULAB_MAX_DIMS - 1]);
                #if ULAB_MAX_DIMS > 1
                    sarray -= self->strides[ULAB_MAX_DIMS - 1] * self->shape[ULAB_MAX_DIMS-1];
                    sarray += self->strides[ULAB_MAX_DIMS - 2];
                    k++;
                } while(k <  self->shape[ULAB_MAX_DIMS - 2]);
                #endif
            #if ULAB_MAX_DIMS > 2
                sarray -= self->strides[ULAB_MAX_DIMS - 2] * self->shape[ULAB_MAX_DIMS-2];
                sarray += self->strides[ULAB_MAX_DIMS - 3];
                j++;
            } while(j <  self->shape[ULAB_MAX_DIMS - 3]);
            #endif
        #if ULAB_MAX_DIMS > 3
            sarray -= self->strides[ULAB_MAX_DIMS - 3] * self->shape[ULAB_MAX_DIMS-3];
            sarray += self->strides[ULAB_MAX_DIMS - 4];
            i++;
        } while(i <  self->shape[ULAB_MAX_DIMS - 4]);
        #endif
    } else {
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
                        memcpy(array, sarray, self->itemsize);
                        array += ndarray->strides[0];
                        sarray += self->strides[0];
                        l++;
                    } while(l <  self->shape[0]);
                #if ULAB_MAX_DIMS > 1
                    sarray -= self->strides[0] * self->shape[0];
                    sarray += self->strides[1];
                    k++;
                } while(k <  self->shape[1]);
                #endif
            #if ULAB_MAX_DIMS > 2
                sarray -= self->strides[1] * self->shape[1];
                sarray += self->strides[2];
                j++;
            } while(j <  self->shape[2]);
            #endif
        #if ULAB_MAX_DIMS > 3
            sarray -= self->strides[2] * self->shape[2];
            sarray += self->strides[3];
            i++;
        } while(i <  self->shape[3]);
        #endif
    }
    return MP_OBJ_FROM_PTR(ndarray);
}

// Binary operations
ndarray_obj_t *ndarray_from_mp_obj(mp_obj_t obj) {
    // creates an ndarray from a micropython int or float
    // if the input is an ndarray, it is returned
    ndarray_obj_t *ndarray;
    if(MP_OBJ_IS_INT(obj)) {
        int32_t ivalue = mp_obj_get_int(obj);
        if((ivalue > 0) && (ivalue < 256)) {
            ndarray = ndarray_new_linear_array(1, NDARRAY_UINT8);
            uint8_t *array = (uint8_t *)ndarray->array;
            array[0] = (uint8_t)ivalue;
        } else if((ivalue > 255) && (ivalue < 65535)) {
            ndarray = ndarray_new_linear_array(1, NDARRAY_UINT16);
            uint16_t *array = (uint16_t *)ndarray->array;
            array[0] = (uint16_t)ivalue;
        } else if((ivalue < 0) && (ivalue > -128)) {
            ndarray = ndarray_new_linear_array(1, NDARRAY_INT8);
            int8_t *array = (int8_t *)ndarray->array;
            array[0] = (int8_t)ivalue;
        } else if((ivalue < -127) && (ivalue > -32767)) {
            ndarray = ndarray_new_linear_array(1, NDARRAY_INT16);
            int16_t *array = (int16_t *)ndarray->array;
            array[0] = (int16_t)ivalue;
        } else { // the integer value clearly does not fit the ulab integer types, so move on to float
            ndarray = ndarray_new_linear_array(1, NDARRAY_FLOAT);
            mp_float_t *array = (mp_float_t *)ndarray->array;
            array[0] = (mp_float_t)ivalue;
        }
    } else if(mp_obj_is_float(obj)) {
        mp_float_t fvalue = mp_obj_get_float(obj);
        ndarray = ndarray_new_linear_array(1, NDARRAY_FLOAT);
        mp_float_t *array = (mp_float_t *)ndarray->array;
        array[0] = (mp_float_t)fvalue;
    } else if(MP_OBJ_IS_TYPE(obj, &ulab_ndarray_type)){
        return obj;
    } else {
        mp_raise_TypeError(translate("wrong operand type"));
    }
    return ndarray;
}

bool ndarray_can_broadcast(ndarray_obj_t *lhs, ndarray_obj_t *rhs, uint8_t *ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {
    // returns True or False, depending on, whether the two arrays can be broadcast together
    // numpy's broadcasting rules are as follows:
    //
    // 1. the two shapes are either equal
    // 2. one of the shapes is 1
    memset(lstrides, 0, sizeof(size_t)*ULAB_MAX_DIMS);
    memset(rstrides, 0, sizeof(size_t)*ULAB_MAX_DIMS);
    lstrides[ULAB_MAX_DIMS - 1] = lhs->strides[ULAB_MAX_DIMS - 1];
    rstrides[ULAB_MAX_DIMS - 1] = rhs->strides[ULAB_MAX_DIMS - 1];    
    for(uint8_t i=ULAB_MAX_DIMS; i > 0; i--) {
        if((lhs->shape[i-1] == rhs->shape[i-1]) || (lhs->shape[i-1] == 0) || (lhs->shape[i-1] == 1) ||
        (rhs->shape[i-1] == 0) || (rhs->shape[i-1] == 1)) {
            shape[i-1] = MAX(lhs->shape[i-1], rhs->shape[i-1]);
            if(shape[i-1] > 0) (*ndim)++;
            if(lhs->shape[i-1] < 2) {
                lstrides[i-1] = 0;
            } else {
                lstrides[i-1] = lhs->strides[i-1];
            }
            if(rhs->shape[i-1] < 2) {
                rstrides[i-1] = 0;
            } else {
                rstrides[i-1] = rhs->strides[i-1];
            }
        } else {
            return false;
        }
    }
    return true;
}

mp_obj_t ndarray_binary_op(mp_binary_op_t _op, mp_obj_t lobj, mp_obj_t robj) {
    // if the ndarray stands on the right hand side of the expression, simply swap the operands
    ndarray_obj_t *lhs, *rhs;
    mp_binary_op_t op = _op;
    if((op == MP_BINARY_OP_REVERSE_ADD) || (op == MP_BINARY_OP_REVERSE_MULTIPLY) ||
        (op == MP_BINARY_OP_REVERSE_POWER) || (op == MP_BINARY_OP_REVERSE_SUBTRACT) ||
        (op == MP_BINARY_OP_REVERSE_TRUE_DIVIDE)) {
        lhs = ndarray_from_mp_obj(robj);
        rhs = ndarray_from_mp_obj(lobj);
    } else {
        lhs = ndarray_from_mp_obj(lobj);
        rhs = ndarray_from_mp_obj(robj);
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

    uint8_t ndim = 0;
    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    int32_t *lstrides = m_new(int32_t, ULAB_MAX_DIMS);
    int32_t *rstrides = m_new(int32_t, ULAB_MAX_DIMS);
    if(!ndarray_can_broadcast(lhs, rhs, &ndim, shape, lstrides, rstrides)) {
    mp_raise_ValueError(translate("operands could not be broadcast together"));
    m_del(size_t, shape, ULAB_MAX_DIMS);
    m_del(int32_t, lstrides, ULAB_MAX_DIMS);
    m_del(int32_t, rstrides, ULAB_MAX_DIMS);
    }

    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    switch(op) {
        case MP_BINARY_OP_LESS:
            // here we simply swap the operands
            return ndarray_binary_op(MP_BINARY_OP_MORE, rhs, lhs);
            break;
            
        case MP_BINARY_OP_LESS_EQUAL:
            // here we simply swap the operands
            return ndarray_binary_op(MP_BINARY_OP_MORE_EQUAL, rhs, lhs);
            break;
            
        // by separating the associative operators, we can save a lot of flash space,
        // because the operands can simply be swapped for half of the cases
        case MP_BINARY_OP_EQUAL:
        case MP_BINARY_OP_NOT_EQUAL:
        case MP_BINARY_OP_ADD:
        case MP_BINARY_OP_MULTIPLY:
            if(lhs->dtype == NDARRAY_UINT8) {
                if(rhs->dtype == NDARRAY_UINT8) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_UINT8, uint8_t, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_INT8) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_INT16, int16_t, uint8_t, int8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_UINT16) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_INT16) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_INT16, int16_t, uint8_t, int16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_FLOAT) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                }
            } else if(lhs->dtype == NDARRAY_INT8) {
                if(rhs->dtype == NDARRAY_INT8) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_INT8, int8_t, int8_t, int8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_UINT16) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_INT16, int16_t, int8_t, uint16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_INT16) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_INT16, int16_t, int8_t, int16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_FLOAT) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else {
                    ndarray_binary_op(op, rhs, lhs);
                }
            } else if(lhs->dtype == NDARRAY_UINT16) {
                if(rhs->dtype == NDARRAY_UINT16) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_INT16) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, uint16_t, int16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_FLOAT) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else {
                    ndarray_binary_op(op, rhs, lhs);
                }
            } else if(lhs->dtype == NDARRAY_INT16) {
                if(rhs->dtype == NDARRAY_INT16) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_INT16, int16_t, int16_t, int16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_FLOAT) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else {
                    ndarray_binary_op(op, rhs, lhs);
                }
            } else if(lhs->dtype == NDARRAY_FLOAT) {
                if(rhs->dtype == NDARRAY_FLOAT) {
                    RUN_ASSOC_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else {
                    ndarray_binary_op(op, rhs, lhs);
                }
            }
            break;
            
        case MP_BINARY_OP_MORE:
        case MP_BINARY_OP_MORE_EQUAL:
        case MP_BINARY_OP_SUBTRACT:
        case MP_BINARY_OP_TRUE_DIVIDE:
//		case MP_BINARY_OP_POWER:
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
            if(lhs->dtype == NDARRAY_UINT8) {
                if(rhs->dtype == NDARRAY_UINT8) {
                    RUN_BINARY_LOOP(NDARRAY_UINT8, uint8_t, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_INT8) {
                    RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, uint8_t, int8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_UINT16) {
                    RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_INT16) {
                    RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, uint8_t, int16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_FLOAT) {
                    RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                }
            } else if(lhs->dtype == NDARRAY_INT8) {
                if(rhs->dtype == NDARRAY_UINT8) {
                    RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int8_t, uint8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_INT8) {
                    RUN_BINARY_LOOP(NDARRAY_INT8, int8_t, int8_t, int8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_UINT16) {
                    RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int8_t, uint16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_INT16) {
                    RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int8_t, int16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_FLOAT) {
                    RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                }
            } else if(lhs->dtype == NDARRAY_UINT16) {
                if(rhs->dtype == NDARRAY_UINT8) {
                    RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, uint8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_INT8) {
                    RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, int8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_UINT16) {
                    RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_INT16) {
                    RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, uint16_t, int16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_FLOAT) {
                    RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                }
            } else if(lhs->dtype == NDARRAY_INT16) {
                if(rhs->dtype == NDARRAY_UINT8) {
                    RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int16_t, uint8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_INT8) {
                    RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int16_t, int8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_UINT16) {
                    RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, int16_t, uint16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_INT16) {
                    RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int16_t, int16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_FLOAT) {
                    RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                }
            } else if(lhs->dtype == NDARRAY_FLOAT) {
                if(rhs->dtype == NDARRAY_UINT8) {
                    RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, uint8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_INT8) {
                    RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, int8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_UINT16) {
                    RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, uint16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_INT16) {
                    RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, int16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                } else if(rhs->dtype == NDARRAY_FLOAT) {
                    RUN_BINARY_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
                }
            }
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

#if ULAB_MAX_DIMS > 1
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
    ndarray_obj_t *source = MP_OBJ_TO_PTR(oin);
	if(!MP_OBJ_IS_TYPE(_shape, &mp_type_tuple)) {
		mp_raise_TypeError(translate("shape must be a tuple"));
    }

    mp_obj_tuple_t *shape = MP_OBJ_TO_PTR(_shape);
    if(shape->len > ULAB_MAX_DIMS) {
        mp_raise_ValueError(translate("maximum number of dimensions is 4"));
	}
    size_t *new_shape = m_new(size_t, ULAB_MAX_DIMS);
    memset(new_shape, 0, sizeof(size_t)*ULAB_MAX_DIMS);
    size_t new_length = 1;
    for(uint8_t i=0; i < shape->len; i++) {
        new_shape[ULAB_MAX_DIMS - i - 1] = mp_obj_get_int(shape->items[shape->len - i - 1]);
        new_length *= new_shape[ULAB_MAX_DIMS - i - 1];
    }
    if(source->len != new_length) {
        mp_raise_ValueError(translate("input and output shapes are not compatible"));
    }
    ndarray_obj_t *ndarray;
    if(ndarray_is_dense(source)) {
        // TODO: check if this is what numpy does
        int32_t *new_strides = strides_from_shape(new_shape, source->dtype);
        ndarray = ndarray_new_view(source, shape->len, new_shape, new_strides, 0);
    } else {
        ndarray = ndarray_new_ndarray_from_tuple(shape, source->dtype);
        ndarray_copy_array(source, ndarray);
    }
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_2(ndarray_reshape_obj, ndarray_reshape);
#endif

mp_obj_t ndarray_info(mp_obj_t obj_in) {
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(obj_in);
	if(!MP_OBJ_IS_TYPE(ndarray, &ulab_ndarray_type)) {
		mp_raise_TypeError(translate("function is defined for ndarrays only"));
    }
//    uint8_t *array = (uint8_t *)ndarray->array;
    mp_printf(MP_PYTHON_PRINTER, "class: ndarray\n");
    mp_printf(MP_PYTHON_PRINTER, "shape: (");
    if(ndarray->ndim == 1) {
        mp_printf(MP_PYTHON_PRINTER, "%d,", ndarray->shape[ULAB_MAX_DIMS-1]);
    } else {
        for(uint8_t i=0; i < ndarray->ndim-1; i++) mp_printf(MP_PYTHON_PRINTER, "%d, ", ndarray->shape[i]);
        mp_printf(MP_PYTHON_PRINTER, "%d", ndarray->shape[ULAB_MAX_DIMS-1]);
    }
    mp_printf(MP_PYTHON_PRINTER, ")\n");
    mp_printf(MP_PYTHON_PRINTER, "strides: (");
    if(ndarray->ndim == 1) {
        mp_printf(MP_PYTHON_PRINTER, "%d,", ndarray->strides[ULAB_MAX_DIMS-1]);
    } else {
        for(uint8_t i=0; i < ndarray->ndim-1; i++) mp_printf(MP_PYTHON_PRINTER, "%d, ", ndarray->strides[i]);
        mp_printf(MP_PYTHON_PRINTER, "%d", ndarray->strides[ULAB_MAX_DIMS-1]);
    }
    mp_printf(MP_PYTHON_PRINTER, ")\n");
    mp_printf(MP_PYTHON_PRINTER, "itemsize: %d\n", ndarray->itemsize);
    mp_printf(MP_PYTHON_PRINTER, "data pointer: 0x%p\n", ndarray->array);
    mp_printf(MP_PYTHON_PRINTER, "type: ");
    if(ndarray->boolean) {
		mp_printf(MP_PYTHON_PRINTER, "bool\n");
	} else if(ndarray->dtype == NDARRAY_UINT8) {
		mp_printf(MP_PYTHON_PRINTER, "uint8\n");
	} else if(ndarray->dtype == NDARRAY_INT8) {
		mp_printf(MP_PYTHON_PRINTER, "int8\n");
	} else if(ndarray->dtype == NDARRAY_UINT16) {
		mp_printf(MP_PYTHON_PRINTER, "uint16\n");
	} else if(ndarray->dtype == NDARRAY_INT16) {
		mp_printf(MP_PYTHON_PRINTER, "int16\n");
	} else if(ndarray->dtype == NDARRAY_FLOAT) {
		mp_printf(MP_PYTHON_PRINTER, "float\n");
	}
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(ndarray_info_obj, ndarray_info);

mp_int_t ndarray_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // buffer_p.get_buffer() returns zero for success, while mp_get_buffer returns true for success
    return !mp_get_buffer(self->array, bufinfo, flags);
}

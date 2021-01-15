/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020 Taku Fukada
*/

//| """Numerical and Statistical functions
//|
//| Most of these functions take an "axis" argument, which indicates whether to
//| operate over the flattened array (None), or a particular axis (integer)."""
//|
//| from ulab import _ArrayLike
//|

//| def argmax(array: _ArrayLike, *, axis: Optional[int] = None) -> int:
//|     """Return the index of the maximum element of the 1D array"""
//|     ...
//|

//| def argmin(array: _ArrayLike, *, axis: Optional[int] = None) -> int:
//|     """Return the index of the minimum element of the 1D array"""
//|     ...
//|

//| def argsort(array: ulab.array, *, axis: int = -1) -> ulab.array:
//|     """Returns an array which gives indices into the input array from least to greatest."""
//|     ...
//|

//| def cross(a: ulab.array, b: ulab.array) -> ulab.array:
//|     """Return the cross product of two vectors of length 3"""
//|     ...
//|

//| def diff(array: ulab.array, *, n: int = 1, axis: int = -1) -> ulab.array:
//|     """Return the numerical derivative of successive elements of the array, as
//|        an array.  axis=None is not supported."""
//|     ...
//|

//| def flip(array: ulab.array, *, axis: Optional[int] = None) -> ulab.array:
//|     """Returns a new array that reverses the order of the elements along the
//|        given axis, or along all axes if axis is None."""
//|     ...
//|

//| def max(array: _ArrayLike, *, axis: Optional[int] = None) -> float:
//|     """Return the maximum element of the 1D array"""
//|     ...
//|

//| def mean(array: _ArrayLike, *, axis: Optional[int] = None) -> float:
//|     """Return the mean element of the 1D array, as a number if axis is None, otherwise as an array."""
//|     ...
//|

//| def median(array: ulab.array, *, axis: int = -1) -> ulab.array:
//|     """Find the median value in an array along the given axis, or along all axes if axis is None."""
//|     ...
//|

//| def min(array: _ArrayLike, *, axis: Optional[int] = None) -> float:
//|     """Return the minimum element of the 1D array"""
//|     ...
//|

//| def roll(array: ulab.array, distance: int, *, axis: Optional[int] = None) -> None:
//|     """Shift the content of a vector by the positions given as the second
//|        argument. If the ``axis`` keyword is supplied, the shift is applied to
//|        the given axis.  The array is modified in place."""
//|     ...
//|

//| def sort(array: ulab.array, *, axis: int = -1) -> ulab.array:
//|     """Sort the array along the given axis, or along all axes if axis is None.
//|        The array is modified in place."""
//|     ...
//|

//| def sum(array: _ArrayLike, *, axis: Optional[int] = None) -> Union[float, int, ulab.array]:
//|     """Return the sum of the array, as a number if axis is None, otherwise as an array."""
//|     ...
//|

STATIC const mp_rom_map_elem_t ulab_numerical_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_numerical) },
    #if ULAB_NUMERICAL_HAS_ARGMINMAX
    { MP_OBJ_NEW_QSTR(MP_QSTR_argmax), (mp_obj_t)&numerical_argmax_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_argmin), (mp_obj_t)&numerical_argmin_obj },
    #endif
    #if ULAB_NUMERICAL_HAS_ARGSORT
    { MP_OBJ_NEW_QSTR(MP_QSTR_argsort), (mp_obj_t)&numerical_argsort_obj },
    #endif
    #if ULAB_NUMERICAL_HAS_CROSS
    { MP_OBJ_NEW_QSTR(MP_QSTR_cross), (mp_obj_t)&numerical_cross_obj },
    #endif
    #if ULAB_NUMERICAL_HAS_DIFF
    { MP_OBJ_NEW_QSTR(MP_QSTR_diff), (mp_obj_t)&numerical_diff_obj },
    #endif
    #if ULAB_NUMERICAL_HAS_FLIP
    { MP_OBJ_NEW_QSTR(MP_QSTR_flip), (mp_obj_t)&numerical_flip_obj },
    #endif
    #if ULAB_NUMERICAL_HAS_MINMAX
    { MP_OBJ_NEW_QSTR(MP_QSTR_max), (mp_obj_t)&numerical_max_obj },
    #endif
    #if ULAB_NUMERICAL_HAS_MEAN
    { MP_OBJ_NEW_QSTR(MP_QSTR_mean), (mp_obj_t)&numerical_mean_obj },
    #endif
    #if ULAB_NUMERICAL_HAS_MEDIAN
    { MP_OBJ_NEW_QSTR(MP_QSTR_median), (mp_obj_t)&numerical_median_obj },
    #endif
    #if ULAB_NUMERICAL_HAS_MINMAX
    { MP_OBJ_NEW_QSTR(MP_QSTR_min), (mp_obj_t)&numerical_min_obj },
    #endif
    #if ULAB_NUMERICAL_HAS_ROLL
    { MP_OBJ_NEW_QSTR(MP_QSTR_roll), (mp_obj_t)&numerical_roll_obj },
    #endif
    #if ULAB_NUMERICAL_HAS_SORT
    { MP_OBJ_NEW_QSTR(MP_QSTR_sort), (mp_obj_t)&numerical_sort_obj },
    #endif
    #if ULAB_NUMERICAL_HAS_STD
    { MP_OBJ_NEW_QSTR(MP_QSTR_std), (mp_obj_t)&numerical_std_obj },
    #endif
    #if ULAB_NUMERICAL_HAS_SUM
    { MP_OBJ_NEW_QSTR(MP_QSTR_sum), (mp_obj_t)&numerical_sum_obj },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_numerical_globals, ulab_numerical_globals_table);

mp_obj_module_t ulab_numerical_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_numerical_globals,
};


/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
*/

#ifndef __ULAB__
#define __ULAB__

// The pre-processor constants in this file determine how ulab behaves:
//
// - how many dimensions ulab can handle
// - which functions are included in the compiled firmware
// - whether the python syntax is numpy-like, or modular
// - whether arrays can be sliced and iterated over
// - which binary/unary operators are supported
//
// A considerable amount of flash space can be saved by removing (setting
// the corresponding constants to 0) the unnecessary functions and features.

// Determines, whether scipy is defined in ulab. The sub-modules and functions
// of scipy have to be defined separately
#define ULAB_HAS_SCIPY                      (1)

// The maximum number of dimensions the firmware should be able to support
// Possible values lie between 1, and 4, inclusive
#define ULAB_MAX_DIMS                       2

// By setting this constant to 1, iteration over array dimensions will be implemented
// as a function (ndarray_rewind_array), instead of writing out the loops in macros
// This reduces firmware size at the expense of speed
#define ULAB_HAS_FUNCTION_ITERATOR          (0)

// If NDARRAY_IS_ITERABLE is 1, the ndarray object defines its own iterator function
// This option saves approx. 250 bytes of flash space
#define NDARRAY_IS_ITERABLE                 (1)

// Slicing can be switched off by setting this variable to 0
#define NDARRAY_IS_SLICEABLE                (1)

// The default threshold for pretty printing. These variables can be overwritten
// at run-time via the set_printoptions() function
#define ULAB_HAS_PRINTOPTIONS               (1)
#define NDARRAY_PRINT_THRESHOLD             10
#define NDARRAY_PRINT_EDGEITEMS             3

// determines, whether the dtype is an object, or simply a character
// the object implementation is numpythonic, but requires more space
#define ULAB_HAS_DTYPE_OBJECT               (0)

// the ndarray binary operators
#define NDARRAY_HAS_BINARY_OPS              (1)

// Firmware size can be reduced at the expense of speed by using function
// pointers in iterations. For each operator, he function pointer saves around
// 2 kB in the two-dimensional case, and around 4 kB in the four-dimensional case.

#define NDARRAY_BINARY_USES_FUN_POINTER     (0)

#define NDARRAY_HAS_BINARY_OP_ADD           (1)
#define NDARRAY_HAS_BINARY_OP_EQUAL         (1)
#define NDARRAY_HAS_BINARY_OP_LESS          (1)
#define NDARRAY_HAS_BINARY_OP_LESS_EQUAL    (1)
#define NDARRAY_HAS_BINARY_OP_MORE          (1)
#define NDARRAY_HAS_BINARY_OP_MORE_EQUAL    (1)
#define NDARRAY_HAS_BINARY_OP_MULTIPLY      (1)
#define NDARRAY_HAS_BINARY_OP_NOT_EQUAL     (1)
#define NDARRAY_HAS_BINARY_OP_POWER         (1)
#define NDARRAY_HAS_BINARY_OP_SUBTRACT      (1)
#define NDARRAY_HAS_BINARY_OP_TRUE_DIVIDE   (1)

#define NDARRAY_HAS_INPLACE_OPS             (1)
#define NDARRAY_HAS_INPLACE_ADD             (1)
#define NDARRAY_HAS_INPLACE_MULTIPLY        (1)
#define NDARRAY_HAS_INPLACE_POWER           (1)
#define NDARRAY_HAS_INPLACE_SUBTRACT        (1)
#define NDARRAY_HAS_INPLACE_TRUE_DIVIDE     (1)

// the ndarray unary operators
#define NDARRAY_HAS_UNARY_OPS               (1)
#define NDARRAY_HAS_UNARY_OP_ABS            (1)
#define NDARRAY_HAS_UNARY_OP_INVERT         (1)
#define NDARRAY_HAS_UNARY_OP_LEN            (1)
#define NDARRAY_HAS_UNARY_OP_NEGATIVE       (1)
#define NDARRAY_HAS_UNARY_OP_POSITIVE       (1)

// determines, which ndarray methods are available
#define NDARRAY_HAS_COPY                (1)
#define NDARRAY_HAS_DTYPE               (1)
#define NDARRAY_HAS_FLATTEN             (1)
#define NDARRAY_HAS_ITEMSIZE            (1)
#define NDARRAY_HAS_RESHAPE             (1)
#define NDARRAY_HAS_SHAPE               (1)
#define NDARRAY_HAS_SIZE                (1)
#define NDARRAY_HAS_SORT                (1)
#define NDARRAY_HAS_STRIDES             (1)
#define NDARRAY_HAS_TOBYTES             (1)
#define NDARRAY_HAS_TRANSPOSE           (1)

// Firmware size can be reduced at the expense of speed by using a function
// pointer in iterations. Setting ULAB_VECTORISE_USES_FUNCPOINTER to 1 saves
// around 800 bytes in the four-dimensional case, and around 200 in two dimensions.
#define ULAB_VECTORISE_USES_FUN_POINTER (1)

// The default threshold for pretty printing. These variables can be overwritten
// at run-time via the set_printoptions() function
#define NDARRAY_PRINT_THRESHOLD         10
#define NDARRAY_PRINT_EDGEITEMS         3

// determines, whether e is defined in ulab.numpy itself
#define ULAB_NUMPY_HAS_E                (1)

// ulab defines infinite as a class constant in ulab.numpy
#define ULAB_NUMPY_HAS_INF              (1)

// ulab defines NaN as a class constant in ulab.numpy
#define ULAB_NUMPY_HAS_NAN              (1)

// determines, whether pi is defined in ulab.numpy itself
#define ULAB_NUMPY_HAS_PI               (1)

// determines, whether the ndinfo function is available
#define ULAB_NUMPY_HAS_NDINFO           (1)

// frombuffer adds 600 bytes to the firmware
#define ULAB_NUMPY_HAS_FROMBUFFER       (1)

// functions that create an array
#define ULAB_NUMPY_HAS_ARANGE           (1)
#define ULAB_NUMPY_HAS_CONCATENATE      (1)
#define ULAB_NUMPY_HAS_DIAG             (1)
#define ULAB_NUMPY_HAS_EYE              (1)
#define ULAB_NUMPY_HAS_FULL             (1)
#define ULAB_NUMPY_HAS_LINSPACE         (1)
#define ULAB_NUMPY_HAS_LOGSPACE         (1)
#define ULAB_NUMPY_HAS_ONES             (1)
#define ULAB_NUMPY_HAS_ZEROS            (1)

// functions that compare arrays
#define ULAB_NUMPY_HAS_CLIP             (1)
#define ULAB_NUMPY_HAS_EQUAL            (1)
#define ULAB_NUMPY_HAS_NOTEQUAL         (1)
#define ULAB_NUMPY_HAS_MAXIMUM          (1)
#define ULAB_NUMPY_HAS_MINIMUM          (1)

// the linalg module; functions of the linalg module still have
// to be defined separately
#ifndef ULAB_NUMPY_HAS_LINALG_MODULE
#define ULAB_NUMPY_HAS_LINALG_MODULE    (1)
#define ULAB_LINALG_HAS_CHOLESKY        (1)
#define ULAB_LINALG_HAS_DET             (1)
#define ULAB_LINALG_HAS_DOT             (1)
#define ULAB_LINALG_HAS_EIG             (1)
#define ULAB_LINALG_HAS_INV             (1)
#define ULAB_LINALG_HAS_NORM            (1)
#define ULAB_LINALG_HAS_TRACE           (1)
#endif

// the FFT module; functions of the fft module still have
// to be defined separately
#ifndef ULAB_NUMPY_HAS_FFT_MODULE
#define ULAB_NUMPY_HAS_FFT_MODULE       (1)
#define ULAB_FFT_HAS_FFT                (1)
#define ULAB_FFT_HAS_IFFT               (1)
#endif

#define ULAB_NUMPY_HAS_ARGMINMAX        (1)
#define ULAB_NUMPY_HAS_ARGSORT          (1)
#define ULAB_NUMPY_HAS_CONVOLVE         (1)
#define ULAB_NUMPY_HAS_CROSS            (1)
#define ULAB_NUMPY_HAS_DIFF             (1)
#define ULAB_NUMPY_HAS_FLIP             (1)
#define ULAB_NUMPY_HAS_INTERP           (1)
#define ULAB_NUMPY_HAS_MEAN             (1)
#define ULAB_NUMPY_HAS_MEDIAN           (1)
#define ULAB_NUMPY_HAS_MINMAX           (1)
#define ULAB_NUMPY_HAS_POLYFIT          (1)
#define ULAB_NUMPY_HAS_POLYVAL          (1)
#define ULAB_NUMPY_HAS_ROLL             (1)
#define ULAB_NUMPY_HAS_SORT             (1)
#define ULAB_NUMPY_HAS_STD              (1)
#define ULAB_NUMPY_HAS_SUM              (1)
#define ULAB_NUMPY_HAS_TRAPZ            (1)

// vectorised versions of the functions of the math python module, with
// the exception of the functions listed in scipy.special
#define ULAB_NUMPY_HAS_ACOS             (1)
#define ULAB_NUMPY_HAS_ACOSH            (1)
#define ULAB_NUMPY_HAS_ARCTAN2          (1)
#define ULAB_NUMPY_HAS_AROUND           (1)
#define ULAB_NUMPY_HAS_ASIN             (1)
#define ULAB_NUMPY_HAS_ASINH            (1)
#define ULAB_NUMPY_HAS_ATAN             (1)
#define ULAB_NUMPY_HAS_ATANH            (1)
#define ULAB_NUMPY_HAS_CEIL             (1)
#define ULAB_NUMPY_HAS_COS              (1)
#define ULAB_NUMPY_HAS_COSH             (1)
#define ULAB_NUMPY_HAS_DEGREES          (1)
#define ULAB_NUMPY_HAS_EXP              (1)
#define ULAB_NUMPY_HAS_EXPM1            (1)
#define ULAB_NUMPY_HAS_FLOOR            (1)
#define ULAB_NUMPY_HAS_LOG              (1)
#define ULAB_NUMPY_HAS_LOG10            (1)
#define ULAB_NUMPY_HAS_LOG2             (1)
#define ULAB_NUMPY_HAS_RADIANS          (1)
#define ULAB_NUMPY_HAS_SIN              (1)
#define ULAB_NUMPY_HAS_SINH             (1)
#define ULAB_NUMPY_HAS_SQRT             (1)
#define ULAB_NUMPY_HAS_TAN              (1)
#define ULAB_NUMPY_HAS_TANH             (1)
#define ULAB_NUMPY_HAS_VECTORIZE        (1)


#define ULAB_SCIPY_HAS_SIGNAL_MODULE        (1)
#define ULAB_SCIPY_SIGNAL_HAS_SPECTROGRAM   (1)
#define ULAB_SCIPY_SIGNAL_HAS_SOSFILT       (1)


#define ULAB_SCIPY_HAS_OPTIMIZE_MODULE      (1)
#define ULAB_SCIPY_OPTIMIZE_HAS_BISECT      (1)
#define ULAB_SCIPY_OPTIMIZE_HAS_CURVE_FIT   (0) // not fully implemented
#define ULAB_SCIPY_OPTIMIZE_HAS_FMIN        (1)
#define ULAB_SCIPY_OPTIMIZE_HAS_NEWTON      (1)


#define ULAB_SCIPY_HAS_SPECIAL_MODULE       (1)
#define ULAB_SCIPY_SPECIAL_HAS_ERF          (1)
#define ULAB_SCIPY_SPECIAL_HAS_ERFC         (1)
#define ULAB_SCIPY_SPECIAL_HAS_GAMMA        (1)
#define ULAB_SCIPY_SPECIAL_HAS_GAMMALN      (1)

// user-defined module; source of the module and
// its sub-modules should be placed in code/user/
#ifndef ULAB_HAS_USER_MODULE
#define ULAB_HAS_USER_MODULE                (0)
#endif

#endif


/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#ifndef __ULAB__
#define __ULAB__

// Setting this variable to 1 produces numpy-compatible firmware, 
// i.e., functions can be called at the top level, 
// without having to import the sub-modules (linalg and fft are exceptions,
// since those must be imported even in numpy)
#define ULAB_NUMPY_COMPATIBILITY        (1)

// The maximum number of dimensions the firmware should be able to support
// Possible values lie between 1, and 4, inclusive
#define ULAB_MAX_DIMS                   2

// The default threshold for pretty printing. These variables can be overwritten
// at run-time via the set_printoptions() function
#define NDARRAY_PRINT_THRESHOLD         10
#define NDARRAY_PRINT_EDGEITEMS         3

// determines, whether pi, and e are defined in ulab itself
#define ULAB_HAS_MATH_CONSTANTS         (1)

// the approx module consumes about 4.5 kB of flash space
#ifndef ULAB_APPROX_MODULE
#define ULAB_APPROX_MODULE              (1)
#define ULAB_APPROX_HAS_BISECT          (1)
#define ULAB_APPROX_HAS_CURVE_FIT       (0) // not fully implemented, set it to 0 for now
#define ULAB_APPROX_HAS_FMIN            (1)
#define ULAB_APPROX_HAS_INTERP          (1)
#define ULAB_APPROX_HAS_NEWTON          (1)
#define ULAB_APPROX_HAS_TRAPZ           (1)
#endif

// the compare module consumes about 12 kB of flash space
#ifndef ULAB_COMPARE_MODULE
#define ULAB_COMPARE_MODULE             (1)
#define ULAB_COMPARE_HAS_CLIP           (1)
#define ULAB_COMPARE_HAS_EQUAL          (1)
#define ULAB_COMPARE_HAS_NOTEQUAL       (1)
#define ULAB_COMPARE_HAS_MAXIMUM        (1)
#define ULAB_COMPARE_HAS_MINIMUM        (1)
#endif

// These are the array initialisation functions that
// are included at the top level, hence the missing
// module constant
#define ULAB_CREATE_HAS_ARANGE          (1)
#define ULAB_CREATE_HAS_EYE             (1)
#define ULAB_CREATE_HAS_LINSPACE        (1)
#define ULAB_CREATE_HAS_LOGSPACE        (0) // not implemented, set it to 0 for now
#define ULAB_CREATE_HAS_ONES            (1)
#define ULAB_CREATE_HAS_ZEROS           (1)

// FFT costs about 2.5 kB of flash space
// Since all FFT module functions are just interfaces to the same kernel, 
// with virtually no overhead, there is no point in defining ULAB_FFT_HAS_* constants
#ifndef ULAB_FFT_MODULE
#define ULAB_FFT_MODULE                 (1)
#endif

// the filter module occupies about 1 kB of flash space
#ifndef ULAB_FILTER_MODULE
#define ULAB_FILTER_MODULE              (1)
#define ULAB_FILTER_HAS_CONVOLVE        (1)
#define ULAB_FILTER_HAS_SOSFILT         (1)
#endif

// linalg adds around 6 kB to the firmware
#ifndef ULAB_LINALG_MODULE
#define ULAB_LINALG_MODULE              (1)
#define ULAB_LINALG_HAS_CHOLESKY        (1)
#define ULAB_LINALG_HAS_DET             (1)
#define ULAB_LINALG_HAS_DOT             (1)
#define ULAB_LINALG_HAS_EIG             (1)
#define ULAB_LINALG_HAS_INV             (1)
#define ULAB_LINALG_HAS_NORM            (1)
#define ULAB_LINALG_HAS_SIZE            (1)
#define ULAB_LINALG_HAS_TRACE           (1)
#endif

// numerical is about 12 kB
#ifndef ULAB_NUMERICAL_MODULE
#define ULAB_NUMERICAL_MODULE           (1)
#define ULAB_NUMERICAL_HAS_MINMAX       (1)
#define ULAB_NUMERICAL_HAS_ARGMINMAX    (1)
#define ULAB_NUMERICAL_HAS_ARGSORT      (1)
#define ULAB_NUMERICAL_HAS_DIFF         (1)
#define ULAB_NUMERICAL_HAS_FLIP         (1)
#define ULAB_NUMERICAL_HAS_MEAN         (1)
#define ULAB_NUMERICAL_HAS_ROLL         (1)
#define ULAB_NUMERICAL_HAS_SORT         (1)
#define ULAB_NUMERICAL_HAS_STD          (1)
#define ULAB_NUMERICAL_HAS_SUM          (1)
#endif

// poly requires approx. 2.5 kB
#ifndef ULAB_POLY_MODULE
#define ULAB_POLY_MODULE                (1)
#define ULAB_POLY_HAS_POLYFIT           (1)
#define ULAB_POLY_HAS_POLYVAL           (1)
#endif

// vectorise (all functions) takes approx. 6 kB of flash space
#ifndef ULAB_VECTORISE_MODULE
#define ULAB_VECTORISE_MODULE           (1)
#define ULAB_VECTORISE_HAS_ACOS         (1)
#define ULAB_VECTORISE_HAS_ACOSH        (1)
#define ULAB_VECTORISE_HAS_ARCTAN2      (0)
#define ULAB_VECTORISE_HAS_AROUND       (1)
#define ULAB_VECTORISE_HAS_ASIN         (1)
#define ULAB_VECTORISE_HAS_ASINH        (1)
#define ULAB_VECTORISE_HAS_ATAN         (1)
#define ULAB_VECTORISE_HAS_ATANH        (1)
#define ULAB_VECTORISE_HAS_CEIL         (1)
#define ULAB_VECTORISE_HAS_COS          (1)
#define ULAB_VECTORISE_HAS_DEGREES      (1)
#define ULAB_VECTORISE_HAS_ERF          (1)
#define ULAB_VECTORISE_HAS_ERFC         (1)
#define ULAB_VECTORISE_HAS_EXP          (1)
#define ULAB_VECTORISE_HAS_EXPM1        (1)
#define ULAB_VECTORISE_HAS_FLOOR        (1)
#define ULAB_VECTORISE_HAS_GAMMA        (1)
#define ULAB_VECTORISE_HAS_LGAMMA       (1)
#define ULAB_VECTORISE_HAS_LOG          (1)
#define ULAB_VECTORISE_HAS_LOG10        (1)
#define ULAB_VECTORISE_HAS_LOG2         (1)
#define ULAB_VECTORISE_HAS_RADIANS      (1)
#define ULAB_VECTORISE_HAS_SIN          (1)
#define ULAB_VECTORISE_HAS_SINH         (1)
#define ULAB_VECTORISE_HAS_SQRT         (1)
#define ULAB_VECTORISE_HAS_TAN          (1)
#define ULAB_VECTORISE_HAS_TANH         (1)
#define ULAB_VECTORISE_HAS_VECTORIZE    (1)
#endif

// user-defined module
#ifndef ULAB_USER_MODULE
#define ULAB_USER_MODULE                (0)
#endif

#endif

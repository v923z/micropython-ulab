
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#ifndef __ULAB_NUMPY_DEFS__
#define __ULAB_NUMPY_DEFS__

// The pre-processor constants in this file determine which functions 
// the numpy module has

// The default threshold for pretty printing. These variables can be overwritten
// at run-time via the set_printoptions() function
#define NDARRAY_PRINT_THRESHOLD         10
#define NDARRAY_PRINT_EDGEITEMS         3

// determines, whether e is defined in ulab.numpy itself
#define ULAB_HAS_E                      (1)

// ulab defines infinite as a class constant in ulab.numpy
#define ULAB_HAS_INF                    (1)

// ulab defines NaN as a class constant in ulab.numpy
#define ULAB_HAS_NAN                    (1)

// determines, whether pi is defined in ulab.numpy itself
#define ULAB_HAS_PI                     (1)

// determines, whether the ndinfo function is available
#define ULAB_HAS_NDINFO                 (1)

// determines, whether the dtype is an object, or simply a character
// the object implementation is numpythonic, but requires more space
#define ULAB_HAS_DTYPE_OBJECT           (0)

// the ndarray binary operators
#define NDARRAY_HAS_BINARY_OPS              (1)

// functions that create an array
#define ULAB_NUMPY_HAS_ARANGE          	(1)
#define ULAB_NUMPY_HAS_CONCATENATE     	(1)
#define ULAB_NUMPY_HAS_DIAG            	(1)
#define ULAB_NUMPY_HAS_EYE             	(1)
#define ULAB_NUMPY_HAS_FULL            	(1)
#define ULAB_NUMPY_HAS_LINSPACE        	(1)
#define ULAB_NUMPY_HAS_LOGSPACE        	(1)
#define ULAB_NUMPY_HAS_ONES            	(1)
#define ULAB_NUMPY_HAS_ZEROS           	(1)

// functions that compare arrays
#define ULAB_NUMPY_HAS_CLIP          	(1)
#define ULAB_NUMPY_HAS_EQUAL          	(1)
#define ULAB_NUMPY_HAS_NOTEQUAL       	(1)
#define ULAB_NUMPY_HAS_MAXIMUM        	(1)
#define ULAB_NUMPY_HAS_MINIMUM        	(1)

// the linalg module; functions of the linalg module still have 
// to be defined separately
#ifndef ULAB_NUMPY_HAS_LINALG_MODULE
#define ULAB_NUMPY_HAS_LINALG_MODULE	(1)
#define ULAB_LINALG_HAS_CHOLESKY		(1)
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
#define ULAB_FFT_MODULE                 (1)
#define ULAB_FFT_HAS_FFT           		(1)
#define ULAB_FFT_HAS_IFFT           	(1)
#endif

#define ULAB_NUMPY_HAS_ARGMINMAX    	(1)
#define ULAB_NUMPY_HAS_ARGSORT      	(1)
#define ULAB_NUMPY_HAS_CONVOLVE        	(1)
#define ULAB_NUMPY_HAS_CROSS        	(1)
#define ULAB_NUMPY_HAS_DIFF         	(1)
#define ULAB_NUMPY_HAS_FLIP         	(1)
#define ULAB_NUMPY_HAS_INTERP			(1)
#define ULAB_NUMPY_HAS_MEAN         	(1)
#define ULAB_NUMPY_HAS_MEDIAN       	(1)
#define ULAB_NUMPY_HAS_MINMAX       	(1)
#define ULAB_NUMPY_HAS_POLYFIT          (1)
#define ULAB_NUMPY_HAS_POLYVAL          (1)
#define ULAB_NUMPY_HAS_ROLL         	(1)
#define ULAB_NUMPY_HAS_SORT         	(1)
#define ULAB_NUMPY_HAS_STD          	(1)
#define ULAB_NUMPY_HAS_SUM          	(1)
#define ULAB_NUMPY_HAS_TRAPZ			(1)

// vectorised versions of the functions of the math python module, with 
// the exception of the functions listed in scipy.special
#define ULAB_NUMPY_HAS_ACOS         	(1)
#define ULAB_NUMPY_HAS_ACOSH        	(1)
#define ULAB_NUMPY_HAS_ARCTAN2      	(1)
#define ULAB_NUMPY_HAS_AROUND       	(1)
#define ULAB_NUMPY_HAS_ASIN         	(1)
#define ULAB_NUMPY_HAS_ASINH        	(1)
#define ULAB_NUMPY_HAS_ATAN         	(1)
#define ULAB_NUMPY_HAS_ATANH        	(1)
#define ULAB_NUMPY_HAS_CEIL         	(1)
#define ULAB_NUMPY_HAS_COS          	(1)
#define ULAB_NUMPY_HAS_COSH         	(1)
#define ULAB_NUMPY_HAS_DEGREES      	(1)
#define ULAB_NUMPY_HAS_EXP          	(1)
#define ULAB_NUMPY_HAS_EXPM1        	(1)
#define ULAB_NUMPY_HAS_FLOOR        	(1)
#define ULAB_NUMPY_HAS_LOG          	(1)
#define ULAB_NUMPY_HAS_LOG10        	(1)
#define ULAB_NUMPY_HAS_LOG2         	(1)
#define ULAB_NUMPY_HAS_RADIANS      	(1)
#define ULAB_NUMPY_HAS_SIN          	(1)
#define ULAB_NUMPY_HAS_SINH         	(1)
#define ULAB_NUMPY_HAS_SQRT         	(1)
#define ULAB_NUMPY_HAS_TAN          	(1)
#define ULAB_NUMPY_HAS_TANH         	(1)
#define ULAB_NUMPY_HAS_VECTORIZE    	(1)

#endif


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

// vectorise (all functions) takes approx. 3 kB of flash space
#define ULAB_VECTORISE_ACOS (1)
#define ULAB_VECTORISE_ACOSH (1)
#define ULAB_VECTORISE_ASIN (1)
#define ULAB_VECTORISE_ASINH (1)
#define ULAB_VECTORISE_ATAN (1)
#define ULAB_VECTORISE_ATANH (1)
#define ULAB_VECTORISE_CEIL (1)
#define ULAB_VECTORISE_COS (1)
#define ULAB_VECTORISE_ERF (1)
#define ULAB_VECTORISE_ERFC (1)
#define ULAB_VECTORISE_EXP (1)
#define ULAB_VECTORISE_EXPM1 (1)
#define ULAB_VECTORISE_FLOOR (1)
#define ULAB_VECTORISE_GAMMA (1)
#define ULAB_VECTORISE_LGAMMA (1)
#define ULAB_VECTORISE_LOG (1)
#define ULAB_VECTORISE_LOG10 (1)
#define ULAB_VECTORISE_LOG2 (1)
#define ULAB_VECTORISE_SIN (1)
#define ULAB_VECTORISE_SINH (1)
#define ULAB_VECTORISE_SQRT (1)
#define ULAB_VECTORISE_TAN (1)
#define ULAB_VECTORISE_TANH (1)

// linalg adds around 6 kB
#define ULAB_LINALG_TRANSPOSE (1)
#define ULAB_LINALG_RESHAPE (1)
#define ULAB_LINALG_SIZE (1)
#define ULAB_LINALG_INV (1)
#define ULAB_LINALG_DOT (1)
#define ULAB_LINALG_ZEROS (1)
#define ULAB_LINALG_ONES (1)
#define ULAB_LINALG_EYE (1)
#define ULAB_LINALG_DET (1)
#define ULAB_LINALG_EIG (1)

// poly is approx. 2.5 kB
#define ULAB_POLY_POLYVAL (1)
#define ULAB_POLY_POLYFIT (1)

//
#define ULAB_NUMERICAL_LINSPACE (1)
#define ULAB_NUMERICAL_SUM (1)
#define ULAB_NUMERICAL_MEAN (1)
#define ULAB_NUMERICAL_STD (1)
#define ULAB_NUMERICAL_MIN (1)
#define ULAB_NUMERICAL_MAX (1)
#define ULAB_NUMERICAL_ARGMIN (1)
#define ULAB_NUMERICAL_ARGMAX (1)
#define ULAB_NUMERICAL_ROLL (1)
#define ULAB_NUMERICAL_FLIP (1)
#define ULAB_NUMERICAL_DIFF (1)
#define ULAB_NUMERICAL_SORT (1)
#define ULAB_NUMERICAL_ARGSORT (1)

// FFT costs about 2 kB of flash space
#define ULAB_FFT_FFT (1)
#define ULAB_FFT_IFFT (1)
#define ULAB_FFT_SPECTRUM (1)

// the filter module takes about 0.8 kB of flash space
#define ULAB_FILTER_CONVOLVE (1)

#endif

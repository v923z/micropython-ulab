
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

// create
#define ULAB_CREATE_MODULE (1)

// vectorise (all functions) takes approx. 3 kB of flash space
#define ULAB_VECTORISE_MODULE (1)

// linalg adds around 6 kB
#define ULAB_LINALG_MODULE (1)

// poly is approx. 2.5 kB
#define ULAB_POLY_MODULE (1)

// numerical is about 12 kB
#define ULAB_NUMERICAL_MODULE (1)

// FFT costs about 2 kB of flash space
#define ULAB_FFT_MODULE (1)

// the filter module takes about 1 kB of flash space
#define ULAB_FILTER_MODULE (1)

// user-defined modules
#define ULAB_EXTRAS_MODULE (0)

#endif

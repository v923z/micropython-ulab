
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

// the create module is always included
#define ULAB_CREATE_MODULE (1)

// vectorise (all functions) takes approx. 6 kB of flash space
#ifndef ULAB_VECTORISE_MODULE
#define ULAB_VECTORISE_MODULE (1)
#endif

// linalg adds around 6 kB to the firmware
#ifndef ULAB_LINALG_MODULE
#define ULAB_LINALG_MODULE (1)
#endif

// poly requires approx. 2.5 kB
#ifndef ULAB_POLY_MODULE
#define ULAB_POLY_MODULE (1)
#endif

// numerical is about 12 kB
#ifndef ULAB_NUMERICAL_MODULE
#define ULAB_NUMERICAL_MODULE (1)
#endif

// FFT costs about 2 kB of flash space
#ifndef ULAB_FFT_MODULE
#define ULAB_FFT_MODULE (1)
#endif

// the filter module occupies about 1 kB of flash space
#ifndef ULAB_FILTER_MODULE
#define ULAB_FILTER_MODULE (1)
#endif

// the compare module consumes about 4 kB of flash space
#ifndef ULAB_COMPARE_MODULE
#define ULAB_COMPARE_MODULE (1)
#endif

// the approx module consumes about ... kB of flash space
#ifndef ULAB_APPROX_MODULE
#define ULAB_APPROX_MODULE (1)
#endif

// user-defined modules
#ifndef ULAB_EXTRAS_MODULE
#define ULAB_EXTRAS_MODULE (1)
#endif

#endif

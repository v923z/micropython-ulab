/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#include <math.h>
#include "py/runtime.h"

#include "fft_tools.h"

#ifndef MP_PI
#define MP_PI MICROPY_FLOAT_CONST(3.14159265358979323846)
#endif
#ifndef MP_E
#define MP_E MICROPY_FLOAT_CONST(2.71828182845904523536)
#endif

#define SWAP(t, a, b) { t tmp = a; a = b; b = tmp; }

/* 
 * The following function takes two arrays, namely, the real and imaginary 
 * parts of a complex array, and calculates the Fourier transform in place.
 * 
 * The function is basically a modification of four1 from Numerical Recipes, 
 * has no dependencies beyond the micropython itself (for the definition of mp_float_t),
 * and can be used independent of ulab.
 */
 
void fft_kernel(mp_float_t *real, mp_float_t *imag, size_t n, int isign) {
    size_t j, m, mmax, istep;
    mp_float_t tempr, tempi;
    mp_float_t wtemp, wr, wpr, wpi, wi, theta;

    j = 0;
    for(size_t i = 0; i < n; i++) {
        if (j > i) {
            SWAP(mp_float_t, real[i], real[j]);
            SWAP(mp_float_t, imag[i], imag[j]);
        }
        m = n >> 1;
        while (j >= m && m > 0) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }

    mmax = 1;
    while (n > mmax) {
        istep = mmax << 1;
        theta = MICROPY_FLOAT_CONST(-2.0)*isign*MP_PI/istep;
        wtemp = MICROPY_FLOAT_C_FUN(sin)(MICROPY_FLOAT_CONST(0.5) * theta);
        wpr = MICROPY_FLOAT_CONST(-2.0) * wtemp * wtemp;
        wpi = MICROPY_FLOAT_C_FUN(sin)(theta);
        wr = MICROPY_FLOAT_CONST(1.0);
        wi = MICROPY_FLOAT_CONST(0.0);
        for(m = 0; m < mmax; m++) {
            for(size_t i = m; i < n; i += istep) {
                j = i + mmax;
                tempr = wr * real[j] - wi * imag[j];
                tempi = wr * imag[j] + wi * real[j];
                real[j] = real[i] - tempr;
                imag[j] = imag[i] - tempi;
                real[i] += tempr;
                imag[i] += tempi;
            }
            wtemp = wr;
            wr = wr*wpr - wi*wpi + wr;
            wi = wi*wpr + wtemp*wpi + wi;
        }
        mmax = istep;
    }
}

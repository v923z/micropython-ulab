#pragma once
#include "f2c.h"


#if(MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE)
#define ZERO 0.0
#else
#define ZERO 0.0f
#endif

/* Subroutine */ int axpy_(const integer *n, const mp_float_t *da, const mp_float_t *dx, 
	const integer *incx, mp_float_t *dy, const integer *incy)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, ix, iy, mp1;

    --dy;
    --dx;

    /* Function Body */
    if (*n <= 0) {
	return 0;
    }
    if (*da == ZERO) {
	return 0;
    }
    if (*incx == 1 && *incy == 1) {
	m = *n % 4;
	if (m != 0) {
	    i__1 = m;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		dy[i__] += *da * dx[i__];
	    }
	}
	if (*n < 4) {
	    return 0;
	}
	mp1 = m + 1;
	i__1 = *n;
	for (i__ = mp1; i__ <= i__1; i__ += 4) {
	    dy[i__] += *da * dx[i__];
	    dy[i__ + 1] += *da * dx[i__ + 1];
	    dy[i__ + 2] += *da * dx[i__ + 2];
	    dy[i__ + 3] += *da * dx[i__ + 3];
	}
    } else {

/*        code for unequal increments or equal increments */
/*          not equal to 1 */

	ix = 1;
	iy = 1;
	if (*incx < 0) {
	    ix = (-(*n) + 1) * *incx + 1;
	}
	if (*incy < 0) {
	    iy = (-(*n) + 1) * *incy + 1;
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dy[iy] += *da * dx[ix];
	    ix += *incx;
	    iy += *incy;
	}
    }
    return 0;
} /* axpy_ */

void cblas_axpy( const int N, const mp_float_t alpha, const mp_float_t *X,
                       const int incX, mp_float_t *Y, const int incY)
{
   long int F77_N=N, F77_incX=incX, F77_incY=incY;
   axpy_( &F77_N, &alpha, X, &F77_incX, Y, &F77_incY);
}

///SDOT
mp_float_t sdot_(const integer *n, const mp_float_t *sx, const integer *incx, const mp_float_t *sy, const integer *incy)
{
    /* System generated locals */
    integer i__1;
    mp_float_t ret_val;

    /* Local variables */
    static integer i__, m, ix, iy, mp1;
    static mp_float_t stemp;

    /* Parameter adjustments */
    --sy;
    --sx;

    /* Function Body */
    stemp = ZERO;
    ret_val = ZERO;
    if (*n <= 0) {
	return ret_val;
    }
    if (*incx == 1 && *incy == 1) {

	m = *n % 5;
	if (m != 0) {
	    i__1 = m;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		stemp += sx[i__] * sy[i__];
	    }
	    if (*n < 5) {
		ret_val = stemp;
		return ret_val;
	    }
	}
	mp1 = m + 1;
	i__1 = *n;
	for (i__ = mp1; i__ <= i__1; i__ += 5) {
	    stemp = stemp + sx[i__] * sy[i__] + sx[i__ + 1] * sy[i__ + 1] + 
		    sx[i__ + 2] * sy[i__ + 2] + sx[i__ + 3] * sy[i__ + 3] + 
		    sx[i__ + 4] * sy[i__ + 4];
	}
    } else {

	ix = 1;
	iy = 1;
	if (*incx < 0) {
	    ix = (-(*n) + 1) * *incx + 1;
	}
	if (*incy < 0) {
	    iy = (-(*n) + 1) * *incy + 1;
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    stemp += sx[ix] * sy[iy];
	    ix += *incx;
	    iy += *incy;
	}
    }
    ret_val = stemp;
    return ret_val;
} /* sdot_ */


/* Subroutine */ int sdotsub_(const integer *n, const mp_float_t *x, const integer *incx, const mp_float_t *y, 
	const integer *incy, mp_float_t *dot)
{
    /* Parameter adjustments */
    --y;
    --x;

    /* Function Body */
    *dot = sdot_(n, &x[1], incx, &y[1], incy);
    return 0;
} /* sdotsub_ */


mp_float_t cblas_dot( const int N, const mp_float_t *X,
                      const int incX, const mp_float_t *Y, const int incY)
{
   mp_float_t dot;
   long int F77_N=N, F77_incX=incX, F77_incY=incY;
   sdotsub_( &F77_N, X, &F77_incX, Y, &F77_incY, &dot);
   return dot;
}


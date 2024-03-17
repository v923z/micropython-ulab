
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Vikas Udupa
 *               2024 Zoltán Vörös
 *
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"

#include "../../ulab.h"
#include "../../ulab_tools.h"
#include "../../numpy/linalg/linalg_tools.h"
#include "linalg.h"

#if ULAB_SCIPY_HAS_LINALG_MODULE
//|
//| import ulab.scipy
//| import ulab.numpy
//|
//| """Linear algebra functions"""
//|

#if ULAB_MAX_DIMS > 1

#if ULAB_SCIPY_LINALG_HAS_SOLVE_TRIANGULAR
//| def solve_triangular(A: ulab.numpy.ndarray, b: ulab.numpy.ndarray, lower: bool) -> ulab.numpy.ndarray:
//|    """
//|    :param ~ulab.numpy.ndarray A: a matrix
//|    :param ~ulab.numpy.ndarray b: a vector
//|    :param ~bool lower: if true, use only data contained in lower triangle of A, else use upper triangle of A
//|    :return: solution to the system A x = b. Shape of return matches b
//|    :raises TypeError: if A and b are not of type ndarray and are not dense
//|    :raises ValueError: if A is a singular matrix
//|
//|    Solve the equation A x = b for x, assuming A is a triangular matrix"""
//|    ...
//|

static mp_obj_t solve_triangular(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    size_t i, j;

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE} } ,
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE} } ,
        { MP_QSTR_lower, MP_ARG_OBJ, { .u_rom_obj = MP_ROM_TRUE } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type) || !mp_obj_is_type(args[1].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(MP_ERROR_TEXT("first two arguments must be ndarrays"));
    }

    ndarray_obj_t *A = MP_OBJ_TO_PTR(args[0].u_obj);
    ndarray_obj_t *b = MP_OBJ_TO_PTR(args[1].u_obj);

    if(!ndarray_is_dense(A) || !ndarray_is_dense(b)) {
        mp_raise_TypeError(MP_ERROR_TEXT("input must be a dense ndarray"));
    }

    size_t A_rows = A->shape[ULAB_MAX_DIMS - 2];
    size_t A_cols = A->shape[ULAB_MAX_DIMS - 1];

    uint8_t *A_arr = (uint8_t *)A->array;
    uint8_t *b_arr = (uint8_t *)b->array;

    mp_float_t (*get_A_ele)(void *) = ndarray_get_float_function(A->dtype);
    mp_float_t (*get_b_ele)(void *) = ndarray_get_float_function(b->dtype);

    uint8_t *temp_A = A_arr;

    // check if input matrix A is singular
    for (i = 0; i < A_rows; i++) {
        if (MICROPY_FLOAT_C_FUN(fabs)(get_A_ele(A_arr)) < LINALG_EPSILON)
            mp_raise_ValueError(MP_ERROR_TEXT("input matrix is singular"));
        A_arr += A->strides[ULAB_MAX_DIMS - 2];
        A_arr += A->strides[ULAB_MAX_DIMS - 1];
    }

    A_arr = temp_A;

    ndarray_obj_t *x = ndarray_new_dense_ndarray(b->ndim, b->shape, NDARRAY_FLOAT);
    mp_float_t *x_arr = (mp_float_t *)x->array;

    if (mp_obj_is_true(args[2].u_obj)) {
        // Solve the lower triangular matrix by iterating each row of A.
        // Start by finding the first unknown using the first row.
        // On finding this unknown, find the second unknown using the second row.
        // Continue the same till the last unknown is found using the last row.

        for (i = 0; i < A_rows; i++) {
            mp_float_t sum = 0.0;
            for (j = 0; j < i; j++) {
                sum += (get_A_ele(A_arr) * (*x_arr++));
                A_arr += A->strides[ULAB_MAX_DIMS - 1];
            }

            sum = (get_b_ele(b_arr) - sum) / (get_A_ele(A_arr));
            *x_arr = sum;

            x_arr -= j;
            A_arr -= A->strides[ULAB_MAX_DIMS - 1] * j;
            A_arr += A->strides[ULAB_MAX_DIMS - 2];
            b_arr += b->strides[ULAB_MAX_DIMS - 1];
        }
    } else {
        // Solve the upper triangular matrix by iterating each row of A.
        // Start by finding the last unknown using the last row.
        // On finding this unknown, find the last-but-one unknown using the last-but-one row.
        // Continue the same till the first unknown is found using the first row.

        A_arr += (A->strides[ULAB_MAX_DIMS - 2] * A_rows);
        b_arr += (b->strides[ULAB_MAX_DIMS - 1] * A_cols);
        x_arr += A_cols;

        for (i = A_rows - 1; i < A_rows; i--) {
            mp_float_t sum = 0.0;
            for (j = i + 1; j < A_cols; j++) {
                sum += (get_A_ele(A_arr) * (*x_arr++));
                A_arr += A->strides[ULAB_MAX_DIMS - 1];
            }

            x_arr -= (j - i);
            A_arr -= (A->strides[ULAB_MAX_DIMS - 1] * (j - i));
            b_arr -= b->strides[ULAB_MAX_DIMS - 1];

            sum = (get_b_ele(b_arr) - sum) / get_A_ele(A_arr);
            *x_arr = sum;

            A_arr -= A->strides[ULAB_MAX_DIMS - 2];
        }
    }

    return MP_OBJ_FROM_PTR(x);
}

MP_DEFINE_CONST_FUN_OBJ_KW(linalg_solve_triangular_obj, 2, solve_triangular);
#endif /* ULAB_SCIPY_LINALG_HAS_SOLVE_TRIANGULAR */

#if ULAB_SCIPY_LINALG_HAS_CHO_SOLVE

//| def cho_solve(L: ulab.numpy.ndarray, b: ulab.numpy.ndarray) -> ulab.numpy.ndarray:
//|    """
//|    :param ~ulab.numpy.ndarray L: the lower triangular, Cholesky factorization of A
//|    :param ~ulab.numpy.ndarray b: right-hand-side vector b
//|    :return: solution to the system A x = b. Shape of return matches b
//|    :raises TypeError: if L and b are not of type ndarray and are not dense
//|
//|    Solve the linear equations A x = b, given the Cholesky factorization of A as input"""
//|    ...
//|

static mp_obj_t cho_solve(mp_obj_t _L, mp_obj_t _b) {

    if(!mp_obj_is_type(_L, &ulab_ndarray_type) || !mp_obj_is_type(_b, &ulab_ndarray_type)) {
        mp_raise_TypeError(MP_ERROR_TEXT("first two arguments must be ndarrays"));
    }

    ndarray_obj_t *L = MP_OBJ_TO_PTR(_L);
    ndarray_obj_t *b = MP_OBJ_TO_PTR(_b);

    if(!ndarray_is_dense(L) || !ndarray_is_dense(b)) {
        mp_raise_TypeError(MP_ERROR_TEXT("input must be a dense ndarray"));
    }

    mp_float_t (*get_L_ele)(void *) = ndarray_get_float_function(L->dtype);
    mp_float_t (*get_b_ele)(void *) = ndarray_get_float_function(b->dtype);
    void (*set_L_ele)(void *, mp_float_t) = ndarray_set_float_function(L->dtype);

    size_t L_rows = L->shape[ULAB_MAX_DIMS - 2];
    size_t L_cols = L->shape[ULAB_MAX_DIMS - 1];

    // Obtain transpose of the input matrix L in L_t
    size_t L_t_shape[ULAB_MAX_DIMS];
    size_t L_t_rows = L_t_shape[ULAB_MAX_DIMS - 2] = L_cols;
    size_t L_t_cols = L_t_shape[ULAB_MAX_DIMS - 1] = L_rows;
    ndarray_obj_t *L_t = ndarray_new_dense_ndarray(L->ndim, L_t_shape, L->dtype);

    uint8_t *L_arr = (uint8_t *)L->array;
    uint8_t *L_t_arr = (uint8_t *)L_t->array;
    uint8_t *b_arr = (uint8_t *)b->array;

    size_t i, j;

    uint8_t *L_ptr = L_arr;
    uint8_t *L_t_ptr = L_t_arr;
    for (i = 0; i < L_rows; i++) {
        for (j = 0; j < L_cols; j++) {
            set_L_ele(L_t_ptr, get_L_ele(L_ptr));
            L_t_ptr += L_t->strides[ULAB_MAX_DIMS - 2];
            L_ptr += L->strides[ULAB_MAX_DIMS - 1];
        }

        L_t_ptr -= j * L_t->strides[ULAB_MAX_DIMS - 2];
        L_t_ptr += L_t->strides[ULAB_MAX_DIMS - 1];
        L_ptr -= j * L->strides[ULAB_MAX_DIMS - 1];
        L_ptr += L->strides[ULAB_MAX_DIMS - 2];
    }

    ndarray_obj_t *x = ndarray_new_dense_ndarray(b->ndim, b->shape, NDARRAY_FLOAT);
    mp_float_t *x_arr = (mp_float_t *)x->array;

    ndarray_obj_t *y = ndarray_new_dense_ndarray(b->ndim, b->shape, NDARRAY_FLOAT);
    mp_float_t *y_arr = (mp_float_t *)y->array;

    // solve L y = b to obtain y, where L_t x = y
    for (i = 0; i < L_rows; i++) {
        mp_float_t sum = 0.0;
        for (j = 0; j < i; j++) {
            sum += (get_L_ele(L_arr) * (*y_arr++));
            L_arr += L->strides[ULAB_MAX_DIMS - 1];
        }

        sum = (get_b_ele(b_arr) - sum) / (get_L_ele(L_arr));
        *y_arr = sum;

        y_arr -= j;
        L_arr -= L->strides[ULAB_MAX_DIMS - 1] * j;
        L_arr += L->strides[ULAB_MAX_DIMS - 2];
        b_arr += b->strides[ULAB_MAX_DIMS - 1];
    }

    // using y, solve L_t x = y to obtain x
    L_t_arr += (L_t->strides[ULAB_MAX_DIMS - 2] * L_t_rows);
    y_arr += L_t_cols;
    x_arr += L_t_cols;

    for (i = L_t_rows - 1; i < L_t_rows; i--) {
        mp_float_t sum = 0.0;
        for (j = i + 1; j < L_t_cols; j++) {
            sum += (get_L_ele(L_t_arr) * (*x_arr++));
            L_t_arr += L_t->strides[ULAB_MAX_DIMS - 1];
        }

        x_arr -= (j - i);
        L_t_arr -= (L_t->strides[ULAB_MAX_DIMS - 1] * (j - i));
        y_arr--;

        sum = ((*y_arr) - sum) / get_L_ele(L_t_arr);
        *x_arr = sum;

        L_t_arr -= L_t->strides[ULAB_MAX_DIMS - 2];
    }

    return MP_OBJ_FROM_PTR(x);
}

MP_DEFINE_CONST_FUN_OBJ_2(linalg_cho_solve_obj, cho_solve);

#endif /* ULAB_SCIPY_LINALG_HAS_CHO_SOLVE */

#if ULAB_SCIPY_LINALG_HAS_SVD

//| def svd(a: ulab.numpy.ndarray) -> (ulab.numpy.ndarray, ulab.numpy.ndarray, ulab.numpy.ndarray):
//|    """
//|    :param ~ulab.numpy.ndarray a: matrix whose singular-value decomposition is requested
//|    :return: tuple of (U, s, Vh) matrices such that a = U s Vh^*. 
//|
//|    Calculate singular-value decomposition of a"""
//|    ...
//|

static void scipy_linalg_svd22(mp_float_t A00, mp_float_t A01, mp_float_t A10, mp_float_t A11, 
                        mp_float_t *U, mp_float_t *S, mp_float_t *V) {
    // adapted from https://github.com/openmv/apriltag/blob/master/common/svd22.c

    mp_float_t B0 = A00 + A11;
    mp_float_t B1 = A00 - A11;
    mp_float_t B2 = A01 + A10;
    mp_float_t B3 = A01 - A10;

    mp_float_t PminusT = atan2(B3, B0);
    mp_float_t PplusT = atan2(B2, B1);

    mp_float_t P = (PminusT + PplusT) / 2;
    mp_float_t T = (-PminusT + PplusT) / 2;

    mp_float_t CP = cos(P), SP = sin(P);
    mp_float_t CT = cos(T), ST = sin(T);

    U[0] = CT;
    U[1] = -ST;
    U[2] = ST;
    U[3] = CT;

    V[0] = CP;
    V[1] = -SP;
    V[2] = SP;
    V[3] = CP;

    // C0 = e + f. There are two ways to compute C0; we pick the one
    // that is better conditioned.
    mp_float_t CPmT = cos(P - T), SPmT = sin(P - T);
    mp_float_t C0 = 0;
    if(MICROPY_FLOAT_C_FUN(fabs)(CPmT) > MICROPY_FLOAT_C_FUN(fabs)(SPmT)) {
        C0 = B0 / CPmT;
    } else {
        C0 = B3 / SPmT;
    }

    // C1 = e - f. There are two ways to compute C1; we pick the one
    // that is better conditioned.
    mp_float_t CPpT = cos(P+T), SPpT = sin(P+T);
    mp_float_t C1 = 0;
    if(MICROPY_FLOAT_C_FUN(fabs)(CPpT) > MICROPY_FLOAT_C_FUN(fabs)(SPpT)) {
        C1 = B1 / CPpT;
    } else {
        C1 = B2 / SPpT;
    }

    // e and f are the singular values
    mp_float_t e = (C0 + C1) / 2;
    mp_float_t f = (C0 - C1) / 2;

    if(e < 0) {
        e = -e;
        U[0] = -U[0];
        U[2] = -U[2];
    }

    if(f < 0) {
        f = -f;
        U[1] = -U[1];
        U[3] = -U[3];
    }

    // sort singular values.
    if(e > f) {
        // already in big-to-small order
        S[0] = e;
        S[1] = f;
    } else {
        // P = [ 0 1 ; 1 0 ]
        // USV' = (UP)(PSP)(PV')
        //      = (UP)(PSP)(VP)'
        //      = (UP)(PSP)(P'V')'
        S[0] = f;
        S[1] = e;

        // exchange columns of U and V
        double tmp[2];
        tmp[0] = U[0];
        tmp[1] = U[2];
        U[0] = U[1];
        U[2] = U[3];
        U[1] = tmp[0];
        U[3] = tmp[1];

        tmp[0] = V[0];
        tmp[1] = V[2];
        V[0] = V[1];
        V[2] = V[3];
        V[1] = tmp[0];
        V[3] = tmp[1];
    }
}

// find the index of the off-diagonal element with the largest mag
static inline size_t linalg_max_index(mp_float_t *A, size_t row, size_t maxcol) {
    // in ulab, columns are the second index of matrices, 
    // so we can simply walk along the axis, as if it was 
    // a linear array; simply pass *A as A[row * columns]

    size_t maxi = 0;
    mp_float_t maxv = -1.0;
    mp_float_t v;

    for(size_t i = 0; i < maxcol; i++) {
        if(i == row) {
            continue;
        }
        v = MICROPY_FLOAT_C_FUN(fabs)(A[i]);
        if(v > maxv) {
            maxi = i;
            maxv = v;
        }
    }

    return maxi;
}

static mp_obj_t linalg_svd(mp_obj_t _a) {

    if(!mp_obj_is_type(_a, &ulab_ndarray_type)) {
        mp_raise_TypeError(MP_ERROR_TEXT("input matrix must be an ndarray"));
    }

    ndarray_obj_t *A = MP_OBJ_TO_PTR(_a);

    if(A->ndim != 2) {
        mp_raise_TypeError(MP_ERROR_TEXT("input must be a 2D array"));
    }

    #if ULAB_SUPPORTS_COMPLEX
    if(A->dtype == NDARRAY_COMPLEX) {
        mp_raise_TypeError(MP_ERROR_TEXT("input matrix must be real"));
    }
    #endif

    ndarray_obj_t *B = ndarray_new_dense_ndarray(2, A->shape, NDARRAY_FLOAT);
    mp_float_t *b = (mp_float_t *)B->array;
    
    mp_float_t (*get_A_element)(void *) = ndarray_get_float_function(A->dtype);
    uint8_t *a = (uint8_t *)A->array;
    
    size_t ncolumns = B->shape[ULAB_MAX_DIMS - 1];
    size_t nrows = B->shape[ULAB_MAX_DIMS - 2];

    // copy data from a to B
    for(size_t i = 0; i < ncolumns; i++) {
        for(size_t j = 0; j < nrows; j++) {
            *b++ = get_A_element(a);
            a += A->strides[ULAB_MAX_DIMS - 1];
        }
        a -= A->strides[ULAB_MAX_DIMS - 1] * ncolumns;
        a += A->strides[ULAB_MAX_DIMS - 2];
    }

    b -= nrows * ncolumns;

    // LS: cumulative left-handed transformations
    mp_float_t *LS = m_new0(mp_float_t, nrows * nrows);
    // start with the unit matrix
    for(size_t i = 0; i < nrows; i++) {
        LS[i * (nrows + 1)] = 1.0; /* LS[i, i] */
    }

    // RS: cumulative right-handed transformations
    mp_float_t *RS = m_new0(mp_float_t, ncolumns * ncolumns);
    // start with the unit matrix
    for(size_t i = 0; i < ncolumns; i++) {
        RS[i * (ncolumns + 1)] = 1.0; /* RS[i, i] */
    }

    for(size_t hhidx = 0; hhidx < nrows; hhidx++)  {
        if(hhidx < ncolumns) {
            size_t vlen = nrows - hhidx;

            mp_float_t *v = m_new0(mp_float_t, vlen);

            mp_float_t mag2 = 0.0;
            for(size_t i = 0; i < vlen; i++) {
                v[i] = b[(hhidx + i) * ncolumns + hhidx]; /* B[hhidx + i, hhidx] */
                mag2 += v[i] * v[i];
            }

            mp_float_t oldv0 = v[0];
            if(oldv0 < 0) {
                v[0] -= MICROPY_FLOAT_C_FUN(sqrt)(mag2);
            } else {
                v[0] += MICROPY_FLOAT_C_FUN(sqrt)(mag2);
            }

            mag2 += -oldv0 * oldv0 + v[0] * v[0];

            // normalize v
            double mag = MICROPY_FLOAT_C_FUN(sqrt)(mag2);

            if(mag == 0.0) {
                continue;
            }

            for(size_t i = 0; i < vlen; i++) {
                v[i] /= mag;
            }

            // Q = I - 2vv'
            mp_float_t *Q = m_new0(mp_float_t, nrows * nrows);
            for(size_t i = 0; i < nrows; i++) {
                Q[i * (ncolumns + 1)] = 1.0;
            }

            for(size_t i = 0; i < vlen; i++) {
                for(size_t j = 0; j < vlen; j++) {
                    Q[(i + hhidx) * nrows +  (j + hhidx)] -= 2*v[i]*v[j]; /* Q[i + hhidx, j + hhidx] */
                }
            }

            // LS = matd_op("F*M", LS, Q);
            // Implementation: take each row of LS, compute dot product with n,
            // subtract n (scaled by dot product) from it.
            for(size_t i = 0; i < nrows; i++) {
                mp_float_t dot = 0.0;
                for(size_t j = 0; j < vlen; j++) {
                    dot += LS[i * nrows + (hhidx + j)] * v[j]; /* U[i, hhidx + j] */
                }
                for(size_t j = 0; j < vlen; j++) {
                    LS[i * nrows + (hhidx + j)] -= 2 * dot * v[j]; /* U[i, hhidx + j] */
                }
            }

            //  B = matd_op("M*F", Q, B); 
            // should be Q', but Q is symmetric.
            for(size_t i = 0; i < ncolumns; i++) {
                mp_float_t dot = 0.0;
                for(size_t j = 0; j < vlen; j++) {
                    dot += b[(hhidx + j) * ncolumns + i] * v[j]; /* B[hhidx + j, i] */
                }
                for(size_t j = 0; j < vlen; j++) {
                    b[(hhidx + j) * ncolumns + i] -= 2*dot*v[j]; /* B[hhidx + j, i] */
                }
            }
            m_del(mp_float_t, v, vlen);
            m_del(mp_float_t, Q, nrows * nrows);
        }

        if(hhidx + 2 < ncolumns) {
            size_t vlen = ncolumns - hhidx - 1;
            mp_float_t *v = m_new0(mp_float_t, vlen);

            mp_float_t mag2 = 0.0;
            for(size_t i = 0; i < vlen; i++) {
                v[i] = b[hhidx * ncolumns + hhidx + i + 1];
                mag2 += v[i] * v[i];
            }

            mp_float_t oldv0 = v[0];
            if(oldv0 < 0) {
                v[0] -= MICROPY_FLOAT_C_FUN(sqrt)(mag2);
            } else {
                v[0] += MICROPY_FLOAT_C_FUN(sqrt)(mag2);
            }

            mag2 += -oldv0 * oldv0 + v[0] * v[0];

            // compute magnitude of ([1 0 0..]+v)
            mp_float_t mag = MICROPY_FLOAT_C_FUN(sqrt)(mag2);

            // this case can occur when the vectors are already perpendicular
            if(mag == 0.0) {
                continue;
            }

            for(size_t i = 0; i < vlen; i++) {
                v[i] /= mag;
            }

            for(size_t i = 0; i < ncolumns; i++) { // this is the number of rows, but RS is of size ncolumns * ncolumns
                mp_float_t dot = 0.0;
                for(size_t j = 0; j < vlen; j++) {
                    dot += RS[i * ncolumns +  hhidx + 1 + j] * v[j]; /* V[i, hhidx + 1 + j] */
                }
                for(size_t j = 0; j < vlen; j++) {
                    RS[i * ncolumns +  hhidx + 1 + j] -= 2 * dot * v[j]; /* V[i, hhidx + 1 + j] */
                }
            }

            //   B = matd_op("F*M", B, Q); // should be Q', but Q is symmetric.
            for(size_t i = 0; i < nrows; i++) {
                mp_float_t dot = 0.0;
                for(size_t j = 0; j < vlen; j++) {
                    dot += b[i * ncolumns + hhidx + 1 + j] * v[j]; /* B[i, hhidx + 1 + j] */
                }
                for(size_t j = 0; j < vlen; j++) {
                    b[i * ncolumns + hhidx + 1 + j] -= 2 * dot * v[j]; /* B[i, hhidx + 1 + j] */
                }
            }
            m_del(mp_float_t, v, vlen);
        }
    }

    mp_float_t maxv; // maximum non-zero value being reduced this iteration
    size_t *maxrowidx = m_new0(size_t, ncolumns);

    for(size_t i = 2; i < ncolumns; i++) {
        maxrowidx[i] = linalg_max_index(b + (i * ncolumns), i, ncolumns); /* B[i, ...] */
    }
    size_t lastmaxi = 0;
    size_t lastmaxj = 1;

    for(size_t iterations = 0; iterations < SCIPY_LINALG_SVD_MAXITERATIONS; iterations++) {
        if(ncolumns < 2) {
            break;
        }
        int32_t maxi = -1;
        int32_t maxj;
        maxv = -1;

        // now, EVERY row also had columns lastmaxi and lastmaxj modified.
        for(size_t rowi = 0; rowi < ncolumns; rowi++) {

            // the magnitude of the largest off-diagonal element
            // in this row.
            mp_float_t thismaxv;

            if((rowi == lastmaxi) || (rowi == lastmaxj)) {
                maxrowidx[rowi] = linalg_max_index(b + rowi * ncolumns, rowi, ncolumns); /* B[rowi, ...] */
                thismaxv = MICROPY_FLOAT_C_FUN(fabs)(b[rowi * ncolumns + maxrowidx[rowi]]); /* B[rowi, maxrowidx[rowi]] */
                goto endrowi;
            }

            if((maxrowidx[rowi] == lastmaxi) || (maxrowidx[rowi] == lastmaxj)) {
                maxrowidx[rowi] = linalg_max_index(b + rowi * ncolumns, rowi, ncolumns); /* B[rowi, ... ]*/
                thismaxv = MICROPY_FLOAT_C_FUN(fabs)(b[rowi * ncolumns + maxrowidx[rowi]]); /* B[rowi, maxrowidx[rowi]] */
                goto endrowi;
            }

            thismaxv = MICROPY_FLOAT_C_FUN(fabs)(b[rowi * ncolumns + maxrowidx[rowi]]); /* B[rowi, maxrowidx[rowi]] */

            // check column lastmaxi. Is it now the maximum?
            if(lastmaxi != rowi) {
                mp_float_t v = MICROPY_FLOAT_C_FUN(fabs)(b[rowi * ncolumns + maxrowidx[rowi]]); /* B[rowi, maxrowidx[rowi]] */
                if (v > thismaxv) {
                    thismaxv = v;
                    maxrowidx[rowi] = lastmaxi;
                }
            }

            // check column lastmaxj
            if(lastmaxj != rowi) {
                mp_float_t v = MICROPY_FLOAT_C_FUN(fabs)(b[rowi * ncolumns + lastmaxj]); /* B[rowi, lastmaxj] */
                if(v > thismaxv) {
                    thismaxv = v;
                    maxrowidx[rowi] = lastmaxj;
                }
            }

            // does this row have the largest value we've seen so far?
            endrowi:
                if(thismaxv > maxv) {
                    maxv = thismaxv;
                    maxi = rowi;
                }
        }

        assert(maxi >= 0);
        maxj = maxrowidx[maxi];

        // save these for the next iteration.
        lastmaxi = maxi;
        lastmaxj = maxj;

        if(maxv < SCIPY_LINALG_EPSILON) {
            break;
        }
        // Solve the 2x2 SVD problem for the matrix
        // [ A00 A01 ]
        // [ A10 A11 ]
        mp_float_t A00 = b[maxi * ncolumns + maxi]; /* B[maxi, maxi] */
        mp_float_t A01 = b[maxi * ncolumns + maxj]; /* B[maxi, maxj] */
        mp_float_t A10 = b[maxj * ncolumns + maxi]; /* B[maxj, maxi] */
        mp_float_t A11 = b[maxj * ncolumns + maxj]; /* B[maxj, maxj] */

        // we should probably move this out of the loop...
        mp_float_t *u = m_new(mp_float_t, 4);
        mp_float_t *s = m_new(mp_float_t, 2);
        mp_float_t *v = m_new(mp_float_t, 4);

        scipy_linalg_svd22(A00, A01, A10, A11, u, s, v);

        //  LS = matd_op("F*M", LS, QL);
        for(size_t i = 0; i < nrows; i++) {
            mp_float_t vi = LS[i * nrows + maxi]; /* U[i, maxi] */
            mp_float_t vj = LS[i * nrows + maxj]; /* U[i, maxj] */

            LS[i * nrows + maxi] = u[0] * vi + u[2] * vj;  /* U[i, maxi] */
            LS[i * nrows + maxj] = u[1] * vi + u[3] * vj;  /* U[i, maxj] */
        }

        //  RS = matd_op("F*M", RS, QR); // remember we'll transpose RS.
        for(size_t i = 0; i < ncolumns; i++) { // this is the nunber of rows in RS, for it is of size columns * columns
            mp_float_t vi = RS[i * ncolumns + maxi]; /* V[i, maxi] */
            mp_float_t vj = RS[i * ncolumns + maxj]; /* V[i, maxj] */

            RS[i * ncolumns + maxi] = v[0] * vi + v[2] * vj; /* V[i, maxi] */
            RS[i * ncolumns + maxj] = v[1] * vi + v[3] * vj; /* V[i, maxj] */
        }

        // B = matd_op("M'*F*M", QL, B, QR);
        // The QL matrix mixes rows of B.
        for(size_t i = 0; i < ncolumns; i++) {
            mp_float_t vi = b[maxi * ncolumns + i]; /* B[maxi, i] */
            mp_float_t vj = b[maxj * ncolumns + i]; /* B[maxj, i] */

            b[maxi * ncolumns + i] = u[0] * vi + u[2] * vj; /* B[maxi, i] */
            b[maxj * ncolumns + i] = u[1] * vi + u[3] * vj; /* B[maxj, i] */
        }

        // The QR matrix mixes columns of B.
        for(size_t i = 0; i < nrows; i++) {
            mp_float_t vi = b[i * ncolumns + maxi]; /* B[i, maxi] */
            mp_float_t vj = b[i * ncolumns + maxj]; /* B[i, maxj] */

            b[i * ncolumns + maxi] = v[0] * vi + v[2] * vj; /* B[i, maxi] */
            b[i * ncolumns + maxj] = v[1] * vi + v[3] * vj; /* B[i, maxj] */
        }

        m_del(mp_float_t, u, 4);
        m_del(mp_float_t, s, 2);
        m_del(mp_float_t, v, 4);
    }

    m_del(size_t, maxrowidx, ncolumns);

    size_t *idxs = m_new(size_t, ncolumns);
    mp_float_t *vals = m_new(mp_float_t, ncolumns);

    for(size_t i = 0; i < ncolumns; i++) {
        idxs[i] = i;
        vals[i] = b[i * ncolumns + i]; /* B[i, i] */
    }

    // Bubble sort
    uint8_t changed;
    do {
        changed = 0;

        for(size_t i = 0; i + 1 < ncolumns; i++) {
            if(MICROPY_FLOAT_C_FUN(fabs)(vals[i+1]) > MICROPY_FLOAT_C_FUN(fabs)(vals[i])) {
                size_t tmpi = idxs[i];
                idxs[i] = idxs[i + 1];
                idxs[i + 1] = tmpi;

                mp_float_t tmpv = vals[i];
                vals[i] = vals[i + 1];
                vals[i + 1] = tmpv;

                changed = 1;
            }
        }
    } while (changed);

    mp_float_t *LP = m_new(mp_float_t, nrows * nrows);
    // start with the unit matrix
    for(size_t i = 0; i < nrows; i++) {
        LP[i * (nrows + 1)] = 1.0; /* LP[i, i] */
    }

    mp_float_t *RP = m_new(mp_float_t, ncolumns * ncolumns);
    // start with the unit matrix
    for(size_t i = 0; i < ncolumns; i++) {
        RP[i * (ncolumns + 1)] = 1.0; /* RP[i, i] */
    }

    for(size_t i = 0; i < ncolumns; i++) {
        LP[idxs[i] * ncolumns + idxs[i]] = 0.0; /* LP[idxs[i], idxs[i]] */
        RP[idxs[i] * nrows + idxs[i]] = 0.0; /* RP[idxs[i], idxs[i]] */

        LP[idxs[i] * ncolumns + i] = vals[i] < 0 ? -1.0 : 1.0;
        RP[idxs[i] * nrows + i] = 1.0;
    }

    m_del(size_t, idxs, ncolumns);
    m_del(mp_float_t, vals, ncolumns);

    // we've factored:
    // LP*(something)*RP'

    // // solve for (something)
    // B = matd_op("M'*F*M", LP, B, RP);

    // // update LS and RS, remembering that RS will be transposed.
    
    // LS = matd_op("F*M", LS, LP);
    ndarray_obj_t *U = ndarray_new_dense_ndarray(2, ndarray_shape_vector(0, 0, nrows, nrows), NDARRAY_FLOAT);
    mp_float_t *u = (mp_float_t *)U->array;

    for(size_t i = 0; i < nrows; i++) {
        for(size_t j = 0; j < nrows; j++) {
            mp_float_t tmp = 0.0;
            for(size_t k = 0; k < nrows; k++) {
                tmp += LS[i * nrows + k] * LP[k * nrows + j]; /* LS[i, k] * LP[k, j] */
            }
            u[i * nrows + j] = tmp; /* U[i, j] */ 
        }
    }

    m_del(mp_float_t, LS, nrows * nrows);
    m_del(mp_float_t, LP, nrows * nrows);

    // RS = matd_op("F*M", RS, RP);
    ndarray_obj_t *V = ndarray_new_dense_ndarray(2, ndarray_shape_vector(0, 0, ncolumns, ncolumns), NDARRAY_FLOAT);
    mp_float_t *v = (mp_float_t *)V->array;
    for(size_t i = 0; i < ncolumns; i++) {
        for(size_t j = 0; j < ncolumns; j++) {
            mp_float_t tmp = 0.0;
            for(size_t k = 0; k < ncolumns; k++) {
                tmp += RS[i * ncolumns + k] * RP[k * ncolumns + j]; /* RS[i, k] * RP[k, j] */
            }
            v[i * ncolumns + j] = tmp; /* V[i, j] */ 
        }
    }

    m_del(mp_float_t, RS, ncolumns * ncolumns);
    m_del(mp_float_t, RP, ncolumns * ncolumns);

    // make B exactly diagonal
    for(size_t i = 0; i < ncolumns; i++) {
        for(size_t j = 0; j < nrows; j++) {
            if(i != j) {
                b[i * ncolumns + j] = 0.0; /* B[i, j] */
            }
        }
    }

    mp_obj_t *items = m_new(mp_obj_t, 3);
    items[0] = U;
    items[1] = B;
    items[2] = V;
    
    mp_obj_t tuple = mp_obj_new_tuple(3, items);
    m_del(mp_obj_t, items, 3);
    return tuple;
}

MP_DEFINE_CONST_FUN_OBJ_1(linalg_svd_obj, linalg_svd);

#endif /* ULAB_SCIPY_LINALG_HAS_SVD */

static const mp_rom_map_elem_t ulab_scipy_linalg_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_linalg) },
    #if ULAB_MAX_DIMS > 1
        #if ULAB_SCIPY_LINALG_HAS_SOLVE_TRIANGULAR
        { MP_ROM_QSTR(MP_QSTR_solve_triangular), MP_ROM_PTR(&linalg_solve_triangular_obj) },
        #endif
        #if ULAB_SCIPY_LINALG_HAS_CHO_SOLVE
        { MP_ROM_QSTR(MP_QSTR_cho_solve), MP_ROM_PTR(&linalg_cho_solve_obj) },
        #endif
        #if ULAB_SCIPY_LINALG_HAS_SVD
        { MP_ROM_QSTR(MP_QSTR_svd), MP_ROM_PTR(&linalg_svd_obj) },
        #endif
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_scipy_linalg_globals, ulab_scipy_linalg_globals_table);

const mp_obj_module_t ulab_scipy_linalg_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_scipy_linalg_globals,
};
#if CIRCUITPY_ULAB
MP_REGISTER_MODULE(MP_QSTR_ulab_dot_scipy_dot_linalg, ulab_scipy_linalg_module);
#endif

#endif /* ULAB_MAX_DIMS > 1 */
#endif /* ULAB_SCIPY_HAS_LINALG_MODULE */

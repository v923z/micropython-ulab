
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020 Roberto Colistete Jr.
 *
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "linalg.h"

#if ULAB_LINALG_MODULE

//| """Linear algebra functions"""
//|

#if ULAB_MAX_DIMS > 1
static ndarray_obj_t *linalg_object_is_square(mp_obj_t obj) {
    // Returns an ndarray, if the object is a square ndarray,
    // raises the appropriate exception otherwise
    if(!MP_OBJ_IS_TYPE(obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("size is defined for ndarrays only"));
    }
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(obj);
    if((ndarray->shape[ULAB_MAX_DIMS - 1] != ndarray->shape[ULAB_MAX_DIMS - 2]) || (ndarray->ndim != 2)) {
        mp_raise_ValueError(translate("input must be square matrix"));
    }
    return ndarray;
}
#endif

bool linalg_invert_matrix(mp_float_t *data, size_t N) {
    // returns true, of the inversion was successful,
    // false, if the matrix is singular

    // initially, this is the unit matrix: the contents of this matrix is what
    // will be returned after all the transformations
    mp_float_t *unit = m_new(mp_float_t, N*N);

    mp_float_t elem = 1.0;
    // initialise the unit matrix
    memset(unit, 0, sizeof(mp_float_t)*N*N);
    for(size_t m=0; m < N; m++) {
        memcpy(&unit[m * (N+1)], &elem, sizeof(mp_float_t));
    }
    for(size_t m=0; m < N; m++){
        // this could be faster with ((c < epsilon) && (c > -epsilon))
        if(MICROPY_FLOAT_C_FUN(fabs)(data[m * (N+1)]) < epsilon) {
            m_del(mp_float_t, unit, N * N);
            return false;
        }
        for(size_t n=0; n < N; n++){
            if(m != n){
                elem = data[N * n + m] / data[m * (N+1)];
                for(size_t k=0; k < N; k++){
                    data[N * n + k] -= elem * data[N * m + k];
                    unit[N * n + k] -= elem * unit[N * m + k];
                }
            }
        }
    }
    for(size_t m=0; m < N; m++){
        elem = data[m * (N+1)];
        for(size_t n=0; n < N; n++){
            data[N * m + n] /= elem;
            unit[N * m + n] /= elem;
        }
    }
    memcpy(data, unit, sizeof(mp_float_t)*N*N);
    m_del(mp_float_t, unit, N * N);
    return true;
}

#if ULAB_MAX_DIMS > 1
//| def cholesky(A: ulab.array) -> ulab.array:
//|     """
//|     :param ~ulab.array A: a positive definite, symmetric square matrix
//|     :return ~ulab.array L: a square root matrix in the lower triangular form
//|     :raises ValueError: If the input does not fulfill the necessary conditions
//|
//|     The returned matrix satisfies the equation m=LL*"""
//|     ...
//|

static mp_obj_t linalg_cholesky(mp_obj_t oin) {
    ndarray_obj_t *ndarray = linalg_object_is_square(oin);
    ndarray_obj_t *L = ndarray_new_dense_ndarray(2, ndarray_shape_vector(0, 0, ndarray->shape[ULAB_MAX_DIMS - 1], ndarray->shape[ULAB_MAX_DIMS - 1]), NDARRAY_FLOAT);
    mp_float_t *Larray = (mp_float_t *)L->array;

    size_t N = ndarray->shape[ULAB_MAX_DIMS - 1];
    uint8_t *array = (uint8_t *)ndarray->array;
    for(size_t m=0; m < N; m++) { // rows
        for(size_t n=0; n < N; n++) { // columns
            *Larray++ = ndarray_get_float_value(array, ndarray->dtype);
            array += ndarray->strides[ULAB_MAX_DIMS - 1];
        }
        array -= ndarray->strides[ULAB_MAX_DIMS - 1] * N;\
        array += ndarray->strides[ULAB_MAX_DIMS - 2];\
    }
    Larray -= N*N;
    // make sure the matrix is symmetric
    for(size_t m=0; m < N; m++) { // rows
        for(size_t n=m+1; n < N; n++) { // columns
            // compare entry (m, n) to (n, m)
            if(epsilon < MICROPY_FLOAT_C_FUN(fabs)(Larray[m * N + n] - Larray[n * N + m])) {
                mp_raise_ValueError(translate("input matrix is asymmetric"));
            }
        }
    }

    // this is actually not needed, but Cholesky in numpy returns the lower triangular matrix
    for(size_t i=0; i < N; i++) { // rows
        for(size_t j=i+1; j < N; j++) { // columns
            Larray[i*N + j] = MICROPY_FLOAT_CONST(0.0);
        }
    }
    mp_float_t sum = 0.0;
    for(size_t i=0; i < N; i++) { // rows
        for(size_t j=0; j <= i; j++) { // columns
            sum = Larray[i * N + j];
            for(size_t k=0; k < j; k++) {
                sum -= Larray[i * N + k] * Larray[j * N + k];
            }
            if(i == j) {
                if(sum <= MICROPY_FLOAT_CONST(0.0)) {
                    mp_raise_ValueError(translate("matrix is not positive definite"));
                } else {
                    Larray[i * N + i] = MICROPY_FLOAT_C_FUN(sqrt)(sum);
                }
            } else {
                Larray[i * N + j] = sum / Larray[j * N + j];
            }
        }
    }
    return MP_OBJ_FROM_PTR(L);
}

MP_DEFINE_CONST_FUN_OBJ_1(linalg_cholesky_obj, linalg_cholesky);

//| def det(m: ulab.array) -> float:
//|     """
//|     :param: m, a square matrix
//|     :return float: The determinant of the matrix
//|
//|     Computes the eigenvalues and eigenvectors of a square matrix"""
//|     ...
//|

static mp_obj_t linalg_det(mp_obj_t oin) {
    ndarray_obj_t *ndarray = linalg_object_is_square(oin);
    uint8_t *array = (uint8_t *)ndarray->array;
    size_t N = ndarray->shape[ULAB_MAX_DIMS - 1];
    mp_float_t *tmp = m_new(mp_float_t, N * N);
    for(size_t m=0; m < N; m++) { // rows
        for(size_t n=0; n < N; n++) { // columns
            *tmp++ = ndarray_get_float_value(array, ndarray->dtype);
            array += ndarray->strides[ULAB_MAX_DIMS - 1];
        }
        array -= ndarray->strides[ULAB_MAX_DIMS - 1] * N;\
        array += ndarray->strides[ULAB_MAX_DIMS - 2];\
    }

    mp_float_t c;
    for(size_t m=0; m < N-1; m++){
        if(MICROPY_FLOAT_C_FUN(fabs)(tmp[m * (N+1)]) < epsilon) {
            m_del(mp_float_t, tmp, N * N);
            return mp_obj_new_float(0.0);
        }
        for(size_t n=0; n < N; n++){
            if(m != n) {
                c = tmp[N * n + m] / tmp[m * (N+1)];
                for(size_t k=0; k < N; k++){
                    tmp[N * n + k] -= c * tmp[N * m + k];
                }
            }
        }
    }
    mp_float_t det = 1.0;

    for(size_t m=0; m < N; m++){
        det *= tmp[m * (N+1)];
    }
    m_del(mp_float_t, tmp, N * N);
    return mp_obj_new_float(det);
}

MP_DEFINE_CONST_FUN_OBJ_1(linalg_det_obj, linalg_det);

#endif

//| def dot(m1: ulab.array, m2: ulab.array) -> Union[ulab.array, float]:
//|    """
//|    :param ~ulab.array m1: a matrix, or a vector
//|    :param ~ulab.array m2: a matrix, or a vector
//|
//|    Computes the product of two matrices, or two vectors. In the letter case, the inner product is returned."""
//|    ...
//|

static mp_obj_t linalg_dot(mp_obj_t _m1, mp_obj_t _m2) {
    // TODO: should the results be upcast?
    // This implements 2D operations only!
    if(!MP_OBJ_IS_TYPE(_m1, &ulab_ndarray_type) || !MP_OBJ_IS_TYPE(_m2, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("arguments must be ndarrays"));
    }
    ndarray_obj_t *m1 = MP_OBJ_TO_PTR(_m1);
    ndarray_obj_t *m2 = MP_OBJ_TO_PTR(_m2);

    #if ULAB_MAX_DIMS > 1
    if ((m1->ndim == 1) && (m2->ndim == 1)) {
    #endif
        // 2 vectors
        if (m1->len != m2->len) {
            mp_raise_ValueError(translate("vectors must have same lengths"));
        }
        mp_float_t dot = 0.0;
        uint8_t *array1 = (uint8_t *)m1->array;
        uint8_t *array2 = (uint8_t *)m2->array;
        for (size_t i=0; i < m1->len; i++) {
            dot += ndarray_get_float_value(array1, m1->dtype)*ndarray_get_float_value(array2, m2->dtype);
            array1 += m1->strides[ULAB_MAX_DIMS - 1];
            array2 += m2->strides[ULAB_MAX_DIMS - 1];
        }
        return mp_obj_new_float(dot);
    #if ULAB_MAX_DIMS > 1
    } else {
        // 2 matrices
        if(m1->shape[ULAB_MAX_DIMS - 1] != m2->shape[ULAB_MAX_DIMS - 2]) {
            mp_raise_ValueError(translate("matrix dimensions do not match"));
        }
        size_t *shape = ndarray_shape_vector(0, 0, m1->shape[ULAB_MAX_DIMS - 2], m2->shape[ULAB_MAX_DIMS - 1]);
        ndarray_obj_t *out = ndarray_new_dense_ndarray(2, shape, NDARRAY_FLOAT);
        mp_float_t *outdata = (mp_float_t *)out->array;
        for(size_t i=0; i < m1->shape[ULAB_MAX_DIMS - 2]; i++) { // rows of m1
            for(size_t j=0; j < m2->shape[ULAB_MAX_DIMS - 1]; j++) { // columns of m2
                mp_float_t sum = 0.0, v1, v2;
                for(size_t k=0; k < m2->shape[ULAB_MAX_DIMS - 2]; k++) {
                    // (i, k) * (k, j)
                    size_t pos1 = i*m1->shape[ULAB_MAX_DIMS - 1]+k;
                    size_t pos2 = k*m2->shape[ULAB_MAX_DIMS - 1]+j;
                    v1 = ndarray_get_float_index(m1->array, m1->dtype, pos1);
                    v2 = ndarray_get_float_index(m2->array, m2->dtype, pos2);
                    sum += v1 * v2;
                }
                *outdata++ = sum;
            }
        }
        return MP_OBJ_FROM_PTR(out);
    }
    #endif
}

MP_DEFINE_CONST_FUN_OBJ_2(linalg_dot_obj, linalg_dot);

#if ULAB_MAX_DIMS > 1
//| def eig(m: ulab.array) -> Tuple[ulab.array, ulab.array]:
//|     """
//|     :param m: a square matrix
//|     :return tuple (eigenvectors, eigenvalues):
//|
//|     Computes the eigenvalues and eigenvectors of a square matrix"""
//|     ...
//|

static mp_obj_t linalg_eig(mp_obj_t oin) {
    ndarray_obj_t *in = linalg_object_is_square(oin);
    uint8_t *iarray = (uint8_t *)in->array;
    mp_float_t *array = m_new(mp_float_t, in->len);
    size_t S = in->shape[ULAB_MAX_DIMS - 2];
    for(size_t i=0; i < S; i++) { // rows
        for(size_t j=0; j < S; j++) { // columns
            *array++ = ndarray_get_float_value(iarray, in->dtype);
            iarray += in->strides[ULAB_MAX_DIMS - 1];
        }
        iarray -= in->strides[ULAB_MAX_DIMS - 1] * S;
        iarray += in->strides[ULAB_MAX_DIMS - 2];
    }
    // make sure the matrix is symmetric
    for(size_t m=0; m < S; m++) {
        for(size_t n=m+1; n < S; n++) {
            // compare entry (m, n) to (n, m)
            // TODO: this must probably be scaled!
            if(epsilon < MICROPY_FLOAT_C_FUN(fabs)(array[m * S + n] - array[n * S + m])) {
                mp_raise_ValueError(translate("input matrix is asymmetric"));
            }
        }
    }

    // if we got this far, then the matrix will be symmetric

    ndarray_obj_t *eigenvectors = ndarray_new_dense_ndarray(2, ndarray_shape_vector(0, 0, S, S), NDARRAY_FLOAT);
    mp_float_t *eigvectors = (mp_float_t *)eigenvectors->array;
    // start out with the unit matrix
    for(size_t m=0; m < S; m++) {
        eigvectors[m * (S+1)] = 1.0;
    }
    mp_float_t largest, w, t, c, s, tau, aMk, aNk, vm, vn;
    size_t M, N;
    size_t iterations = JACOBI_MAX * S * S;
    do {
        iterations--;
        // find the pivot here
        M = 0;
        N = 0;
        largest = 0.0;
        for(size_t m=0; m < S-1; m++) { // -1: no need to inspect last row
            for(size_t n=m+1; n < S; n++) {
                w = MICROPY_FLOAT_C_FUN(fabs)(array[m * S + n]);
                if((largest < w) && (epsilon < w)) {
                    M = m;
                    N = n;
                    largest = w;
                }
            }
        }
        if(M + N == 0) { // all entries are smaller than epsilon, there is not much we can do...
            break;
        }
        // at this point, we have the pivot, and it is the entry (M, N)
        // now we have to find the rotation angle
        w = (array[N * S + N] - array[M * S + M]) / (MICROPY_FLOAT_CONST(2.0)*array[M * S + N]);
        // The following if/else chooses the smaller absolute value for the tangent
        // of the rotation angle. Going with the smaller should be numerically stabler.
        if(w > 0) {
            t = MICROPY_FLOAT_C_FUN(sqrt)(w*w + MICROPY_FLOAT_CONST(1.0)) - w;
        } else {
            t = MICROPY_FLOAT_CONST(-1.0)*(MICROPY_FLOAT_C_FUN(sqrt)(w*w + MICROPY_FLOAT_CONST(1.0)) + w);
        }
        s = t / MICROPY_FLOAT_C_FUN(sqrt)(t*t + MICROPY_FLOAT_CONST(1.0)); // the sine of the rotation angle
        c = MICROPY_FLOAT_CONST(1.0) / MICROPY_FLOAT_C_FUN(sqrt)(t*t + MICROPY_FLOAT_CONST(1.0)); // the cosine of the rotation angle
        tau = (MICROPY_FLOAT_CONST(1.0)-c)/s; // this is equal to the tangent of the half of the rotation angle

        // at this point, we have the rotation angles, so we can transform the matrix
        // first the two diagonal elements
        // a(M, M) = a(M, M) - t*a(M, N)
        array[M * S + M] = array[M * S + M] - t * array[M * S + N];
        // a(N, N) = a(N, N) + t*a(M, N)
        array[N * S + N] = array[N * S + N] + t * array[M * S + N];
        // after the rotation, the a(M, N), and a(N, M) entries should become zero
        array[M * S + N] = array[N * S + M] = MICROPY_FLOAT_CONST(0.0);
        // then all other elements in the column
        for(size_t k=0; k < S; k++) {
            if((k == M) || (k == N)) {
                continue;
            }
            aMk = array[M * S + k];
            aNk = array[N * S + k];
            // a(M, k) = a(M, k) - s*(a(N, k) + tau*a(M, k))
            array[M * S + k] -= s * (aNk + tau * aMk);
            // a(N, k) = a(N, k) + s*(a(M, k) - tau*a(N, k))
            array[N * S + k] += s * (aMk - tau * aNk);
            // a(k, M) = a(M, k)
            array[k * S + M] = array[M * S + k];
            // a(k, N) = a(N, k)
            array[k * S + N] = array[N * S + k];
        }
        // now we have to update the eigenvectors
        // the rotation matrix, R, multiplies from the right
        // R is the unit matrix, except for the
        // R(M,M) = R(N, N) = c
        // R(N, M) = s
        // (M, N) = -s
        // entries. This means that only the Mth, and Nth columns will change
        for(size_t m=0; m < S; m++) {
            vm = eigvectors[m * S + M];
            vn = eigvectors[m * S + N];
            // the new value of eigvectors(m, M)
            eigvectors[m * S + M] = c * vm - s * vn;
            // the new value of eigvectors(m, N)
            eigvectors[m * S + N] = s * vm + c * vn;
        }
    } while(iterations > 0);

    if(iterations == 0) {
        // the computation did not converge; numpy raises LinAlgError
        m_del(mp_float_t, array, in->len);
        mp_raise_ValueError(translate("iterations did not converge"));
    }
    ndarray_obj_t *eigenvalues = ndarray_new_linear_array(S, NDARRAY_FLOAT);
    mp_float_t *eigvalues = (mp_float_t *)eigenvalues->array;
    for(size_t i=0; i < S; i++) {
        eigvalues[i] = array[i * (S + 1)];
    }
    m_del(mp_float_t, array, in->len);

    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
    tuple->items[0] = MP_OBJ_FROM_PTR(eigenvalues);
    tuple->items[1] = MP_OBJ_FROM_PTR(eigenvectors);
    return tuple;
}

MP_DEFINE_CONST_FUN_OBJ_1(linalg_eig_obj, linalg_eig);

//| def inv(m: ulab.array) -> ulab.array:
//|     """
//|     :param ~ulab.array m: a square matrix
//|     :return: The inverse of the matrix, if it exists
//|     :raises ValueError: if the matrix is not invertible
//|
//|     Computes the inverse of a square matrix"""
//|     ...
//|
static mp_obj_t linalg_inv(mp_obj_t o_in) {
    ndarray_obj_t *ndarray = linalg_object_is_square(o_in);
    uint8_t *array = (uint8_t *)ndarray->array;
    size_t N = ndarray->shape[ULAB_MAX_DIMS - 1];

    ndarray_obj_t *inverted = ndarray_new_dense_ndarray(2, ndarray_shape_vector(0, 0, N, N), NDARRAY_FLOAT);
    mp_float_t *iarray = (mp_float_t *)inverted->array;

    for(size_t i=0; i < N; i++) { // rows
        for(size_t j=0; j < N; j++) { // columns
            *iarray++ = ndarray_get_float_value(array, ndarray->dtype);
            array += ndarray->strides[ULAB_MAX_DIMS - 1];
        }
        array -= ndarray->strides[ULAB_MAX_DIMS - 1] * N;
        array += ndarray->strides[ULAB_MAX_DIMS - 2];
    }
    if(!linalg_invert_matrix(iarray, N)) {
        // TODO: I am not sure this is needed here. Otherwise,
        // how should we free up the unused RAM of inverted?
        mp_raise_ValueError(translate("input matrix is singular"));
    }
    return MP_OBJ_FROM_PTR(inverted);
}

MP_DEFINE_CONST_FUN_OBJ_1(linalg_inv_obj, linalg_inv);
#endif

//| def norm(x: ulab.array) -> float:
//|    """
//|    :param ~ulab.array x: a vector or a matrix
//|
//|    Computes the 2-norm of a vector or a matrix, i.e., ``sqrt(sum(x*x))``, however, without the RAM overhead."""
//|    ...
//|

static mp_obj_t linalg_norm(mp_obj_t _x) {
    if (!MP_OBJ_IS_TYPE(_x, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("argument must be ndarray"));
    }
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(_x);
    if((ndarray->ndim != 1) || (ndarray->ndim != 2)) {
        mp_raise_ValueError(translate("norm is defined for 1D and 2D arrays"));
    }
    mp_float_t dot = 0.0, v;
    uint8_t *array = (uint8_t *)ndarray->array;
    size_t k = 0;
    do {
        size_t l = 0;
        do {
            v = ndarray_get_float_value(array, ndarray->dtype);
            array += ndarray->strides[ULAB_MAX_DIMS - 1];
            dot += v*v;
        } while(l < ndarray->shape[ULAB_MAX_DIMS - 1]);
        array -= ndarray->strides[ULAB_MAX_DIMS - 2] + ndarray->shape[ULAB_MAX_DIMS - 2];
        array += ndarray->strides[ULAB_MAX_DIMS - 1];
    } while(k < ndarray->shape[ULAB_MAX_DIMS - 1]);
    return mp_obj_new_float(MICROPY_FLOAT_C_FUN(sqrt)(dot));
}

MP_DEFINE_CONST_FUN_OBJ_1(linalg_norm_obj, linalg_norm);

//| def size(array: ulab.array) -> int:
//|     """Return the total number of elements in the array, as an integer."""
//|     ...
//|

static mp_obj_t linalg_size(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!MP_OBJ_IS_TYPE(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("size is defined for ndarrays only"));
    } else {
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0].u_obj);
        if(args[1].u_obj == mp_const_none) {
            return mp_obj_new_int(ndarray->len);
        } else if(MP_OBJ_IS_INT(args[1].u_obj)) {
            int8_t ax = mp_obj_get_int(args[1].u_obj);
            if(ax == 0) {
                return mp_obj_new_int(ndarray->len);
            } else {
                if(ax < 0) {
                    ax += ndarray->ndim;
                }
                if(ax > ndarray->ndim) {
                    mp_raise_ValueError(translate("tuple index out of range"));
                } else {
                    mp_obj_new_int(ndarray->shape[ULAB_MAX_DIMS - 1 - ndarray->ndim + ax]);
                }
            }
        }
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(linalg_size_obj, 1, linalg_size);

#if ULAB_MAX_DIMS > 1
//| def trace(m: ulab.array) -> float:
//|     """
//|     :param m: a square matrix
//|
//|     Compute the trace of the matrix, the sum of its diagonal elements."""
//|     ...
//|

static mp_obj_t linalg_trace(mp_obj_t oin) {
    ndarray_obj_t *ndarray = linalg_object_is_square(oin);
    mp_float_t trace = 0.0;
    for(size_t i=0; i < ndarray->len; i++) {
        size_t pos = i * (ndarray->strides[ULAB_MAX_DIMS - 1] + ndarray->strides[ULAB_MAX_DIMS - 2]);
        trace += ndarray_get_float_index(ndarray->array, ndarray->dtype, pos);
    }
    if(ndarray->dtype == NDARRAY_FLOAT) {
        return mp_obj_new_float(trace);
    }
    return mp_obj_new_int_from_float(trace);
}

MP_DEFINE_CONST_FUN_OBJ_1(linalg_trace_obj, linalg_trace);
#endif

STATIC const mp_rom_map_elem_t ulab_linalg_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_linalg) },
    #if ULAB_MAX_DIMS > 1
    { MP_ROM_QSTR(MP_QSTR_cholesky), (mp_obj_t)&linalg_cholesky_obj },
    { MP_ROM_QSTR(MP_QSTR_det), (mp_obj_t)&linalg_det_obj },
    #endif
    { MP_ROM_QSTR(MP_QSTR_dot), (mp_obj_t)&linalg_dot_obj },
    #if ULAB_MAX_DIMS > 1
    { MP_ROM_QSTR(MP_QSTR_eig), (mp_obj_t)&linalg_eig_obj },
    { MP_ROM_QSTR(MP_QSTR_inv), (mp_obj_t)&linalg_inv_obj },
    #endif
    { MP_ROM_QSTR(MP_QSTR_norm), (mp_obj_t)&linalg_norm_obj },
    { MP_ROM_QSTR(MP_QSTR_size), (mp_obj_t)&linalg_size_obj },
    #if ULAB_MAX_DIMS > 1
    { MP_ROM_QSTR(MP_QSTR_trace), (mp_obj_t)&linalg_trace_obj },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_linalg_globals, ulab_linalg_globals_table);

mp_obj_module_t ulab_linalg_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_linalg_globals,
};

#endif

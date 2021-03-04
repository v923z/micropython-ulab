
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020 Roberto Colistete Jr.
 *               2020 Taku Fukada
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
#include "linalg.h"

#if ULAB_NUMPY_HAS_LINALG_MODULE
//| """Linear algebra functions"""
//|

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
    ndarray_obj_t *ndarray = tools_object_is_square(oin);
    ndarray_obj_t *L = ndarray_new_dense_ndarray(2, ndarray_shape_vector(0, 0, ndarray->shape[ULAB_MAX_DIMS - 1], ndarray->shape[ULAB_MAX_DIMS - 1]), NDARRAY_FLOAT);
    mp_float_t *Larray = (mp_float_t *)L->array;

    size_t N = ndarray->shape[ULAB_MAX_DIMS - 1];
    uint8_t *array = (uint8_t *)ndarray->array;
    mp_float_t (*func)(void *) = ndarray_get_float_function(ndarray->dtype.type);

    for(size_t m=0; m < N; m++) { // rows
        for(size_t n=0; n < N; n++) { // columns
            *Larray++ = func(array);
            array += ndarray->strides[ULAB_MAX_DIMS - 1];
        }
        array -= ndarray->strides[ULAB_MAX_DIMS - 1] * N;
        array += ndarray->strides[ULAB_MAX_DIMS - 2];
    }
    Larray -= N*N;
    // make sure the matrix is symmetric
    for(size_t m=0; m < N; m++) { // rows
        for(size_t n=m+1; n < N; n++) { // columns
            // compare entry (m, n) to (n, m)
            if(LINALG_EPSILON < MICROPY_FLOAT_C_FUN(fabs)(Larray[m * N + n] - Larray[n * N + m])) {
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
    ndarray_obj_t *ndarray = tools_object_is_square(oin);
    uint8_t *array = (uint8_t *)ndarray->array;
    size_t N = ndarray->shape[ULAB_MAX_DIMS - 1];
    mp_float_t *tmp = m_new(mp_float_t, N * N);
    for(size_t m=0; m < N; m++) { // rows
        for(size_t n=0; n < N; n++) { // columns
            *tmp++ = ndarray_get_float_value(array, ndarray->dtype.type);
            array += ndarray->strides[ULAB_MAX_DIMS - 1];
        }
        array -= ndarray->strides[ULAB_MAX_DIMS - 1] * N;
        array += ndarray->strides[ULAB_MAX_DIMS - 2];
    }

    // re-wind the pointer
    tmp -= N*N;

    mp_float_t c;
    mp_float_t det_sign = 1.0;

    for(size_t m=0; m < N-1; m++){
        if(MICROPY_FLOAT_C_FUN(fabs)(tmp[m * (N+1)]) < LINALG_EPSILON) {
            size_t m1 = m + 1;
            for(; m1 < N; m1++) {
                if(!(MICROPY_FLOAT_C_FUN(fabs)(tmp[m1*N+m]) < LINALG_EPSILON)) {
                     //look for a line to swap
                    for(size_t m2=0; m2 < N; m2++) {
                        mp_float_t swapVal = tmp[m*N+m2];
                        tmp[m*N+m2] = tmp[m1*N+m2];
                        tmp[m1*N+m2] = swapVal;
                    }
                    det_sign = -det_sign;
                    break;
                }
            }
            if (m1 >= N) {
                m_del(mp_float_t, tmp, N * N);
                return mp_obj_new_float(0.0);
            }
        }
        for(size_t n=0; n < N; n++) {
            if(m != n) {
                c = tmp[N * n + m] / tmp[m * (N+1)];
                for(size_t k=0; k < N; k++){
                    tmp[N * n + k] -= c * tmp[N * m + k];
                }
            }
        }
    }
    mp_float_t det = det_sign;

    for(size_t m=0; m < N; m++){
        det *= tmp[m * (N+1)];
    }
    m_del(mp_float_t, tmp, N * N);
    return mp_obj_new_float(det);
}

MP_DEFINE_CONST_FUN_OBJ_1(linalg_det_obj, linalg_det);

#endif

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
    ndarray_obj_t *in = tools_object_is_square(oin);
    uint8_t *iarray = (uint8_t *)in->array;
    size_t S = in->shape[ULAB_MAX_DIMS - 1];
    mp_float_t *array = m_new(mp_float_t, S*S);
    for(size_t i=0; i < S; i++) { // rows
        for(size_t j=0; j < S; j++) { // columns
            *array++ = ndarray_get_float_value(iarray, in->dtype.type);
            iarray += in->strides[ULAB_MAX_DIMS - 1];
        }
        iarray -= in->strides[ULAB_MAX_DIMS - 1] * S;
        iarray += in->strides[ULAB_MAX_DIMS - 2];
    }
    array -= S * S;
    // make sure the matrix is symmetric
    for(size_t m=0; m < S; m++) {
        for(size_t n=m+1; n < S; n++) {
            // compare entry (m, n) to (n, m)
            // TODO: this must probably be scaled!
            if(LINALG_EPSILON < MICROPY_FLOAT_C_FUN(fabs)(array[m * S + n] - array[n * S + m])) {
                mp_raise_ValueError(translate("input matrix is asymmetric"));
            }
        }
    }

    // if we got this far, then the matrix will be symmetric

    ndarray_obj_t *eigenvectors = ndarray_new_dense_ndarray(2, ndarray_shape_vector(0, 0, S, S), NDARRAY_FLOAT);
    mp_float_t *eigvectors = (mp_float_t *)eigenvectors->array;

    size_t iterations = linalg_jacobi_rotations(array, eigvectors, S);

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
    ndarray_obj_t *ndarray = tools_object_is_square(o_in);
    uint8_t *array = (uint8_t *)ndarray->array;
    size_t N = ndarray->shape[ULAB_MAX_DIMS - 1];
    ndarray_obj_t *inverted = ndarray_new_dense_ndarray(2, ndarray_shape_vector(0, 0, N, N), NDARRAY_FLOAT);
    mp_float_t *iarray = (mp_float_t *)inverted->array;

    mp_float_t (*func)(void *) = ndarray_get_float_function(ndarray->dtype.type);

    for(size_t i=0; i < N; i++) { // rows
        for(size_t j=0; j < N; j++) { // columns
            *iarray++ = func(array);
            array += ndarray->strides[ULAB_MAX_DIMS - 1];
        }
        array -= ndarray->strides[ULAB_MAX_DIMS - 1] * N;
        array += ndarray->strides[ULAB_MAX_DIMS - 2];
    }
    // re-wind the pointer
    iarray -= N*N;

    if(!linalg_invert_matrix(iarray, N)) {
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

static mp_obj_t linalg_norm(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none} } ,
        { MP_QSTR_axis, MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t x = args[0].u_obj;
    mp_obj_t axis = args[1].u_obj;

    mp_float_t dot = 0.0, value;
    size_t count = 1;

    if(MP_OBJ_IS_TYPE(x, &mp_type_tuple) || MP_OBJ_IS_TYPE(x, &mp_type_list) || MP_OBJ_IS_TYPE(x, &mp_type_range)) {
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t item, iterable = mp_getiter(x, &iter_buf);
        while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            value = mp_obj_get_float(item);
            // we could simply take the sum of value ** 2,
            // but this method is numerically stable
            dot = dot + (value * value - dot) / count++;
        }
        return mp_obj_new_float(MICROPY_FLOAT_C_FUN(sqrt)(dot * (count - 1)));
    } else if(MP_OBJ_IS_TYPE(x, &ulab_ndarray_type)) {
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(x);
        uint8_t *array = (uint8_t *)ndarray->array;
        // always get a float, so that we don't have to resolve the dtype later
        mp_float_t (*func)(void *) = ndarray_get_float_function(ndarray->dtype.type);
        shape_strides _shape_strides = tools_reduce_axes(ndarray, axis);
        ndarray_obj_t *results = ndarray_new_dense_ndarray(_shape_strides.ndim, _shape_strides.shape, NDARRAY_FLOAT);
        mp_float_t *rarray = (mp_float_t *)results->array;

        #if ULAB_MAX_DIMS > 3
        size_t i = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 2
            size_t j = 0;
            do {
            #endif
                #if ULAB_MAX_DIMS > 1
                size_t k = 0;
                do {
                #endif
                    size_t l = 0;
                    if(axis != mp_const_none) {
                        count = 1;
                        dot = 0.0;
                    }
                    do {
                        value = func(array);
                        dot = dot + (value * value - dot) / count++;
                        array += _shape_strides.strides[0];
                        l++;
                    } while(l < _shape_strides.shape[0]);
                    *rarray = MICROPY_FLOAT_C_FUN(sqrt)(dot * (count - 1));
                #if ULAB_MAX_DIMS > 1
                    rarray += _shape_strides.increment;
                    array -= _shape_strides.strides[0] * _shape_strides.shape[0];
                    array += _shape_strides.strides[ULAB_MAX_DIMS - 1];
                    k++;
                } while(k < _shape_strides.shape[ULAB_MAX_DIMS - 1]);
                #endif
            #if ULAB_MAX_DIMS > 2
                array -= _shape_strides.strides[ULAB_MAX_DIMS - 1] * _shape_strides.shape[ULAB_MAX_DIMS - 1];
                array += _shape_strides.strides[ULAB_MAX_DIMS - 2];
                j++;
            } while(j < _shape_strides.shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 3
            array -= _shape_strides.strides[ULAB_MAX_DIMS - 2] * _shape_strides.shape[ULAB_MAX_DIMS - 2];
            array += _shape_strides.strides[ULAB_MAX_DIMS - 3];
            i++;
        } while(i < _shape_strides.shape[ULAB_MAX_DIMS - 3]);
        #endif
        if(results->ndim == 0) {
            return mp_obj_new_float(*rarray);
        }
        return results;
    }
    return mp_const_none; // we should never reach this point
}

MP_DEFINE_CONST_FUN_OBJ_KW(linalg_norm_obj, 1, linalg_norm);
// MP_DEFINE_CONST_FUN_OBJ_1(linalg_norm_obj, linalg_norm);

STATIC const mp_rom_map_elem_t ulab_linalg_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_linalg) },
    #if ULAB_MAX_DIMS > 1
        #if ULAB_LINALG_HAS_CHOLESKY
        { MP_ROM_QSTR(MP_QSTR_cholesky), (mp_obj_t)&linalg_cholesky_obj },
        #endif
        #if ULAB_LINALG_HAS_DET
        { MP_ROM_QSTR(MP_QSTR_det), (mp_obj_t)&linalg_det_obj },
        #endif
        #if ULAB_LINALG_HAS_EIG
        { MP_ROM_QSTR(MP_QSTR_eig), (mp_obj_t)&linalg_eig_obj },
        #endif
        #if ULAB_LINALG_HAS_INV
        { MP_ROM_QSTR(MP_QSTR_inv), (mp_obj_t)&linalg_inv_obj },
        #endif
    #endif
    #if ULAB_LINALG_HAS_NORM
    { MP_ROM_QSTR(MP_QSTR_norm), (mp_obj_t)&linalg_norm_obj },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_linalg_globals, ulab_linalg_globals_table);

mp_obj_module_t ulab_linalg_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_linalg_globals,
};

#endif

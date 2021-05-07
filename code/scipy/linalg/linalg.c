
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Vikas Udupa
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

#if ULAB_SCIPY_HAS_LINALG_MODULE
//|
//| import ulab.scipy
//|
//| """Linear algebra functions"""
//|

#if ULAB_MAX_DIMS > 1

#define TOLERANCE 0.0000001

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
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none} } ,
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none} } ,
        { MP_QSTR_lower, MP_ARG_OBJ, { .u_rom_obj = mp_const_false } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type) || !mp_obj_is_type(args[1].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("first two arguments must be ndarrays"));
    }

    ndarray_obj_t *A = MP_OBJ_TO_PTR(args[0].u_obj);
    ndarray_obj_t *b = MP_OBJ_TO_PTR(args[1].u_obj);
    
    if(!ndarray_is_dense(A) || !ndarray_is_dense(b)) {
        mp_raise_TypeError(translate("input must be a dense ndarray"));
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
        if (MICROPY_FLOAT_C_FUN(fabs)(get_A_ele(A_arr)) < TOLERANCE)
            mp_raise_ValueError(translate("input matrix is singular"));
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

#endif

static const mp_rom_map_elem_t ulab_scipy_linalg_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_linalg) },
    #if ULAB_MAX_DIMS > 1
        #if ULAB_SCIPY_LINALG_HAS_SOLVE_TRIANGULAR
        { MP_ROM_QSTR(MP_QSTR_solve_triangular), (mp_obj_t)&linalg_solve_triangular_obj },
        #endif
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_scipy_linalg_globals, ulab_scipy_linalg_globals_table);

mp_obj_module_t ulab_scipy_linalg_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_scipy_linalg_globals,
};

#endif

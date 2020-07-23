
/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös 
 *               2020 Scott Shawcroft for Adafruit Industries
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

static ndarray_obj_t *linalg_object_is_square(mp_obj_t obj) {
	// Returns an ndarray, if the object is a square ndarray, 
	// raises the appropriate exception otherwise
	if(!MP_OBJ_IS_TYPE(obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("size is defined for ndarrays only"));
    }
	ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(obj);
	if(ndarray->m != ndarray->n) {
		mp_raise_ValueError(translate("input must be square matrix"));
	}
	return ndarray;
}

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
        memcpy(&unit[m*(N+1)], &elem, sizeof(mp_float_t));
    }
    for(size_t m=0; m < N; m++){
        // this could be faster with ((c < epsilon) && (c > -epsilon))
        if(MICROPY_FLOAT_C_FUN(fabs)(data[m*(N+1)]) < epsilon) {
            m_del(mp_float_t, unit, N*N);
            return false;
        }
        for(size_t n=0; n < N; n++){
            if(m != n){
                elem = data[N*n+m] / data[m*(N+1)];
                for(size_t k=0; k < N; k++){
                    data[N*n+k] -= elem * data[N*m+k];
                    unit[N*n+k] -= elem * unit[N*m+k];
                }
            }
        }
    }
    for(size_t m=0; m < N; m++){ 
        elem = data[m*(N+1)];
        for(size_t n=0; n < N; n++){
            data[N*m+n] /= elem;
            unit[N*m+n] /= elem;
        }
    }
    memcpy(data, unit, sizeof(mp_float_t)*N*N);
    m_del(mp_float_t, unit, N*N);
    return true;
}

//| def cholesky(A):
//|    """
//|    :param ~ulab.array A: a positive definite, symmetric square matrix
//|    :return ~ulab.array L: a square root matrix in the lower triangular form
//|    :raises ValueError: If the input does not fulfill the necessary conditions
//|
//|    The returned matrix satisfies the equation m=LL*"""
//|    ...
//|

static mp_obj_t linalg_cholesky(mp_obj_t oin) {
	ndarray_obj_t *in = MP_OBJ_TO_PTR(oin);
	ndarray_obj_t *L = create_new_ndarray(in->n, in->n, NDARRAY_FLOAT);
    mp_float_t *array = (mp_float_t *)L->array->items;

	size_t pos = 0;
    for(size_t m=0; m < in->m; m++) { // rows
		for(size_t n=0; n < in->n; n++) { // columns
			array[m*in->m+n] = ndarray_get_float_value(in->array->items, in->array->typecode, pos++);
		}
	}

	// make sure the matrix is symmetric
    for(size_t m=0; m < in->m; m++) { // rows
        for(size_t n=m+1; n < in->n; n++) { // columns
            // compare entry (m, n) to (n, m)
            if(epsilon < MICROPY_FLOAT_C_FUN(fabs)(array[m*in->n + n] - array[n*in->n + m])) {
                mp_raise_ValueError(translate("input matrix is asymmetric"));
            }
        }
    }
	
	// this is actually not needed, but Cholesky in numpy returns the lower triangular matrix
	for(size_t i=0; i < in->m; i++) { // rows
		for(size_t j=i+1; j < in->n; j++) { // columns
			array[i*in->m + j] = 0.0;
		}
	}	
	mp_float_t sum = 0.0;
	for(size_t i=0; i < in->m; i++) { // rows
		for(size_t j=0; j <= i; j++) { // columns
			sum = array[i*in->m + j];
			for(size_t k=0; k < j; k++) {
				sum -= array[i*in->n + k] * array[j*in->n + k];
			}
			if(i == j) {
				if(sum <= 0.0) {
					mp_raise_ValueError(translate("matrix is not positive definite"));
				} else {
					array[i*in->m+i] = MICROPY_FLOAT_C_FUN(sqrt)(sum);
				}
			} else {
				array[i*in->m + j] = sum / array[j*in->m+j];
			}
		}
	}
	return MP_OBJ_FROM_PTR(L);
}

MP_DEFINE_CONST_FUN_OBJ_1(linalg_cholesky_obj, linalg_cholesky);

//| def det():
//|    """
//|    :param: m, a square matrix
//|    :return float: The determinant of the matrix"""
//|
//|    ...
//|

static mp_obj_t linalg_det(mp_obj_t oin) {
    ndarray_obj_t *in = linalg_object_is_square(oin);  
    mp_float_t *tmp = m_new(mp_float_t, in->n*in->n);
    for(size_t i=0; i < in->array->len; i++){
        tmp[i] = ndarray_get_float_value(in->array->items, in->array->typecode, i);
    }
    mp_float_t c;
    for(size_t m=0; m < in->m-1; m++){
        if(MICROPY_FLOAT_C_FUN(fabs)(tmp[m*(in->n+1)]) < epsilon) {
            m_del(mp_float_t, tmp, in->n*in->n);
            return mp_obj_new_float(0.0);
        }
        for(size_t n=0; n < in->n; n++){
            if(m != n) {
                c = tmp[in->n*n+m] / tmp[m*(in->n+1)];
                for(size_t k=0; k < in->n; k++){
                    tmp[in->n*n+k] -= c * tmp[in->n*m+k];
                }
            }
        }
    }
    mp_float_t det = 1.0;
                            
    for(size_t m=0; m < in->m; m++){ 
        det *= tmp[m*(in->n+1)];
    }
    m_del(mp_float_t, tmp, in->n*in->n);
    return mp_obj_new_float(det);
}

MP_DEFINE_CONST_FUN_OBJ_1(linalg_det_obj, linalg_det);

//| def dot(m1, m2):
//|    """
//|    :param ~ulab.array m1: a matrix, or a vector
//|    :param ~ulab.array m2: a matrix, or a vector
//|
//|    Computes the product of two matrices, or two vectors. In the letter case, the inner product is returned."""
//|    ...
//|

static mp_obj_t linalg_dot(mp_obj_t _m1, mp_obj_t _m2) {
    // TODO: should the results be upcast?
    if(!MP_OBJ_IS_TYPE(_m1, &ulab_ndarray_type) || !MP_OBJ_IS_TYPE(_m2, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("arguments must be ndarrays"));
    }
    ndarray_obj_t *m1 = MP_OBJ_TO_PTR(_m1);
    ndarray_obj_t *m2 = MP_OBJ_TO_PTR(_m2); 

    if ((m1->m == 1) && (m2->m == 1)) {
        // 2 vectors
        if (m1->array->len != m2->array->len) {
            mp_raise_ValueError(translate("vectors must have same lengths"));
        }
        mp_float_t dot = 0.0;
        for (size_t pos=0; pos < m1->array->len; pos++) {
            dot += ndarray_get_float_value(m1->array->items, m1->array->typecode, pos)*ndarray_get_float_value(m2->array->items, m2->array->typecode, pos);
        }
        return mp_obj_new_float(dot);        
    } else {
        // 2 matrices
        if(m1->n != m2->m) {
            mp_raise_ValueError(translate("matrix dimensions do not match"));
        }
        // TODO: numpy uses upcasting here
        ndarray_obj_t *out = create_new_ndarray(m1->m, m2->n, NDARRAY_FLOAT);
        mp_float_t *outdata = (mp_float_t *)out->array->items;
        for(size_t i=0; i < m1->m; i++) { // rows of m1
            for(size_t j=0; j < m2->n; j++) { // columns of m2
                mp_float_t sum = 0.0, v1, v2;
                for(size_t k=0; k < m2->m; k++) {
                    // (i, k) * (k, j)
                    v1 = ndarray_get_float_value(m1->array->items, m1->array->typecode, i*m1->n+k);
                    v2 = ndarray_get_float_value(m2->array->items, m2->array->typecode, k*m2->n+j);
                    sum += v1 * v2;
                }
                outdata[i*m2->n+j] = sum;
            }
        }
        return MP_OBJ_FROM_PTR(out);
    }
}

MP_DEFINE_CONST_FUN_OBJ_2(linalg_dot_obj, linalg_dot);

//| def eig(m):
//|    """
//|    :param m: a square matrix
//|    :return tuple (eigenvectors, eigenvalues):
//|
//|    Computes the eigenvalues and eigenvectors of a square matrix"""
//|    ...
//|

static mp_obj_t linalg_eig(mp_obj_t oin) {
	ndarray_obj_t *in = linalg_object_is_square(oin);
    mp_float_t *array = m_new(mp_float_t, in->array->len);
    for(size_t i=0; i < in->array->len; i++) {
        array[i] = ndarray_get_float_value(in->array->items, in->array->typecode, i);
    }
    // make sure the matrix is symmetric
    for(size_t m=0; m < in->m; m++) {
        for(size_t n=m+1; n < in->n; n++) {
            // compare entry (m, n) to (n, m)
            // TODO: this must probably be scaled!
            if(epsilon < MICROPY_FLOAT_C_FUN(fabs)(array[m*in->n + n] - array[n*in->n + m])) {
                mp_raise_ValueError(translate("input matrix is asymmetric"));
            }
        }
    }
    
    // if we got this far, then the matrix will be symmetric
    
    ndarray_obj_t *eigenvectors = create_new_ndarray(in->m, in->n, NDARRAY_FLOAT);
    mp_float_t *eigvectors = (mp_float_t *)eigenvectors->array->items;
    // start out with the unit matrix
    for(size_t m=0; m < in->m; m++) {
        eigvectors[m*(in->n+1)] = 1.0;
    }
    mp_float_t largest, w, t, c, s, tau, aMk, aNk, vm, vn;
    size_t M, N;
    size_t iterations = JACOBI_MAX*in->n*in->n;
    do {
        iterations--;
        // find the pivot here
        M = 0;
        N = 0;
        largest = 0.0;
        for(size_t m=0; m < in->m-1; m++) { // -1: no need to inspect last row
            for(size_t n=m+1; n < in->n; n++) {
                w = MICROPY_FLOAT_C_FUN(fabs)(array[m*in->n + n]);
                if((largest < w) && (epsilon < w)) {
                    M = m;
                    N = n;
                    largest = w;
                }
            }
        }
        if(M+N == 0) { // all entries are smaller than epsilon, there is not much we can do...
            break;
        }
        // at this point, we have the pivot, and it is the entry (M, N)
        // now we have to find the rotation angle
        w = (array[N*in->n + N] - array[M*in->n + M]) / (2.0*array[M*in->n + N]);
        // The following if/else chooses the smaller absolute value for the tangent 
        // of the rotation angle. Going with the smaller should be numerically stabler.
        if(w > 0) {
            t = MICROPY_FLOAT_C_FUN(sqrt)(w*w + 1.0) - w;
        } else {
            t = -1.0*(MICROPY_FLOAT_C_FUN(sqrt)(w*w + 1.0) + w);
        }
        s = t / MICROPY_FLOAT_C_FUN(sqrt)(t*t + 1.0); // the sine of the rotation angle
        c = 1.0 / MICROPY_FLOAT_C_FUN(sqrt)(t*t + 1.0); // the cosine of the rotation angle
        tau = (1.0-c)/s; // this is equal to the tangent of the half of the rotation angle
        
        // at this point, we have the rotation angles, so we can transform the matrix
        // first the two diagonal elements
        // a(M, M) = a(M, M) - t*a(M, N)
        array[M*in->n + M] = array[M*in->n + M] - t * array[M*in->n + N];
        // a(N, N) = a(N, N) + t*a(M, N)
        array[N*in->n + N] = array[N*in->n + N] + t * array[M*in->n + N];
        // after the rotation, the a(M, N), and a(N, M) entries should become zero
        array[M*in->n + N] = array[N*in->n + M] = 0.0;
        // then all other elements in the column
        for(size_t k=0; k < in->m; k++) {
            if((k == M) || (k == N)) {
                continue;
            }
            aMk = array[M*in->n + k];
            aNk = array[N*in->n + k];
            // a(M, k) = a(M, k) - s*(a(N, k) + tau*a(M, k))
            array[M*in->n + k] -= s*(aNk + tau*aMk);
            // a(N, k) = a(N, k) + s*(a(M, k) - tau*a(N, k))
            array[N*in->n + k] += s*(aMk - tau*aNk);
            // a(k, M) = a(M, k)
            array[k*in->n + M] = array[M*in->n + k];
            // a(k, N) = a(N, k)
            array[k*in->n + N] = array[N*in->n + k];
        }
        // now we have to update the eigenvectors
        // the rotation matrix, R, multiplies from the right
        // R is the unit matrix, except for the 
        // R(M,M) = R(N, N) = c
        // R(N, M) = s
        // (M, N) = -s
        // entries. This means that only the Mth, and Nth columns will change
        for(size_t m=0; m < in->m; m++) {
            vm = eigvectors[m*in->n+M];
            vn = eigvectors[m*in->n+N];
            // the new value of eigvectors(m, M)
            eigvectors[m*in->n+M] = c * vm - s * vn;
            // the new value of eigvectors(m, N)
            eigvectors[m*in->n+N] = s * vm + c * vn;
        }
    } while(iterations > 0);
    
    if(iterations == 0) { 
        // the computation did not converge; numpy raises LinAlgError
        m_del(mp_float_t, array, in->array->len);
        mp_raise_ValueError(translate("iterations did not converge"));
    }
    ndarray_obj_t *eigenvalues = create_new_ndarray(1, in->n, NDARRAY_FLOAT);
    mp_float_t *eigvalues = (mp_float_t *)eigenvalues->array->items;
    for(size_t i=0; i < in->n; i++) {
        eigvalues[i] = array[i*(in->n+1)];
    }
    m_del(mp_float_t, array, in->array->len);
    
    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
    tuple->items[0] = MP_OBJ_FROM_PTR(eigenvalues);
    tuple->items[1] = MP_OBJ_FROM_PTR(eigenvectors);
    return tuple;
}

MP_DEFINE_CONST_FUN_OBJ_1(linalg_eig_obj, linalg_eig);

//| def inv(m):
//|    """
//|    :param ~ulab.array m: a square matrix
//|    :return: The inverse of the matrix, if it exists
//|    :raises ValueError: if the matrix is not invertible
//|
//|    Computes the inverse of a square matrix"""
//|    ...
//|

static mp_obj_t linalg_inv(mp_obj_t o_in) {
	ndarray_obj_t *o = linalg_object_is_square(o_in);
    ndarray_obj_t *inverted = create_new_ndarray(o->m, o->n, NDARRAY_FLOAT);
    mp_float_t *data = (mp_float_t *)inverted->array->items;
    mp_obj_t elem;
    for(size_t m=0; m < o->m; m++) { // rows first
        for(size_t n=0; n < o->n; n++) { // columns next
            // this could, perhaps, be done in single line... 
            // On the other hand, we probably spend little time here
            elem = mp_binary_get_val_array(o->array->typecode, o->array->items, m*o->n+n);
            data[m*o->n+n] = (mp_float_t)mp_obj_get_float(elem);
        }
    }
    
    if(!linalg_invert_matrix(data, o->m)) {
        // TODO: I am not sure this is needed here. Otherwise, 
        // how should we free up the unused RAM of inverted?
        m_del(mp_float_t, inverted->array->items, o->n*o->n);
        mp_raise_ValueError(translate("input matrix is singular"));
    }
    return MP_OBJ_FROM_PTR(inverted);
}

MP_DEFINE_CONST_FUN_OBJ_1(linalg_inv_obj, linalg_inv);

//| def size(array):
//|    """Return the total number of elements in the array, as an integer."""
//|    ...
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
            return mp_obj_new_int(ndarray->array->len);
        } else if(MP_OBJ_IS_INT(args[1].u_obj)) {
            uint8_t ax = mp_obj_get_int(args[1].u_obj);
            if(ax == 0) {
                if(ndarray->m == 1) {
                    return mp_obj_new_int(ndarray->n);
                } else {
                    return mp_obj_new_int(ndarray->m);                    
                }
            } else if(ax == 1) {
                if(ndarray->m == 1) {
                    mp_raise_ValueError(translate("tuple index out of range"));
                } else {
                    return mp_obj_new_int(ndarray->n);
                }
            } else {
                    mp_raise_ValueError(translate("tuple index out of range"));
            }
        } else {
            mp_raise_TypeError(translate("wrong argument type"));
        }
    }
}

MP_DEFINE_CONST_FUN_OBJ_KW(linalg_size_obj, 1, linalg_size);

//| def trace(m):
//|    """
//|    :param m: a square matrix
//|
//|    Compute the trace of the matrix, the sum of its diagonal elements."""
//|    ...
//|

static mp_obj_t linalg_trace(mp_obj_t oin) {
	ndarray_obj_t *ndarray = linalg_object_is_square(oin);
	mp_float_t trace = 0.0;
	for(size_t pos=0; pos < ndarray->array->len; pos+=(ndarray->m+1)) {
		trace += ndarray_get_float_value(ndarray->array->items, ndarray->array->typecode, pos);
	}
	if(ndarray->array->typecode == NDARRAY_FLOAT) {
		return mp_obj_new_float(trace);
	}
	return mp_obj_new_int_from_float(trace);
}

MP_DEFINE_CONST_FUN_OBJ_1(linalg_trace_obj, linalg_trace);

STATIC const mp_rom_map_elem_t ulab_linalg_globals_table[] = {
	{ MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_linalg) },
    { MP_ROM_QSTR(MP_QSTR_cholesky), (mp_obj_t)&linalg_cholesky_obj },
	{ MP_ROM_QSTR(MP_QSTR_det), (mp_obj_t)&linalg_det_obj },
	{ MP_ROM_QSTR(MP_QSTR_dot), (mp_obj_t)&linalg_dot_obj },
	{ MP_ROM_QSTR(MP_QSTR_eig), (mp_obj_t)&linalg_eig_obj },
    { MP_ROM_QSTR(MP_QSTR_inv), (mp_obj_t)&linalg_inv_obj },
    { MP_ROM_QSTR(MP_QSTR_size), (mp_obj_t)&linalg_size_obj },
	{ MP_ROM_QSTR(MP_QSTR_trace), (mp_obj_t)&linalg_trace_obj },	
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_linalg_globals, ulab_linalg_globals_table);

mp_obj_module_t ulab_linalg_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_linalg_globals,
};

#endif

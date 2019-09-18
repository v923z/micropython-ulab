/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Zoltán Vörös
*/
    
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "linalg.h"

mp_obj_t linalg_transpose(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // the size of a single item in the array
    uint8_t _sizeof = mp_binary_get_size('@', self->data->typecode, NULL);
    
    // NOTE: In principle, we could simply specify the stride direction, and then we wouldn't 
    // even have to shuffle the elements. The downside of that approach is that we would have 
    // to implement two versions of the matrix multiplication and inversion functions
    
    // NOTE: 
    // if the matrices are square, we can simply swap items, but 
    // generic matrices can't be transposed in place, so we have to 
    // declare a temporary variable
    
    // NOTE: 
    //  In the old matrix, the coordinate (m, n) is m*self->n + n
    //  We have to assign this to the coordinate (n, m) in the new 
    //  matrix, i.e., to n*self->m + m
    
    // one-dimensional arrays can be transposed by simply swapping the dimensions
    if((self->m != 1) && (self->n != 1)) {
        uint8_t *c = (uint8_t *)self->data->items;
        // self->bytes is the size of the bytearray, irrespective of the typecode
        uint8_t *tmp = m_new(uint8_t, self->bytes);
        for(size_t m=0; m < self->m; m++) {
            for(size_t n=0; n < self->n; n++) {
                memcpy(tmp+_sizeof*(n*self->m + m), c+_sizeof*(m*self->n + n), _sizeof);
            }
        }
        memcpy(self->data->items, tmp, self->bytes);
        m_del(uint8_t, tmp, self->bytes);
    } 
    SWAP(size_t, self->m, self->n);
    return mp_const_none;
}

mp_obj_t linalg_reshape(mp_obj_t self_in, mp_obj_t shape) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if(!MP_OBJ_IS_TYPE(shape, &mp_type_tuple) || (MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(shape)) != 2)) {
        mp_raise_ValueError("shape must be a 2-tuple");
    }

    mp_obj_iter_buf_t iter_buf;
    mp_obj_t item, iterable = mp_getiter(shape, &iter_buf);
    uint16_t m, n;
    item = mp_iternext(iterable);
    m = mp_obj_get_int(item);
    item = mp_iternext(iterable);
    n = mp_obj_get_int(item);
    if(m*n != self->m*self->n) {
        // TODO: the proper error message would be "cannot reshape array of size %d into shape (%d, %d)"
        mp_raise_ValueError("cannot reshape array (incompatible input/output shape)");
    }
    self->m = m;
    self->n = n;
    return MP_OBJ_FROM_PTR(self);
}

ndarray_obj_t *invert_matrix(mp_obj_array_t *data, size_t N) {
    // After inversion the matrix is most certainly a float
    ndarray_obj_t *tmp = create_new_ndarray(N, N, NDARRAY_FLOAT);
    // initially, this is the unit matrix: this is what will be returned a
    // after all the transformations
    ndarray_obj_t *unitm = create_new_ndarray(N, N, NDARRAY_FLOAT);

    float *c = (float *)tmp->data->items;
    float *unit = (float *)unitm->data->items;
    mp_obj_t elem;
    float elemf;
    for(size_t m=0; m < N; m++) { // rows first
        for(size_t n=0; n < N; n++) { // columns next
            // this could, perhaps, be done in single line...
            elem = mp_binary_get_val_array(data->typecode, data->items, m*N+n);
            elemf = (float)mp_obj_get_float(elem);
            memcpy(&c[m*N+n], &elemf, sizeof(float));
        }
        // initialise the unit matrix
        elemf = 1.0;
        memcpy(&unit[m*(N+1)], &elemf, sizeof(float));
    }
    for(size_t m=0; m < N; m++){
        // this could be faster with ((c < epsilon) && (c > -epsilon))
        if(abs(c[m*(N+1)]) < epsilon) {
            // TODO: check what kind of exception numpy raises
            mp_raise_ValueError("input matrix is singular");
        }
        for(size_t n=0; n < N; n++){
            if(m != n){
                elemf = c[N*n+m] / c[m*(N+1)];
                for(size_t k=0; k < N; k++){
                    c[N*n+k] -= elemf * c[N*m+k];
                    unit[N*n+k] -= elemf * unit[N*m+k];
                }
            }
        }
    }
    for(size_t m=0; m < N; m++){ 
        elemf = c[m*(N+1)];
        for(size_t n=0; n < N; n++){
            c[N*m+n] /= elemf;
            unit[N*m+n] /= elemf;
        }
    }
    return unitm;
}

mp_obj_t linalg_inv(mp_obj_t o_in) {
    ndarray_obj_t *o = MP_OBJ_TO_PTR(o_in);
    if(!MP_OBJ_IS_TYPE(o_in, &ulab_ndarray_type)) {
        mp_raise_TypeError("only ndarray objects can be inverted");
    }
    if(o->m != o->n) {
        mp_raise_ValueError("only square matrices can be inverted");
    }
    ndarray_obj_t *inverted = invert_matrix(o->data, o->m);
    return MP_OBJ_FROM_PTR(inverted);
}

mp_obj_t linalg_dot(mp_obj_t _m1, mp_obj_t _m2) {
    // TODO: should the results be upcast?
    ndarray_obj_t *m1 = MP_OBJ_TO_PTR(_m1);
    ndarray_obj_t *m2 = MP_OBJ_TO_PTR(_m2);    
    if(m1->n != m2->m) {
        mp_raise_ValueError("matrix dimensions do not match");
    }
    ndarray_obj_t *out = create_new_ndarray(m1->m, m2->n, NDARRAY_FLOAT);
    float *outdata = (float *)out->data->items;
    float sum, v1, v2;
    for(size_t i=0; i < m1->n; i++) {
        for(size_t j=0; j < m2->m; j++) {
            sum = 0.0;
            for(size_t k=0; k < m1->m; k++) {
                // (j, k) * (k, j)
                v1 = ndarray_get_float_value(m1->data->items, m1->data->typecode, i*m1->n+k);
                v2 = ndarray_get_float_value(m2->data->items, m2->data->typecode, k*m2->n+j);
                sum += v1 * v2;
            }
            outdata[i*m1->m+j] = sum;
        }
    }
    return MP_OBJ_FROM_PTR(out);
}

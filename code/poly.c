/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Zoltán Vörös
*/
    
#include "py/obj.h"
#include "py/runtime.h"
#include "py/objarray.h"
#include "ndarray.h"
#include "linalg.h"
#include "poly.h"


bool object_is_nditerable(mp_obj_t o_in) {
    if(mp_obj_is_type(o_in, &ulab_ndarray_type) || 
      mp_obj_is_type(o_in, &mp_type_tuple) || 
      mp_obj_is_type(o_in, &mp_type_list) || 
      mp_obj_is_type(o_in, &mp_type_range)) {
        return true;
    }
    return false;
}

size_t get_nditerable_len(mp_obj_t o_in) {
    if(mp_obj_is_type(o_in, &ulab_ndarray_type)) {
        ndarray_obj_t *in = MP_OBJ_TO_PTR(o_in);
        return in->array->len;
    } else {
        return (size_t)mp_obj_get_int(mp_obj_len_maybe(o_in));
    }
}

mp_obj_t poly_polyval(mp_obj_t o_p, mp_obj_t o_x) {
    // TODO: return immediately, if o_p is not an iterable
    // TODO: there is a bug here: matrices won't work, 
    // because there is a single iteration loop
    size_t m, n;
    if(MP_OBJ_IS_TYPE(o_x, &ulab_ndarray_type)) {
        ndarray_obj_t *ndx = MP_OBJ_TO_PTR(o_x);
        m = ndx->m;
        n = ndx->n;
    } else {
        mp_obj_array_t *ix = MP_OBJ_TO_PTR(o_x);
        m = 1;
        n = ix->len;
    }
    // polynomials are going to be of type float, except, when both 
    // the coefficients and the independent variable are integers
    ndarray_obj_t *out = create_new_ndarray(m, n, NDARRAY_FLOAT);
    mp_obj_iter_buf_t x_buf;
    mp_obj_t x_item, x_iterable = mp_getiter(o_x, &x_buf);

    mp_obj_iter_buf_t p_buf;
    mp_obj_t p_item, p_iterable;

    mp_float_t x, y;
    mp_float_t *outf = (mp_float_t *)out->array->items;
    uint8_t plen = mp_obj_get_int(mp_obj_len_maybe(o_p));
    mp_float_t *p = m_new(mp_float_t, plen);
    p_iterable = mp_getiter(o_p, &p_buf);
    uint16_t i = 0;    
    while((p_item = mp_iternext(p_iterable)) != MP_OBJ_STOP_ITERATION) {
        p[i] = mp_obj_get_float(p_item);
        i++;
    }
    i = 0;
    while ((x_item = mp_iternext(x_iterable)) != MP_OBJ_STOP_ITERATION) {
        x = mp_obj_get_float(x_item);
        y = p[0];
        for(uint8_t j=0; j < plen-1; j++) {
            y *= x;
            y += p[j+1];
        }
        outf[i++] = y;
    }
    m_del(mp_float_t, p, plen);
    return MP_OBJ_FROM_PTR(out);
}

mp_obj_t poly_polyfit(size_t  n_args, const mp_obj_t *args) {
    if((n_args != 2) && (n_args != 3)) {
        mp_raise_ValueError("number of arguments must be 2, or 3");
    }
    if(!object_is_nditerable(args[0])) {
        mp_raise_ValueError("input data must be an iterable");
    }
    uint16_t lenx = 0, leny = 0;
    uint8_t deg = 0;
    mp_float_t *x, *XT, *y, *prod;

    if(n_args == 2) { // only the y values are supplied
        // TODO: this is actually not enough: the first argument can very well be a matrix, 
        // in which case we are between the rock and a hard place
        leny = (uint16_t)mp_obj_get_int(mp_obj_len_maybe(args[0]));
        deg = (uint8_t)mp_obj_get_int(args[1]);
        if(leny < deg) {
            mp_raise_ValueError("more degrees of freedom than data points");
        }
        lenx = leny;
        x = m_new(mp_float_t, lenx); // assume uniformly spaced data points
        for(size_t i=0; i < lenx; i++) {
            x[i] = i;
        }
        y = m_new(mp_float_t, leny);
        fill_array_iterable(y, args[0]);
    } else if(n_args == 3) {
        lenx = (uint16_t)mp_obj_get_int(mp_obj_len_maybe(args[0]));
        leny = (uint16_t)mp_obj_get_int(mp_obj_len_maybe(args[0]));
        if(lenx != leny) {
            mp_raise_ValueError("input vectors must be of equal length");
        }
        deg = (uint8_t)mp_obj_get_int(args[2]);
        if(leny < deg) {
            mp_raise_ValueError("more degrees of freedom than data points");
        }
        x = m_new(mp_float_t, lenx);
        fill_array_iterable(x, args[0]);
        y = m_new(mp_float_t, leny);
        fill_array_iterable(y, args[1]);
    }
    
    // one could probably express X as a function of XT, 
    // and thereby save RAM, because X is used only in the product
    XT = m_new(mp_float_t, (deg+1)*leny); // XT is a matrix of shape (deg+1, len) (rows, columns)
    for(uint8_t i=0; i < leny; i++) { // column index
        XT[i+0*lenx] = 1.0; // top row
        for(uint8_t j=1; j < deg+1; j++) { // row index
            XT[i+j*leny] = XT[i+(j-1)*leny]*x[i];
        }
    }
    
    prod = m_new(mp_float_t, (deg+1)*(deg+1)); // the product matrix is of shape (deg+1, deg+1)
    mp_float_t sum;
    for(uint16_t i=0; i < deg+1; i++) { // column index
        for(uint16_t j=0; j < deg+1; j++) { // row index
            sum = 0.0;
            for(size_t k=0; k < lenx; k++) {
                // (j, k) * (k, i) 
                // Note that the second matrix is simply the transpose of the first: 
                // X(k, i) = XT(i, k) = XT[k*lenx+i]
                sum += XT[j*lenx+k]*XT[i*lenx+k]; // X[k*(deg+1)+i];
            }
            prod[j*(deg+1)+i] = sum;
        }
    }
    if(!linalg_invert_matrix(prod, deg+1)) {
        // Although X was a Vandermonde matrix, whose inverse is guaranteed to exist, 
        // we bail out here, if prod couldn't be inverted: if the values in x are not all 
        // distinct, prod is singular
        m_del(mp_float_t, XT, (deg+1)*lenx);
        m_del(mp_float_t, x, lenx);
        m_del(mp_float_t, y, lenx);
        m_del(mp_float_t, prod, (deg+1)*(deg+1));
        mp_raise_ValueError("could not invert Vandermonde matrix");
    } 
    // at this point, we have the inverse of X^T * X
    // y is a column vector; x is free now, we can use it for storing intermediate values
    for(uint16_t i=0; i < deg+1; i++) { // row index
        sum = 0.0;
        for(uint16_t j=0; j < lenx; j++) { // column index
            sum += XT[i*lenx+j]*y[j];
        }
        x[i] = sum;
    }
    // XT is no longer needed
    m_del(mp_float_t, XT, (deg+1)*leny);
    
    ndarray_obj_t *beta = create_new_ndarray(deg+1, 1, NDARRAY_FLOAT);
    mp_float_t *betav = (mp_float_t *)beta->array->items;
    // x[0..(deg+1)] contains now the product X^T * y; we can get rid of y
    m_del(float, y, leny);
    
    // now, we calculate beta, i.e., we apply prod = (X^T * X)^(-1) on x = X^T * y; x is a column vector now
    for(uint16_t i=0; i < deg+1; i++) {
        sum = 0.0;
        for(uint16_t j=0; j < deg+1; j++) {
            sum += prod[i*(deg+1)+j]*x[j];
        }
        betav[i] = sum;
    }
    m_del(mp_float_t, x, lenx);
    m_del(mp_float_t, prod, (deg+1)*(deg+1));
    for(uint8_t i=0; i < (deg+1)/2; i++) {
        // We have to reverse the array, for the leading coefficient comes first. 
        SWAP(mp_float_t, betav[i], betav[deg-i]);
    }
    return MP_OBJ_FROM_PTR(beta);
}

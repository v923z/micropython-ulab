/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Zoltán Vörös
*/
    
#include "py/runtime.h"
#include "py/objarray.h"
#include "ndarray.h"
#include "poly.h"

mp_obj_t poly_polyval(mp_obj_t o_p, mp_obj_t o_x) {
    // TODO: return immediately, if o_p is not an iterable
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
    // polynomials are going to be of float, except, when both 
    // the coefficients and the independent variable are integers
    ndarray_obj_t *out = create_new_ndarray(m, n, NDARRAY_FLOAT);
    mp_obj_iter_buf_t x_buf;
    mp_obj_t x_item, x_iterable = mp_getiter(o_x, &x_buf);

    mp_obj_iter_buf_t p_buf;
    mp_obj_t p_item, p_iterable;

    size_t i = 0;
    mp_float_t x, y;
    float *outf = (float *)out->data->items;
    while ((x_item = mp_iternext(x_iterable)) != MP_OBJ_STOP_ITERATION) {
        x = mp_obj_get_float(x_item);
        p_iterable = mp_getiter(o_p, &p_buf);
        p_item = mp_iternext(p_iterable);
        y = mp_obj_get_float(p_item);
        while((p_item = mp_iternext(p_iterable)) != MP_OBJ_STOP_ITERATION) {
            y *= x;
            y += mp_obj_get_float(p_item);
        }
        outf[i++] = y;
    }
    return MP_OBJ_FROM_PTR(out);
}


/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *               2020 Zoltán Vörös
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "filter.h"

#if ULAB_FILTER_MODULE
static mp_obj_t filter_convolve(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_a, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_v, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!MP_OBJ_IS_TYPE(args[0].u_obj, &ulab_ndarray_type) || !MP_OBJ_IS_TYPE(args[1].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("convolve arguments must be ndarrays"));
    }

    ndarray_obj_t *a = MP_OBJ_TO_PTR(args[0].u_obj);
    ndarray_obj_t *c = MP_OBJ_TO_PTR(args[1].u_obj);
    size_t len_a = a->array->len;
    size_t len_c = c->array->len;
    // deal with linear arrays only
    if(a->m*a->n != len_a || c->m*c->n != len_c) {
        mp_raise_TypeError(translate("convolve arguments must be linear arrays"));
    }
    if(len_a == 0 || len_c == 0) {
        mp_raise_TypeError(translate("convolve arguments must not be empty"));
    }

    int len = len_a + len_c - 1; // convolve mode "full"
    ndarray_obj_t *out = create_new_ndarray(1, len, NDARRAY_FLOAT);
    mp_float_t *outptr = out->array->items;
    int off = len_c-1;

    if(a->array->typecode == NDARRAY_FLOAT && c->array->typecode == NDARRAY_FLOAT) {
        mp_float_t* a_items = (mp_float_t*)a->array->items;
        mp_float_t* c_items = (mp_float_t*)c->array->items;
        for(int k=-off; k<len-off; k++) {
            mp_float_t accum = (mp_float_t)0;
            int top_n = MIN(len_c, len_a - k);
            int bot_n = MAX(-k, 0);
            mp_float_t* a_ptr = a_items + bot_n + k;
            mp_float_t* a_end = a_ptr + (top_n - bot_n);
            mp_float_t* c_ptr = c_items + len_c - bot_n - 1;
            for(; a_ptr != a_end;) {
                accum += *a_ptr++ * *c_ptr--;
            }
            *outptr++ = accum;
        }
    } else {
        for(int k=-off; k<len-off; k++) {
            mp_float_t accum = (mp_float_t)0;
            int top_n = MIN(len_c, len_a - k);
            int bot_n = MAX(-k, 0);
            for(int n=bot_n; n<top_n; n++) {
                int idx_c = len_c - n - 1;
                int idx_a = n+k;
                mp_float_t ai = ndarray_get_float_value(a->array->items, a->array->typecode, idx_a);
                mp_float_t ci = ndarray_get_float_value(c->array->items, c->array->typecode, idx_c);
                accum += ai * ci;
            }
            *outptr++ = accum;
        }
    }

    return out;
}

MP_DEFINE_CONST_FUN_OBJ_KW(filter_convolve_obj, 2, filter_convolve);

static void filter_sosfilt_array(mp_float_t *x, const mp_float_t *coeffs, mp_float_t *zf, const size_t len) {
    for(size_t i=0; i < len; i++) {
        mp_float_t xn = *x;
        *x = coeffs[0] * xn + zf[0];
        zf[0] = zf[1] + coeffs[1] * xn - coeffs[4] * *x;
        zf[1] = coeffs[2] * xn - coeffs[5] * *x;
        x++;
    }
    x -= len;
}

static mp_obj_t filter_sosfilt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sos, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_zi, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!ndarray_object_is_nditerable(args[0].u_obj) || !ndarray_object_is_nditerable(args[1].u_obj)) {
        mp_raise_TypeError(translate("sosfilt requires iterable arguments"));
    }
    size_t lenx = (size_t)mp_obj_get_int(mp_obj_len_maybe(args[1].u_obj));
    ndarray_obj_t *y = create_new_ndarray(1, lenx, NDARRAY_FLOAT);
    mp_float_t *yarray = (mp_float_t *)y->array->items;
    mp_float_t coeffs[6];
    if(MP_OBJ_IS_TYPE(args[1].u_obj, &ulab_ndarray_type)) {
        ndarray_obj_t *inarray = MP_OBJ_TO_PTR(args[1].u_obj);
        for(size_t i=0; i < lenx; i++) {
            *yarray++ = ndarray_get_float_value(inarray->array->items, inarray->array->typecode, i);
        }
        yarray -= lenx;
    } else {
        fill_array_iterable(yarray, args[1].u_obj);
    }

    mp_obj_iter_buf_t iter_buf;
    mp_obj_t item, iterable = mp_getiter(args[0].u_obj, &iter_buf);
    size_t lensos = (size_t)mp_obj_get_int(mp_obj_len_maybe(args[0].u_obj));

    ndarray_obj_t *zf = create_new_ndarray(lensos, 2, NDARRAY_FLOAT);
    mp_float_t *zf_array = (mp_float_t *)zf->array->items;

    if(args[2].u_obj != mp_const_none) {
        if(!MP_OBJ_IS_TYPE(args[2].u_obj, &ulab_ndarray_type)) {
            mp_raise_TypeError(translate("zi must be an ndarray"));
        } else {
            ndarray_obj_t *zi = MP_OBJ_TO_PTR(args[2].u_obj);
            if((zi->m != lensos) || (zi->n != 2)) {
                mp_raise_ValueError(translate("zi must be of shape (n_section, 2)"));
            }
            if(zi->array->typecode != NDARRAY_FLOAT) {
                mp_raise_ValueError(translate("zi must be of float type"));
            }
            memcpy(zf_array, zi->array->items, 2*lensos*sizeof(mp_float_t));
        }
    }
    while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        if(mp_obj_get_int(mp_obj_len_maybe(item)) != 6) {
            mp_raise_ValueError(translate("sos array must be of shape (n_section, 6)"));
        } else {
            fill_array_iterable(coeffs, item);
            if(coeffs[3] != 1.0) {
                mp_raise_ValueError(translate("sos[:, 3] should be all ones"));
            }
            filter_sosfilt_array(yarray, coeffs, zf_array, lenx);
            zf_array += 2;
        }
    }
    if(args[2].u_obj == mp_const_none) {
        return MP_OBJ_FROM_PTR(y);
    } else {
        mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
        tuple->items[0] = MP_OBJ_FROM_PTR(y);
        tuple->items[1] = MP_OBJ_FROM_PTR(zf);
        return tuple;
    }
}

MP_DEFINE_CONST_FUN_OBJ_KW(filter_sosfilt_obj, 2, filter_sosfilt);

STATIC const mp_rom_map_elem_t ulab_filter_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_filter) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_convolve), (mp_obj_t)&filter_convolve_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sosfilt), (mp_obj_t)&filter_sosfilt_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_filter_globals, ulab_filter_globals_table);

mp_obj_module_t ulab_filter_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_filter_globals,
};

#endif

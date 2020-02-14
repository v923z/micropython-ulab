
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "filter.h"

#if ULAB_FILTER_MODULE
mp_obj_t filter_convolve(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_a, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_v, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(2, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!MP_OBJ_IS_TYPE(args[0].u_obj, &ulab_ndarray_type) || !MP_OBJ_IS_TYPE(args[1].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("convolve arguments must be ndarrays"));
    }

    ndarray_obj_t *a = MP_OBJ_TO_PTR(args[0].u_obj);
    ndarray_obj_t *c = MP_OBJ_TO_PTR(args[1].u_obj);
    int len_a = a->array->len;
    int len_c = c->array->len;
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

STATIC const mp_rom_map_elem_t ulab_filter_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_filter) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_convolve), (mp_obj_t)&filter_convolve_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_filter_globals, ulab_filter_globals_table);

mp_obj_module_t ulab_filter_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_filter_globals,
};

#endif

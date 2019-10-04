/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Zoltán Vörös
*/
    
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/misc.h"
#include "numerical.h"

enum NUMERICAL_FUNCTION_TYPE {
    NUMERICAL_MIN,
    NUMERICAL_MAX,
    NUMERICAL_ARGMIN,
    NUMERICAL_ARGMAX,
    NUMERICAL_SUM,
    NUMERICAL_MEAN,
    NUMERICAL_STD,
};

mp_obj_t numerical_linspace(mp_obj_t _start, mp_obj_t _stop, mp_obj_t _len) {
    // TODO: accept keyword argument endpoint=True, dtype=...
    mp_int_t len = mp_obj_get_int_truncated(_len);
    if(len < 2) {
        mp_raise_ValueError("number of points must be at least 2");
    }
    mp_float_t value, step;
    value = mp_obj_get_float(_start);
    step = (mp_obj_get_float(_stop)-value)/(len-1);
    ndarray_obj_t *nd_array = create_new_ndarray(1, len, NDARRAY_FLOAT);
    for(size_t i=0; i < len; i++, value += step) {
        mp_binary_set_val_array(NDARRAY_FLOAT, nd_array->array->items, i, mp_obj_new_float(value));
    }
    return MP_OBJ_FROM_PTR(nd_array);
}

mp_obj_t numerical_sum_mean_std_array(mp_obj_t oin, uint8_t optype) {
    mp_float_t value, sum = 0.0, sq_sum = 0.0;
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t item, iterable = mp_getiter(oin, &iter_buf);
    mp_int_t len = mp_obj_get_int(mp_obj_len(oin));
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        value = mp_obj_get_float(item);
        sum += value;
        if(optype == NUMERICAL_STD) {
            sq_sum += value*value;
        }
    }
    if(optype ==  NUMERICAL_SUM) {
        return mp_obj_new_float(sum);
    } else if(optype == NUMERICAL_MEAN) {
        return mp_obj_new_float(sum/len);
    } else {
        sum /= len; // this is now the mean!
        return mp_obj_new_float(sqrtf((sq_sum/len-sum*sum)));
    }
}

STATIC mp_obj_t numerical_argmin_argmax_array(mp_obj_t o_in, mp_uint_t op, uint8_t type) {
    size_t idx = 0, best_idx = 0;
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(o_in, &iter_buf);
    mp_obj_t best_obj = MP_OBJ_NULL;
    mp_obj_t item;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        if ((best_obj == MP_OBJ_NULL) || (mp_binary_op(op, item, best_obj) == mp_const_true)) {
            best_obj = item;
            best_idx = idx;
        }
        idx++;
    }
    if((type == NUMERICAL_ARGMIN) || (type == NUMERICAL_ARGMAX)) {
        return MP_OBJ_NEW_SMALL_INT(best_idx);
    } else {
        return best_obj;
    }
}

STATIC size_t numerical_argmin_argmax_single_line(void *data, size_t start, size_t stop, 
                                                  size_t stride, uint8_t typecode, uint8_t optype) {
    size_t best_idx = start;
    mp_float_t value, best_value = ndarray_get_float_value(data, typecode, start);
    
    for(size_t i=start; i < stop; i+=stride) {
        value = ndarray_get_float_value(data, typecode, i);
        if((optype == NUMERICAL_MIN) || (optype == NUMERICAL_ARGMIN)) {
            if(best_value > value) {
                best_value = value;
                best_idx = i;
            }
        } else if((optype == NUMERICAL_MAX) || (optype == NUMERICAL_ARGMAX)) {
            if(best_value < value) {
                best_value = value;
                best_idx = i;
            }
        }
    }
    return best_idx;
}

STATIC mp_obj_t numerical_argmin_argmax_matrix(mp_obj_t oin, mp_obj_t axis, uint8_t optype) {
    ndarray_obj_t *in = MP_OBJ_TO_PTR(oin);
    size_t best_idx;
    if((axis == mp_const_none) || (in->m == 1) || (in->n == 1)) { 
        // return the value for the flattened array
        best_idx = numerical_argmin_argmax_single_line(in->array->items, 0, 
                                                      in->array->len, 1, in->array->typecode, optype);
        if((optype == NUMERICAL_ARGMIN) || (optype == NUMERICAL_ARGMAX)) {
            return MP_OBJ_NEW_SMALL_INT(best_idx);
        } else {
            // TODO: do we have to do type conversion here, depending on the type of the input array?
            return mp_obj_new_float(ndarray_get_float_value(in->array->items, in->array->typecode, best_idx));
        }
    } else {
        uint8_t _axis = mp_obj_get_int(axis);
        size_t m = (_axis == 0) ? 1 : in->m;
        size_t n = (_axis == 0) ? in->n : 1;
        size_t len = in->array->len;
        // TODO: pass in->array->typcode to create_new_ndarray
        ndarray_obj_t *out = create_new_ndarray(m, n, NDARRAY_FLOAT);

        // TODO: these two cases could probably be combined in a more elegant fashion...
        if(_axis == 0) { // vertical
            for(size_t i=0; i < n; i++) {
                best_idx = numerical_argmin_argmax_single_line(in->array->items, i, len, 
                                                               n, in->array->typecode, optype);
                if((optype == NUMERICAL_ARGMIN) || (optype == NUMERICAL_ARGMAX)) {
                    ((float_t *)out->array->items)[i] = (float)best_idx;
                } else {
                    ((float_t *)out->array->items)[i] = ndarray_get_float_value(in->array->items, in->array->typecode, best_idx);
                }
            }
        } else { // horizontal
            for(size_t i=0; i < m; i++) {
                best_idx = numerical_argmin_argmax_single_line(in->array->items, i*in->n, 
                                                               (i+1)*in->n, 1, in->array->typecode, optype);
                if((optype == NUMERICAL_ARGMIN) || (optype == NUMERICAL_ARGMAX)) {
                    ((float_t *)out->array->items)[i] = (float)best_idx;
                } else {
                    ((float_t *)out->array->items)[i] = ndarray_get_float_value(in->array->items, in->array->typecode, best_idx);
                }

            }
        }
    return MP_OBJ_FROM_PTR(out);
    }
    return mp_const_none;
}

STATIC mp_float_t numerical_sum_mean_std_single_line(void *data, size_t start, size_t stop, 
                                                  size_t stride, uint8_t typecode, uint8_t optype) {
    
    mp_float_t sum = 0.0, sq_sum = 0.0, value;
    size_t len = 0;
    for(size_t i=start; i < stop; i+=stride, len++) {
        value = ndarray_get_float_value(data, typecode, i);        
        sum += value;
        if(optype == NUMERICAL_STD) {
            sq_sum += value*value;
        }
    }
    if(len == 0) {
        mp_raise_ValueError("data length is 0!");
    }
    if(optype ==  NUMERICAL_SUM) {
        return sum;
    } else if(optype == NUMERICAL_MEAN) {
        return sum/len;
    } else {
        sum /= len; // this is now the mean!
        return sqrtf((sq_sum/len-sum*sum));
    }
}

STATIC mp_obj_t numerical_sum_mean_std_matrix(mp_obj_t oin, mp_obj_t axis, uint8_t optype) {
    ndarray_obj_t *in = MP_OBJ_TO_PTR(oin);
    if((axis == mp_const_none) || (in->m == 1) || (in->n == 1)) { 
        // return the value for the flattened array
        return mp_obj_new_float(numerical_sum_mean_std_single_line(in->array->items, 0, 
                                                      in->array->len, 1, in->array->typecode, optype));
    } else {
        uint8_t _axis = mp_obj_get_int(axis);
        size_t m = (_axis == 0) ? 1 : in->m;
        size_t n = (_axis == 0) ? in->n : 1;
        size_t len = in->array->len;
        mp_float_t sms;
        // TODO: pass in->array->typcode to create_new_ndarray
        ndarray_obj_t *out = create_new_ndarray(m, n, NDARRAY_FLOAT);

        // TODO: these two cases could probably be combined in a more elegant fashion...
        if(_axis == 0) { // vertical
            for(size_t i=0; i < n; i++) {
                sms = numerical_sum_mean_std_single_line(in->array->items, i, len, 
                                                               n, in->array->typecode, optype);
                ((float_t *)out->array->items)[i] = sms;
            }
        } else { // horizontal
            for(size_t i=0; i < m; i++) {
                sms = numerical_sum_mean_std_single_line(in->array->items, i*in->n, 
                                                               (i+1)*in->n, 1, in->array->typecode, optype);
                ((float_t *)out->array->items)[i] = sms;
            }
        }
    return MP_OBJ_FROM_PTR(out);
    }
}

STATIC mp_obj_t numerical_function(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args, uint8_t type) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} } ,
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(1, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    mp_obj_t oin = args[0].u_obj;
    mp_obj_t axis = args[1].u_obj;
    if((axis != mp_const_none) && (mp_obj_get_int(axis) != 0) && (mp_obj_get_int(axis) != 1)) {
        // this seems to pass with False, and True...
        mp_raise_ValueError("axis must be None, 0, or 1");
    }
    
    if(MP_OBJ_IS_TYPE(oin, &mp_type_tuple) || MP_OBJ_IS_TYPE(oin, &mp_type_list) || 
        MP_OBJ_IS_TYPE(oin, &mp_type_range)) {
        switch(type) {
            case NUMERICAL_MIN:
            case NUMERICAL_ARGMIN:
                return numerical_argmin_argmax_array(oin, MP_BINARY_OP_LESS, type);
            case NUMERICAL_MAX:
            case NUMERICAL_ARGMAX:
                return numerical_argmin_argmax_array(oin, MP_BINARY_OP_MORE, type);
            case NUMERICAL_SUM:
            case NUMERICAL_MEAN:
            case NUMERICAL_STD:
                return numerical_sum_mean_std_array(oin, type);
            default: // we should never reach this point, but whatever
                return mp_const_none;
        }
    } else if(MP_OBJ_IS_TYPE(oin, &ulab_ndarray_type)) {
        switch(type) {
            case NUMERICAL_MIN:
            case NUMERICAL_MAX:
            case NUMERICAL_ARGMIN:
            case NUMERICAL_ARGMAX:
                return numerical_argmin_argmax_matrix(oin, axis, type);
            case NUMERICAL_SUM:
            case NUMERICAL_MEAN:
            case NUMERICAL_STD:
                return numerical_sum_mean_std_matrix(oin, axis, type);            
            default:
                mp_raise_NotImplementedError("operation is not implemented on ndarrays");
        }
    } else {
        mp_raise_TypeError("input must be tuple, list, range, or ndarray");
    }
    return mp_const_none;
}

mp_obj_t numerical_min(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_MIN);
}

mp_obj_t numerical_max(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_MAX);
}

mp_obj_t numerical_argmin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_ARGMIN);
}

mp_obj_t numerical_argmax(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_ARGMAX);
}

mp_obj_t numerical_sum(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_SUM);
}

mp_obj_t numerical_mean(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_MEAN);
}

mp_obj_t numerical_std(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_STD);
}

mp_obj_t numerical_roll(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // TODO: replace memcpy by memmove
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(2, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    mp_obj_t oin = args[0].u_obj;
    int16_t shift = mp_obj_get_int(args[1].u_obj);
    int8_t axis = args[2].u_int;
    if((axis != 0) && (axis != 1)) {
        mp_raise_ValueError("axis must be None, 0, or 1");
    }
    ndarray_obj_t *in = MP_OBJ_TO_PTR(oin);
    uint8_t _sizeof = mp_binary_get_size('@', in->array->typecode, NULL);
    size_t len;
    int16_t _shift;
    uint8_t *data = (uint8_t *)in->array->items;
    // TODO: transpose the matrix, if axis == 0
    if(shift < 0) {
        _shift = -shift;
    } else {
        _shift = shift;
    }
    if(axis == 0) {
        len = in->m;
        // temporary buffer
        uint8_t *_data = m_new(uint8_t, _sizeof*len);
        
        _shift = _shift % len;
        if(shift < 0) _shift = len - _shift;
        _shift *= _sizeof;
        uint8_t *tmp = m_new(uint8_t, _shift);

        for(size_t n=0; n < in->n; n++) {
            for(size_t m=0; m < len; m++) {
                // this loop should fill up the temporary buffer
                memcpy(&_data[m*_sizeof], &data[(m*in->n+n)*_sizeof], _sizeof);
            }
            // now, the actual shift
            memcpy(tmp, _data, _shift);
            memcpy(_data, &_data[_shift], len*_sizeof-_shift);
            memcpy(&_data[len*_sizeof-_shift], tmp, _shift);
            for(size_t m=0; m < len; m++) {
                // this loop should dump the content of the temporary buffer into data
                memcpy(&data[(m*in->n+n)*_sizeof], &_data[m*_sizeof], _sizeof);
            }            
        }
        m_del(uint8_t, tmp, _shift);
        m_del(uint8_t, _data, _sizeof*len);
        return mp_const_none;
    }
    len = in->n;
    if((in->m == 1) || (in->n == 1)) {
        len = in->array->len;
    }
    _shift = _shift % len;
    if(shift < 0) _shift = len - _shift;
    // TODO: if(shift > len/2), we should move in the opposite direction. That would save RAM
    _shift *= _sizeof;
    uint8_t *tmp = m_new(uint8_t, _shift);
    for(size_t m=0; m < in->m; m++) {
        memcpy(tmp, &data[m*len*_sizeof], _shift);
        memcpy(&data[m*len*_sizeof], &data[m*len*_sizeof+_shift], len*_sizeof-_shift);
        memcpy(&data[(m+1)*len*_sizeof-_shift], tmp, _shift);
    }
    m_del(uint8_t, tmp, _shift);
    return mp_const_none;
}

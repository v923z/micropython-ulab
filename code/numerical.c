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
#include "py/objint.h"
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

mp_obj_t numerical_linspace(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) } },
        { MP_QSTR_num, MP_ARG_INT, {.u_int = 50} },
        { MP_QSTR_endpoint, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_true_obj)} },
        { MP_QSTR_retstep, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_false_obj)} },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = NDARRAY_FLOAT} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(2, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint16_t len = args[2].u_int;
    if(len < 2) {
        mp_raise_ValueError("number of points must be at least 2");
    }
    mp_float_t value, step;
    value = mp_obj_get_float(args[0].u_obj);
    uint8_t typecode = args[5].u_int;
    if(args[3].u_obj == mp_const_true) step = (mp_obj_get_float(args[1].u_obj)-value)/(len-1);
    else step = (mp_obj_get_float(args[1].u_obj)-value)/len;
    ndarray_obj_t *ndarray = create_new_ndarray(1, len, typecode);
    if(typecode == NDARRAY_UINT8) {
        uint8_t *array = (uint8_t *)ndarray->array->items;
        for(size_t i=0; i < len; i++, value += step) array[i] = (uint8_t)value;
    } else if(typecode == NDARRAY_INT8) {
        int8_t *array = (int8_t *)ndarray->array->items;
        for(size_t i=0; i < len; i++, value += step) array[i] = (int8_t)value;
    } else if(typecode == NDARRAY_UINT16) {
        uint16_t *array = (uint16_t *)ndarray->array->items;
        for(size_t i=0; i < len; i++, value += step) array[i] = (uint16_t)value;
    } else if(typecode == NDARRAY_INT16) {
        int16_t *array = (int16_t *)ndarray->array->items;
        for(size_t i=0; i < len; i++, value += step) array[i] = (int16_t)value;
    } else {
        mp_float_t *array = (mp_float_t *)ndarray->array->items;
        for(size_t i=0; i < len; i++, value += step) array[i] = value;
    }
    if(args[4].u_obj == mp_const_false) {
        return MP_OBJ_FROM_PTR(ndarray);
    } else {
        mp_obj_t tuple[2];
        tuple[0] = ndarray;
        tuple[1] = mp_obj_new_float(step);
        return mp_obj_new_tuple(2, tuple);
    }
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
        return mp_obj_new_float(MICROPY_FLOAT_C_FUN(sqrt)(sq_sum/len-sum*sum));
    }
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
        return MICROPY_FLOAT_C_FUN(sqrt)(sq_sum/len-sum*sum);
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

size_t numerical_argmin_argmax_array(ndarray_obj_t *in, size_t start, 
                                       size_t stop, size_t stride, uint8_t op) {
    size_t best_idx = start;
    if(in->array->typecode == NDARRAY_UINT8) {
        ARG_MIN_LOOP(in, uint8_t, start, stop, stride, op);
    } else if(in->array->typecode == NDARRAY_INT8) {
        ARG_MIN_LOOP(in, int8_t, start, stop, stride, op);
    } else if(in->array->typecode == NDARRAY_UINT16) {
        ARG_MIN_LOOP(in, uint16_t, start, stop, stride, op);
    } else if(in->array->typecode == NDARRAY_INT16) {
        ARG_MIN_LOOP(in, uint16_t, start, stop, stride, op);
    } else if(in->array->typecode == NDARRAY_FLOAT) {
        ARG_MIN_LOOP(in, mp_float_t, start, stop, stride, op);
    }
    return best_idx;
}

void copy_value_into_ndarray(ndarray_obj_t *target, ndarray_obj_t *source, size_t target_idx, size_t source_idx) {
    // since we are simply copying, it doesn't matter, whether the arrays are signed or unsigned, 
    // we can cast them in any way we like
    // This could also be done with byte copies. I don't know, whether that would have any benefits
    if((target->array->typecode == NDARRAY_UINT8) || (target->array->typecode == NDARRAY_INT8)) {
        ((uint8_t *)target->array->items)[target_idx] = ((uint8_t *)source->array->items)[source_idx];
    } else if((target->array->typecode == NDARRAY_UINT16) || (target->array->typecode == NDARRAY_INT16)) {
        ((uint16_t *)target->array->items)[target_idx] = ((uint16_t *)source->array->items)[source_idx];
    } else { 
        ((float *)target->array->items)[target_idx] = ((float *)source->array->items)[source_idx];
    }
}
 
STATIC mp_obj_t numerical_argmin_argmax(mp_obj_t oin, mp_obj_t axis, uint8_t optype) {
    if(MP_OBJ_IS_TYPE(oin, &mp_type_tuple) || MP_OBJ_IS_TYPE(oin, &mp_type_list) || 
        MP_OBJ_IS_TYPE(oin, &mp_type_range)) {
        // This case will work for single iterables only 
        size_t idx = 0, best_idx = 0;
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable = mp_getiter(oin, &iter_buf);
        mp_obj_t best_obj = MP_OBJ_NULL;
        mp_obj_t item;
        mp_uint_t op = MP_BINARY_OP_LESS;
        if((optype == NUMERICAL_ARGMAX) || (optype == NUMERICAL_MAX)) op = MP_BINARY_OP_MORE;
        while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            if ((best_obj == MP_OBJ_NULL) || (mp_binary_op(op, item, best_obj) == mp_const_true)) {
                best_obj = item;
                best_idx = idx;
            }
            idx++;
        }
        if((optype == NUMERICAL_ARGMIN) || (optype == NUMERICAL_ARGMAX)) {
            return MP_OBJ_NEW_SMALL_INT(best_idx);
        } else {
            return best_obj;
        }
    } else if(mp_obj_is_type(oin, &ulab_ndarray_type)) {
            ndarray_obj_t *in = MP_OBJ_TO_PTR(oin);
            size_t best_idx;
            if((axis == mp_const_none) || (in->m == 1) || (in->n == 1)) {
                // return the value for the flattened array                
                best_idx = numerical_argmin_argmax_array(in, 0, in->array->len, 1, optype);
                if((optype == NUMERICAL_ARGMIN) || (optype == NUMERICAL_ARGMAX)) {
                    return MP_OBJ_NEW_SMALL_INT(best_idx);
                } else {
                    if(in->array->typecode == NDARRAY_FLOAT) {
                        return mp_obj_new_float(ndarray_get_float_value(in->array->items, in->array->typecode, best_idx));
                    } else {
                        return mp_binary_get_val_array(in->array->typecode, in->array->items, best_idx);
                    }
                }
            } else { // we have to work with a full matrix here
                uint8_t _axis = mp_obj_get_int(axis);
                size_t m = (_axis == 0) ? 1 : in->m;
                size_t n = (_axis == 0) ? in->n : 1;
                size_t len = in->array->len;
                ndarray_obj_t *ndarray = NULL;
                if((optype == NUMERICAL_MAX) || (optype == NUMERICAL_MIN)) {
                    ndarray = create_new_ndarray(m, n, in->array->typecode);
                } else { // argmin/argmax
                    // TODO: one might get away with uint8_t, if both m, and n < 255
                    ndarray = create_new_ndarray(m, n, NDARRAY_UINT16);
                }

                // TODO: these two cases could probably be combined in a more elegant fashion...
                if(_axis == 0) { // vertical
                    for(size_t i=0; i < n; i++) {
                        best_idx = numerical_argmin_argmax_array(in, i, len, n, optype);
                        if((optype == NUMERICAL_MIN) || (optype == NUMERICAL_MAX)) {
                            copy_value_into_ndarray(ndarray, in, i, best_idx);
                        } else {
                            ((uint16_t *)ndarray->array->items)[i] = (uint16_t)(best_idx / n);
                        }
                    }
                } else { // horizontal
                    for(size_t i=0; i < m; i++) {
                        best_idx = numerical_argmin_argmax_array(in, i*in->n, (i+1)*in->n, 1, optype);
                        if((optype == NUMERICAL_MIN) || (optype == NUMERICAL_MAX)) {
                             copy_value_into_ndarray(ndarray, in, i, best_idx);
                        } else {
                            ((uint16_t *)ndarray->array->items)[i] = (uint16_t)(best_idx - i*in->n);
                        }
                    }
                }
                return MP_OBJ_FROM_PTR(ndarray);
            }
            return mp_const_none;
        }
    mp_raise_TypeError("input type is not supported");
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
            case NUMERICAL_MAX:
            case NUMERICAL_ARGMAX:
                return numerical_argmin_argmax(oin, axis, type);
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
                return numerical_argmin_argmax(oin, axis, type);
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
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(2, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    mp_obj_t oin = args[0].u_obj;
    int16_t shift = mp_obj_get_int(args[1].u_obj);
    if((args[2].u_obj != mp_const_none) && 
           (mp_obj_get_int(args[2].u_obj) != 0) && 
           (mp_obj_get_int(args[2].u_obj) != 1)) {
        mp_raise_ValueError("axis must be None, 0, or 1");
    }

    ndarray_obj_t *in = MP_OBJ_TO_PTR(oin);
    uint8_t _sizeof = mp_binary_get_size('@', in->array->typecode, NULL);
    size_t len;
    int16_t _shift;
    uint8_t *array = (uint8_t *)in->array->items;
    // TODO: transpose the matrix, if axis == 0. Though, that is hard on the RAM...
    if(shift < 0) {
        _shift = -shift;
    } else {
        _shift = shift;
    }
    if((args[2].u_obj == mp_const_none) || (mp_obj_get_int(args[2].u_obj) == 1)) { // shift horizontally
        uint16_t M;
        if(args[2].u_obj == mp_const_none) {
            len = in->array->len;
            M = 1;
        } else {
            len = in->n;
            M = in->m;
        }
        _shift = _shift % len;
        if(shift < 0) _shift = len - _shift;
        // TODO: if(shift > len/2), we should move in the opposite direction. That would save RAM
        _shift *= _sizeof;
        uint8_t *tmp = m_new(uint8_t, _shift);
        for(size_t m=0; m < M; m++) {
            memmove(tmp, &array[m*len*_sizeof], _shift);
            memmove(&array[m*len*_sizeof], &array[m*len*_sizeof+_shift], len*_sizeof-_shift);
            memmove(&array[(m+1)*len*_sizeof-_shift], tmp, _shift);
        }
        m_del(uint8_t, tmp, _shift);
        return mp_const_none;
    } else {
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
                memmove(&_data[m*_sizeof], &array[(m*in->n+n)*_sizeof], _sizeof);
            }
            // now, the actual shift
            memmove(tmp, _data, _shift);
            memmove(_data, &_data[_shift], len*_sizeof-_shift);
            memmove(&_data[len*_sizeof-_shift], tmp, _shift);
            for(size_t m=0; m < len; m++) {
                // this loop should dump the content of the temporary buffer into data
                memmove(&array[(m*in->n+n)*_sizeof], &_data[m*_sizeof], _sizeof);
            }            
        }
        m_del(uint8_t, tmp, _shift);
        m_del(uint8_t, _data, _sizeof*len);
        return mp_const_none;
    }
}

mp_obj_t numerical_flip(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(1, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError("flip argument must be an ndarray");
    }
    if((args[1].u_obj != mp_const_none) && 
           (mp_obj_get_int(args[1].u_obj) != 0) && 
           (mp_obj_get_int(args[1].u_obj) != 1)) {
        mp_raise_ValueError("axis must be None, 0, or 1");
    }

    ndarray_obj_t *in = MP_OBJ_TO_PTR(args[0].u_obj);
    mp_obj_t oout = ndarray_copy(args[0].u_obj);
    ndarray_obj_t *out = MP_OBJ_TO_PTR(oout);
    uint8_t _sizeof = mp_binary_get_size('@', in->array->typecode, NULL);
    uint8_t *array_in = (uint8_t *)in->array->items;
    uint8_t *array_out = (uint8_t *)out->array->items;    
    size_t len;
    if((args[1].u_obj == mp_const_none) || (mp_obj_get_int(args[1].u_obj) == 1)) { // flip horizontally
        uint16_t M = in->m;
        len = in->n;
        if(args[1].u_obj == mp_const_none) { // flip flattened array
            len = in->array->len;
            M = 1;
        }
        for(size_t m=0; m < M; m++) {
            for(size_t n=0; n < len; n++) {
                memcpy(array_out+_sizeof*(m*len+n), array_in+_sizeof*((m+1)*len-n-1), _sizeof);
            }
        }
    } else { // flip vertically
        for(size_t m=0; m < in->m; m++) {
            for(size_t n=0; n < in->n; n++) {
                memcpy(array_out+_sizeof*(m*in->n+n), array_in+_sizeof*((in->m-m-1)*in->n+n), _sizeof);
            }
        }
    }
    return out;
}

mp_obj_t numerical_diff(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) } },
        { MP_QSTR_n, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1 } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(1, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError("diff argument must be an ndarray");
    }
    
    ndarray_obj_t *in = MP_OBJ_TO_PTR(args[0].u_obj);
    size_t increment, N, M;
    if((args[2].u_int == -1) || (args[2].u_int == 1)) { // differentiate along the horizontal axis
        increment = 1;
    } else if(args[2].u_int == 0) { // differtiate along vertical axis
        increment = in->n;
    } else {
        mp_raise_ValueError("axis must be -1, 0, or 1");        
    }
    if((args[1].u_int < 0) || (args[1].u_int > 9)) {
        mp_raise_ValueError("n must be between 0, and 9");
    }
    uint8_t n = args[1].u_int;
    int8_t *stencil = m_new(int8_t, n+1);
    stencil[0] = 1;
    for(uint8_t i=1; i < n+1; i++) {
        stencil[i] = -stencil[i-1]*(n-i+1)/i;
    }

    ndarray_obj_t *out;
    
    if(increment == 1) { // differentiate along the horizontal axis 
        if(n >= in->n) {
            out = create_new_ndarray(in->m, 0, in->array->typecode);
            m_del(uint8_t, stencil, n);
            return MP_OBJ_FROM_PTR(out);
        }
        N = in->n - n;
        M = in->m;
    } else { // differentiate along vertical axis
        if(n >= in->m) {
            out = create_new_ndarray(0, in->n, in->array->typecode);
            m_del(uint8_t, stencil, n);
            return MP_OBJ_FROM_PTR(out);
        }
        M = in->m - n;
        N = in->n;
    }
    out = create_new_ndarray(M, N, in->array->typecode);
    if(in->array->typecode == NDARRAY_UINT8) {
        CALCULATE_DIFF(in, out, uint8_t, M, N, in->n, increment);
    } else if(in->array->typecode == NDARRAY_INT8) {
        CALCULATE_DIFF(in, out, int8_t, M, N, in->n, increment);
    }  else if(in->array->typecode == NDARRAY_UINT16) {
        CALCULATE_DIFF(in, out, uint16_t, M, N, in->n, increment);
    } else if(in->array->typecode == NDARRAY_INT16) {
        CALCULATE_DIFF(in, out, int16_t, M, N, in->n, increment);
    } else {
        CALCULATE_DIFF(in, out, mp_float_t, M, N, in->n, increment);
    }
    m_del(int8_t, stencil, n);
    return MP_OBJ_FROM_PTR(out);
}

mp_obj_t numerical_sort_helper(mp_obj_t oin, mp_obj_t axis, uint8_t inplace) {
    if(!mp_obj_is_type(oin, &ulab_ndarray_type)) {
        mp_raise_TypeError("sort argument must be an ndarray");
    }

    ndarray_obj_t *ndarray;
    mp_obj_t out;
    if(inplace == 1) {
        ndarray = MP_OBJ_TO_PTR(oin);
    } else {
        out = ndarray_copy(oin);
        ndarray = MP_OBJ_TO_PTR(out);
    }
    size_t increment, start_inc, end, N;
    if(axis == mp_const_none) { // flatten the array
        ndarray->m = 1;
        ndarray->n = ndarray->array->len;
        increment = 1;
        start_inc = ndarray->n;
        end = ndarray->n;
        N = ndarray->n;
    } else if((mp_obj_get_int(axis) == -1) || 
              (mp_obj_get_int(axis) == 1)) { // sort along the horizontal axis
        increment = 1;
        start_inc = ndarray->n;
        end = ndarray->array->len;
        N = ndarray->n;
    } else if(mp_obj_get_int(axis) == 0) { // sort along vertical axis
        increment = ndarray->n;
        start_inc = 1;
        end = ndarray->m;
        N = ndarray->m;
    } else {
        mp_raise_ValueError("axis must be -1, 0, None, or 1");        
    }
    
    size_t q, k, p, c;

    for(size_t start=0; start < end; start+=start_inc) {
        q = N; 
        k = (q >> 1);
        if((ndarray->array->typecode == NDARRAY_UINT8) || (ndarray->array->typecode == NDARRAY_INT8)) {
            HEAPSORT(uint8_t, ndarray);
        } else if((ndarray->array->typecode == NDARRAY_INT16) || (ndarray->array->typecode == NDARRAY_INT16)) {
            HEAPSORT(uint16_t, ndarray);
        } else {
            HEAPSORT(mp_float_t, ndarray);
        }
    }
    if(inplace == 1) {
        return mp_const_none;
    } else {
        return out;
    }
}

// numpy function
mp_obj_t numerical_sort(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(1, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    return numerical_sort_helper(args[0].u_obj, args[1].u_obj, 0);
}
// method of an ndarray
mp_obj_t numerical_sort_inplace(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(1, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    return numerical_sort_helper(args[0].u_obj, args[1].u_obj, 1);
}

mp_obj_t numerical_argsort(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_int = -1 } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(1, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError("argsort argument must be an ndarray");
    }

    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0].u_obj);
    size_t increment, start_inc, end, N, m, n;
    if(args[1].u_obj == mp_const_none) { // flatten the array
        m = 1;
        n = ndarray->array->len;
        ndarray->m = m;
        ndarray->n = n;
        increment = 1;
        start_inc = ndarray->n;
        end = ndarray->n;
        N = n;
    } else if((mp_obj_get_int(args[1].u_obj) == -1) || 
              (mp_obj_get_int(args[1].u_obj) == 1)) { // sort along the horizontal axis
        m = ndarray->m;
        n = ndarray->n;
        increment = 1;
        start_inc = n;
        end = ndarray->array->len;
        N = n;
    } else if(mp_obj_get_int(args[1].u_obj) == 0) { // sort along vertical axis
        m = ndarray->m;
        n = ndarray->n;
        increment = n;
        start_inc = 1;
        end = m;
        N = m;
    } else {
        mp_raise_ValueError("axis must be -1, 0, None, or 1");
    }

    // at the expense of flash, we could save RAM by creating 
    // an NDARRAY_UINT16 ndarray only, if needed, otherwise, NDARRAY_UINT8
    ndarray_obj_t *indices = create_new_ndarray(m, n, NDARRAY_UINT16);
    uint16_t *index_array = (uint16_t *)indices->array->items;
    // initialise the index array
    // if array is flat: 0 to indices->n
    // if sorting vertically, identical indices are arranged row-wise
    // if sorting horizontally, identical indices are arranged colunn-wise
    for(uint16_t start=0; start < end; start+=start_inc) {
        for(uint16_t s=0; s < N; s++) {
            index_array[start+s*increment] = s;
        }
    }

    size_t q, k, p, c;
    for(size_t start=0; start < end; start+=start_inc) {
        q = N; 
        k = (q >> 1);
        if((ndarray->array->typecode == NDARRAY_UINT8) || (ndarray->array->typecode == NDARRAY_INT8)) {
            HEAP_ARGSORT(uint8_t, ndarray, index_array);
        } else if((ndarray->array->typecode == NDARRAY_INT16) || (ndarray->array->typecode == NDARRAY_INT16)) {
            HEAP_ARGSORT(uint16_t, ndarray, index_array);
        } else {
            HEAP_ARGSORT(mp_float_t, ndarray, index_array);
        }
    }
    return MP_OBJ_FROM_PTR(indices);
}

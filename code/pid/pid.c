/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Zoltán Vörös
*/


#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/runtime.h"
#include "py/misc.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/objtuple.h"


#include "pid.h"

#if ULAB_HAS_PID_MODULE

// methods of the PID object
static const mp_rom_map_elem_t ulab_pid_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_bitdepth), MP_ROM_PTR(&pid_pid_bitdepth_obj) },
    { MP_ROM_QSTR(MP_QSTR_offset), MP_ROM_PTR(&pid_pid_offset_obj) },
    { MP_ROM_QSTR(MP_QSTR_evaluate), MP_ROM_PTR(&pid_pid_evaluate_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&pid_pid_reset_obj) },
    // { MP_ROM_QSTR(MP_QSTR_buffer), MP_ROM_PTR(&pid_pid_buffer_obj) },
    { MP_ROM_QSTR(MP_QSTR_series), MP_ROM_PTR(&pid_pid_series_obj) },
    { MP_ROM_QSTR(MP_QSTR_parameters), MP_ROM_PTR(&pid_pid_parameters_obj) },
    { MP_ROM_QSTR(MP_QSTR_float_step), MP_ROM_PTR(&pid_pid_float_step_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_point), MP_ROM_PTR(&pid_pid_set_point_obj) },
    { MP_ROM_QSTR(MP_QSTR_step), MP_ROM_PTR(&pid_pid_step_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&pid_pid_value_obj) },
};

static MP_DEFINE_CONST_DICT(ulab_pid_locals_dict, ulab_pid_locals_dict_table);

#if defined(MP_DEFINE_CONST_OBJ_TYPE)
MP_DEFINE_CONST_OBJ_TYPE(
    ulab_pid_type,
    MP_QSTR_PID,
    MP_TYPE_FLAG_NONE,
    print, pid_pid_print,
    make_new, pid_pid_make_new, 
    locals_dict, &ulab_pid_locals_dict
);
#else
const mp_obj_type_t ulab_pid_type = {
    { &mp_type_type },
    .name = MP_QSTR_PID,
    .print = pid_pid_print,
    .make_new = pid_pid_make_new,
    .locals_dict = (mp_obj_dict_t*)&pid_locals_dict
};
#endif

void pid_pid_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    pid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(MP_PYTHON_PRINTER, "PID object at 0x%p", self);
    mp_printf(MP_PYTHON_PRINTER, "\nvalue: %f", self->value);
    mp_printf(MP_PYTHON_PRINTER, "\nlast time: %f", self->last_time);
    mp_printf(MP_PYTHON_PRINTER, "\nsteps: %lu", self->steps);
}

mp_obj_t pid_pid_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void) type;
    mp_arg_check_num(n_args, n_kw, 0, 2, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);

    pid_obj_t *self = m_new_obj(pid_obj_t);
    self->base.type = &ulab_pid_type;

    self->set_point = MICROPY_FLOAT_CONST(0.0);

    // initialise the converter object with default values, i.e, expand around 0, to first order in the Taylor series
    pid_data_converter_t in, out;
    in.series.len = 2;
    in.series.x0 = MICROPY_FLOAT_CONST(0.0);
    in.series.coeffs = m_new0(mp_float_t, 2);
    in.series.coeffs[0] = MICROPY_FLOAT_CONST(1.0);
    self->in = in;

    out.series.len = 2;
    out.series.x0 = MICROPY_FLOAT_CONST(0.0);
    out.series.coeffs = m_new0(mp_float_t, 2);
    out.series.coeffs[0] = MICROPY_FLOAT_CONST(1.0);
    self->out = out;

    self->P = MICROPY_FLOAT_CONST(0.0);
    self->I = MICROPY_FLOAT_CONST(0.0);
    self->D = MICROPY_FLOAT_CONST(0.0);
    self->integral = MICROPY_FLOAT_CONST(0.0);
    self->value = MICROPY_FLOAT_CONST(0.0);
    self->error = MICROPY_FLOAT_CONST(0.0);
    self->last_time = MICROPY_FLOAT_CONST(0.0);
    self->output = MICROPY_FLOAT_CONST(0.0);
    self->steps = 0;
    return self;
}

mp_obj_t pid_pid_bitdepth(size_t n_args, const mp_obj_t *args) {
    // set/get resolution of ADC/DAC and mask
    pid_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    GET_STR_DATA_LEN(args[1], which, length);

    pid_buffer_t *buffer;

    if(memcmp(which, "in", 2) == 0) {
        buffer = &(self->in.buffer);
    } else if(memcmp(which, "out", 3) == 0) {
        buffer = &(self->out.buffer);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("first argument must be 'in' or 'out'"));
    }
    if(n_args == 2) { // this is a getter
        return MP_OBJ_NEW_SMALL_INT(buffer->bitdepth);
    }
    
    if(n_args == 3) { // this is a setter
        uint8_t bitdepth = (uint8_t)MP_OBJ_SMALL_INT_VALUE(args[2]);
        buffer->bitdepth = bitdepth;
        buffer->mask = (1 << bitdepth) - 1;
        buffer->nbytes = (bitdepth + 7) / 8;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pid_pid_bitdepth_obj, 2, 3, pid_pid_bitdepth);

mp_obj_t pid_pid_offset(size_t n_args, const mp_obj_t *args) {
    // set/get resolution of ADC/DAC and mask
    pid_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    GET_STR_DATA_LEN(args[1], which, length);

    pid_buffer_t *buffer;

    if(memcmp(which, "in", 2) == 0) {
        buffer = &(self->in.buffer);
    } else if(memcmp(which, "out", 3) == 0) {
        buffer = &(self->out.buffer);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("first argument must be 'in' or 'out'"));
    }
    if(n_args == 2) { // this is a getter
        return MP_OBJ_NEW_SMALL_INT(buffer->offset);
    }
    
    if(n_args == 3) { // this is a setter
        buffer->offset = (uint8_t)MP_OBJ_SMALL_INT_VALUE(args[2]);
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pid_pid_offset_obj, 2, 3, pid_pid_offset);


static mp_float_t pid_pid_convert(pid_series_t series, mp_float_t x) {

    mp_float_t dx = x - series.x0;
    mp_float_t *coeffs = (mp_float_t *)series.coeffs;
    mp_float_t y = *coeffs++;

    for(uint8_t i = 0; i < series.len - 1; i++) {
        y *= dx;
        y += *coeffs++;
    }
    return y;
}

mp_obj_t pid_pid_evaluate(mp_obj_t _self, mp_obj_t io, mp_obj_t x) {
    // set/get coefficients and positions of Taylor series expansion 
    pid_obj_t *self = MP_OBJ_TO_PTR(_self);
    GET_STR_DATA_LEN(io, which, length);

    if(memcmp(which, "in", 2) == 0) {
        return mp_obj_new_float(pid_pid_convert(self->in.series, mp_obj_get_float(x)));
    } else if(memcmp(which, "out", 3) == 0) {
        return mp_obj_new_float(pid_pid_convert(self->out.series, mp_obj_get_float(x)));
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("first argument must be 'in' or 'out'"));
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_3(pid_pid_evaluate_obj, pid_pid_evaluate);

mp_obj_t pid_pid_parameters(size_t n_args, const mp_obj_t *args) {
    // set/get PID parameters 
    pid_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    GET_STR_DATA_LEN(args[1], which, length);

    mp_float_t *param;

    if(memcmp(which, "P", 1) == 0) {
        param = &(self->P);
    } else if(memcmp(which, "I", 1) == 0) {
        param = &(self->I);
    } else if(memcmp(which, "D", 1) == 0) {
        param = &(self->D);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("first argument must be 'P', 'I', or 'D'"));
    }

    if(n_args == 2) { // this is a getter
        return mp_obj_new_float(*param);
    } else { // this is a setter
        *param = mp_obj_get_float(args[2]);
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pid_pid_parameters_obj, 2, 3, pid_pid_parameters);


mp_obj_t pid_pid_set_point(size_t n_args, const mp_obj_t *args) {
    // set/get PID parameters 
    pid_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if(n_args == 1) { // this is a getter
        return mp_obj_new_float(self->set_point);
    } else { // this is a setter
        self->set_point = mp_obj_get_float(args[1]);
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pid_pid_set_point_obj, 1, 2, pid_pid_set_point);


mp_obj_t pid_pid_reset(mp_obj_t _self) {
    // resets only the PID values but not the parameters
    pid_obj_t *self = MP_OBJ_TO_PTR(_self);
    self->integral = MICROPY_FLOAT_CONST(0.0);
    self->value = MICROPY_FLOAT_CONST(0.0);
    self->last_time = MICROPY_FLOAT_CONST(0.0);
    self->steps = 0;

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pid_pid_reset_obj, pid_pid_reset);

mp_obj_t pid_pid_series(size_t n_args, const mp_obj_t *args) {
    // set/get coefficients and point of Taylor series expansion 
    pid_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    GET_STR_DATA_LEN(args[1], which, length);

    pid_series_t *series;

    if(memcmp(which, "in", 2) == 0) {
        series = &(self->in.series);
    } else if(memcmp(which, "out", 3) == 0) {
        series = &(self->out.series);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("first argument must be 'in' or 'out'"));
    }

    mp_float_t *coeffs = (mp_float_t *)series->coeffs;

    if(n_args == 2) { // this is a getter
        mp_obj_t *citems = m_new(mp_obj_t, series->len);
        for(uint8_t i = 0; i < series->len; i++) {
            citems[i] = mp_obj_new_float(*coeffs++);
        }
        mp_obj_t ctuple = mp_obj_new_tuple(series->len, citems);        
        m_del(mp_obj_t, citems, series->len);

        mp_obj_t *items = m_new(mp_obj_t, 2);
        items[0] = mp_obj_new_float(series->x0);
        items[1] = ctuple;
        mp_obj_t tuple = mp_obj_new_tuple(2, items);        
        m_del(mp_obj_t, items, 2);

        return tuple;
    } else if(n_args == 4) { // this is a setter
        series->x0 = mp_obj_get_float(args[2]);

        m_del(mp_float_t, coeffs, series->len);
        uint8_t len = (uint8_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(args[3]));
        coeffs = m_new0(mp_float_t, len);
        series->len = len;
        series->coeffs = coeffs;

        mp_obj_iter_buf_t buf;
        mp_obj_t item, iterable = mp_getiter(args[3], &buf);
        while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            *coeffs++ = (mp_float_t)mp_obj_get_float(item);
        }
    } else { // n_args == 3
        mp_raise_ValueError(MP_ERROR_TEXT("wrong number of arguments"));
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pid_pid_series_obj, 2, 4, pid_pid_series);


/*
mp_obj_t pid_pid_set_buffer(mp_obj_t _self, mp_obj_t _bytes, mp_obj_t _offset) {
    pid_obj_t *self = MP_OBJ_TO_PTR(_self);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_3(pid_pid_set_buffer_obj, pid_pid_set_buffer);
*/

void pid_pid_loop(pid_obj_t *self, mp_float_t value, mp_float_t t) {
    // This is the standard PID loop, stripped of everything

    mp_float_t dt = t - self->last_time;
    self->last_time = t;

    mp_float_t error = value - self->set_point;
    
    mp_float_t diff = (error - self->error) / dt;    
    self->integral += error * dt;
    self->output = self->P * error + self->I * self->integral + self->D * diff;
    
    self->error = error;
    self->value = value;
    self->steps++;
}

mp_obj_t pid_pid_step(mp_obj_t _self) {
    // The complete PID loop, converting the value from the buffer, 
    // and converting the results to the buffer
    pid_obj_t *self = MP_OBJ_TO_PTR(_self);

    // convert value in input buffer
    // assume that all bytes in the buffer are significant bytes (i.e., there are no auxiliary bits)
    uint32_t input = 0; // 32 bits should suffice for most ADCs
    uint8_t *input_bytes = (uint8_t *)&input;
    uint8_t *buffer_bytes = (uint8_t *)self->in.buffer.bytes;
    input_bytes += 4;
    buffer_bytes += self->in.buffer.len;
    for(uint8_t i = 0; i < self->in.buffer.len; i++) {
        *(--input_bytes) = *(--buffer_bytes);
    }
    mp_float_t x = (mp_float_t)(input & self->in.buffer.mask);
    mp_float_t value = pid_pid_convert(self->in.series, x);

    // TODO: get system time here!
    mp_float_t t = self->last_time + 1.0;
    pid_pid_loop(self, value, t);
    
    // convert value in output buffer
    value = pid_pid_convert(self->out.series, self->output);
    // ensure that output is within limits
    
    if(self->out.buffer.bitdepth != 0) {
        value = (value < 0 ? 0 : value);
        value = (value > self->max ? self->max: value); 
    }

    uint32_t output = ((uint32_t)value) & self->out.buffer.mask;
    uint8_t *output_bytes = (uint8_t *)&output;
    buffer_bytes = (uint8_t *)self->out.buffer.bytes;
    output_bytes += 4;
    buffer_bytes += self->out.buffer.len;
    for(uint8_t i = 0; i < self->out.buffer.len; i++) {
        *(--buffer_bytes) = *(--output_bytes);
    }

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pid_pid_step_obj, pid_pid_step);

mp_obj_t pid_pid_float_step(size_t n_args, const mp_obj_t *args) {
    // The simplified PID loop, working with floating points numbers directly
    pid_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    mp_float_t t = self->last_time + 1.0;
    if(n_args == 3) {
        t = mp_obj_get_float(args[2]);
    }
    mp_float_t value = mp_obj_get_float(args[1]);

    pid_pid_loop(self, value, t);

    return mp_obj_new_float(self->output);
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pid_pid_float_step_obj, 2, 3, pid_pid_float_step);


mp_obj_t pid_pid_value(mp_obj_t _self) {
    pid_obj_t *self = MP_OBJ_TO_PTR(_self);
    return mp_obj_new_float(self->value);
}

MP_DEFINE_CONST_FUN_OBJ_1(pid_pid_value_obj, pid_pid_value);

static const mp_rom_map_elem_t ulab_pid_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_PID) },
    { MP_ROM_QSTR(MP_QSTR_PID), MP_ROM_PTR(&ulab_pid_type) },
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_pid_globals, ulab_pid_globals_table);

const mp_obj_module_t ulab_pid_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_pid_globals,
};

#endif /* ULAB_HAS_PID_MODULE */

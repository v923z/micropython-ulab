/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Zoltán Vörös
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

// methods of the PID buffer object
static const mp_rom_map_elem_t ulab_pid_buffer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_evaluate), MP_ROM_PTR(&pid_buffer_evaluate_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pid_buffer_init_obj) },
};

static MP_DEFINE_CONST_DICT(ulab_pid_buffer_locals_dict, ulab_pid_buffer_locals_dict_table);

// attributes of the PID buffer object
static void pid_buffer_attributes(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if(dest[0] == MP_OBJ_NULL) {
        switch(attr) {
            case MP_QSTR_order:
                dest[0] = pid_buffer_getter(self_in, PID_BUFFER_ORDER);
                break;
            case MP_QSTR_x0:
                dest[0] = pid_buffer_getter(self_in, PID_BUFFER_X0);
                break;
            case MP_QSTR_coeffs:
                dest[0] = pid_buffer_getter(self_in, PID_BUFFER_COEFFS);
                break;
            case MP_QSTR_offset:
                dest[0] = pid_buffer_getter(self_in, PID_BUFFER_OFFSET);
                break;
            case MP_QSTR_bitdepth:
                dest[0] = pid_buffer_getter(self_in, PID_BUFFER_BITDEPTH);
                break;
            case MP_QSTR_mask:
                dest[0] = pid_buffer_getter(self_in, PID_BUFFER_MASK);
                break;
            case MP_QSTR_bytes:
                dest[0] = pid_buffer_getter(self_in, PID_BUFFER_BYTES);
                break;
            default:
                // forward to locals dict
                dest[1] = MP_OBJ_SENTINEL;
                break;
        }
    } else {
        if(dest[1]) {
            switch(attr) {
                case MP_QSTR_x0:
                    pid_buffer_setter(self_in, dest[1], PID_BUFFER_X0);
                    break;
                case MP_QSTR_coeffs:
                    pid_buffer_setter(self_in, dest[1], PID_BUFFER_COEFFS);
                    break;
                case MP_QSTR_offset:
                    pid_buffer_setter(self_in, dest[1], PID_BUFFER_OFFSET);
                    break;
                case MP_QSTR_bitdepth:
                    pid_buffer_setter(self_in, dest[1], PID_BUFFER_BITDEPTH);
                    break;
                case MP_QSTR_bytes:
                    pid_buffer_setter(self_in, dest[1], PID_BUFFER_BYTES);
                    break;
                default:
                    return;
                    break;
            }
            dest[0] = MP_OBJ_NULL;
        }
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    ulab_pid_buffer_type,
    MP_QSTR_buffer,
    MP_TYPE_FLAG_NONE,
    print, pid_buffer_print,
    make_new, pid_buffer_make_new,
    locals_dict, &ulab_pid_buffer_locals_dict,
    attr, pid_buffer_attributes
);

static pid_buffer_obj_t *pid_buffer_create(void) {
    pid_buffer_obj_t *buffer = m_new_obj(pid_buffer_obj_t);
    buffer->base.type = &ulab_pid_buffer_type;    
    return buffer;
}

mp_obj_t pid_buffer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void) type;
    mp_arg_check_num(n_args, n_kw, 0, 2, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    return MP_OBJ_FROM_PTR(pid_buffer_create());
}

void pid_buffer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    pid_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(MP_PYTHON_PRINTER, "PID buffer object at 0x%p", self);
}

mp_obj_t pid_buffer_getter(mp_obj_t self_in, uint8_t attribute) {
    pid_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if(attribute == PID_BUFFER_ORDER) {
        return MP_OBJ_NEW_SMALL_INT(self->series.order);
    } else if(attribute == PID_BUFFER_X0) {
        return mp_obj_new_float(self->series.x0);
    } else if(attribute == PID_BUFFER_COEFFS) {
        mp_float_t *coeffs = (mp_float_t *)self->series.coeffs;
        mp_obj_t *items = m_new(mp_obj_t, self->series.order);
        for(uint8_t i = 0; i < self->series.order; i++) {
            items[i] = mp_obj_new_float(*coeffs++);
        }
        mp_obj_t tuple = mp_obj_new_tuple(self->series.order, items);
        return tuple;
    } else if(attribute == PID_BUFFER_OFFSET) {
        return MP_OBJ_NEW_SMALL_INT(self->converter.offset);
    } else if(attribute == PID_BUFFER_BITDEPTH) {
        return MP_OBJ_NEW_SMALL_INT(self->converter.bitdepth);
    } else if(attribute == PID_BUFFER_MASK) {
        return MP_OBJ_NEW_SMALL_INT(self->converter.mask);
    } else if(attribute == PID_BUFFER_BYTES) {
        return mp_obj_new_bytearray_by_ref(self->converter.len, self->converter.bytes);
    }
    return mp_const_none;
}

mp_obj_t pid_buffer_setter(mp_obj_t self_in, mp_obj_t value, uint8_t attribute) {
    pid_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if(attribute == PID_BUFFER_X0) {
            self->series.x0 = mp_obj_get_float(value);
    } else if(attribute == PID_BUFFER_COEFFS) {
        mp_float_t *coeffs = (mp_float_t *)self->series.coeffs;
        m_del(mp_float_t, coeffs, self->series.order);
        uint8_t order = (uint8_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(value));
        coeffs = m_new0(mp_float_t, order);
        self->series.order = order;
        self->series.coeffs = coeffs;

        mp_obj_iter_buf_t buf;
        mp_obj_t item, iterable = mp_getiter(value, &buf);
        while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            *coeffs++ = (mp_float_t)mp_obj_get_float(item);
        }
    } else if(attribute == PID_BUFFER_OFFSET) {
        self->converter.offset = (uint8_t)MP_OBJ_SMALL_INT_VALUE(value);
    } else if(attribute == PID_BUFFER_BITDEPTH) {
        uint8_t bitdepth = (uint8_t)MP_OBJ_SMALL_INT_VALUE(value);
        self->converter.bitdepth = bitdepth;
        // TODO: we might have to consider the offset here!!!
        self->converter.mask = (1 << bitdepth) - 1;
        self->converter.nbytes = (bitdepth + 7) / 8;
    } else if(attribute == PID_BUFFER_BYTES) {
        mp_buffer_info_t bufinfo;
        if(mp_get_buffer(value, &bufinfo, MP_BUFFER_READ)) {
            if(bufinfo.len < (self->converter.offset + self->converter.bitdepth) / 8) {
                mp_raise_ValueError(MP_ERROR_TEXT("buffer is shorter than offset plus bitdepth"));
            }
        }
        self->converter.len = bufinfo.len;
        self->converter.bytes = bufinfo.buf;
    }
    return mp_const_none;
}

static mp_float_t pid_pid_convert(pid_series_t series, mp_float_t x) {
    // taking the coefficients of the Taylor expansion, returns the 
    // approximate value of a function at the value x
    mp_float_t dx = x - series.x0;
    mp_float_t *coeffs = (mp_float_t *)series.coeffs;
    mp_float_t y = *coeffs++;

    for(uint8_t i = 0; i < series.order - 1; i++) {
        y *= dx;
        y += *coeffs++;
    }
    return y;
}

static mp_obj_t pid_buffer_evaluate(mp_obj_t self_in, mp_obj_t x) {
    // convenience function for manually checking the conversion
    pid_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(pid_pid_convert(self->series, mp_obj_get_float(x)));
}

MP_DEFINE_CONST_FUN_OBJ_2(pid_buffer_evaluate_obj, pid_buffer_evaluate);

static mp_obj_t pid_buffer_init(mp_obj_t self_in) {
    // initialise buffer with some default values
    pid_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    
    self->series.order = 2;
    self->series.x0 = MICROPY_FLOAT_CONST(0.0);
    self->series.coeffs = m_new0(mp_float_t, 2);
    self->series.coeffs[0] = MICROPY_FLOAT_CONST(1.0);

    self->converter.len = 2;
    self->converter.bitdepth = 16;
    self->converter.mask = 0xffff;
    self->converter.offset = 0;
    self->converter.bytes = (uint8_t *)&self->converter.mask;
    self->converter.nbytes = 2;
    return mp_const_none;
} 

MP_DEFINE_CONST_FUN_OBJ_1(pid_buffer_init_obj, pid_buffer_init);



// methods of the PID object
static const mp_rom_map_elem_t ulab_pid_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&pid_pid_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_float_step), MP_ROM_PTR(&pid_pid_float_step_obj) },
    { MP_ROM_QSTR(MP_QSTR_step), MP_ROM_PTR(&pid_pid_step_obj) },
};

static MP_DEFINE_CONST_DICT(ulab_pid_locals_dict, ulab_pid_locals_dict_table);

// attributes of the PID object; the input/output buffers are defer to till later
static void pid_pid_attributes(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL) {
        switch(attr) {
            case MP_QSTR_P:
                dest[0] = pid_pid_parameters(self_in, MP_OBJ_NULL, PID_PARAMETER_P);
                break;
            case MP_QSTR_I:
                dest[0] = pid_pid_parameters(self_in, MP_OBJ_NULL, PID_PARAMETER_I);
                break;
            case MP_QSTR_D:
                dest[0] = pid_pid_parameters(self_in, MP_OBJ_NULL, PID_PARAMETER_D);
                break;
            case MP_QSTR_setpoint:
                dest[0] = pid_pid_parameters(self_in, MP_OBJ_NULL, PID_SETPOINT);
                break;
            case MP_QSTR_steps:
                dest[0] = pid_pid_parameters(self_in, MP_OBJ_NULL, PID_STEPS);
                break;
            case MP_QSTR_value:
                dest[0] = pid_pid_parameters(self_in, MP_OBJ_NULL, PID_VALUE);
                break;
            case MP_QSTR_out:
                dest[0] = pid_pid_parameters(self_in, MP_OBJ_NULL, PID_OUT);
                break;
            case MP_QSTR_input:
                dest[0] = pid_return_buffer(self_in, MP_OBJ_NULL, PID_BUFFER_INPUT);
                break;
            case MP_QSTR_output:
                dest[0] = pid_return_buffer(self_in, MP_OBJ_NULL, PID_BUFFER_OUTPUT);
                break;
            default:
                // forward to locals dict
                dest[1] = MP_OBJ_SENTINEL;
                break;
        }
    } else {
        if(dest[1]) {
            switch(attr) {
                case MP_QSTR_P:
                    pid_pid_parameters(self_in, dest[1], PID_PARAMETER_P);
                    break;
                case MP_QSTR_I:
                    pid_pid_parameters(self_in, dest[1], PID_PARAMETER_I);
                    break;
                case MP_QSTR_D:
                    pid_pid_parameters(self_in, dest[1], PID_PARAMETER_D);
                    break;
                case MP_QSTR_setpoint:
                    pid_pid_parameters(self_in, dest[1], PID_SETPOINT);
                    break;
                default:
                    return;
                    break;
            }
            dest[0] = MP_OBJ_NULL;
        }
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    ulab_pid_type,
    MP_QSTR_PID,
    MP_TYPE_FLAG_NONE,
    print, pid_pid_print,
    make_new, pid_pid_make_new, 
    locals_dict, &ulab_pid_locals_dict,
    attr, pid_pid_attributes
);

void pid_pid_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    pid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(MP_PYTHON_PRINTER, "PID object at 0x%p", self);
}

mp_obj_t pid_pid_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void) type;
    mp_arg_check_num(n_args, n_kw, 0, 2, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);

    pid_obj_t *self = m_new_obj(pid_obj_t);
    self->base.type = &ulab_pid_type;

    self->setpoint = MICROPY_FLOAT_CONST(0.0);
    self->input = pid_buffer_create();
    self->output = pid_buffer_create();

    self->P = MICROPY_FLOAT_CONST(0.0);
    self->I = MICROPY_FLOAT_CONST(0.0);
    self->D = MICROPY_FLOAT_CONST(0.0);
    self->integral = MICROPY_FLOAT_CONST(0.0);
    self->value = MICROPY_FLOAT_CONST(0.0);
    self->error = MICROPY_FLOAT_CONST(0.0);
    self->last_time = MICROPY_FLOAT_CONST(0.0);
    self->out = MICROPY_FLOAT_CONST(0.0);
    self->steps = 0;
    return self;
}

mp_obj_t pid_return_buffer(mp_obj_t self_in, mp_obj_t value, uint8_t designator) {
    pid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if(designator == PID_BUFFER_INPUT) {
        return self->input;
    } else {
        return self->output;
    }
}

mp_obj_t pid_pid_parameters(mp_obj_t self_in, mp_obj_t value, uint8_t attribute) {
    pid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_float_t *parameter;

    if(attribute == PID_STEPS) {
        if(value == MP_OBJ_NULL) {
            return MP_OBJ_NEW_SMALL_INT(self->steps);
        } // internal variable, no setter for this
    } {
        if(attribute == PID_PARAMETER_P) {
            parameter = &(self->P);
        } else if(attribute == PID_PARAMETER_I) {
            parameter = &(self->I);
        } else if(attribute == PID_PARAMETER_D) {
            parameter = &(self->D);
        } else if(attribute == PID_SETPOINT) {
            parameter = &(self->setpoint);    
        } else if(attribute == PID_OUT) {
            parameter = &(self->out);
        }
        else {
            parameter = &(self->value);
        }

        if(value == MP_OBJ_NULL) {
            return mp_obj_new_float(*parameter);
        } else {
            *parameter = mp_obj_get_float(value);
        }
    }
    return mp_const_none;
}

mp_obj_t pid_pid_reset(mp_obj_t _self) {
    // resets only the PID values but not the parameters
    pid_obj_t *self = MP_OBJ_TO_PTR(_self);
    self->integral = MICROPY_FLOAT_CONST(0.0);
    self->value = MICROPY_FLOAT_CONST(0.0);
    self->last_time = MICROPY_FLOAT_CONST(0.0);
    self->steps = 0;
    self->out = MICROPY_FLOAT_CONST(0.0);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pid_pid_reset_obj, pid_pid_reset);

void pid_pid_loop(pid_obj_t *self, mp_float_t value, mp_float_t t) {
    // This is the standard PID loop, stripped of everything

    mp_float_t dt = t - self->last_time;
    self->last_time = t;

    mp_float_t error = value - self->setpoint;
    
    mp_float_t diff = (error - self->error) / dt;    
    self->integral += error * dt;
    self->out = self->P * error + self->I * self->integral + self->D * diff;
    
    self->error = error;
    self->value = value;
    self->steps++;
}

mp_obj_t pid_pid_step(mp_obj_t self_in) {
    // The complete PID loop, converting the value from the input buffer, 
    // using the value in the PID loop, and then converting the results to the output buffer
    pid_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // // convert value in input buffer
    // // assume for now that all bytes in the buffer are significant bytes (i.e., there are no auxiliary bytes)
    uint32_t input = 0; // 32 bits should suffice for most ADCs
    uint8_t *input_bytes = (uint8_t *)&input;
    uint8_t *buffer_bytes = (uint8_t *)self->input->converter.bytes;
    input_bytes += 4;
    buffer_bytes += self->input->converter.len;
    for(uint8_t i = 0; i < self->input->converter.len; i++) {
        *(--input_bytes) = *(--buffer_bytes);
    }
    mp_float_t x = (mp_float_t)(input & self->input->converter.mask);
    mp_float_t value = pid_pid_convert(self->input->series, x);

    // TODO: get system time here!
    mp_float_t t = self->last_time + 1.0;
    pid_pid_loop(self, value, t);

    // convert value in output buffer
    value = pid_pid_convert(self->output->series, self->out);
    
    // ensure that output is within limits
    if(self->output->converter.bitdepth != 0) {
        value = (value < 0 ? 0 : value);
        value = (value > (mp_float_t)self->output->converter.mask ? (mp_float_t)self->output->converter.mask : value); 
    }

    uint32_t output = ((uint32_t)value) & self->output->converter.mask;
    uint8_t *output_bytes = (uint8_t *)&output;
    buffer_bytes = (uint8_t *)self->output->converter.bytes;
    output_bytes += 4;
    buffer_bytes += self->output->converter.len;
    for(uint8_t i = 0; i < self->output->converter.len; i++) {
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

    return mp_obj_new_float(self->out);
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pid_pid_float_step_obj, 2, 3, pid_pid_float_step);

static const mp_rom_map_elem_t ulab_pid_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_PID) },
    { MP_ROM_QSTR(MP_QSTR_PID), MP_ROM_PTR(&ulab_pid_type) },
    { MP_ROM_QSTR(MP_QSTR_buffer), MP_ROM_PTR(&ulab_pid_buffer_type) },
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_pid_globals, ulab_pid_globals_table);

const mp_obj_module_t ulab_pid_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_pid_globals,
};

#endif /* ULAB_HAS_PID_MODULE */

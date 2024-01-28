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
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"


#include "pid.h"

#if ULAB_HAS_PID_MODULE

// methods of the PID object
static const mp_rom_map_elem_t ulab_pid_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&pid_pid_reset_obj) },
    // { MP_ROM_QSTR(MP_QSTR_set_buffer), MP_ROM_PTR(&pid_pid_set_buffer_obj) },
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
    mp_printf(MP_PYTHON_PRINTER, "\nin buffer at 0x%p, out buffer at 0x%p", self->in.buffer, self->out.buffer);
    mp_printf(MP_PYTHON_PRINTER, "\n(P: %f, I: %f, D: %f)", self->P, self->I, self->D);
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
    self->P = MICROPY_FLOAT_CONST(0.0);
    self->I = MICROPY_FLOAT_CONST(0.0);
    self->D = MICROPY_FLOAT_CONST(0.0);
    self->integral = MICROPY_FLOAT_CONST(0.0);
    self->value = MICROPY_FLOAT_CONST(0.0);
    self->last_time = MICROPY_FLOAT_CONST(0.0);
    self->steps = 0;
    return self;
}

mp_obj_t pid_pid_reset(mp_obj_t _self) {
    pid_obj_t *self = MP_OBJ_TO_PTR(_self);
    self->integral = MICROPY_FLOAT_CONST(0.0);
    self->value = MICROPY_FLOAT_CONST(0.0);
    self->last_time = MICROPY_FLOAT_CONST(0.0);
    self->steps = 0;

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pid_pid_reset_obj, pid_pid_reset);

/*
mp_obj_t pid_pid_set_buffer(mp_obj_t _self, mp_obj_t _bytes, mp_obj_t _offset) {
    pid_obj_t *self = MP_OBJ_TO_PTR(_self);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_3(pid_pid_set_buffer_obj, pid_pid_set_buffer);
*/

mp_obj_t pid_pid_step(mp_obj_t _self) {
    pid_obj_t *self = MP_OBJ_TO_PTR(_self);
    self->steps++;
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pid_pid_step_obj, pid_pid_step);

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



// #if CIRCUITPY_ULAB
// #if !defined(MICROPY_VERSION) || MICROPY_VERSION <= 70144
// MP_REGISTER_MODULE(MP_QSTR_ulab_dot_pid, ulab_pid_module, MODULE_ULAB_ENABLED);
// #else
// MP_REGISTER_MODULE(MP_QSTR_ulab_dot_pid, ulab_pid_module);
// #endif
// #endif

#endif /* ULAB_HAS_PID_MODULE */

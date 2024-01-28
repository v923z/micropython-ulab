/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Zoltán Vörös
*/

#ifndef _PID_
#define _PID_

#include "../ulab.h"

extern const mp_obj_module_t ulab_pid_module;

extern const mp_obj_type_t ulab_pid_type;

enum PID_BUFFER_TYPE {
    PID_UINT8 = 'B',
    PID_INT8 = 'b',
    PID_UINT16 = 'H',
    PID_INT16 = 'h',
    PID_UINT32 = 'I',
    PID_INT32 = 'i',
};

typedef struct _pid_buffer_t {
    uint8_t len;
    uint8_t dtype;
    uint8_t offset;
    uint8_t *buffer;
} pid_buffer_t;

typedef struct _pid_polynomial_t {
    uint8_t len;
    mp_float_t *coeffs;
} pid_polynomial_t;


typedef struct _pid_obj_t {
    mp_obj_base_t base;
    pid_buffer_t in;
    pid_buffer_t out;
    pid_polynomial_t in_convert;
    pid_polynomial_t out_convert;
    mp_float_t P;
    mp_float_t I;
    mp_float_t D;
    mp_float_t integral;
    mp_float_t value;
    mp_float_t last_time;
    uint64_t steps;
} pid_obj_t;

mp_obj_t pid_pid_make_new(const mp_obj_type_t *, size_t , size_t , const mp_obj_t *);
void pid_pid_print(const mp_print_t *, mp_obj_t , mp_print_kind_t );


MP_DECLARE_CONST_FUN_OBJ_1(pid_pid_reset_obj);
MP_DECLARE_CONST_FUN_OBJ_3(pid_pid_set_buffer_obj);
MP_DECLARE_CONST_FUN_OBJ_1(pid_pid_step_obj);
MP_DECLARE_CONST_FUN_OBJ_1(pid_pid_value_obj);

#endif

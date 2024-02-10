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

// structure holding the Taylor series representation of 
// the conversion of physical values to unitless numbers
typedef struct _pid_series_t {
    uint8_t len;            // order of the Taylor expansion
    mp_float_t x0;          // the function is expanded around this point
    mp_float_t *coeffs;     // pointer to coefficients of the Taylor series
} pid_series_t;

// structure holding the description and byte data of the input/output buffers
typedef struct _pid_buffer_t {
    uint8_t len;            // length of the input/output buffer
    uint8_t bitdepth;       // resolution of ADC/DAC       
    uint32_t mask;          // mask to get rid non-numerical data
    uint8_t offset;         // offset of beginning of data relative to beginning of buffer
    uint8_t *bytes;         // pointer to bytes of the buffer
} pid_buffer_t;

typedef struct _pid_data_converter_t {
    pid_buffer_t buffer;
    pid_series_t series;
} pid_data_converter_t;

typedef struct _pid_obj_t {
    mp_obj_base_t base;
    pid_data_converter_t in;
    pid_data_converter_t out;
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

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(pid_pid_bitdepth_obj);
MP_DECLARE_CONST_FUN_OBJ_3(pid_pid_buffer_obj);
MP_DECLARE_CONST_FUN_OBJ_3(pid_pid_convert_obj);
MP_DECLARE_CONST_FUN_OBJ_1(pid_pid_reset_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(pid_pid_series_obj);
MP_DECLARE_CONST_FUN_OBJ_1(pid_pid_step_obj);
MP_DECLARE_CONST_FUN_OBJ_1(pid_pid_value_obj);

#endif

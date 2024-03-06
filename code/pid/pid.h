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

enum PID_PARAMETER {
    PID_PARAMETER_P,
    PID_PARAMETER_I,
    PID_PARAMETER_D,
    PID_SETPOINT,
    PID_STEPS,
    PID_VALUE,
    PID_OUT,
};

enum PID_BUFFER {
    PID_BUFFER_INPUT,
    PID_BUFFER_OUTPUT,
    PID_BUFFER_BITDEPTH,
    PID_BUFFER_BYTES,
    PID_BUFFER_COEFFS,
    PID_BUFFER_MASK,
    PID_BUFFER_OFFSET,
    PID_BUFFER_ORDER,
    PID_BUFFER_X0,
};


// structure holding the Taylor series representation of 
// the conversion of physical values to unitless numbers
typedef struct _pid_series_t {
    uint8_t order;          // order of the Taylor expansion
    mp_float_t x0;          // the function is expanded around this point
    mp_float_t *coeffs;     // pointer to coefficients of the Taylor series
} pid_series_t;

// structure holding the description and byte data of the input/output buffers
typedef struct _pid_converter_t {
    uint8_t len;            // length of the input/output buffer, bytes
    uint8_t bitdepth;       // resolution of ADC/DAC, bits
    uint32_t mask;          // mask to get rid of non-numerical data (calculated)
    uint8_t offset;         // offset of beginning of data relative to beginning of buffer, bytes
    uint8_t *bytes;         // pointer to bytes of the buffer
    uint8_t nbytes;         // number of bytes in value; calculated from bitdepth
} pid_converter_t;

typedef struct _pid_buffer_t {
    pid_converter_t converter;
    pid_series_t series;
} pid_buffer_t;

typedef struct _pid_buffer_obj_t {
    mp_obj_base_t base;
    pid_converter_t converter;
    pid_series_t series;
} pid_buffer_obj_t;

typedef struct _pid_obj_t {
    mp_obj_base_t base;
    mp_float_t setpoint;        // set point of the controller loop
    pid_buffer_obj_t *input;    // the input buffer
    pid_buffer_obj_t *output;   // the output buffer
    mp_float_t P;               // coefficient of the proportional term
    mp_float_t I;               // coefficient of the integral term
    mp_float_t D;               // coefficient of the differential term
    mp_float_t integral;        // the last value of the integral term
    mp_float_t out;             // the output value
    mp_float_t value;           // the last converted value supplied to the loop
    mp_float_t error;           // the last calculated error; the difference between value and set_point
    mp_float_t last_time;       // the last time the loop was advanced
    uint64_t steps;             // the step method has been called this many times
} pid_obj_t;

mp_obj_t pid_buffer_make_new(const mp_obj_type_t *, size_t , size_t , const mp_obj_t *);
void pid_buffer_print(const mp_print_t *, mp_obj_t , mp_print_kind_t );
mp_obj_t pid_buffer_getter(mp_obj_t , uint8_t );
mp_obj_t pid_buffer_setter(mp_obj_t , mp_obj_t , uint8_t );

MP_DECLARE_CONST_FUN_OBJ_2(pid_buffer_evaluate_obj);
MP_DECLARE_CONST_FUN_OBJ_1(pid_buffer_init_obj);

mp_obj_t pid_pid_make_new(const mp_obj_type_t *, size_t , size_t , const mp_obj_t *);
void pid_pid_print(const mp_print_t *, mp_obj_t , mp_print_kind_t );
mp_obj_t pid_pid_parameters(mp_obj_t , mp_obj_t , uint8_t );
mp_obj_t pid_return_buffer(mp_obj_t , mp_obj_t , uint8_t );

MP_DECLARE_CONST_FUN_OBJ_1(pid_pid_reset_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(pid_pid_float_step_obj);
MP_DECLARE_CONST_FUN_OBJ_1(pid_pid_step_obj);

#endif

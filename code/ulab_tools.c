/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Zoltán Vörös
 */



#include "py/runtime.h"

#include "ulab.h"
#include "ndarray.h"
#include "ulab_tools.h"

// The following five functions return a float from a void type
// The value in question is supposed to be located at the head of the pointer

mp_float_t ndarray_get_float_uint8(void *data) {
    // Returns a float value from an uint8_t type
    return (mp_float_t)(*(uint8_t *)data);
}

mp_float_t ndarray_get_float_int8(void *data) {
    // Returns a float value from an int8_t type
    return (mp_float_t)(*(int8_t *)data);
}

mp_float_t ndarray_get_float_uint16(void *data) {
    // Returns a float value from an uint16_t type
    return (mp_float_t)(*(uint16_t *)data);
}

mp_float_t ndarray_get_float_int16(void *data) {
    // Returns a float value from an int16_t type
    return (mp_float_t)(*(int16_t *)data);
}


mp_float_t ndarray_get_float_float(void *data) {
    // Returns a float value from an mp_float_t type
    return *((mp_float_t *)data);
}

// returns a single function pointer, depending on the dtype
void *ndarray_get_float_function(uint8_t dtype) {
    if(dtype == NDARRAY_UINT8) {
        return ndarray_get_float_uint8;
    } else if(dtype == NDARRAY_INT8) {
        return ndarray_get_float_int8;
    } else if(dtype == NDARRAY_UINT16) {
        return ndarray_get_float_uint16;
    } else if(dtype == NDARRAY_INT16) {
        return ndarray_get_float_int16;
    } else {
        return ndarray_get_float_float;
    }
}

mp_float_t ndarray_get_float_index(void *data, uint8_t dtype, size_t index) {
	// returns a single float value from an array located at index
    if(dtype == NDARRAY_UINT8) {
        return (mp_float_t)((uint8_t *)data)[index];
    } else if(dtype == NDARRAY_INT8) {
        return (mp_float_t)((int8_t *)data)[index];
    } else if(dtype == NDARRAY_UINT16) {
        return (mp_float_t)((uint16_t *)data)[index];
    } else if(dtype == NDARRAY_INT16) {
        return (mp_float_t)((int16_t *)data)[index];
    } else {
        return (mp_float_t)((mp_float_t *)data)[index];
    }
}

mp_float_t ndarray_get_float_value(void *data, uint8_t dtype) {
    // Returns a float value from an arbitrary data type
    // The value in question is supposed to be located at the head of the pointer
    if(dtype == NDARRAY_UINT8) {
        return (mp_float_t)(*(uint8_t *)data);
    } else if(dtype == NDARRAY_INT8) {
        return (mp_float_t)(*(int8_t *)data);
    } else if(dtype == NDARRAY_UINT16) {
        return (mp_float_t)(*(uint16_t *)data);
    } else if(dtype == NDARRAY_INT16) {
        return (mp_float_t)(*(int16_t *)data);
    } else {
        return *((mp_float_t *)data);
    }
}

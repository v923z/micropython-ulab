/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Zoltán Vörös
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

#if NDARRAY_BINARY_USES_FUN_POINTER
uint8_t ndarray_upcast_dtype(uint8_t ldtype, uint8_t rdtype) {
    // returns a single character that corresponds to the broadcasting rules
    // - if one of the operarands is a float, the result is always float
    // - operation on identical types preserves type
    //
    // uint8 + int8 => int16
    // uint8 + int16 => int16
    // uint8 + uint16 => uint16
    // int8 + int16 => int16
    // int8 + uint16 => uint16
    // uint16 + int16 => float

    if(ldtype == rdtype) {
        // if the two dtypes are equal, the result is also of that type
        return ldtype;
    } else if(((ldtype == NDARRAY_UINT8) && (rdtype == NDARRAY_INT8)) ||
            ((ldtype == NDARRAY_INT8) && (rdtype == NDARRAY_UINT8)) ||
            ((ldtype == NDARRAY_UINT8) && (rdtype == NDARRAY_INT16)) ||
            ((ldtype == NDARRAY_INT16) && (rdtype == NDARRAY_UINT8)) ||
            ((ldtype == NDARRAY_INT8) && (rdtype == NDARRAY_INT16)) ||
            ((ldtype == NDARRAY_INT16) && (rdtype == NDARRAY_INT8))) {
        return NDARRAY_INT16;
    } else if(((ldtype == NDARRAY_UINT8) && (rdtype == NDARRAY_UINT16)) ||
            ((ldtype == NDARRAY_UINT16) && (rdtype == NDARRAY_UINT8)) ||
            ((ldtype == NDARRAY_INT8) && (rdtype == NDARRAY_UINT16)) ||
            ((ldtype == NDARRAY_UINT16) && (rdtype == NDARRAY_INT8))) {
        return NDARRAY_UINT16;
    }
    return NDARRAY_FLOAT;
}

void ndarray_set_float_uint8(void *data, mp_float_t datum) {
    *((uint8_t *)data) = (uint8_t)datum;
}

void ndarray_set_float_int8(void *data, int8_t datum) {
    *((int8_t *)data) = (int8_t)datum;
}

void ndarray_set_float_uint16(void *data, mp_float_t datum) {
    *((uint16_t *)data) = (uint16_t)datum;
}

void ndarray_set_float_int16(void *data, int8_t datum) {
    *((int16_t *)data) = (int16_t)datum;
}

void ndarray_set_float_float(void *data, mp_float_t datum) {
    *((mp_float_t *)data) = datum;
}

// returns a single function pointer, depending on the dtype
void *ndarray_set_float_function(uint8_t dtype) {
    if(dtype == NDARRAY_UINT8) {
        return ndarray_set_float_uint8;
    } else if(dtype == NDARRAY_INT8) {
        return ndarray_set_float_int8;
    } else if(dtype == NDARRAY_UINT16) {
        return ndarray_set_float_uint16;
    } else if(dtype == NDARRAY_INT16) {
        return ndarray_set_float_int16;
    } else {
        return ndarray_set_float_float;
    }
}
#endif /* NDARRAY_BINARY_USES_FUN_POINTER */

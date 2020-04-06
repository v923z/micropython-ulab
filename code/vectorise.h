
/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#ifndef _VECTORISE_
#define _VECTORISE_

#include "ulab.h"
#include "ndarray.h"

#if ULAB_VECTORISE_MODULE

mp_obj_module_t ulab_vectorise_module;

#define ITERATE_VECTOR(type, source, out) do {\
    type *input = (type *)(source)->array->items;\
    for(size_t i=0; i < (source)->array->len; i++) {\
                (out)[i] = f(input[i]);\
    }\
} while(0)

#define MATH_FUN_1(py_name, c_name) \
    static mp_obj_t vectorise_ ## py_name(mp_obj_t x_obj) { \
        return vectorise_generic_vector(x_obj, MICROPY_FLOAT_C_FUN(c_name)); \
}
    
#endif
#endif

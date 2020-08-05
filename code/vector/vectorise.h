
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

#include "../ulab.h"
#include "../ndarray.h"

#if ULAB_VECTORISE_MODULE

typedef struct _vectorized_function_obj_t {
    mp_obj_base_t base;
    uint8_t otypes;
    mp_obj_t fun;
    const mp_obj_type_t *type;
} vectorized_function_obj_t;

extern mp_obj_module_t ulab_vectorise_module;

#define ITERATE_VECTOR(type, source, array) do {\
    type *input = (type *)(source)->array;\
    size_t i=0, j, k, l;\
    do {\
        j = 0;\
        do {\
            k = 0;\
            do {\
                l = 0;\
                do {\
                    *(array)++ = f(*input);\
                    l++;\
                } while(l < (source)->shape[3]);\
            } while(k < (source)->shape[2]);\
        } while(j < (source)->shape[1]);\
    } while(i < (source)->shape[0]);\
} while(0)

#define MATH_FUN_1(py_name, c_name) \
    static mp_obj_t vectorise_ ## py_name(mp_obj_t x_obj) { \
        return vectorise_generic_vector(x_obj, MICROPY_FLOAT_C_FUN(c_name)); \
}
    
#endif
#endif

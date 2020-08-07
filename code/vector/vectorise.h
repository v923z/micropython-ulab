
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

#if ULAB_MAX_DIMS == 4
#define ITERATE_VECTOR(type, source, array) do {\
    type *input = (type *)(source)->array;\
    size_t i=0;\
    do {\
        size_t j = 0;\
        do {\
            size_t k = 0;\
            do {\
                size_t l = 0;\
                do {\
                    *(array) = f(*input);\
                    l++;\
                } while(l < (source)->shape[ULAB_MAX_DIMS-1]);\
            } while(k < (source)->shape[ULAB_MAX_DIMS-2]);\
        } while(j < (source)->shape[ULAB_MAX_DIMS-3]);\
    } while(i < (source)->shape[ULAB_MAX_DIMS-4]);\
} while(0)
#endif

#if ULAB_MAX_DIMS == 3
#define ITERATE_VECTOR(type, source, array) do {\
    type *input = (type *)(source)->array;\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                *(array)++ = f(*input);\
                l++;\
            } while(l < (source)->shape[ULAB_MAX_DIMS-1]);\
        } while(k < (source)->shape[ULAB_MAX_DIMS-2]);\
    } while(j < (source)->shape[ULAB_MAX_DIMS-3]);\
} while(0)
#endif

#if ULAB_MAX_DIMS == 2
#define ITERATE_VECTOR(type, source, array) do {\
    type *input = (type *)(source)->array;\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            *(array)++ = f(*input);\
            l++;\
        } while(l < (source)->shape[ULAB_MAX_DIMS-1]);\
    } while(k < (source)->shape[ULAB_MAX_DIMS-2]);\
} while(0)
#endif

#if ULAB_MAX_DIMS == 1
#define ITERATE_VECTOR(type, source, array) do {\
    type *input = (type *)(source)->array;\
    size_t l = 0;\
    do {\
        *(array)++ = f(*input);\
        l++;\
    } while(l < (source)->shape[ULAB_MAX_DIMS-1]);\
} while(0)
#endif

#define MATH_FUN_1(py_name, c_name) \
    static mp_obj_t vectorise_ ## py_name(mp_obj_t x_obj) { \
        return vectorise_generic_vector(x_obj, MICROPY_FLOAT_C_FUN(c_name)); \
}
    
#endif
#endif

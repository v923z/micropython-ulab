
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
*/

#ifndef _VECTOR_
#define _VECTOR_

#include "../ulab.h"
#include "../ndarray.h"


#if ULAB_MATH_FUNCTIONS_OUT_KEYWORD
MP_DECLARE_CONST_FUN_OBJ_KW(vector_acos_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_acosh_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_asin_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_asinh_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_atan_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_atanh_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_ceil_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_cos_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_cosh_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_degrees_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_erf_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_erfc_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_exp_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_expm1_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_floor_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_gamma_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_lgamma_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_log_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_log10_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_log2_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_radians_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_sin_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_sinc_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_sinh_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_tan_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_tanh_obj);
#else
MP_DECLARE_CONST_FUN_OBJ_1(vector_acos_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_acosh_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_asin_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_asinh_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_atan_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_atanh_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_ceil_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_cos_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_cosh_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_degrees_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_erf_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_erfc_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_exp_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_expm1_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_floor_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_gamma_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_lgamma_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_log_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_log10_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_log2_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_radians_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_sin_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_sinc_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_sinh_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_tan_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vector_tanh_obj);
#endif

MP_DECLARE_CONST_FUN_OBJ_2(vector_arctan2_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vector_around_obj);

#if ULAB_SUPPORTS_COMPLEX | ULAB_MATH_FUNCTIONS_OUT_KEYWORD
MP_DECLARE_CONST_FUN_OBJ_KW(vector_sqrt_obj);
#else
MP_DECLARE_CONST_FUN_OBJ_1(vector_sqrt_obj);
#endif

MP_DECLARE_CONST_FUN_OBJ_KW(vector_vectorize_obj);

typedef struct _vectorized_function_obj_t {
    mp_obj_base_t base;
    uint8_t otypes;
    mp_obj_t fun;
    const mp_obj_type_t *type;
} vectorized_function_obj_t;


#if ULAB_MATH_FUNCTIONS_OUT_KEYWORD

#if ULAB_HAS_FUNCTION_ITERATOR
#define ITERATE_VECTOR(type, target, tarray, tstrides, source, sarray)\
({\
    size_t *scoords = ndarray_new_coords((source)->ndim);\
    for(size_t i = 0; i < (source)->len / (source)->shape[ULAB_MAX_DIMS - 1]; i++) {\
        for(size_t l = 0; l < (source)->shape[ULAB_MAX_DIMS - 1]; l++) {\
            *(tarray) = f(*((type *)(sarray)));\
            (tarray) += (tstrides)[ULAB_MAX_DIMS - 1];\
            (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
        }\
        ndarray_rewind_array((source)->ndim, sarray, (source)->shape, (source)->strides, scoords);\
    }\
})

#else 

#if ULAB_MAX_DIMS == 1
#define ITERATE_VECTOR(type, target, tarray, tstrides, source, sarray) do {\
    size_t l = 0;\
    do {\
        *(tarray) = f(*((type *)(sarray)));\
        (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
        (tarray) += (tstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (source)->shape[ULAB_MAX_DIMS-1]);\
} while(0)
#endif /* ULAB_MAX_DIMS == 1 */

#if ULAB_MAX_DIMS == 2
#define ITERATE_VECTOR(type, target, tarray, tstrides, source, sarray) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            *(tarray) = f(*((type *)(sarray)));\
            (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
            (tarray) += (tstrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (source)->shape[ULAB_MAX_DIMS-1]);\
        (sarray) -= (source)->strides[ULAB_MAX_DIMS - 1] * (source)->shape[ULAB_MAX_DIMS - 1];\
        (sarray) += (source)->strides[ULAB_MAX_DIMS - 2];\
        (tarray) -= (tstrides)[ULAB_MAX_DIMS - 1] * (target)->shape[ULAB_MAX_DIMS - 1];\
        (tarray) += (tstrides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (source)->shape[ULAB_MAX_DIMS - 2]);\
} while(0)
#endif /* ULAB_MAX_DIMS == 2 */

#if ULAB_MAX_DIMS == 3
#define ITERATE_VECTOR(type, target, tarray, tstrides, source, sarray) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                *(tarray) = f(*((type *)(sarray)));\
                (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
                (tarray) += (tstrides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (source)->shape[ULAB_MAX_DIMS-1]);\
            (sarray) -= (source)->strides[ULAB_MAX_DIMS - 1] * (source)->shape[ULAB_MAX_DIMS - 1];\
            (sarray) += (source)->strides[ULAB_MAX_DIMS - 2];\
            (tarray) -= (tstrides)[ULAB_MAX_DIMS - 1] * (target)->shape[ULAB_MAX_DIMS - 1];\
            (tarray) += (tstrides)[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (source)->shape[ULAB_MAX_DIMS-2]);\
        (sarray) -= (source)->strides[ULAB_MAX_DIMS - 2] * (source)->shape[ULAB_MAX_DIMS - 2];\
        (sarray) += (source)->strides[ULAB_MAX_DIMS - 3];\
        (tarray) -= (tstrides)[ULAB_MAX_DIMS - 2] * (target)->shape[ULAB_MAX_DIMS - 2];\
        (tarray) += (tstrides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (source)->shape[ULAB_MAX_DIMS - 3]);\
} while(0)
#endif /* ULAB_MAX_DIMS == 3 */

#if ULAB_MAX_DIMS == 4
#define ITERATE_VECTOR(type, target, tshape, tstrides, source, sarray) do {\
    size_t i = 0;\
    do {\
        size_t j = 0;\
        do {\
            size_t k = 0;\
            do {\
                size_t l = 0;\
                do {\
                    *(tarray) = f(*((type *)(sarray)));\
                    (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
                    (tarray) += (tstrides)[ULAB_MAX_DIMS - 1];\
                    l++;\
                } while(l < (source)->shape[ULAB_MAX_DIMS-1]);\
                (sarray) -= (source)->strides[ULAB_MAX_DIMS - 1] * (source)->shape[ULAB_MAX_DIMS - 1];\
                (sarray) += (source)->strides[ULAB_MAX_DIMS - 2];\
                (tarray) -= (tstrides)[ULAB_MAX_DIMS - 1] * (target)->shape[ULAB_MAX_DIMS - 1];\
                (tarray) += (tstrides)[ULAB_MAX_DIMS - 2];\
                k++;\
            } while(k < (source)->shape[ULAB_MAX_DIMS-2]);\
            (sarray) -= (source)->strides[ULAB_MAX_DIMS - 2] * (source)->shape[ULAB_MAX_DIMS - 2];\
            (sarray) += (source)->strides[ULAB_MAX_DIMS - 3];\
            (tarray) -= (tstrides)[ULAB_MAX_DIMS - 2] * (target)->shape[ULAB_MAX_DIMS - 2];\
            (tarray) += (tstrides)[ULAB_MAX_DIMS - 3];\
            j++;\
        } while(j < (source)->shape[ULAB_MAX_DIMS - 3]);\
        (sarray) -= (source)->strides[ULAB_MAX_DIMS - 3] * (source)->shape[ULAB_MAX_DIMS - 3];\
        (sarray) += (source)->strides[ULAB_MAX_DIMS - 4];\
        (tarray) -= (tstrides)[ULAB_MAX_DIMS - 3] * (target)->shape[ULAB_MAX_DIMS - 3];\
        (tarray) += (tstrides)[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (source)->shape[ULAB_MAX_DIMS - 4]);\
} while(0)
#endif /* ULAB_MAX_DIMS == 4 */
#endif /* ULAB_HAS_FUNCTION_ITERATOR */

#define MATH_FUN_1(py_name, c_name) \
    static mp_obj_t vector_ ## py_name(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) { \
        return vector_generic_vector(n_args, pos_args, kw_args, MICROPY_FLOAT_C_FUN(c_name)); \
}

#else /* ULAB_MATH_FUNCTIONS_OUT_KEYWORD */

#if ULAB_HAS_FUNCTION_ITERATOR
#define ITERATE_VECTOR(type, array, source, sarray, shift)\
({\
    size_t *scoords = ndarray_new_coords((source)->ndim);\
    for(size_t i=0; i < (source)->len / (source)->shape[ULAB_MAX_DIMS - 1]; i++) {\
        for(size_t l = 0; l < (source)->shape[ULAB_MAX_DIMS - 1]; l++) {\
            *(array) = f(*((type *)(sarray)));\
            (array)++;\
            (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
        }\
        ndarray_rewind_array((source)->ndim, sarray, (source)->shape, (source)->strides, scoords);\
    }\
})

#else 

#if ULAB_MAX_DIMS == 1
#define ITERATE_VECTOR(type, array, source, sarray) do {\
    size_t l = 0;\
    do {\
        *(array)++ = f(*((type *)(sarray)));\
        (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (source)->shape[ULAB_MAX_DIMS - 1]);\
} while(0)
#endif /* ULAB_MAX_DIMS == 1 */

#if ULAB_MAX_DIMS == 2
#define ITERATE_VECTOR(type, array, source, sarray) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            *(array)++ = f(*((type *)(sarray)));\
            (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (source)->shape[ULAB_MAX_DIMS - 1]);\
        (sarray) -= (source)->strides[ULAB_MAX_DIMS - 1] * (source)->shape[ULAB_MAX_DIMS - 1];\
        (sarray) += (source)->strides[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (source)->shape[ULAB_MAX_DIMS - 2]);\
} while(0)
#endif /* ULAB_MAX_DIMS == 2 */

#if ULAB_MAX_DIMS == 3
#define ITERATE_VECTOR(type, array, source, sarray) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                *(array)++ = f(*((type *)(sarray)));\
                (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (source)->shape[ULAB_MAX_DIMS - 1]);\
            (sarray) -= (source)->strides[ULAB_MAX_DIMS - 1] * (source)->shape[ULAB_MAX_DIMS - 1];\
            (sarray) += (source)->strides[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (source)->shape[ULAB_MAX_DIMS - 2]);\
        (sarray) -= (source)->strides[ULAB_MAX_DIMS - 2] * (source)->shape[ULAB_MAX_DIMS - 2];\
        (sarray) += (source)->strides[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (source)->shape[ULAB_MAX_DIMS - 3]);\
} while(0)
#endif /* ULAB_MAX_DIMS == 3 */

#if ULAB_MAX_DIMS == 4
#define ITERATE_VECTOR(type, array, source, sarray) do {\
    size_t i = 0;\
    do {\
        size_t j = 0;\
        do {\
            size_t k = 0;\
            do {\
                size_t l = 0;\
                do {\
                    *(array)++ = f(*((type *)(sarray)));\
                    (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
                    l++;\
                } while(l < (source)->shape[ULAB_MAX_DIMS - 1]);\
                (sarray) -= (source)->strides[ULAB_MAX_DIMS - 1] * (source)->shape[ULAB_MAX_DIMS - 1];\
                (sarray) += (source)->strides[ULAB_MAX_DIMS - 2];\
                k++;\
            } while(k < (source)->shape[ULAB_MAX_DIMS - 2]);\
            (sarray) -= (source)->strides[ULAB_MAX_DIMS - 2] * (source)->shape[ULAB_MAX_DIMS - 2];\
            (sarray) += (source)->strides[ULAB_MAX_DIMS - 3];\
            j++;\
        } while(j < (source)->shape[ULAB_MAX_DIMS - 3]);\
        (sarray) -= (source)->strides[ULAB_MAX_DIMS - 3] * (source)->shape[ULAB_MAX_DIMS - 3];\
        (sarray) += (source)->strides[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (source)->shape[ULAB_MAX_DIMS - 4]);\
} while(0)
#endif /* ULAB_MAX_DIMS == 4 */

#endif /* ULAB_HAS_FUNCTION_ITERATOR */

#define MATH_FUN_1(py_name, c_name) \
    static mp_obj_t vector_ ## py_name(mp_obj_t x_obj) { \
        return vector_generic_vector(x_obj, MICROPY_FLOAT_C_FUN(c_name)); \
}
#endif /* ULAB_MATH_FUNCTIONS_OUT_KEYWORD */
#endif /* _VECTOR_ */

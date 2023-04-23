/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2023 Zoltán Vörös
*/

#include "ndarray.h"

mp_obj_t ndarray_binary_equality(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *,  int32_t *, int32_t *, mp_binary_op_t );
mp_obj_t ndarray_binary_add(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *);
mp_obj_t ndarray_binary_multiply(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *);
mp_obj_t ndarray_binary_more(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *, mp_binary_op_t );
mp_obj_t ndarray_binary_power(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *);
mp_obj_t ndarray_binary_subtract(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *);
mp_obj_t ndarray_binary_true_divide(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *);
mp_obj_t ndarray_binary_floor_divide(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *);

mp_obj_t ndarray_inplace_ams(ndarray_obj_t *, ndarray_obj_t *, int32_t *, uint8_t );
mp_obj_t ndarray_inplace_power(ndarray_obj_t *, ndarray_obj_t *, int32_t *);
mp_obj_t ndarray_inplace_divide(ndarray_obj_t *, ndarray_obj_t *, int32_t *);

#define UNWRAP_INPLACE_OPERATOR(lhs, larray, rarray, rstrides, OPERATOR)\
({\
    if((lhs)->dtype == NDARRAY_UINT8) {\
        if((rhs)->dtype == NDARRAY_UINT8) {\
            INPLACE_LOOP((lhs), uint8_t, uint8_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else if(rhs->dtype == NDARRAY_INT8) {\
            INPLACE_LOOP((lhs), uint8_t, int8_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else if(rhs->dtype == NDARRAY_UINT16) {\
            INPLACE_LOOP((lhs), uint8_t, uint16_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else {\
            INPLACE_LOOP((lhs), uint8_t, int16_t, (larray), (rarray), (rstrides), OPERATOR);\
        }\
    } else if(lhs->dtype == NDARRAY_INT8) {\
        if(rhs->dtype == NDARRAY_UINT8) {\
            INPLACE_LOOP((lhs), int8_t, uint8_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else if(rhs->dtype == NDARRAY_INT8) {\
            INPLACE_LOOP((lhs), int8_t, int8_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else if(rhs->dtype == NDARRAY_UINT16) {\
            INPLACE_LOOP((lhs), int8_t, uint16_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else {\
            INPLACE_LOOP((lhs), int8_t, int16_t, (larray), (rarray), (rstrides), OPERATOR);\
        }\
    } else if(lhs->dtype == NDARRAY_UINT16) {\
        if(rhs->dtype == NDARRAY_UINT8) {\
            INPLACE_LOOP((lhs), uint16_t, uint8_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else if(rhs->dtype == NDARRAY_INT8) {\
            INPLACE_LOOP((lhs), uint16_t, int8_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else if(rhs->dtype == NDARRAY_UINT16) {\
            INPLACE_LOOP((lhs), uint16_t, uint16_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else {\
            INPLACE_LOOP((lhs), uint16_t, int16_t, (larray), (rarray), (rstrides), OPERATOR);\
        }\
    } else if(lhs->dtype == NDARRAY_INT16) {\
        if(rhs->dtype == NDARRAY_UINT8) {\
            INPLACE_LOOP((lhs), int16_t, uint8_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else if(rhs->dtype == NDARRAY_INT8) {\
            INPLACE_LOOP((lhs), int16_t, int8_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else if(rhs->dtype == NDARRAY_UINT16) {\
            INPLACE_LOOP((lhs), int16_t, uint16_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else {\
            INPLACE_LOOP((lhs), int16_t, int16_t, (larray), (rarray), (rstrides), OPERATOR);\
        }\
    } else if(lhs->dtype == NDARRAY_FLOAT) {\
        if(rhs->dtype == NDARRAY_UINT8) {\
            INPLACE_LOOP((lhs), mp_float_t, uint8_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else if(rhs->dtype == NDARRAY_INT8) {\
            INPLACE_LOOP((lhs), mp_float_t, int8_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else if(rhs->dtype == NDARRAY_UINT16) {\
            INPLACE_LOOP((lhs), mp_float_t, uint16_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else if(rhs->dtype == NDARRAY_INT16) {\
            INPLACE_LOOP((lhs), mp_float_t, int16_t, (larray), (rarray), (rstrides), OPERATOR);\
        } else {\
            INPLACE_LOOP((lhs), mp_float_t, mp_float_t, (larray), (rarray), (rstrides), OPERATOR);\
        }\
    }\
})

#if ULAB_MAX_DIMS == 1
#define INPLACE_POWER(results, type_left, type_right, larray, rarray, rstrides)\
({  size_t l = 0;\
    do {\
        *((type_left *)(larray)) = MICROPY_FLOAT_C_FUN(pow)(*((type_left *)(larray)), *((type_right *)(rarray)));\
        (larray) += (results)->strides[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
})

#define FUNC_POINTER_LOOP(results, array, get_lhs, get_rhs, larray, lstrides, rarray, rstrides, OPERATION)\
({  size_t l = 0;\
    do {\
        mp_float_t lvalue = (get_lhs)((larray));\
        mp_float_t rvalue = (get_rhs)((rarray));\
        (set_result)((array), OPERATION);\
        (array) += (results)->itemsize;\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
})
#endif /* ULAB_MAX_DIMS == 1 */

#if ULAB_MAX_DIMS == 2
#define INPLACE_POWER(results, type_left, type_right, larray, rarray, rstrides)\
({  size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            *((type_left *)(larray)) = MICROPY_FLOAT_C_FUN(pow)(*((type_left *)(larray)), *((type_right *)(rarray)));\
            (larray) += (results)->strides[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
        (larray) -= (results)->strides[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (larray) += (results)->strides[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
})

#define FUNC_POINTER_LOOP(results, array, get_lhs, get_rhs, larray, lstrides, rarray, rstrides, OPERATION)\
({  size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            mp_float_t lvalue = (get_lhs)((larray));\
            mp_float_t rvalue = (get_rhs)((rarray));\
            (set_result)((array), OPERATION);\
            (array) += (results)->itemsize;\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < results->shape[ULAB_MAX_DIMS - 2]);\
})
#endif /* ULAB_MAX_DIMS == 2 */

#if ULAB_MAX_DIMS == 3
#define INPLACE_POWER(results, type_left, type_right, larray, rarray, rstrides)\
({  size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                *((type_left *)(larray)) = MICROPY_FLOAT_C_FUN(pow)(*((type_left *)(larray)), *((type_right *)(rarray)));\
                (larray) += (results)->strides[ULAB_MAX_DIMS - 1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
            (larray) -= (results)->strides[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (larray) += (results)->strides[ULAB_MAX_DIMS - 2];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
        (larray) -= (results)->strides[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (larray) += (results)->strides[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\
})


#define FUNC_POINTER_LOOP(results, array, get_lhs, get_rhs, larray, lstrides, rarray, rstrides, OPERATION)\
({  size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                mp_float_t lvalue = (get_lhs)((larray));\
                mp_float_t rvalue = (get_rhs)((rarray));\
                (set_result)((array), OPERATION);\
                (array) += (results)->itemsize;\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < results->shape[ULAB_MAX_DIMS - 2]);\
        (larray) -= (results)->strides[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (larray) += (results)->strides[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\
})
#endif /* ULAB_MAX_DIMS == 3 */

#if ULAB_MAX_DIMS == 4
#define INPLACE_POWER(results, type_left, type_right, larray, rarray, rstrides)\
({  size_t i = 0;\
    do {\
        size_t j = 0;\
        do {\
            size_t k = 0;\
            do {\
                size_t l = 0;\
                do {\
                    *((type_left *)(larray)) = MICROPY_FLOAT_C_FUN(pow)(*((type_left *)(larray)), *((type_right *)(rarray)));\
                    (larray) += (results)->strides[ULAB_MAX_DIMS - 1];\
                    (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                    l++;\
                } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
                (larray) -= (results)->strides[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
                (larray) += (results)->strides[ULAB_MAX_DIMS - 2];\
                (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
                k++;\
            } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
            (larray) -= (results)->strides[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
            (larray) += (results)->strides[ULAB_MAX_DIMS - 3];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
            j++;\
        } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\
        (larray) -= (results)->strides[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (larray) += (results)->strides[ULAB_MAX_DIMS - 4];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (results)->shape[ULAB_MAX_DIMS - 4]);\
})

#define FUNC_POINTER_LOOP(results, array, get_lhs, get_rhs, larray, lstrides, rarray, rstrides, OPERATION)\
({  size_t i = 0;\
    do {\
        size_t j = 0;\
        do {\
            size_t k = 0;\
            do {\
                size_t l = 0;\
                do {\
                    mp_float_t lvalue = (get_lhs)((larray));\
                    mp_float_t rvalue = (get_rhs)((rarray));\
                    (set_result)((array), OPERATION);\
                    (array) += (results)->itemsize;\
                    (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
                    (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                    l++;\
                } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
                (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
                (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
                k++;\
            } while(k < results->shape[ULAB_MAX_DIMS - 2]);\
            (larray) -= (results)->strides[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
            (larray) += (results)->strides[ULAB_MAX_DIMS - 3];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
            j++;\
        } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\
        (larray) -= (results)->strides[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (larray) += (results)->strides[ULAB_MAX_DIMS - 4];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (results)->shape[ULAB_MAX_DIMS - 4]);\
})
#endif /* ULAB_MAX_DIMS == 4 */

#define FLOOR_DIVIDE_UINT1(results, array, type_left, type_right, larray, lstrides, rarray, rstrides)\
({\
    size_t l = 0;\
    do {\
        *(array)++ = *((type_left *)(larray)) / *((type_right *)(rarray));\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
})

#define FLOOR_DIVIDE1(results, array, type_left, type_right, larray, lstrides, rarray, rstrides)\
({\
    size_t l = 0;\
    int16_t num;\
    int16_t denom = (int16_t)*((type_right *)(rarray));\
    do {\
        num = (int16_t)*((type_left *)(larray));\
        if(num >= 0) {\
            if(denom < 0) {\
                num += -denom - 1;\
            }\
        } else {\
            if(denom >= 0) {\
                num += -denom + 1;\
            }\
        }\
        *(array)++ =  num / denom;\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
})

#define FLOOR_DIVIDE_FLOAT1(results, array, type_left, type_right, larray, lstrides, rarray, rstrides)\
({\
    size_t l = 0;\
    do {\
        *(array)++ = MICROPY_FLOAT_C_FUN(floor)(*((type_left *)(larray)) / *((type_right *)(rarray)));\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
})

#if ULAB_MAX_DIMS == 1
#define FLOOR_DIVIDE_LOOP_UINT(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides) do {\
    type_out *array = (type_out *)(results)->array;\
    FLOOR_DIVIDE_UINT1((results), (array), type_left, type_right, (larray), (lstrides), (rarray), (rstrides));\
} while(0)

#define FLOOR_DIVIDE_LOOP(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides) do {\
    type_out *array = (type_out *)(results)->array;\
    FLOOR_DIVIDE1((results), (array), type_left, type_right, (larray), (lstrides), (rarray), (rstrides));\
} while(0)

#define FLOOR_DIVIDE_LOOP_FLOAT(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides) do {\
    type_out *array = (type_out *)(results)->array;\
    FLOOR_DIVIDE_FLOAT1((results), (array), type_left, type_right, (larray), (lstrides), (rarray), (rstrides));\
} while(0)
#endif /* ULAB_MAX_DIMS == 1 */

#if ULAB_MAX_DIMS == 2
#define FLOOR_DIVIDE_LOOP_UINT(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides) do {\
    type_out *array = (type_out *)(results)->array;\
    size_t l = 0;\
    do {\
        FLOOR_DIVIDE_UINT1((results), (array), type_left, type_right, (larray), (lstrides), (rarray), (rstrides));\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 2]);\
} while(0)

#define FLOOR_DIVIDE_LOOP(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides) do {\
    type_out *array = (type_out *)(results)->array;\
    size_t l = 0;\
    do {\
        FLOOR_DIVIDE1((results), (array), type_left, type_right, (larray), (lstrides), (rarray), (rstrides));\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 2]);\
} while(0)

#define FLOOR_DIVIDE_LOOP_FLOAT(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides) do {\
    type_out *array = (type_out *)(results)->array;\
    size_t l = 0;\
    do {\
        FLOOR_DIVIDE_FLOAT1((results), (array), type_left, type_right, (larray), (lstrides), (rarray), (rstrides));\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 2]);\
} while(0)

#endif /* ULAB_MAX_DIMS == 2 */

#if ULAB_MAX_DIMS == 3
#define FLOOR_DIVIDE_LOOP_UINT(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides) do {\
  type_out *array = (type_out *)(results)->array;\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            FLOOR_DIVIDE_UINT1((results), (array), type_left, type_right, (larray), (lstrides), (rarray), (rstrides));\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 2]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        k++;\
    } while(k < (results)->shape[ULAB_MAX_DIMS - 3]);\
} while(0)

#define FLOOR_DIVIDE_LOOP(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides) do {\
    type_out *array = (type_out *)(results)->array;\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            FLOOR_DIVIDE1((results), (array), type_left, type_right, (larray), (lstrides), (rarray), (rstrides));\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 2]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        k++;\
    } while(k < (results)->shape[ULAB_MAX_DIMS - 3]);\
} while(0)

#define FLOOR_DIVIDE_LOOP_FLOAT(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides) do {\
    type_out *array = (type_out *)(results)->array;\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            FLOOR_DIVIDE_FLOAT1((results), (array), type_left, type_right, (larray), (lstrides), (rarray), (rstrides));\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 2]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        k++;\
    } while(k < (results)->shape[ULAB_MAX_DIMS - 3]);\
} while(0)

#endif /* ULAB_MAX_DIMS == 3 */

#if ULAB_MAX_DIMS == 4
#define FLOOR_DIVIDE_LOOP_UINT(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides) do {\
    type_out *array = (type_out *)(results)->array;\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                FLOOR_DIVIDE_UINT1((results), (array), type_left, type_right, (larray), (lstrides), (rarray), (rstrides));\
                (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
                (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
                l++;\
            } while(l < (results)->shape[ULAB_MAX_DIMS - 2]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
            k++;\
        } while(k < (results)->shape[ULAB_MAX_DIMS - 3]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 4];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 4]);\
} while(0)

#define FLOOR_DIVIDE_LOOP(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides) do {\
    type_out *array = (type_out *)(results)->array;\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                FLOOR_DIVIDE1((results), (array), type_left, type_right, (larray), (lstrides), (rarray), (rstrides));\
                (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
                (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
                l++;\
            } while(l < (results)->shape[ULAB_MAX_DIMS - 2]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
            k++;\
        } while(k < (results)->shape[ULAB_MAX_DIMS - 3]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 4];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 4]);\
} while(0)

#define FLOOR_DIVIDE_LOOP_FLOAT(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides) do {\
    type_out *array = (type_out *)(results)->array;\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                FLOOR_DIVIDE_FLOAT1((results), (array), type_left, type_right, (larray), (lstrides), (rarray), (rstrides));\
                (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
                (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
                l++;\
            } while(l < (results)->shape[ULAB_MAX_DIMS - 2]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
            k++;\
        } while(k < (results)->shape[ULAB_MAX_DIMS - 3]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 4];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 4]);\
} while(0)

#endif /* ULAB_MAX_DIMS == 4 */

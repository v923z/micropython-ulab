
/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#ifndef _NUMERICAL_
#define _NUMERICAL_

#include "../ulab.h"
#include "../ndarray.h"

#if ULAB_NUMERICAL_MODULE

extern mp_obj_module_t ulab_numerical_module;

// TODO: implement cumsum
//mp_obj_t numerical_cumsum(size_t , const mp_obj_t *, mp_map_t *);


#define RUN_ARGMIN1(ndarray, type, array, results, rarray, index, op)\
({\
    uint16_t best_index = 0;\
    type best_value = *((type *)(array));\
    if(((op) == NUMERICAL_MAX) || ((op) == NUMERICAL_ARGMAX)) {\
        for(uint16_t i=0; i < (ndarray)->shape[(index)]; i++) {\
            if(*((type *)(array)) > best_value) {\
                best_index = i;\
                best_value = *((type *)(array));\
            }\
            (array) += (ndarray)->strides[(index)];\
        }\
    } else {\
        for(uint16_t i=0; i < (ndarray)->shape[(index)]; i++) {\
            if(*((type *)(array)) < best_value) {\
                best_index = i;\
                best_value = *((type *)(array));\
            }\
            (array) += (ndarray)->strides[(index)];\
        }\
    }\
    if(((op) == NUMERICAL_ARGMAX) || ((op) == NUMERICAL_ARGMIN)) {\
        memcpy((rarray), &best_index, (results)->itemsize);\
    } else {\
        memcpy((rarray), &best_value, (results)->itemsize);\
    }\
    (rarray) += (results)->itemsize;\
})

#define RUN_SUM1(ndarray, type, array, results, rarray, index)\
({\
    type sum = 0;\
    for(size_t i=0; i < (ndarray)->shape[(index)]; i++) {\
        sum += *((type *)(array));\
        (array) += (ndarray)->strides[(index)];\
    }\
    memcpy((rarray), &sum, (ndarray)->itemsize);\
    (rarray) += (ndarray)->itemsize;\
})

// The mean could be calculated by simply dividing the sum by
// the number of elements, but that method is numerically unstable
#define RUN_MEAN1(ndarray, type, array, results, r, index)\
({\
    mp_float_t M, m;\
    M = m = *(mp_float_t *)((type *)(array));\
    for(size_t i=1; i < (ndarray)->shape[(index)]; i++) {\
        (array) += (ndarray)->strides[(index)];\
        mp_float_t value = *(mp_float_t *)((type *)(array));\
        m = M + (value - M) / (mp_float_t)(i+1);\
        M = m;\
    }\
    (array) += (ndarray)->strides[(index)];\
    *(r)++ = M;\
})

// Instead of the straightforward implementation of the definition,
// we take the numerically stable Welford algorithm here
// https://www.johndcook.com/blog/2008/09/26/comparing-three-methods-of-computing-standard-deviation/
#define RUN_STD1(ndarray, type, array, results, r, index, div)\
({\
    mp_float_t M, m, S = 0.0, s = 0.0;\
    M = m = *(mp_float_t *)((type *)(array));\
    for(size_t i=1; i < (ndarray)->shape[(index)]; i++) {\
        (array) += (ndarray)->strides[(index)];\
        mp_float_t value = *(mp_float_t *)((type *)(array));\
        m = M + (value - M) / (mp_float_t)i;\
        s = S + (value - M) * (value - m);\
        M = m;\
        S = s;\
    }\
    (array) += (ndarray)->strides[(index)];\
    *(r)++ = MICROPY_FLOAT_C_FUN(sqrt)((ndarray)->shape[(index)] * s / (div));\
})

#define RUN_DIFF1(ndarray, type, array, results, rarray, index, stencil, N)\
({\
    for(size_t i=0; i < (ndarray)->shape[(index)] - (N); i++) {\
        type sum = 0;\
        uint8_t *source = (array);\
        for(uint8_t d=0; d < (N)+1; d++) {\
            sum -= (stencil)[d] * *((type *)source);\
            source += (ndarray)->strides[(index)];\
        }\
        (array) += (strides)[ULAB_MAX_DIMS - 1];\
        *(type *)(rarray) = sum;\
        (rarray) += (ndarray)->itemsize;\
    }\
})

#define HEAPSORT1(ndarray, type, shape, strides, index, increment);
({\
    type *_array = (type *)(ndarray)->array;\
    type tmp;\
    size_t c, q, p, k = ((ndarray)->shape[(index)] >> 1);\
    for (;;) {\
        if (k > 0) {\
            tmp = _array[(--k)*(increment)];\
        } else {\
            q--;\
            if(q == 0) {\
                break;\
            }\
            tmp = _array[q*(increment)];\
            _array[q*(increment)] = array[0];\
        }\
        p = k;\
        c = k + k + 1;\
        while (c < q) {\
            if((c + 1 < q)  &&  (_array[(c+1)*(increment)] > _array[c*(increment)])) {\
                c++;\
            }\
            if(_array[c*(increment)] > tmp) {\
                _array[p*(increment)] = _array[c*(increment)];\
                p = c;\
                c = p + p + 1;\
            } else {\
                break;\
            }\
        }\
        _array[p*(increment)] = tmp;\
    }\
})

#if ULAB_MAX_DIMS == 1
#define RUN_SUM(ndarray, type, array, results, rarray, shape, strides, index) do {\
    RUN_SUM1((ndarray), type, (array), (results), (rarray), (index));\
} while(0)

#define RUN_MEAN(ndarray, type, array, results, r, shape, strides, index) do {\
    RUN_MEAN1((ndarray), type, (array), (results), (r), (index));\
} while(0)

#define RUN_STD(ndarray, type, array, results, r, shape, strides, index, div) do {\
    RUN_STD1((ndarray), type, (array), (results), (r), (index), (div));\
} while(0)

#define RUN_ARGMIN(ndarray, type, array, results, rarray, shape, strides, index, op) do {\
    RUN_ARGMIN1((ndarray), type, (array), (results), (rarray), (index), (op));\
} while(0)

#define RUN_DIFF(ndarray, type, array, results, rarray, shape, strides, index, stencil, N) do {\
    RUN_DIFF1((ndarray), type, (array), (results), (rarray), (index), (stencil), (N));
} while(0)

#define HEAPSORT(ndarray, type, shape, strides, index, increment) do {\
    HEAPSORT1((ndarray), type, (shape), (strides), (index), (increment));
} while(0)

#endif

#if ULAB_MAX_DIMS == 2
#define RUN_SUM(ndarray, type, array, results, rarray, shape, strides, index) do {\
    size_t l = 0;\
    do {\
        RUN_SUM1((ndarray), type, (array), (results), (rarray), (index));\
        (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
        (array) += (strides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
} while(0)

#define RUN_MEAN(ndarray, type, array, results, r, shape, strides, index) do {\
    size_t l = 0;\
    do {\
        RUN_MEAN1((ndarray), type, (array), (results), (r), (index));\
        (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
        (array) += (strides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
} while(0)

#define RUN_STD(ndarray, type, array, results, r, shape, strides, index, div) do {\
    size_t l = 0;\
    do {\
        RUN_STD1((ndarray), type, (array), (results), (r), (index), (div));\
        (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
        (array) += (strides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
} while(0)

#define RUN_ARGMIN(ndarray, type, array, results, rarray, shape, strides, index, op) do {\
    size_t l = 0;\
    do {\
        RUN_ARGMIN1((ndarray), type, (array), (results), (rarray), (index), (op));\
        (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
        (array) += (strides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
} while(0)

#define RUN_DIFF(ndarray, type, array, results, rarray, shape, strides, index, stencil, N) do {\
    size_t l = 0;\
    do {\
        RUN_DIFF1((ndarray), type, (array), (results), (rarray), (index), (stencil), (N));\
        (array) -= (strides)[ULAB_MAX_DIMS - 1] * (shape)[ULAB_MAX_DIMS - 1];\
        (array) += (strides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
} while(0)

#endif

#if ULAB_MAX_DIMS == 3
#define RUN_SUM(ndarray, type, array, results, rarray, shape, strides, index) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            RUN_SUM1((ndarray), type, (array), (results), (rarray), (index));\
            (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
            (array) += (strides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
        (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
        (array) += (strides)[ULAB_MAX_DIMS - 3];\
        k++;\
    } while(k < (shape)[ULAB_MAX_DIMS - 2]);\
} while(0)

#define RUN_MEAN(ndarray, type, array, results, r, shape, strides, index) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            RUN_MEAN1((ndarray), type, (array), (results), (r), (index));\
            (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
            (array) += (strides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
        (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
        (array) += (strides)[ULAB_MAX_DIMS - 3];\
        k++;\
    } while(k < (shape)[ULAB_MAX_DIMS - 2]);\
} while(0)

#define RUN_STD(ndarray, type, array, results, r, shape, strides, index, div) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            RUN_STD1((ndarray), type, (array), (results), (r), (index), (div));\
            (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
            (array) += (strides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
        (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
        (array) += (strides)[ULAB_MAX_DIMS - 3];\
        k++;\
    } while(k < (shape)[ULAB_MAX_DIMS - 2]);\
} while(0)

#define RUN_ARGMIN(ndarray, type, array, results, rarray, shape, strides, index, op) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            RUN_ARGMIN1((ndarray), type, (array), (results), (rarray), (index), (op));\
            (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
            (array) += (strides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
        (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
        (array) += (strides)[ULAB_MAX_DIMS - 3];\
        k++;\
    } while(k < (shape)[ULAB_MAX_DIMS - 2]);\
} while(0)

#endif

#if ULAB_MAX_DIMS == 4
#define RUN_SUM(ndarray, type, array, results, rarray, shape, strides, index) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                RUN_SUM1((ndarray), type, (array), (results), (rarray), (index));\
                (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
                (array) += (strides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
            (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
            (array) += (strides)[ULAB_MAX_DIMS - 3];\
            k++;\
        } while(k < (shape)[ULAB_MAX_DIMS - 2]);\
        (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
        (array) += (strides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (shape)[ULAB_MAX_DIMS - 3]);\
} while(0)

#define RUN_MEAN(ndarray, type, array, results, r, shape, strides, index) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                RUN_MEAN1((ndarray), type, (array), (results), (r), (index));\
                (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
                (array) += (strides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
            (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
            (array) += (strides)[ULAB_MAX_DIMS - 3];\
            k++;\
        } while(k < (shape)[ULAB_MAX_DIMS - 2]);\
        (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
        (array) += (strides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (shape)[ULAB_MAX_DIMS - 3]);\
} while(0)

#define RUN_STD(ndarray, type, array, results, r, shape, strides, index, div) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                RUN_STD1((ndarray), type, (array), (results), (r), (index), (div));\
                (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
                (array) += (strides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
            (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
            (array) += (strides)[ULAB_MAX_DIMS - 3];\
            k++;\
        } while(k < (shape)[ULAB_MAX_DIMS - 2]);\
        (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
        (array) += (strides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (shape)[ULAB_MAX_DIMS - 3]);\
} while(0)

#define RUN_ARGMIN(ndarray, type, array, results, rarray, shape, strides, index, op) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                RUN_ARGMIN1((ndarray), type, (array), (results), (rarray), (index), (op));\
                (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
                (array) += (strides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
            (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
            (array) += (strides)[ULAB_MAX_DIMS - 3];\
            k++;\
        } while(k < (shape)[ULAB_MAX_DIMS - 2]);\
        (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
        (array) += (strides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (shape)[ULAB_MAX_DIMS - 3]);\
} while(0)

#endif

// This is pretty similar to HEAPSORT above; perhaps, the two could be combined somehow
// On the other hand, since this is a macro, it doesn't really matter
// Keep in mind that initially, index_array[start+s*increment] = s
#define HEAP_ARGSORT(type, ndarray, index_array) do {\
    type *array = (type *)(ndarray)->array->items;\
    type tmp;\
    uint16_t itmp;\
    for (;;) {\
        if (k > 0) {\
            k--;\
            tmp = array[start+index_array[start+k*increment]*increment];\
            itmp = index_array[start+k*increment];\
        } else {\
            q--;\
            if(q == 0) {\
                break;\
            }\
            tmp = array[start+index_array[start+q*increment]*increment];\
            itmp = index_array[start+q*increment];\
            index_array[start+q*increment] = index_array[start];\
        }\
        p = k;\
        c = k + k + 1;\
        while (c < q) {\
            if((c + 1 < q)  &&  (array[start+index_array[start+(c+1)*increment]*increment] > array[start+index_array[start+c*increment]*increment])) {\
                c++;\
            }\
            if(array[start+index_array[start+c*increment]*increment] > tmp) {\
                index_array[start+p*increment] = index_array[start+c*increment];\
                p = c;\
                c = p + p + 1;\
            } else {\
                break;\
            }\
        }\
        index_array[start+p*increment] = itmp;\
    }\
} while(0)

MP_DECLARE_CONST_FUN_OBJ_KW(numerical_min_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_max_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_argmin_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_argmax_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_sum_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_mean_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_std_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_roll_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_flip_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_diff_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_sort_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_sort_inplace_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_argsort_obj);

#endif
#endif

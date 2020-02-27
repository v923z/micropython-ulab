
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

#include "ulab.h"
#include "ndarray.h"

#if ULAB_NUMERICAL_MODULE

extern mp_obj_module_t ulab_numerical_module;

// TODO: implement minimum/maximum, and cumsum
//mp_obj_t numerical_minimum(mp_obj_t , mp_obj_t );
//mp_obj_t numerical_maximum(mp_obj_t , mp_obj_t );
//mp_obj_t numerical_cumsum(size_t , const mp_obj_t *, mp_map_t *);

#define RUN_ARGMIN(in, out, typein, typeout, len, start, increment, op, pos) do {\
    typein *array = (typein *)(in)->array->items;\
    typeout *outarray = (typeout *)(out)->array->items;\
    size_t best_index = 0;\
    if(((op) == NUMERICAL_MAX) || ((op) == NUMERICAL_ARGMAX)) {\
        for(size_t i=1; i < (len); i++) {\
            if(array[(start)+i*(increment)] > array[(start)+best_index*(increment)]) best_index = i;\
        }\
        if((op) == NUMERICAL_MAX) outarray[(pos)] = array[(start)+best_index*(increment)];\
        else outarray[(pos)] = best_index;\
    } else{\
        for(size_t i=1; i < (len); i++) {\
            if(array[(start)+i*(increment)] < array[(start)+best_index*(increment)]) best_index = i;\
        }\
        if((op) == NUMERICAL_MIN) outarray[(pos)] = array[(start)+best_index*(increment)];\
        else outarray[(pos)] = best_index;\
    }\
} while(0)

#define RUN_SUM(ndarray, type, optype, len, start, increment) do {\
    type *array = (type *)(ndarray)->array->items;\
    type value;\
    for(size_t j=0; j < (len); j++) {\
        value = array[(start)+j*(increment)];\
        sum += value;\
    }\
} while(0)

#define RUN_STD(ndarray, type, len, start, increment) do {\
    type *array = (type *)(ndarray)->array->items;\
    mp_float_t value;\
    for(size_t j=0; j < (len); j++) {\
        sum += array[(start)+j*(increment)];\
    }\
    sum /= (len);\
    for(size_t j=0; j < (len); j++) {\
        value = (array[(start)+j*(increment)] - sum);\
        sum_sq += value * value;\
    }\
} while(0)

#define CALCULATE_DIFF(in, out, type, M, N, inn, increment) do {\
    type *source = (type *)(in)->array->items;\
    type *target = (type *)(out)->array->items;\
    for(size_t i=0; i < (M); i++) {\
        for(size_t j=0; j < (N); j++) {\
            for(uint8_t k=0; k < n+1; k++) {\
                target[i*(N)+j] -= stencil[k]*source[i*(inn)+j+k*(increment)];\
            }\
        }\
    }\
} while(0)

#define HEAPSORT(type, ndarray) do {\
    type *array = (type *)(ndarray)->array->items;\
    type tmp;\
    for (;;) {\
        if (k > 0) {\
            tmp = array[start+(--k)*increment];\
        } else {\
            q--;\
            if(q == 0) {\
                break;\
            }\
            tmp = array[start+q*increment];\
            array[start+q*increment] = array[start];\
        }\
        p = k;\
        c = k + k + 1;\
        while (c < q) {\
            if((c + 1 < q)  &&  (array[start+(c+1)*increment] > array[start+c*increment])) {\
                c++;\
            }\
            if(array[start+c*increment] > tmp) {\
                array[start+p*increment] = array[start+c*increment];\
                p = c;\
                c = p + p + 1;\
            } else {\
                break;\
            }\
        }\
        array[start+p*increment] = tmp;\
    }\
} while(0)

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

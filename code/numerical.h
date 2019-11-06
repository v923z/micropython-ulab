/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Zoltán Vörös
*/
    
#ifndef _NUMERICAL_
#define _NUMERICAL_

#include "ndarray.h"

mp_obj_t numerical_linspace(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_sum(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_mean(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_std(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_min(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_max(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_argmin(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_argmax(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_roll(size_t , const mp_obj_t *, mp_map_t *);

// TODO: implement minimum/maximum, and cumsum
mp_obj_t numerical_minimum(mp_obj_t , mp_obj_t );
mp_obj_t numerical_maximum(mp_obj_t , mp_obj_t );
mp_obj_t numerical_cumsum(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_flip(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_diff(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_sort(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_sort_inplace(size_t , const mp_obj_t *, mp_map_t *);
mp_obj_t numerical_argsort(size_t , const mp_obj_t *, mp_map_t *);

// this macro could be tighter, if we moved the ifs to the argmin function, assigned <, as well as >
#define ARG_MIN_LOOP(in, type, start, stop, stride, op) do {\
    type *array = (type *)(in)->array->items;\
    if(((op) == NUMERICAL_MAX) || ((op) == NUMERICAL_ARGMAX)) {\
        for(size_t i=(start)+(stride); i < (stop); i+=(stride)) {\
            if((array)[i] > (array)[best_idx]) {\
                best_idx = i;\
            }\
        }\
    } else{\
        for(size_t i=(start)+(stride); i < (stop); i+=(stride)) {\
            if((array)[i] < (array)[best_idx]) best_idx = i;\
        }\
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

#endif

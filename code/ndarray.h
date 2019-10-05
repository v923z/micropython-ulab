/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Zoltán Vörös
*/
    
#ifndef _NDARRAY_
#define _NDARRAY_

#include "py/objarray.h"
#include "py/binary.h"
#include "py/objstr.h"
#include "py/objlist.h"

#define PRINT_MAX  10


const mp_obj_type_t ulab_ndarray_type;

#define RUN_BINARY_SCALAR(typecode, type_out, type_array, type_scalar, ndarray, scalar, op) do {\
    type_array *avalue = (type_array *)(ndarray)->array->items;\
    if(((op) == MP_BINARY_OP_ADD) || ((op) == MP_BINARY_OP_SUBTRACT) || ((op) == MP_BINARY_OP_MULTIPLY)) {\
        ndarray_obj_t *out = create_new_ndarray((ndarray)->m, (ndarray)->n, typecode);\
        type_out *(odata) = (type_out *)out->array->items;\
        type_scalar svalue = (type_scalar)scalar;\
        if((op) == MP_BINARY_OP_ADD) { for(size_t i=0; i < out->array->len; i++) odata[i] = avalue[i] + svalue;}\
        if((op) == MP_BINARY_OP_SUBTRACT) { for(size_t i=0; i < out->array->len; i++) odata[i] = avalue[i] - svalue;}\
        if((op) == MP_BINARY_OP_MULTIPLY) { for(size_t i=0; i < out->array->len; i++) odata[i] = avalue[i] * svalue;}\
        return MP_OBJ_FROM_PTR(out);\
    } else if((op) == MP_BINARY_OP_TRUE_DIVIDE) {\
        float value = (float)scalar;\
        ndarray_obj_t *out = create_new_ndarray((ndarray)->m, (ndarray)->n, NDARRAY_FLOAT);\
        float *odata = (float *)out->array->items;\
        for(size_t i=0; i < out->array->len; i++) odata[i] = (float)avalue[i]/value;\
        return MP_OBJ_FROM_PTR(out);\
    } else if(((op) == MP_BINARY_OP_LESS) || ((op) == MP_BINARY_OP_LESS_EQUAL) ||  \
             ((op) == MP_BINARY_OP_MORE) || ((op) == MP_BINARY_OP_MORE_EQUAL)) {\
        mp_obj_t out_list = mp_obj_new_list(0, NULL);\
        size_t m = (ndarray)->m, n = (ndarray)->n;\
        float value = (float)scalar;\
        for(size_t i=0; i < m; i++) {\
            mp_obj_t row = mp_obj_new_list(n, NULL);\
            mp_obj_list_t *row_ptr = MP_OBJ_TO_PTR(row);\
            for(size_t j=0; j < n; j++) {\
                row_ptr->items[j] = mp_const_false;\
                if((op) == MP_BINARY_OP_LESS) {\
                    if(avalue[i*n+j] < value) row_ptr->items[j] = mp_const_true;\
                } else if((op) == MP_BINARY_OP_LESS_EQUAL) {\
                    if(avalue[i*n+j] <= value) row_ptr->items[j] = mp_const_true;\
                } else if((op) == MP_BINARY_OP_MORE) {\
                    if(avalue[i*n+j] > value) row_ptr->items[j] = mp_const_true;\
                } else if((op) == MP_BINARY_OP_MORE_EQUAL) {\
                    if(avalue[i*n+j] >= value) row_ptr->items[j] = mp_const_true;\
                }\
            }\
            if(m == 1) return row;\
            mp_obj_list_append(out_list, row);\
        }\
        return out_list;\
    }\
} while(0)

/*  
    mp_obj_t row = mp_obj_new_list(n, NULL);
    mp_obj_list_t *row_ptr = MP_OBJ_TO_PTR(row);
    
    should work outside the loop, but it doesn't. Go figure! */

#define RUN_BINARY_COMPARE(type_array, ndarray, svalue, op) do {\
    mp_obj_t out_list = mp_obj_new_list(0, NULL);\
    size_t m = (ndarray)->m, n = (ndarray)->n;\
    type_array *avalue = (type_array *)(ndarray)->array->items;\
    for(size_t i=0; i < m; i++) {\
        mp_obj_t row = mp_obj_new_list(n, NULL);\
        mp_obj_list_t *row_ptr = MP_OBJ_TO_PTR(row);\
        for(size_t j=0; j < n; j++) {\
            row_ptr->items[j] = mp_const_false;\
            if((op) == MP_BINARY_OP_LESS) {\
                if(avalue[i*n+j] < svalue) row_ptr->items[j] = mp_const_true;\
            } else if((op) == MP_BINARY_OP_LESS_EQUAL) {\
                if(avalue[i*n+j] <= svalue) row_ptr->items[j] = mp_const_true;\
            } else if((op) == MP_BINARY_OP_MORE) {\
                if(avalue[i*n+j] > svalue) row_ptr->items[j] = mp_const_true;\
            } else if((op) == MP_BINARY_OP_MORE_EQUAL) {\
                if(avalue[i*n+j] >= svalue) row_ptr->items[j] = mp_const_true;\
            }\
        }\
        if(m == 1) return row;\
        mp_obj_list_append(out_list, row);\
    }\
    return out_list;\
} while(0)

#define RUN_BINARY_LOOP(typecode, type_out, type_left, type_right, ol, or, op) do {\
    type_left *left = (type_left *)(ol)->array->items;\
    type_right *right = (type_right *)(or)->array->items;\
    if(((op) == MP_BINARY_OP_ADD) || ((op) == MP_BINARY_OP_SUBTRACT) || ((op) == MP_BINARY_OP_MULTIPLY)) {\
        ndarray_obj_t *out = create_new_ndarray(ol->m, ol->n, typecode);\
        type_out *(odata) = (type_out *)out->array->items;\
        if((op) == MP_BINARY_OP_ADD) { for(size_t i=0; i < (ol)->array->len; i++) odata[i] = left[i] + right[i];}\
        if((op) == MP_BINARY_OP_SUBTRACT) { for(size_t i=0; i < (ol)->array->len; i++) odata[i] = left[i] - right[i];}\
        if((op) == MP_BINARY_OP_MULTIPLY) { for(size_t i=0; i < (ol)->array->len; i++) odata[i] = left[i] * right[i];}\
        return MP_OBJ_FROM_PTR(out);\
    } else if((op) == MP_BINARY_OP_TRUE_DIVIDE) {\
        ndarray_obj_t *out = create_new_ndarray(ol->m, ol->n, NDARRAY_FLOAT);\
        float *odata = (float *)out->array->items;\
        for(size_t i=0; i < (ol)->array->len; i++) odata[i] = (float)left[i]/(float)right[i];\
        return MP_OBJ_FROM_PTR(out);\
    } else if(((op) == MP_BINARY_OP_LESS) || ((op) == MP_BINARY_OP_LESS_EQUAL) ||  \
             ((op) == MP_BINARY_OP_MORE) || ((op) == MP_BINARY_OP_MORE_EQUAL)) {\
        mp_obj_t out_list = mp_obj_new_list(0, NULL);\
        size_t m = (ol)->m, n = (ol)->n;\
        for(size_t i=0; i < m; i++) {\
            mp_obj_t row = mp_obj_new_list(n, NULL);\
            mp_obj_list_t *row_ptr = MP_OBJ_TO_PTR(row);\
            for(size_t j=0; j < n; j++) {\
                row_ptr->items[j] = mp_const_false;\
                if((op) == MP_BINARY_OP_LESS) {\
                    if(left[i*n+j] < right[i*n+j]) row_ptr->items[j] = mp_const_true;\
                } else if((op) == MP_BINARY_OP_LESS_EQUAL) {\
                    if(left[i*n+j] <= right[i*n+j]) row_ptr->items[j] = mp_const_true;\
                } else if((op) == MP_BINARY_OP_MORE) {\
                    if(left[i*n+j] > right[i*n+j]) row_ptr->items[j] = mp_const_true;\
                } else if((op) == MP_BINARY_OP_MORE_EQUAL) {\
                    if(left[i*n+j] >= right[i*n+j]) row_ptr->items[j] = mp_const_true;\
                }\
            }\
            if(m == 1) return row;\
            mp_obj_list_append(out_list, row);\
        }\
        return out_list;\
    }\
} while(0)

enum NDARRAY_TYPE {
    NDARRAY_UINT8 = 'B',
    NDARRAY_INT8 = 'b',
    NDARRAY_UINT16 = 'H', 
    NDARRAY_INT16 = 'h',
    NDARRAY_FLOAT = 'f',
};

typedef struct _ndarray_obj_t {
    mp_obj_base_t base;
    size_t m, n;
    size_t len;
    mp_obj_array_t *array;
    size_t bytes;
} ndarray_obj_t;

mp_obj_t mp_obj_new_ndarray_iterator(mp_obj_t , size_t , mp_obj_iter_buf_t *);

float ndarray_get_float_value(void *, uint8_t , size_t );
void fill_array_iterable(float *, mp_obj_t );

void ndarray_print_row(const mp_print_t *, mp_obj_array_t *, size_t , size_t );
void ndarray_print(const mp_print_t *, mp_obj_t , mp_print_kind_t );
void ndarray_assign_elements(mp_obj_array_t *, mp_obj_t , uint8_t , size_t *);
ndarray_obj_t *create_new_ndarray(size_t , size_t , uint8_t );

mp_obj_t ndarray_copy(mp_obj_t );
mp_obj_t ndarray_make_new(const mp_obj_type_t *, size_t , size_t , const mp_obj_t *);
mp_obj_t ndarray_subscr(mp_obj_t , mp_obj_t , mp_obj_t );
mp_obj_t ndarray_getiter(mp_obj_t , mp_obj_iter_buf_t *);
mp_obj_t ndarray_binary_op(mp_binary_op_t , mp_obj_t , mp_obj_t );
mp_obj_t ndarray_unary_op(mp_unary_op_t , mp_obj_t );

mp_obj_t ndarray_shape(mp_obj_t );
mp_obj_t ndarray_size(mp_obj_t , mp_obj_t );
mp_obj_t ndarray_rawsize(mp_obj_t );
mp_obj_t ndarray_flatten(size_t , const mp_obj_t *, mp_map_t *);

#endif

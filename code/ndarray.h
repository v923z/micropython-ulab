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

#include "py/objarray.h" // this can in the future be dropped
#include "py/binary.h"   // this can in the future be dropped
#include "py/objstr.h"

#define PRINT_MAX  10


const mp_obj_type_t ulab_ndarray_type;

#define RUN_BINARY_LOOP(typecode, type_out, type_left, type_right, ol, or, op) do {\
    ndarray_obj_t *out = create_new_ndarray(ol->m, ol->n, typecode);\
    type_out *(odata) = (type_out *)out->data->items;\
    type_left *left = (type_left *)(ol)->data->items;\
    type_right *right = (type_right *)(or)->data->items;\
    if((op) == MP_BINARY_OP_ADD) { for(size_t i=0; i < (ol)->data->len; i++) odata[i] = left[i] + right[i];}\
    if((op) == MP_BINARY_OP_SUBTRACT) { for(size_t i=0; i < (ol)->data->len; i++) odata[i] = left[i] - right[i];}\
    if((op) == MP_BINARY_OP_MULTIPLY) { for(size_t i=0; i < (ol)->data->len; i++) odata[i] = left[i] * right[i];}\
    if((op) == MP_BINARY_OP_TRUE_DIVIDE) { for(size_t i=0; i < (ol)->data->len; i++) odata[i] = left[i] / right[i];}\
    return MP_OBJ_FROM_PTR(out);\
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
    mp_obj_array_t *data;
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

#endif

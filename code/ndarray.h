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


enum NDARRAY_TYPE {
    NDARRAY_UINT8 = 'b',
    NDARRAY_INT8 = 'B',
    NDARRAY_UINT16 = 'i', 
    NDARRAY_INT16 = 'I',
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
void ndarray_print_row(const mp_print_t *, mp_obj_array_t *, size_t , size_t );
void ndarray_print(const mp_print_t *, mp_obj_t , mp_print_kind_t );
void ndarray_assign_elements(mp_obj_array_t *, mp_obj_t , uint8_t , size_t *);
ndarray_obj_t *create_new_ndarray(size_t , size_t , uint8_t );

mp_obj_t ndarray_copy(mp_obj_t );
mp_obj_t ndarray_make_new(const mp_obj_type_t *, size_t , size_t , const mp_obj_t *);
mp_obj_t ndarray_subscr(mp_obj_t , mp_obj_t , mp_obj_t );
mp_obj_t ndarray_getiter(mp_obj_t , mp_obj_iter_buf_t *);
mp_obj_t ndarray_binary_op(mp_binary_op_t , mp_obj_t , mp_obj_t );


mp_obj_t ndarray_shape(mp_obj_t );
mp_obj_t ndarray_size(mp_obj_t , mp_obj_t );
mp_obj_t ndarray_rawsize(mp_obj_t );

#endif

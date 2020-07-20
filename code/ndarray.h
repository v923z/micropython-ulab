
/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#ifndef _NDARRAY_
#define _NDARRAY_

#include "py/objarray.h"
#include "py/binary.h"
#include "py/objstr.h"
#include "py/objlist.h"

#define NDARRAY_PRINT_THRESHOLD  	10
#define NDARRAY_PRINT_EDGEITEMS		3

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define FLOAT_TYPECODE 'f'
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define FLOAT_TYPECODE 'd'
#endif

// this typedef is lifted from objfloat.c, because mp_obj_float_t is not exposed
typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;

#ifdef OPENMV
#define mp_obj_is_bool(o) (MP_OBJ_IS_TYPE((o), &mp_type_bool))
#define translate(x) x

typedef struct _mp_obj_slice_t {
    mp_obj_base_t base;
    mp_obj_t start;
    mp_obj_t stop;
    mp_obj_t step;
} mp_obj_slice_t;

void mp_obj_slice_get(mp_obj_t self_in, mp_obj_t *, mp_obj_t *, mp_obj_t *);
#else
#if CIRCUITPY
#define mp_obj_is_bool(o) (MP_OBJ_IS_TYPE((o), &mp_type_bool))
#define mp_obj_is_int(x) (MP_OBJ_IS_INT((x)))
#else
#define translate(x) MP_ERROR_TEXT(x)
#endif
#endif

#define SWAP(t, a, b) { t tmp = a; a = b; b = tmp; }

extern const mp_obj_type_t ulab_ndarray_type;

enum NDARRAY_TYPE {
    NDARRAY_UINT8 = 'B',
    NDARRAY_INT8 = 'b',
    NDARRAY_UINT16 = 'H', 
    NDARRAY_INT16 = 'h',
    NDARRAY_FLOAT = FLOAT_TYPECODE,
};

typedef struct _ndarray_obj_t {
    mp_obj_base_t base;
    size_t m, n;
    size_t len;
    mp_obj_array_t *array;
    size_t bytes;
} ndarray_obj_t;

mp_obj_t mp_obj_new_ndarray_iterator(mp_obj_t , size_t , mp_obj_iter_buf_t *);

mp_float_t ndarray_get_float_value(void *, uint8_t , size_t );
bool ndarray_object_is_nditerable(mp_obj_t );
void fill_array_iterable(mp_float_t *, mp_obj_t );

mp_obj_t ndarray_set_printoptions(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(ndarray_set_printoptions_obj);
mp_obj_t ndarray_get_printoptions(void);
MP_DECLARE_CONST_FUN_OBJ_0(ndarray_get_printoptions_obj);
void ndarray_print_row(const mp_print_t *, mp_obj_array_t *, size_t , size_t );
void ndarray_print(const mp_print_t *, mp_obj_t , mp_print_kind_t );
void ndarray_assign_elements(mp_obj_array_t *, mp_obj_t , uint8_t , size_t *);
ndarray_obj_t *create_new_ndarray(size_t , size_t , uint8_t );

mp_obj_t ndarray_copy(mp_obj_t );
#ifdef CIRCUITPY
mp_obj_t ndarray_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args);
#else
mp_obj_t ndarray_make_new(const mp_obj_type_t *, size_t , size_t , const mp_obj_t *);
#endif
mp_obj_t ndarray_subscr(mp_obj_t , mp_obj_t , mp_obj_t );
mp_obj_t ndarray_getiter(mp_obj_t , mp_obj_iter_buf_t *);
mp_obj_t ndarray_binary_op(mp_binary_op_t , mp_obj_t , mp_obj_t );
mp_obj_t ndarray_unary_op(mp_unary_op_t , mp_obj_t );

mp_obj_t ndarray_shape(mp_obj_t );
mp_obj_t ndarray_size(mp_obj_t );
mp_obj_t ndarray_itemsize(mp_obj_t );
mp_obj_t ndarray_flatten(size_t , const mp_obj_t *, mp_map_t *);

mp_obj_t ndarray_reshape(mp_obj_t , mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_2(ndarray_reshape_obj);

mp_obj_t ndarray_transpose(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(ndarray_transpose_obj);

mp_int_t ndarray_get_buffer(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags);
//void ndarray_attributes(mp_obj_t , qstr , mp_obj_t *);

ndarray_obj_t *ndarray_from_mp_obj(mp_obj_t );

#define CREATE_SINGLE_ITEM(outarray, type, typecode, value) do {\
    ndarray_obj_t *tmp = create_new_ndarray(1, 1, (typecode));\
    type *tmparr = (type *)tmp->array->items;\
    tmparr[0] = (type)(value);\
    (outarray) = MP_OBJ_FROM_PTR(tmp);\
} while(0)

/*  
    mp_obj_t row = mp_obj_new_list(n, NULL);
    mp_obj_list_t *row_ptr = MP_OBJ_TO_PTR(row);
    
    should work outside the loop, but it doesn't. Go figure! 
*/

#define RUN_BINARY_LOOP(typecode, type_out, type_left, type_right, ol, or, op, m, n, len, linc, rinc) do {\
    type_left *left = (type_left *)(ol)->array->items;\
    type_right *right = (type_right *)(or)->array->items;\
    if(((op) == MP_BINARY_OP_ADD) || ((op) == MP_BINARY_OP_SUBTRACT) || ((op) == MP_BINARY_OP_MULTIPLY) || ((op) == MP_BINARY_OP_POWER)) {\
        ndarray_obj_t *out = create_new_ndarray((m), (n), (typecode));\
        type_out *(odata) = (type_out *)out->array->items;\
        if((op) == MP_BINARY_OP_ADD) { for(size_t i=0; i < (len); i++, left+=linc, right+=rinc) *odata++ = *left + *right; }\
		else if((op) == MP_BINARY_OP_MULTIPLY) { for(size_t i=0; i < (len); i++, left+=linc, right+=rinc) *odata++ = *left * *right; }\
        else if((op) == MP_BINARY_OP_POWER) { for(size_t i=0; i < (len); i++, left+=linc, right+=rinc) *odata++ = (type_out)MICROPY_FLOAT_C_FUN(pow)(*left, *right); }\
        else if((op) == MP_BINARY_OP_SUBTRACT) { for(size_t i=0; i < (len); i++, left+=linc, right+=rinc) *odata++ = *left - *right; }\
        return MP_OBJ_FROM_PTR(out);\
	} else if((op) == MP_BINARY_OP_TRUE_DIVIDE) {\
        ndarray_obj_t *out = create_new_ndarray((m), (n), NDARRAY_FLOAT);\
        mp_float_t *odata = (mp_float_t *)out->array->items;\
        for(size_t i=0; i < (len); i++, left+=linc, right+=rinc) {*odata++ = (mp_float_t)(*left)/(mp_float_t)(*right);}\
        return MP_OBJ_FROM_PTR(out);\
	} else if(((op) == MP_BINARY_OP_LESS) || ((op) == MP_BINARY_OP_LESS_EQUAL) ||  \
             ((op) == MP_BINARY_OP_MORE) || ((op) == MP_BINARY_OP_MORE_EQUAL) || \
             ((op) == MP_BINARY_OP_EQUAL) || ((op) == MP_BINARY_OP_NOT_EQUAL)) {\
        mp_obj_t out_list = mp_obj_new_list(0, NULL);\
        for(size_t i=0; i < m; i++) {\
            mp_obj_t row = mp_obj_new_list(n, NULL);\
            mp_obj_list_t *row_ptr = MP_OBJ_TO_PTR(row);\
			if((op) == MP_BINARY_OP_LESS) { for(size_t j=0; j < n; j++, left+=linc, right+=rinc) row_ptr->items[j] = *left < *right ? mp_const_true : mp_const_false; }\
			else if((op) == MP_BINARY_OP_LESS_EQUAL) { for(size_t j=0; j < n; j++, left+=linc, right+=rinc) row_ptr->items[j] = *left <= *right ? mp_const_true : mp_const_false; }\
			else if((op) == MP_BINARY_OP_MORE) { for(size_t j=0; j < n; j++, left+=linc, right+=rinc) row_ptr->items[j] = *left > *right ? mp_const_true : mp_const_false; }\
			else if((op) == MP_BINARY_OP_MORE_EQUAL) { for(size_t j=0; j < n; j++, left+=linc, right+=rinc) row_ptr->items[j] = *left >= *right ? mp_const_true : mp_const_false; }\
			else if((op) == MP_BINARY_OP_EQUAL) { for(size_t j=0; j < n; j++, left+=linc, right+=rinc) row_ptr->items[j] = *left == *right ? mp_const_true : mp_const_false; }\
			else if((op) == MP_BINARY_OP_NOT_EQUAL) { for(size_t j=0; j < n; j++, left+=linc, right+=rinc) row_ptr->items[j] = *left != *right ? mp_const_true : mp_const_false; }\
            if(m == 1) return row;\
            mp_obj_list_append(out_list, row);\
		}\
        return out_list;\
    }\
} while(0)

#endif

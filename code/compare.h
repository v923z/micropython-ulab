
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Zoltán Vörös
*/

#ifndef _COMPARE_
#define _COMPARE_

#include "ulab.h"
#include "ndarray.h"

#if ULAB_COMPARE_MODULE

enum COMPARE_FUNCTION_TYPE {
    COMPARE_MINIMUM,
    COMPARE_MAXIMUM,
    COMPARE_CLIP,    
};

extern mp_obj_module_t ulab_compare_module;

MP_DECLARE_CONST_FUN_OBJ_2(compare_minimum_obj);
MP_DECLARE_CONST_FUN_OBJ_2(compare_maximum_obj);
MP_DECLARE_CONST_FUN_OBJ_3(compare_clip_obj);

#define RUN_COMPARE_LOOP(typecode, type_out, type1, type2, nd1, nd2, op, m, n, len, inc1, inc2) do {\
	type1 *array1 = (type1 *)(nd1)->array->items;\
    type2 *array2 = (type2 *)(nd2)->array->items;\
	ndarray_obj_t *out = create_new_ndarray((m), (n), (typecode));\
	type_out *(odata) = (type_out *)out->array->items;\
	if((op) == COMPARE_MINIMUM) { for(size_t i=0; i < (len); i++, array1+=(inc1), array2+=(inc2)) *odata++ = *array1 < *array2 ? *array1 : *array2; }\
	if((op) == COMPARE_MAXIMUM) { for(size_t i=0; i < (len); i++, array1+=(inc1), array2+=(inc2)) *odata++ = *array1 > *array2 ? *array1 : *array2; }\
	return MP_OBJ_FROM_PTR(out);\
} while(0)

#endif
#endif

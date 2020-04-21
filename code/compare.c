
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Zoltán Vörös
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "compare.h"

#if ULAB_COMPARE_MODULE

static mp_obj_t compare_function(mp_obj_t x1, mp_obj_t x2, uint8_t comptype) {
	// the function is implemented for scalars and ndarrays only, with partial 
	// broadcasting: arguments must be either scalars, or ndarrays of equal size/shape
	if(!(MP_OBJ_IS_INT(x1) || mp_obj_is_float(x1) || MP_OBJ_IS_TYPE(x1, &ulab_ndarray_type)) &&
		!(MP_OBJ_IS_INT(x2) || mp_obj_is_float(x2) || MP_OBJ_IS_TYPE(x2, &ulab_ndarray_type))) {
		mp_raise_TypeError(translate("function is implemented for scalars and ndarrays only"));
	}
	ndarray_obj_t *ndarray1 = ndarray_from_mp_obj(x1);
	ndarray_obj_t *ndarray2 = ndarray_from_mp_obj(x2); 
	// check, whether partial broadcasting is possible here
	if((ndarray1->m != ndarray2->m) || (ndarray1->n != ndarray2->n)) {
		if((ndarray1->array->len != 1) && (ndarray2->array->len != 1)) {
            mp_raise_ValueError(translate("operands could not be broadcast together"));
		}
	}
	size_t m = MAX(ndarray1->m, ndarray2->m);
	size_t n = MAX(ndarray1->n, ndarray2->n);
	size_t len = MAX(ndarray1->array->len, ndarray2->array->len);
	size_t inc1 = ndarray1->array->len == 1 ? 0 : 1;
	size_t inc2 = ndarray2->array->len == 1 ? 0 : 1;
	// These are the upcasting rules
	// float always becomes float
	// operation on identical types preserves type
	// uint8 + int8 => int16
	// uint8 + int16 => int16
	// uint8 + uint16 => uint16
	// int8 + int16 => int16
	// int8 + uint16 => uint16
	// uint16 + int16 => float
	// The parameters of RUN_BINARY_LOOP are 
	// typecode of result, type_out, type_left, type_right, lhs operand, rhs operand, operator
	if(ndarray1->array->typecode == NDARRAY_UINT8) {
		if(ndarray2->array->typecode == NDARRAY_UINT8) {
			RUN_COMPARE_LOOP(NDARRAY_UINT8, uint8_t, uint8_t, uint8_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_INT8) {
			RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, uint8_t, int8_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_UINT16) {
			RUN_COMPARE_LOOP(NDARRAY_UINT16, uint16_t, uint8_t, uint16_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_INT16) {
			RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, uint8_t, int16_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_FLOAT) {
			RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, uint8_t, mp_float_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		}
	} else if(ndarray1->array->typecode == NDARRAY_INT8) {
		if(ndarray2->array->typecode == NDARRAY_UINT8) {
			RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, int8_t, uint8_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_INT8) {
			RUN_COMPARE_LOOP(NDARRAY_INT8, int8_t, int8_t, int8_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_UINT16) {
			RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, int8_t, uint16_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_INT16) {
			RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, int8_t, int16_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_FLOAT) {
			RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, int8_t, mp_float_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		}                
	} else if(ndarray1->array->typecode == NDARRAY_UINT16) {
		if(ndarray2->array->typecode == NDARRAY_UINT8) {
			RUN_COMPARE_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, uint8_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_INT8) {
			RUN_COMPARE_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, int8_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_UINT16) {
			RUN_COMPARE_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, uint16_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_INT16) {
			RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, uint16_t, int16_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_FLOAT) {
			RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, uint8_t, mp_float_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		}
	} else if(ndarray1->array->typecode == NDARRAY_INT16) {
		if(ndarray2->array->typecode == NDARRAY_UINT8) {
			RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, int16_t, uint8_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_INT8) {
			RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, int16_t, int8_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_UINT16) {
			RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, int16_t, uint16_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_INT16) {
			RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, int16_t, int16_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_FLOAT) {
			RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, uint16_t, mp_float_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		}
	} else if(ndarray1->array->typecode == NDARRAY_FLOAT) {
		if(ndarray2->array->typecode == NDARRAY_UINT8) {
			RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, uint8_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_INT8) {
			RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, int8_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_UINT16) {
			RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, uint16_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_INT16) {
			RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, int16_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		} else if(ndarray2->array->typecode == NDARRAY_FLOAT) {
			RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, mp_float_t, ndarray1, ndarray2, comptype, m, n, len, inc1, inc2);
		}
	}
    return mp_const_none; // we should never reach this point
}

static mp_obj_t compare_minimum(mp_obj_t x1, mp_obj_t x2) {
	// extra round, so that we can return minimum(3, 4) properly
	mp_obj_t result = compare_function(x1, x2, COMPARE_MINIMUM);
	if((MP_OBJ_IS_INT(x1) || mp_obj_is_float(x1)) && (MP_OBJ_IS_INT(x2) || mp_obj_is_float(x2))) {
		ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(result);
		return mp_binary_get_val_array(ndarray->array->typecode, ndarray->array->items, 0);
	}
	return result;
}

MP_DEFINE_CONST_FUN_OBJ_2(compare_minimum_obj, compare_minimum);

static mp_obj_t compare_maximum(mp_obj_t x1, mp_obj_t x2) {
	// extra round, so that we can return maximum(3, 4) properly
	mp_obj_t result = compare_function(x1, x2, COMPARE_MAXIMUM);
	if((MP_OBJ_IS_INT(x1) || mp_obj_is_float(x1)) && (MP_OBJ_IS_INT(x2) || mp_obj_is_float(x2))) {
		ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(result);
		return mp_binary_get_val_array(ndarray->array->typecode, ndarray->array->items, 0);
	}
	return result;
}

MP_DEFINE_CONST_FUN_OBJ_2(compare_maximum_obj, compare_maximum);

static mp_obj_t compare_clip(mp_obj_t x1, mp_obj_t x2, mp_obj_t x3) {
	// Note: this function could be made faster by implementing a single-loop comparison in 
	// RUN_COMPARE_LOOP. However, that would add around 2 kB of compile size, while we 
	// would not gain a factor of two in speed, since the two comparisons should still be 
	// evaluated. In contrast, calling the function twice adds only 140 bytes to the firmware
	return compare_function(x2, compare_function(x1, x3, COMPARE_MINIMUM), COMPARE_MAXIMUM);
}

MP_DEFINE_CONST_FUN_OBJ_3(compare_clip_obj, compare_clip);

STATIC const mp_rom_map_elem_t ulab_compare_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_compare) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_maximum), (mp_obj_t)&compare_maximum_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_minimum), (mp_obj_t)&compare_minimum_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_clip), (mp_obj_t)&compare_clip_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_compare_globals, ulab_compare_globals_table);

mp_obj_module_t ulab_compare_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_compare_globals,
};

#endif

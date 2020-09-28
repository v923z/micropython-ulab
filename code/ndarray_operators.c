#include <math.h>

#include "py/objtuple.h"
#include "ndarray.h"
#include "ndarray_operators.h"
#include "ulab.h"

/*
	This file contains the actual implementations of the various
	ndarray operators.

  	These are the upcasting rules of the binary operators

	- if one of the operarands is a float, the result is always float
	- operation on identical types preserves type

	uint8 + int8 => int16
	uint8 + int16 => int16
	uint8 + uint16 => uint16
	int8 + int16 => int16
	int8 + uint16 => uint16
	uint16 + int16 => float
*/

#if NDARRAY_HAS_BINARY_OP_EQUAL | NDARRAY_HAS_BINARY_OP_NOT_EQUAL
ndarray_obj_t *ndarray_binary_equality(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                                            uint8_t ndim, size_t *shape,  int32_t *lstrides, int32_t *rstrides, mp_binary_op_t op) {

    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT8);
    uint8_t *array = (uint8_t *)results->array;
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    #if NDARRAY_HAS_BINARY_OP_EQUAL
    if(op == MP_BINARY_OP_EQUAL) {
        if(lhs->dtype == NDARRAY_UINT8) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, uint8_t, int8_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint8_t, int16_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, ==);
            }
        } else if(lhs->dtype == NDARRAY_INT8) {
            if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, int8_t, int8_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, int8_t, uint16_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int8_t, int16_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, ==);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        } else if(lhs->dtype == NDARRAY_UINT16) {
            if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint16_t, int16_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, ==);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        } else if(lhs->dtype == NDARRAY_INT16) {
            if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int16_t, int16_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, int16_t, mp_float_t, larray, lstrides, rarray, rstrides, ==);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        } else if(lhs->dtype == NDARRAY_FLOAT) {
            if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, ==);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        }
    }
    #endif /* NDARRAY_HAS_BINARY_OP_EQUAL */

    #if NDARRAY_HAS_BINARY_OP_NOT_EQUAL
    if(op == MP_BINARY_OP_NOT_EQUAL) {
        if(lhs->dtype == NDARRAY_UINT8) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, uint8_t, int8_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint8_t, int16_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, !=);
            }
        } else if(lhs->dtype == NDARRAY_INT8) {
            if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, int8_t, int8_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, int8_t, uint16_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int8_t, int16_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, !=);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        } else if(lhs->dtype == NDARRAY_UINT16) {
            if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint16_t, int16_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, !=);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        } else if(lhs->dtype == NDARRAY_INT16) {
            if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int16_t, int16_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, int16_t, mp_float_t, larray, lstrides, rarray, rstrides, !=);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        } else if(lhs->dtype == NDARRAY_FLOAT) {
            if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, !=);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        }
    }
    #endif /* NDARRAY_HAS_BINARY_OP_NOT_EQUAL */

    return MP_OBJ_FROM_PTR(results);
}
#endif /* NDARRAY_HAS_BINARY_OP_EQUAL | NDARRAY_HAS_BINARY_OP_NOT_EQUAL */

#if NDARRAY_HAS_BINARY_OP_ADD
ndarray_obj_t *ndarray_binary_add(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                                        uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {

    ndarray_obj_t *results = NULL;
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    if(lhs->dtype == NDARRAY_UINT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, uint8_t, int8_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, uint8_t, int16_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, +);
        }
    } else if(lhs->dtype == NDARRAY_INT8) {
        if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT8);
            BINARY_LOOP(results, int8_t, int8_t, int8_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int8_t, uint16_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int8_t, int16_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, +);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_ADD, rhs, lhs);
        }
    } else if(lhs->dtype == NDARRAY_UINT16) {
        if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint16_t, int16_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, +);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_ADD, rhs, lhs);
        }
    } else if(lhs->dtype == NDARRAY_INT16) {
        if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int16_t, int16_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, int16_t, mp_float_t, larray, lstrides, rarray, rstrides, +);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_ADD, rhs, lhs);
        }
    } else if(lhs->dtype == NDARRAY_FLOAT) {
        if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, +);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_ADD, rhs, lhs);
        }
    }

    return MP_OBJ_FROM_PTR(results);
}
#endif /* NDARRAY_HAS_BINARY_OP_ADD */

#if NDARRAY_HAS_BINARY_OP_MULTIPLY
ndarray_obj_t *ndarray_binary_multiply(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {

    ndarray_obj_t *results = NULL;
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    if(lhs->dtype == NDARRAY_UINT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, uint8_t, int8_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, uint8_t, int16_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, *);
        }
    } else if(lhs->dtype == NDARRAY_INT8) {
        if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT8);
            BINARY_LOOP(results, int8_t, int8_t, int8_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int8_t, uint16_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int8_t, int16_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, *);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_MULTIPLY, rhs, lhs);
        }
    } else if(lhs->dtype == NDARRAY_UINT16) {
        if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint16_t, int16_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, *);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_MULTIPLY, rhs, lhs);
        }
    } else if(lhs->dtype == NDARRAY_INT16) {
        if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int16_t, int16_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, int16_t, mp_float_t, larray, lstrides, rarray, rstrides, *);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_MULTIPLY, rhs, lhs);
        }
    } else if(lhs->dtype == NDARRAY_FLOAT) {
        if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, *);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_MULTIPLY, rhs, lhs);
        }
    }

    return MP_OBJ_FROM_PTR(results);
}
#endif /* NDARRAY_HAS_BINARY_OP_MULTIPLY */

#if NDARRAY_HAS_BINARY_OP_MORE | NDARRAY_HAS_BINARY_OP_MORE_EQUAL | NDARRAY_HAS_BINARY_OP_LESS | NDARRAY_HAS_BINARY_OP_LESS_EQUAL
ndarray_obj_t *ndarray_binary_more(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides, mp_binary_op_t op) {

    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT8);
    uint8_t *array = (uint8_t *)results->array;
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    #if NDARRAY_HAS_BINARY_OP_MORE | NDARRAY_HAS_BINARY_OP_LESS
    if(op == MP_BINARY_OP_MORE) {
        if(lhs->dtype == NDARRAY_UINT8) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, uint8_t, int8_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint8_t, int16_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, <);
            }
        } else if(lhs->dtype == NDARRAY_INT8) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, int8_t, uint8_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, int8_t, int8_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, int8_t, uint16_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int8_t, int16_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, <);
            }
        } else if(lhs->dtype == NDARRAY_UINT16) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, uint16_t, uint8_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, uint16_t, int8_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint16_t, int16_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, <);
            }
        } else if(lhs->dtype == NDARRAY_INT16) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, int16_t, uint8_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, int16_t, int8_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, int16_t, uint16_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int16_t, int16_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, <);
            }
        } else if(lhs->dtype == NDARRAY_FLOAT) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, mp_float_t, uint8_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, mp_float_t, int8_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, mp_float_t, uint16_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, mp_float_t, int16_t, larray, lstrides, rarray, rstrides, <);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, <);
            }
        }
    }
    #endif /* NDARRAY_HAS_BINARY_OP_MORE | NDARRAY_HAS_BINARY_OP_LESS*/
    #if NDARRAY_HAS_BINARY_OP_MORE_EQUAL | NDARRAY_HAS_BINARY_OP_LESS_EQUAL
    if(op == MP_BINARY_OP_MORE) {
        if(lhs->dtype == NDARRAY_UINT8) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, uint8_t, int8_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint8_t, int16_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, <=);
            }
        } else if(lhs->dtype == NDARRAY_INT8) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, int8_t, uint8_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, int8_t, int8_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, int8_t, uint16_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int8_t, int16_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, <=);
            }
        } else if(lhs->dtype == NDARRAY_UINT16) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, uint16_t, uint8_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, uint16_t, int8_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint16_t, int16_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, <=);
            }
        } else if(lhs->dtype == NDARRAY_INT16) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, int16_t, uint8_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, int16_t, int8_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, int16_t, uint16_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int16_t, int16_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, <=);
            }
        } else if(lhs->dtype == NDARRAY_FLOAT) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, mp_float_t, uint8_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, mp_float_t, int8_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, mp_float_t, uint16_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, mp_float_t, int16_t, larray, lstrides, rarray, rstrides, <=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, <=);
            }
        }
    }
    #endif /* NDARRAY_HAS_BINARY_OP_MORE_EQUAL | NDARRAY_HAS_BINARY_OP_LESS_EQUAL */

    return MP_OBJ_FROM_PTR(results);
}
#endif /* NDARRAY_HAS_BINARY_OP_MORE | NDARRAY_HAS_BINARY_OP_MORE_EQUAL | NDARRAY_HAS_BINARY_OP_LESS | NDARRAY_HAS_BINARY_OP_LESS_EQUAL */

#if NDARRAY_HAS_BINARY_OP_SUBTRACT
ndarray_obj_t *ndarray_binary_subtract(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {

    ndarray_obj_t *results = NULL;
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    if(lhs->dtype == NDARRAY_UINT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT8);
            BINARY_LOOP(results, uint8_t, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, uint8_t, int8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, uint8_t, int16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, -);
        }
    } else if(lhs->dtype == NDARRAY_INT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int8_t, uint8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT8);
            BINARY_LOOP(results, int8_t, int8_t, int8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int8_t, uint16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int8_t, int16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, -);
        }
    } else if(lhs->dtype == NDARRAY_UINT16) {
        if(rhs->dtype == NDARRAY_UINT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint16_t, uint8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint16_t, int8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint16_t, int16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, -);
        }
    } else if(lhs->dtype == NDARRAY_INT16) {
        if(rhs->dtype == NDARRAY_UINT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int16_t, uint8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int16_t, int8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, int16_t, uint16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int16_t, int16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, -);
        }
    } else if(lhs->dtype == NDARRAY_FLOAT) {
        if(rhs->dtype == NDARRAY_UINT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, mp_float_t, uint8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, mp_float_t, int8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, mp_float_t, uint16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, mp_float_t, int16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, -);
        }
    }

    return MP_OBJ_FROM_PTR(results);
}
#endif /* NDARRAY_HAS_BINARY_OP_SUBTRACT */

#if NDARRAY_HAS_BINARY_OP_TRUE_DIVIDE
ndarray_obj_t *ndarray_binary_true_divide(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {

    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    if(lhs->dtype == NDARRAY_UINT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            BINARY_LOOP(results, mp_float_t, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT8) {
            BINARY_LOOP(results, mp_float_t, uint8_t, int8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            BINARY_LOOP(results, mp_float_t, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT16) {
            BINARY_LOOP(results, mp_float_t, uint8_t, int16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            BINARY_LOOP(results, mp_float_t, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, /);
        }
    } else if(lhs->dtype == NDARRAY_INT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            BINARY_LOOP(results, mp_float_t, int8_t, uint8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT8) {
            BINARY_LOOP(results, mp_float_t, int8_t, int8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            BINARY_LOOP(results, mp_float_t, int8_t, uint16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT16) {
            BINARY_LOOP(results, mp_float_t, int8_t, int16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            BINARY_LOOP(results, mp_float_t, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, /);
        }
    } else if(lhs->dtype == NDARRAY_UINT16) {
        if(rhs->dtype == NDARRAY_UINT8) {
            BINARY_LOOP(results, mp_float_t, uint16_t, uint8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT8) {
            BINARY_LOOP(results, mp_float_t, uint16_t, int8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            BINARY_LOOP(results, mp_float_t, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT16) {
            BINARY_LOOP(results, mp_float_t, uint16_t, int16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            BINARY_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, /);
        }
    } else if(lhs->dtype == NDARRAY_INT16) {
        if(rhs->dtype == NDARRAY_UINT8) {
            BINARY_LOOP(results, mp_float_t, int16_t, uint8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT8) {
            BINARY_LOOP(results, mp_float_t, int16_t, int8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            BINARY_LOOP(results, mp_float_t, int16_t, uint16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT16) {
            BINARY_LOOP(results, mp_float_t, int16_t, int16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            BINARY_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, /);
        }
    } else if(lhs->dtype == NDARRAY_FLOAT) {
        if(rhs->dtype == NDARRAY_UINT8) {
            BINARY_LOOP(results, mp_float_t, mp_float_t, uint8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT8) {
            BINARY_LOOP(results, mp_float_t, mp_float_t, int8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            BINARY_LOOP(results, mp_float_t, mp_float_t, uint16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT16) {
            BINARY_LOOP(results, mp_float_t, mp_float_t, int16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            BINARY_LOOP(results, mp_float_t, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, /);
        }
    }

    return MP_OBJ_FROM_PTR(results);
}
#endif /* NDARRAY_HAS_BINARY_OP_TRUE_DIVIDE */

#if NDARRAY_HAS_BINARY_OP_POWER
ndarray_obj_t *ndarray_binary_power(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {

	// TODO: numpy upcasts the results to int64, if the inputs are of integer type,
	// while we always return a float array. Is upcasting to int16 sensible for integer types?
    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    if(lhs->dtype == NDARRAY_UINT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            POWER_LOOP(results, mp_float_t, uint8_t, uint8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT8) {
            POWER_LOOP(results, mp_float_t, uint8_t, int8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            POWER_LOOP(results, mp_float_t, uint8_t, uint16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT16) {
            POWER_LOOP(results, mp_float_t, uint8_t, int16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            POWER_LOOP(results, mp_float_t, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides);
        }
    } else if(lhs->dtype == NDARRAY_INT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            POWER_LOOP(results, mp_float_t, int8_t, uint8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT8) {
            POWER_LOOP(results, mp_float_t, int8_t, int8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            POWER_LOOP(results, mp_float_t, int8_t, uint16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT16) {
            POWER_LOOP(results, mp_float_t, int8_t, int16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            POWER_LOOP(results, mp_float_t, int8_t, mp_float_t, larray, lstrides, rarray, rstrides);
        }
    } else if(lhs->dtype == NDARRAY_UINT16) {
        if(rhs->dtype == NDARRAY_UINT8) {
            POWER_LOOP(results, mp_float_t, uint16_t, uint8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT8) {
            POWER_LOOP(results, mp_float_t, uint16_t, int8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            POWER_LOOP(results, mp_float_t, uint16_t, uint16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT16) {
            POWER_LOOP(results, mp_float_t, uint16_t, int16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            POWER_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides);
        }
    } else if(lhs->dtype == NDARRAY_INT16) {
        if(rhs->dtype == NDARRAY_UINT8) {
            POWER_LOOP(results, mp_float_t, int16_t, uint8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT8) {
            POWER_LOOP(results, mp_float_t, int16_t, int8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            POWER_LOOP(results, mp_float_t, int16_t, uint16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT16) {
            POWER_LOOP(results, mp_float_t, int16_t, int16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            POWER_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides);
        }
    } else if(lhs->dtype == NDARRAY_FLOAT) {
        if(rhs->dtype == NDARRAY_UINT8) {
            POWER_LOOP(results, mp_float_t, mp_float_t, uint8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT8) {
            POWER_LOOP(results, mp_float_t, mp_float_t, int8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            POWER_LOOP(results, mp_float_t, mp_float_t, uint16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT16) {
            POWER_LOOP(results, mp_float_t, mp_float_t, int16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            POWER_LOOP(results, mp_float_t, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides);
        }
    }

    return MP_OBJ_FROM_PTR(results);
}
#endif /* NDARRAY_HAS_BINARY_OP_POWER */

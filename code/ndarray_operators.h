#include "ndarray.h"

ndarray_obj_t *ndarray_binary_equality(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *,  int32_t *, int32_t *, mp_binary_op_t );
ndarray_obj_t *ndarray_binary_add(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *);
ndarray_obj_t *ndarray_binary_multiply(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *);
ndarray_obj_t *ndarray_binary_more(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *, mp_binary_op_t );
ndarray_obj_t *ndarray_binary_subtract(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *);
ndarray_obj_t *ndarray_binary_true_divide(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *);





/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Zoltán Vörös
*/

#ifndef _CARRAY_
#define _CARRAY_

MP_DECLARE_CONST_FUN_OBJ_1(carray_real_obj);
MP_DECLARE_CONST_FUN_OBJ_1(carray_imag_obj);

mp_obj_t carray_abs(ndarray_obj_t *, ndarray_obj_t *);
void carray_binary_add(ndarray_obj_t *, mp_float_t *, uint8_t *, uint8_t *, int32_t *, int32_t *, uint8_t);

#if ULAB_MAX_DIMS == 1
#define BINARY_LOOP_COMPLEX(results, resarray, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t l = 0;\
    do {\
        *(resarray) = *((mp_float_t *)(larray)) OPERATOR *((type_right *)(rarray));\
        (resarray) += 2;\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\

#endif /* ULAB_MAX_DIMS == 1 */

#if ULAB_MAX_DIMS == 2
#define BINARY_LOOP_COMPLEX(results, resarray, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            *(resarray) = *((mp_float_t *)(larray)) OPERATOR *((type_right *)(rarray));\
            (resarray) += 2;\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\

#endif /* ULAB_MAX_DIMS == 2 */

#if ULAB_MAX_DIMS == 3
#define BINARY_LOOP_COMPLEX(results, resarray, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                *(resarray) = *((mp_float_t *)(larray)) OPERATOR *((type_right *)(rarray));\
                (resarray) += 2;\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\

#endif /* ULAB_MAX_DIMS == 3 */

#if ULAB_MAX_DIMS == 4
#define BINARY_LOOP_COMPLEX(results, resarray, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t i = 0;\
    do {\
        size_t j = 0;\
        do {\
            size_t k = 0;\
            do {\
                size_t l = 0;\
                do {\
                    *(resarray) = *((mp_float_t *)(larray)) OPERATOR *((type_right *)(rarray));\
                    (resarray) += 2;\
                    (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
                    (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                    l++;\
                } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
                (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
                (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
                k++;\
            } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
            j++;\
        } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 4];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (results)->shape[ULAB_MAX_DIMS - 4]);\

#endif /* ULAB_MAX_DIMS == 4 */

#endif

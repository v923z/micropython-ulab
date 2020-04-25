
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
#include "extras.h"

#if ULAB_EXTRAS_MODULE

#include "blas.h"


static mp_obj_t extras_spectrogram(size_t n_args, const mp_obj_t *args) {
    if(n_args == 2) {
        return fft_fft_ifft_spectrum(n_args, args[0], args[1], FFT_SPECTRUM);
    } else {
        return fft_fft_ifft_spectrum(n_args, args[0], mp_const_none, FFT_SPECTRUM);
    }
}

static mp_obj_t extras_blas_saxpy(size_t n_args, const mp_obj_t *args) {
    int        N     = 0;
    mp_float_t alpha = (mp_float_t)0;
    int        incX  = 1;
    int        incY  = 1;
   
    int x_loc = 2;
    int y_loc = 4;

    if(n_args == 6) {
        N     = mp_obj_get_int(args[0]);
        alpha = mp_obj_get_float(args[1]);
        incX  = mp_obj_get_int(args[3]);
        incY  = mp_obj_get_int(args[5]);
    }
    else if(n_args == 5){
        alpha = mp_obj_get_float(args[0]);
        incX  = mp_obj_get_int(args[2]);
        incY  = mp_obj_get_int(args[4]);
        x_loc = 1;
        y_loc = 3;
        ndarray_obj_t *Xptr = MP_OBJ_TO_PTR(args[x_loc]);
        N = Xptr->array->len;

    }
    else if(n_args == 3){
        x_loc = 1;
        y_loc = 2;
        alpha = mp_obj_get_float(args[0]);
        ndarray_obj_t *Xptr = MP_OBJ_TO_PTR(args[x_loc]);
        N = Xptr->array->len;
    }
    else{
        mp_raise_ValueError(translate("usage saxpy(N,alpha,x,inc,y,incy)||saxpy(N,alpha,x,incx,y,incy)||saxpy(N,alpha,x,y)"));
    }

    if(!MP_OBJ_IS_TYPE(args[x_loc], &ulab_ndarray_type) || !MP_OBJ_IS_TYPE(args[y_loc], &ulab_ndarray_type)) {
        mp_raise_NotImplementedError(translate("SAXPY is defined for ndarrays only"));
    } 

    int len;
    ndarray_obj_t *Xptr = MP_OBJ_TO_PTR(args[x_loc]);
    len = Xptr->array->len;
    if((len) != (N*incX)) {
        mp_raise_ValueError(translate("size mismatch X"));
    }

    ndarray_obj_t *Yptr = MP_OBJ_TO_PTR(args[y_loc]);
    len = Yptr->array->len;
    if((len) != (N*incY)) {
        mp_raise_ValueError(translate("size mismatch Y"));
    }    
    if((Xptr->array->typecode != NDARRAY_FLOAT) || (Yptr->array->typecode != NDARRAY_FLOAT)) { 
        mp_raise_ValueError(translate("only for float"));
    }
    mp_float_t *X = (mp_float_t *)Xptr->array->items;
    mp_float_t *Y = (mp_float_t *)Yptr->array->items;
    cblas_axpy(N,alpha,X,incX,Y,incY);

    return args[y_loc];
}

static mp_obj_t extras_blas_sdot(size_t n_args, const mp_obj_t *args) {
    int        N     = 0;
    int        incX  = 1;
    int        incY  = 1;
   
    int x_loc = 1;
    int y_loc = 3;

    if(n_args == 5) {
        N     = mp_obj_get_int(args[0]);
        incX  = mp_obj_get_int(args[2]);
        incY  = mp_obj_get_int(args[4]);
    }
    else if(n_args == 4) {
        incX  = mp_obj_get_int(args[1]);
        incY  = mp_obj_get_int(args[3]);
        x_loc = 0;
        y_loc = 2;
        ndarray_obj_t *Xptr = MP_OBJ_TO_PTR(args[x_loc]);
        N = Xptr->array->len;
    }
    else if(n_args == 2){
        x_loc = 0;
        y_loc = 1;
        ndarray_obj_t *Xptr = MP_OBJ_TO_PTR(args[x_loc]);
        N = Xptr->array->len;

    }
    else{
        mp_raise_ValueError(translate("usage sdot(N,x,inc,y,incy)||sdot(x,incx,y,incy)||sdot(x,y)"));
    }

    if(!MP_OBJ_IS_TYPE(args[x_loc], &ulab_ndarray_type) || !MP_OBJ_IS_TYPE(args[y_loc], &ulab_ndarray_type)) {
        mp_raise_NotImplementedError(translate("SAXPY is defined for ndarrays only"));
    } 


    int len;
    ndarray_obj_t *Xptr = MP_OBJ_TO_PTR(args[x_loc]);
    len = Xptr->array->len;
    if((len) != (N*incX)) {
        mp_raise_ValueError(translate("size mismatch X"));
    }

    ndarray_obj_t *Yptr = MP_OBJ_TO_PTR(args[y_loc]);
    len = Yptr->array->len;
    if((len) != (N*incY)) {
        mp_raise_ValueError(translate("size mismatch Y"));
    }    
    if((Xptr->array->typecode != NDARRAY_FLOAT) || (Yptr->array->typecode != NDARRAY_FLOAT)) { 
        mp_raise_ValueError(translate("only for float"));
    }
    mp_float_t *X = (mp_float_t *)Xptr->array->items;
    mp_float_t *Y = (mp_float_t *)Yptr->array->items;
    return mp_obj_new_float(cblas_dot(N,X,incX,Y,incY));
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(extras_spectrogram_obj, 1, 2, extras_spectrogram);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(extras_blas_saxpy_obj, 3, 6, extras_blas_saxpy);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(extras_blas_sdot_obj, 2, 5, extras_blas_sdot);

STATIC const mp_rom_map_elem_t ulab_extras_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_extras) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_spectrogram), (mp_obj_t)&extras_spectrogram_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_blas_saxpy), (mp_obj_t)&extras_blas_saxpy_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_blas_sdot), (mp_obj_t)&extras_blas_sdot_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_extras_globals, ulab_extras_globals_table);

mp_obj_module_t ulab_extras_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_extras_globals,
};

#endif

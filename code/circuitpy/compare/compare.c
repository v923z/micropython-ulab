

/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Zoltán Vörös
 *               2020 Jeff Epler for Adafruit Industries
*/

//| """Comparison functions"""
//|

//| def clip(
//|     x1: Union[ulab.array, float],
//|     x2: Union[ulab.array, float],
//|     x3: Union[ulab.array, float],
//| ) -> ulab.array:
//|     """
//|     Constrain the values from ``x1`` to be between ``x2`` and ``x3``.
//|     ``x2`` is assumed to be less than or equal to ``x3``.
//|
//|     Arguments may be ulab arrays or numbers.  All array arguments
//|     must be the same size.  If the inputs are all scalars, a
//|     single scalar is returned.
//|
//|     Shorthand for ``ulab.maximum(x2, ulab.minimum(x1, x3))``"""
//|     ...
//|

//| def equal(x1: Union[ulab.array, float], x2: Union[ulab.array, float]) -> List[bool]:
//|     """Return an array of bool which is true where x1[i] == x2[i] and false elsewhere"""
//|     ...
//|

//| def not_equal(x1: Union[ulab.array, float], x2: Union[ulab.array, float]) -> List[bool]:
//|     """Return an array of bool which is false where x1[i] == x2[i] and true elsewhere"""
//|     ...
//|

//| def maximum(x1: Union[ulab.array, float], x2: Union[ulab.array, float]) -> ulab.array:
//|     """
//|     Compute the element by element maximum of the arguments.
//|
//|     Arguments may be ulab arrays or numbers.  All array arguments
//|     must be the same size.  If the inputs are both scalars, a number is
//|     returned"""
//|     ...
//|

//| def minimum(x1: Union[ulab.array, float], x2: Union[ulab.array, float]) -> ulab.array:
//|     """Compute the element by element minimum of the arguments.
//|
//|     Arguments may be ulab arrays or numbers.  All array arguments
//|     must be the same size.  If the inputs are both scalars, a number is
//|     returned"""
//|     ...
//|

STATIC const mp_rom_map_elem_t ulab_compare_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_compare) },
    #if ULAB_COMPARE_HAS_CLIP
    { MP_OBJ_NEW_QSTR(MP_QSTR_clip), (mp_obj_t)&compare_clip_obj },
    #endif
    #if ULAB_COMPARE_HAS_EQUAL
    { MP_OBJ_NEW_QSTR(MP_QSTR_equal), (mp_obj_t)&compare_equal_obj },
    #endif
    #if ULAB_COMPARE_HAS_NOTEQUAL
    { MP_OBJ_NEW_QSTR(MP_QSTR_not_equal), (mp_obj_t)&compare_not_equal_obj },
    #endif
    #if ULAB_COMPARE_HAS_MAXIMUM
    { MP_OBJ_NEW_QSTR(MP_QSTR_maximum), (mp_obj_t)&compare_maximum_obj },
    #endif
    #if ULAB_COMPARE_HAS_MINIMUM
    { MP_OBJ_NEW_QSTR(MP_QSTR_minimum), (mp_obj_t)&compare_minimum_obj },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_compare_globals, ulab_compare_globals_table);

mp_obj_module_t ulab_compare_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_compare_globals,
};

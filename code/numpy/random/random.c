/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Zoltán Vörös
*/

#include "py/builtin.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "../../ndarray.h"
#include "random.h"

#if ULAB_NUMPY_RANDOM_HAS_UNIFORM
static mp_obj_t random_uniform(mp_obj_t oin) {
    (void)oin;
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(random_uniform_obj, random_uniform);
#endif /* ULAB_NUMPY_RANDOM_HAS_UNIFORM */


STATIC const mp_rom_map_elem_t ulab_numpy_random_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_random) },
    #if ULAB_NUMPY_RANDOM_HAS_UNIFORM
    { MP_ROM_QSTR(MP_QSTR_uniform), MP_ROM_PTR(&random_uniform_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_numpy_random_globals, ulab_numpy_random_globals_table);

const mp_obj_module_t ulab_numpy_random_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_numpy_random_globals,
};


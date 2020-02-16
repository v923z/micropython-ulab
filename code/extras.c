
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

STATIC const mp_rom_map_elem_t ulab_filter_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_extras) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_extras_globals, ulab_extras_globals_table);

mp_obj_module_t ulab_filter_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_extras_globals,
};

#endif

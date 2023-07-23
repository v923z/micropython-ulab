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

// methods of the Generator object
static const mp_rom_map_elem_t random_generator_locals_dict_table[] = {
    #if ULAB_NUMPY_RANDOM_HAS_RANDOM
        { MP_ROM_QSTR(MP_QSTR_random), MP_ROM_PTR(&random_random_obj) },
    #endif
    #if ULAB_NUMPY_RANDOM_HAS_UNIFORM
        { MP_ROM_QSTR(MP_QSTR_uniform), MP_ROM_PTR(&random_uniform_obj) },
    #endif
};

static MP_DEFINE_CONST_DICT(random_generator_locals_dict, random_generator_locals_dict_table);

// random's Generator object is defined here
#if defined(MP_DEFINE_CONST_OBJ_TYPE)
MP_DEFINE_CONST_OBJ_TYPE(
    random_generator_type,
    MP_QSTR_generator,
    MP_TYPE_FLAG_NONE,
    print, random_generator_print,
    make_new, random_generator_make_new, 
    locals_dict, &random_generator_locals_dict
);
#else
const mp_obj_type_t random_generator_type = {
    { &mp_type_type },
    .name = MP_QSTR_generator,
    .print = random_generator_print,
    .make_new = random_generator_make_new,
    .locals_dict = (mp_obj_dict_t*)&random_generator_locals_dict
};
#endif

void random_generator_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    random_generator_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(MP_PYTHON_PRINTER, "Gnerator() at 0x%p", self);
}

mp_obj_t random_generator_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void) type;
    mp_arg_check_num(n_args, n_kw, 0, 1, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
    };
    mp_arg_val_t _args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, _args);

    random_generator_obj_t *generator = m_new_obj(random_generator_obj_t);
    generator->base.type = &random_generator_type;
    // we should add the seed here!
    generator->value = 1;

    return MP_OBJ_FROM_PTR(generator);
}

// END OF GENERATOR COMPONENTS


#if ULAB_NUMPY_RANDOM_HAS_RANDOM
static mp_obj_t random_random(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_out, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    if(args[0].u_obj != mp_const_none) {
    }
    if(args[1].u_obj != mp_const_none) {
    }

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(random_random_obj, 1, random_random);
#endif /* ULAB_NUMPY_RANDOM_HAS_RANDOM */

#if ULAB_NUMPY_RANDOM_HAS_UNIFORM
static mp_obj_t random_uniform(mp_obj_t oin) {
    (void)oin;
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(random_uniform_obj, random_uniform);
#endif /* ULAB_NUMPY_RANDOM_HAS_UNIFORM */


static const mp_rom_map_elem_t ulab_numpy_random_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_random) },
    { MP_ROM_QSTR(MP_QSTR_Generator), MP_ROM_PTR(&random_generator_type) },
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_numpy_random_globals, ulab_numpy_random_globals_table);

const mp_obj_module_t ulab_numpy_random_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_numpy_random_globals,
};


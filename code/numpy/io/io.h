/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Zoltán Vörös
*/

#ifndef _ULAB_IO_
#define _ULAB_IO_

#if MICROPY_VFS
#define io_open_stream mp_vfs_open
#else
#define io_open_stream mp_builtin_open
#endif

MP_DECLARE_CONST_FUN_OBJ_2(io_save_obj);
MP_DECLARE_CONST_FUN_OBJ_1(io_load_obj);

#endif

/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Zoltán Vörös
*/

#ifndef _CARRAY_TOOLS_
#define _CARRAY_TOOLS_

void raise_complex_NotImplementedError(void);

#if ULAB_SUPPORTS_COMPLEX
    #define NOT_IMPLEMENTED_FOR_COMPLEX() raise_complex_NotImplementedError();
#else
    #define NOT_IMPLEMENTED_FOR_COMPLEX() // do nothing
#endif

#endif

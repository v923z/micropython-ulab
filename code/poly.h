/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Zoltán Vörös
*/
    
#ifndef _POLY_
#define _POLY_

mp_obj_t poly_polyval(mp_obj_t , mp_obj_t );
mp_obj_t poly_polyfit(size_t  , const mp_obj_t *);

#endif

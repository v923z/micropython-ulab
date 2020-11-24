
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#ifndef __ULAB_SCIPY_DEFS__
#define __ULAB_SCIPY_DEFS__


#define ULAB_SCIPY_HAS_SIGNAL_MODULE        (1)
#define ULAB_SCIPY_SIGNAL_HAS_SPECTROGRAM   (1)
#define ULAB_SCIPY_SIGNAL_HAS_SOSFILT       (1)


#define ULAB_SCIPY_HAS_OPTIMIZE_MODULE      (1)
#define ULAB_SCIPY_OPTIMIZE_HAS_BISECT		(1)
#define ULAB_SCIPY_OPTIMIZE_HAS_CURVE_FIT	(0) // not fully implemented
#define ULAB_SCIPY_OPTIMIZE_HAS_FMIN		(1)
#define ULAB_SCIPY_OPTIMIZE_HAS_NEWTON		(1)


#define ULAB_SCIPY_HAS_SPECIAL_MODULE       (1)
#define ULAB_SCIPY_SPECIAL_HAS_ERF          (1)
#define ULAB_SCIPY_SPECIAL_HAS_ERFC         (1)
#define ULAB_SCIPY_SPECIAL_HAS_GAMMA        (1)
#define ULAB_SCIPY_SPECIAL_HAS_GAMMALN      (1)

#endif

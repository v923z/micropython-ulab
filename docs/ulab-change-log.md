Mon, 2 Nov 2020

version 1.1.3

    fixed small glitch in diagonal, and ndarray_make_new_core

Sun, 1 Nov 2020

version 1.1.1

    fixed compilation error for 4D

Sat, 31 Oct 2020

version 1.1.0

    added the diagonal function

Fri, 30 Oct 2020

version 1.0.0

    added :
        support for tensors of rank 4
        proper broadcasting
        views
        .tobytes()
        concatenate
        cross
        full
        logspace
        in-place operators

Sat, 25 Oct 2020

version 0.54.5

    wrong type in slices raise TypeError exception

Fri, 23 Oct 2020

version 0.54.4

    fixed indexing error in slices

Mon, 17 Aug 2020

version 0.54.3

    fixed small error in linalg

Mon, 03 Aug 2020

version 0.54.2

    argsort throws an error, if the array is longer than 65535

Wed, 29 Jul 2020

version 0.54.1

    changed to size_t for the length of arrays
    
Thu, 23 Jul 2020

version 0.54.0

    added norm to linalg
    
Wed, 22 Jul 2020

version 0.53.2

    added circuitpython documentation stubs to the source files
    
Wed, 22 Jul 2020

version 0.53.1

    fixed arange with negative steps
    
Mon, 20 Jul 2020

version 0.53.0

    added arange to create.c
    
Thu, 16 Jul 2020

version 0.52.0

    added trapz to approx

Mon, 29 Jun 2020

version 0.51.1

    fixed argmin/argmax issue

Fri, 19 Jun 2020

version 0.51.0

    add sosfilt to the filter sub-module

Fri, 12 Jun 2020

version 0.50.2

    fixes compilation error in openmv

Mon, 1 Jun 2020

version 0.50.1

    fixes error in numerical max/min
    
Mon, 18 May 2020

version 0.50.0

    move interp to the approx sub-module
    
Wed, 06 May 2020

version 0.46.0

    add curve_fit to the approx sub-module

version 0.44.0

	add approx sub-module with newton, fmin, and bisect functions

Thu, 30 Apr 2020

version 0.44.0

	add approx sub-module with newton, fmin, and bisect functions

Tue, 19 May 2020

version 0.46.1

    fixed bad error in binary_op

Wed, 6 May 2020

version 0.46

    added vectorisation of python functions

Sat, 2 May 2020

version 0.45.0

	add equal/not_equal to the compare module
	
Tue, 21 Apr 2020

version 0.42.0

	add minimum/maximum/clip functions

Mon, 20 Apr 2020

version 0.41.6

	argument handling improvement in polyfit

Mon, 20 Apr 2020

version 0.41.5

	fix compilation errors due to https://github.com/micropython/micropython/commit/30840ebc9925bb8ef025dbc2d5982b1bfeb75f1b

Sat, 18 Apr 2020

version 0.41.4

	fix compilation error on hardware ports

Tue, 14 Apr 2020

version 0.41.3

	fix indexing error in dot function

Thu, 9 Apr 2020

version 0.41.2

	fix transpose function

Tue, 7 Apr 2020

version 0.41.2

	fix discrepancy in argmin/argmax behaviour

Tue, 7 Apr 2020

version 0.41.1

	fix error in argsort

Sat, 4 Apr 2020

version 0.41.0

	implemented == and != binary operators

Fri, 3 Apr 2020

version 0.40.0

	added trace to linalg

Thu, 2 Apr 2020

version 0.39.0

	added the ** operator, and operand swapping in binary operators

Thu, 2 Apr 2020

version 0.38.1

	added fast option, when initialising from ndarray_properties

Thu, 12 Mar 2020

version 0.38.0

	added initialisation from ndarray, and the around function

Tue, 10 Mar 2020

version 0.37.0

	added Cholesky decomposition to linalg.c
	
Thu, 27 Feb 2020

version 0.36.0

	moved zeros, ones, eye and linspace into separate module (they are still bound at the top level)
	
Thu, 27 Feb 2020

version 0.35.0

    Move zeros, ones back into top level ulab module

Tue, 18 Feb 2020

version 0.34.0

    split ulab into multiple modules

Sun, 16 Feb 2020

version 0.33.2

    moved properties into ndarray_properties.h, implemented pointer arithmetic in fft.c to save some time

Fri, 14 Feb 2020

version 0.33.1

    added the __name__attribute to all sub-modules

Thu, 13 Feb 2020

version 0.33.0

    sub-modules are now proper sub-modules of ulab

Mon, 17 Feb 2020

version 0.32.1

    temporary fix for issue #40
    
Tue, 11 Feb 2020

version 0.32.0

    added itemsize, size and shape attributes to ndarrays, and removed rawsize

Mon, 10 Feb 2020

version 0.31.0

    removed asbytearray, and added buffer protocol to ndarrays, fixed bad error in filter.c
    
Sun, 09 Feb 2020

version 0.30.2

    fixed slice_length in ndarray.c

Sat, 08 Feb 2020

version 0.30.1

    fixed typecode error, added variable inspection, and replaced ternary operators in filter.c
    
Fri, 07 Feb 2020

version 0.30.0

    ulab functions can arbitrarily be excluded from the firmware via the ulab.h configuration file
    
Thu, 06 Feb 2020

version 0.27.0

    add convolve, the start of a 'filter' functionality group

Wed, 29 Jan 2020

version 0.26.7

    fixed indexing error in linalg.dot

Mon, 20 Jan 2020

version 0.26.6

    replaced MP_ROM_PTR(&mp_const_none_obj), so that module can be compiled for the nucleo board

Tue, 7 Jan 2020

version 0.26.5

    fixed glitch in numerical.c, numerical.h

Mon, 6 Jan 2020

version 0.26.4

    switched version constant to string

Tue, 31 Dec 2019

version 0.263

    changed declaration of ulab_ndarray_type to extern

Fri, 29 Nov 2019

version 0.262

    fixed error in macro in vectorise.h

Thu, 28 Nov 2019

version 0.261

    fixed bad indexing error in linalg.dot

Tue, 6 Nov 2019

version 0.26

    added in-place sorting (method of ndarray), and argsort
    
Mon, 4 Nov 2019

version 0.25

    added first implementation of sort, and fixed section on compiling the module in the manual

Thu, 31 Oct 2019

version 0.24

    added diff to numerical.c
    
Tue, 29 Oct 2019

version 0.23

    major revamp of subscription method

Sat, 19 Oct 2019

version 0.21

    fixed trivial bug in .rawsize()

Sat, 19 Oct 2019

version 0.22

    fixed small error in linalg_det, and implemented linalg_eig.


Thu, 17 Oct 2019

version 0.21

    implemented uniform interface for fft, and spectrum, and added ifft.

Wed, 16 Oct 2019

version 0.20

    Added flip function to numerical.c, and moved the size function to linalg. In addition, 
    size is a function now, and not a method.

Tue, 15 Oct 2019

version 0.19

    fixed roll in numerical.c: it can now accept the axis=None keyword argument, added determinant to linalg.c

Mon, 14 Oct 2019

version 0.18

    fixed min/man function in numerical.c; it conforms to numpy behaviour

Fri, 11 Oct 2019

version 0.171

    found and fixed small bux in roll function

Fri, 11 Oct 2019

version 0.17

    universal function can now take arbitrary typecodes

Fri, 11 Oct 2019

version 0.161

    fixed bad error in iterator, and make_new_ndarray 
    
Thu, 10 Oct 2019

varsion 0.16

    changed ndarray to array in ulab.c, so as to conform to numpy's notation
    extended subscr method to include slices (partially works)
    
Tue, 8 Oct 2019

version 0.15

    added inv, neg, pos, and abs unary operators to ndarray.c
    
Mon, 7 Oct 2019

version 0.14

    made the internal binary_op function tighter, and added keyword arguments to linspace
    
Sat, 4 Oct 2019

version 0.13

    added the <, <=, >, >= binary operators to ndarray

Fri, 4 Oct 2019

version 0.12

    added .flatten to ndarray, ones, zeros, and eye to linalg

Thu, 3 Oct 2019

version 0.11
    
    binary operators are now based on macros

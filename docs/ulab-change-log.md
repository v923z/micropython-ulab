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

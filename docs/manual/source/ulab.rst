Introduction
============

In the `last
chapter <https://micropython-usermod.readthedocs.io/en/latest/usermods_15.html>`__
of the usermod documentation, I mentioned that I have another story, for
another day. The day has come, so here is my story.

Enter ulab
----------

``ulab`` is a ``numpy``-like module for ``micropython``, meant to
simplify and speed up common mathematical operations on arrays. The
primary goal was to implement a small subset of ``numpy`` that might be
useful in the context of a microcontroller. This means low-level data
processing of linear (array) and two-dimensional (matrix) data.

Purpose
-------

Of course, the first question that one has to answer is, why on Earth
one would need a fast math library on a microcontroller. After all, it
is not expected that heavy number crunching is going to take place on
bare metal. It is not meant to. On a PC, the main reason for writing
fast code is the sheer amount of data that one wants to process. On a
microcontroller, the data volume is probably small, but it might lead to
catastrophic system failure, if these data are not processed in time,
because the microcontroller is supposed to interact with the outside
world in a timely fashion. In fact, this latter objective was the
initiator of this project: I needed the Fourier transform of a signal
coming from the ADC of the pyboard, and all available options were
simply too slow.

In addition to speed, another issue that one has to keep in mind when
working with embedded systems is the amount of available RAM: I believe,
everything here could be implemented in pure python with relatively
little effort, but the price we would have to pay for that is not only
speed, but RAM, too. python code, if is not frozen, and compiled into
the firmware, has to be compiled at runtime, which is not exactly a
cheap process. On top of that, if numbers are stored in a list or tuple,
which would be the high-level container, then they occupy 8 bytes, no
matter, whether they are all smaller than 100, or larger than one
hundred million. This is obviously a waste of resources in an
environment, where resources are scarce.

Finally, there is a reason for using ``micropython`` in the first place.
Namely, that a microcontroller can be programmed in a very elegant, and
*pythonic* way. But if it is so, why should we not extend this idea to
other tasks and concepts that might come up in this context? If there
was no other reason than this *elegance*, I would find that convincing
enough.

Based on the above-mentioned considerations, all functions in ``ulab``
are implemented in a way that

1. conforms to ``numpy`` as much as possible
2. is so frugal with RAM as possible,
3. and yet, fast. Much faster than pure python. Think of a number
   between 30 and 50!

The main points of ``ulab`` are

-  compact, iterable and slicable containers of numerical data in 1, and
   2 dimensions (arrays and matrices). These containers support all the
   relevant unary and binary operators (e.g., ``len``, ==, +, \*, etc.)
-  vectorised computations on micropython iterables and numerical
   arrays/matrices (in ``numpy``-speak, universal functions)
-  basic linear algebra routines (matrix inversion, multiplication,
   reshaping, transposition, determinant, and eigenvalues)
-  polynomial fits to numerical data
-  fast Fourier transforms

At the time of writing this manual (for version 0.50.0), the library
adds approximately 40 kB of extra compiled code to the micropython
(pyboard.v.11) firmware. However, if you are tight with flash space, you
can easily shave off a couple of kB. See the section on `customising
ulab <#Custom_builds>`__.

Resources and legal matters
---------------------------

The source code of the module can be found under
https://github.com/v923z/micropython-ulab/tree/master/code. The source
of this user manual is under
https://github.com/v923z/micropython-ulab/tree/master/docs, while the
technical details of the implementation are discussed at great length in
https://github.com/v923z/micropython-ulab/tree/master/docs/ulab.ipynb.
If you want an even thorougher explanation on why the various constructs
of the implementation work, and work in that particular way, you can
read more on the subject under
https://micropython-usermod.readthedocs.io/en/latest/, where I
demonstrate, what you have to do, if you want to make a C object behave
in a *pythonic* way.

The MIT licence applies to all material.

Friendly request
----------------

If you use ``ulab``, and bump into a bug, or think that a particular
function is missing, or its behaviour does not conform to ``numpy``,
please, raise a `ulab
issue <#https://github.com/v923z/micropython-ulab/issues>`__ on github,
so that the community can profit from your experiences.

Even better, if you find the project useful, and think that it could be
made better, faster, tighter, and shinier, please, consider
contributing, and issue a pull request with the implementation of your
improvements and new features. ``ulab`` can only become successful, if
it offers what the community needs.

These last comments apply to the documentation, too. If, in your
opinion, the documentation is obscure, misleading, or not detailed
enough, please, let me know, so that *we* can fix it.

Differences between micropython-ulab and circuitpython-ulab
-----------------------------------------------------------

``ulab`` has originally been developed for ``micropython``, but has
since been integrated into a number of its flavours. Most of these
flavours are simply forks of ``micropython`` itself, with some
additional functionality. One of the notable exceptions is
``circuitpython``, which has slightly diverged at the core level, and
this has some minor consequences. Some of these concern the C
implementation details only, which all have been sorted out with the
generous and enthusiastic support of Jeff Epler from `Adafruit
Industries <http://www.adafruit.com>`__.

There are, however, a couple of instances, where the usage in the two
environments is slightly different at the python level. These are how
the packages can be imported, and how the class properties can be
accessed. In both cases, the ``circuitpython`` implementation results in
``numpy``-conform code. ``numpy``-compatibility in ``micropython`` will
be implemented as soon as ``micropython`` itself has the required tools.
Till then we have to live with a workaround, which I will point out at
the relevant places.

Customising ``ulab``
====================

``ulab`` implements a great number of functions, which are organised in
sub-modules. E.g., functions related to Fourier transforms are located
in the ``ulab.fft`` sub-module, so you would import ``fft`` as

.. code:: python

   import ulab
   from ulab import fft

by which point you can get the FFT of your data by calling
``fft.fft(...)``.

The idea of such grouping of functions and methods is to provide a means
for granularity: It is quite possible that you do not need all functions
in a particular application. If you want to save some flash space, you
can easily exclude arbitrary sub-modules from the firmware. The
`ulab.h <https://github.com/v923z/micropython-ulab/blob/master/code/ulab.h>`__
header file contains a pre-processor flag for each sub-module. The
default setting is 1 for each of them. Setting them to 0 removes the
module from the compiled firmware.

The first couple of lines of the file look like this

.. code:: c

   // vectorise (all functions) takes approx. 6 kB of flash space
   #define ULAB_VECTORISE_MODULE (1)

   // linalg adds around 6 kB
   #define ULAB_LINALG_MODULE (1)

   // poly requires approx. 2.5 kB
   #define ULAB_POLY_MODULE (1)

In order to simplify navigation in the header, each flag begins with
``ULAB_``, and continues with the name of the sub-module. This name is
also the ``.c`` file, where the sub-module is implemented. So, e.g., the
linear algebra routines can be found in ``linalg.c``, and the
corresponding compiler flag is ``ULAB_LINALG_MODULE``. Each section
displays a hint as to how much space you can save by un-setting the
flag.

At first, having to import everything in this way might appear to be
overly complicated, but there is a very good reason behind all this: you
can find out at the time of importing, whether a function or sub-module
is part of your ``ulab`` firmware, or not. The alternative, namely, that
you do not have to import anything beyond ``ulab``, could prove
catastrophic: you would learn only at run time (at the moment of calling
the function in your code) that a particular function is not in the
firmware, and that is most probably too late.

Except for ``fft``, the standard sub-modules, ``vector``, ``linalg``,
``numerical``, and ``poly`` are all ``numpy``-compatible. User-defined
functions that accept ``ndarray``\ s as their argument should be
implemented in the ``extras`` sub-module, or its sub-modules. Hints as
to how to do that can be found in the section `Extending
ulab <#Extending-ulab>`__.

Supported functions and methods
===============================

``ulab`` supports a number of array operators, which are listed here. I
tried to follow the specifications of the ``numpy`` interface as closely
as possible, though, it was not always practical to implement verbatim
behaviour. The differences, if any, are in each case small (e.g., a
function cannot take all possible keyword arguments), and should not
hinder everyday use. In the list below, a single asterisk denotes slight
deviations from ``numpy``\ ’s nomenclature, and a double asterisk
denotes those cases, where a bit more caution should be exercised,
though this usually means functions that are not supported by ``numpy``.

The detailed discussion of the various functions always contains a link
to the corresponding ``numpy`` documentation. However, before going down
the rabbit hole, the module also defines a constant, the version, which
can always be queried as

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    print('you are running ulab version', np.__version__)

.. parsed-literal::

    you are running ulab version 0.50.2
    
    


If you find a bug, please, include this number in your report!

Basic ndarray operations
------------------------

`Unary operators <#Unary-operators>`__

`Binary operators <#Binary-operators>`__

`Indexing and slicing <#Slicing-and-indexing>`__

`ndarray iterators <#Iterating-over-arrays>`__

`Comparison operators\* <#Comparison-operators>`__

`Universal functions <#Universal-functions>`__ (also support function
calls on general iterables, and vectorisation of user-defined ``python``
functions.)

Methods of ndarrays
-------------------

`.shape\* <#.shape>`__

`size\* <#size>`__

`itemsize\* <#itemsize>`__

`.reshape <#.reshape>`__

`.transpose <#.transpose>`__

`.flatten\*\* <#.flatten>`__

Matrix methods
--------------

`inv <#inv>`__

`dot <#dot>`__

`det <#det>`__

`roll <#roll>`__

`flip <#flip>`__

Array initialisation functions
------------------------------

`eye <#eye>`__

`ones <#ones,-zeros>`__

`zeros <#ones,-zeros>`__

`linspace <#linspace>`__

Statistical and other properties of arrays
------------------------------------------

`min <#min,-argmin,-max,-argmax>`__

`argmin <#min,-argmin,-max,-argmax>`__

`max <#min,-argmin,-max,-argmax>`__

`argmax <#min,-argmin,-max,-argmax>`__

`sum <#sum,-std,-mean>`__

`std <#sum,-std,-mean>`__

`mean <#sum,-std,-mean>`__

`diff <#diff>`__

`sort <#sort>`__

`argsort <#argsort>`__

Linear algebra functions
------------------------

`size <#size>`__

`inv <#inv>`__

`dot <#dot>`__

`det <#det>`__

`eig <#eig>`__

`cholesky <#cholesky>`__

`trace <#trace>`__

Manipulation of polynomials
---------------------------

`polyval <#polyval>`__

`polyfit <#polyfit>`__

FFT routines
------------

`fft\*\* <#fft>`__

`ifft\*\* <#ifft>`__

`spectrogram\*\* <#spectrogram>`__

Filter functions
----------------

`convolve <#convolve>`__

`sosfilt <#sosfilt>`__

Comparison of arrays
--------------------

`equal <#equal,-not_equal>`__

`not_equal <#equal,-not_equal>`__

`minimum <#minimum>`__

`maximum <#maximum>`__

`clip <#clip>`__

Interpolation, root finding, function minimasation
--------------------------------------------------

`interp <#interp>`__

`newton <#newton>`__

`fmin <#fmin>`__

`bisect <#bisect>`__

ndarray, the basic container
============================

The ``ndarray`` is the underlying container of numerical data. It is
derived from micropython’s own ``array`` object, but has a great number
of extra features starting with how it can be initialised, which
operations can be done on it, and which functions can accept it as an
argument. One important property of an ``ndarray`` is that it is also a
proper ``micropython`` iterable.

Since the ``ndarray`` is a binary container, it is also compact, meaning
that it takes only a couple of bytes of extra RAM in addition to what is
required for storing the numbers themselves. ``ndarray``\ s are also
type-aware, i.e., one can save RAM by specifying a data type, and using
the smallest reasonable one. Five such types are defined, namely
``uint8``, ``int8``, which occupy a single byte of memory per datum,
``uint16``, and ``int16``, which occupy two bytes per datum, and
``float``, which occupies four or eight bytes per datum. The
precision/size of the ``float`` type depends on the definition of
``mp_float_t``. Some platforms, e.g., the PYBD, implement ``double``\ s,
but some, e.g., the pyboard.v.11, don’t. You can find out, what type of
float your particular platform implements by looking at the output of
the `.itemsize <#.itemsize>`__ class property.

On the following pages, we will see how one can work with
``ndarray``\ s. Those familiar with ``numpy`` should find that the
nomenclature and naming conventions of ``numpy`` are adhered to as
closely as possible. I will point out the few differences, where
necessary.

For the sake of comparison, in addition to the ``ulab`` code snippets,
sometimes the equivalent ``numpy`` code is also presented. You can find
out, where the snippet is supposed to run by looking at its first line,
the header.

Hint: you can easily port existing ``numpy`` code, if you
``import ulab as np``.

Initialising an array
---------------------

A new array can be created by passing either a standard micropython
iterable, or another ``ndarray`` into the constructor.

Initialising by passing iterables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the iterable is one-dimensional, i.e., one whose elements are
numbers, then a row vector will be created and returned. If the iterable
is two-dimensional, i.e., one whose elements are again iterables, a
matrix will be created. If the lengths of the iterables are not
consistent, a ``ValueError`` will be raised. Iterables of different
types can be mixed in the initialisation function.

If the ``dtype`` keyword with the possible
``uint8/int8/uint16/int16/float`` values is supplied, the new
``ndarray`` will have that type, otherwise, it assumes ``float`` as
default.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = [1, 2, 3, 4, 5, 6, 7, 8]
    b = np.array(a)
    
    print("a:\t", a)
    print("b:\t", b)
    
    # a two-dimensional array with mixed-type initialisers
    c = np.array([range(5), range(20, 25, 1), [44, 55, 66, 77, 88]], dtype=np.uint8)
    print("\nc:\t", c)
    
    # and now we throw an exception
    d = np.array([range(5), range(10), [44, 55, 66, 77, 88]], dtype=np.uint8)
    print("\nd:\t", d)

.. parsed-literal::

    a:	 [1, 2, 3, 4, 5, 6, 7, 8]
    b:	 array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0], dtype=float)
    
    c:	 array([[0, 1, 2, 3, 4],
    	 [20, 21, 22, 23, 24],
    	 [44, 55, 66, 77, 88]], dtype=uint8)
    
    Traceback (most recent call last):
      File "/dev/shm/micropython.py", line 15, in <module>
    ValueError: iterables are not of the same length
    


Initialising by passing arrays
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

An ``ndarray`` can be initialised by supplying another array. This
statement is almost trivial, since ``ndarray``\ s are iterables
themselves, though it should be pointed out that initialising through
arrays is faster, because simply a new copy is created, without
inspection, iteration etc. It is also possible to coerce type conversion
of the output (with type conversion, the iteration cannot be avoided,
therefore, this case will always be slower than straight copying):

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = [1, 2, 3, 4, 5, 6, 7, 8]
    b = np.array(a)
    c = np.array(b)
    d = np.array(b, dtype=np.uint8)
    
    print("a:\t", a)
    print("\nb:\t", b)
    print("\nc:\t", c)
    print("\nd:\t", d)

.. parsed-literal::

    a:	 [1, 2, 3, 4, 5, 6, 7, 8]
    
    b:	 array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0], dtype=float)
    
    c:	 array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0], dtype=float)
    
    d:	 array([1, 2, 3, 4, 5, 6, 7, 8], dtype=uint8)
    
    


Note that the default type of the ``ndarray`` is ``float``. Hence, if
the array is initialised from another array, type conversion will always
take place, except, when the output type is specifically supplied. I.e.,

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array(range(5), dtype=np.uint8)
    b = np.array(a)
    print("a:\t", a)
    print("\nb:\t", b)

.. parsed-literal::

    a:	 array([0, 1, 2, 3, 4], dtype=uint8)
    
    b:	 array([0.0, 1.0, 2.0, 3.0, 4.0], dtype=float)
    
    


will iterate over the elements in ``a``, since in the assignment
``b = np.array(a)`` no output type was given, therefore, ``float`` was
assumed. On the other hand,

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array(range(5), dtype=np.uint8)
    b = np.array(a, dtype=np.uint8)
    print("a:\t", a)
    print("\nb:\t", b)

.. parsed-literal::

    a:	 array([0, 1, 2, 3, 4], dtype=uint8)
    
    b:	 array([0, 1, 2, 3, 4], dtype=uint8)
    
    


will simply copy the content of ``a`` into ``b`` without any iteration,
and will, therefore, be faster. Keep this in mind, whenever the output
type, or performance is important.

Array initialisation functions
------------------------------

There are four functions that can be used for initialising an array.
These are bound to ``ulab`` itself at the top level, i.e., no module has
to be imported for the function invocations.

ones, zeros
~~~~~~~~~~~

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.zeros.html

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.ones.html

A couple of special arrays and matrices can easily be initialised by
calling one of the ``ones``, or ``zeros`` functions. ``ones`` and
``zeros`` follow the same pattern, and have the call signature

.. code:: python

   ones(shape, dtype=float)
   zeros(shape, dtype=float)

where shape is either an integer, or a 2-tuple.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    print(np.ones(6, dtype=np.uint8))
    print(np.zeros((6, 4)))

.. parsed-literal::

    array([1, 1, 1, 1, 1, 1], dtype=uint8)
    array([[0.0, 0.0, 0.0, 0.0],
    	 [0.0, 0.0, 0.0, 0.0],
    	 [0.0, 0.0, 0.0, 0.0],
    	 [0.0, 0.0, 0.0, 0.0],
    	 [0.0, 0.0, 0.0, 0.0],
    	 [0.0, 0.0, 0.0, 0.0]], dtype=float)
    
    


eye
~~~

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.eye.html

Another special array method is the ``eye`` function, whose call
signature is

.. code:: python

   eye(N, M, k=0, dtype=float)

where ``N`` (``M``) specify the dimensions of the matrix (if only ``N``
is supplied, then we get a square matrix, otherwise one with ``M`` rows,
and ``N`` columns), and ``k`` is the shift of the ones (the main
diagonal corresponds to ``k=0``). Here are a couple of examples.

With a single argument
^^^^^^^^^^^^^^^^^^^^^^

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    print(np.eye(5))

.. parsed-literal::

    array([[1.0, 0.0, 0.0, 0.0, 0.0],
    	 [0.0, 1.0, 0.0, 0.0, 0.0],
    	 [0.0, 0.0, 1.0, 0.0, 0.0],
    	 [0.0, 0.0, 0.0, 1.0, 0.0],
    	 [0.0, 0.0, 0.0, 0.0, 1.0]], dtype=float)
    
    


Specifying the dimensions of the matrix
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code::

    # code to be run in CPython
    
    ### Shifting the diagonal
    
    %%micropython -unix 1
    
    import ulab as np
    
    print(np.eye(4, M=6, k=-1, dtype=np.int16))

.. parsed-literal::

    array([[0, 0, 0, 0],
    	 [1, 0, 0, 0],
    	 [0, 1, 0, 0],
    	 [0, 0, 1, 0],
    	 [0, 0, 0, 1],
    	 [0, 0, 0, 0]], dtype=int16)
    
    


.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    print(np.eye(4, M=6, dtype=np.int8))

.. parsed-literal::

    array([[1, 0, 0, 0],
    	 [0, 1, 0, 0],
    	 [0, 0, 1, 0],
    	 [0, 0, 0, 1],
    	 [0, 0, 0, 0],
    	 [0, 0, 0, 0]], dtype=int8)
    
    


linspace
~~~~~~~~

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.linspace.html

This function returns an array, whose elements are uniformly spaced
between the ``start``, and ``stop`` points. The number of intervals is
determined by the ``num`` keyword argument, whose default value is 50.
With the ``endpoint`` keyword argument (defaults to ``True``) one can
include ``stop`` in the sequence. In addition, the ``dtype`` keyword can
be supplied to force type conversion of the output. The default is
``float``. Note that, when ``dtype`` is of integer type, the sequence is
not necessarily evenly spaced. This is not an error, rather a
consequence of rounding. (This is also the ``numpy`` behaviour.)

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    # generate a sequence with defaults
    print('default sequence:\t', np.linspace(0, 10))
    
    # num=5
    print('num=5:\t\t\t', np.linspace(0, 10, num=5))
    
    # num=5, endpoint=False
    print('num=5:\t\t\t', np.linspace(0, 10, num=5, endpoint=False))
    
    # num=5, endpoint=False, dtype=uint8
    print('num=5:\t\t\t', np.linspace(0, 5, num=7, endpoint=False, dtype=np.uint8))

.. parsed-literal::

    default sequence:	 array([0.0, 0.2040816396474838, 0.4081632792949677, ..., 9.591833114624023, 9.795914649963379, 9.999996185302734], dtype=float)
    num=5:			 array([0.0, 2.5, 5.0, 7.5, 10.0], dtype=float)
    num=5:			 array([0.0, 2.0, 4.0, 6.0, 8.0], dtype=float)
    num=5:			 array([0, 0, 1, 2, 2, 3, 4], dtype=uint8)
    
    


Customising array printouts
---------------------------

``ndarray``\ s are pretty-printed, i.e., if the length is larger than 10
(default value), then only the first and last three entries will be
printed. Also note that, as opposed to ``numpy``, the printout always
contains the ``dtype``.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array(range(200))
    print("a:\t", a)

.. parsed-literal::

    a:	 array([0.0, 1.0, 2.0, ..., 197.0, 198.0, 199.0], dtype=float)
    
    


set_printoptions
~~~~~~~~~~~~~~~~

The default values can be overwritten by means of the
``set_printoptions`` function
`numpy.set_printoptions <https://numpy.org/doc/1.18/reference/generated/numpy.set_printoptions.html>`__,
which accepts two keywords arguments, the ``threshold``, and the
``edgeitems``. The first of these arguments determines the length of the
longest array that will be printed in full, while the second is the
number of items that will be printed on the left and right hand side of
the ellipsis, if the array is longer than ``threshold``.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array(range(20))
    print("a printed with defaults:\t", a)
    
    np.set_printoptions(threshold=200)
    print("\na printed in full:\t\t", a)
    
    np.set_printoptions(threshold=10, edgeitems=2)
    print("\na truncated with 2 edgeitems:\t", a)

.. parsed-literal::

    a printed with defaults:	 array([0.0, 1.0, 2.0, ..., 17.0, 18.0, 19.0], dtype=float)
    
    a printed in full:		 array([0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0], dtype=float)
    
    a truncated with 2 edgeitems:	 array([0.0, 1.0, ..., 18.0, 19.0], dtype=float)
    
    


get_printoptions
~~~~~~~~~~~~~~~~

The set value of the ``threshold`` and ``edgeitems`` can be retrieved by
calling the ``get_printoptions`` function with no arguments. The
function returns a dictionary with two keys.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    np.set_printoptions(threshold=100, edgeitems=20)
    print(np.get_printoptions())

.. parsed-literal::

    {'threshold': 100, 'edgeitems': 20}
    
    


Methods of ndarrays
-------------------

.shape
~~~~~~

The ``.shape`` method (property) returns a 2-tuple with the number of
rows, and columns.

**WARNING:** In ``circuitpython``, you can call the method as a
property, i.e.,

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3, 4], dtype=np.int8)
    print("a:\n", a)
    print("shape of a:", a.shape)
    
    b= np.array([[1, 2], [3, 4]], dtype=np.int8)
    print("\nb:\n", b)
    print("shape of b:", b.shape)

.. parsed-literal::

    a:
     array([1, 2, 3, 4], dtype=int8)
    shape of a: (1, 4)
    
    b:
     array([[1, 2],
    	 [3, 4]], dtype=int8)
    shape of b: (2, 2)
    
    


**WARNING:** On the other hand, since properties are not implemented in
``micropython``, there you would call the method as a function, i.e.,

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3, 4], dtype=np.int8)
    print("a:\n", a)
    print("shape of a:", a.shape)
    
    b= np.array([[1, 2], [3, 4]], dtype=np.int8)
    print("\nb:\n", b)
    print("shape of b:", b.shape())

.. parsed-literal::

    a:
     array([1, 2, 3, 4], dtype=int8)
    shape of a: (1, 4)
    
    b:
     array([[1, 2],
    	 [3, 4]], dtype=int8)
    shape of b: (2, 2)
    
    


.size
~~~~~

The ``.size`` method (property) returns an integer with the number of
elements in the array.

**WARNING:** In ``circuitpython``, the ``numpy`` nomenclature applies,
i.e.,

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3], dtype=np.int8)
    print("a:\n", a)
    print("size of a:", a.size)
    
    b= np.array([[1, 2], [3, 4]], dtype=np.int8)
    print("\nb:\n", b)
    print("size of b:", b.size)

.. parsed-literal::

    a:
     array([1, 2, 3], dtype=int8)
    size of a: 3
    
    b:
     array([[1, 2],
    	 [3, 4]], dtype=int8)
    size of b: 4
    
    


**WARNING:** In ``micropython``, ``size`` is a method, i.e.,

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3], dtype=np.int8)
    print("a:\n", a)
    print("size of a:", a.size)
    
    b= np.array([[1, 2], [3, 4]], dtype=np.int8)
    print("\nb:\n", b)
    print("size of b:", b.size())

.. parsed-literal::

    a:
     array([1, 2, 3], dtype=int8)
    size of a: 3
    
    b:
     array([[1, 2],
    	 [3, 4]], dtype=int8)
    size of b: 4
    
    


.itemsize
~~~~~~~~~

The ``.itemsize`` method (property) returns an integer with the siz
enumber of elements in the array.

**WARNING:** In ``circuitpython``:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3], dtype=np.int8)
    print("a:\n", a)
    print("itemsize of a:", a.itemsize)
    
    b= np.array([[1, 2], [3, 4]], dtype=np.float)
    print("\nb:\n", b)
    print("itemsize of b:", b.itemsize)

.. parsed-literal::

    a:
     array([1, 2, 3], dtype=int8)
    itemsize of a: 1
    
    b:
     array([[1.0, 2.0],
    	 [3.0, 4.0]], dtype=float)
    itemsize of b: 8
    
    


**WARNING:** In ``micropython``:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3], dtype=np.int8)
    print("a:\n", a)
    print("itemsize of a:", a.itemsize)
    
    b= np.array([[1, 2], [3, 4]], dtype=np.float)
    print("\nb:\n", b)
    print("itemsize of b:", b.itemsize())

.. parsed-literal::

    a:
     array([1, 2, 3], dtype=int8)
    itemsize of a: 1
    
    b:
     array([[1.0, 2.0],
    	 [3.0, 4.0]], dtype=float)
    itemsize of b: 8
    
    


.reshape
~~~~~~~~

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.reshape.html

``reshape`` re-writes the shape properties of an ``ndarray``, but the
array will not be modified in any other way. The function takes a single
2-tuple with two integers as its argument. The 2-tuple should specify
the desired number of rows and columns. If the new shape is not
consistent with the old, a ``ValueError`` exception will be raised.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]], dtype=np.uint8)
    print('a (4 by 4):', a)
    print('a (2 by 8):', a.reshape((2, 8)))
    print('a (1 by 16):', a.reshape((1, 16)))

.. parsed-literal::

    a (4 by 4): array([[1, 2, 3, 4],
    	 [5, 6, 7, 8],
    	 [9, 10, 11, 12],
    	 [13, 14, 15, 16]], dtype=uint8)
    a (2 by 8): array([[1, 2, 3, 4, 5, 6, 7, 8],
    	 [9, 10, 11, 12, 13, 14, 15, 16]], dtype=uint8)
    a (1 by 16): array([1, 2, 3, ..., 14, 15, 16], dtype=uint8)
    
    


.flatten
~~~~~~~~

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.ndarray.flatten.htm

``.flatten`` returns the flattened array. The array can be flattened in
``C`` style (i.e., moving horizontally in the matrix), or in ``fortran``
style (i.e., moving vertically in the matrix). The ``C``-style
flattening is the default, and it is also fast, because this is just a
verbatim copy of the contents.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3, 4], dtype=np.int8)
    print("a: \t\t", a)
    print("a flattened: \t", a.flatten())
    
    b = np.array([[1, 2, 3], [4, 5, 6]], dtype=np.int8)
    print("\nb:", b)
    
    print("b flattened (C): \t", b.flatten())
    print("b flattened (F): \t", b.flatten(order='F'))

.. parsed-literal::

    a: 		 array([1, 2, 3, 4], dtype=int8)
    a flattened: 	 array([1, 2, 3, 4], dtype=int8)
    
    b: array([[1, 2, 3],
    	 [4, 5, 6]], dtype=int8)
    b flattened (C): 	 array([1, 2, 3, 4, 5, 6], dtype=int8)
    b flattened (F): 	 array([1, 4, 2, 5, 3, 6], dtype=int8)
    
    


.transpose
~~~~~~~~~~

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.transpose.html

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9], [10, 11, 12]], dtype=np.uint8)
    print('a:\n', a)
    print('shape of a:', a.shape())
    a.transpose()
    print('\ntranspose of a:\n', a)
    print('shape of a:', a.shape())

.. parsed-literal::

    a:
     array([[1, 2, 3],
    	 [4, 5, 6],
    	 [7, 8, 9],
    	 [10, 11, 12]], dtype=uint8)
    shape of a: (4, 3)
    
    transpose of a:
     array([[1, 4, 7, 10],
    	 [2, 5, 8, 11],
    	 [3, 6, 9, 12]], dtype=uint8)
    shape of a: (3, 4)
    
    


.sort
~~~~~

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.sort.html

In-place sorting of an ``ndarray``. For a more detailed exposition, see
`sort <#sort>`__.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([[1, 12, 3, 0], [5, 3, 4, 1], [9, 11, 1, 8], [7, 10, 0, 1]], dtype=np.uint8)
    print('\na:\n', a)
    a.sort(axis=0)
    print('\na sorted along vertical axis:\n', a)
    
    a = np.array([[1, 12, 3, 0], [5, 3, 4, 1], [9, 11, 1, 8], [7, 10, 0, 1]], dtype=np.uint8)
    a.sort(a, axis=1)
    print('\na sorted along horizontal axis:\n', a)
    
    a = np.array([[1, 12, 3, 0], [5, 3, 4, 1], [9, 11, 1, 8], [7, 10, 0, 1]], dtype=np.uint8)
    a.sort(a, axis=None)
    print('\nflattened a sorted:\n', a)

.. parsed-literal::

    
    a:
     array([[1, 12, 3, 0],
    	 [5, 3, 4, 1],
    	 [9, 11, 1, 8],
    	 [7, 10, 0, 1]], dtype=uint8)
    
    a sorted along vertical axis:
     array([[1, 3, 0, 0],
    	 [5, 10, 1, 1],
    	 [7, 11, 3, 1],
    	 [9, 12, 4, 8]], dtype=uint8)
    
    a sorted along horizontal axis:
     array([[0, 1, 3, 12],
    	 [1, 3, 4, 5],
    	 [1, 8, 9, 11],
    	 [0, 1, 7, 10]], dtype=uint8)
    
    flattened a sorted:
     array([0, 0, 1, ..., 10, 11, 12], dtype=uint8)
    
    


Unary operators
---------------

With the exception of ``len``, which returns a single number, all unary
operators manipulate the underlying data element-wise.

len
~~~

This operator takes a single argument, and returns either the length
(for row vectors), or the number of rows (for matrices) of its argument.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3, 4, 5], dtype=np.uint8)
    b = np.array([range(5), range(5), range(5), range(5)], dtype=np.uint8)
    
    print("a:\t", a)
    print("length of a: ", len(a))
    print("shape of a: ", a.shape())
    print("\nb:\t", b)
    print("length of b: ", len(b))
    print("shape of b: ", b.shape())

.. parsed-literal::

    a:	 array([1, 2, 3, 4, 5], dtype=uint8)
    length of a:  5
    shape of a:  (1, 5)
    
    b:	 array([[0, 1, 2, 3, 4],
    	 [0, 1, 2, 3, 4],
    	 [0, 1, 2, 3, 4],
    	 [0, 1, 2, 3, 4]], dtype=uint8)
    length of b:  4
    shape of b:  (4, 5)
    
    


The number returned by ``len`` is also the length of the iterations,
when the array supplies the elements for an iteration (see later).

invert
~~~~~~

The function is defined for integer data types (``uint8``, ``int8``,
``uint16``, and ``int16``) only, takes a single argument, and returns
the element-by-element, bit-wise inverse of the array. If a ``float`` is
supplied, the function raises a ``ValueError`` exception.

With signed integers (``int8``, and ``int16``), the results might be
unexpected, as in the example below:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([0, -1, -100], dtype=np.int8)
    print("a:\t\t", a)
    print("inverse of a:\t", ~a)
    
    a = np.array([0, 1, 254, 255], dtype=np.uint8)
    print("\na:\t\t", a)
    print("inverse of a:\t", ~a)

.. parsed-literal::

    a:		 array([0, -1, -100], dtype=int8)
    inverse of a:	 array([-1, 0, 99], dtype=int8)
    
    a:		 array([0, 1, 254, 255], dtype=uint8)
    inverse of a:	 array([255, 254, 1, 0], dtype=uint8)
    
    


abs
~~~

This function takes a single argument, and returns the
element-by-element absolute value of the array. When the data type is
unsigned (``uint8``, or ``uint16``), a copy of the array will be
returned immediately, and no calculation takes place.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([0, -1, -100], dtype=np.int8)
    print("a:\t\t\t ", a)
    print("absolute value of a:\t ", abs(a))

.. parsed-literal::

    a:			  array([0, -1, -100], dtype=int8)
    absolute value of a:	  array([0, 1, 100], dtype=int8)
    
    


neg
~~~

This operator takes a single argument, and changes the sign of each
element in the array. Unsigned values are wrapped.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([10, -1, 1], dtype=np.int8)
    print("a:\t\t", a)
    print("negative of a:\t", -a)
    
    b = np.array([0, 100, 200], dtype=np.uint8)
    print("\nb:\t\t", b)
    print("negative of b:\t", -b)

.. parsed-literal::

    a:		 array([10, -1, 1], dtype=int8)
    negative of a:	 array([-10, 1, -1], dtype=int8)
    
    b:		 array([0, 100, 200], dtype=uint8)
    negative of b:	 array([0, 156, 56], dtype=uint8)
    
    


pos
~~~

This function takes a single argument, and simply returns a copy of the
array.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([10, -1, 1], dtype=np.int8)
    print("a:\t\t", a)
    print("positive of a:\t", +a)

.. parsed-literal::

    a:		 array([10, -1, 1], dtype=int8)
    positive of a:	 array([10, -1, 1], dtype=int8)
    
    


Binary operators
----------------

``ulab`` implements the ``+``, ``-``, ``*``, ``/``, ``**``, ``<``,
``>``, ``<=``, ``>=``, ``==``, ``!=`` binary operators that work
element-wise. Partial broadcasting is available, meaning that the
operands either must have the same shape, or one of them must be a
scalar.

The operators raise a ``ValueError`` exception, if partial broadcasting
is not possible. The only exceptions are the ``==`` and ``!=`` operators
that will return ``False`` in this case.

**WARNING**: note that relational operators (``<``, ``>``, ``<=``,
``>=``, ``==``, ``!=``) should have the ``ndarray`` on their left hand
side, when compared to scalars. This means that the following works

.. code::
        
    # code to be run in micropython
    
    import ulab
    a = ulab.array([1, 2, 3])
    print(a > 2)

.. parsed-literal::

    [False, False, True]
    
    


while the equivalent statement, ``2 < a``, will raise a ``TypeError``
exception:

.. code::
        
    # code to be run in micropython
    
    import ulab
    a = ulab.array([1, 2, 3])
    print(2 < a)

.. parsed-literal::

    
    Traceback (most recent call last):
      File "/dev/shm/micropython.py", line 4, in <module>
    TypeError: unsupported types for __lt__: 'int', 'ndarray'
    


**WARNING:** ``numpy`` also allows operations between a matrix, and a
row vector, if the row vector has exactly as many elements, as many
columns the matrix has. This feature will be added in future versions of
``ulab``.

.. code::

    # code to be run in CPython
    
    a = array([[1, 2, 3], [4, 5, 6], [7, 8, 6]])
    b = array([10, 20, 30])
    a+b



.. parsed-literal::

    array([[11, 22, 33],
           [14, 25, 36],
           [17, 28, 36]])



**WARNING:** ``circuitpython`` users should use the ``equal``, and
``not_equal`` operators instead of ``==``, and ``!=``. See the section
on `array comparison <#Comparison-of-arrays>`__ for details.

Upcasting
~~~~~~~~~

Binary operations require special attention, because two arrays with
different typecodes can be the operands of an operation, in which case
it is not trivial, what the typecode of the result is. This decision on
the result’s typecode is called upcasting. Since the number of typecodes
in ``ulab`` is significantly smaller than in ``numpy``, we have to
define new upcasting rules. Where possible, I followed ``numpy``\ ’s
conventions.

``ulab`` observes the following upcasting rules:

1. Operations with two ``ndarray``\ s of the same ``dtype`` preserve
   their ``dtype``, even when the results overflow.

2. if either of the operands is a float, the result is automatically a
   float

3. When the right hand side of a binary operator is a micropython
   variable, ``mp_obj_int``, or ``mp_obj_float``, then the result will
   be promoted to ``dtype`` ``float``. This is necessary, because a
   micropython integer can be 31 bites wide. Other micropython types
   (e.g., lists, tuples, etc.) raise a ``TypeError`` exception.

4. 

============== =============== =========== ============
left hand side right hand side ulab result numpy result
============== =============== =========== ============
``uint8``      ``int8``        ``int16``   ``int16``
``uint8``      ``int16``       ``int16``   ``int16``
``uint8``      ``uint16``      ``uint16``  ``uint16``
``int8``       ``int16``       ``int16``   ``int16``
``int8``       ``uint16``      ``uint16``  ``int32``
``uint16``     ``int16``       ``float``   ``int32``
============== =============== =========== ============

Note that the last two operations are promoted to ``int32`` in
``numpy``.

**WARNING:** Due to the lower number of available data types, the
upcasting rules of ``ulab`` are slightly different to those of
``numpy``. Watch out for this, when porting code!

When one of the operands is a scalar, it will internally be turned into
a single-element ``ndarray`` with the *smallest* possible ``dtype``.
Thus, e.g., if the scalar is 123, it will be converted to an array of
``dtype`` ``uint8``.

Upcasting can be seen in action in the following snippet:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3, 4], dtype=np.uint8)
    b = np.array([1, 2, 3, 4], dtype=np.int8)
    print("a:\t", a)
    print("b:\t", b)
    print("a+b:\t", a+b)
    
    c = np.array([1, 2, 3, 4], dtype=np.float)
    print("\na:\t", a)
    print("c:\t", c)
    print("a*c:\t", a*c)

.. parsed-literal::

    a:	 array([1, 2, 3, 4], dtype=uint8)
    b:	 array([1, 2, 3, 4], dtype=int8)
    a+b:	 array([2, 4, 6, 8], dtype=int16)
    
    a:	 array([1, 2, 3, 4], dtype=uint8)
    c:	 array([1.0, 2.0, 3.0, 4.0], dtype=float)
    a*c:	 array([1.0, 4.0, 9.0, 16.0], dtype=float)
    
    


Benchmarks
~~~~~~~~~~

The following snippet compares the performance of binary operations to a
possible implementation in python. For the time measurement, we will
take the following snippet from the micropython manual:

.. code::
        
    # code to be run in micropython
    
    import utime
    
    def timeit(f, *args, **kwargs):
        func_name = str(f).split(' ')[1]
        def new_func(*args, **kwargs):
            t = utime.ticks_us()
            result = f(*args, **kwargs)
            print('execution time: ', utime.ticks_diff(utime.ticks_us(), t), ' us')
            return result
        return new_func

.. parsed-literal::

    


.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    @timeit
    def py_add(a, b):
        return [a[i]+b[i] for i in range(1000)]
    
    @timeit
    def py_multiply(a, b):
        return [a[i]*b[i] for i in range(1000)]
    
    @timeit
    def ulab_add(a, b):
        return a + b
    
    @timeit
    def ulab_multiply(a, b):
        return a * b
    
    a = [0.0]*1000
    b = range(1000)
    
    print('python add:')
    py_add(a, b)
    
    print('\npython multiply:')
    py_multiply(a, b)
    
    a = np.linspace(0, 10, num=1000)
    b = np.ones(1000)
    
    print('\nulab add:')
    ulab_add(a, b)
    
    print('\nulab multiply:')
    ulab_multiply(a, b)

.. parsed-literal::

    python add:
    execution time:  10051  us
    
    python multiply:
    execution time:  14175  us
    
    ulab add:
    execution time:  222  us
    
    ulab multiply:
    execution time:  213  us
    


I do not claim that the python implementation above is perfect, and
certainly, there is much room for improvement. However, the factor of 50
difference in execution time is very spectacular. This is nothing but a
consequence of the fact that the ``ulab`` functions run ``C`` code, with
very little python overhead. The factor of 50 appears to be quite
universal: the FFT routine obeys similar scaling (see `Speed of
FFTs <#Speed-of-FFTs>`__), and this number came up with font rendering,
too: `fast font rendering on graphical
displays <https://forum.micropython.org/viewtopic.php?f=15&t=5815&p=33362&hilit=ufont#p33383>`__.

Comparison operators
--------------------

The smaller than, greater than, smaller or equal, and greater or equal
operators return a vector of Booleans indicating the positions
(``True``), where the condition is satisfied.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3, 4, 5, 6, 7, 8], dtype=np.uint8)
    print(a < 5)

.. parsed-literal::

    [True, True, True, True, False, False, False, False]
    
    


**WARNING**: at the moment, due to implementation details, the
``ndarray`` must be on the left hand side of the relational operators.
This will change in a future version of ``ulab``.

That is, while ``a < 5`` and ``5 > a`` have the same meaning, the
following code will not work:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3, 4, 5, 6, 7, 8], dtype=np.uint8)
    print(5 > a)

.. parsed-literal::

    
    Traceback (most recent call last):
      File "/dev/shm/micropython.py", line 5, in <module>
    TypeError: unsupported types for __gt__: 'int', 'ndarray'
    


**WARNING:** Note that ``numpy`` returns an array of Booleans. For most
use cases this fact should not make a difference.

.. code::

    # code to be run in CPython
    
    a = array([1, 2, 3, 4, 5, 6, 7, 8])
    a < 5



.. parsed-literal::

    array([ True,  True,  True,  True, False, False, False, False])



These operators work with matrices, too, in which case a list of lists
of Booleans will be returned:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([range(0, 5, 1), range(1, 6, 1), range(2, 7, 1)], dtype=np.uint8)
    print(a)
    print(a < 5)

.. parsed-literal::

    array([[0, 1, 2, 3, 4],
    	 [1, 2, 3, 4, 5],
    	 [2, 3, 4, 5, 6]], dtype=uint8)
    [[True, True, True, True, True], [True, True, True, True, False], [True, True, True, False, False]]
    
    


Iterating over arrays
---------------------

``ndarray``\ s are iterable, which means that their elements can also be
accessed as can the elements of a list, tuple, etc. If the array is
one-dimensional, the iterator returns scalars, otherwise a new
one-dimensional ``ndarray``, which is simply a copy of the corresponding
row of the matrix, i.e, its data type will be inherited.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3, 4, 5], dtype=np.uint8)
    b = np.array([range(5), range(10, 15, 1), range(20, 25, 1), range(30, 35, 1)], dtype=np.uint8)
    
    print("a:\t", a)
    
    for i, _a in enumerate(a):
        print("element %d in a:"%i, _a)
        
    print("\nb:\t", b)
    
    for i, _b in enumerate(b):
        print("element %d in b:"%i, _b)

.. parsed-literal::

    a:	 array([1, 2, 3, 4, 5], dtype=uint8)
    element 0 in a: 1
    element 1 in a: 2
    element 2 in a: 3
    element 3 in a: 4
    element 4 in a: 5
    
    b:	 array([[0, 1, 2, 3, 4],
    	 [10, 11, 12, 13, 14],
    	 [20, 21, 22, 23, 24],
    	 [30, 31, 32, 33, 34]], dtype=uint8)
    element 0 in b: array([0, 1, 2, 3, 4], dtype=uint8)
    element 1 in b: array([10, 11, 12, 13, 14], dtype=uint8)
    element 2 in b: array([20, 21, 22, 23, 24], dtype=uint8)
    element 3 in b: array([30, 31, 32, 33, 34], dtype=uint8)
    
    


Slicing and indexing
--------------------

Copies of sub-arrays can be created by indexing, and slicing.

Indexing
~~~~~~~~

The simplest form of indexing is specifying a single integer between the
square brackets as in

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array(range(10), dtype=np.uint8)
    print("a:\t\t\t\t\t\t", a)
    print("the first, and first from right element of a:\t", a[0], a[-1])
    print("the second, and second from right element of a:\t", a[1], a[-2])

.. parsed-literal::

    a:						 array([0, 1, 2, ..., 7, 8, 9], dtype=uint8)
    the first, and first from right element of a:	 0 9
    the second, and second from right element of a:	 1 8
    
    


Indices are (not necessarily non-negative) integers, or a list of
Booleans. By using a Boolean list, we can select those elements of an
array that satisfy a specific condition. At the moment, such indexing is
defined for row vectors only, for matrices the function raises a
``ValueError`` exception, though this will be rectified in a future
version of ``ulab``.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array(range(9), dtype=np.float)
    print("a:\t", a)
    print("a < 5:\t", a[a < 5])

.. parsed-literal::

    a:	 array([0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0], dtype=float)
    a < 5:	 array([0.0, 1.0, 2.0, 3.0, 4.0], dtype=float)
    
    


Indexing with Boolean arrays can take more complicated expressions. This
is a very concise way of comparing two vectors, e.g.:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array(range(9), dtype=np.uint8)
    b = np.array([4, 4, 4, 3, 3, 3, 13, 13, 13], dtype=np.uint8)
    print("a:\t", a)
    print("\na**2:\t", a*a)
    print("\nb:\t", b)
    print("\n100*sin(b):\t", np.sin(b)*100.0)
    print("\na[a*a > np.sin(b)*100.0]:\t", a[a*a > np.sin(b)*100.0])

.. parsed-literal::

    a:	 array([0, 1, 2, 3, 4, 5, 6, 7, 8], dtype=uint8)
    
    a**2:	 array([0, 1, 4, 9, 16, 25, 36, 49, 64], dtype=uint8)
    
    b:	 array([4, 4, 4, 3, 3, 3, 13, 13, 13], dtype=uint8)
    
    100*sin(b):	 array([-75.68025, -75.68025, -75.68025, 14.112, 14.112, 14.112, 42.01671, 42.01671, 42.01671], dtype=float)
    
    a[a*a > np.sin(b)*100.0]:	 array([0, 1, 2, 4, 5, 7, 8], dtype=uint8)
    


Slicing and assigning to slices
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can also generate sub-arrays by specifying slices as the index of an
array. Slices are special python objects of the form

.. code:: python

   slice = start:end:stop

where ``start``, ``end``, and ``stop`` are (not necessarily
non-negative) integers. Not all of these three numbers must be specified
in an index, in fact, all three of them can be missing. The interpreter
takes care of filling in the missing values. (Note that slices cannot be
defined in this way, only there, where an index is expected.) For a good
explanation on how slices work in python, you can read the stackoverflow
question
https://stackoverflow.com/questions/509211/understanding-slice-notation.

Slices work on both axes:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]], dtype=np.uint8)
    print('a:\n', a)
    
    # the first row
    print('\na[0]:\n', a[0])
    
    # the first two elements of the first row
    print('\na[0,:2]:\n', a[0,:2])
    
    # the zeroth element in each row (also known as the zeroth column)
    print('\na[:,0]:\n', a[:,0])
    
    # the last but one row
    print('\na[-1]:\n', a[-1])
    
    # the last two rows backwards
    print('\na[::1]:\n', a[::-1])

.. parsed-literal::

    a:
     array([[1, 2, 3],
    	 [4, 5, 6],
    	 [7, 8, 9]], dtype=uint8)
    
    a[0]:
     array([1, 2, 3], dtype=uint8)
    
    a[0,:2]:
     array([1, 2], dtype=uint8)
    
    a[:,0]:
     array([1, 4, 7], dtype=uint8)
    
    a[-1]:
     array([7, 8, 9], dtype=uint8)
    
    a[::1]:
     array([[7, 8, 9],
    	 [4, 5, 6]], dtype=uint8)
    
    


Assignment to slices can be done for the whole slice, per row, and per
column. A couple of examples should make these statements clearer:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    zero_list = [0, 0, 0]
    a = np.array([zero_list, zero_list, zero_list], dtype=np.uint8)
    print('a:\n', a)
    
    # assigning to the whole row
    a[0] = 1
    print('\na[0] = 1\n', a)
    
    # assigning to the whole row
    a[0] = np.array([1, 2, -333], dtype=np.float)
    print('\na[0] = np.array([1, 2, 3])\n', a)
    
    # assigning to a column
    a[:,2] = 3.0
    print('\na[:,0]:\n', a)

.. parsed-literal::

    a:
     array([[0, 0, 0],
    	 [0, 0, 0],
    	 [0, 0, 0]], dtype=uint8)
    
    a[0] = 1
     array([[1, 1, 1],
    	 [0, 0, 0],
    	 [0, 0, 0]], dtype=uint8)
    
    a[0] = np.array([1, 2, 3])
     array([[1, 2, 179],
    	 [0, 0, 0],
    	 [0, 0, 0]], dtype=uint8)
    
    a[:,0]:
     array([[1, 2, 3],
    	 [0, 0, 3],
    	 [0, 0, 3]], dtype=uint8)
    
    


Universal functions
===================

Standard mathematical functions are defined in the ``vector``
sub-module, and can be calculated on any scalar, scalar-valued iterable
(ranges, lists, tuples containing numbers), and on ``ndarray``\ s
without having to change the call signature. In all cases the functions
return a new ``ndarray`` of typecode ``float`` (since these functions
usually generate float values, anyway). The functions execute faster
with ``ndarray`` arguments than with iterables, because the values of
the input vector can be extracted faster.

At present, the following functions are supported:

``acos``, ``acosh``, ``arctan2``, ``around``, ``asin``, ``asinh``,
``atan``, ``atanh``, ``ceil``, ``cos``, ``erf``, ``erfc``, ``exp``,
``expm1``, ``floor``, ``tgamma``, ``lgamma``, ``log``, ``log10``,
``log2``, ``sin``, ``sinh``, ``sqrt``, ``tan``, ``tanh``.

These functions are applied element-wise to the arguments, thus, e.g.,
the exponential of a matrix cannot be calculated in this way. The
functions can be invoked by importing the ``vector`` sub-module first.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import vector
    
    a = range(9)
    b = np.array(a)
    
    # works with ranges, lists, tuples etc.
    print('a:\t', a)
    print('exp(a):\t', vector.exp(a))
    
    # with 1D arrays
    print('\nb:\t', b)
    print('exp(b):\t', vector.exp(b))
    
    # as well as with matrices
    c = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
    print('\nc:\t', c)
    print('exp(c):\t', vector.exp(c))

.. parsed-literal::

    a:	 range(0, 9)
    exp(a):	 array([1.0, 2.718282, 7.389056, 20.08554, 54.59816, 148.4132, 403.4288, 1096.633, 2980.958], dtype=float)
    
    b:	 array([0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0], dtype=float)
    exp(b):	 array([1.0, 2.718282, 7.389056, 20.08554, 54.59816, 148.4132, 403.4288, 1096.633, 2980.958], dtype=float)
    
    c:	 array([[1.0, 2.0, 3.0],
    	 [4.0, 5.0, 6.0],
    	 [7.0, 8.0, 9.0]], dtype=float)
    exp(c):	 array([[2.718282, 7.389056, 20.08554],
    	 [54.59816, 148.4132, 403.4288],
    	 [1096.633, 2980.958, 8103.084]], dtype=float)
    


Computation expenses
--------------------

The overhead for calculating with micropython iterables is quite
significant: for the 1000 samples below, the difference is more than 800
microseconds, because internally the function has to create the
``ndarray`` for the output, has to fetch the iterable’s items of unknown
type, and then convert them to floats. All these steps are skipped for
``ndarray``\ s, because these pieces of information are already known.

Doing the same with ``list`` comprehension requires 30 times more time
than with the ``ndarray``, which would become even more, if we converted
the resulting list to an ``ndarray``.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import vector
    import math
    
    a = [0]*1000
    b = np.array(a)
    
    @timeit
    def timed_vector(iterable):
        return vector.exp(iterable)
    
    @timeit
    def timed_list(iterable):
        return [math.exp(i) for i in iterable]
    
    print('iterating over ndarray in ulab')
    timed_vector(b)
    
    print('\niterating over list in ulab')
    timed_vector(a)
    
    print('\niterating over list in python')
    timed_list(a)

.. parsed-literal::

    iterating over ndarray in ulab
    execution time:  441  us
    
    iterating over list in ulab
    execution time:  1266  us
    
    iterating over list in python
    execution time:  11379  us
    


Vectorising generic python functions
------------------------------------

``numpy``:
https://numpy.org/doc/stable/reference/generated/numpy.vectorize.html

The examples above use factory functions. In fact, they are nothing but
the vectorised versions of the standard mathematical functions.
User-defined ``python`` functions can also be vectorised by help of
``vectorize``. This function takes a positional argument, namely, the
``python`` function that you want to vectorise, and a non-mandatory
keyword argument, ``otypes``, which determines the ``dtype`` of the
output array. The ``otypes`` must be ``None`` (default), or any of the
``dtypes`` defined in ``ulab``. With ``None``, the output is
automatically turned into a float array.

The return value of ``vectorize`` is a ``micropython`` object that can
be called as a standard function, but which now accepts either a scalar,
an ``ndarray``, or a generic ``micropython`` iterable as its sole
argument. Note that the function that is to be vectorised must have a
single argument.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import vector
    
    def f(x):
        return x*x
    
    vf = vector.vectorize(f)
    
    # calling with a scalar
    print('{:20}'.format('f on a scalar: '), vf(44.0))
    
    # calling with an ndarray
    a = np.array([1, 2, 3, 4])
    print('{:20}'.format('f on an ndarray: '), vf(a))
    
    # calling with a list
    print('{:20}'.format('f on a list: '), vf([2, 3, 4]))

.. parsed-literal::

    f on a scalar:       array([1936.0], dtype=float)
    f on an ndarray:     array([1.0, 4.0, 9.0, 16.0], dtype=float)
    f on a list:         array([4.0, 9.0, 16.0], dtype=float)
    
    


As mentioned, the ``dtype`` of the resulting ``ndarray`` can be
specified via the ``otypes`` keyword. The value is bound to the function
object that ``vectorize`` returns, therefore, if the same function is to
be vectorised with different output types, then for each type a new
function object must be created.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import vector
    
    l = [1, 2, 3, 4]
    def f(x):
        return x*x
    
    vf1 = vector.vectorize(f, otypes=np.uint8)
    vf2 = vector.vectorize(f, otypes=np.float)
    
    print('{:20}'.format('output is uint8: '), vf1(l))
    print('{:20}'.format('output is float: '), vf2(l))

.. parsed-literal::

    output is uint8:     array([1, 4, 9, 16], dtype=uint8)
    output is float:     array([1.0, 4.0, 9.0, 16.0], dtype=float)
    
    


The ``otypes`` keyword argument cannot be used for type coercion: if the
function evaluates to a float, but ``otypes`` would dictate an integer
type, an exception will be raised:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import vector
    
    int_list = [1, 2, 3, 4]
    float_list = [1.0, 2.0, 3.0, 4.0]
    def f(x):
        return x*x
    
    vf = vector.vectorize(f, otypes=np.uint8)
    
    print('{:20}'.format('integer list: '), vf(int_list))
    # this will raise a TypeError exception
    print(vf(float_list))

.. parsed-literal::

    integer list:        array([1, 4, 9, 16], dtype=uint8)
    
    Traceback (most recent call last):
      File "/dev/shm/micropython.py", line 14, in <module>
    TypeError: can't convert float to int
    


Benchmarks
~~~~~~~~~~

It should be pointed out that the ``vectorize`` function produces the
pseudo-vectorised version of the ``python`` function that is fed into
it, i.e., on the C level, the same ``python`` function is called, with
the all-encompassing ``mp_obj_t`` type arguments, and all that happens
is that the ``for`` loop in ``[f(i) for i in iterable]`` runs purely in
C. Since type checking and type conversion in ``f()`` is expensive, the
speed-up is not so spectacular as when iterating over an ``ndarray``
with a factory function: a gain of approximately 30% can be expected,
when a native ``python`` type (e.g., ``list``) is returned by the
function, and this becomes around 50% (a factor of 2), if conversion to
an ``ndarray`` is also counted.

The following code snippet calculates the square of a 1000 numbers with
the vectorised function (which returns an ``ndarray``), with ``list``
comprehension, and with ``list`` comprehension followed by conversion to
an ``ndarray``. For comparison, the execution time is measured also for
the case, when the square is calculated entirely in ``ulab``.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import vector
    
    def f(x):
        return x*x
    
    vf = vector.vectorize(f)
    
    @timeit
    def timed_vectorised_square(iterable):
        return vf(iterable)
    
    @timeit
    def timed_python_square(iterable):
        return [f(i) for i in iterable]
    
    @timeit
    def timed_ndarray_square(iterable):
        return np.array([f(i) for i in iterable])
    
    @timeit
    def timed_ulab_square(ndarray):
        return ndarray**2
    
    print('vectorised function')
    squares = timed_vectorised_square(range(1000))
    
    print('\nlist comprehension')
    squares = timed_python_square(range(1000))
    
    print('\nlist comprehension + ndarray conversion')
    squares = timed_ndarray_square(range(1000))
    
    print('\nsquaring an ndarray entirely in ulab')
    a = np.array(range(1000))
    squares = timed_ulab_square(a)

.. parsed-literal::

    vectorised function
    execution time:  7237  us
    
    list comprehension
    execution time:  10248  us
    
    list comprehension + ndarray conversion
    execution time:  12562  us
    
    squaring an ndarray entirely in ulab
    execution time:  560  us
    


From the comparisons above, it is obvious that ``python`` functions
should only be vectorised, when the same effect cannot be gotten in
``ulab`` only. However, although the time savings are not significant,
there is still a good reason for caring about vectorised functions.
Namely, user-defined ``python`` functions become universal, i.e., they
can accept generic iterables as well as ``ndarray``\ s as their
arguments. A vectorised function is still a one-liner, resulting in
transparent and elegant code.

A final comment on this subject: the ``f(x)`` that we defined is a
*generic* ``python`` function. This means that it is not required that
it just crunches some numbers. It has to return a number object, but it
can still access the hardware in the meantime. So, e.g.,

.. code:: python


   led = pyb.LED(2)

   def f(x):
       if x < 100:
           led.toggle()
       return x*x

is perfectly valid code.

around
------

``numpy``:
https://docs.scipy.org/doc/numpy-1.17.0/reference/generated/numpy.around.html

``numpy``\ ’s ``around`` function can also be found in the ``vector``
sub-module. The function implements the ``decimals`` keyword argument
with default value ``0``. The first argument must be an ``ndarray``. If
this is not the case, the function raises a ``TypeError`` exception.
Note that ``numpy`` accepts general iterables. The ``out`` keyword
argument known from ``numpy`` is not accepted. The function always
returns an ndarray of type ``mp_float_t``.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import vector
    
    a = np.array([1, 2.2, 33.33, 444.444])
    print('a:\t\t', a)
    print('\ndecimals = 0\t', vector.around(a, decimals=0))
    print('\ndecimals = 1\t', vector.around(a, decimals=1))
    print('\ndecimals = -1\t', vector.around(a, decimals=-1))

.. parsed-literal::

    a:		 array([1.0, 2.2, 33.33, 444.444], dtype=float)
    
    decimals = 0	 array([1.0, 2.0, 33.0, 444.0], dtype=float)
    
    decimals = 1	 array([1.0, 2.2, 33.3, 444.4], dtype=float)
    
    decimals = -1	 array([0.0, 0.0, 30.0, 440.0], dtype=float)
    
    


arctan2
-------

``numpy``:
https://docs.scipy.org/doc/numpy-1.17.0/reference/generated/numpy.arctan2.html

The two-argument inverse tangent function is also part of the ``vector``
sub-module. The function implements only partial broadcasting, i.e., its
two arguments either have the same shape, or at least one of them must
be a single-element array. Scalars (``micropython`` integers or floats)
are also allowed.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import vector
    
    a = np.array([1, 2.2, 33.33, 444.444])
    print('a:\t\t', a)
    print('\narctan2(a, 1.0)\t', vector.arctan2(a, 1.0))
    print('\narctan2(1.0, a)\t', vector.arctan2(1.0, a))
    print('\narctan2(a, a): \t', vector.arctan2(a, a))

.. parsed-literal::

    a:		 array([1.0, 2.2, 33.33, 444.444], dtype=float)
    
    arctan2(a, 1.0)	 array([0.7853981633974483, 1.14416883366802, 1.5408023243361, 1.568546328341769], dtype=float)
    
    arctan2(1.0, a)	 array([0.7853981633974483, 0.426627493126876, 0.02999400245879636, 0.002249998453127392], dtype=float)
    
    arctan2(a, a): 	 array([0.7853981633974483, 0.7853981633974483, 0.7853981633974483, 0.7853981633974483], dtype=float)
    
    


Numerical
=========

Function in the ``numerical`` sub-module can be called by importing the
sub-module first.

min, argmin, max, argmax
------------------------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.min.html

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.argmax.html

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.max.html

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.argmax.html

**WARNING:** Difference to ``numpy``: the ``out`` keyword argument is
not implemented.

These functions follow the same pattern, and work with generic
iterables, and ``ndarray``\ s. ``min``, and ``max`` return the minimum
or maximum of a sequence. If the input array is two-dimensional, the
``axis`` keyword argument can be supplied, in which case the
minimum/maximum along the given axis will be returned. If ``axis=None``
(this is also the default value), the minimum/maximum of the flattened
array will be determined.

``argmin/argmax`` return the position (index) of the minimum/maximum in
the sequence.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import numerical
    
    a = np.array([1, 2, 0, 1, 10])
    print('a:', a)
    print('min of a:', numerical.min(a))
    print('argmin of a:', numerical.argmin(a))
    
    b = np.array([[1, 2, 0], [1, 10, -1]])
    print('\nb:\n', b)
    print('min of b (flattened):', numerical.min(b))
    print('min of b (axis=0):', numerical.min(b, axis=0))
    print('min of b (axis=1):', numerical.min(b, axis=1))

.. parsed-literal::

    a: array([1.0, 2.0, 0.0, 1.0, 10.0], dtype=float)
    min of a: 0.0
    argmin of a: 2
    
    b:
     array([[1.0, 2.0, 0.0],
    	 [1.0, 10.0, -1.0]], dtype=float)
    min of b (flattened): -1.0
    min of b (axis=0): array([1.0, 2.0, -1.0], dtype=float)
    min of b (axis=1): array([0.0, -1.0], dtype=float)
    
    


sum, std, mean
--------------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.sum.html

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.std.html

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.mean.html

These three functions follow the same pattern: if the axis keyword is
not specified, it assumes the default value of ``None``, and returns the
result of the computation for the flattened array. Otherwise, the
calculation is along the given axis.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import numerical
    
    a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
    print('a: \n', a)
    
    print('sum, flat array: ', numerical.sum(a))
    
    print('mean, horizontal: ', numerical.mean(a, axis=1))
    
    print('std, vertical: ', numerical.std(a, axis=0))

.. parsed-literal::

    a: 
     array([[1.0, 2.0, 3.0],
    	 [4.0, 5.0, 6.0],
    	 [7.0, 8.0, 9.0]], dtype=float)
    sum, flat array:  45.0
    mean, horizontal:  array([2.0, 5.0, 8.0], dtype=float)
    std, vertical:  array([2.44949, 2.44949, 2.44949], dtype=float)
    


roll
----

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.roll.html

The roll function shifts the content of a vector by the positions given
as the second argument. If the ``axis`` keyword is supplied, the shift
is applied to the given axis.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import numerical
    
    a = np.array([1, 2, 3, 4, 5, 6, 7, 8])
    print("a:\t\t\t", a)
    
    numerical.roll(a, 2)
    print("a rolled to the left:\t", a)
    
    # this should be the original vector
    numerical.roll(a, -2)
    print("a rolled to the right:\t", a)

.. parsed-literal::

    a:			 array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0], dtype=float)
    a rolled to the left:	 array([3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 1.0, 2.0], dtype=float)
    a rolled to the right:	 array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0], dtype=float)
    
    


Rolling works with matrices, too. If the ``axis`` keyword is 0, the
matrix is rolled along its vertical axis, otherwise, horizontally.

Horizontal rolls are faster, because they require fewer steps, and
larger memory chunks are copied, however, they also require more RAM:
basically the whole row must be stored internally. Most expensive are
the ``None`` keyword values, because with ``axis = None``, the array is
flattened first, hence the row’s length is the size of the whole matrix.

Vertical rolls require two internal copies of single columns.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import numerical
    
    a = np.array([[1, 2, 3, 4], [5, 6, 7, 8]])
    print("a:\n", a)
    
    numerical.roll(a, 2)
    print("\na rolled to the left:\n", a)
    
    numerical.roll(a, -1, axis=1)
    print("\na rolled up:\n", a)
    
    numerical.roll(a, 1, axis=None)
    print("\na rolled with None:\n", a)

.. parsed-literal::

    a:
     array([[1.0, 2.0, 3.0, 4.0],
    	 [5.0, 6.0, 7.0, 8.0]], dtype=float)
    
    a rolled to the left:
     array([[3.0, 4.0, 5.0, 6.0],
    	 [7.0, 8.0, 1.0, 2.0]], dtype=float)
    
    a rolled up:
     array([[6.0, 3.0, 4.0, 5.0],
    	 [2.0, 7.0, 8.0, 1.0]], dtype=float)
    
    a rolled with None:
     array([[3.0, 4.0, 5.0, 2.0],
    	 [7.0, 8.0, 1.0, 6.0]], dtype=float)
    
    


Simple running weighted average
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As a demonstration of the conciseness of ``ulab/numpy`` operations, we
will calculate an exponentially weighted running average of a
measurement vector in just a couple of lines. I chose this particular
example, because I think that this can indeed be used in real-life
applications.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import numerical
    from ulab import vector
    
    def dummy_adc():
        # dummy adc function, so that the results are reproducible
        return 2
        
    n = 10
    # These are the normalised weights; the last entry is the most dominant
    weight = vector.exp([1, 2, 3, 4, 5])
    weight = weight/numerical.sum(weight)
    
    print(weight)
    # initial array of samples
    samples = np.array([0]*n)
    
    for i in range(n):
        # a new datum is inserted on the right hand side. This simply overwrites whatever was in the last slot
        samples[-1] = dummy_adc()
        print(numerical.mean(samples[-5:]*weight))
        print(samples[-5:])
        # the data are shifted by one position to the left
        numerical.roll(samples, 1)

.. parsed-literal::

    array([0.01165623031556606, 0.03168492019176483, 0.08612854033708572, 0.234121635556221, 0.6364086270332336], dtype=float)
    0.2545634508132935
    array([0.0, 0.0, 0.0, 0.0, 2.0], dtype=float)
    0.3482121050357819
    array([0.0, 0.0, 0.0, 2.0, 2.0], dtype=float)
    0.3826635211706161
    array([0.0, 0.0, 2.0, 2.0, 2.0], dtype=float)
    0.3953374892473221
    array([0.0, 2.0, 2.0, 2.0, 2.0], dtype=float)
    0.3999999813735485
    array([2.0, 2.0, 2.0, 2.0, 2.0], dtype=float)
    0.3999999813735485
    array([2.0, 2.0, 2.0, 2.0, 2.0], dtype=float)
    0.3999999813735485
    array([2.0, 2.0, 2.0, 2.0, 2.0], dtype=float)
    0.3999999813735485
    array([2.0, 2.0, 2.0, 2.0, 2.0], dtype=float)
    0.3999999813735485
    array([2.0, 2.0, 2.0, 2.0, 2.0], dtype=float)
    0.3999999813735485
    array([2.0, 2.0, 2.0, 2.0, 2.0], dtype=float)
    
    


flip
----

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.flip.html

The ``flip`` function takes one positional, an ``ndarray``, and one
keyword argument, ``axis = None``, and reverses the order of elements
along the given axis. If the keyword argument is ``None``, the matrix’
entries are flipped along all axes. ``flip`` returns a new copy of the
array.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import numerical
    
    a = np.array([1, 2, 3, 4, 5])
    print("a: \t", a)
    print("a flipped:\t", np.flip(a))
    
    a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]], dtype=np.uint8)
    print("\na flipped horizontally\n", numerical.flip(a, axis=1))
    print("\na flipped vertically\n", numerical.flip(a, axis=0))
    print("\na flipped horizontally+vertically\n", numerical.flip(a))

.. parsed-literal::

    a: 	 array([1.0, 2.0, 3.0, 4.0, 5.0], dtype=float)
    a flipped:	 array([5.0, 4.0, 3.0, 2.0, 1.0], dtype=float)
    
    a flipped horizontally
     array([[3, 2, 1],
    	 [6, 5, 4],
    	 [9, 8, 7]], dtype=uint8)
    
    a flipped vertically
     array([[7, 8, 9],
    	 [4, 5, 6],
    	 [1, 2, 3]], dtype=uint8)
    
    a flipped horizontally+vertically
     array([[9, 8, 7],
    	 [6, 5, 4],
    	 [3, 2, 1]], dtype=uint8)
    
    


diff
----

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.diff.html

The ``diff`` function returns the numerical derivative of the forward
scheme, or more accurately, the differences of an ``ndarray`` along a
given axis. The order of derivative can be stipulated with the ``n``
keyword argument, which should be between 0, and 9. Default is 1. If
higher order derivatives are required, they can be gotten by repeated
calls to the function. The ``axis`` keyword argument should be -1 (last
axis, in ``ulab`` equivalent to the second axis, and this also happens
to be the default value), 0, or 1.

Beyond the output array, the function requires only a couple of bytes of
extra RAM for the differentiation stencil. (The stencil is an ``int8``
array, one byte longer than ``n``. This also explains, why the highest
order is 9: the coefficients of a ninth-order stencil all fit in signed
bytes, while 10 would require ``int16``.) Note that as usual in
numerical differentiation (and also in ``numpy``), the length of the
respective axis will be reduced by ``n`` after the operation. If ``n``
is larger than, or equal to the length of the axis, an empty array will
be returned.

**WARNING**: the ``diff`` function does not implement the ``prepend``
and ``append`` keywords that can be found in ``numpy``.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import numerical
    
    a = np.array(range(9), dtype=np.uint8)
    print('a:\n', a)
    
    print('\nfirst derivative:\n', numerical.diff(a, n=1))
    print('\nsecond derivative:\n', numerical.diff(a, n=2))
    
    c = np.array([[1, 2, 3, 4], [4, 3, 2, 1], [1, 4, 9, 16], [0, 0, 0, 0]])
    print('\nc:\n', c)
    print('\nfirst derivative, first axis:\n', numerical.diff(c, axis=0))
    print('\nfirst derivative, second axis:\n', numerical.diff(c, axis=1))

.. parsed-literal::

    a:
     array([0, 1, 2, 3, 4, 5, 6, 7, 8], dtype=uint8)
    
    first derivative:
     array([1, 1, 1, 1, 1, 1, 1, 1], dtype=uint8)
    
    second derivative:
     array([0, 0, 0, 0, 0, 0, 0], dtype=uint8)
    
    c:
     array([[1.0, 2.0, 3.0, 4.0],
    	 [4.0, 3.0, 2.0, 1.0],
    	 [1.0, 4.0, 9.0, 16.0],
    	 [0.0, 0.0, 0.0, 0.0]], dtype=float)
    
    first derivative, first axis:
     array([[3.0, 1.0, -1.0, -3.0],
    	 [-3.0, 1.0, 7.0, 15.0],
    	 [-1.0, -4.0, -9.0, -16.0]], dtype=float)
    
    first derivative, second axis:
     array([[1.0, 1.0, 1.0],
    	 [-1.0, -1.0, -1.0],
    	 [3.0, 5.0, 7.0],
    	 [0.0, 0.0, 0.0]], dtype=float)
    
    


sort
----

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.sort.html

The sort function takes an ndarray, and sorts its elements in ascending
order along the specified axis using a heap sort algorithm. As opposed
to the ``.sort()`` method discussed earlier, this function creates a
copy of its input before sorting, and at the end, returns this copy.
Sorting takes place in place, without auxiliary storage. The ``axis``
keyword argument takes on the possible values of -1 (the last axis, in
``ulab`` equivalent to the second axis, and this also happens to be the
default value), 0, 1, or ``None``. The first three cases are identical
to those in `diff <#diff>`__, while the last one flattens the array
before sorting.

If descending order is required, the result can simply be ``flip``\ ped,
see `flip <#flip>`__.

**WARNING:** ``numpy`` defines the ``kind``, and ``order`` keyword
arguments that are not implemented here. The function in ``ulab`` always
uses heap sort, and since ``ulab`` does not have the concept of data
fields, the ``order`` keyword argument would have no meaning.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import numerical
    
    a = np.array([[1, 12, 3, 0], [5, 3, 4, 1], [9, 11, 1, 8], [7, 10, 0, 1]], dtype=np.float)
    print('\na:\n', a)
    b = numerical.sort(a, axis=0)
    print('\na sorted along vertical axis:\n', b)
    
    c = numerical.sort(a, axis=1)
    print('\na sorted along horizontal axis:\n', c)
    
    c = numerical.sort(a, axis=None)
    print('\nflattened a sorted:\n', c)

.. parsed-literal::

    
    a:
     array([[1.0, 12.0, 3.0, 0.0],
    	 [5.0, 3.0, 4.0, 1.0],
    	 [9.0, 11.0, 1.0, 8.0],
    	 [7.0, 10.0, 0.0, 1.0]], dtype=float)
    
    a sorted along vertical axis:
     array([[1.0, 3.0, 0.0, 0.0],
    	 [5.0, 10.0, 1.0, 1.0],
    	 [7.0, 11.0, 3.0, 1.0],
    	 [9.0, 12.0, 4.0, 8.0]], dtype=float)
    
    a sorted along horizontal axis:
     array([[0.0, 1.0, 3.0, 12.0],
    	 [1.0, 3.0, 4.0, 5.0],
    	 [1.0, 8.0, 9.0, 11.0],
    	 [0.0, 1.0, 7.0, 10.0]], dtype=float)
    
    flattened a sorted:
     array([0.0, 0.0, 1.0, ..., 10.0, 11.0, 12.0], dtype=float)
    
    


Heap sort requires :math:`\sim N\log N` operations, and notably, the
worst case costs only 20% more time than the average. In order to get an
order-of-magnitude estimate, we will take the sine of 1000 uniformly
spaced numbers between 0, and two pi, and sort them:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import vector
    from ulab import numerical
    
    @timeit
    def sort_time(array):
        return numerical.sort(array)
    
    b = vector.sin(np.linspace(0, 6.28, num=1000))
    print('b: ', b)
    sort_time(b)
    print('\nb sorted:\n', b)
argsort
-------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.argsort.html

Similarly to `sort <#sort>`__, ``argsort`` takes a positional, and a
keyword argument, and returns an unsigned short index array of type
``ndarray`` with the same dimensions as the input, or, if ``axis=None``,
as a row vector with length equal to the number of elements in the input
(i.e., the flattened array). The indices in the output sort the input in
ascending order. The routine in ``argsort`` is the same as in ``sort``,
therefore, the comments on computational expenses (time and RAM) also
apply. In particular, since no copy of the original data is required,
virtually no RAM beyond the output array is used.

Since the underlying container of the output array is of type
``uint16_t``, neither of the output dimensions should be larger than
65535.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import numerical
    
    a = np.array([[1, 12, 3, 0], [5, 3, 4, 1], [9, 11, 1, 8], [7, 10, 0, 1]], dtype=np.float)
    print('\na:\n', a)
    b = numerical.argsort(a, axis=0)
    print('\na sorted along vertical axis:\n', b)
    
    c = numerical.argsort(a, axis=1)
    print('\na sorted along horizontal axis:\n', c)
    
    c = numerical.argsort(a, axis=None)
    print('\nflattened a sorted:\n', c)

.. parsed-literal::

    
    a:
     array([[1.0, 12.0, 3.0, 0.0],
    	 [5.0, 3.0, 4.0, 1.0],
    	 [9.0, 11.0, 1.0, 8.0],
    	 [7.0, 10.0, 0.0, 1.0]], dtype=float)
    
    a sorted along vertical axis:
     array([[0, 1, 3, 0],
    	 [1, 3, 2, 1],
    	 [3, 2, 0, 3],
    	 [2, 0, 1, 2]], dtype=uint16)
    
    a sorted along horizontal axis:
     array([[3, 0, 2, 1],
    	 [3, 1, 2, 0],
    	 [2, 3, 0, 1],
    	 [2, 3, 0, 1]], dtype=uint16)
    
    flattened a sorted:
     array([3, 14, 0, ..., 13, 9, 1], dtype=uint16)
    
    


Since during the sorting, only the indices are shuffled, ``argsort``
does not modify the input array, as one can verify this by the following
example:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import numerical
    
    a = np.array([0, 5, 1, 3, 2, 4], dtype=np.uint8)
    print('\na:\n', a)
    b = numerical.argsort(a, axis=1)
    print('\nsorting indices:\n', b)
    print('\nthe original array:\n', a)

.. parsed-literal::

    
    a:
     array([0, 5, 1, 3, 2, 4], dtype=uint8)
    
    sorting indices:
     array([0, 2, 4, 3, 5, 1], dtype=uint16)
    
    the original array:
     array([0, 5, 1, 3, 2, 4], dtype=uint8)
    
    


Linalg
======

Functions in the ``linalg`` module can be called by importing the
sub-module first.

size
----

``size`` takes a single argument, the axis, whose size is to be
returned. Depending on the value of the argument, the following
information will be returned:

1. argument is 0: the number of elements of the array
2. argument is 1: the number of rows
3. argument is 2: the number of columns

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import linalg
    
    a = np.array([1, 2, 3, 4], dtype=np.int8)
    print("a:\n", a)
    print("size of a:", linalg.size(a, axis=None), ",", linalg.size(a, axis=0))
    
    b= np.array([[1, 2], [3, 4]], dtype=np.int8)
    print("\nb:\n", b)
    print("size of b:", linalg.size(b, axis=None), ",", linalg.size(b, axis=0), ",", linalg.size(b, axis=1))

.. parsed-literal::

    a:
     array([1, 2, 3, 4], dtype=int8)
    size of a: 4 , 4
    
    b:
     array([[1, 2],
    	 [3, 4]], dtype=int8)
    size of b: 4 , 2 , 2
    
    


inv
---

``numpy``:
https://docs.scipy.org/doc/numpy-1.17.0/reference/generated/numpy.linalg.inv.html

A square matrix, provided that it is not singular, can be inverted by
calling the ``inv`` function that takes a single argument. The inversion
is based on successive elimination of elements in the lower left
triangle, and raises a ``ValueError`` exception, if the matrix turns out
to be singular (i.e., one of the diagonal entries is zero).

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import linalg
    
    m = np.array([[1, 2, 3, 4], [4, 5, 6, 4], [7, 8.6, 9, 4], [3, 4, 5, 6]])
    
    print(linalg.inv(m))

.. parsed-literal::

    array([[-2.166666, 1.499999, -0.8333326, 1.0],
    	 [1.666666, -3.333331, 1.666666, -4.768516e-08],
    	 [0.1666672, 2.166666, -0.8333327, -1.0],
    	 [-0.1666666, -0.3333334, 4.96705e-08, 0.5]], dtype=float)
    


Computation expenses
~~~~~~~~~~~~~~~~~~~~

Note that the cost of inverting a matrix is approximately twice as many
floats (RAM), as the number of entries in the original matrix, and
approximately as many operations, as the number of entries. Here are a
couple of numbers:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import linalg
    
    @timeit
    def invert_matrix(m):
        return linalg.inv(m)
    
    m = np.array([[1, 2,], [4, 5]])
    print('2 by 2 matrix:')
    invert_matrix(m)
    
    m = np.array([[1, 2, 3, 4], [4, 5, 6, 4], [7, 8.6, 9, 4], [3, 4, 5, 6]])
    print('\n4 by 4 matrix:')
    invert_matrix(m)
    
    m = np.array([[1, 2, 3, 4, 5, 6, 7, 8], [0, 5, 6, 4, 5, 6, 4, 5], 
                  [0, 0, 9, 7, 8, 9, 7, 8], [0, 0, 0, 10, 11, 12, 11, 12], 
                 [0, 0, 0, 0, 4, 6, 7, 8], [0, 0, 0, 0, 0, 5, 6, 7], 
                 [0, 0, 0, 0, 0, 0, 7, 6], [0, 0, 0, 0, 0, 0, 0, 2]])
    print('\n8 by 8 matrix:')
    invert_matrix(m)

.. parsed-literal::

    2 by 2 matrix:
    execution time:  65  us
    
    4 by 4 matrix:
    execution time:  105  us
    
    8 by 8 matrix:
    execution time:  299  us
    


The above-mentioned scaling is not obeyed strictly. The reason for the
discrepancy is that the function call is still the same for all three
cases: the input must be inspected, the output array must be created,
and so on.

dot
---

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.dot.html

**WARNING:** numpy applies upcasting rules for the multiplication of
matrices, while ``ulab`` simply returns a float matrix.

Once you can invert a matrix, you might want to know, whether the
inversion is correct. You can simply take the original matrix and its
inverse, and multiply them by calling the ``dot`` function, which takes
the two matrices as its arguments. If the matrix dimensions do not
match, the function raises a ``ValueError``. The result of the
multiplication is expected to be the unit matrix, which is demonstrated
below.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import linalg
    
    m = np.array([[1, 2, 3], [4, 5, 6], [7, 10, 9]], dtype=np.uint8)
    n = linalg.inv(m)
    print("m:\n", m)
    print("\nm^-1:\n", n)
    # this should be the unit matrix
    print("\nm*m^-1:\n", linalg.dot(m, n))

.. parsed-literal::

    m:
     array([[1, 2, 3],
    	 [4, 5, 6],
    	 [7, 10, 9]], dtype=uint8)
    
    m^-1:
     array([[-1.25, 1.0, -0.25],
    	 [0.5, -1.0, 0.5],
    	 [0.4166667, 0.3333334, -0.25]], dtype=float)
    
    m*m^-1:
     array([[1.0, 2.384186e-07, -1.490116e-07],
    	 [-2.980232e-07, 1.000001, -4.172325e-07],
    	 [-3.278255e-07, 1.311302e-06, 0.9999992]], dtype=float)
    


Note that for matrix multiplication you don’t necessarily need square
matrices, it is enough, if their dimensions are compatible (i.e., the
the left-hand-side matrix has as many columns, as does the
right-hand-side matrix rows):

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import linalg
    
    m = np.array([[1, 2, 3, 4], [5, 6, 7, 8]], dtype=np.uint8)
    n = np.array([[1, 2], [3, 4], [5, 6], [7, 8]], dtype=np.uint8)
    print(m)
    print(n)
    print(linalg.dot(m, n))

.. parsed-literal::

    array([[1, 2, 3, 4],
    	 [5, 6, 7, 8]], dtype=uint8)
    array([[1, 2],
    	 [3, 4],
    	 [5, 6],
    	 [7, 8]], dtype=uint8)
    array([[7.0, 10.0],
    	 [23.0, 34.0]], dtype=float)
    
    


det
---

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.linalg.det.html

The ``det`` function takes a square matrix as its single argument, and
calculates the determinant. The calculation is based on successive
elimination of the matrix elements, and the return value is a float,
even if the input array was of integer type.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import linalg
    
    a = np.array([[1, 2], [3, 4]], dtype=np.uint8)
    print(linalg.det(a))

.. parsed-literal::

    -2.0
    


Benchmark
~~~~~~~~~

Since the routine for calculating the determinant is pretty much the
same as for finding the `inverse of a matrix <#inv>`__, the execution
times are similar:

.. code::
        
    # code to be run in micropython
    
    @timeit
    def matrix_det(m):
        return linalg.inv(m)
    
    m = np.array([[1, 2, 3, 4, 5, 6, 7, 8], [0, 5, 6, 4, 5, 6, 4, 5], 
                  [0, 0, 9, 7, 8, 9, 7, 8], [0, 0, 0, 10, 11, 12, 11, 12], 
                 [0, 0, 0, 0, 4, 6, 7, 8], [0, 0, 0, 0, 0, 5, 6, 7], 
                 [0, 0, 0, 0, 0, 0, 7, 6], [0, 0, 0, 0, 0, 0, 0, 2]])
    
    matrix_det(m)

.. parsed-literal::

    execution time:  294  us
    


eig
---

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.linalg.eig.html

The ``eig`` function calculates the eigenvalues and the eigenvectors of
a real, symmetric square matrix. If the matrix is not symmetric, a
``ValueError`` will be raised. The function takes a single argument, and
returns a tuple with the eigenvalues, and eigenvectors. With the help of
the eigenvectors, amongst other things, you can implement sophisticated
stabilisation routines for robots.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import linalg
    
    a = np.array([[1, 2, 1, 4], [2, 5, 3, 5], [1, 3, 6, 1], [4, 5, 1, 7]], dtype=np.uint8)
    x, y = linalg.eig(a)
    print('eigenvectors of a:\n', x)
    print('\neigenvalues of a:\n', y)

.. parsed-literal::

    eigenvectors of a:
     array([-1.165288, 0.8029362, 5.585626, 13.77673], dtype=float)
    
    eigenvalues of a:
     array([[0.8151754, -0.4499267, -0.1643907, 0.3256237],
    	 [0.2211193, 0.7847154, 0.08373602, 0.5729892],
    	 [-0.1340859, -0.3100657, 0.8742685, 0.3486182],
    	 [-0.5182822, -0.2926556, -0.4490192, 0.6664218]], dtype=float)
    


The same matrix diagonalised with ``numpy`` yields:

.. code::

    # code to be run in CPython
    
    a = array([[1, 2, 1, 4], [2, 5, 3, 5], [1, 3, 6, 1], [4, 5, 1, 7]], dtype=np.uint8)
    x, y = eig(a)
    print('eigenvectors of a:\n', x)
    print('\neigenvalues of a:\n', y)

.. parsed-literal::

    eigenvectors of a:
     [13.77672606 -1.16528837  0.80293655  5.58562576]
    
    eigenvalues of a:
     [[ 0.32561419  0.815156    0.44994112 -0.16446602]
     [ 0.57300777  0.22113342 -0.78469926  0.08372081]
     [ 0.34861093 -0.13401142  0.31007764  0.87427868]
     [ 0.66641421 -0.51832581  0.29266348 -0.44897499]]


When comparing results, we should keep two things in mind:

1. the eigenvalues and eigenvectors are not necessarily sorted in the
   same way
2. an eigenvector can be multiplied by an arbitrary non-zero scalar, and
   it is still an eigenvector with the same eigenvalue. This is why all
   signs of the eigenvector belonging to 5.58, and 0.80 are flipped in
   ``ulab`` with respect to ``numpy``. This difference, however, is of
   absolutely no consequence.

Computation expenses
~~~~~~~~~~~~~~~~~~~~

Since the function is based on `Givens
rotations <https://en.wikipedia.org/wiki/Givens_rotation>`__ and runs
till convergence is achieved, or till the maximum number of allowed
rotations is exhausted, there is no universal estimate for the time
required to find the eigenvalues. However, an order of magnitude can, at
least, be guessed based on the measurement below:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import linalg
    
    @timeit
    def matrix_eig(a):
        return linalg.eig(a)
    
    a = np.array([[1, 2, 1, 4], [2, 5, 3, 5], [1, 3, 6, 1], [4, 5, 1, 7]], dtype=np.uint8)
    
    matrix_eig(a)

.. parsed-literal::

    execution time:  111  us
    


Cholesky decomposition
----------------------

``numpy``:
https://docs.scipy.org/doc/numpy-1.17.0/reference/generated/numpy.linalg.cholesky.html

``cholesky`` takes a positive definite, symmetric square matrix as its
single argument, and returns *square root matrix* in the lower
triangular form. If the input argument does not fulfill the positivity
or symmetry condition, a ``ValueError`` is raised.

.. code::
        
    # code to be run in micropython
    
    import ulab
    from ulab import linalg
    
    a = ulab.array([[25, 15, -5], [15, 18,  0], [-5,  0, 11]])
    print('a: ', a)
    print('\n' + '='*20 + '\nCholesky decomposition\n', linalg.cholesky(a))

.. parsed-literal::

    a:  array([[25.0, 15.0, -5.0],
    	 [15.0, 18.0, 0.0],
    	 [-5.0, 0.0, 11.0]], dtype=float)
    
    ====================
    Cholesky decomposition
     array([[5.0, 0.0, 0.0],
    	 [3.0, 3.0, 0.0],
    	 [-1.0, 1.0, 3.0]], dtype=float)
    
    


trace
-----

``numpy``:
https://docs.scipy.org/doc/numpy-1.17.0/reference/generated/numpy.linalg.trace.html

The ``trace`` function returns the sum of the diagonal elements of a
square matrix. If the input argument is not a square matrix, an
exception will be raised.

The scalar so returned will inherit the type of the input array, i.e.,
integer arrays have integer trace, and floating point arrays a floating
point trace.

.. code::
        
    # code to be run in micropython
    
    import ulab
    from ulab import linalg
    
    a = ulab.array([[25, 15, -5], [15, 18,  0], [-5,  0, 11]], dtype=ulab.int8)
    print('a: ', a)
    print('\ntrace of a: ', linalg.trace(a))
    
    b = ulab.array([[25, 15, -5], [15, 18,  0], [-5,  0, 11]], dtype=ulab.float)
    
    print('='*20 + '\nb: ', b)
    print('\ntrace of b: ', linalg.trace(b))

.. parsed-literal::

    a:  array([[25, 15, -5],
    	 [15, 18, 0],
    	 [-5, 0, 11]], dtype=int8)
    
    trace of a:  54
    ====================
    b:  array([[25.0, 15.0, -5.0],
    	 [15.0, 18.0, 0.0],
    	 [-5.0, 0.0, 11.0]], dtype=float)
    
    trace of b:  54.0
    
    


Polynomials
===========

Functions in the polynomial sub-module can be invoked by importing the
module first.

polyval
-------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.polyval.html

``polyval`` takes two arguments, both arrays or other iterables.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import poly
    
    p = [1, 1, 1, 0]
    x = [0, 1, 2, 3, 4]
    print('coefficients: ', p)
    print('independent values: ', x)
    print('\nvalues of p(x): ', poly.polyval(p, x))
    
    # the same works with one-dimensional ndarrays
    a = np.array(x)
    print('\nndarray (a): ', a)
    print('value of p(a): ', poly.polyval(p, a))

.. parsed-literal::

    coefficients:  [1, 1, 1, 0]
    independent values:  [0, 1, 2, 3, 4]
    
    values of p(x):  array([0.0, 3.0, 14.0, 39.0, 84.0], dtype=float)
    
    ndarray (a):  array([0.0, 1.0, 2.0, 3.0, 4.0], dtype=float)
    value of p(a):  array([0.0, 3.0, 14.0, 39.0, 84.0], dtype=float)
    
    


polyfit
-------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.polyfit.html

polyfit takes two, or three arguments. The last one is the degree of the
polynomial that will be fitted, the last but one is an array or iterable
with the ``y`` (dependent) values, and the first one, an array or
iterable with the ``x`` (independent) values, can be dropped. If that is
the case, ``x`` will be generated in the function, assuming uniform
sampling.

If the length of ``x``, and ``y`` are not the same, the function raises
a ``ValueError``.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import poly
    
    x = np.array([0, 1, 2, 3, 4, 5, 6])
    y = np.array([9, 4, 1, 0, 1, 4, 9])
    print('independent values:\t', x)
    print('dependent values:\t', y)
    print('fitted values:\t\t', poly.polyfit(x, y, 2))
    
    # the same with missing x
    print('\ndependent values:\t', y)
    print('fitted values:\t\t', poly.polyfit(y, 2))

.. parsed-literal::

    independent values:	 array([0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0], dtype=float)
    dependent values:	 array([9.0, 4.0, 1.0, 0.0, 1.0, 4.0, 9.0], dtype=float)
    fitted values:		 array([1.0, -6.0, 9.000000000000004], dtype=float)
    
    dependent values:	 array([9.0, 4.0, 1.0, 0.0, 1.0, 4.0, 9.0], dtype=float)
    fitted values:		 array([1.0, -6.0, 9.000000000000004], dtype=float)
    
    


Execution time
~~~~~~~~~~~~~~

``polyfit`` is based on the inversion of a matrix (there is more on the
background in https://en.wikipedia.org/wiki/Polynomial_regression), and
it requires the intermediate storage of ``2*N*(deg+1)`` floats, where
``N`` is the number of entries in the input array, and ``deg`` is the
fit’s degree. The additional computation costs of the matrix inversion
discussed in `inv <#inv>`__ also apply. The example from above needs
around 150 microseconds to return:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import poly
    
    @timeit
    def time_polyfit(x, y, n):
        return poly.polyfit(x, y, n)
    
    x = np.array([0, 1, 2, 3, 4, 5, 6])
    y = np.array([9, 4, 1, 0, 1, 4, 9])
    
    time_polyfit(x, y, 2)

.. parsed-literal::

    execution time:  153  us
    


Fourier transforms
==================

Functions related to Fourier transforms can be called by importing the
``fft`` sub-module first.

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.fft.ifft.html

fft
---

Since ``ulab``\ ’s ``ndarray`` does not support complex numbers, the
invocation of the Fourier transform differs from that in ``numpy``. In
``numpy``, you can simply pass an array or iterable to the function, and
it will be treated as a complex array:

.. code::

    # code to be run in CPython
    
    fft.fft([1, 2, 3, 4, 1, 2, 3, 4])



.. parsed-literal::

    array([20.+0.j,  0.+0.j, -4.+4.j,  0.+0.j, -4.+0.j,  0.+0.j, -4.-4.j,
            0.+0.j])



**WARNING:** The array that is returned is also complex, i.e., the real
and imaginary components are cast together. In ``ulab``, the real and
imaginary parts are treated separately: you have to pass two
``ndarray``\ s to the function, although, the second argument is
optional, in which case the imaginary part is assumed to be zero.

**WARNING:** The function, as opposed to ``numpy``, returns a 2-tuple,
whose elements are two ``ndarray``\ s, holding the real and imaginary
parts of the transform separately.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import vector
    from ulab import fft
    
    x = np.linspace(0, 10, num=1024)
    y = vector.sin(x)
    z = np.zeros(len(x))
    
    a, b = fft.fft(x)
    print('real part:\t', a)
    print('\nimaginary part:\t', b)
    
    c, d = fft.fft(x, z)
    print('\nreal part:\t', c)
    print('\nimaginary part:\t', d)

.. parsed-literal::

    real part:	 array([5119.996, -5.004663, -5.004798, ..., -5.005482, -5.005643, -5.006577], dtype=float)
    
    imaginary part:	 array([0.0, 1631.333, 815.659, ..., -543.764, -815.6588, -1631.333], dtype=float)
    
    real part:	 array([5119.996, -5.004663, -5.004798, ..., -5.005482, -5.005643, -5.006577], dtype=float)
    
    imaginary part:	 array([0.0, 1631.333, 815.659, ..., -543.764, -815.6588, -1631.333], dtype=float)
    


ifft
----

The above-mentioned rules apply to the inverse Fourier transform. The
inverse is also normalised by ``N``, the number of elements, as is
customary in ``numpy``. With the normalisation, we can ascertain that
the inverse of the transform is equal to the original array.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import vector
    from ulab import fft
    
    x = np.linspace(0, 10, num=1024)
    y = vector.sin(x)
    
    a, b = fft.fft(y)
    
    print('original vector:\t', y)
    
    y, z = fft.ifft(a, b)
    # the real part should be equal to y
    print('\nreal part of inverse:\t', y)
    # the imaginary part should be equal to zero
    print('\nimaginary part of inverse:\t', z)

.. parsed-literal::

    original vector:	 array([0.0, 0.009775016, 0.0195491, ..., -0.5275068, -0.5357859, -0.5440139], dtype=float)
    
    real part of inverse:	 array([-2.980232e-08, 0.0097754, 0.0195494, ..., -0.5275064, -0.5357857, -0.5440133], dtype=float)
    
    imaginary part of inverse:	 array([-2.980232e-08, -1.451171e-07, 3.693752e-08, ..., 6.44871e-08, 9.34986e-08, 2.18336e-07], dtype=float)
    


Note that unlike in ``numpy``, the length of the array on which the
Fourier transform is carried out must be a power of 2. If this is not
the case, the function raises a ``ValueError`` exception.

spectrogram
-----------

In addition to the Fourier transform and its inverse, ``ulab`` also
sports a function called ``spectrogram``, which returns the absolute
value of the Fourier transform. This could be used to find the dominant
spectral component in a time series. The arguments are treated in the
same way as in ``fft``, and ``ifft``. In order to keep compatibility of
the core modules with ``numpy``, this function is defined in the
``extras`` sub-module.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import vector
    from ulab import extras
    
    x = np.linspace(0, 10, num=1024)
    y = vector.sin(x)
    
    a = extras.spectrogram(y)
    
    print('original vector:\t', y)
    print('\nspectrum:\t', a)

.. parsed-literal::

    original vector:	 array([0.0, 0.009775015390171337, 0.01954909674625918, ..., -0.5275140569487312, -0.5357931822978732, -0.5440211108893639], dtype=float)
    
    spectrum:	 array([187.8635087634579, 315.3112063607119, 347.8814873399374, ..., 84.45888934298905, 347.8814873399374, 315.3112063607118], dtype=float)
    
    


As such, ``spectrogram`` is really just a shorthand for
``np.sqrt(a*a + b*b)``:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import fft
    from ulab import vector
    from ulab import extras
    
    x = np.linspace(0, 10, num=1024)
    y = vector.sin(x)
    
    a, b = fft.fft(y)
    
    print('\nspectrum calculated the hard way:\t', vector.sqrt(a*a + b*b))
    
    a = extras.spectrogram(y)
    
    print('\nspectrum calculated the lazy way:\t', a)

.. parsed-literal::

    
    spectrum calculated the hard way:	 array([187.8641, 315.3125, 347.8804, ..., 84.4587, 347.8803, 315.3124], dtype=float)
    
    spectrum calculated the lazy way:	 array([187.8641, 315.3125, 347.8804, ..., 84.4587, 347.8803, 315.3124], dtype=float)
    


Computation and storage costs
-----------------------------

RAM
~~~

The FFT routine of ``ulab`` calculates the transform in place. This
means that beyond reserving space for the two ``ndarray``\ s that will
be returned (the computation uses these two as intermediate storage
space), only a handful of temporary variables, all floats or 32-bit
integers, are required.

Speed of FFTs
~~~~~~~~~~~~~

A comment on the speed: a 1024-point transform implemented in python
would cost around 90 ms, and 13 ms in assembly, if the code runs on the
pyboard, v.1.1. You can gain a factor of four by moving to the D series
https://github.com/peterhinch/micropython-fourier/blob/master/README.md#8-performance.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import vector
    from ulab import fft
    
    x = np.linspace(0, 10, num=1024)
    y = vector.sin(x)
    
    @timeit
    def np_fft(y):
        return fft.fft(y)
    
    a, b = np_fft(y)

.. parsed-literal::

    execution time:  1985  us
    


The C implementation runs in less than 2 ms on the pyboard (we have just
measured that), and has been reported to run in under 0.8 ms on the D
series board. That is an improvement of at least a factor of four.

Filter routines
===============

Functions in the ``filter`` module can be called by importing the
sub-module first.

convolve
--------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.convolve.html

Returns the discrete, linear convolution of two one-dimensional
sequences.

Only the ``full`` mode is supported, and the ``mode`` named parameter is
not accepted. Note that all other modes can be had by slicing a ``full``
result.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    from ulab import filter
    
    x = np.array((1,2,3))
    y = np.array((1,10,100,1000))
    
    print(filter.convolve(x, y))

.. parsed-literal::

    array([1.0, 12.0, 123.0, 1230.0, 2300.0, 3000.0], dtype=float)
    
    


sosfilt
-------

``scipy``:
https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.sosfilt.html

Filter data along one dimension using cascaded second-order sections.

The function takes two positional arguments, ``sos``, the filter
segments of length 6, and the one-dimensional, uniformly sample data set
to be filtered. Returns the filtered data, or the filtered data and the
final filter delays, if the ``zi`` keyword arguments is supplied. The
keyword argument be a float ``ndarray`` of shape ``(n_sections, 2)``. If
``zi`` is not passed to the function, the initial values are assumed to
be 0.

.. code::
        
    # code to be run in micropython
    
    import ulab
    from ulab import filter as filter
    
    x = ulab.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
    sos = [[1, 2, 3, 1, 5, 6], [1, 2, 3, 1, 5, 6]]
    y = filter.sosfilt(sos, x)
    print('y: ', y)

.. parsed-literal::

    y:  array([0.0, 1.0, -4.0, 24.0, -104.0, 440.0, -1728.0, 6532.000000000001, -23848.0, 84864.0], dtype=float)
    
    


.. code::
        
    # code to be run in micropython
    
    import ulab
    from ulab import filter as filter
    
    x = ulab.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
    sos = [[1, 2, 3, 1, 5, 6], [1, 2, 3, 1, 5, 6]]
    # initial conditions of the filter
    zi = ulab.array([[1, 2], [3, 4]])
    
    y, zf = filter.sosfilt(sos, x, zi=zi)
    print('y: ', y)
    print('\n' + '='*40 + '\nzf: ', zf)

.. parsed-literal::

    y:  array([4.0, -16.0, 63.00000000000001, -227.0, 802.9999999999999, -2751.0, 9271.000000000001, -30775.0, 101067.0, -328991.0000000001], dtype=float)
    
    ========================================
    zf:  array([[37242.0, 74835.0],
    	 [1026187.0, 1936542.0]], dtype=float)
    
    


Comparison of arrays
====================

Functions in the ``compare`` module can be called by importing the
sub-module first.

equal, not_equal
----------------

``numpy``:
https://numpy.org/doc/stable/reference/generated/numpy.equal.html

``numpy``:
https://numpy.org/doc/stable/reference/generated/numpy.not_equal.html

In ``micropython``, equality of arrays or scalars can be established by
utilising the ``==``, ``!=``, ``<``, ``>``, ``<=``, or ``=>`` binary
operators. In ``circuitpython``, ``==`` and ``!=`` will produce
unexpected results. In order to avoid this discrepancy, and to maintain
compatibility with ``numpy``, ``ulab`` implements the ``equal`` and
``not_equal`` operators that return the same results, irrespective of
the ``python`` implementation.

These two functions take two ``ndarray``\ s, or scalars as their
arguments. No keyword arguments are implemented.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array(range(9))
    b = np.zeros(9)
    
    print('a: ', a)
    print('b: ', b)
    print('\na == b: ', np.compare.equal(a, b))
    print('a != b: ', np.compare.not_equal(a, b))
    
    # comparison with scalars
    print('a == 2: ', np.compare.equal(a, 2))

.. parsed-literal::

    a:  array([0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0], dtype=float)
    b:  array([0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0], dtype=float)
    
    a == b:  [True, False, False, False, False, False, False, False, False]
    a != b:  [False, True, True, True, True, True, True, True, True]
    a == 2:  [False, False, True, False, False, False, False, False, False]
    
    


minimum
-------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.minimum.html

Returns the minimum of two arrays, or two scalars, or an array, and a
scalar. Partial broadcasting is implemented. If the arrays are of
different ``dtype``, the output is upcast as in `Binary
operators <#Binary-operators>`__. If both inputs are scalars, a scalar
is returned. Only positional arguments are implemented.

maximum
-------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.maximum.html

Returns the maximum of two arrays, or two scalars, or an array, and a
scalar. Partial broadcasting is implemented. If the arrays are of
different ``dtype``, the output is upcast as in `Binary
operators <#Binary-operators>`__. If both inputs are scalars, a scalar
is returned. Only positional arguments are implemented.

.. code::
        
    # code to be run in micropython
    
    import ulab
    
    a = ulab.array([1, 2, 3, 4, 5], dtype=ulab.uint8)
    b = ulab.array([5, 4, 3, 2, 1], dtype=ulab.float)
    print('minimum of a, and b:')
    print(ulab.compare.minimum(a, b))
    
    print('\nmaximum of a, and b:')
    print(ulab.compare.maximum(a, b))
    
    print('\nmaximum of 1, and 5.5:')
    print(ulab.compare.maximum(1, 5.5))

.. parsed-literal::

    minimum of a, and b:
    array([1.0, 2.0, 3.0, 2.0, 1.0], dtype=float)
    
    maximum of a, and b:
    array([5.0, 4.0, 3.0, 4.0, 5.0], dtype=float)
    
    maximum of 1, and 5.5:
    5.5
    
    


clip
----

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.clip.html

Clips an array, i.e., values that are outside of an interval are clipped
to the interval edges. The function is equivalent to
``maximum(a_min, minimum(a, a_max))``. or two scalars, hence partial
broadcasting takes place exactly as in `minimum <#minimum>`__. If the
arrays are of different ``dtype``, the output is upcast as in `Binary
operators <#Binary-operators>`__.

.. code::
        
    # code to be run in micropython
    
    import ulab
    
    a = ulab.array(range(9), dtype=ulab.uint8)
    print('a:\t\t', a)
    print('clipped:\t', ulab.compare.clip(a, 3, 7))
    
    b = 3 * ulab.ones(len(a), dtype=ulab.float)
    print('\na:\t\t', a)
    print('b:\t\t', b)
    print('clipped:\t', ulab.compare.clip(a, b, 7))

.. parsed-literal::

    a:		 array([0, 1, 2, 3, 4, 5, 6, 7, 8], dtype=uint8)
    clipped:	 array([3, 3, 3, 3, 4, 5, 6, 7, 7], dtype=uint8)
    
    a:		 array([0, 1, 2, 3, 4, 5, 6, 7, 8], dtype=uint8)
    b:		 array([3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0], dtype=float)
    clipped:	 array([3.0, 3.0, 3.0, 3.0, 4.0, 5.0, 6.0, 7.0, 7.0], dtype=float)
    
    


Interpolation, root finding, and function minimisation
======================================================

The ``approx`` sub-module defines functions for interpolating numerical
data, and finding the roots and the minimum of arbitrary functions
defined in ``python``. Note that routines that work with user-defined
functions still have to call the underlying ``python`` code, and
therefore, gains in speed are not as significant as with other
vectorised operations. As a rule of thumb, a factor of two can be
expected, when compared to an optimised python implementation.

interp
------

``numpy``: https://docs.scipy.org/doc/numpy/numpy.interp

The ``interp`` function returns the linearly interpolated values of a
one-dimensional numerical array. It requires three positional
arguments,\ ``x``, at which the interpolated values are evaluated,
``xp``, the array of the independent variables of the data, and ``fp``,
the array of the dependent values of the data. ``xp`` must be a
monotonically increasing sequence of numbers.

Two keyword arguments, ``left``, and ``right`` can also be supplied;
these determine the return values, if ``x < xp[0]``, and ``x > xp[-1]``,
respectively. If these arguments are not supplied, ``left``, and
``right`` default to ``fp[0]``, and ``fp[-1]``, respectively.

.. code::
        
    # code to be run in micropython
    
    import ulab
    from ulab import approx
    
    x = ulab.array([1, 2, 3, 4, 5])
    xp = ulab.array([1, 2, 3, 4])
    fp = ulab.array([1, 2, 3, 5])
    x = x - 0.2
    print(x)
    print(approx.interp(x, xp, fp))
    print(approx.interp(x, xp, fp, left=0.0))
    print(approx.interp(x, xp, fp, right=10.0))

.. parsed-literal::

    array([0.8, 1.8, 2.8, 3.8, 4.8], dtype=float)
    array([1.0, 1.8, 2.8, 4.6, 5.0], dtype=float)
    array([0.0, 1.8, 2.8, 4.6, 5.0], dtype=float)
    array([1.0, 1.8, 2.8, 4.6, 10.0], dtype=float)
    
    


newton
------

``scipy``:https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.newton.html

``newton`` finds a zero of a real, user-defined function using the
Newton-Raphson (or secant or Halley’s) method. The routine requires two
positional arguments, the function, and the initial value. Three keyword
arguments can be supplied to control the iteration. These are the
absolute and relative tolerances ``tol``, and ``rtol``, respectively,
and the number of iterations before stopping, ``maxiter``. The function
retuns a single scalar, the position of the root.

.. code::
        
    # code to be run in micropython
    
    import ulab
    from ulab import approx
        
    def f(x):
        return x*x*x - 2.0
    
    print(approx.newton(f, 3., tol=0.001, rtol=0.01))

.. parsed-literal::

    1.260135727246117
    
    


bisect
------

``scipy``:
https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.bisect.html

``bisect`` finds the root of a function of one variable using a simple
bisection routine. It takes three positional arguments, the function
itself, and two starting points. The function must have opposite signs
at the starting points. Returned is the position of the root.

Two keyword arguments, ``xtol``, and ``maxiter`` can be supplied to
control the accuracy, and the number of bisections, respectively.

.. code::
        
    # code to be run in micropython
    
    import ulab
    from ulab import approx
        
    def f(x):
        return x*x - 1
    
    print(approx.bisect(f, 0, 4))
    
    print('only 8 bisections: ',  approx.bisect(f, 0, 4, maxiter=8))
    
    print('with 0.1 accuracy: ',  approx.bisect(f, 0, 4, xtol=0.1))

.. parsed-literal::

    0.9999997615814209
    only 8 bisections:  0.984375
    with 0.1 accuracy:  0.9375
    
    


Performance
~~~~~~~~~~~

Since the ``bisect`` routine calls user-defined ``python`` functions,
the speed gain is only about a factor of two, if compared to a purely
``python`` implementation.

.. code::
        
    # code to be run in micropython
    
    import ulab
    from ulab import approx
    
    def f(x):
        return (x-1)*(x-1) - 2.0
    
    def bisect(f, a, b, xtol=2.4e-7, maxiter=100):
        if f(a) * f(b) > 0:
            raise ValueError
    
        rtb = a if f(a) < 0.0 else b
        dx = b - a if f(a) < 0.0 else a - b
        for i in range(maxiter):
            dx *= 0.5
            x_mid = rtb + dx
            mid_value = f(x_mid)
            if mid_value < 0:
                rtb = x_mid
            if abs(dx) < xtol:
                break
    
        return rtb
    
    @timeit
    def bisect_approx(f, a, b):
        return approx.bisect(f, a, b)
    
    @timeit
    def bisect_timed(f, a, b):
        return bisect(f, a, b)
    
    print('bisect running in python')
    bisect_timed(f, 3, 2)
    
    print('bisect running in C')
    bisect_approx(f, 3, 2)

.. parsed-literal::

    bisect running in python
    execution time:  1270  us
    bisect running in C
    execution time:  642  us
    


fmin
----

``scipy``:
https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.fmin.html

The ``fmin`` function finds the position of the minimum of a
user-defined function by using the downhill simplex method. Requires two
positional arguments, the function, and the initial value. Three keyword
arguments, ``xatol``, ``fatol``, and ``maxiter`` stipulate conditions
for stopping.

.. code::
        
    # code to be run in micropython
    import ulab
    from ulab import approx
    
    def f(x):
        return (x-1)**2 - 1
    
    print(approx.fmin(f, 3.0))
    print(approx.fmin(f, 3.0, xatol=0.1))

.. parsed-literal::

    0.9996093749999952
    1.199999999999996
    
    


Extending ulab
==============

As mentioned at the beginning, ``ulab`` is a set of sub-modules, out of
which one, ``extra`` is explicitly reserved for user code. You should
implement your functions in this sub-module, or sub-modules thereof.

The new functions can easily be added to ``extra`` in a couple of simple
steps. At the C level, the type definition of an ``ndarray`` is as
follows:

.. code:: c

   typedef struct _ndarray_obj_t {
       mp_obj_base_t base;
       size_t m, n;
       mp_obj_array_t *array;
       size_t bytes;
   } ndarray_obj_t;

Creating a new ndarray
----------------------

A new ``ndarray`` can be created by calling

.. code:: c

   ndarray_obj_t *new_ndarray = create_new_ndarray(m, n, typecode);

where ``m``, and ``n`` are the number of rows and columns, respectively,
and ``typecode`` is one of the values from

.. code:: c

   enum NDARRAY_TYPE {
       NDARRAY_UINT8 = 'B',
       NDARRAY_INT8 = 'b',
       NDARRAY_UINT16 = 'H', 
       NDARRAY_INT16 = 'h',
       NDARRAY_FLOAT = 'f',
   };

or

.. code:: c

   enum NDARRAY_TYPE {
       NDARRAY_UINT8 = 'B',
       NDARRAY_INT8 = 'b',
       NDARRAY_UINT16 = 'H', 
       NDARRAY_INT16 = 'h',
       NDARRAY_FLOAT = 'd',
   };

The ambiguity is caused by the fact that not all platforms implement
``double``, and there one has to take ``float``\ s. But you haven’t
actually got to be concerned by this, because at the very beginning of
``ndarray.h``, this is already taken care of: the pre-processor figures
out, what the ``float`` implementation of the hardware platform is, and
defines the ``NDARRAY_FLOAT`` typecode accordingly. All you have to keep
in mind is that wherever you would use ``float`` or ``double``, you have
to use ``mp_float_t``. That type is defined in ``py/mpconfig.h`` of the
micropython code base.

Therefore, a 4-by-5 matrix of type float can be created as

.. code:: c

   ndarray_obj_t *new_ndarray = create_new_ndarray(4, 5, NDARRAY_FLOAT);

This function also fills in the ``ndarray`` structure’s ``m``, ``n``,
and ``bytes`` members, as well initialises the ``array`` member with
zeros.

Alternatively, a one-to-one copy of an ``ndarray`` can be gotten by
calling

.. code:: c

   mp_obj_t copy_of_input_object = ndarray_copy(object_in);

Note, however, that this function takes an input object of type
``mp_obj_t``, and returns a copy of type ``mp_obj_t``, i.e., something
that can be taken from, and can immediately be returned to the
interpreter. If you want to work on the data in the copy, you still have
to create a pointer to it

.. code:: c

   ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(copy_of_input_object);

The values stored in ``array`` can be modified or retrieved by accessing
``array->items``. Note that ``array->items`` is a void pointer,
therefore, it must be cast before trying to access the elements.
``array`` has at least two useful members. One of them is ``len``, which
is the number of elements that the array holds, while the second one is
the ``typecode`` that we passed to ``create_new_ndarray`` earlier.

Thus, staying with our example of a 4-by-5 float matrix, we can loop
through all entries as

.. code:: c

   mp_float_t *items = (mp_float_t *)new_ndarray->array->items;
   mp_float_t item;

   for(size_t i=0; i < new_ndarray->array->len; i++) {
       item = items[i];
       // do something with item...
   }

or, since the data are stored in the pointer in a C-like fashion, as

.. code:: c

   mp_float_t *items = (mp_float_t *)new_ndarray->array->items;
   mp_float_t item;

   for(size_t m=0; m < new_ndarray->m; m++) { // loop through the rows
       for(size_t n=0; n < new_ndarray->n; n++) { // loop through the columns
           item = items[m*new_ndarray->n+n]; // get the (m,n) entry
           // do something with item...
       }
   }

Accessing data in the ndarray
-----------------------------

We have already seen, how the entries of an array can be accessed. If
the object in question comes from the user (i.e., via the micropython
interface), we can get a pointer to it by calling

.. code:: c

   ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(object_in);

Once the pointer is at our disposal, we can get a pointer to the
underlying numerical array as discussed earlier.

If you need to find out the typecode of the array, you can get it by
accessing the ``typecode`` member of ``array``, i.e.,

.. code:: c

   ndarray->array->typecode

should be equal to ``B``, ``b``, ``H``, ``h``, or ``f``. The size of a
single item is returned by the function
``mp_binary_get_size('@', ndarray->array->typecode, NULL)``. This number
is equal to 1, if the typecode is ``B``, or ``b``, 2, if the typecode is
``H``, or ``h``, 4, if the typecode is ``f``, and 8 for ``d``.

Alternatively, the size can be found out by dividing ``ndarray->bytes``
with the product of ``m``, and ``n``, i.e.,

.. code:: c

   ndarray->bytes/(ndarray->m*ndarray*n) 

is equal to ``mp_binary_get_size('@', ndarray->array->typecode, NULL)``.

Making certain that we have an ndarray
--------------------------------------

A number of operations make sense for ``ndarray``\ s only, therefore,
before doing any heavy work on the data, it might be reasonable to
check, whether the input argument is of the proper type. This you do by
evaluating

.. code:: c

   mp_obj_is_type(object_in, &ulab_ndarray_type)

which should return ``true``.

Boilerplate of sorts
--------------------

To summarise the contents of the previous three sections, here is a
useless function that prints out the size (``m``, and ``n``) of an
array, creates a copy of the input, and fills up the resulting matrix
with 13.

.. code:: c

   mp_obj_t useless_function(mp_obj_t object_in) {
       if(!mp_obj_is_type(object_in, &ulab_ndarray_type)) {
           mp_raise_TypeError("useless_function takes only ndarray arguments");
       }
       
       mp_obj_t object_out = ndarray_copy(object_int);
       
       ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(object_out);
       printf("\nsize (m): %ld, size (n): %ld\n", ndarray->m, ndarray->n);
       printf("\nlength (len): %ld, typecode: %d\n", ndarray->array->len, ndarray->array->typecode);
       if(ndarray->array->typecode == NDARRAY_UINT8) {
           // cast the pointer to the items
           uint8_t *items = (uint8_t *)ndarray->array->items;
           // retrieve the length of the array, and loop over the elements
           for(size_t i=0; i < ndarray->array->len; i++) items[i] = 13;
       } else if(ndarray->array->typecode == NDARRAY_INT8) {
           int8_t *items = (int8_t *)ndarray->array->items;
           for(size_t i=0; i < ndarray->array->len; i++) items[i] = 13;
       } else if(ndarray->array->typecode == NDARRAY_UINT16) {
           uint16_t *items = (uint16_t *)ndarray->array->items;
           for(size_t i=0; i < ndarray->array->len; i++) items[i] = 13;
       } else if(ndarray->array->typecode == NDARRAY_INT16) {
           int16_t *items = (int16_t *)ndarray->array->items;
           for(size_t i=0; i < ndarray->array->len; i++) items[i] = 13;
       } else {
           float *items = (mp_float_t *)ndarray->array->items;
           for(size_t i=0; i < ndarray->array->len; i++) items[i] = 13;
       }
       return object_out;
   }

If, on the other hand, you want to create an ``ndarray`` from scratch,
and return that, you could work along the following lines:

.. code:: c

   mp_obj_t useless_function(mp_obj_t object_in) {
       uint16_t m = mp_obj_get_int(object_in);
       
       ndarray_obj_t *ndarray = create_new_ndarray(1, m, NDARRAY_UINT8);
       
       uint8_t *items = (uint8_t *)ndarray->array->items;
       // do something with the array's entries
       // ...
       
       // and at the very end, return an mp_object_t
       return MP_PTR_TO_OBJ(ndarray);
   }

Once the function implementation is done, you should add the function
object to the globals dictionary of the ``extra`` sub-module as

.. code:: c

   ...
       MP_DEFINE_CONST_FUN_OBJ_1(useless_function_obj, userless_function);
   ...
       STATIC const mp_map_elem_t extra_globals_table[] = {
   ...
       { MP_OBJ_NEW_QSTR(MP_QSTR_useless), (mp_obj_t)&useless_function_obj },
   ...
   };

The exact form of the function object definition

.. code:: c

       MP_DEFINE_CONST_FUN_OBJ_1(useless_function_obj, userless_function);

depends naturally on what exactly you implemented, i.e., how many
arguments the function takes, whether only positional arguments allowed
and so on. For a thorough discussion on how function objects have to be
defined, see, e.g., the `user module programming
manual <#https://micropython-usermod.readthedocs.io/en/latest/>`__.

And with that, you are done. You can simply call the compiler as

.. code:: bash

   make BOARD=PYBV11 USER_C_MODULES=../../../ulab all

and the rest is taken care of.

In the boilerplate above, we cast the pointer to ``array->items`` to the
required type. There are certain operations, however, when you do not
need the casting. If you do not want to change the array’s values, only
their position within the array, you can get away with copying the
memory content, regardless the type. A good example for such a scenario
is the transpose function in
https://github.com/v923z/micropython-ulab/blob/master/code/linalg.c.

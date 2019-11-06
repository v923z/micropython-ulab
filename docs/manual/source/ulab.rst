Introduction
============

In
https://micropython-usermod.readthedocs.io/en/latest/usermods_14.html, I
mentioned that I have another story, for another day. The day has come,
so here is my story.

Enter ulab
----------

``ulab`` is a numpy-like module for micropython, meant to simplify and
speed up common mathematical operations on arrays. The primary goal was
to implement a small subset of numpy that might be useful in the context
of a microcontroller. This means low-level data processing of linear
(array) and two-dimensional (matrix) data.

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
initiator of this project: I needed the Fourier transform of the ADC
signal, and all the available options were simply too slow.

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

Finally, there is a reason for using micropython in the first place.
Namely, that a microcontroller can be programmed in a very elegant, and
*pythonic* way. But if it is so, why should we not extend this idea to
other tasks and concepts that might come up in this context? If there
was no other reason than this *elegance*, I would find that convincing
enough.

Based on the above-mentioned considerations, all functions are
implemented in a way that

1. conforms to ``numpy`` as much as possible
2. is so frugal with RAM as possible,
3. and yet, fast. Much faster than pure python.

The main points of ``ulab`` are

-  compact, iterable and slicable containers of numerical data in 1, and
   2 dimensions (arrays and matrices). These containers support all the
   relevant unary and binary operators (e.g., ``len``, ==, +, \*, etc.)
-  vectorised computations on micropython iterables and numerical
   arrays/matrices (in numpy-speak, universal functions)
-  basic linear algebra routines (matrix inversion, multiplication,
   reshaping, transposition, determinant, and eigenvalues)
-  polynomial fits to numerical data
-  fast Fourier transforms

At the time of writing this manual (for version 0.26), the library adds
approximately 30 kB of extra compiled code to the micropython
(pyboard.v.11) firmware.

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
please, raise an issue on github, so that the community can profit from
your experiences.

Even better, if you find the project useful, and think that it could be
made better, faster, tighter, and shinier, please, consider
contributing, and issue a pull request with the implementation of your
improvements and new features. ``ulab`` can only become successful, if
it offers what the community needs.

These last comments apply to the documentation, too. If, in your
opinion, the documentation is obscure, misleading, or not detailed
enough, please, let me know, so that *we* can fix it.

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

    you are running ulab version 0.24
    
    


If you find a bug, please, include this number in your report!

Basic ndarray operations
------------------------

`Unary operators <#Unary-operators>`__

`Binary operators <#Binary-operators>`__

`Indexing and slicing <#Slicing-and-indexing>`__

`ndarray iterators <#Iterating-over-arrays>`__

`Comparison operators\* <#Comparison-operators>`__

`Universal functions <#Universal-functions>`__ (also support function
calls on general iterables)

Methods of ndarrays
-------------------

`.shape <#.shape>`__

`.reshape <#.reshape>`__

`.rawsize\*\* <#.rawsize>`__

`.transpose <#.transpose>`__

`.flatten\*\* <#.flatten>`__

`.asbytearray <#.asbytearray>`__

Matrix methods
--------------

`size <#size>`__

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

Manipulation of polynomials
---------------------------

`polyval <#polyval>`__

`polyfit <#polyfit>`__

FFT routines
------------

`fft\*\* <#fft>`__

`ifft\*\* <#ifft>`__

`spectrum\*\* <#spectrum>`__

ndarray, the basic container
============================

The ``ndarray`` is the underlying container of numerical data. It is
derived from micropython’s own ``array`` object, but has a great number
of extra features starting with how it can be initialised, how
operations can be done on it, and which functions can accept it as an
argument.

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
the `.rawsize <#.rawsize>`__ class method.

On the following pages, we will see how one can work with
``ndarray``\ s. Those familiar with ``numpy`` should find that the
nomenclature and naming conventions of ``numpy`` are adhered to as
closely as possible. I will point out the few differences, where
necessary.

For the sake of comparison, in addition to ``ulab`` code snippets,
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
matrix will be created. If the lengths of the iterables is not
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
    


``ndarray``\ s are pretty-printed, i.e., if the length is larger than
10, then only the first and last three entries will be printed. Also
note that, as opposed to ``numpy``, the printout always contains the
``dtype``.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array(range(200))
    print("a:\t", a)

.. parsed-literal::

    a:	 array([0.0, 1.0, 2.0, ..., 197.0, 198.0, 199.0], dtype=float)
    
    


Initialising by passing arrays
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

An ``ndarray`` can be initialised by supplying another array. This
statement is almost trivial, since ``ndarray``\ s are iterables
themselves, though it should be pointed out that initialising through
arrays should be faster, because simply a new copy is created, without
inspection, iteration etc.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = [1, 2, 3, 4, 5, 6, 7, 8]
    b = np.array(a)
    c = np.array(b)
    
    print("a:\t", a)
    print("b:\t", b)
    print("\nc:\t", c)

.. parsed-literal::

    a:	 [1, 2, 3, 4, 5, 6, 7, 8]
    b:	 array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0], dtype=float)
    
    c:	 array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0], dtype=float)
    
    


Methods of ndarrays
-------------------

.shape
~~~~~~

The ``.shape`` method returns a 2-tuple with the number of rows, and
columns.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3, 4], dtype=np.int8)
    print("a:\n", a)
    print("shape of a:", a.shape())
    
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
    
    


.reshape
~~~~~~~~

numpy:
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
    
    


.rawsize
~~~~~~~~

The ``rawsize`` method of the ``ndarray`` returns a 5-tuple with the
following data

1. number of rows
2. number of columns
3. length of the storage (should be equal to the product of 1. and 2.)
4. length of the data storage in bytes
5. datum size in bytes (1 for ``uint8``/``int8``, 2 for
   ``uint16``/``int16``, and 4, or 8 for ``floats``, see `ndarray, the
   basic container <#ndarray,-the-basic-container>`__)

**WARNING:** ``rawsize`` is a ``ulab``-only method; it has no equivalent
in ``numpy``.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3, 4], dtype=np.float)
    print("a: \t\t", a)
    print("rawsize of a: \t", a.rawsize())

.. parsed-literal::

    a: 		 array([1.0, 2.0, 3.0, 4.0], dtype=float)
    rawsize of a: 	 (1, 4, 4, 16, 4)
    
    


.flatten
~~~~~~~~

numpy:
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
    
    


.asbytearray
~~~~~~~~~~~~

The contents of an ``ndarray`` can be accessed directly by calling the
``.asbytearray`` method. This will simply return a pointer to the
underlying flat ``array`` object, which can then be manipulated
directly.

**WARNING:** ``asbytearray`` is a ``ulab``-only method; it has no
equivalent in ``numpy``.

In the example below, note the difference between ``a``, and ``buffer``:
while both are designated as an array, you recognise the micropython
array from the fact that it prints the typecode (``b`` in this
particular case). The ``ndarray``, on the other hand, prints out the
``dtype`` (``int8`` here).

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3, 4], dtype=np.int8)
    print('a: ', a)
    buffer = a.asbytearray()
    print("array content:", buffer)
    buffer[1] = 123
    print("array content:", buffer)

.. parsed-literal::

    a:  array([1, 2, 3, 4], dtype=int8)
    array content: array('b', [1, 2, 3, 4])
    array content: array('b', [1, 123, 3, 4])
    
    


This in itself wouldn’t be very interesting, but since ``buffer`` is a
proper micropython ``array``, we can pass it to functions that can
employ the buffer protocol. E.g., all the ``ndarray`` facilities can be
applied to the results of timed ADC conversions.

.. code::
        
    # code to be run in micropython
    
    import pyb
    import ulab as np
    
    n = 100
    
    adc = pyb.ADC(pyb.Pin.board.X19)
    tim = pyb.Timer(6, freq=10)
    
    a = np.array([0]*n, dtype=np.uint8)
    buffer = a.asbytearray()
    adc.read_timed(buffer, tim)
    
    print("ADC results:\t", a)
    print("mean of results:\t", np.mean(a))
    print("std of results:\t", np.std(a))

.. parsed-literal::

    ADC results:	 array([48, 2, 2, ..., 0, 0, 0], dtype=uint8)
    mean of results:	 1.22
    std of results:	 4.744639
    


Likewise, data can be read directly into ``ndarray``\ s from other
interfaces, e.g., SPI, I2C etc, and also, by laying bare the
``ndarray``, we can pass results of ``ulab`` computations to anything
that can read from a buffer.

.transpose
~~~~~~~~~~

numpy:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.transpose.html

Note that only square matrices can be transposed in place, and in
general, an internal copy of the matrix is required. If RAM is a
concern, plan accordingly!

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

numpy:
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

The function function is defined for integer data types (``uint8``,
``int8``, ``uint16``, and ``int16``) only, takes a single argument, and
returns the element-by-element, bit-wise inverse of the array. If a
``float`` is supplied, the function raises a ``ValueError`` exception.

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

All binary operators work element-wise. This also means that the
operands either must have the same shape, or one of them must be a
scalar.

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

+----------------+-----------------+-------------+--------------+
| left hand side | right hand side | ulab result | numpy result |
+================+=================+=============+==============+
| ``uint8``      | ``int8``        | ``int16``   | ``int16``    |
+----------------+-----------------+-------------+--------------+
| ``uint8``      | ``int16``       | ``int16``   | ``int16``    |
+----------------+-----------------+-------------+--------------+
| ``uint8``      | ``uint16``      | ``uint16``  | ``uint16``   |
+----------------+-----------------+-------------+--------------+
| ``int8``       | ``int16``       | ``int16``   | ``int16``    |
+----------------+-----------------+-------------+--------------+
| ``int8``       | ``uint16``      | ``uint16``  | ``int32``    |
+----------------+-----------------+-------------+--------------+
| ``uint16``     | ``int16``       | ``float``   | ``int32``    |
+----------------+-----------------+-------------+--------------+

Note that the last two operations are promoted to ``int32`` in
``numpy``.

**WARNING:** Due to the lower number of available data types, the
upcasting rules of ``ulab`` are slightly different to those of
``numpy``. Watch out for this, when porting code!

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
    
    


**WARNING:** If a binary operation involves an ``ndarray`` and a
micropython type (integer, or float), then the array must be on the left
hand side.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    # this is going to work
    a = np.array([1, 2, 3, 4], dtype=np.uint8)
    b = 12
    print("a:\t", a)
    print("b:\t", b)
    print("a+b:\t", a+b)
    
    # but this will spectacularly fail
    print("b+a:\t", b+a)

.. parsed-literal::

    a:	 array([1, 2, 3, 4], dtype=uint8)
    b:	 12
    a+b:	 array([13, 14, 15, 16], dtype=uint8)
    
    Traceback (most recent call last):
      File "/dev/shm/micropython.py", line 12, in <module>
    TypeError: unsupported types for __add__: 'int', 'ndarray'
    


The reason for this lies in how micropython resolves binary operators,
and this means that a fix can only be implemented, if micropython itself
changes the corresponding function(s). Till then, keep ``ndarray``\ s on
the left hand side.

Benchmarks
~~~~~~~~~~

The following snippet compares the performance of binary operations to a
possible implementation in python. For the time measurement, we will
take the following snippet from the micropython manual:

.. code::
        
    # code to be run in micropython
    
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

Standard mathematical functions can be calculated on any scalar-valued
iterable (ranges, lists, tuples containing numbers), and on
``ndarray``\ s without having to change the call signature. In all cases
the functions return a new ``ndarray`` of typecode ``float`` (since
these functions usually generate float values, anyway). The functions
execute faster with ``ndarray`` arguments than with iterables, because
the values of the input vector can be extracted faster.

At present, the following functions are supported:

``acos``, ``acosh``, ``asin``, ``asinh``, ``atan``, ``atanh``, ``ceil``,
``cos``, ``erf``, ``erfc``, ``exp``, ``expm1``, ``floor``, ``tgamma``,
``lgamma``, ``log``, ``log10``, ``log2``, ``sin``, ``sinh``, ``sqrt``,
``tan``, ``tanh``.

These functions are applied element-wise to the arguments, thus, e.g.,
the exponential of a matrix cannot be calculated in this way.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = range(9)
    b = np.array(a)
    
    # works with ranges, lists, tuples etc.
    print('a:\t', a)
    print('exp(a):\t', np.exp(a))
    
    # with 1D arrays
    print('\nb:\t', b)
    print('exp(b):\t', np.exp(b))
    
    # as well as with matrices
    c = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
    print('\nc:\t', c)
    print('exp(c):\t', np.exp(c))

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

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = [0]*1000
    b = np.array(a)
    
    @timeit
    def measure_run_time(x):
        return np.exp(x)
    
    measure_run_time(a)
    
    measure_run_time(b)

.. parsed-literal::

    execution time:  1259  us
    execution time:  408  us
    


Of course, such a time saving is reasonable only, if the data are
already available as an ``ndarray``. If one has to initialise the
``ndarray`` from the list, then there is no gain, because the iterator
was simply pushed into the initialisation function.

Numerical
=========

linspace
--------

numpy:
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
    
    


min, argmin, max, argmax
------------------------

numpy:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.min.html

numpy:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.argmax.html

numpy:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.max.html

numpy:
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
    
    a = np.array([1, 2, 0, 1, 10])
    print('a:', a)
    print('min of a:', np.min(a))
    print('argmin of a:', np.argmin(a))
    
    b = np.array([[1, 2, 0], [1, 10, -1]])
    print('\nb:\n', b)
    print('min of b (flattened):', np.min(b))
    print('min of b (axis=0):', np.min(b, axis=0))
    print('min of b (axis=1):', np.min(b, axis=1))

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

numpy:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.sum.html

numpy:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.std.html

numpy:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.mean.html

These three functions follow the same pattern: if the axis keyword is
not specified, it assumes the default value of ``None``, and returns the
result of the computation for the flattened array. Otherwise, the
calculation is along the given axis.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
    print('a: \n', a)
    
    print('sum, flat array: ', np.sum(a))
    
    print('mean, horizontal: ', np.mean(a, axis=1))
    
    print('std, vertical: ', np.std(a, axis=0))

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

numpy:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.roll.html

The roll function shifts the content of a vector by the positions given
as the second argument. If the ``axis`` keyword is supplied, the shift
is applied to the given axis.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3, 4, 5, 6, 7, 8])
    print("a:\t\t\t", a)
    
    np.roll(a, 2)
    print("a rolled to the left:\t", a)
    
    # this should be the original vector
    np.roll(a, -2)
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
    
    a = np.array([[1, 2, 3, 4], [5, 6, 7, 8]])
    print("a:\n", a)
    
    np.roll(a, 2)
    print("\na rolled to the left:\n", a)
    
    np.roll(a, -1, axis=1)
    print("\na rolled up:\n", a)
    
    np.roll(a, 1, axis=None)
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
    
    def dummy_adc():
        # dummy adc function, so that the results are reproducible
        return 2
        
    n = 10
    # These are the normalised weights; the last entry is the most dominant
    weight = np.exp([1, 2, 3, 4, 5])
    weight = weight/np.sum(weight)
    
    print(weight)
    # initial array of samples
    samples = np.array([0]*n)
    
    for i in range(n):
        # a new datum is inserted on the right hand side. This simply overwrites whatever was in the last slot
        samples[-1] = dummy_adc()
        print(np.mean(samples[-5:]*weight))
        print(samples[-5:])
        # the data are shifted by one position to the left
        np.roll(samples, 1)

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

numpy:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.flip.html

The ``flip`` function takes one positional, an ``ndarray``, and one
keyword argument, ``axis = None``, and reverses the order of elements
along the given axis. If the keyword argument is ``None``, the matrix’
entries are flipped along all axes. ``flip`` returns a new copy of the
array.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([1, 2, 3, 4, 5])
    print("a: \t", a)
    print("a flipped:\t", np.flip(a))
    
    a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]], dtype=np.uint8)
    print("\na flipped horizontally\n", np.flip(a, axis=1))
    print("\na flipped vertically\n", np.flip(a, axis=0))
    print("\na flipped horizontally+vertically\n", np.flip(a))

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

numpy:
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
    
    a = np.array(range(9), dtype=np.uint8)
    print('a:\n', a)
    
    print('\nfirst derivative:\n', np.diff(a, n=1))
    print('\nsecond derivative:\n', np.diff(a, n=2))
    
    c = np.array([[1, 2, 3, 4], [4, 3, 2, 1], [1, 4, 9, 16], [0, 0, 0, 0]])
    print('\nc:\n', c)
    print('\nfirst derivative, first axis:\n', np.diff(c, axis=0))
    print('\nfirst derivative, second axis:\n', np.diff(c, axis=1))

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

numpy:
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
    
    a = np.array([[1, 12, 3, 0], [5, 3, 4, 1], [9, 11, 1, 8], [7, 10, 0, 1]], dtype=np.float)
    print('\na:\n', a)
    b = np.sort(a, axis=0)
    print('\na sorted along vertical axis:\n', b)
    
    c = np.sort(a, axis=1)
    print('\na sorted along horizontal axis:\n', c)
    
    c = np.sort(a, axis=None)
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
    
    @timeit
    def sort_time(array):
        return np.sort(array)
    
    b = np.sin(np.linspace(0, 6.28, num=1000))
    print('b: ', b)
    sort_time(b)
    print('\nb sorted:\n', b)
argsort
-------

numpy:
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
    
    a = np.array([[1, 12, 3, 0], [5, 3, 4, 1], [9, 11, 1, 8], [7, 10, 0, 1]], dtype=np.float)
    print('\na:\n', a)
    b = np.argsort(a, axis=0)
    print('\na sorted along vertical axis:\n', b)
    
    c = np.argsort(a, axis=1)
    print('\na sorted along horizontal axis:\n', c)
    
    c = np.argsort(a, axis=None)
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
    
    a = np.array([0, 5, 1, 3, 2, 4], dtype=np.uint8)
    print('\na:\n', a)
    b = np.argsort(a, axis=1)
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
    
    a = np.array([1, 2, 3, 4], dtype=np.int8)
    print("a:\n", a)
    print("size of a:", np.size(a, axis=None), ",", np.size(a, axis=0))
    
    b= np.array([[1, 2], [3, 4]], dtype=np.int8)
    print("\nb:\n", b)
    print("size of b:", np.size(b, axis=None), ",", np.size(b, axis=0), ",", np.size(b, axis=1))

.. parsed-literal::

    a:
     array([1, 2, 3, 4], dtype=int8)
    size of a: 4 , 4
    
    b:
     array([[1, 2],
    	 [3, 4]], dtype=int8)
    size of b: 4 , 2 , 2
    
    


ones, zeros
-----------

numpy:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.zeros.html

numpy:
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
---

numpy:
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
~~~~~~~~~~~~~~~~~~~~~~

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
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
    
    


Shifting the diagonal
~~~~~~~~~~~~~~~~~~~~~

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    print(np.eye(4, M=6, k=-1, dtype=np.int16))

.. parsed-literal::

    array([[0, 0, 0, 0],
    	 [1, 0, 0, 0],
    	 [0, 1, 0, 0],
    	 [0, 0, 1, 0],
    	 [0, 0, 0, 1],
    	 [0, 0, 0, 0]], dtype=int16)
    
    


inv
---

A square matrix, provided that it is not singular, can be inverted by
calling the ``inv`` function that takes a single argument. The inversion
is based on successive elimination of elements in the lower left
triangle, and raises a ``ValueError`` exception, if the matrix turns out
to be singular (i.e., one of the diagonal entries is zero).

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    m = np.array([[1, 2, 3, 4], [4, 5, 6, 4], [7, 8.6, 9, 4], [3, 4, 5, 6]])
    
    print(np.inv(m))

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
    
    @timeit
    def invert_matrix(m):
        return np.inv(m)
    
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

numpy:
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
    
    m = np.array([[1, 2, 3], [4, 5, 6], [7, 10, 9]], dtype=np.uint8)
    n = np.inv(m)
    print("m:\n", m)
    print("\nm^-1:\n", n)
    # this should be the unit matrix
    print("\nm*m^-1:\n", np.dot(m, n))

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
    
    m = np.array([[1, 2, 3, 4], [5, 6, 7, 8]], dtype=np.uint8)
    n = np.array([[1, 2], [3, 4], [5, 6], [7, 8]], dtype=np.uint8)
    print(m)
    print(n)
    print(np.dot(m, n))

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

numpy:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.linalg.det.html

The ``det`` function takes a square matrix as its single argument, and
calculates the determinant. The calculation is based on successive
elimination of the matrix elements, and the return value is a float,
even if the input array was of integer type.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    a = np.array([[1, 2], [3, 4]], dtype=np.uint8)
    print(np.det(a))

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
        return np.inv(m)
    
    m = np.array([[1, 2, 3, 4, 5, 6, 7, 8], [0, 5, 6, 4, 5, 6, 4, 5], 
                  [0, 0, 9, 7, 8, 9, 7, 8], [0, 0, 0, 10, 11, 12, 11, 12], 
                 [0, 0, 0, 0, 4, 6, 7, 8], [0, 0, 0, 0, 0, 5, 6, 7], 
                 [0, 0, 0, 0, 0, 0, 7, 6], [0, 0, 0, 0, 0, 0, 0, 2]])
    
    matrix_det(m)

.. parsed-literal::

    execution time:  294  us
    


eig
---

numpy:
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
    
    a = np.array([[1, 2, 1, 4], [2, 5, 3, 5], [1, 3, 6, 1], [4, 5, 1, 7]], dtype=np.uint8)
    x, y = np.eig(a)
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
    
    @timeit
    def matrix_eig(a):
        return np.eig(a)
    
    a = np.array([[1, 2, 1, 4], [2, 5, 3, 5], [1, 3, 6, 1], [4, 5, 1, 7]], dtype=np.uint8)
    
    matrix_eig(a)

.. parsed-literal::

    execution time:  111  us
    


Polynomials
===========

polyval
-------

numpy:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.polyval.html

polyval takes two arguments, both arrays or other iterables.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    p = [1, 1, 1, 0]
    x = [0, 1, 2, 3, 4]
    print('coefficients: ', p)
    print('independent values: ', x)
    print('\nvalues of p(x): ', np.polyval(p, x))
    
    # the same works with one-dimensional ndarrays
    a = np.array(x)
    print('\nndarray (a): ', a)
    print('value of p(a): ', np.polyval(p, a))

.. parsed-literal::

    coefficients:  [1, 1, 1, 0]
    independent values:  [0, 1, 2, 3, 4]
    
    values of p(x):  array([0.0, 3.0, 14.0, 39.0, 84.0], dtype=float)
    
    ndarray (a):  array([0.0, 1.0, 2.0, 3.0, 4.0], dtype=float)
    value of p(a):  array([0.0, 3.0, 14.0, 39.0, 84.0], dtype=float)
    
    


polyfit
-------

numpy:
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
    
    x = np.array([0, 1, 2, 3, 4, 5, 6])
    y = np.array([9, 4, 1, 0, 1, 4, 9])
    print('independent values:\t', x)
    print('dependent values:\t', y)
    print('fitted values:\t\t', np.polyfit(x, y, 2))
    
    # the same with missing x
    print('\ndependent values:\t', y)
    print('fitted values:\t\t', np.polyfit(y, 2))

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
    
    @timeit
    def time_polyfit(x, y, n):
        return np.polyfit(x, y, n)
    
    x = np.array([0, 1, 2, 3, 4, 5, 6])
    y = np.array([9, 4, 1, 0, 1, 4, 9])
    
    time_polyfit(x, y, 2)

.. parsed-literal::

    execution time:  153  us
    


Fourier transforms
==================

numpy:
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
    
    x = np.linspace(0, 10, num=1024)
    y = np.sin(x)
    z = np.zeros(len(x))
    
    a, b = np.fft(x)
    print('real part:\t', a)
    print('\nimaginary part:\t', b)
    
    c, d = np.fft(x, z)
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
    
    x = np.linspace(0, 10, num=1024)
    y = np.sin(x)
    
    a, b = np.fft(y)
    
    print('original vector:\t', y)
    
    y, z = np.ifft(a, b)
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

spectrum
--------

In addition to the Fourier transform and its inverse, ``ulab`` also
sports a function called ``spectrum``, which returns the absolute value
of the Fourier transform. This could be used to find the dominant
spectral component in a time series. The arguments are treated in the
same way as in ``fft``, and ``ifft``.

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    x = np.linspace(0, 10, num=1024)
    y = np.sin(x)
    
    a = np.spectrum(y)
    
    print('original vector:\t', y)
    print('\nspectrum:\t', a)

.. parsed-literal::

    original vector:	 array([0.0, 0.009775016, 0.0195491, ..., -0.5275068, -0.5357859, -0.5440139], dtype=float)
    
    spectrum:	 array([187.8641, 315.3125, 347.8804, ..., 84.4587, 347.8803, 315.3124], dtype=float)
    


As such, ``spectrum`` is really just a shorthand for
``np.sqrt(a*a + b*b)``:

.. code::
        
    # code to be run in micropython
    
    import ulab as np
    
    x = np.linspace(0, 10, num=1024)
    y = np.sin(x)
    
    a, b = np.fft(y)
    
    print('\nspectrum calculated the hard way:\t', np.sqrt(a*a + b*b))
    
    a = np.spectrum(y)
    
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
    
    x = np.linspace(0, 10, num=1024)
    y = np.sin(x)
    
    np.fft(y)
    
    @timeit
    def np_fft(y):
        return np.fft(y)
    
    a, b = np_fft(y)

.. parsed-literal::

    execution time:  1985  us
    


The C implementation runs in less than 2 ms on the pyboard (we have just
measured that), and has been reported to run in under 0.8 ms on the D
series board. That is an improvement of at least a factor of four.

Calculating FFTs of real signals
--------------------------------

Now, if you have real signals, and you are really pressed for time, you
can still gain a bit on speed without sacrificing anything at all.

If you take the FFT of a real-valued signal, the real part of the
transform will be symmetric, while the imaginary part will be
anti-symmetric in frequency.

If, on the other hand, the signal is imaginary-valued, then the real
part of the transform will be anti-symmetric, and the imaginary part
will be symmetric in frequency. These two statements follow from the
definition of the Fourier transform.

By combining the two observations above, if you place the first signal,
:math:`y_1(t)`, into the real part, and the second signal,
:math:`y_2(t)`, into the imaginary part of your input vector, i.e.,
:math:`y(t) = y_1(t) + iy_2(t)`, and take the Fourier transform of the
combined signal, then the Fourier transforms of the two components can
be recovered as

:raw-latex:`\begin{eqnarray}
Y_1(k) &=& \frac{1}{2}\left(Y(k) + Y^*(N-k)\right)
\\
Y_2(k) &=& -\frac{i}{2}\left(Y(k) - Y^*(N-k)\right)
\end{eqnarray}` where :math:`N` is the length of :math:`y_1`, and
:math:`Y_1, Y_2`, and :math:`Y`, respectively, are the Fourier
transforms of :math:`y_1, y_2`, and :math:`y = y_1 + iy_2`.

Extending ulab
==============

New functions can easily be added to ``ulab`` in a couple of simple
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
``ndarray.h``, this is already taken care of: the preprocessor figures
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

In the boilerplate above, we cast the pointer to ``array->items`` to the
required type. There are certain operations, however, when you do not
need the casting. If you do not want to change the array’s values, only
their position within the array, you can get away with copying the
memory content, regardless the type. A good example for such a scenario
is the transpose function in
https://github.com/v923z/micropython-ulab/blob/master/code/linalg.c.

Compiling your module
---------------------

Once you have implemented the functionality you wanted, you have to
include the source code in the make file by adding it to
``micropython.mk``:

.. code:: makefile

   USERMODULES_DIR := $(USERMOD_DIR)

   # Add all C files to SRC_USERMOD.
   SRC_USERMOD += $(USERMODULES_DIR)/ndarray.c
   SRC_USERMOD += $(USERMODULES_DIR)/linalg.c
   SRC_USERMOD += $(USERMODULES_DIR)/vectorise.c
   SRC_USERMOD += $(USERMODULES_DIR)/poly.c
   SRC_USERMOD += $(USERMODULES_DIR)/fft.c
   SRC_USERMOD += $(USERMODULES_DIR)/numerical.c
   SRC_USERMOD += $(USERMODULES_DIR)/ulab.c

   SRC_USERMOD += $(USERMODULES_DIR)/your_module.c

   CFLAGS_USERMOD += -I$(USERMODULES_DIR)

In addition, you also have to add the function objects to ``ulab.c``,
and create a ``QString`` for the function name:

.. code:: c

   ...
       MP_DEFINE_CONST_FUN_OBJ_1(useless_function_obj, userless_function);
   ...
       STATIC const mp_map_elem_t ulab_globals_table[] = {
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

At this point, you should be able to compile the module with your
extension by running ``make`` on the command line

.. code:: bash

   make USER_C_MODULES=../../../ulab all

for the unix port, and

.. code:: bash

   make BOARD=PYBV11 CROSS_COMPILE=<arm_tools_path>/bin/arm-none-eabi- USER_C_MODULES=../../../ulab all

for the ``pyboard``, provided that the you have defined

.. code:: makefile

   #define MODULE_ULAB_ENABLED (1)

somewhere in ``micropython/port/unix/mpconfigport.h``, or
``micropython/stm32/mpconfigport.h``, respectively.

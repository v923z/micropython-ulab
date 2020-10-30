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
``atan``, ``arctan2``, ``atanh``, ``ceil``, ``cos``, ``degrees``,
``erf``, ``erfc``, ``exp``, ``expm1``, ``floor``, ``tgamma``,
``lgamma``, ``log``, ``log10``, ``log2``, ``radians``, ``sin``,
``sinh``, ``sqrt``, ``tan``, ``tanh``.

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
sub-module. The function implements broadcasting as discussed in the
section on ``ndarray``\ s. Scalars (``micropython`` integers or floats)
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
    
    


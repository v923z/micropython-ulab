
Numpy functions
===============

This section of the manual discusses those functions that were adapted
from ``numpy``.

1.  `numpy.argmax <#argmax>`__
2.  `numpy.argmin <#argmin>`__
3.  `numpy.argsort <#argsort>`__
4.  `numpy.clip <#clip>`__
5.  `numpy.convolve <#convolve>`__
6.  `numpy.diff <#diff>`__
7.  `numpy.equal <#equal>`__
8.  `numpy.flip <#flip>`__
9.  `numpy.interp <#interp>`__
10. `numpy.max <#max>`__
11. `numpy.maximum <#maximum>`__
12. `numpy.mean <#mean>`__
13. `numpy.median <#median>`__
14. `numpy.min <#min>`__
15. `numpy.minimum <#minimum>`__
16. `numpy.not_equal <#equal>`__
17. `numpy.polyfit <#polyfit>`__
18. `numpy.polyval <#polyval>`__
19. `numpy.roll <#roll>`__
20. `numpy.sort <#sort>`__
21. `numpy.std <#std>`__
22. `numpy.sum <#sum>`__
23. `numpy.trapz <#trapz>`__

argmax
------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.argmax.html

See `numpy.max <#max>`__.

argmin
------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.argmin.html

See `numpy.max <#max>`__.

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
65535. If that happens to be the case, the function will bail out with a
``ValueError``.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
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
           [7.0, 10.0, 0.0, 1.0]], dtype=float64)
    
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
    
    Traceback (most recent call last):
      File "/dev/shm/micropython.py", line 12, in <module>
    NotImplementedError: argsort is not implemented for flattened arrays
    


Since during the sorting, only the indices are shuffled, ``argsort``
does not modify the input array, as one can verify this by the following
example:

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    a = np.array([0, 5, 1, 3, 2, 4], dtype=np.uint8)
    print('\na:\n', a)
    b = np.argsort(a, axis=0)
    print('\nsorting indices:\n', b)
    print('\nthe original array:\n', a)

.. parsed-literal::

    
    a:
     array([0, 5, 1, 3, 2, 4], dtype=uint8)
    
    sorting indices:
     array([0, 2, 4, 3, 5, 1], dtype=uint16)
    
    the original array:
     array([0, 5, 1, 3, 2, 4], dtype=uint8)
    
    


clip
----

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.clip.html

Clips an array, i.e., values that are outside of an interval are clipped
to the interval edges. The function is equivalent to
``maximum(a_min, minimum(a, a_max))`` broadcasting takes place exactly
as in `minimum <#minimum>`__. If the arrays are of different ``dtype``,
the output is upcast as in `Binary operators <#Binary-operators>`__.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    a = np.array(range(9), dtype=np.uint8)
    print('a:\t\t', a)
    print('clipped:\t', np.clip(a, 3, 7))
    
    b = 3 * np.ones(len(a), dtype=np.float)
    print('\na:\t\t', a)
    print('b:\t\t', b)
    print('clipped:\t', np.clip(a, b, 7))

.. parsed-literal::

    a:		 array([0, 1, 2, 3, 4, 5, 6, 7, 8], dtype=uint8)
    clipped:	 array([3, 3, 3, 3, 4, 5, 6, 7, 7], dtype=uint8)
    
    a:		 array([0, 1, 2, 3, 4, 5, 6, 7, 8], dtype=uint8)
    b:		 array([3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0], dtype=float64)
    clipped:	 array([3.0, 3.0, 3.0, 3.0, 4.0, 5.0, 6.0, 7.0, 7.0], dtype=float64)
    
    


convolve
--------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.convolve.html

Returns the discrete, linear convolution of two one-dimensional arrays.

Only the ``full`` mode is supported, and the ``mode`` named parameter is
not accepted. Note that all other modes can be had by slicing a ``full``
result.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    x = np.array((1, 2, 3))
    y = np.array((1, 10, 100, 1000))
    
    print(np.convolve(x, y))

.. parsed-literal::

    array([1.0, 12.0, 123.0, 1230.0, 2300.0, 3000.0], dtype=float64)
    
    


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
    
    from ulab import numpy as np
    
    a = np.array(range(9), dtype=np.uint8)
    a[3] = 10
    print('a:\n', a)
    
    print('\nfirst derivative:\n', np.diff(a, n=1))
    print('\nsecond derivative:\n', np.diff(a, n=2))
    
    c = np.array([[1, 2, 3, 4], [4, 3, 2, 1], [1, 4, 9, 16], [0, 0, 0, 0]])
    print('\nc:\n', c)
    print('\nfirst derivative, first axis:\n', np.diff(c, axis=0))
    print('\nfirst derivative, second axis:\n', np.diff(c, axis=1))

.. parsed-literal::

    a:
     array([0, 1, 2, 10, 4, 5, 6, 7, 8], dtype=uint8)
    
    first derivative:
     array([1, 1, 8, 250, 1, 1, 1, 1], dtype=uint8)
    
    second derivative:
     array([0, 249, 14, 249, 0, 0, 0], dtype=uint8)
    
    c:
     array([[1.0, 2.0, 3.0, 4.0],
           [4.0, 3.0, 2.0, 1.0],
           [1.0, 4.0, 9.0, 16.0],
           [0.0, 0.0, 0.0, 0.0]], dtype=float64)
    
    first derivative, first axis:
     array([[3.0, 1.0, -1.0, -3.0],
           [-3.0, 1.0, 7.0, 15.0],
           [-1.0, -4.0, -9.0, -16.0]], dtype=float64)
    
    first derivative, second axis:
     array([[1.0, 1.0, 1.0],
           [-1.0, -1.0, -1.0],
           [3.0, 5.0, 7.0],
           [0.0, 0.0, 0.0]], dtype=float64)
    
    


equal
-----

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
    
    from ulab import numpy as np
    
    a = np.array(range(9))
    b = np.zeros(9)
    
    print('a: ', a)
    print('b: ', b)
    print('\na == b: ', np.equal(a, b))
    print('a != b: ', np.not_equal(a, b))
    
    # comparison with scalars
    print('a == 2: ', np.equal(a, 2))

.. parsed-literal::

    a:  array([0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0], dtype=float64)
    b:  array([0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0], dtype=float64)
    
    a == b:  array([True, False, False, False, False, False, False, False, False], dtype=bool)
    a != b:  array([False, True, True, True, True, True, True, True, True], dtype=bool)
    a == 2:  array([False, False, True, False, False, False, False, False, False], dtype=bool)
    
    


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
    
    from ulab import numpy as np
    
    a = np.array([1, 2, 3, 4, 5])
    print("a: \t", a)
    print("a flipped:\t", np.flip(a))
    
    a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]], dtype=np.uint8)
    print("\na flipped horizontally\n", np.flip(a, axis=1))
    print("\na flipped vertically\n", np.flip(a, axis=0))
    print("\na flipped horizontally+vertically\n", np.flip(a))

.. parsed-literal::

    a: 	 array([1.0, 2.0, 3.0, 4.0, 5.0], dtype=float64)
    a flipped:	 array([5.0, 4.0, 3.0, 2.0, 1.0], dtype=float64)
    
    a flipped horizontally
     array([[3, 2, 1],
           [6, 5, 4],
           [9, 8, 7]], dtype=uint8)
    
    a flipped vertically
     array([[7, 8, 9],
           [4, 5, 6],
           [1, 2, 3]], dtype=uint8)
    
    a flipped horizontally+vertically
     array([9, 8, 7, 6, 5, 4, 3, 2, 1], dtype=uint8)
    
    


interp
------

``numpy``: https://docs.scipy.org/doc/numpy/numpy.interp

The ``interp`` function returns the linearly interpolated values of a
one-dimensional numerical array. It requires three positional
arguments,\ ``x``, at which the interpolated values are evaluated,
``xp``, the array of the independent data variable, and ``fp``, the
array of the dependent values of the data. ``xp`` must be a
monotonically increasing sequence of numbers.

Two keyword arguments, ``left``, and ``right`` can also be supplied;
these determine the return values, if ``x < xp[0]``, and ``x > xp[-1]``,
respectively. If these arguments are not supplied, ``left``, and
``right`` default to ``fp[0]``, and ``fp[-1]``, respectively.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    x = np.array([1, 2, 3, 4, 5]) - 0.2
    xp = np.array([1, 2, 3, 4])
    fp = np.array([1, 2, 3, 5])
    
    print(x)
    print(np.interp(x, xp, fp))
    print(np.interp(x, xp, fp, left=0.0))
    print(np.interp(x, xp, fp, right=10.0))

.. parsed-literal::

    array([0.8, 1.8, 2.8, 3.8, 4.8], dtype=float64)
    array([1.0, 1.8, 2.8, 4.6, 5.0], dtype=float64)
    array([0.0, 1.8, 2.8, 4.6, 5.0], dtype=float64)
    array([1.0, 1.8, 2.8, 4.6, 10.0], dtype=float64)
    
    


mean
----

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.mean.html

If the axis keyword is not specified, it assumes the default value of
``None``, and returns the result of the computation for the flattened
array. Otherwise, the calculation is along the given axis.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
    print('a: \n', a)
    print('mean, flat: ', np.mean(a))
    print('mean, horizontal: ', np.mean(a, axis=1))
    print('mean, vertical: ', np.mean(a, axis=0))

.. parsed-literal::

    a: 
     array([[1.0, 2.0, 3.0],
           [4.0, 5.0, 6.0],
           [7.0, 8.0, 9.0]], dtype=float64)
    mean, flat:  5.0
    mean, horizontal:  array([2.0, 5.0, 8.0], dtype=float64)
    mean, vertical:  array([4.0, 5.0, 6.0], dtype=float64)
    
    


max
---

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.max.html

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.argmax.html

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.min.html

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.argmin.html

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
    
    from ulab import numpy as np
    
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

    a: array([1.0, 2.0, 0.0, 1.0, 10.0], dtype=float64)
    min of a: 0.0
    argmin of a: 2
    
    b:
     array([[1.0, 2.0, 0.0],
           [1.0, 10.0, -1.0]], dtype=float64)
    min of b (flattened): -1.0
    min of b (axis=0): array([1.0, 2.0, -1.0], dtype=float64)
    min of b (axis=1): array([0.0, -1.0], dtype=float64)
    
    


median
------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.median.html

The function computes the median along the specified axis, and returns
the median of the array elements. If the ``axis`` keyword argument is
``None``, the arrays is flattened first. The ``dtype`` of the results is
always float.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    a = np.array(range(12), dtype=np.int8).reshape((3, 4))
    print('a:\n', a)
    print('\nmedian of the flattened array: ', np.median(a))
    print('\nmedian along the vertical axis: ', np.median(a, axis=0))
    print('\nmedian along the horizontal axis: ', np.median(a, axis=1))

.. parsed-literal::

    a:
     array([[0, 1, 2, 3],
           [4, 5, 6, 7],
           [8, 9, 10, 11]], dtype=int8)
    
    median of the flattened array:  5.5
    
    median along the vertical axis:  array([4.0, 5.0, 6.0, 7.0], dtype=float64)
    
    median along the horizontal axis:  array([1.5, 5.5, 9.5], dtype=float64)
    
    


min
---

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.min.html

See `numpy.max <#max>`__.

minimum
-------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.minimum.html

See `numpy.maximum <#maximum>`__

maximum
-------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.maximum.html

Returns the maximum of two arrays, or two scalars, or an array, and a
scalar. If the arrays are of different ``dtype``, the output is upcast
as in `Binary operators <#Binary-operators>`__. If both inputs are
scalars, a scalar is returned. Only positional arguments are
implemented.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    a = np.array([1, 2, 3, 4, 5], dtype=np.uint8)
    b = np.array([5, 4, 3, 2, 1], dtype=np.float)
    print('minimum of a, and b:')
    print(np.minimum(a, b))
    
    print('\nmaximum of a, and b:')
    print(np.maximum(a, b))
    
    print('\nmaximum of 1, and 5.5:')
    print(np.maximum(1, 5.5))

.. parsed-literal::

    minimum of a, and b:
    array([1.0, 2.0, 3.0, 2.0, 1.0], dtype=float64)
    
    maximum of a, and b:
    array([5.0, 4.0, 3.0, 4.0, 5.0], dtype=float64)
    
    maximum of 1, and 5.5:
    5.5
    
    


not_equal
---------

See `numpy.equal <#equal>`__.

polyfit
-------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.polyfit.html

polyfit takes two, or three arguments. The last one is the degree of the
polynomial that will be fitted, the last but one is an array or iterable
with the ``y`` (dependent) values, and the first one, an array or
iterable with the ``x`` (independent) values, can be dropped. If that is
the case, ``x`` will be generated in the function as ``range(len(y))``.

If the lengths of ``x``, and ``y`` are not the same, the function raises
a ``ValueError``.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    x = np.array([0, 1, 2, 3, 4, 5, 6])
    y = np.array([9, 4, 1, 0, 1, 4, 9])
    print('independent values:\t', x)
    print('dependent values:\t', y)
    print('fitted values:\t\t', np.polyfit(x, y, 2))
    
    # the same with missing x
    print('\ndependent values:\t', y)
    print('fitted values:\t\t', np.polyfit(y, 2))

.. parsed-literal::

    independent values:	 array([0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0], dtype=float64)
    dependent values:	 array([9.0, 4.0, 1.0, 0.0, 1.0, 4.0, 9.0], dtype=float64)
    fitted values:		 array([1.0, -6.0, 9.000000000000004], dtype=float64)
    
    dependent values:	 array([9.0, 4.0, 1.0, 0.0, 1.0, 4.0, 9.0], dtype=float64)
    fitted values:		 array([1.0, -6.0, 9.000000000000004], dtype=float64)
    
    


Execution time
~~~~~~~~~~~~~~

``polyfit`` is based on the inversion of a matrix (there is more on the
background in https://en.wikipedia.org/wiki/Polynomial_regression), and
it requires the intermediate storage of ``2*N*(deg+1)`` floats, where
``N`` is the number of entries in the input array, and ``deg`` is the
fit’s degree. The additional computation costs of the matrix inversion
discussed in `linalg.inv <#inv>`__ also apply. The example from above
needs around 150 microseconds to return:

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    @timeit
    def time_polyfit(x, y, n):
        return np.polyfit(x, y, n)
    
    x = np.array([0, 1, 2, 3, 4, 5, 6])
    y = np.array([9, 4, 1, 0, 1, 4, 9])
    
    time_polyfit(x, y, 2)

.. parsed-literal::

    execution time:  153  us


polyval
-------

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.polyval.html

``polyval`` takes two arguments, both arrays or generic ``micropython``
iterables returning scalars.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
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
    
    values of p(x):  array([0.0, 3.0, 14.0, 39.0, 84.0], dtype=float64)
    
    ndarray (a):  array([0.0, 1.0, 2.0, 3.0, 4.0], dtype=float64)
    value of p(a):  array([0.0, 3.0, 14.0, 39.0, 84.0], dtype=float64)
    
    


roll
----

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.roll.html

The roll function shifts the content of a vector by the positions given
as the second argument. If the ``axis`` keyword is supplied, the shift
is applied to the given axis.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    a = np.array([1, 2, 3, 4, 5, 6, 7, 8])
    print("a:\t\t\t", a)
    
    a = np.roll(a, 2)
    print("a rolled to the left:\t", a)
    
    # this should be the original vector
    a = np.roll(a, -2)
    print("a rolled to the right:\t", a)

.. parsed-literal::

    a:			 array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0], dtype=float64)
    a rolled to the left:	 array([7.0, 8.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0], dtype=float64)
    a rolled to the right:	 array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0], dtype=float64)
    
    


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
    
    from ulab import numpy as np
    
    a = np.array(range(12)).reshape((3, 4))
    print("a:\n", a)
    a = np.roll(a, 2, axis=0)
    print("\na rolled up:\n", a)
    
    a = np.array(range(12)).reshape((3, 4))
    print("a:\n", a)
    a = np.roll(a, -1, axis=1)
    print("\na rolled to the left:\n", a)
    
    a = np.array(range(12)).reshape((3, 4))
    print("a:\n", a)
    a = np.roll(a, 1, axis=None)
    print("\na rolled with None:\n", a)

.. parsed-literal::

    a:
     array([[0.0, 1.0, 2.0, 3.0],
           [4.0, 5.0, 6.0, 7.0],
           [8.0, 9.0, 10.0, 11.0]], dtype=float64)
    
    a rolled up:
     array([[4.0, 5.0, 6.0, 7.0],
           [8.0, 9.0, 10.0, 11.0],
           [0.0, 1.0, 2.0, 3.0]], dtype=float64)
    a:
     array([[0.0, 1.0, 2.0, 3.0],
           [4.0, 5.0, 6.0, 7.0],
           [8.0, 9.0, 10.0, 11.0]], dtype=float64)
    
    a rolled to the left:
     array([[1.0, 2.0, 3.0, 0.0],
           [5.0, 6.0, 7.0, 4.0],
           [9.0, 10.0, 11.0, 8.0]], dtype=float64)
    a:
     array([[0.0, 1.0, 2.0, 3.0],
           [4.0, 5.0, 6.0, 7.0],
           [8.0, 9.0, 10.0, 11.0]], dtype=float64)
    
    a rolled with None:
     array([[11.0, 0.0, 1.0, 2.0],
           [3.0, 4.0, 5.0, 6.0],
           [7.0, 8.0, 9.0, 10.0]], dtype=float64)
    
    


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
    
    from ulab import numpy as np
    
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
           [7.0, 10.0, 0.0, 1.0]], dtype=float64)
    
    a sorted along vertical axis:
     array([[1.0, 3.0, 0.0, 0.0],
           [5.0, 10.0, 1.0, 1.0],
           [7.0, 11.0, 3.0, 1.0],
           [9.0, 12.0, 4.0, 8.0]], dtype=float64)
    
    a sorted along horizontal axis:
     array([[0.0, 1.0, 3.0, 12.0],
           [1.0, 3.0, 4.0, 5.0],
           [1.0, 8.0, 9.0, 11.0],
           [0.0, 1.0, 7.0, 10.0]], dtype=float64)
    
    flattened a sorted:
     array([0.0, 0.0, 1.0, ..., 10.0, 11.0, 12.0], dtype=float64)
    
    


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
std
---

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.std.html

If the axis keyword is not specified, it assumes the default value of
``None``, and returns the result of the computation for the flattened
array. Otherwise, the calculation is along the given axis.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
    print('a: \n', a)
    print('sum, flat array: ', np.std(a))
    print('std, vertical: ', np.std(a, axis=0))
    print('std, horizonal: ', np.std(a, axis=1))

.. parsed-literal::

    a: 
     array([[1.0, 2.0, 3.0],
           [4.0, 5.0, 6.0],
           [7.0, 8.0, 9.0]], dtype=float64)
    sum, flat array:  2.581988897471611
    std, vertical:  array([2.449489742783178, 2.449489742783178, 2.449489742783178], dtype=float64)
    std, horizonal:  array([0.8164965809277261, 0.8164965809277261, 0.8164965809277261], dtype=float64)
    
    


sum
---

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.sum.html

If the axis keyword is not specified, it assumes the default value of
``None``, and returns the result of the computation for the flattened
array. Otherwise, the calculation is along the given axis.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
    print('a: \n', a)
    
    print('sum, flat array: ', np.sum(a))
    print('sum, horizontal: ', np.sum(a, axis=1))
    print('std, vertical: ', np.sum(a, axis=0))

.. parsed-literal::

    a: 
     array([[1.0, 2.0, 3.0],
           [4.0, 5.0, 6.0],
           [7.0, 8.0, 9.0]], dtype=float64)
    sum, flat array:  45.0
    sum, horizontal:  array([6.0, 15.0, 24.0], dtype=float64)
    std, vertical:  array([12.0, 15.0, 18.0], dtype=float64)
    
    


trapz
-----

``numpy``:
https://numpy.org/doc/stable/reference/generated/numpy.trapz.html

The function takes one or two one-dimensional ``ndarray``\ s, and
integrates the dependent values (``y``) using the trapezoidal rule. If
the independent variable (``x``) is given, that is taken as the sample
points corresponding to ``y``.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    x = np.linspace(0, 9, num=10)
    y = x*x
    
    print('x: ',  x)
    print('y: ',  y)
    print('============================')
    print('integral of y: ', np.trapz(y))
    print('integral of y at x: ', np.trapz(y, x=x))

.. parsed-literal::

    x:  array([0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0], dtype=float64)
    y:  array([0.0, 1.0, 4.0, 9.0, 16.0, 25.0, 36.0, 49.0, 64.0, 81.0], dtype=float64)
    ============================
    integral of y:  244.5
    integral of y at x:  244.5
    
    


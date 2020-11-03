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
    
    a = np.array([1, 2, 3])
    print(a)
    print(a[-1:-1:-3])
    try:
        sa = list(a[-1:-1:-3])
        la = len(sa)
    except IndexError as e:
        sa = str(e)
        la = -1
        
    print(sa, la)
    
    a[-1:-1:-3] = np.ones(0)
    print(a)
    
    b = np.ones(0) + 1
    print(b)
    # print('b', b.shape())

.. parsed-literal::

    array([1.0, 2.0, 3.0], dtype=float)
    array([], dtype=float)
    [] 0
    array([1.0, 2.0, 3.0], dtype=float)
    array([], dtype=float)
    
    


.. code::
        
    # code to be run in micropython
    
    import ulab as np
    a = np.array([1, 2, 3])
    print(a[0:1:-3])

.. parsed-literal::

    0, 1, -3array([], dtype=float)
    
    


.. code::

    # code to be run in CPython
    
    l = list(range(13))
    
    l[0:10:113]



.. parsed-literal::

    [0]



.. code::

    # code to be run in CPython
    
    a = np.array([1, 2, 3])
    np.ones(0, dtype=uint8) / np.zeros(0, dtype=uint16)
    np.ones(0).shape



.. parsed-literal::

    (0,)



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
    
    import ulab as np
    
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
    
    median along the vertical axis:  array([4.0, 5.0, 6.0, 7.0], dtype=float)
    
    median along the horizontal axis:  array([1.5, 5.5, 9.5], dtype=float)
    
    


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
65535. If that happens to be the case, the function will bail out with a
``ValueError``.

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
    
    


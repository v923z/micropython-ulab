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
    
    


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
    


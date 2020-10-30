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
    
    



scipy.signal
============

This module defines the functions:

1. `scipy.signal.sosfilt <#sosfilt>`__
2. `scipy.signal.oaconvolve <#oaconvolve>`__

sosfilt
-------

``scipy``:
https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.sosfilt.html

Filter data along one dimension using cascaded second-order sections.

The function takes two positional arguments, ``sos``, the filter
segments of length 6, and the one-dimensional, uniformly sampled data
set to be filtered. Returns the filtered data, or the filtered data and
the final filter delays, if the ``zi`` keyword arguments is supplied.
The keyword argument must be a float ``ndarray`` of shape
``(n_sections, 2)``. If ``zi`` is not passed to the function, the
initial values are assumed to be 0.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    from ulab import scipy as spy
    
    x = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
    sos = [[1, 2, 3, 1, 5, 6], [1, 2, 3, 1, 5, 6]]
    y = spy.signal.sosfilt(sos, x)
    print('y: ', y)

.. parsed-literal::

    y:  array([0.0, 1.0, -4.0, 24.0, -104.0, 440.0, -1728.0, 6532.000000000001, -23848.0, 84864.0], dtype=float)
    
    


.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    from ulab import scipy as spy
    
    x = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
    sos = [[1, 2, 3, 1, 5, 6], [1, 2, 3, 1, 5, 6]]
    # initial conditions of the filter
    zi = np.array([[1, 2], [3, 4]])
    
    y, zf = spy.signal.sosfilt(sos, x, zi=zi)
    print('y: ', y)
    print('\n' + '='*40 + '\nzf: ', zf)

.. parsed-literal::

    y:  array([4.0, -16.0, 63.00000000000001, -227.0, 802.9999999999999, -2751.0, 9271.000000000001, -30775.0, 101067.0, -328991.0000000001], dtype=float)
    
    ========================================
    zf:  array([[37242.0, 74835.0],
    	 [1026187.0, 1936542.0]], dtype=float)

oaconvolve
-------

``scipy``:
https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.oaconvolve.html

Convolve two N-dimensional arrays using the overlap-add method.

Convolve in1 and in2 using the overlap-add method. Similarly to numpy.convolve, 
this method works in full mode and other modes can be obtained by slicing the result?

This is generally much faster than linear convolve for large arrays (n > ~500), 
and generally much faster than fftconvolve (not implemented yet in ulab) when one array is much larger 
than the other (e.g. for a matched filter algorithm), but can be slower when only a few output values 
are needed or when the arrays are very similar in shape, and can only output float arrays (int or object array inputs will be cast to float).

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    from ulab import scipy as spy
    
    x = np.array((1,2,3))
    y = np.array((1,10,100,1000))
    result = spy.signal.oaconvolve(x, y)
    print('result: ', result)

.. parsed-literal::

    result:  array([1.0, 12.00024, 123.0001, 1230.0, 2300.0, 3000.0], dtype=float32)
    
    
    


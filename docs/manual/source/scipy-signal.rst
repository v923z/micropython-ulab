
Signal
======

Functions in the ``signal`` module can be called by prepending them by
``scipy.signal.``. The module defines the following two functions:

1. `scipy.signal.sosfilt <#sosfilt>`__
2. `scipy.signal.spectrogram <#spectrogram>`__

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
    
    


spectrogram
-----------

In addition to the Fourier transform and its inverse, ``ulab`` also
sports a function called ``spectrogram``, which returns the absolute
value of the Fourier transform. This could be used to find the dominant
spectral component in a time series. The arguments are treated in the
same way as in ``fft``, and ``ifft``.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    from ulab import scipy as spy
    
    x = np.linspace(0, 10, num=1024)
    y = np.sin(x)
    
    a = spy.signal.spectrogram(y)
    
    print('original vector:\t', y)
    print('\nspectrum:\t', a)

.. parsed-literal::

    original vector:	 array([0.0, 0.009775015390171337, 0.01954909674625918, ..., -0.5275140569487312, -0.5357931822978732, -0.5440211108893639], dtype=float64)
    
    spectrum:	 array([187.8635087634579, 315.3112063607119, 347.8814873399374, ..., 84.45888934298905, 347.8814873399374, 315.3112063607118], dtype=float64)
    
    


As such, ``spectrogram`` is really just a shorthand for
``np.sqrt(a*a + b*b)``:

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    from ulab import scipy as spy
    
    x = np.linspace(0, 10, num=1024)
    y = np.sin(x)
    
    a, b = np.fft.fft(y)
    
    print('\nspectrum calculated the hard way:\t', np.sqrt(a*a + b*b))
    
    a = spy.signal.spectrogram(y)
    
    print('\nspectrum calculated the lazy way:\t', a)

.. parsed-literal::

    
    spectrum calculated the hard way:	 array([187.8635087634579, 315.3112063607119, 347.8814873399374, ..., 84.45888934298905, 347.8814873399374, 315.3112063607118], dtype=float64)
    
    spectrum calculated the lazy way:	 array([187.8635087634579, 315.3112063607119, 347.8814873399374, ..., 84.45888934298905, 347.8814873399374, 315.3112063607118], dtype=float64)
    
    


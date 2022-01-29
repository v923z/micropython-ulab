
ulab utilities
==============

There might be cases, when the format of your data does not conform to
``ulab``, i.e., there is no obvious way to map the data to any of the
five supported ``dtype``\ s. A trivial example is an ADC or microphone
signal with 32-bit resolution. For such cases, ``ulab`` defines the
``utils`` module, which, at the moment, has four functions that are not
``numpy`` compatible, but which should ease interfacing ``ndarray``\ s
to peripheral devices.

The ``utils`` module can be enabled by setting the
``ULAB_HAS_UTILS_MODULE`` constant to 1 in
`ulab.h <https://github.com/v923z/micropython-ulab/blob/master/code/ulab.h>`__:

.. code:: c

   #ifndef ULAB_HAS_UTILS_MODULE
   #define ULAB_HAS_UTILS_MODULE               (1)
   #endif

This still does not compile any functions into the firmware. You can add
a function by setting the corresponding pre-processor constant to 1.
E.g.,

.. code:: c

   #ifndef ULAB_UTILS_HAS_FROM_INT16_BUFFER
   #define ULAB_UTILS_HAS_FROM_INT16_BUFFER    (1)
   #endif

from_int32_buffer, from_uint32_buffer
-------------------------------------

With the help of ``utils.from_int32_buffer``, and
``utils.from_uint32_buffer``, it is possible to convert 32-bit integer
buffers to ``ndarrays`` of float type. These functions have a syntax
similar to ``numpy.frombuffer``; they support the ``count=-1``, and
``offset=0`` keyword arguments. However, in addition, they also accept
``out=None``, and ``byteswap=False``.

Here is an example without keyword arguments

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    from ulab import utils
    
    a = bytearray([1, 1, 0, 0, 0, 0, 0, 255])
    print('a: ', a)
    print()
    print('unsigned integers: ', utils.from_uint32_buffer(a))
    
    b = bytearray([1, 1, 0, 0, 0, 0, 0, 255])
    print('\nb:  ', b)
    print()
    print('signed integers: ', utils.from_int32_buffer(b))

.. parsed-literal::

    a:  bytearray(b'\x01\x01\x00\x00\x00\x00\x00\xff')
    
    unsigned integers:  array([257.0, 4278190080.000001], dtype=float64)
    
    b:   bytearray(b'\x01\x01\x00\x00\x00\x00\x00\xff')
    
    signed integers:  array([257.0, -16777216.0], dtype=float64)
    
    


The meaning of ``count``, and ``offset`` is similar to that in
``numpy.frombuffer``. ``count`` is the number of floats that will be
converted, while ``offset`` would discard the first ``offset`` number of
bytes from the buffer before the conversion.

In the example above, repeated calls to either of the functions returns
a new ``ndarray``. You can save RAM by supplying the ``out`` keyword
argument with a pre-defined ``ndarray`` of sufficient size, in which
case the results will be inserted into the ``ndarray``. If the ``dtype``
of ``out`` is not ``float``, a ``TypeError`` exception will be raised.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    from ulab import utils
    
    a = np.array([1, 2], dtype=np.float)
    b = bytearray([1, 0, 1, 0, 0, 1, 0, 1])
    print('b: ', b)
    utils.from_uint32_buffer(b, out=a)
    print('a: ', a)

.. parsed-literal::

    b:  bytearray(b'\x01\x00\x01\x00\x00\x01\x00\x01')
    a:  array([65537.0, 16777472.0], dtype=float64)
    
    


Finally, since there is no guarantee that the endianness of a particular
peripheral device supplying the buffer is the same as that of the
microcontroller, ``from_(u)intbuffer`` allows a conversion via the
``byteswap`` keyword argument.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    from ulab import utils
    
    a = bytearray([1, 0, 0, 0, 0, 0, 0, 1])
    print('a: ', a)
    print('buffer without byteswapping: ', utils.from_uint32_buffer(a))
    print('buffer with byteswapping: ', utils.from_uint32_buffer(a, byteswap=True))

.. parsed-literal::

    a:  bytearray(b'\x01\x00\x00\x00\x00\x00\x00\x01')
    buffer without byteswapping:  array([1.0, 16777216.0], dtype=float64)
    buffer with byteswapping:  array([16777216.0, 1.0], dtype=float64)
    
    


from_int16_buffer, from_uint16_buffer
-------------------------------------

These two functions are identical to ``utils.from_int32_buffer``, and
``utils.from_uint32_buffer``, with the exception that they convert
16-bit integers to floating point ``ndarray``\ s.

spectrogram
-----------

In addition to the Fourier transform and its inverse, ``ulab`` also
sports a function called ``spectrogram``, which returns the absolute
value of the Fourier transform, also known as the power spectrum. This
could be used to find the dominant spectral component in a time series.
The arguments are treated in the same way as in ``fft``, and ``ifft``.
This means that, if the firmware was compiled with complex support, the
input can also be a complex array.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    from ulab import utils as utils
    
    x = np.linspace(0, 10, num=1024)
    y = np.sin(x)
    
    a = utils.spectrogram(y)
    
    print('original vector:\n', y)
    print('\nspectrum:\n', a)

.. parsed-literal::

    original vector:
     array([0.0, 0.009775015390171337, 0.01954909674625918, ..., -0.5275140569487312, -0.5357931822978732, -0.5440211108893697], dtype=float64)
    
    spectrum:
     array([187.8635087634579, 315.3112063607119, 347.8814873399374, ..., 84.45888934298905, 347.8814873399374, 315.3112063607118], dtype=float64)
    
    


As such, ``spectrogram`` is really just a shorthand for
``np.sqrt(a*a + b*b)``, however, it saves significant amounts of RAM:
the expression ``a*a + b*b`` has to allocate memory for ``a*a``,
``b*b``, and finally, their sum. In contrast, ``spectrogram`` calculates
the spectrum internally, and stores it in the memory segment that was
reserved for the real part of the Fourier transform.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    from ulab import utils as utils
    
    x = np.linspace(0, 10, num=1024)
    y = np.sin(x)
    
    a, b = np.fft.fft(y)
    
    print('\nspectrum calculated the hard way:\n', np.sqrt(a*a + b*b))
    
    a = utils.spectrogram(y)
    
    print('\nspectrum calculated the lazy way:\n', a)

.. parsed-literal::

    
    spectrum calculated the hard way:
     array([187.8635087634579, 315.3112063607119, 347.8814873399374, ..., 84.45888934298905, 347.8814873399374, 315.3112063607118], dtype=float64)
    
    spectrum calculated the lazy way:
     array([187.8635087634579, 315.3112063607119, 347.8814873399374, ..., 84.45888934298905, 347.8814873399374, 315.3112063607118], dtype=float64)
    
    


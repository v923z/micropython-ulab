:mod:`ulab.fft`
===============

.. py:module:: ulab.fft

.. autoapi-nested-parse::

   Frequency-domain functions



.. function:: fft(r: ulab.array, c: Optional[ulab.array] = None) -> Tuple[ulab.array, ulab.array]

   :param ulab.array r: A 1-dimension array of values whose size is a power of 2
   :param ulab.array c: An optional 1-dimension array of values whose size is a power of 2, giving the complex part of the value
   :return tuple (r, c): The real and complex parts of the FFT

   Perform a Fast Fourier Transform from the time domain into the frequency domain

   See also ~ulab.extras.spectrum, which computes the magnitude of the fft,
   rather than separately returning its real and imaginary parts.


.. function:: ifft(r: ulab.array, c: Optional[ulab.array] = None) -> Tuple[ulab.array, ulab.array]

   :param ulab.array r: A 1-dimension array of values whose size is a power of 2
   :param ulab.array c: An optional 1-dimension array of values whose size is a power of 2, giving the complex part of the value
   :return tuple (r, c): The real and complex parts of the inverse FFT

   Perform an Inverse Fast Fourier Transform from the frequeny domain into the time domain


.. function:: spectrogram(r: ulab.array) -> ulab.array

   :param ulab.array r: A 1-dimension array of values whose size is a power of 2

   Computes the spectrum of the input signal.  This is the absolute value of the (complex-valued) fft of the signal.
   This function is similar to scipy's ``scipy.signal.spectrogram``.



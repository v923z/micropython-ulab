:mod:`ulab.vector`
==================

.. py:module:: ulab.vector

.. autoapi-nested-parse::

   Element-by-element functions

   These functions can operate on numbers, 1-D iterables, 1-D arrays, or 2-D arrays by
   applying the function to every element in the array.  This is typically
   much more efficient than expressing the same operation as a Python loop.



.. function:: acos(a: _ArrayLike) -> ulab.array

   Computes the inverse cosine function


.. function:: acosh(a: _ArrayLike) -> ulab.array

   Computes the inverse hyperbolic cosine function


.. function:: asin(a: _ArrayLike) -> ulab.array

   Computes the inverse sine function


.. function:: asinh(a: _ArrayLike) -> ulab.array

   Computes the inverse hyperbolic sine function


.. function:: around(a: _ArrayLike, *, decimals: int = 0) -> ulab.array

   Returns a new float array in which each element is rounded to
   ``decimals`` places.


.. function:: atan(a: _ArrayLike) -> ulab.array

   Computes the inverse tangent function; the return values are in the
   range [-pi/2,pi/2].


.. function:: arctan2(ya: _ArrayLike, xa: _ArrayLike) -> ulab.array

   Computes the inverse tangent function of y/x; the return values are in
   the range [-pi, pi].


.. function:: atanh(a: _ArrayLike) -> ulab.array

   Computes the inverse hyperbolic tangent function


.. function:: ceil(a: _ArrayLike) -> ulab.array

   Rounds numbers up to the next whole number


.. function:: cos(a: _ArrayLike) -> ulab.array

   Computes the cosine function


.. function:: cosh(a: _ArrayLike) -> ulab.array

   Computes the hyperbolic cosine function


.. function:: degrees(a: _ArrayLike) -> ulab.array

   Converts angles from radians to degrees


.. function:: erf(a: _ArrayLike) -> ulab.array

   Computes the error function, which has applications in statistics


.. function:: erfc(a: _ArrayLike) -> ulab.array

   Computes the complementary error function, which has applications in statistics


.. function:: exp(a: _ArrayLike) -> ulab.array

   Computes the exponent function.


.. function:: expm1(a: _ArrayLike) -> ulab.array

   Computes $e^x-1$.  In certain applications, using this function preserves numeric accuracy better than the `exp` function.


.. function:: floor(a: _ArrayLike) -> ulab.array

   Rounds numbers up to the next whole number


.. function:: gamma(a: _ArrayLike) -> ulab.array

   Computes the gamma function


.. function:: lgamma(a: _ArrayLike) -> ulab.array

   Computes the natural log of the gamma function


.. function:: log(a: _ArrayLike) -> ulab.array

   Computes the natural log


.. function:: log10(a: _ArrayLike) -> ulab.array

   Computes the log base 10


.. function:: log2(a: _ArrayLike) -> ulab.array

   Computes the log base 2


.. function:: radians(a: _ArrayLike) -> ulab.array

   Converts angles from degrees to radians


.. function:: sin(a: _ArrayLike) -> ulab.array

   Computes the sine function


.. function:: sinh(a: _ArrayLike) -> ulab.array

   Computes the hyperbolic sine


.. function:: sqrt(a: _ArrayLike) -> ulab.array

   Computes the square root


.. function:: tan(a: _ArrayLike) -> ulab.array

   Computes the tangent


.. function:: tanh(a: _ArrayLike) -> ulab.array

   Computes the hyperbolic tangent


.. function:: vectorize(f: Union[Callable[[int], float], Callable[[float], float]], *, otypes: Optional[_DType] = None) -> Callable[[_ArrayLike], ulab.array]

   :param callable f: The function to wrap
   :param otypes: List of array types that may be returned by the function.  None is interpreted to mean the return value is float.

   Wrap a Python function ``f`` so that it can be applied to arrays.
   The callable must return only values of the types specified by ``otypes``, or the result is undefined.



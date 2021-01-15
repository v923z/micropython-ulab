"""Element-by-element functions

These functions can operate on numbers, 1-D iterables, 1-D arrays, or 2-D arrays by
applying the function to every element in the array.  This is typically
much more efficient than expressing the same operation as a Python loop."""

from ulab import _DType, _ArrayLike

def acos(a: _ArrayLike) -> ulab.array:
   """Computes the inverse cosine function"""
   ...

def acosh(a: _ArrayLike) -> ulab.array:
   """Computes the inverse hyperbolic cosine function"""
   ...

def asin(a: _ArrayLike) -> ulab.array:
   """Computes the inverse sine function"""
   ...

def asinh(a: _ArrayLike) -> ulab.array:
   """Computes the inverse hyperbolic sine function"""
   ...

def around(a: _ArrayLike, *, decimals: int = 0) -> ulab.array:
   """Returns a new float array in which each element is rounded to
      ``decimals`` places."""
   ...

def atan(a: _ArrayLike) -> ulab.array:
   """Computes the inverse tangent function; the return values are in the
      range [-pi/2,pi/2]."""
   ...

def arctan2(ya: _ArrayLike, xa: _ArrayLike) -> ulab.array:
   """Computes the inverse tangent function of y/x; the return values are in
      the range [-pi, pi]."""
   ...

def atanh(a: _ArrayLike) -> ulab.array:
   """Computes the inverse hyperbolic tangent function"""
   ...

def ceil(a: _ArrayLike) -> ulab.array:
   """Rounds numbers up to the next whole number"""
   ...

def cos(a: _ArrayLike) -> ulab.array:
   """Computes the cosine function"""
   ...

def cosh(a: _ArrayLike) -> ulab.array:
   """Computes the hyperbolic cosine function"""
   ...

def degrees(a: _ArrayLike) -> ulab.array:
   """Converts angles from radians to degrees"""
   ...

def erf(a: _ArrayLike) -> ulab.array:
   """Computes the error function, which has applications in statistics"""
   ...

def erfc(a: _ArrayLike) -> ulab.array:
   """Computes the complementary error function, which has applications in statistics"""
   ...

def exp(a: _ArrayLike) -> ulab.array:
   """Computes the exponent function."""
   ...

def expm1(a: _ArrayLike) -> ulab.array:
   """Computes $e^x-1$.  In certain applications, using this function preserves numeric accuracy better than the `exp` function."""
   ...

def floor(a: _ArrayLike) -> ulab.array:
   """Rounds numbers up to the next whole number"""
   ...

def gamma(a: _ArrayLike) -> ulab.array:
   """Computes the gamma function"""
   ...

def lgamma(a: _ArrayLike) -> ulab.array:
   """Computes the natural log of the gamma function"""
   ...

def log(a: _ArrayLike) -> ulab.array:
   """Computes the natural log"""
   ...

def log10(a: _ArrayLike) -> ulab.array:
   """Computes the log base 10"""
   ...

def log2(a: _ArrayLike) -> ulab.array:
   """Computes the log base 2"""
   ...

def radians(a: _ArrayLike) -> ulab.array:
   """Converts angles from degrees to radians"""
   ...

def sin(a: _ArrayLike) -> ulab.array:
   """Computes the sine function"""
   ...

def sinh(a: _ArrayLike) -> ulab.array:
   """Computes the hyperbolic sine"""
   ...

def sqrt(a: _ArrayLike) -> ulab.array:
   """Computes the square root"""
   ...

def tan(a: _ArrayLike) -> ulab.array:
   """Computes the tangent"""
   ...

def tanh(a: _ArrayLike) -> ulab.array:
   """Computes the hyperbolic tangent"""
   ...
def vectorize(
    f: Union[Callable[[int], float], Callable[[float], float]],
    *,
    otypes: Optional[_DType] = None
) -> Callable[[_ArrayLike], ulab.array]:
   """
   :param callable f: The function to wrap
   :param otypes: List of array types that may be returned by the function.  None is interpreted to mean the return value is float.

   Wrap a Python function ``f`` so that it can be applied to arrays.
   The callable must return only values of the types specified by ``otypes``, or the result is undefined."""
   ...


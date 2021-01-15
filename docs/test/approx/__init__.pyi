"""Numerical approximation methods"""

def bisect(
    fun: Callable[[float], float],
    a: float,
    b: float,
    *,
    xtol: float = 2.4e-7,
    maxiter: int = 100
) -> float:
    """
    :param callable f: The function to bisect
    :param float a: The left side of the interval
    :param float b: The right side of the interval
    :param float xtol: The tolerance value
    :param float maxiter: The maximum number of iterations to perform

    Find a solution (zero) of the function ``f(x)`` on the interval
    (``a``..``b``) using the bisection method.  The result is accurate to within
    ``xtol`` unless more than ``maxiter`` steps are required."""
    ...

def fmin(
    fun: Callable[[float], float],
    x0: float,
    *,
    xatol: float = 2.4e-7,
    fatol: float = 2.4e-7,
    maxiter: int = 200
) -> float:
    """
    :param callable f: The function to bisect
    :param float x0: The initial x value
    :param float xatol: The absolute tolerance value
    :param float fatol: The relative tolerance value

    Find a minimum of the function ``f(x)`` using the downhill simplex method.
    The located ``x`` is within ``fxtol`` of the actual minimum, and ``f(x)``
    is within ``fatol`` of the actual minimum unless more than ``maxiter``
    steps are requried."""
    ...

def interp(
    x: ulab.array,
    xp: ulab.array,
    fp: ulab.array,
    *,
    left: Optional[float] = None,
    right: Optional[float] = None
) -> ulab.array:
    """
    :param ulab.array x: The x-coordinates at which to evaluate the interpolated values.
    :param ulab.array xp: The x-coordinates of the data points, must be increasing
    :param ulab.array fp: The y-coordinates of the data points, same length as xp
    :param left: Value to return for ``x < xp[0]``, default is ``fp[0]``.
    :param right: Value to return for ``x > xp[-1]``, default is ``fp[-1]``.

    Returns the one-dimensional piecewise linear interpolant to a function with given discrete data points (xp, fp), evaluated at x."""
    ...

def newton(
    fun: Callable[[float], float],
    x0: float,
    *,
    xtol: float = 2.4e-7,
    rtol: float = 0.0,
    maxiter: int = 50
) -> float:
    """
    :param callable f: The function to bisect
    :param float x0: The initial x value
    :param float xtol: The absolute tolerance value
    :param float rtol: The relative tolerance value
    :param float maxiter: The maximum number of iterations to perform

    Find a solution (zero) of the function ``f(x)`` using Newton's Method.
    The result is accurate to within ``xtol * rtol * |f(x)|`` unless more than
    ``maxiter`` steps are requried."""
    ...

def trapz(y: ulab.array, x: Optional[ulab.array] = None, dx: float = 1.0) -> float:
    """
    :param 1D ulab.array y: the values of the dependent variable
    :param 1D ulab.array x: optional, the coordinates of the independent variable. Defaults to uniformly spaced values.
    :param float dx: the spacing between sample points, if x=None

    Returns the integral of y(x) using the trapezoidal rule.
    """
    ...


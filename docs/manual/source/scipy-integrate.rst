
scipy.integrate
===============

This module provides a simplified subset of CPython’s
``scipy.integrate`` module. The algorithms were not ported from
CPython’s ``scipy.integrate`` for the sake of resource usage, but
derived from a paper found in https://www.genivia.com/qthsh.html. There
are four numerical integration algorithms:

1. `scipy.integrate.quad <#quad>`__
2. `scipy.integrate.romberg <#romberg>`__
3. `scipy.integrate.simpson <#simpson>`__
4. `scipy.integrate.tanhsinh <#tanhsinh>`__

Introduction
------------

Numerical integration works best with float64 math enabled. If you
require float64 math, be sure to set ``MICROPY_OBJ_REPR_A`` and
``MICROPY_FLOAT_IMPL_DOUBLE``. This being said, the modules work equally
well using float32, albeit with reduced precision. The required error
tolerance can be specified for each of the function calls using the
“eps=” option, defaulting to the compiled in ``etolerance`` value (1e-14
for fp64, 1e-8 for fp32).

The submodule can be enabled by setting
``ULAB_SCIPY_HAS_INTEGRATE_MODULE`` in ``code/ulab.h``. As for the
individual integration algorithms, you can select which to include by
setting one or more of ``ULAB_INTEGRATE_HAS_QUAD``,
``ULAB_INTEGRATE_HAS_ROMBERG``, ``ULAB_INTEGRATE_HAS_SIMPSON``, and
``ULAB_INTEGRATE_HAS_TANHSINH``.

Also note that these algorithms do not support complex numbers, although
it is certainly possible to implement complex integration in MicroPython
on top of this module, e.g. as in
https://stackoverflow.com/questions/5965583/use-scipy-integrate-quad-to-integrate-complex-numbers.

quad
----

``scipy``:
https://docs.scipy.org/doc/scipy/reference/generated/scipy.integrate.quad.html

In CPython ``scipy.integrate``, ``quad`` is a wrapper implementing many
algorithms based on the Fortran QUADPACK package. Gauss-Kronrod is just
one of them, and it is useful for most general-purpose tasks. This
particular function implements an Adaptive Gauss-Kronrod (G10,K21)
quadrature algorithm. The Gauss–Kronrod quadrature formula is a variant
of Gaussian quadrature, in which the evaluation points are chosen so
that an accurate approximation can be computed by re-using the
information produced by the computation of a less accurate approximation
(https://en.wikipedia.org/wiki/Gauss%E2%80%93Kronrod_quadrature_formula).

The function takes three to five arguments:

-  f, a callable,
-  a and b, the lower and upper integration limit,
-  order=, the order of integration (default 5),
-  eps=, the error tolerance (default etolerance)

The function returns the result and the error estimate as a tuple of
floats.

.. code::
        
    # code to be run in micropython
    
    from ulab import scipy
    
    f = lambda x: x**2 + 2*x + 1
    result = scipy.integrate.quad(f, 0, 5, order=5, eps=1e-10)
    print (f"result = {result}")

.. parsed-literal::

    UsageError: Cell magic `%%micropython` not found.


romberg
-------

``scipy``:
https://docs.scipy.org/doc/scipy/reference/generated/scipy.integrate.romberg.html

This function implements the Romberg quadrature algorithm. Romberg’s
method is a Newton–Cotes formula – it evaluates the integrand at equally
spaced points. The integrand must have continuous derivatives, though
fairly good results may be obtained if only a few derivatives exist. If
it is possible to evaluate the integrand at unequally spaced points,
then other methods such as Gaussian quadrature and Clenshaw–Curtis
quadrature are generally more accurate
(https://en.wikipedia.org/wiki/Romberg%27s_method).

Please note: This function is deprecated as of SciPy 1.12.0 and will be
removed in SciPy 1.15.0. Please use ``scipy.integrate.quad`` instead.

The function takes three to five arguments:

-  f, a callable,
-  a and b, the lower and upper integration limit,
-  steps=, the number of steps taken to calculate (default 100),
-  eps=, the error tolerance (default etolerance)

The function returns the result as a float.

.. code::
        
    # code to be run in micropython
    
    from ulab import scipy
    
    f = lambda x: x**2 + 2*x + 1
    result = scipy.integrate.romberg(f, 0, 5)
    print (f"result = {result}")

.. parsed-literal::

    UsageError: Cell magic `%%micropython` not found.


simpson
-------

``scipy``:
https://docs.scipy.org/doc/scipy/reference/generated/scipy.integrate.simpson.html

This function is different from CPython’s ``simpson`` method in that it
does not take an array of function values but determines the optimal
spacing of samples itself. Adaptive Simpson’s method, also called
adaptive Simpson’s rule, is a method of numerical integration proposed
by G.F. Kuncir in 1962. It is probably the first recursive adaptive
algorithm for numerical integration to appear in print, although more
modern adaptive methods based on Gauss–Kronrod quadrature and
Clenshaw–Curtis quadrature are now generally preferred
(https://en.wikipedia.org/wiki/Adaptive_Simpson%27s_method).

The function takes three to five arguments:

-  f, a callable,
-  a and b, the lower and upper integration limit,
-  steps=, the number of steps taken to calculate (default 100),
-  eps=, the error tolerance (default etolerance)

The function returns the result as a float.

.. code::
        
    # code to be run in micropython
    
    from ulab import scipy
    
    f = lambda x: x**2 + 2*x + 1
    result = scipy.integrate.simpson(f, 0, 5)
    print (f"result = {result}")

.. parsed-literal::

    UsageError: Cell magic `%%micropython` not found.


tanhsinh
--------

``scipy``:
https://docs.scipy.org/doc/scipy/reference/generated/scipy.integrate.quad.html

In CPython ``scipy.integrate``, ``tanhsinh`` is written in Python
(https://github.com/scipy/scipy/blob/main/scipy/integrate/\_tanhsinh.py).
It is used in cases where Newton-Cotes, Gauss-Kronrod, and other
formulae do not work due to properties of the integrand or the
integration limits. (In SciPy v1.14.1, it is not a public function but
it has been marked as public in SciPy v1.15.0rc1).

This particular function implements an optimized Tanh-Sinh, Sinh-Sinh
and Exp-Sinh quadrature algorithm. It is especially applied where
singularities or infinite derivatives exist at one or both endpoints.
The method uses hyperbolic functions in a change of variables to
transform an integral on the interval x ∈ (−1, 1) to an integral on the
entire real line t ∈ (−∞, ∞), the two integrals having the same value.
After this transformation, the integrand decays with a double
exponential rate, and thus, this method is also known as the double
exponential (DE) formula
(https://en.wikipedia.org/wiki/Tanh-sinh_quadrature).

As opposed to the three algorithms mentioned before, it also supports
integrals with infinite limits like the Gaussian integral
(https://en.wikipedia.org/wiki/Gaussian_integral), as shown below.

The function takes three to five arguments:

-  f, a callable,
-  a and b, the lower and upper integration limit,
-  levels=, the number of loops taken to calculate (default 6),
-  eps=, the error tolerance (default: etolerance)

The function returns the result and the error estimate as a tuple of
floats.

.. code::
        
    # code to be run in micropython
    
    from ulab import scipy, numpy as np
    from math import *
    f = lambda x: exp(- x**2)
    result = scipy.integrate.tanhsinh(f, -np.inf, np.inf)
    print (f"result = {result}")
    exact = sqrt(pi)   # which is the exact value
    print (f"exact value = {exact}")

.. parsed-literal::

    UsageError: Cell magic `%%micropython` not found.


.. code::

    # code to be run in CPython
    

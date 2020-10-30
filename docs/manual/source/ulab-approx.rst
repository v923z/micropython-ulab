Interpolation, root finding, and function minimisation
======================================================

The ``approx`` sub-module defines functions for interpolating numerical
data, and finding the roots and the minimum of arbitrary functions
defined in ``python``. Note that routines that work with user-defined
functions still have to call the underlying ``python`` code, and
therefore, gains in speed are not as significant as with other
vectorised operations. As a rule of thumb, a factor of two can be
expected, when compared to an optimised python implementation.

interp
------

``numpy``: https://docs.scipy.org/doc/numpy/numpy.interp

The ``interp`` function returns the linearly interpolated values of a
one-dimensional numerical array. It requires three positional
arguments,\ ``x``, at which the interpolated values are evaluated,
``xp``, the array of the independent variables of the data, and ``fp``,
the array of the dependent values of the data. ``xp`` must be a
monotonically increasing sequence of numbers.

Two keyword arguments, ``left``, and ``right`` can also be supplied;
these determine the return values, if ``x < xp[0]``, and ``x > xp[-1]``,
respectively. If these arguments are not supplied, ``left``, and
``right`` default to ``fp[0]``, and ``fp[-1]``, respectively.

.. code::
        
    # code to be run in micropython
    
    import ulab
    from ulab import approx
    
    x = ulab.array([1, 2, 3, 4, 5])
    xp = ulab.array([1, 2, 3, 4])
    fp = ulab.array([1, 2, 3, 5])
    x = x - 0.2
    print(x)
    print(approx.interp(x, xp, fp))
    print(approx.interp(x, xp, fp, left=0.0))
    print(approx.interp(x, xp, fp, right=10.0))

.. parsed-literal::

    array([0.8, 1.8, 2.8, 3.8, 4.8], dtype=float)
    array([1.0, 1.8, 2.8, 4.6, 5.0], dtype=float)
    array([0.0, 1.8, 2.8, 4.6, 5.0], dtype=float)
    array([1.0, 1.8, 2.8, 4.6, 10.0], dtype=float)
    
    


newton
------

``scipy``:https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.newton.html

``newton`` finds a zero of a real, user-defined function using the
Newton-Raphson (or secant or Halley’s) method. The routine requires two
positional arguments, the function, and the initial value. Three keyword
arguments can be supplied to control the iteration. These are the
absolute and relative tolerances ``tol``, and ``rtol``, respectively,
and the number of iterations before stopping, ``maxiter``. The function
retuns a single scalar, the position of the root.

.. code::
        
    # code to be run in micropython
    
    import ulab
    from ulab import approx
        
    def f(x):
        return x*x*x - 2.0
    
    print(approx.newton(f, 3., tol=0.001, rtol=0.01))

.. parsed-literal::

    1.260135727246117
    
    


bisect
------

``scipy``:
https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.bisect.html

``bisect`` finds the root of a function of one variable using a simple
bisection routine. It takes three positional arguments, the function
itself, and two starting points. The function must have opposite signs
at the starting points. Returned is the position of the root.

Two keyword arguments, ``xtol``, and ``maxiter`` can be supplied to
control the accuracy, and the number of bisections, respectively.

.. code::
        
    # code to be run in micropython
    
    import ulab
    from ulab import approx
        
    def f(x):
        return x*x - 1
    
    print(approx.bisect(f, 0, 4))
    
    print('only 8 bisections: ',  approx.bisect(f, 0, 4, maxiter=8))
    
    print('with 0.1 accuracy: ',  approx.bisect(f, 0, 4, xtol=0.1))

.. parsed-literal::

    0.9999997615814209
    only 8 bisections:  0.984375
    with 0.1 accuracy:  0.9375
    
    


Performance
~~~~~~~~~~~

Since the ``bisect`` routine calls user-defined ``python`` functions,
the speed gain is only about a factor of two, if compared to a purely
``python`` implementation.

.. code::
        
    # code to be run in micropython
    
    import ulab
    from ulab import approx
    
    def f(x):
        return (x-1)*(x-1) - 2.0
    
    def bisect(f, a, b, xtol=2.4e-7, maxiter=100):
        if f(a) * f(b) > 0:
            raise ValueError
    
        rtb = a if f(a) < 0.0 else b
        dx = b - a if f(a) < 0.0 else a - b
        for i in range(maxiter):
            dx *= 0.5
            x_mid = rtb + dx
            mid_value = f(x_mid)
            if mid_value < 0:
                rtb = x_mid
            if abs(dx) < xtol:
                break
    
        return rtb
    
    @timeit
    def bisect_approx(f, a, b):
        return approx.bisect(f, a, b)
    
    @timeit
    def bisect_timed(f, a, b):
        return bisect(f, a, b)
    
    print('bisect running in python')
    bisect_timed(f, 3, 2)
    
    print('bisect running in C')
    bisect_approx(f, 3, 2)

.. parsed-literal::

    bisect running in python
    execution time:  1270  us
    bisect running in C
    execution time:  642  us
    


fmin
----

``scipy``:
https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.fmin.html

The ``fmin`` function finds the position of the minimum of a
user-defined function by using the downhill simplex method. Requires two
positional arguments, the function, and the initial value. Three keyword
arguments, ``xatol``, ``fatol``, and ``maxiter`` stipulate conditions
for stopping.

.. code::
        
    # code to be run in micropython
    import ulab
    from ulab import approx
    
    def f(x):
        return (x-1)**2 - 1
    
    print(approx.fmin(f, 3.0))
    print(approx.fmin(f, 3.0, xatol=0.1))

.. parsed-literal::

    0.9996093749999952
    1.199999999999996
    
    


trapz
-----

``numpy``:
https://numpy.org/doc/stable/reference/generated/numpy.trapz.html

The function takes one or two one-dimensional ``ndarray``\ s, and
integrates the dependent values (``y``) using the trapezoidal rule. If
the independent variable (``x``) is given, that is taken as the sample
points corresponding to ``y``.

.. code::
        
    # code to be run in micropython
    
    import ulab
    from ulab import approx
    
    x = ulab.linspace(0, 9, num=10)
    y = x*x
    
    print('x: ',  x)
    print('y: ',  y)
    print('============================')
    print('integral of y: ', approx.trapz(y))
    print('integral of y at x: ', approx.trapz(y, x=x))

.. parsed-literal::

    x:  array([0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0], dtype=float)
    y:  array([0.0, 1.0, 4.0, 9.0, 16.0, 25.0, 36.0, 49.0, 64.0, 81.0], dtype=float)
    ============================
    integral of y:  244.5
    integral of y at x:  244.5
    
    


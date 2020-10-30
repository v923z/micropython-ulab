:mod:`ulab.poly`
================

.. py:module:: ulab.poly

.. autoapi-nested-parse::

   Polynomial functions



.. function:: polyfit(y: _ArrayLike, degree: int) -> ulab.array


.. function:: polyfit(x: _ArrayLike, y: _ArrayLike, degree: int) -> ulab.array

   Return a polynomial of given degree that approximates the function
   f(x)=y.  If x is not supplied, it is the range(len(y)).


.. function:: polyval(p: _ArrayLike, x: _ArrayLike) -> ulab.array

   Evaluate the polynomial p at the points x.  x must be an array.



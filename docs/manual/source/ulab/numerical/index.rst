:mod:`ulab.numerical`
=====================

.. py:module:: ulab.numerical

.. autoapi-nested-parse::

   Numerical and Statistical functions

   Most of these functions take an "axis" argument, which indicates whether to
   operate over the flattened array (None), or a particular axis (integer).



.. function:: argmax(array: _ArrayLike, *, axis: Optional[int] = None) -> int

   Return the index of the maximum element of the 1D array


.. function:: argmin(array: _ArrayLike, *, axis: Optional[int] = None) -> int

   Return the index of the minimum element of the 1D array


.. function:: argsort(array: ulab.array, *, axis: int = -1) -> ulab.array

   Returns an array which gives indices into the input array from least to greatest.


.. function:: cross(a: ulab.array, b: ulab.array) -> ulab.array

   Return the cross product of two vectors of length 3


.. function:: diff(array: ulab.array, *, n: int = 1, axis: int = -1) -> ulab.array

   Return the numerical derivative of successive elements of the array, as
   an array.  axis=None is not supported.


.. function:: flip(array: ulab.array, *, axis: Optional[int] = None) -> ulab.array

   Returns a new array that reverses the order of the elements along the
   given axis, or along all axes if axis is None.


.. function:: max(array: _ArrayLike, *, axis: Optional[int] = None) -> float

   Return the maximum element of the 1D array


.. function:: mean(array: _ArrayLike, *, axis: Optional[int] = None) -> float

   Return the mean element of the 1D array, as a number if axis is None, otherwise as an array.


.. function:: min(array: _ArrayLike, *, axis: Optional[int] = None) -> float

   Return the minimum element of the 1D array


.. function:: roll(array: ulab.array, distance: int, *, axis: Optional[int] = None) -> None

   Shift the content of a vector by the positions given as the second
   argument. If the ``axis`` keyword is supplied, the shift is applied to
   the given axis.  The array is modified in place.


.. function:: sort(array: ulab.array, *, axis: int = -1) -> ulab.array

   Sort the array along the given axis, or along all axes if axis is None.
   The array is modified in place.


.. function:: std(array: _ArrayLike, *, axis: Optional[int] = None, ddof: int = 0) -> float

   Return the standard deviation of the array, as a number if axis is None, otherwise as an array.


.. function:: sum(array: _ArrayLike, *, axis: Optional[int] = None) -> Union[float, int, ulab.array]

   Return the sum of the array, as a number if axis is None, otherwise as an array.



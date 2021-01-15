"""Manipulate numeric data similar to numpy

`ulab` is a numpy-like module for micropython, meant to simplify and
speed up common mathematical operations on arrays. The primary goal was to
implement a small subset of numpy that might be useful in the context of a
microcontroller. This means low-level data processing of linear (array) and
two-dimensional (matrix) data.

`ulab` is adapted from micropython-ulab, and the original project's
documentation can be found at
https://micropython-ulab.readthedocs.io/en/latest/

`ulab` is modeled after numpy, and aims to be a compatible subset where
possible.  Numpy's documentation can be found at
https://docs.scipy.org/doc/numpy/index.html"""

from typing import Dict

_DType = int
"""`ulab.int8`, `ulab.uint8`, `ulab.int16`, `ulab.uint16`, `ulab.float` or `ulab.bool`"""

_float = float
"""Type alias of the bulitin float"""

_bool = bool
"""Type alias of the bulitin bool"""

_Index = Union[int, slice, ulab.array, Tuple[Union[int, slice], ...]]

class array:
    """1- and 2- dimensional array"""

    def __init__(
        self,
        values: Union[array, Iterable[Union[_float, _bool, Iterable[Any]]]],
        *,
        dtype: _DType = ulab.float
    ) -> None:
        """:param sequence values: Sequence giving the initial content of the array.
          :param ~ulab._DType dtype: The type of array values, `ulab.int8`, `ulab.uint8`, `ulab.int16`, `ulab.uint16`, `ulab.float` or `ulab.bool`

          The ``values`` sequence can either be another ~ulab.array, sequence of numbers
          (in which case a 1-dimensional array is created), or a sequence where each
          subsequence has the same length (in which case a 2-dimensional array is
          created).

          Passing a `ulab.array` and a different dtype can be used to convert an array
          from one dtype to another.

          In many cases, it is more convenient to create an array from a function
          like `zeros` or `linspace`.

          `ulab.array` implements the buffer protocol, so it can be used in many
          places an `array.array` can be used."""
        ...

    shape: Tuple[int, ...]
    """The size of the array, a tuple of length 1 or 2"""

    size: int
    """The number of elements in the array"""

    itemsize: int
    """The size of a single item in the array"""

    strides: Tuple[int, ...]
    """Tuple of bytes to step in each dimension, a tuple of length 1 or 2"""

    def copy(self) -> ulab.array:
        """Return a copy of the array"""
        ...

    def flatten(self, *, order: str = "C") -> ulab.array:
        """:param order: Whether to flatten by rows ('C') or columns ('F')

           Returns a new `ulab.array` object which is always 1 dimensional.
           If order is 'C' (the default", then the data is ordered in rows;
           If it is 'F', then the data is ordered in columns.  "C" and "F" refer
           to the typical storage organization of the C and Fortran languages."""
        ...

    def reshape(self, shape: Tuple[int, ...]) -> ulab.array:
        """Returns an array containing the same data with a new shape."""
        ...

    def sort(self, *, axis: Optional[int] = 1) -> None:
        """:param axis: Whether to sort elements within rows (0), columns (1), or elements (None)"""
        ...

    def tobytes(self) -> bytearray:
        """Return the raw data bytes in the array"""
        ...

    def transpose(self) -> ulab.array:
        """Swap the rows and columns of a 2-dimensional array"""
        ...

    def __add__(self, other: Union[array, _float]) -> ulab.array:
        """Adds corresponding elements of the two arrays, or adds a number to all
           elements of the array.  If both arguments are arrays, their sizes must match."""
        ...
    def __radd__(self, other: _float) -> ulab.array: ...

    def __sub__(self, other: Union[array, _float]) -> ulab.array:
        """Subtracts corresponding elements of the two arrays, or subtracts a number from all
           elements of the array.  If both arguments are arrays, their sizes must match."""
        ...
    def __rsub__(self, other: _float) -> ulab.array: ...

    def __mul__(self, other: Union[array, _float]) -> ulab.array:
        """Multiplies corresponding elements of the two arrays, or multiplies
           all elements of the array by a number.  If both arguments are arrays,
           their sizes must match."""
        ...
    def __rmul__(self, other: _float) -> ulab.array: ...

    def __div__(self, other: Union[array, _float]) -> ulab.array:
        """Multiplies corresponding elements of the two arrays, or divides
           all elements of the array by a number.  If both arguments are arrays,
           their sizes must match."""
        ...
    def __rdiv__(self, other: _float) -> ulab.array: ...

    def __pow__(self, other: Union[array, _float]) -> ulab.array:
        """Computes the power (x**y) of corresponding elements of the the two arrays,
           or one number and one array.  If both arguments are arrays, their sizes
           must match."""
        ...
    def __rpow__(self, other: _float) -> ulab.array: ...

    def __inv__(self) -> ulab.array:
        ...
    def __neg__(self) -> ulab.array:
        ...
    def __pos__(self) -> ulab.array:
        ...
    def __abs__(self) -> ulab.array:
        ...
    def __len__(self) -> int:
        ...
    def __lt__(self, other: Union[array, _float]) -> ulab.array:
        ...
    def __le__(self, other: Union[array, _float]) -> ulab.array:
        ...
    def __gt__(self, other: Union[array, _float]) -> ulab.array:
        ...
    def __ge__(self, other: Union[array, _float]) -> ulab.array:
        ...

    def __iter__(self) -> Union[Iterator[array], Iterator[_float]]:
        ...

    def __getitem__(self, index: _Index) -> Union[array, _float]:
        """Retrieve an element of the array."""
        ...

    def __setitem__(self, index: _Index, value: Union[array, _float]) -> None:
        """Set an element of the array."""
        ...

_ArrayLike = Union[array, List[_float], Tuple[_float], range]
"""`ulab.array`, ``List[float]``, ``Tuple[float]`` or `range`"""

int8: _DType
"""Type code for signed integers in the range -128 .. 127 inclusive, like the 'b' typecode of `array.array`"""

int16: _DType
"""Type code for signed integers in the range -32768 .. 32767 inclusive, like the 'h' typecode of `array.array`"""

float: _DType
"""Type code for floating point values, like the 'f' typecode of `array.array`"""

uint8: _DType
"""Type code for unsigned integers in the range 0 .. 255 inclusive, like the 'H' typecode of `array.array`"""

uint16: _DType
"""Type code for unsigned integers in the range 0 .. 65535 inclusive, like the 'h' typecode of `array.array`"""

bool: _DType
"""Type code for boolean values"""

def get_printoptions() -> Dict[str, int]:
    """Get printing options"""
    ...

def set_printoptions(threshold: Optional[int] = None, edgeitems: Optional[int] = None) -> None:
    """Set printing options"""
    ...

def ndinfo(array: ulab.array) -> None:
    ...

@overload
def arange(stop: _float, step: _float = 1, *, dtype: _DType = ulab.float) -> ulab.array: ...
@overload
def arange(start: _float, stop: _float, step: _float = 1, *, dtype: _DType = ulab.float) -> ulab.array:
    """
    .. param: start
      First value in the array, optional, defaults to 0
    .. param: stop
      Final value in the array
    .. param: step
      Difference between consecutive elements, optional, defaults to 1.0
    .. param: dtype
      Type of values in the array

    Return a new 1-D array with elements ranging from ``start`` to ``stop``, with step size ``step``."""
    ...

def concatenate(arrays: Tuple[ulab.array], *, axis: int = 0) -> ulab.array:
    """
    .. param: arrays
      tuple of ndarrays
    .. param: axis
      axis along which the arrays will be joined

    Join a sequence of arrays along an existing axis."""
    ...

def eye(size: int, *, M: Optional[int] = None, k: int = 0, dtype: _DType = ulab.float) -> ulab.array:
    """Return a new square array of size, with the diagonal elements set to 1
       and the other elements set to 0."""
    ...

def full(shape: Union[int, Tuple[int, ...]], fill_value: Union[_float, _bool], *, dtype: _DType = ulab.float) -> ulab.array:
   """
   .. param: shape
      Shape of the array, either an integer (for a 1-D array) or a tuple of integers (for tensors of higher rank)
   .. param: fill_value
      scalar, the value with which the array is filled
   .. param: dtype
      Type of values in the array

   Return a new array of the given shape with all elements set to 0."""
   ...

def linspace(
    start: _float,
    stop: _float,
    *,
    dtype: _DType = ulab.float,
    num: int = 50,
    endpoint: _bool = True,
    retstep: _bool = False
) -> ulab.array:
    """
    .. param: start
      First value in the array
    .. param: stop
      Final value in the array
    .. param int: num
      Count of values in the array.
    .. param: dtype
      Type of values in the array
    .. param bool: endpoint
      Whether the ``stop`` value is included.  Note that even when
      endpoint=True, the exact ``stop`` value may not be included due to the
      inaccuracy of floating point arithmetic.
      If True, return (`samples`, `step`), where `step` is the spacing between samples.

    Return a new 1-D array with ``num`` elements ranging from ``start`` to ``stop`` linearly."""
    ...

def logspace(
    start: _float,
    stop: _float,
    *,
    dtype: _DType = ulab.float,
    num: int = 50,
    endpoint: _bool = True,
    base: _float = 10.0
) -> ulab.array:
    """
    .. param: start
      First value in the array
    .. param: stop
      Final value in the array
    .. param int: num
      Count of values in the array. Defaults to 50.
    .. param: base
      The base of the log space. The step size between the elements in
      ``ln(samples) / ln(base)`` (or ``log_base(samples)``) is uniform. Defaults to 10.0.
    .. param: dtype
      Type of values in the array
    .. param bool: endpoint
      Whether the ``stop`` value is included.  Note that even when
      endpoint=True, the exact ``stop`` value may not be included due to the
      inaccuracy of floating point arithmetic. Defaults to True.

    Return a new 1-D array with ``num`` evenly spaced elements on a log scale.
    The sequence starts at ``base ** start``, and ends with ``base ** stop``."""
    ...

def ones(shape: Union[int, Tuple[int, ...]], *, dtype: _DType = ulab.float) -> ulab.array:
   """
   .. param: shape
      Shape of the array, either an integer (for a 1-D array) or a tuple of 2 integers (for a 2-D array)
   .. param: dtype
      Type of values in the array

   Return a new array of the given shape with all elements set to 1."""
   ...

def zeros(shape: Union[int, Tuple[int, ...]], *, dtype: _DType = ulab.float) -> ulab.array:
   """
   .. param: shape
      Shape of the array, either an integer (for a 1-D array) or a tuple of 2 integers (for a 2-D array)
   .. param: dtype
      Type of values in the array

   Return a new array of the given shape with all elements set to 0."""
   ...


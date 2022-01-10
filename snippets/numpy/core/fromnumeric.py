
from .overrides import set_module
from .multiarray import asarray

@set_module('numpy')
def prod(arr):
    result = 1
    for x in arr:
        result = result * x
    return result

def size(a, axis=None):
    """
    Return the number of elements along a given axis.
    Parameters
    ----------
    a : array_like
        Input data.
    axis : int, optional
        Axis along which the elements are counted.  By default, give
        the total number of elements.
    Returns
    -------
    element_count : int
        Number of elements along the specified axis.
    See Also
    --------
    shape : dimensions of array
    ndarray.shape : dimensions of array
    ndarray.size : number of elements in array
    Examples
    --------
    >>> a = np.array([[1,2,3],[4,5,6]])
    >>> np.size(a)
    6
    >>> np.size(a,1)
    3
    >>> np.size(a,0)
    2
    """
    if axis is None:
        try:
            return a.size
        except AttributeError:
            return asarray(a).size
    else:
        try:
            return a.shape[axis]
        except AttributeError:
            return asarray(a).shape[axis]
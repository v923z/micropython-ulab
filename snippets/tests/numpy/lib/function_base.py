from ulab import numpy as np
from ..core.multiarray import asarray

def append(arr, values, axis=None):
    arr = asarray(arr)
    if axis is None:
        if len(arr.shape) != 1:
            arr.flatten()
        values.flatten()
        axis = len(arr.shape)-1
    return np.concatenate((arr, values), axis=axis)
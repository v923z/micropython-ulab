from ulab import numpy as np
from ..core.multiarray import (asarray)
from ..core.overrides import set_module

@set_module('numpy')
def append(arr, values, axis=None):
    arr = asarray(arr)
    values = asarray(values)
    if axis is None:
        if len(arr.shape) != 1:
            arr = arr.flatten()
        values = values.flatten()
        axis = len(arr.shape)-1
    return np.concatenate((arr, values), axis=axis)


def delete(arr, obj, axis=None):
    mask = np.ones(len(arr), dtype=np.bool)
    mask[obj] = 0
    return arr[mask]
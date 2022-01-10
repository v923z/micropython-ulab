from ulab import numpy as np
from .multiarray import asarray

def atleast_1d(*arys):
    res = []
    for ary in arys:
        ary = asarray(ary)
        if not isinstance(ary,(np.ndarray)):
            result = ary.reshape((1,))
        else:
            result = ary
        res.append(result)
    if len(res) == 1:
        return res[0]
    else:
        return res
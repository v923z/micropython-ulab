import sys

use_ulab = False

try:
    from ubinascii import b2a_base64 as b64encode
    from ubinascii import hexlify
    import ujson as json
    from ulab import numpy as np
    use_ulab = True
except:
    from base64 import b64encode
    import json
    import numpy as np
    from numpy.lib.format import dtype_to_descr

def ulab_dtype_to_descr(dtype):
    desc = '<'
    if sys.byteorder == 'little':
        desc = '>'

    if dtype == ord('B'):
        desc = desc + 'u1'
    elif dtype == ord('b'):
        desc = desc + 'i1'
    elif dtype == ord('H'):
        desc = desc + 'u2'
    elif dtype == ord('h'):
        desc = desc + 'i2'
    elif dtype == ord('d'):
        desc = desc + 'f8'
    elif dtype == ord('f'):
        desc = desc + 'f4'
    elif dtype == ord('c'):
        desc = desc + 'c16'
        if np.array([1], dtype=np.float).itemsize == 4:
            desc = desc + 'c8'

    return desc

def ndarray_to_json(obj, b64=False):
    if not isinstance(obj, np.ndarray):
         raise TypeError('input argument must be an ndarray')

    if use_ulab:
        dtype_desciptor = ulab_dtype_to_descr(obj.dtype)
    else:
        dtype_desciptor = dtype_to_descr(obj.dtype)

    if not b64:
        data = hexlify(obj.tobytes())
    else:
        data = b64encode(obj.tobytes())

    return json.dumps({'__numpy__': data, 'dtype': dtype_desciptor, 'shape': obj.shape})


dtypes = (np.uint8, np.int8, np.uint16, np.int16, np.float)

for dtype in dtypes:
    ndarray = np.array([1, 2, 3], dtype=dtype)
    print(ndarray_to_json(ndarray))
    print(ndarray_to_json(ndarray, b64=True))

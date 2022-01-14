import sys

use_ulab = False

try:
    from ubinascii import a2b_base64 as b64decode
    from ubinascii import unhexlify
    import ujson as json
    from ulab import numpy as np
    use_ulab = True
except:
    from base64 import b64decode
    import json
    import numpy as np
    from numpy.lib.format import descr_to_dtype

def ulab_descr_to_dtype(descriptor):
    descriptor = descriptor[1:]

    if descriptor == 'u1':
        return np.uint8
    elif descriptor == 'i1':
        return np.int8
    if descriptor == 'u2':
        return np.uint16
    if descriptor == 'i2':
        return np.int16
    elif descriptor == 'f8':
        if np.float != ord('d'):
            raise TypeError('doubles are not supported')
        else:
            return np.float
    elif descriptor == 'f16':
        if np.float != ord('f'):
            raise TypeError('')
        else:
            return np.float


def json_to_ndarray(json_string, b64=False):
    obj = json.loads(json_string)
    print(obj)
    if not isinstance(obj, dict):
         raise TypeError('input argument must be a dictionary')
    if set(obj.keys()) != {'__numpy__', 'dtype', 'shape'}:
        raise ValueError('input must have the keys "__numpy__", "dtype", "shape"')

    descriptor = obj['dtype']
    if use_ulab:
        dtype = ulab_descr_to_dtype(descriptor)
    else:
        dtype = descr_to_dtype(descriptor)

    if not b64:
        data = unhexlify(obj['__numpy__'])
    else:
        data = b64decode(obj['__numpy__'])

    ndarray = np.frombuffer(data, dtype=dtype).reshape(tuple(obj['shape']))

    if dtype in (np.uint16, np.int16, np.float):
        if sys.byteorder != descriptor[1]:
            ndarray.byteswap()

    return ndarray


str = '{"dtype": "<f8", "__numpy__": "AAAAAAAAAAAAAAAAAADwPwAAAAAAAABAAAAAAAAACEAAAAAAAAAQQAAAAAAAABRAAAAAAAAAGEAAAAAAAAAcQAAAAAAAACBA\n", "shape": [3, 3]}'

print(json_to_ndarray(str, b64=True))
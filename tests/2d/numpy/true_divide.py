try:
    from ulab import numpy as np
except ImportError:
    import numpy as np

dtypes = (np.uint8, np.int8, np.uint16, np.int16)

for left_dtype in dtypes:
    left = np.array([1, 2, 3, 4], dtype=left_dtype)
    for right_dtype in dtypes:
        right = np.array([4, 4, 4, 4], dtype=right_dtype)
        print(left / right)

for dtype in dtypes:
    values = np.array([1, 2, 3, 4], dtype=dtype)
    reciprocals = np.array([1, 2, 4, 8], dtype=dtype)
    print(values / 4)
    print(4 / reciprocals)

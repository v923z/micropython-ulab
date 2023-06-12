try:
    from ulab import numpy as np
except:
    import numpy as np


shift_values = (
    (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
    (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1),
    (2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2),
)


for shift_value in shift_values:
    dtypes = (np.uint8, np.int8, np.uint16, np.int16)
    for dtype1 in dtypes:
        x1 = np.array([0, 1, 2, 4, 8, 16, 32, 3, 5, 7, 11, 13], dtype=dtype1)
        for dtype2 in dtypes:
            x2 = np.array(shift_value, dtype=dtype2)
            print(np.left_shift(x1, x2))

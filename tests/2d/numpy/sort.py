try:
    from ulab import numpy as np
except:
    import numpy as np

# 2D sort and argsort over all dtypes, all axes
for dtype in (np.uint8, np.int8, np.uint16, np.int16, np.float):
    a = np.array([[4, 1, 3], [2, 5, 0]], dtype=dtype)
    print(np.sort(a, axis=None))
    print(np.sort(a, axis=0))
    print(np.sort(a, axis=1))
    print(np.argsort(a, axis=0))
    print()

# 2D sort with negative values for signed and float dtypes, axis=0 and axis=1
for dtype in (np.int8, np.int16, np.float):
    a = np.array([[-3, 1], [2, -4], [-1, 3]], dtype=dtype)
    print(np.sort(a, axis=0))
    a = np.array([[3, -1, 2], [-4, 5, -2]], dtype=dtype)
    print(np.sort(a, axis=1))
    print()

# 2D median with negative values for signed and float dtypes, axis=0 and axis=1
for dtype in (np.int16, np.float):
    a = np.array([[-3, 1, 2], [0, -4, 1], [2, 3, -1]], dtype=dtype)
    print(np.median(a, axis=0))
    print(np.median(a, axis=1))
    print()




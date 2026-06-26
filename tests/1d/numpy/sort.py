try:
    from ulab import numpy as np
except:
    import numpy as np

# sort and argsort over all dtypes, positive values
for dtype in (np.uint8, np.int8, np.uint16, np.int16, np.float):
    print()
    a = np.array([], dtype=dtype)
    print(np.sort(a, axis=0))
    print(list(np.argsort(a, axis=0)))

    a = np.array([4, 1, 3, 2], dtype=dtype)
    print(np.sort(a, axis=0))
    print(list(np.argsort(a, axis=0)))

# sort and argsort with negative values for signed and float dtypes
for dtype in (np.int8, np.int16, np.float):
    print()
    a = np.array([-3, 1, -2, 0], dtype=dtype)
    print(np.sort(a, axis=0))
    print(list(np.argsort(a, axis=0)))

    a = np.array([5, -1, 3, -4, 2], dtype=dtype)
    print(np.sort(a, axis=0))
    print(list(np.argsort(a, axis=0)))

# median with negative values for signed and float dtypes
for dtype in (np.int8, np.int16, np.float):
    print()
    a = np.array([-5, -1, -3, -2, -4], dtype=dtype)
    print(np.median(a))

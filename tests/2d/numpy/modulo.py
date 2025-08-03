try:
    from ulab import numpy as np
except:
    import numpy as np


dtypes = (np.uint8, np.int8, np.uint16, np.int16, np.float)

for dtype1 in dtypes:
    x1 = np.array(range(6), dtype=dtype1).reshape((2, 3))
    for dtype2 in dtypes:
        x2 = np.array(range(1, 4), dtype=dtype2)
        print(x1 % x2)

print()
print('=' * 30)
print('inplace modulo')
print('=' * 30)
print()

for dtype1 in dtypes:
    x1 = np.array(range(6), dtype=dtype1).reshape((2, 3))
    for dtype2 in dtypes:
        x2 = np.array(range(1, 4), dtype=dtype2)
        x1 %= x2
        print(x1)

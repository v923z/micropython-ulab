try:
    from ulab import numpy as np
except:
    import numpy as np

dtypes = (np.uint8, np.int8, np.uint16, np.int16, np.float)

print('flattened array')
for dtype in dtypes:
    a = np.array(range(12), dtype=dtype).reshape((3, 4))
    print(np.take(a, (0, 10)))

for dtype in dtypes:
    a = np.array(range(12), dtype=dtype).reshape((3, 4))
    print('\na:', a)
    print('\nfirst axis')
    print(np.take(a, (0, 2, 2, 1), axis=0))
    print('\nsecond axis')
    print(np.take(a, (0, 2, 2, 1), axis=1))


try:
    from ulab import numpy as np
except ImportError:
    import numpy as np

for dtype in (np.uint8, np.int8, np.uint16, np.int8, np.float):
    a = np.array(range(12), dtype=dtype)
    b = a.reshape((3, 4))

    print(a)
    print(b)
    print()

    print(np.sum(a))
    print(np.sum(a, axis=0))
    print(np.sum(a, axis=0, keepdims=True))

    print()
    print(np.sum(b))
    print(np.sum(b, axis=0))
    print(np.sum(b, axis=1))
    print(np.sum(b, axis=0, keepdims=True))
    print(np.sum(b, axis=1, keepdims=True))
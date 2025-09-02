try:
    from ulab import numpy as np
except:
    import numpy as np

for dtype in (np.uint8, np.uint16):
    a = np.array([0, 1, 1, 3, 3, 3], dtype=dtype)
    print(np.bincount(a))

for dtype in (np.uint8, np.uint16):
    a = np.array([0, 2, 2, 4], dtype=dtype)
    print(np.bincount(a, minlength=3))

for dtype in (np.uint8, np.uint16):
    a = np.array([0, 2, 2, 4], dtype=dtype)
    print(np.bincount(a, minlength=8))

for dtype in (np.uint8, np.uint16):
    a = np.array([], dtype=dtype)
    print(np.bincount(a))
    print(np.bincount(a, minlength=8))

for dtype in (np.uint8, np.uint16):
    a = np.array([0, 1, 1, 3], dtype=dtype)
    w = np.array([0.5, 1.0, 2.5, 0.25])
    print(np.where(abs(np.bincount(a, weights=w) - np.array([0.5, 2.0, 0.0, 0.25])) < 0.001, 1, 0))

    w = np.array([1, 2, 3, 4], dtype=np.uint8)
    print(np.bincount(a, weights=w))

for dtype in (np.uint8, np.uint16):
    a = np.array([1, 1], dtype=dtype)
    w = np.array([0.5, 1.5])
    print(np.bincount(a, weights=w, minlength=4))

for dtype in (np.uint8, np.uint16):
    a = np.array([2, 2, 2, 3], dtype=dtype)
    for wtype in (np.uint8, np.uint16, np.int8, np.int16, np.float):
        w = np.array([1, 2, 3, 4], dtype=wtype)
        print(np.bincount(a, weights=w))

for dtype in (np.int8, np.int16, np.float):
    a = np.array([2, 2, 2, 3], dtype=dtype)
    try:
        np.bincount(a)
    except Exception as e:
        print(e)

for dtype in (np.uint8, np.int8, np.uint16, np.int16, np.float):
    a = np.array(range(4), dtype=dtype).reshape((2, 2))
    try:
        np.bincount(a)
    except Exception as e:
        print(e)

for dtype in (np.uint8, np.uint16):
    a = np.array([1, 2, 3], dtype=dtype)
    w = np.array([1, 2])
    try:
        np.bincount(a, weights=w)
    except Exception as e:
        print(e)

for dtype in (np.uint8, np.uint16):
    a = np.array([1, 2, 3], dtype=dtype)
    try:
        np.bincount(a, minlength=-1)
    except Exception as e:
        print(e)

for dtype in (np.uint8, np.uint16):
    a = np.array([1, 2, 3], dtype=dtype)
    w = np.array([1j, 2j, 3j], dtype=np.complex)
    try:
        np.bincount(a, weights=w)
    except Exception as e:
        print(e)


a = np.array([0, 1000], dtype=np.uint16)
y = np.bincount(a)
print(y[0], y[1000], len(y))
try:
    import numpy as np
except:
    import ulab as np

def itemsize(x):
    x = x.itemsize
    if not isinstance(x, int): return x()
    return x

def size(x):
    x = x.size
    if not isinstance(x, int): return x()
    return x

def dtype(x):
    x = x.dtype
    print("dtype", x, type(x))
    if not isinstance(x, (int, type, np.dtype)): return x()
    return x

def shape(x):
    x = x.shape
    if callable(x): return x()
    return x

a = np.array([1, 2, 3, 4], dtype=np.int8)
b = a.copy()
print(b)
a = np.array([[1,2,3],[4,5,6],[7,8,9]], dtype=np.int16)
b = a.copy()
print(b)
a = np.array([[1,2,3],[4,5,6],[7,8,9]], dtype=np.float)
b = a.copy()
print(b)
print(dtype(a))
print(a.flatten())
print(itemsize(np.array([1,2,3], dtype=np.uint8)))
print(itemsize(np.array([1,2,3], dtype=np.uint16)))
print(itemsize(np.array([1,2,3], dtype=np.int8)))
print(itemsize(np.array([1,2,3], dtype=np.int16)))
print(itemsize(np.array([1,2,3], dtype=np.float)))
print(shape(np.array([1,2,3], dtype=np.float)))
print(shape(np.array([[1],[2],[3]], dtype=np.float)))
print(np.array([[1],[2],[3]], dtype=np.float).reshape((1,3)))
print(size(np.array([[1],[2],[3]], dtype=np.float)))
print(size(np.array([1,2,3], dtype=np.float)))
print(np.array([1,2,3], dtype=np.uint8).tobytes())
print(np.array([1,2,3], dtype=np.int8).tobytes())
print(shape(np.array([1,2,3], dtype=np.float).transpose()))
print(shape(np.array([[1],[2],[3]], dtype=np.float).transpose()))

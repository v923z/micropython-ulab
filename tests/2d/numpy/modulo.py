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

# integer lhs %= float rhs raises TypeError (cannot store float result in integer array),
# matching NumPy's casting semantics for in-place operations.
for dtype1 in dtypes:
    x1 = np.array(range(6), dtype=dtype1).reshape((2, 3))
    for dtype2 in dtypes:
        x2 = np.array(range(1, 4), dtype=dtype2)
        try:
            x1 %= x2
            print(x1)
        except TypeError:
            print('TypeError')

print()
print('=' * 30)
print('regression tests')
print('=' * 30)
print()

# Bug: binary uint16 % uint8 was returning dtype=uint8 with corrupted values.
# Result dtype must be uint16 and values must be correct.
a = np.array([0, 1, 2, 3, 4, 5], dtype=np.uint16)
b = np.array([1, 2, 3, 1, 2, 3], dtype=np.uint8)
r = a % b
print(r)

# Bug: inplace_modulo was disabled by a typo in the feature-guard macro
# (NDARRAY_HAS_INPLACE_MODU instead of NDARRAY_HAS_INPLACE_MODULO), so %=
# silently fell back to binary % for all types.  Verify it now works in-place
# for integer types without changing the array dtype.
a = np.array([3, 4, 5], dtype=np.uint8)
a %= np.array([2, 3, 4], dtype=np.uint8)
print(a)

a = np.array([3, 4, 5], dtype=np.int8)
a %= np.array([2, 3, 4], dtype=np.int8)
print(a)

a = np.array([3, 4, 5], dtype=np.uint16)
a %= np.array([2, 3, 4], dtype=np.uint8)
print(a)

# Bug: inplace float %= int8 was a no-op because the second branch checked
# NDARRAY_UINT8 again instead of NDARRAY_INT8.  Verify fmod is applied.
a = np.array([3.5, 4.5, 5.5])
a %= np.array([2, 3, 4], dtype=np.int8)
print(a)

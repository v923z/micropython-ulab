try:
    from ulab import numpy as np
except ImportError:
    import numpy as np

dtypes = (np.uint8, np.int8, np.uint16, np.int16, np.float)

print("Basic 2D transpose")
a = np.array(range(6)).reshape((2, 3))
print("Original array:")
print(a)
print("Transposed (no axes):")
print(a.transpose())
print()

print("2D transpose with axes=(1, 0)")
a = np.array(range(6)).reshape((2, 3))
print("Original array:")
print(a)
print("Transposed with axes=(1, 0):")
print(a.transpose(axes=(1, 0)))
print()

print("2D transpose with axes=(0, 1)")
a = np.array(range(6)).reshape((2, 3))
print("Original array:")
print(a)
print("Transposed with axes=(0, 1):")
print(a.transpose(axes=(0, 1)))
print()

print("1D array transpose")
a = np.array(range(5))
print("Original array:")
print(a)
print("Transposed array:")
print(a.transpose())
print()

print("Transpose of different dtypes")
for dtype in dtypes:
    a = np.array(range(4), dtype=dtype).reshape((2, 2))
    print('Original array:')
    print(a)
    print('Transposed array:')
    print(a.transpose())
print()

print(".T property vs transpose() method")
a = np.array(range(6)).reshape((2, 3))
print("Original array:")
print(a)
print("Using .T property:")
print(a.T)
print("Using .transpose():")
print(a.transpose())
print("Both should be equal:", np.all(a.T == a.transpose()))
print()
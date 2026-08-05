from ulab import numpy as np

a = np.array(range(0, 100, 10), dtype=np.uint8)
b = np.array([0.5, 1.5, 0.2, 4.3], dtype=np.float)

# integer array indexing
print(a[np.array([0, 4, 2], dtype=np.uint8)])
print(b[np.array([3, 2, 2, 3], dtype=np.int16)])
# TODO: test negative indices
# TODO: check range checking
 
# boolean array indexing
print(a[a >= 50])
print(b[b > 1])

# boolean array index assignment
a[a > 1] = 0
print(a)

b[b > 50] += 5
print(b)

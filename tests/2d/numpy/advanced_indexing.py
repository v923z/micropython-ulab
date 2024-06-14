from ulab import numpy as np

a = np.array(range(0, 100, 10), dtype=np.uint8)
b = np.array([0.5, 1.5, 0.2, 4.3], dtype=np.float)

# integer array indexing
print(a[np.array([[0, 4], [1, 2]], dtype=np.uint8)])
print(b[np.array([[3, 2], [2, 3]], dtype=np.uint8)])
# TODO: test negative indices
# TODO: check range checking

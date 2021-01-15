from ulab import numpy as np

a = np.array([[1, 2], [3, 4]])
print(np.linalg.inv(a))

b = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 7]])
print(np.linalg.inv(b))

c = np.array([[1, 2, 0, 0], [0, 6, 7, 0], [0, 0, 8, 9], [0, 0, 15, 13]])
print(np.linalg.inv(c))

print(np.linalg.det(a))
print(np.linalg.det(b))
print(np.linalg.det(c))



import ulab
from ulab import linalg

a = ulab.array([[1, 2], [3, 4]])
print(linalg.inv(a))

b = ulab.array([[1, 2, 3], [4, 5, 6], [7, 8, 7]])
print(linalg.inv(b))

c = ulab.array([[1, 2, 0, 0], [0, 6, 7, 0], [0, 0, 8, 9], [0, 0, 15, 13]])
print(linalg.inv(c))

print(linalg.det(a))
print(linalg.det(b))
print(linalg.det(c))



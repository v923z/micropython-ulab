import ulab
from ulab import linalg

a = ulab.array([[1, 2], [2, 5]])
print(linalg.cholesky(a))

b = a = ulab.array([[25, 15, -5], [15, 18,  0], [-5,  0, 11]])
print(linalg.cholesky(b))

c = ulab.array([[18, 22,  54,  42], [22, 70,  86,  62], [54, 86, 174, 134], [42, 62, 134, 106]])
print(linalg.cholesky(c))





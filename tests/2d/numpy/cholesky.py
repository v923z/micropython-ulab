from ulab import numpy as np

import math

a = np.array([[1, 2], [2, 5]])
reference = np.array([[1.0, 0.0], [2.0, 1.0]])
result = np.linalg.cholesky(a)
for i in range(2):
    for j in range(2): 
        print(f'({i}, {j}): {math.isclose(result[i][j], reference[i][j], rel_tol=1E-6, abs_tol=1E-6)}')

b = np.array([[25, 15, -5], [15, 18,  0], [-5,  0, 11]])
reference = np.array([[5.0, 0.0, 0.0], [3.0, 3.0, 0.0], [-1.0, 1.0, 3.0]])
result = np.linalg.cholesky(b)
for i in range(3):
    for j in range(3): 
        print(f'({i}, {j}): {math.isclose(result[i][j], reference[i][j], rel_tol=1E-6, abs_tol=1E-6)}')

c = np.array([[18, 22,  54,  42], [22, 70,  86,  62], [54, 86, 174, 134], [42, 62, 134, 106]])
reference = np.array([[4.242640687119285, 0.0, 0.0, 0.0],
       [5.185449728701349, 6.565905201197403, 0.0, 0.0],
       [12.727922061357857, 3.0460384954008553, 1.6497422479090682, 0.0],
       [9.899494936611665, 1.6245538642137891, 1.8497110052313865, 1.3926212476455826]])
result = np.linalg.cholesky(c)
for i in range(4):
    for j in range(4): 
        print(f'({i}, {j}): {math.isclose(result[i][j], reference[i][j], rel_tol=1E-6, abs_tol=1E-6)}')

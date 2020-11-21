import math
try:
    import ulab as np
    use_ulab = True
except ImportError:
    import numpy as np
    use_ulab = False
    
p = [1, 1, 1, 0]
x = [0, 1, 2, 3, 4]
result = (np.polyval(p, x))
ref_result = np.array([0, 3, 14, 39, 84])
for i in range(len(x)):
    print(math.isclose(result[i], ref_result[i], rel_tol=1E-9, abs_tol=1E-9))

a = np.array(x)
result = (np.polyval(p, a))
ref_result = np.array([0, 3, 14, 39, 84])
for i in range(len(x)):
    print(math.isclose(result[i], ref_result[i], rel_tol=1E-9, abs_tol=1E-9))

x = np.array([0, 1, 2, 3, 4, 5, 6])
y = np.array([9, 4, 1, 0, 1, 4, 9])
result = (np.polyfit(x, y, 2))
ref_result = np.array([ 1., -6.,  9.])
for i in range(3):
    print(math.isclose(result[i], ref_result[i], rel_tol=1E-9, abs_tol=1E-9))

result = (np.polyfit(x, y, 3))
ref_result = np.array([0.0,  1.0, -6.0,  9.0])
for i in range(4):
    print(math.isclose(result[i], ref_result[i], rel_tol=1E-9, abs_tol=1E-9))

result = (np.polyfit(x, y, 4))
ref_result = np.array([0.0, 0.0, 1.0, -6.0,  9.0])
for i in range(5):
    print(math.isclose(result[i], ref_result[i], rel_tol=1E-9, abs_tol=1E-9))

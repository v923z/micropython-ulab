import math

try:
    from ulab import scipy, numpy as np
except ImportError:
    import scipy
    import numpy as np

x = np.array((1,2,3))
y = np.array((1,10,100,1000))
result = (scipy.signal.oaconvolve(x, y))
ref_result = np.array([1, 12, 123, 1230, 2300, 3000],dtype=np.float)
cmp_result = []
for p,q in zip(list(result), list(ref_result)):
    cmp_result.append(math.isclose(p, q, rel_tol=1e-06, abs_tol=5e-04))
print(cmp_result)

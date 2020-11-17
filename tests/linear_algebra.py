import math
try:
    import ulab as np
    use_ulab = True
except ImportError:
    import numpy as np
    from scipy import special
    use_ulab = False
    
    
if use_ulab:
    a = np.array([1, 2, 3, 4], dtype=np.int8)  ## Here is problem
    print(np.linalg.size(a, axis=None))
    print(np.linalg.size(a, axis=0))
    b = np.array([[1, 2], [3, 4]], dtype=np.int8)
    print(np.linalg.size(b, axis=None))
    print(np.linalg.size(b, axis=0))
    print(np.linalg.size(b, axis=1))
    a = np.array([[1., 2.], [3., 4.]])
    b = np.linalg.inv(a)
    print(np.linalg.dot(a, b))
    a = np.array([[1, 2, 3, 4], [4, 5, 6, 4], [7, 8.6, 9, 4], [3, 4, 5, 6]])
    b = np.linalg.inv(a)
    print(np.linalg.dot(a,b))
    a = np.array([1,2,3])
    b = np.array([4,5,6])
    print(np.linalg.dot(a.transpose(), b))
else:
    a = np.array([[1., 2.], [3., 4.]])
    b = np.linalg.inv(a)
    print(np.dot(a, b))
    a = np.array([[1, 2, 3, 4], [4, 5, 6, 4], [7, 8.6, 9, 4], [3, 4, 5, 6]])
    b = np.linalg.inv(a)
    print(np.dot(a,b))
    a = np.array([1,2,3])
    b = np.array([4,5,6])
    print(np.dot(a.transpose(), b))
    
a = np.array([[1, 2, 3, 4], [4, 5, 6, 4], [7, 8.6, 9, 4], [3, 4, 5, 6]])
print(np.linalg.det(a))    
w, v = np.linalg.eig(np.diag(np.array([1, 2, 3])))
print(w)
print(v)
a = np.array([[25, 15, -5], [15, 18,  0], [-5,  0, 11]])
print(np.linalg.cholesky(a))

a = np.array([1,2,3,4,5], dtype=np.float)
print(np.linalg.norm(a))
a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
result = (np.linalg.norm(a))  ## Here is a problem
ref_result = 16.881943016134134
print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

if use_ulab:
    print(np.linalg.trace(np.eye(3)))
else:
    print(np.trace(np.eye(3)))


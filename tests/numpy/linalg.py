import math
try:
    from ulab import numpy as np
    use_ulab = True
except ImportError:
    import numpy as np
    use_ulab = False
    
    
if use_ulab:
    a = np.array([1,2,3], dtype=np.int16)
    b = np.array([4,5,6], dtype=np.int16)
    ab = np.linalg.dot(a.transpose(), b)
    print(math.isclose(ab, 32.0, rel_tol=1E-9, abs_tol=1E-9))

    a = np.array([1,2,3], dtype=np.int16)
    b = np.array([4,5,6], dtype=np.float)
    ab = np.linalg.dot(a.transpose(), b)
    print(math.isclose(ab, 32.0, rel_tol=1E-9, abs_tol=1E-9))

    a = np.array([[1., 2.], [3., 4.]])
    b = np.linalg.inv(a)
    ab = np.linalg.dot(a, b)
    m,n = ab.shape()
    for i in range(m):
        for j in range(n):
            if i == j:
                print(math.isclose(ab[i][j], 1.0, rel_tol=1E-9, abs_tol=1E-9))
            else:
                print(math.isclose(ab[i][j], 0.0, rel_tol=1E-9, abs_tol=1E-9))

    a = np.array([[1, 2, 3, 4], [4, 5, 6, 4], [7, 8.6, 9, 4], [3, 4, 5, 6]])
    b = np.linalg.inv(a)
    ab = np.linalg.dot(a, b)
    m,n = ab.shape()
    for i in range(m):
        for j in range(n):
            if i == j:
                print(math.isclose(ab[i][j], 1.0, rel_tol=1E-9, abs_tol=1E-9))
            else:
                print(math.isclose(ab[i][j], 0.0, rel_tol=1E-9, abs_tol=1E-9))
else:
    a = np.array([1,2,3], dtype=np.int16)
    b = np.array([4,5,6], dtype=np.int16)
    ab = np.dot(a.transpose(), b)
    print(math.isclose(ab, 32.0, rel_tol=1E-9, abs_tol=1E-9))

    a = np.array([1,2,3], dtype=np.int16)
    b = np.array([4,5,6], dtype=np.float)
    ab = np.dot(a.transpose(), b)
    print(math.isclose(ab, 32.0, rel_tol=1E-9, abs_tol=1E-9))
    
    a = np.array([[1., 2.], [3., 4.]])
    b = np.linalg.inv(a)
    ab = np.dot(a, b)
    m,n = ab.shape
    for i in range(m):
        for j in range(n):
            if i == j:
                print(math.isclose(ab[i][j], 1.0, rel_tol=1E-9, abs_tol=1E-9))
            else:
                print(math.isclose(ab[i][j], 0.0, rel_tol=1E-9, abs_tol=1E-9))

    a = np.array([[1, 2, 3, 4], [4, 5, 6, 4], [7, 8.6, 9, 4], [3, 4, 5, 6]])
    b = np.linalg.inv(a)
    ab = np.dot(a, b)
    m,n = ab.shape
    for i in range(m):
        for j in range(n):
            if i == j:
                print(math.isclose(ab[i][j], 1.0, rel_tol=1E-9, abs_tol=1E-9))
            else:
                print(math.isclose(ab[i][j], 0.0, rel_tol=1E-9, abs_tol=1E-9))
    
a = np.array([[1, 2, 3, 4], [4, 5, 6, 4], [7, 8.6, 9, 4], [3, 4, 5, 6]])
result = (np.linalg.det(a))    
ref_result = 7.199999999999995
print(math.isclose(result, ref_result, rel_tol=1E-9, abs_tol=1E-9))

a = np.array([1, 2, 3])
w, v = np.linalg.eig(np.diag(a))
for i in range(3):
    print(math.isclose(w[i], a[i], rel_tol=1E-9, abs_tol=1E-9))
for i in range(3):
    for j in range(3):
        if i == j:
            print(math.isclose(v[i][j], 1.0, rel_tol=1E-9, abs_tol=1E-9))
        else:
            print(math.isclose(v[i][j], 0.0, rel_tol=1E-9, abs_tol=1E-9))


a = np.array([[25, 15, -5], [15, 18,  0], [-5,  0, 11]])
result = (np.linalg.cholesky(a))
ref_result = np.array([[5.,  0.,  0.], [ 3.,  3.,  0.], [-1.,  1.,  3.]])
for i in range(3):
    for j in range(3):
        print(math.isclose(result[i][j], ref_result[i][j], rel_tol=1E-9, abs_tol=1E-9))

a = np.array([1,2,3,4,5], dtype=np.float)
result = (np.linalg.norm(a))
ref_result = 7.416198487095663
print(math.isclose(result, ref_result, rel_tol=1E-9, abs_tol=1E-9))

a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
result = (np.linalg.norm(a))  ## Here is a problem
ref_result = 16.881943016134134
print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

if use_ulab:
    print(np.linalg.trace(np.eye(3)))
else:
    print(np.trace(np.eye(3)))


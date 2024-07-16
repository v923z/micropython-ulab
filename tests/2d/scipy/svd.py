from ulab import scipy as sp
from ulab import numpy as np

tol = 1e-8

A = np.zeros((3, 3))
U, S, Vh = sp.linalg.svd(A)
# print(np.dot(U * S, Vh))
B = np.dot(np.dot(U, S), Vh)
print("Zero Matrix S:", np.max(abs(S)) < tol)
print("Zero Matrix Compare:", np.max(abs(A - B)) < tol)
print()

A = np.eye(4)
U, S, Vh = sp.linalg.svd(A)
# print(np.dot(U * S, Vh))
print("Identity Matrix S:", np.max(abs(A - S)) < tol)
B = np.dot(np.dot(U, S), Vh)
print("Identity Matrix Compare:", np.max(abs(A - B)) < tol)
print()

A = np.diag([1., 2., 3., 4.])
S_exp = np.diag([4., 3., 2., 1.])
U, S, Vh = sp.linalg.svd(A)
# B = np.dot(U * S, Vh)
B = np.dot(np.dot(U, S), Vh)
print("Diagonal Matrix S:", np.max(abs(A - S)) < tol)
print("Diagonal Matrix Compare", np.max(abs(A - B)) < tol)
print()

A = np.array([
    [0., 2., 0., 0.],
    [-1., 0., 0., 0.],
    [0., 0., -3., 0.],
    [0., 0., 0., -4.],
])
U, S, Vh = sp.linalg.svd(A)
# B = np.dot(U * S, Vh)
B = np.dot(np.dot(U, S), Vh)
print("Simultaneous 4D Rotation Compare:", np.max(abs(A - B)) < tol)
print()

A = np.array([
    [0., -1.],
    [2., 0.],
    [0., 0.]
])
U, S, Vh = sp.linalg.svd(A)
# B = np.dot(U * S, Vh)
B = np.dot(np.dot(U, S), Vh)
print("3x2 Compare:", np.max(abs(A - B)) < tol)
print()

A = np.array([
    [0., -1., 0.],
    [2., 0., 0.],
])
U, S, Vh = sp.linalg.svd(A)
# print(np.dot(U * S, Vh))
B = np.dot(np.dot(U, S), Vh)
print("2x3 Compare:", np.max(abs(A - B)) < tol)
print()

A = np.array([
    [4., 0.],
    [3., -5.],
])
U, S, Vh = sp.linalg.svd(A)
# B = np.dot(U * S, Vh)
B = np.dot(np.dot(U, S), Vh)
print("2x2 Compare", np.max(abs(A - B)) < tol)
print()

rng = np.random.Generator(123456)
A = rng.random((10, 10))
U, S, Vh = sp.linalg.svd(A)
# print(np.dot(U * S, Vh))
B = np.dot(np.dot(U, S), Vh)
print("10x10 Random Compare:", np.max(abs(A - B)) < tol)
print()

A = rng.random((10, 15))
U, S, Vh = sp.linalg.svd(A)
# print(np.dot(U * S, Vh))
B = np.dot(np.dot(U, S), Vh)
print("10x15 Random Compare:", np.max(abs(A - B)) < tol)
print()


A = rng.random((15, 10))
U, S, Vh = sp.linalg.svd(A)
# print(np.dot(U * S, Vh))
B = np.dot(np.dot(U, S), Vh)
print("15x10 Random Compare:", np.max(abs(A - B)) < tol)
print()

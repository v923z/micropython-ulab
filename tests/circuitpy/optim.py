import ulab as np

def f(x):
    return x**2 - 2.0

ref_result = 1.41421826342255
result = (np.approx.newton(f, 3.0, tol=0.001, rtol=0.01))
# since isclose is missing on circuitpython, we simply compare the results in the pedestrian way
print(abs(result - ref_result) < 1e-3)

result = (np.approx.newton(f, 3.0, tol=0.001, rtol=0.01, maxiter=100))
print(abs(result - ref_result) < 1e-3)

ref_result = 1.4142135623715149
result = (np.approx.bisect(f, 1.0, 3.0))
print(abs(result - ref_result) < 1e-3)

ref_result = -7.105427357601002e-15
result = np.approx.fmin(f, 3.0, fatol=1e-15)
print(abs(result - ref_result) < 1e-6)

ref_result = -7.105427357601002e-15
result = np.approx.fmin(f, 3.0, xatol=1e-8, fatol=1e-15, maxiter=500)
print(abs(result - ref_result) < 1e-6)

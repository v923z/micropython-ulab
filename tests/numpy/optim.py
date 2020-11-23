import math
try:
    import scipy.optimize as optimize
except:
    try:
        import ulab.approx as optimize
    except:
        import ulab as optimize

try:
    from math import isclose
except:
    def isclose(a, b, *, rel_tol=1e-9, abs_tol=0):
        print("isclose", a, b);
        return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)

def f(x):
    return x**2 - 2.0

ref_result = 1.41421826342255
result = optimize.newton(f, 3., tol=0.001, rtol=0.01)
print(ref_result, result)
print(isclose(result, ref_result, rel_tol=1E-9, abs_tol=1E-9))
result = optimize.newton(f, 3., tol=0.001, rtol=0.01, maxiter=100)
print(isclose(result, ref_result, rel_tol=1E-9, abs_tol=1E-9))

ref_result = 1.4142135623715149
result = optimize.bisect(f, 1.0, 3.0)
print(isclose(result, ref_result, rel_tol=1E-9, abs_tol=1E-9))

#ref_result = -7.105427357601002e-15
#result = optimize.fmin(f, 3.0, disp=0)
#print(isclose(result[0], ref_result, rel_tol=1E-9, abs_tol=1E-9))
#result = optimize.fmin(f, 3.0, xtol=0.0001, ftol=0.0001, disp=0)
#print(isclose(result[0], ref_result, rel_tol=1E-9, abs_tol=1E-9))



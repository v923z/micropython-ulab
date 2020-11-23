import math
try:
    import ulab as np
    use_ulab = True
except ImportError:
    from scipy import optimize
    use_ulab = False

def f(x):
    return x**2 - 2.0

if use_ulab:
    ref_result = 1.41421826342255
    result = (np.newton(f, 3.0, tol=0.001, rtol=0.01))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

    result = (np.newton(f, 3.0, tol=0.001, rtol=0.01, maxiter=100))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

    ref_result = 1.4142135623715149
    result = (np.bisect(f, 1.0, 3.0))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

    ref_result = -7.105427357601002e-15
    result = np.fmin(f, 3.0)
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

    ref_result = -7.105427357601002e-15
    result = np.fmin(f, 3.0, xatol=0.0001, maxiter=500)
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))
else:
    ref_result = 1.41421826342255
    result = optimize.newton(f, 3., tol=0.001, rtol=0.01)
    print(math.isclose(result, ref_result, rel_tol=1E-9, abs_tol=1E-9))
    result = optimize.newton(f, 3., tol=0.001, rtol=0.01, maxiter=100)
    print(math.isclose(result, ref_result, rel_tol=1E-9, abs_tol=1E-9))

    ref_result = 1.4142135623715149
    result = optimize.bisect(f, 1.0, 3.0)
    print(math.isclose(result, ref_result, rel_tol=1E-9, abs_tol=1E-9))

    ref_result = -7.105427357601002e-15
    result = optimize.fmin(f, 3.0, disp=0)
    print(math.isclose(result[0], ref_result, rel_tol=1E-9, abs_tol=1E-9))
    result = optimize.fmin(f, 3.0, xtol=0.0001, ftol=0.0001, disp=0)
    print(math.isclose(result[0], ref_result, rel_tol=1E-9, abs_tol=1E-9))



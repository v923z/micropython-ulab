import math
try:
    import ulab as np
    use_ulab = True
except ImportError:
    import numpy as np
    use_ulab = False
    
x = np.array([1, 2, 3, 4, 5])
xp = np.array([1, 2, 3, 4])
fp = np.array([1, 2, 3, 4])    
print(np.interp(x, xp, fp))  ## Here is problem
print(np.interp(x, xp, fp, left=0.0))
print(np.interp(x, xp, fp, right=10.0))
print(np.interp(x, xp, fp, left=0.0, right=10.0))

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

    ref_result = -7.10542736e-15
    result = (np.fmin(f, 3.0))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

    result = (np.fmin(f, 3.0, xatol=0.0001, maxiter=500))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

    x = np.linspace(0, 9, num=10)
    y = x*x
    print(np.trapz(y))
    print(np.trapz(y, x=x))
else:
    from scipy import optimize
    print(optimize.newton(f, 3., tol=0.001, rtol=0.01))
    print(optimize.newton(f, 3., tol=0.001, rtol=0.01, maxiter=100))
    print(optimize.bisect(f, 1.0, 3.0))
    print(optimize.fmin(f, 3.0))
    print(optimize.fmin(f, 3.0, xtol=0.0001, ftol=0.0001))

    x = np.linspace(0, 9, num=10)
    y = x*x
    print(np.trapz(y))
    print(np.trapz(y, x=x))  ## Here is problem

try:
    from ulab import scipy as spy
    from ulab import numpy as np
except ImportError:
    import scipy

import math

f = lambda x: x * np.sin(x) * np.exp(x)
a = 1
b = 2

(res, err) = spy.integrate.tanhsinh(f, a, b)
print('testing tanhsinh')
print(math.isclose(res, 7.11263821415851, rel_tol=1E-6, abs_tol=1E-6))
print()

print('testing romberg')
res = spy.integrate.romberg(f, a, b)
print(math.isclose(res, 7.112638214158507, rel_tol=1E-6, abs_tol=1E-6))
print()

print('testing simpson')
res = spy.integrate.simpson(f, a, b)
print(math.isclose(res, 7.112638214158507, rel_tol=1E-6, abs_tol=1E-6))
print()

print('testing quad')
(res, err) = spy.integrate.quad(f, a, b)
print(math.isclose(res, 7.112638214158507, rel_tol=1E-6, abs_tol=1E-6))

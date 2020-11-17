import math
try:
    import ulab as np
    use_ulab = True
except ImportError:
    import numpy as np
    from scipy import special
    use_ulab = False
    
result = (np.sin(np.pi/2))
ref_result = 1.0
print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

result = (np.cos(np.pi/2))
ref_result = 0.0
print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

result = (np.tan(np.pi/2))
ref_result = 1.633123935319537e+16
print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

result = (np.sinh(np.pi/2))
ref_result = 2.3012989023072947
print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

result = (np.cosh(np.pi/2))
ref_result = 2.5091784786580567
print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

result = (np.tanh(np.pi/2))
ref_result = 0.9171523356672744
print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

if use_ulab:
    ref_result = np.pi/2
    result = (np.asin(np.sin(np.pi/2)))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))
    
    result = (np.acos(np.cos(np.pi/2)))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

    result = (np.atan(np.tan(np.pi/2)))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

    result = (np.cosh(np.acosh(np.pi/2)))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

    result = (np.sinh(np.asinh(np.pi/2)))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))
else:
    ref_result = np.pi/2
    result = (np.arcsin(np.sin(np.pi/2)))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

    result = (np.arccos(np.cos(np.pi/2)))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

    result = (np.arctan(np.tan(np.pi/2)))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

    result = (np.cosh(np.arccosh(np.pi/2)))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))

    result = np.sinh((np.arcsinh(np.pi/2)))
    print(math.isclose(result, ref_result, rel_tol=1E-6, abs_tol=1E-6))
    
print(np.degrees(np.pi))
print(np.radians(np.degrees(np.pi)))
print(np.floor(np.pi))
print(np.ceil(np.pi))
print(np.sqrt(np.pi))

print(np.exp(1))
print(np.log(np.exp(1)))

if use_ulab:
    print(np.log2(2**1))
else:
    print(np.log2(2**1))

print(np.log10(10**1))
print(np.exp(1) - np.expm1(1))

x = np.array([-1, +1, +1, -1])
y = np.array([-1, -1, +1, +1])
print(np.arctan2(y, x) * 180 / np.pi)

x = np.linspace(-2*np.pi, 2*np.pi, 5)
print(np.sin(x))
print(np.cos(x))
print(np.tan(x))
print(np.sinh(x))
print(np.cosh(x))
print(np.tanh(x))

if use_ulab:
    print(np.erf(np.linspace(-3, 3, num=5)))
    print(np.erfc(np.linspace(-3, 3, num=5)))
    print(np.gamma(np.array([0, 0.5, 1, 5])))
    print(np.lgamma([0, -1, -2, -3, -4]))
else:
    print(special.erf(np.linspace(-3, 3, num=5)))
    print(special.erfc(np.linspace(-3, 3, num=5)))
    print(special.gamma(np.array([0, 0.5, 1, 5])))
    print(special.gammaln([0, -1, -2, -3, -4]))


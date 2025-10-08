import math

try:
    from ulab import numpy as np
    from ulab import scipy as spy
except ImportError:
    import numpy as np
    import scipy as spy
    
def test_function(func, reference, expression):
     result = expression
     print(f'testing {func}')
     print(math.isclose(result, reference, rel_tol=1E-6, abs_tol=1E-6))
     print()

def test_function_array(func, reference, expression):
    print(f'testing {func}')
    results = []
    for i in range(len(references)):
        results.append(math.isclose(references[i], expression[i], rel_tol=1E-6, abs_tol=1E-6))
    print(results)
    print()


test_function('sin', 1.0, np.sin(np.pi/2))

test_function('cos', 0.0, (np.cos(np.pi/2)))

test_function('tan', 1.633123935319537e+16, np.tan(np.pi/2))

test_function('sinh', 2.3012989023072947, np.sinh(np.pi/2))

test_function('cosh:', 2.5091784786580567, np.cosh(np.pi/2))

test_function('tanh', 0.9171523356672744, np.tanh(np.pi/2))

test_function('asin', np.pi/2, np.asin(np.sin(np.pi/2)))

test_function('acos', np.pi/2, np.acos(np.cos(np.pi/2)))

test_function('atan', np.pi/2, np.atan(np.tan(np.pi/2)))

test_function('acosh', np.pi/2, np.cosh(np.acosh(np.pi/2)))

test_function('asinh', np.pi/2, np.sinh(np.asinh(np.pi/2)))

test_function('degrees', 180.0, np.degrees(np.pi))

test_function('radians', np.pi, np.radians(180.0))

test_function('floor', 3.0, np.floor(np.pi))

test_function('ceil', 4.0, np.ceil(np.pi))

test_function('sqrt', 1.7724538509055159, np.sqrt(np.pi))

test_function('exp', 2.718281828459045, np.exp(1))

test_function('log', 1.0, np.log(2.718281828459045))

test_function('log2', 1.0, np.log2(2))

test_function('log10', 1.0, np.log10(10.0))

test_function('expm1', 1.0, np.exp(1) - np.expm1(1))

x = [-1, +1, +1, -1]
y = [-1, -1, +1, +1]
reference = [-135.0, -45.0, 45.0, 135.0]
for i in range(4):
     test_function('arctan2', reference[i], np.arctan2(y[i], x[i]) * 180 / np.pi)

x = np.linspace(-2*np.pi, 2*np.pi, 5)

references = [2.4492936e-16, -1.2246468e-16,  0.0000000e+00,  1.2246468e-16, -2.4492936e-16]
test_function_array('sin', references, np.sin(x))

references = [1., -1.,  1., -1.,  1.]
test_function_array('cos', references, np.cos(x))

references = [0.0, 0.0, 0.0, 0.0, 0.0]
test_function_array('tan', references, np.tan(x))

references = [-267.74489404, -11.54873936, 0., 11.54873936, 267.74489404]
test_function_array('sinh', references, np.sinh(x))

references = [267.74676148,  11.59195328, 1.0, 11.59195328, 267.74676148]
test_function_array('cosh', references, np.cosh(x))

references = [-0.9999930253396107, -0.99627207622075, 0.0, 0.99627207622075, 0.9999930253396107]
test_function_array('tanh', references, np.tanh(x))

references = [0.03935584386392389, -0.04359862862918773, 1.0, -0.04359862862918773, 0.03935584386392389]
test_function_array('sinc', references, np.sinc(x))

references = [-0.9999779095030014, -0.9661051464753108, 0.0, 0.9661051464753108, 0.9999779095030014]
test_function_array('erf', references, spy.special.erf(np.linspace(-3, 3, num=5)))

references = [1.99997791e+00, 1.96610515e+00, 1.00000000e+00, 3.38948535e-02, 2.20904970e-05]
test_function_array('erfc', references, spy.special.erfc(np.linspace(-3, 3, num=5)))
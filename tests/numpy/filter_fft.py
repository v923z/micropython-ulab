import math
try:
    import ulab as np
    use_ulab = True
except ImportError:
    import numpy as np
    use_ulab = False

np.set_printoptions(threshold=8, edgeitems=4)
x = np.array((1,2,3))
y = np.array((1,10,100,1000))

result = (np.convolve(x, y))
ref_result = [1, 12, 123, 1230, 2300, 3000]
cmp_result = []
for p,q in zip(list(result), ref_result):
    cmp_result.append(math.isclose(p, q, rel_tol=1e-06, abs_tol=1e-06))
print(cmp_result)

x = np.linspace(-np.pi, np.pi, num=8)
y = np.sin(x)
if use_ulab:
    a, b = np.fft.fft(y)
    c, d = np.fft.ifft(a, b)
    # c should be equal to y
    cmp_result = []
    for p,q in zip(list(y), list(c)):
        cmp_result.append(math.isclose(p, q, rel_tol=1e-09, abs_tol=1e-09))
    print(cmp_result)

    z = np.zeros(len(x))
    a, b = np.fft.fft(y, z)
    c, d = np.fft.ifft(a, b)
    # c should be equal to y
    cmp_result = []
    for p,q in zip(list(y), list(c)):
        cmp_result.append(math.isclose(p, q, rel_tol=1e-09, abs_tol=1e-09))
    print(cmp_result)

    g = np.fft.spectrogram(y)
    h = np.sqrt(a*a+b*b)
    cmp_result = []
    for p,q in zip(list(g), list(h)):
        cmp_result.append(math.isclose(p, q, rel_tol=1e-09, abs_tol=1e-09))
    print(cmp_result)
else:
    a = np.fft.fft(y)
    c = np.fft.ifft(a)
    # c should be equal to y
    cmp_result = []
    for p,q in zip(list(y), list(c.real)):
        cmp_result.append(math.isclose(p, q, rel_tol=1e-09, abs_tol=1e-09))
    print(cmp_result) 


  

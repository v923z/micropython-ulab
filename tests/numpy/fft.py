import math
try:
    import numpy as np
except ImportError:
    import ulab as np

try:
    spectrogram = np.spectrogram
except:
    def spectrogram(seq):
        a, b = np.fft.fft(seq)
        return np.hypot(a, b)

try:
    from math import isclose
except:
    def isclose(a, b, *, rel_tol=1e-9, abs_tol=0):
        return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)

x = np.linspace(-np.pi, np.pi, num=8)
y = np.sin(x)
a, b = np.fft.fft(y)
c, d = np.fft.ifft(a, b)
# c should be equal to y
cmp_result = []
for p,q in zip(list(y), list(c)):
    cmp_result.append(isclose(p, q, rel_tol=1e-09, abs_tol=1e-09))
print(cmp_result)

z = np.zeros(len(x))
a, b = np.fft.fft(y, z)
c, d = np.fft.ifft(a, b)
# c should be equal to y
cmp_result = []
for p,q in zip(list(y), list(c)):
    cmp_result.append(isclose(p, q, rel_tol=1e-09, abs_tol=1e-09))
print(cmp_result)

g = np.fft.spectrogram(y)
h = np.sqrt(a*a+b*b)
cmp_result = []
for p,q in zip(list(g), list(h)):
    cmp_result.append(isclose(p, q, rel_tol=1e-09, abs_tol=1e-09))
print(cmp_result)

import math

from ulab import numpy
from ulab import numpy as np
from ulab import scipy as spy

def to_tuple(a):
    result = []
    for x in a:
       result.append([x.real, x.imag])
    return result

def to_complex(a):
    result = np.array([], dtype=np.complex)
    for x in a:
       t = np.array([complex(x[0] + x[1] * 1j)], dtype=np.complex)
       result = np.concatenate((result, t), axis=0)
    return result

roots = np.array([0.99847722-0.01125341j, 0.99552224-0.01283305j, 0.99552224+0.01283305j, 0.99847722+0.01125341j,
0.99698268+0.02147022j, 0.9940139500000001+0.01703982j, 0.9940139500000001-0.01703982j, 0.99698268-0.02147022j], dtype=np.complex)

print(roots)
r = to_tuple(roots)
s = sorted(r,key=lambda x:(x[0], x[1]))
f = to_complex(s)

print(f)
#print()
#print(sorted(r,key=lambda x:(x[0], x[1])))

#print(np.sort_complex(np.array([-0.9800000000000001+1.0j, 0.9800000000000001-1.0j, 0.99+1.0j, 0.99-1.0j], dtype=np.complex)))
# this test is meaningful only, when the firmware supports complex arrays

import math
import cmath

try:
    from ulab import numpy as np
except:
    import numpy as np

def check(label, result, reference, dtype):
    # compare each element of result against reference, as complex numbers, using isclose
    values = result.flatten().tolist()
    close = len(values) == len(reference) and all(
        math.isclose(complex(v).real, complex(r).real, rel_tol=1E-6, abs_tol=1E-6) and
        math.isclose(complex(v).imag, complex(r).imag, rel_tol=1E-6, abs_tol=1E-6)
        for v, r in zip(values, reference))
    print(f'{label}: shape {result.shape}')
    if result.dtype == dtype:
        print('dtypes match')
    else:
        print(f'dtypes differ: {result.dtype} vs {dtype}')
    if close:
        print('values match reference')
    else:
        print(f'values differ: {values} vs {reference}')

dtypes = (np.uint8, np.int8, np.uint16, np.int16, np.float, np.complex)

for dtype in dtypes:
    a = np.array(range(4), dtype=dtype)
    b = a.reshape((2, 2))
    outtype = np.float if dtype is not np.complex else np.complex
    print('\narray:\n', a)
    check('exponential', np.exp(a), [math.exp(x) for x in range(4)], outtype)
    print('\narray:\n', b)
    check('exponential', np.exp(b), [math.exp(x) for x in range(4)], outtype)

values = [0, 1j, 2+2j, 3-3j]
a = np.array(values, dtype=np.complex)
b = np.array(values * 2, dtype=np.complex).reshape((2, 4))
c = np.array(values * 2, dtype=np.complex).reshape((2, 2, 2))
d = np.array(values * 4, dtype=np.complex).reshape((2, 2, 2, 2))

for m in (a, b, c, d):
    print('\n\narray:\n', m)
    check('exponential', np.exp(m), [cmath.exp(x) for x in values] * (m.size // 4), np.complex)

try:
    from ulab import numpy as np
except:
    import numpy as np

import math

dtypes = (np.uint8, np.int8, np.uint16, np.int16)

for dtype in dtypes:
    print(np.logspace(0, 10, num=5, endpoint=False, dtype=dtype))
    print(np.logspace(0, 10, num=5, endpoint=True, dtype=dtype))


reference = [1.0, 100.0, 10000.0, 1000000.0, 100000000.0]
result = np.logspace(0, 10, num=5, endpoint=False, dtype=np.float)
print([math.isclose(result[i], reference[i], rel_tol=1E-6, abs_tol=1E-6) for i in range(len(reference))])


reference = [1.0, 316.22776601683796, 100000.00000000001, 31622776.6016838, 10000000000.000002]
result = np.logspace(0, 10, num=5, endpoint=True, dtype=np.float)
print([math.isclose(result[i], reference[i], rel_tol=1E-6, abs_tol=1E-6) for i in range(len(reference))])

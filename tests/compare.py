import ulab
from ulab import compare

a = ulab.array([1, 2, 3, 4, 5], dtype=ulab.uint8)
b = ulab.array([5, 4, 3, 2, 1], dtype=ulab.float)
print(compare.minimum(a, b))
print(compare.maximum(a, b))
print(compare.maximum(1, 5.5))

a = ulab.array(range(9), dtype=ulab.uint8)
print(compare.clip(a, 3, 7))

b = 3 * ulab.ones(len(a), dtype=ulab.float)
print(compare.clip(a, b, 7))

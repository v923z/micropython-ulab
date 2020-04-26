import ulab
from ulab import poly

x = ulab.array([1, 2, 3, 4, 5])
xp = ulab.array([1, 2, 3, 4])
fp = ulab.array([1, 2, 3, 4])
x = x - 0.5
print(poly.interp(x, xp, fp))
print(poly.interp(x, xp, fp, left=0.0))
print(poly.interp(x, xp, fp, right=10.0))

x = ulab.array([1, 2, 3, 4, 5])
xp = ulab.array([1, 2, 3, 4])
fp = ulab.array([1, 2, 3, 5])
x = x - 0.2
print(poly.interp(x, xp, fp))
print(poly.interp(x, xp, fp, left=0.0))
print(poly.interp(x, xp, fp, right=10.0))

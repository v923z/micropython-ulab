import ulab
from ulab import poly
from ulab import vector

# polynom evaluation
x = ulab.linspace(0, 10, num=9)
p = [1, 2, 3]
y = poly.polyval(p, x)
print(y)

# linear fit
x = ulab.linspace(-5, 5, num=11)
y = x + vector.sin(x)
p = poly.polyfit(x, y, 1)
print(p)

# quadratic fit
x = ulab.linspace(-5, 5, num=11)
y = x*x + vector.sin(x)*3.0
p = poly.polyfit(x, y, 2)
print(p)

# cubic fit
x = ulab.linspace(-5, 5, num=11)
y = x*x*x + vector.sin(x)*10.0
p = poly.polyfit(x, y, 3)
print(p)

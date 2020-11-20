import ulab as np

# polynom evaluation
x = np.linspace(0, 10, num=9)
p = [1, 2, 3]
y = np.poly.polyval(p, x)
print(y)

# linear fit
x = np.linspace(-5, 5, num=11)
y = x + np.vector.sin(x)
p = np.poly.polyfit(x, y, 1)
print(p)

# quadratic fit
x = np.linspace(-5, 5, num=11)
y = x*x + np.vector.sin(x)*3.0
p = np.poly.polyfit(x, y, 2)
print(p)

# cubic fit
x = np.linspace(-5, 5, num=11)
y = x*x*x + np.vector.sin(x)*10.0
p = np.poly.polyfit(x, y, 3)
print(p)

try:
    import ulab as np
    use_ulab = True
except ImportError:
    import numpy as np
    use_ulab = False
    
p = [1, 1, 1, 0]
x = [0, 1, 2, 3, 4]
print(np.polyval(p, x))
a = np.array(x)
print(np.polyval(p, a))

x = np.array([0, 1, 2, 3, 4, 5, 6])
y = np.array([9, 4, 1, 0, 1, 4, 9])
print(np.polyfit(x, y, 2))
print(np.polyfit(x, y, 3))
print(np.polyfit(x, y, 4))

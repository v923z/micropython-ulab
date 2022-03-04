try:
    from ulab import numpy as np
except:
    import numpy as np

print(list(np.argsort(np.array([]), axis=0)))
print(list(np.argsort(np.array([4,1,3,2]), axis=0)))

from ulab import numpy as np

print(np.ones(3))
print(np.ones((2,3)))
print(np.zeros(3))
print(np.zeros((2,3)))
print(np.eye(3))
print(np.ones(1, dtype=np.int8))
print(np.ones(2, dtype=np.uint8))
print(np.ones(3, dtype=np.int16))
print(np.ones(4, dtype=np.uint16))
print(np.ones(5, dtype=np.float))
print(np.linspace(0, 1, 9))

# test ndarray constructor
dtypes = [np.int8, np.uint8, np.int16, np.uint16, np.float, np.bool]
shapes = [[3, 2], (3,), 3]
for dtype in dtypes:
    for shape in shapes:
        x = np.ndarray(shape, dtype=dtype)
        print(x, isinstance(x, np.ndarray),
              # bools end up as uint8s (for firmware size I guess?) 
              x.dtype() == (dtype if dtype != np.bool else np.uint8) )
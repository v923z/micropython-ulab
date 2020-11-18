import ulab as np
print("Array creation using RANGE:")
print(np.array(range(2**8-5, 2**8), dtype=np.uint8))
print(np.array(range(2**8-5, 2**8), dtype=np.int8))
print(np.array(range(2**16-5, 2**16), dtype=np.uint16))
print(np.array(range(2**16-5, 2**16), dtype=np.int16))
print(np.array(range(2**56-3, 2**56), dtype=np.float))

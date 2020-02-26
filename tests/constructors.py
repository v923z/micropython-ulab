from ulab import linalg
import ulab
print(linalg.ones(3))
print(linalg.ones((2,3)))
print(linalg.zeros(3))
print(linalg.zeros((2,3)))
print(linalg.eye(3))
print(linalg.ones(1, dtype=ulab.int8))
print(linalg.ones(2, dtype=ulab.uint8))
print(linalg.ones(3, dtype=ulab.int16))
print(linalg.ones(4, dtype=ulab.uint16))
print(linalg.ones(5, dtype=ulab.float))

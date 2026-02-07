try:
    from ulab import numpy as np
except ImportError:
    import numpy as np

# --- 3D Cases ---
a = np.array([1, 2], dtype=np.uint8)
b = np.array([5], dtype=np.uint8)
c = np.array([9], dtype=np.uint8)

print("--- 3D xy ---")
# Shape: (1, 2, 1) -> First two dims swapped
ma, mb, mc = np.meshgrid(a, b, c, indexing='xy')
print(ma)
print(mb)
print(mc)

print("\n--- 3D ij ---")
# Shape: (2, 1, 1)
ma, mb, mc = np.meshgrid(a, b, c, indexing='ij')
print(ma)
print(mb)
print(mc)

print("\n=== 3D Strides ===")
# a: [0, 10, 20] -> [::2] -> [0, 20] (len 2)
a = np.array([0, 10, 20], dtype=np.uint8)[::2]
# b: [5] (len 1)
b = np.array([5], dtype=np.uint8)
# c: [1, 2, 3] -> [::-1] -> [3, 2, 1] (len 3)
c = np.array([1, 2, 3], dtype=np.uint8)[::-1]

print("--- XY ---")
# Shape: (1, 2, 3) -> [b, a, c]
m1, m2, m3 = np.meshgrid(a, b, c, indexing='xy')
print(m1)
print(m2)
print(m3)

print("--- IJ ---")
# Shape: (2, 1, 3) -> [a, b, c]
m1, m2, m3 = np.meshgrid(a, b, c, indexing='ij')
print(m1)
print(m2)
print(m3)

try:
    from ulab import numpy as np
except ImportError:
    import numpy as np

# --- 4D Cases ---
# Using small arrays to verify hypercube generation
x = np.array([10], dtype=np.uint8)      # Len 1
y = np.array([20, 30], dtype=np.uint8)  # Len 2
z = np.array([40], dtype=np.uint8)      # Len 1
w = np.array([50], dtype=np.uint8)      # Len 1

print("--- 4D xy ---")
# Expected Shape: (2, 1, 1, 1) -> Swaps x(len 1) and y(len 2)
mx, my, mz, mw = np.meshgrid(x, y, z, w, indexing='xy')
print(mx)
print(my)
print(mz)
print(mw)

print("\n--- 4D ij ---")
# Expected Shape: (1, 2, 1, 1)
mx, my, mz, mw = np.meshgrid(x, y, z, w, indexing='ij')
print(mx)
print(my)
print(mz)
print(mw)

print("\n=== 4D Strides ===")
# x: [100, 10] -> [1:] -> [10] (len 1)
x = np.array([100, 10], dtype=np.uint8)[1:]
# y: [0, 1, 2, 3] -> [::3] -> [0, 3] (len 2)
y = np.array([0, 1, 2, 3], dtype=np.uint8)[::3]
# z: [50] (len 1)
z = np.array([50], dtype=np.uint8)
# w: [8, 9] -> [::-1] -> [9, 8] (len 2)
w = np.array([8, 9], dtype=np.uint8)[::-1]

print("--- XY ---")
# Shape: (2, 1, 1, 2) -> [y, x, z, w]
m1, m2, m3, m4 = np.meshgrid(x, y, z, w, indexing='xy')
print(m1)
print(m2)
print(m3)
print(m4)

print("--- IJ ---")
# Shape: (1, 2, 1, 2) -> [x, y, z, w]
m1, m2, m3, m4 = np.meshgrid(x, y, z, w, indexing='ij')
print(m1)
print(m2)
print(m3)
print(m4)
try:
    from ulab import numpy as np
except:
    import numpy as np

# 1. Standard 2D Case - XY Indexing (Default)
# Expected Output: x repeated in rows, y repeated in cols. Shape (len(y), len(x))
print("--- 2D xy ---")
x = np.array([1, 2, 3], dtype=np.uint8)
y = np.array([10, 20], dtype=np.uint8)

xv, yv = np.meshgrid(x, y, indexing='xy')
print(xv)
print(yv)

# 2. Standard 2D Case - IJ Indexing
# Expected Output: x repeated in cols, y repeated in rows. Shape (len(x), len(y))
print("\n--- 2D ij ---")
xi, yi = np.meshgrid(x, y, indexing='ij')
print(xi)
print(yi)

print("\n=== 2D Strides ===")
# x: [0, 1, 2, 3] -> [::2] -> [0, 2] (len 2)
x = np.array([0, 1, 2, 3], dtype=np.uint8)[::2]
# y: [10, 20, 30] -> [::-1] -> [30, 20, 10] (len 3)
y = np.array([10, 20, 30], dtype=np.uint8)[::-1]

print("--- XY ---")
# Shape: (3, 2)
mx, my = np.meshgrid(x, y, indexing='xy')
print(mx)
print(my)

print("--- IJ ---")
# Shape: (2, 3)
mx, my = np.meshgrid(x, y, indexing='ij')
print(mx)
print(my)
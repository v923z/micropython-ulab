try:
   from ulab import numpy as np
except:
   import numpy as np

array = np.array(range(16)).reshape((4,4))
boolean_array = array < 3

# np.nonzero() returns a tuple of corresponding row and column 
# indices array for each element being true in boolean array
row_indices, column_indices = np.nonzero(boolean_array)

print("Given Array:\n", array)
print("\nBoolean Array:\n", boolean_array)
print("\nFollowing indices have non-zero values:")
for i in range(len(row_indices)):
	print("row", row_indices[i], "and column", column_indices[i])
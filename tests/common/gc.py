import ulab
import gc

data = ulab.ones(1000)[6:-6]
print(sum(data))
print(data)

gc.collect()

print(sum(data))
print(data)

try:
    from ulab import numpy as np
except ImportError:
    import numpy as np

rng = np.random.Generator(1234)

random_array = rng.random((1, 2))
print("random() shape:", random_array.shape)
try:
    from ulab import numpy as np
except ImportError:
    import numpy as np

rng = np.random.Generator(1234)

for generator in (rng.normal, rng.random, rng.uniform):
    random_array = generator(size=(1, 2))
    print("array shape:", random_array.shape)
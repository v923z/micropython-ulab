from ulab import numpy as np

def asarray(a, dtype=None):
    if isinstance(a,(np.ndarray)):
        return a
    try:
        if dtype is not None:
          a = np.array([a], dtype=dtype)
        else:
          a = np.array([a])
        return a
    except Exception as e:
        if "can't convert complex to float" in e.args or "'complex' object isn't iterable" in e.args:
          try:
             a = np.array([a], dtype=np.complex).flatten()
             return a
          except:
              pass
        raise ValueError('Could not cast %s to array' % (a))

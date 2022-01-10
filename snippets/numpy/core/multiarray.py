from ulab import numpy as np

def asarray(a, dtype=None):
    if isinstance(a,(np.ndarray)):
        return a
    try:
        a = np.array(a)
        return a
    except Exception as e:
        if "can't convert complex to float" in e.args:
          try:
             a = np.array(a, dtype=np.complex)
             return a
          except:
              pass
        raise ValueError('Could not cast %s to array' % (a))



import math
import sys
sys.path.append('.')

from snippets import numpy
from ulab import numpy as np

a = np.array([1, 2j, 3, 4j], dtype=np.complex)
print (numpy.isreal(a))
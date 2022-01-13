import math

from ulab import numpy
from ulab import numpy as np
from ulab import scipy as spy

def asarray(a, dtype=None):
    if isinstance(a,(np.ndarray)):
        return a
    return a

def atleast_1d(*arys):
    res = []
    for ary in arys:
        ary = asarray(ary)
        if not isinstance(ary,(np.ndarray)):
            ary = np.array([ary])
            result = ary.reshape((1,))
        else:
            result = ary
        res.append(result)
    if len(res) == 1:
        return res[0]
    else:
        return res

def poly(seq_of_zeros):
	seq_of_zeros = atleast_1d(seq_of_zeros)
	sh = seq_of_zeros.shape

	if len(sh) == 2 and sh[0] == sh[1] and sh[0] != 0:
		seq_of_zeros = eigvals(seq_of_zeros)
	elif len(sh) == 1:
		dt = seq_of_zeros.dtype
		# Let object arrays slip through, e.g. for arbitrary precision
		if dt != object:
			seq_of_zeros = seq_of_zeros #seq_of_zeros.astype(mintypecode(dt.char))
	else:
		raise ValueError("input must be 1d or non-empty square 2d array.")

	if len(seq_of_zeros) == 0:
		return 1.0
	dt = seq_of_zeros.dtype
	print(dt)
	a = np.ones((1,), dtype=dt)
	print(a)
	print(seq_of_zeros)
	for k in range(len(seq_of_zeros)):
		a = np.convolve(a, np.array([1, -seq_of_zeros[k]], dtype=dt))
	print(a)
	#if issubclass(a.dtype.type, NX.complexfloating):
		# if complex roots are all complex conjugates, the roots are real.
	roots = asarray(seq_of_zeros, complex)
	print('roots',roots)
	p = np.sort_complex(roots)
	print(p)
	q = np.conjugate(roots)

	s = np.sort_complex(q)
	print(s)
	#if NX.all(NX.sort(roots) == NX.sort(roots.conjugate())):
	a = a.real.copy()

	return a


p = np.array([-0.3826834323650898+0.9238795325112868j, -0.9238795325112868+0.3826834323650898j,
-0.9238795325112868-0.3826834323650898j, -0.3826834323650898-0.9238795325112868j], dtype=np.complex)

print(poly(p))
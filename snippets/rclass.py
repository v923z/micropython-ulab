from ulab import numpy as np
from typing import List, Tuple, Union  # upip.install("pycopy-typing")

ndarray = np.array
_DType = int
RClassKeyType = Union[slice, int, float]

# this is a stripped down version of RClass (used by np.r_[...etc])
# it doesn't include support for string arguments as the first index element
class RClass:

    def __getitem__(self, key: Union[RClassKeyType, Tuple[RClassKeyType, ...]]):

        if not isinstance(key, tuple):
            key = (key,)

        objs: List[ndarray] = []
        scalars: List[int] = []
        arraytypes: List[_DType] = []
        scalartypes: List[_DType] = []

        # these may get overridden in following loop
        axis = 0

        for idx, item in enumerate(key):
            scalar = False
            if isinstance(item, slice):
                step = item.step
                start = item.start
                stop = item.stop
                if start is None:
                    start = 0
                if step is None:
                    nstep = 1
                if isinstance(step, complex):
                    size = int(abs(step))
                    newobj = cast(ndarray, linspace(start, stop, num=size))
                else:
                    newobj = np.arange(start, stop, step)

            # if is number
            elif isinstance(item, (int, float)):
                newobj = np.array([item])
                scalars.append(len(objs))
                scalar = True
                scalartypes.append(newobj.dtype())

            else:
                raise Exception("index object %s of type %s is not supported by r_[]" % (
                    str(item), type(item)))

            objs.append(newobj)
            if not scalar and isinstance(newobj, ndarray):
                arraytypes.append(newobj.dtype())

        # Ensure that scalars won't up-cast unless warranted
        # TODO: ensure that this actually works for dtype coercion
        # likelihood is we're going to have to do some funky logic for this
        final_dtype = max(arraytypes + scalartypes)
        if final_dtype is not None:
            for idx in scalars:
                objs[idx] = np.array(objs[idx], dtype=final_dtype)

        res = np.concatenate(tuple(objs), axis=axis)

        return res

    # this seems weird - not sure what it's for
    def __len__(self):
        return 0


r_ = RClass()

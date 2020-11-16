:mod:`ulab.compare`
===================

.. py:module:: ulab.compare

.. autoapi-nested-parse::

   Comparison functions



.. function:: clip(x1: Union[ulab.array, float], x2: Union[ulab.array, float], x3: Union[ulab.array, float]) -> ulab.array

   Constrain the values from ``x1`` to be between ``x2`` and ``x3``.
   ``x2`` is assumed to be less than or equal to ``x3``.

   Arguments may be ulab arrays or numbers.  All array arguments
   must be the same size.  If the inputs are all scalars, a
   single scalar is returned.

   Shorthand for ``ulab.maximum(x2, ulab.minimum(x1, x3))``


.. function:: equal(x1: Union[ulab.array, float], x2: Union[ulab.array, float]) -> List[bool]

   Return an array of bool which is true where x1[i] == x2[i] and false elsewhere


.. function:: not_equal(x1: Union[ulab.array, float], x2: Union[ulab.array, float]) -> List[bool]

   Return an array of bool which is false where x1[i] == x2[i] and true elsewhere


.. function:: maximum(x1: Union[ulab.array, float], x2: Union[ulab.array, float]) -> ulab.array

   Compute the element by element maximum of the arguments.

   Arguments may be ulab arrays or numbers.  All array arguments
   must be the same size.  If the inputs are both scalars, a number is
   returned


.. function:: minimum(x1: Union[ulab.array, float], x2: Union[ulab.array, float]) -> ulab.array

   Compute the element by element minimum of the arguments.

   Arguments may be ulab arrays or numbers.  All array arguments
   must be the same size.  If the inputs are both scalars, a number is
   returned



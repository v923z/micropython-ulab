
Linalg
======

Functions in the ``linalg`` module can be called by prepending them by
``numpy.linalg.``. The module defines the following seven functions:

1. `numpy.linalg.cholesky <#cholesky>`__
2. `numpy.linalg.det <#det>`__
3. `numpy.linalg.dot <#dot>`__
4. `numpy.linalg.eig <#eig>`__
5. `numpy.linalg.inv <#inv>`__
6. `numpy.linalg.norm <#norm>`__
7. `numpy.linalg.trace <#trace>`__

cholesky
--------

``numpy``:
https://docs.scipy.org/doc/numpy-1.17.0/reference/generated/numpy.linalg.cholesky.html

The function of the Cholesky decomposition takes a positive definite,
symmetric square matrix as its single argument, and returns the *square
root matrix* in the lower triangular form. If the input argument does
not fulfill the positivity or symmetry condition, a ``ValueError`` is
raised.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    a = np.array([[25, 15, -5], [15, 18,  0], [-5,  0, 11]])
    print('a: ', a)
    print('\n' + '='*20 + '\nCholesky decomposition\n', np.linalg.cholesky(a))

.. parsed-literal::

    a:  array([[25.0, 15.0, -5.0],
    	 [15.0, 18.0, 0.0],
    	 [-5.0, 0.0, 11.0]], dtype=float)
    
    ====================
    Cholesky decomposition
     array([[5.0, 0.0, 0.0],
    	 [3.0, 3.0, 0.0],
    	 [-1.0, 1.0, 3.0]], dtype=float)
    
    


det
---

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.linalg.det.html

The ``det`` function takes a square matrix as its single argument, and
calculates the determinant. The calculation is based on successive
elimination of the matrix elements, and the return value is a float,
even if the input array was of integer type.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    a = np.array([[1, 2], [3, 4]], dtype=np.uint8)
    print(np.linalg.det(a))

.. parsed-literal::

    -2.0
    


Benchmark
~~~~~~~~~

Since the routine for calculating the determinant is pretty much the
same as for finding the `inverse of a matrix <#inv>`__, the execution
times are similar:

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    @timeit
    def matrix_det(m):
        return np.linalg.inv(m)
    
    m = np.array([[1, 2, 3, 4, 5, 6, 7, 8], [0, 5, 6, 4, 5, 6, 4, 5], 
                  [0, 0, 9, 7, 8, 9, 7, 8], [0, 0, 0, 10, 11, 12, 11, 12], 
                 [0, 0, 0, 0, 4, 6, 7, 8], [0, 0, 0, 0, 0, 5, 6, 7], 
                 [0, 0, 0, 0, 0, 0, 7, 6], [0, 0, 0, 0, 0, 0, 0, 2]])
    
    matrix_det(m)

.. parsed-literal::

    execution time:  294  us
    


dot
---

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.dot.html

**WARNING:** numpy applies upcasting rules for the multiplication of
matrices, while ``ulab`` simply returns a float matrix.

Once you can invert a matrix, you might want to know, whether the
inversion is correct. You can simply take the original matrix and its
inverse, and multiply them by calling the ``dot`` function, which takes
the two matrices as its arguments. If the matrix dimensions do not
match, the function raises a ``ValueError``. The result of the
multiplication is expected to be the unit matrix, which is demonstrated
below.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    m = np.array([[1, 2, 3], [4, 5, 6], [7, 10, 9]], dtype=np.uint8)
    n = np.linalg.inv(m)
    print("m:\n", m)
    print("\nm^-1:\n", n)
    # this should be the unit matrix
    print("\nm*m^-1:\n", np.linalg.dot(m, n))

.. parsed-literal::

    m:
     array([[1, 2, 3],
    	 [4, 5, 6],
    	 [7, 10, 9]], dtype=uint8)
    
    m^-1:
     array([[-1.25, 1.0, -0.25],
    	 [0.5, -1.0, 0.5],
    	 [0.4166667, 0.3333334, -0.25]], dtype=float)
    
    m*m^-1:
     array([[1.0, 2.384186e-07, -1.490116e-07],
    	 [-2.980232e-07, 1.000001, -4.172325e-07],
    	 [-3.278255e-07, 1.311302e-06, 0.9999992]], dtype=float)
    


Note that for matrix multiplication you don’t necessarily need square
matrices, it is enough, if their dimensions are compatible (i.e., the
the left-hand-side matrix has as many columns, as does the
right-hand-side matrix rows):

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    m = np.array([[1, 2, 3, 4], [5, 6, 7, 8]], dtype=np.uint8)
    n = np.array([[1, 2], [3, 4], [5, 6], [7, 8]], dtype=np.uint8)
    print(m)
    print(n)
    print(np.linalg.dot(m, n))

.. parsed-literal::

    array([[1, 2, 3, 4],
    	 [5, 6, 7, 8]], dtype=uint8)
    array([[1, 2],
    	 [3, 4],
    	 [5, 6],
    	 [7, 8]], dtype=uint8)
    array([[7.0, 10.0],
    	 [23.0, 34.0]], dtype=float)
    
    


eig
---

``numpy``:
https://docs.scipy.org/doc/numpy/reference/generated/numpy.linalg.eig.html

The ``eig`` function calculates the eigenvalues and the eigenvectors of
a real, symmetric square matrix. If the matrix is not symmetric, a
``ValueError`` will be raised. The function takes a single argument, and
returns a tuple with the eigenvalues, and eigenvectors. With the help of
the eigenvectors, amongst other things, you can implement sophisticated
stabilisation routines for robots.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    a = np.array([[1, 2, 1, 4], [2, 5, 3, 5], [1, 3, 6, 1], [4, 5, 1, 7]], dtype=np.uint8)
    x, y = np.linalg.eig(a)
    print('eigenvectors of a:\n', y)
    print('\neigenvalues of a:\n', x)

.. parsed-literal::

    eigenvectors of a:
     array([[0.8151560042509081, -0.4499411232970823, -0.1644660242574522, 0.3256141906686505],
           [0.2211334179893007, 0.7846992598235538, 0.08372081379922657, 0.5730077734355189],
           [-0.1340114162071679, -0.3100776411558949, 0.8742786816656, 0.3486109343758527],
           [-0.5183258053659028, -0.292663481927148, -0.4489749870391468, 0.6664142156731531]], dtype=float)
    
    eigenvalues of a:
     array([-1.165288365404889, 0.8029365530314914, 5.585625756072663, 13.77672605630074], dtype=float)
    
    


The same matrix diagonalised with ``numpy`` yields:

.. code::

    # code to be run in CPython
    
    a = array([[1, 2, 1, 4], [2, 5, 3, 5], [1, 3, 6, 1], [4, 5, 1, 7]], dtype=np.uint8)
    x, y = eig(a)
    print('eigenvectors of a:\n', y)
    print('\neigenvalues of a:\n', x)

.. parsed-literal::

    eigenvectors of a:
     [[ 0.32561419  0.815156    0.44994112 -0.16446602]
     [ 0.57300777  0.22113342 -0.78469926  0.08372081]
     [ 0.34861093 -0.13401142  0.31007764  0.87427868]
     [ 0.66641421 -0.51832581  0.29266348 -0.44897499]]
    
    eigenvalues of a:
     [13.77672606 -1.16528837  0.80293655  5.58562576]


When comparing results, we should keep two things in mind:

1. the eigenvalues and eigenvectors are not necessarily sorted in the
   same way
2. an eigenvector can be multiplied by an arbitrary non-zero scalar, and
   it is still an eigenvector with the same eigenvalue. This is why all
   signs of the eigenvector belonging to 5.58, and 0.80 are flipped in
   ``ulab`` with respect to ``numpy``. This difference, however, is of
   absolutely no consequence.

Computation expenses
~~~~~~~~~~~~~~~~~~~~

Since the function is based on `Givens
rotations <https://en.wikipedia.org/wiki/Givens_rotation>`__ and runs
till convergence is achieved, or till the maximum number of allowed
rotations is exhausted, there is no universal estimate for the time
required to find the eigenvalues. However, an order of magnitude can, at
least, be guessed based on the measurement below:

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    @timeit
    def matrix_eig(a):
        return np.linalg.eig(a)
    
    a = np.array([[1, 2, 1, 4], [2, 5, 3, 5], [1, 3, 6, 1], [4, 5, 1, 7]], dtype=np.uint8)
    
    matrix_eig(a)

.. parsed-literal::

    execution time:  111  us
    


inv
---

``numpy``:
https://docs.scipy.org/doc/numpy-1.17.0/reference/generated/numpy.linalg.inv.html

A square matrix, provided that it is not singular, can be inverted by
calling the ``inv`` function that takes a single argument. The inversion
is based on successive elimination of elements in the lower left
triangle, and raises a ``ValueError`` exception, if the matrix turns out
to be singular (i.e., one of the diagonal entries is zero).

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    m = np.array([[1, 2, 3, 4], [4, 5, 6, 4], [7, 8.6, 9, 4], [3, 4, 5, 6]])
    
    print(np.linalg.inv(m))

.. parsed-literal::

    array([[-2.166666666666667, 1.500000000000001, -0.8333333333333337, 1.0],
           [1.666666666666667, -3.333333333333335, 1.666666666666668, -0.0],
           [0.1666666666666666, 2.166666666666668, -0.8333333333333337, -1.0],
           [-0.1666666666666667, -0.3333333333333333, 0.0, 0.5]], dtype=float64)
    
    


Computation expenses
~~~~~~~~~~~~~~~~~~~~

Note that the cost of inverting a matrix is approximately twice as many
floats (RAM), as the number of entries in the original matrix, and
approximately as many operations, as the number of entries. Here are a
couple of numbers:

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    @timeit
    def invert_matrix(m):
        return np.linalg.inv(m)
    
    m = np.array([[1, 2,], [4, 5]])
    print('2 by 2 matrix:')
    invert_matrix(m)
    
    m = np.array([[1, 2, 3, 4], [4, 5, 6, 4], [7, 8.6, 9, 4], [3, 4, 5, 6]])
    print('\n4 by 4 matrix:')
    invert_matrix(m)
    
    m = np.array([[1, 2, 3, 4, 5, 6, 7, 8], [0, 5, 6, 4, 5, 6, 4, 5], 
                  [0, 0, 9, 7, 8, 9, 7, 8], [0, 0, 0, 10, 11, 12, 11, 12], 
                 [0, 0, 0, 0, 4, 6, 7, 8], [0, 0, 0, 0, 0, 5, 6, 7], 
                 [0, 0, 0, 0, 0, 0, 7, 6], [0, 0, 0, 0, 0, 0, 0, 2]])
    print('\n8 by 8 matrix:')
    invert_matrix(m)

.. parsed-literal::

    2 by 2 matrix:
    execution time:  65  us
    
    4 by 4 matrix:
    execution time:  105  us
    
    8 by 8 matrix:
    execution time:  299  us
    


The above-mentioned scaling is not obeyed strictly. The reason for the
discrepancy is that the function call is still the same for all three
cases: the input must be inspected, the output array must be created,
and so on.

norm
----

``numpy``:
https://numpy.org/doc/stable/reference/generated/numpy.linalg.norm.html

The function takes a vector or matrix without options, and returns its
2-norm, i.e., the square root of the sum of the square of the elements.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    a = np.array([1, 2, 3, 4, 5])
    b = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
    
    print('norm of a:', np.linalg.norm(a))
    print('norm of b:', np.linalg.norm(b))

.. parsed-literal::

    norm of a: 7.416198487095663
    norm of b: 16.88194301613414
    
    


trace
-----

``numpy``:
https://docs.scipy.org/doc/numpy-1.17.0/reference/generated/numpy.linalg.trace.html

The ``trace`` function returns the sum of the diagonal elements of a
square matrix. If the input argument is not a square matrix, an
exception will be raised.

The scalar so returned will inherit the type of the input array, i.e.,
integer arrays have integer trace, and floating point arrays a floating
point trace.

.. code::
        
    # code to be run in micropython
    
    from ulab import numpy as np
    
    a = np.array([[25, 15, -5], [15, 18,  0], [-5,  0, 11]], dtype=np.int8)
    print('a: ', a)
    print('\ntrace of a: ', np.linalg.trace(a))
    
    b = np.array([[25, 15, -5], [15, 18,  0], [-5,  0, 11]], dtype=np.float)
    
    print('='*20 + '\nb: ', b)
    print('\ntrace of b: ', np.linalg.trace(b))

.. parsed-literal::

    a:  array([[25, 15, -5],
    	 [15, 18, 0],
    	 [-5, 0, 11]], dtype=int8)
    
    trace of a:  54
    ====================
    b:  array([[25.0, 15.0, -5.0],
    	 [15.0, 18.0, 0.0],
    	 [-5.0, 0.0, 11.0]], dtype=float)
    
    trace of b:  54.0
    
    


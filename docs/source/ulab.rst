
ipython3.. code ::
        
    %pylab inline
.. parsed-literal::

    Populating the interactive namespace from numpy and matplotlib

Introduction
============

ulab is a C module for micropython. My goal was to implement a small
subset of numpy. I chose those functions and methods that might be
useful in the context of a microcontroller. This means low-level data
processing of linear (array) and two-dimensional (matrix) data.

The main points of ulab are

-  compact, iterable and slicable container of numerical data in 1, and
   2 dimensions (arrays and matrices). In addition, these containers
   support all the relevant unary and binary operators (e.g., ``len``,
   ==, +, \*, etc.)
-  vectorised computations on micropython iterables and numerical
   arrays/matrices (universal functions)
-  basic linear algebra routines (matrix inversion, matrix reshaping,
   and transposition)
-  polynomial fits to numerical data
-  fast Fourier transforms

The code itself is split into submodules. This should make exclusion of
unnecessary functions, if storage space is a concern. Each section of
the implementation part kicks out with a short discussion on what can be
done with the particular submodule, and what are the tripping points at
the C level. I hope that these musings can be used as a starting point
for further discussion on the code.

The code and its documentation can be found under
https://github.com/v923z/micropython-ulab/. The MIT licence applies to
all material.

Environmental settings and magic commands
=========================================

The entire C source code, as well as the documentation (mainly verbose
comments on certain aspects of the implementation) are contained in this
notebook. The code is exported to separate C files in ``/ulab/``, and
then compiled from this notebook. However, I would like to stress that
the compilation does not require a jupyter notebook. It can be done from
the command line by invoking the command in the `make <#make>`__, or
`Compiling the module <#Compiling-the-module>`__. After all, the ipython
kernel simply passes the ``make`` commands to the underlying operating
system.

Testing is done on the unix and stm32 ports of micropython, also
directly from the notebook. This is why this section contains a couple
of magic functions. But once again: the C module can be used without the
notebook.

ipython3.. code ::
        
    %cd ../../micropython/ports/unix/
.. parsed-literal::

    /home/v923z/sandbox/micropython/v1.11/micropython/ports/unix

ipython3.. code ::
        
    from IPython.core.magic import Magics, magics_class, line_cell_magic
    from IPython.core.magic import cell_magic, register_cell_magic, register_line_magic
    from IPython.core.magic_arguments import argument, magic_arguments, parse_argstring
    import subprocess
    import os
ipython3.. code ::
        
    def string_to_matrix(string):
        matrix = []
        string = string.replace("array(\'d\', ", '').replace(')', '').replace('[', '').replace(']', '')
        for _str in string.split('\r\n'):
            if len(_str) > 0:
                matrix.append([float(n) for n in _str.split(',')])
        return array(matrix)
micropython magic command
-------------------------

The following magic class takes the content of a cell, and depending on
the arguments, either passes it to the unix, or the stm32
implementation. In the latter case, a pyboard must be connected to the
computer, and must be initialised beforehand.

ipython3.. code ::
        
    @magics_class
    class PyboardMagic(Magics):
        @cell_magic
        @magic_arguments()
        @argument('-skip')
        @argument('-unix')
        @argument('-file')
        @argument('-data')
        @argument('-time')
        @argument('-memory')
        def micropython(self, line='', cell=None):
            args = parse_argstring(self.micropython, line)
            if args.skip: # doesn't care about the cell's content
                print('skipped execution')
                return None # do not parse the rest
            if args.unix: # tests the code on the unix port. Note that this works on unix only
                with open('/dev/shm/micropython.py', 'w') as fout:
                    fout.write(cell)
                proc = subprocess.Popen(["./micropython", "/dev/shm/micropython.py"], 
                                        stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                print(proc.stdout.read().decode("utf-8"))
                print(proc.stderr.read().decode("utf-8"))
                return None
            if args.file: # can be used to copy the cell content onto the pyboard's flash
                spaces = "    "
                try:
                    with open(args.file, 'w') as fout:
                        fout.write(cell.replace('\t', spaces))
                        printf('written cell to {}'.format(args.file))
                except:
                    print('Failed to write to disc!')
                return None # do not parse the rest
            if args.data: # can be used to load data from the pyboard directly into kernel space
                message = pyb.exec(cell)
                if len(message) == 0:
                    print('pyboard >>>')
                else:
                    print(message.decode('utf-8'))
                    # register new variable in user namespace
                    self.shell.user_ns[args.data] = string_to_matrix(message.decode("utf-8"))
            
            if args.time: # measures the time of executions
                pyb.exec('import utime')
                message = pyb.exec('t = utime.ticks_us()\n' + cell + '\ndelta = utime.ticks_diff(utime.ticks_us(), t)' + 
                                   "\nprint('execution time: {:d} us'.format(delta))")
                print(message.decode('utf-8'))
            
            if args.memory: # prints out memory information 
                message = pyb.exec('from micropython import mem_info\nprint(mem_info())\n')
                print("memory before execution:\n========================\n", message.decode('utf-8'))
                message = pyb.exec(cell)
                print(">>> ", message.decode('utf-8'))
                message = pyb.exec('print(mem_info())')
                print("memory after execution:\n========================\n", message.decode('utf-8'))
    
            else:
                message = pyb.exec(cell)
                print(message.decode('utf-8'))
    
    ip = get_ipython()
    ip.register_magics(PyboardMagic)
pyboard initialisation
~~~~~~~~~~~~~~~~~~~~~~

ipython3.. code ::
        
    import pyboard
    pyb = pyboard.Pyboard('/dev/ttyACM0')
    pyb.enter_raw_repl()
pyboad detach
~~~~~~~~~~~~~

ipython3.. code ::
        
    pyb.exit_raw_repl()
    pyb.close()
ipython3.. code ::
        
    import IPython
    
    js = """
        (function () {
            var defaults = IPython.CodeCell.config_defaults || IPython.CodeCell.options_default;
            defaults.highlight_modes['magic_text/x-csrc'] = {'reg':[/^\\s*%%ccode/]};
        })();
        """
    cjs = """
            IPython.CodeCell.options_default.highlight_modes['magic_text/x-csrc'] = {'reg':[/^\\s*%%ccode/]};
        """
    
    IPython.core.display.display_javascript(cjs, raw=True)
    
    js = """
        (function () {
            var defaults = IPython.CodeCell.config_defaults || IPython.CodeCell.options_default;
            defaults.highlight_modes['magic_text/x-csrc'] = {'reg':[/^\\s*%%makefile/]};
        })();
        """
    IPython.core.display.display_javascript(js, raw=True)





Code magic
----------

The following cell magic simply writes a licence header, and the
contents of the cell to the file given in the header of the cell.

ipython3.. code ::
        
    @magics_class
    class MyMagics(Magics):
            
        @cell_magic
        def ccode(self, line, cell):
            copyright = """/*
     * This file is part of the micropython-ulab project, 
     *
     * https://github.com/v923z/micropython-ulab
     *
     * The MIT License (MIT)
     *
     * Copyright (c) 2019 Zoltán Vörös
    */
        """
            if line:
                with open('../../../ulab/code/'+line, 'w') as cout:
                    cout.write(copyright)
                    cout.write(cell)
                print('written %d bytes to %s'%(len(copyright) + len(cell), line))
                return None
    
    ip = get_ipython()
    ip.register_magics(MyMagics)
Notebook conversion
===================

ipython3.. code ::
        
    %cd ../../../ulab/docs/
.. parsed-literal::

    /home/v923z/sandbox/micropython/v1.11/ulab/docs

ipython3.. code ::
        
    import nbformat as nb
    import nbformat.v4.nbbase as nb4
    from nbconvert import RSTExporter
    
    def convert_notebook(nbfile, rstfile):
        (rst, resources) = rstexporter.from_filename(nbfile)
        with open(rstfile, 'w') as fout:
            fout.write(rst)
            
    rstexporter = RSTExporter()
    rstexporter.template_file = './templates/rst.tpl'
    
    convert_notebook('ulab.ipynb', './source/ulab.rst')
.. parsed-literal::

    /home/v923z/anaconda3/lib/python3.7/site-packages/nbconvert/filters/datatypefilter.py:41: UserWarning: Your element with mimetype(s) dict_keys(['application/javascript']) is not able to be represented.
      mimetypes=output.keys())

Compiling the module
====================

Detailed instructions on how to set up and compile a C module can be
found in chapter 2 of
https://micropython-usermod.readthedocs.io/en/latest/.

First, on the command line, you should clone both the micropython, and
the ``ulab`` repositories:

.. code:: bash

   git clone https://github.com/micropython/micropython.git

Then navigate to your micropython folder, and run

.. code:: bash

   git clone https://github.com/v923z/micropython-ulab.git ulab

Finally, in the ``mpconfigport.h`` header file of the port that you want
to compile for, you have to define the variable ``MODULE_ULAB_ENABLED``

.. code:: make

   #define MODULE_ULAB_ENABLED (1)

At this point, you should be able to run make in the port’s root folder:

.. code:: bash

   make USER_C_MODULES=../../../ulab all

(unix port) or

.. code:: bash

   make BOARD=PYBV11 CROSS_COMPILE=<Path where you uncompressed the toolchain>/bin/arm-none-eabi-

(pyboard). When compiling for the pyboard (or any other hardware
platform), you might or might not have to set the cross-compiler’s path.
If your installation of the cross-compiler is system-wide, you can drop
the ``make`` argument ``CROSS_COMPILE``.

The ndarray type
================

General comments
----------------

``ndarrays`` are efficient containers of numerical data of the same type
(i.e., signed/unsigned chars, signed/unsigned integers or floats).
Beyond storing the actual data, the type definition has three additional
members (on top of the ``base`` type). Namely, two ``size_t`` objects,
``m``, and ``n``, which give the dimensions of the matrix (obviously, if
the ``ndarray`` is meant to be linear, either ``m``, or ``n`` is equal
to 1), as well as the byte size, ``bytes``, i.e., the total number of
bytes consumed by the data container. ``bytes`` is equal to ``m*n`` for
``byte`` types (``uint8``, and ``int8``), to ``2*m*n`` for integers
(``uint16``, and ``int16``), and ``4*m*n`` for floats.

The type definition is as follows:

.. code:: c

   typedef struct _ndarray_obj_t {
       mp_obj_base_t base;
       size_t m, n;
       mp_obj_array_t *data;
       size_t bytes;
   } ndarray_obj_t;

**NOTE: with a little bit of extra effort, mp_obj_array_t can be
replaced by a single void array. We should, perhaps, consider the pros
and cons of that. One patent advantage is that we could get rid of the
verbatim copy of array_new function in ndarray.c. On the other hand,
objarray.c facilities couldn’t be used anymore.**

Handling different types
------------------------

In order to make the code type-agnostic, we will resort to macros, where
necessary. This will inevitably add to the firmware size, because, in
effect, we unroll the code for each possible case. However, the source
will be much more readable. Also note that by unrolling, we no longer
need intermediate containers and we no longer need to dispatch
type-conversion functions, which means that we should be able to gain in
speed.

Additional structure members in numpy
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Also note that, in addition, ``numpy`` defines the following members:

-  ``.ndim``: the number of dimensions of the array (in our case, it
   would be 1, or 2)
-  ``.size``: the number of elements in the array; it is the product of
   m, and n
-  ``.dtype``: the data type; in our case, it is basically stored in
   data->typecode
-  ``.itemsize``: the size of a single element in the array: this can be
   gotten by calling ``mp_binary_get_size('@', data->typecode, NULL)``.

One should, perhaps, consider, whether these are necessary fields. E.g.,
if ``ndim`` were defined, then

.. code:: c

   if((myarray->m == 1) || (myarray->n == 1)) {
       ...
   }

could be replaced by

.. code:: c

   if(myarray->ndim == 1) {
       ...
   }

and

.. code:: c

   if((myarray->m > 1) && (myarray->n > 1)) {
       ...
   }

would be equivalent to

.. code:: c

   if(myarray->ndim == 2) {
       ...
   }

One could also save the extra function call
``mp_binary_get_size('@', data->typecode, NULL)``, if ``itemsize`` is
available.

Returning and accepting raw bytes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It might make sense to have a function that returns the raw content of
the ``ndarray``. The rationale for this is that this would make direct
use of calculation results a piece of cake. E.g., the DAC could be fed
as

.. code:: python

   length = 100
   amp = 127

   x = linspace(0, 2*pi, length)
   y = ndarray(128 + amp*sin(x), dtype=uint8)
   buf = y.bytearray()

   dac = DAC(1)
   dac.write_timed(buf, 400*length, mode=DAC.CIRCULAR)

Likewise, having the option of writing raw data directly into the
``ndarray`` could simplify data analysis. E.g., ADC results could be
processed as follows:

.. code:: python

   length = 100
   y = ndarray([0]*length, dtype=uint16)

   adc = pyb.ADC(pyb.Pin.board.X19)
   tim = pyb.Timer(6, freq=10)
   buf = y.bytearray()
   adc.read_timed(buf, tim)

   y.reshape((10, 10)) # or whatever

Exposed functions and properties
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Most of the functions in ndarray.c are internal (i.e., not exposed to
the python interpreter). Exception to this rule are the ``shape``,
``size``, and ``rawsize`` functions, and the ``.unary_op``,
``.binary_op``, and ``.iter_next`` class methods. Note that ``rawsize``
is is not standard in numpy, and is meant to return the total number of
bytes used by the container. Since the RAM of a microcontroller is
limited, I deemed this to be a reasonable addition for optimisation
purposes, but could later be removed, if it turns out to be of no
interest.

As mentioned above, ``numpy`` defines a number of extra members in its
``ndarray``. It would be great, if we could return these members as
properties of the ``ndarray`` instance. At the moment, ``shape`` is a
function, as is ``rawsize``.

Initialisation
--------------

An ``ndarray`` can be initialised by passing an iterable (linear
arrays), or an iterable of iterables (matrices) into the constructor. In
addition, the constructor can take a keyword argument, ``dtype``, that
will force type conversion. The default value is ``float``.

ipython3.. code ::
        
    %%micropython -unix 1
    
    from ulab import ndarray
    
    a = ndarray([1, 2, 3, 4])
    print(a)
    a = ndarray([[1, 2, 3, 4], [2, 3, 4, 5]])
    print('\n', a)
    a = ndarray([range(10), range(10)])
    print('\n', a)
.. parsed-literal::

    ndarray([1.0, 2.0, 3.0, 4.0], dtype=float)
    
     ndarray([[1.0, 2.0, 3.0, 4.0],
    	 [2.0, 3.0, 4.0, 5.0]], dtype=float)
    
     ndarray([[0.0, 1.0, 2.0, ..., 7.0, 8.0, 9.0],
    	 [0.0, 1.0, 2.0, ..., 7.0, 8.0, 9.0]], dtype=float)
    
    

Slicing and subscriptions
-------------------------

Slicing and subscription work work only partially. This has to do with
the fact that slices can be given only in the form ``myobject[1:10:2]``,
and slice objects cannot be separated by commas, i.e., this won’t be
interpreted:

.. code:: python

   ndarray[1:10:2, 3:10:3]

This is something that should be sorted out in the future. As a
temporary solution, we could implement the ``getter`` and ``setter``
special methods that handle this issue.

ipython3.. code ::
        
    %%micropython -unix 1
    
    from ulab import ndarray
    
    # initialise a matrix
    a = ndarray([[1, 2, 3, 4], [6, 7, 8, 9]])
    print('2D array: \n', a)
    
    # print out the second row
    print('second row of matrix: ', a[1])
    
    #initialise an array
    a = ndarray([1, 2, 3, 4, 5, 6, 7, 8, 9])
    print('\n1D array: ', a)
    # slice the array
    print('slize between 1, and 5: ', a[1:5])
.. parsed-literal::

    2D array: 
     ndarray([[1.0, 2.0, 3.0, 4.0],
    	 [6.0, 7.0, 8.0, 9.0]], dtype=float)
    second row of matrix:  ndarray([6.0, 7.0, 8.0, 9.0], dtype=float)
    
    1D array:  ndarray([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0], dtype=float)
    slize between 1, and 5:  ndarray([2.0, 3.0, 4.0, 5.0], dtype=float)
    
    

Iterators
---------

``ndarray`` objects can be iterated on, and just as in numpy, matrices
are iterated along their first axis, and they return ``ndarray``\ s.

ipython3.. code ::
        
    %%micropython -unix 1
    
    from ulab import ndarray
    
    #  initialise a matrix
    a = ndarray([[1, 2, 3, 4], [6, 7, 8, 9]])
    print('2D array: \n', a)
    
    # print out the matrix' rows, one by one
    for i, _a in enumerate(a): 
        print('\nrow %d: '%i, _a)
.. parsed-literal::

    2D array: 
     ndarray([[1.0, 2.0, 3.0, 4.0],
    	 [6.0, 7.0, 8.0, 9.0]], dtype=float)
    
    row 0:  ndarray([1.0, 2.0, 3.0, 4.0], dtype=float)
    
    row 1:  ndarray([6.0, 7.0, 8.0, 9.0], dtype=float)
    
    

On the other hand, flat arrays return their elements:

ipython3.. code ::
        
    %%micropython -unix 1
    
    from ulab import ndarray, uint8
    
    # initialise an array
    a = ndarray(range(10), dtype=uint8)
    print('1D array: ', a)
    
    # print out the array's elements, one by one
    for i, _a in enumerate(a): 
        print('element %d: '%i, _a)
.. parsed-literal::

    1D array:  ndarray([0, 1, 2, ..., 7, 8, 9], dtype=uint8)
    element 0:  0
    element 1:  1
    element 2:  2
    element 3:  3
    element 4:  4
    element 5:  5
    element 6:  6
    element 7:  7
    element 8:  8
    element 9:  9
    
    

Upcasting
---------

The following section shows the upcasting rules of ``numpy``, and
immediately after each case, the test for ``ulab``.

uint8
~~~~~

ipython3.. code ::
        
    a = array([100], dtype=uint8)
    b = array([101], dtype=uint8)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([201], dtype=uint8),
     array([255], dtype=uint8),
     array([116], dtype=uint8),
     array([0.99009901]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.uint8)
    b = ulab.ndarray([101], dtype=ulab.uint8)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([201], dtype=uint8)
    ndarray([255], dtype=uint8)
    ndarray([116], dtype=uint8)
    ndarray([0.9900990128517151], dtype=float)
    
    

ipython3.. code ::
        
    a = array([100], dtype=uint8)
    b = array([101], dtype=int8)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([201], dtype=int16),
     array([-1], dtype=int16),
     array([10100], dtype=int16),
     array([0.99009901]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.uint8)
    b = ulab.ndarray([101], dtype=ulab.int8)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([201], dtype=int16)
    ndarray([-1], dtype=int16)
    ndarray([10100], dtype=int16)
    ndarray([0.9900990128517151], dtype=float)
    
    

ipython3.. code ::
        
    a = array([100], dtype=uint8)
    b = array([101], dtype=uint16)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([201], dtype=uint16),
     array([65535], dtype=uint16),
     array([10100], dtype=uint16),
     array([0.99009901]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.uint8)
    b = ulab.ndarray([101], dtype=ulab.uint16)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([201], dtype=uint16)
    ndarray([65535], dtype=uint16)
    ndarray([10100], dtype=uint16)
    ndarray([0.9900990128517151], dtype=float)
    
    

ipython3.. code ::
        
    a = array([100], dtype=uint8)
    b = array([101], dtype=int16)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([201], dtype=int16),
     array([-1], dtype=int16),
     array([10100], dtype=int16),
     array([0.99009901]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.uint8)
    b = ulab.ndarray([101], dtype=ulab.int16)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([201], dtype=int16)
    ndarray([-1], dtype=int16)
    ndarray([10100], dtype=int16)
    ndarray([0.9900990128517151], dtype=float)
    
    

ipython3.. code ::
        
    a = array([100], dtype=uint8)
    b = array([101], dtype=float)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([201.]), array([-1.]), array([10100.]), array([0.99009901]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.uint8)
    b = ulab.ndarray([101], dtype=ulab.float)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([201.0], dtype=float)
    ndarray([-1.0], dtype=float)
    ndarray([10100.0], dtype=float)
    ndarray([0.9900990128517151], dtype=float)
    
    

int8
~~~~

ipython3.. code ::
        
    a = array([100], dtype=int8)
    b = array([101], dtype=uint8)
    a + b, a-b, a*b, a/b




.. parsed-literal::

    (array([201], dtype=int16),
     array([-1], dtype=int16),
     array([10100], dtype=int16),
     array([0.99009901]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.int8)
    b = ulab.ndarray([101], dtype=ulab.uint8)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([201], dtype=int16)
    ndarray([-1], dtype=int16)
    ndarray([10100], dtype=int16)
    ndarray([0.9900990128517151], dtype=float)
    
    

ipython3.. code ::
        
    a = array([100, 101], dtype=int8)
    b = array([200, 101], dtype=int8)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([ 44, -54], dtype=int8),
     array([-100,    0], dtype=int8),
     array([ 32, -39], dtype=int8),
     array([-1.78571429,  1.        ]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100, 101], dtype=ulab.int8)
    b = ulab.ndarray([200, 101], dtype=ulab.int8)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([44, -54], dtype=int8)
    ndarray([-100, 0], dtype=int8)
    ndarray([32, -39], dtype=int8)
    ndarray([-1.785714268684387, 1.0], dtype=float)
    
    

ipython3.. code ::
        
    a = array([100], dtype=int8)
    b = array([200], dtype=uint16)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([300], dtype=int32),
     array([-100], dtype=int32),
     array([20000], dtype=int32),
     array([0.5]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.int8)
    b = ulab.ndarray([200], dtype=ulab.uint16)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([300], dtype=int16)
    ndarray([-100], dtype=int16)
    ndarray([20000], dtype=int16)
    ndarray([0], dtype=int16)
    
    

ipython3.. code ::
        
    a = array([100], dtype=int8)
    b = array([200], dtype=int16)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([300], dtype=int16),
     array([-100], dtype=int16),
     array([20000], dtype=int16),
     array([0.5]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.int8)
    b = ulab.ndarray([200], dtype=ulab.int16)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([300], dtype=int16)
    ndarray([-100], dtype=int16)
    ndarray([20000], dtype=int16)
    ndarray([0.5], dtype=float)
    
    

ipython3.. code ::
        
    a = array([100, 101], dtype=int8)
    b = array([200, 101], dtype=float)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([300., 202.]),
     array([-100.,    0.]),
     array([20000., 10201.]),
     array([0.5, 1. ]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100, 101], dtype=ulab.int8)
    b = ulab.ndarray([200, 101], dtype=ulab.float)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([300.0, 202.0], dtype=float)
    ndarray([-100.0, 0.0], dtype=float)
    ndarray([20000.0, 10201.0], dtype=float)
    ndarray([0.5, 1.0], dtype=float)
    
    

uint16
~~~~~~

ipython3.. code ::
        
    a = array([100, 101], dtype=uint16)
    b = array([200, 101], dtype=uint8)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([300, 202], dtype=uint16),
     array([65436,     0], dtype=uint16),
     array([20000, 10201], dtype=uint16),
     array([0.5, 1. ]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100, 101], dtype=ulab.uint16)
    b = ulab.ndarray([200, 101], dtype=ulab.uint8)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([300, 202], dtype=uint16)
    ndarray([65436, 0], dtype=uint16)
    ndarray([20000, 10201], dtype=uint16)
    ndarray([0.5, 1.0], dtype=float)
    
    

ipython3.. code ::
        
    a = array([100, 101], dtype=uint16)
    b = array([200, 101], dtype=int8)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([ 44, 202], dtype=int32),
     array([156,   0], dtype=int32),
     array([-5600, 10201], dtype=int32),
     array([-1.78571429,  1.        ]))



**This deviates from numpy’s behaviour, because we don’t have int32.**

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100, 101], dtype=ulab.uint16)
    b = ulab.ndarray([200, 101], dtype=ulab.int8)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([44, 202], dtype=uint16)
    ndarray([156, 0], dtype=uint16)
    ndarray([59936, 10201], dtype=uint16)
    ndarray([-1.785714268684387, 1.0], dtype=float)
    
    

ipython3.. code ::
        
    a = array([100], dtype=uint16)
    b = array([101], dtype=uint16)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([201], dtype=uint16),
     array([65535], dtype=uint16),
     array([10100], dtype=uint16),
     array([0.99009901]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.uint16)
    b = ulab.ndarray([101], dtype=ulab.uint16)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([201], dtype=uint16)
    ndarray([65535], dtype=uint16)
    ndarray([10100], dtype=uint16)
    ndarray([0.9900990128517151], dtype=float)
    
    

ipython3.. code ::
        
    a = array([100], dtype=uint16)
    b = array([101], dtype=int16)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([201], dtype=int32),
     array([-1], dtype=int32),
     array([10100], dtype=int32),
     array([0.99009901]))



**Again, in numpy, the result is an int32**

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.uint16)
    b = ulab.ndarray([101], dtype=ulab.int16)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([201.0], dtype=float)
    ndarray([-1.0], dtype=float)
    ndarray([10100.0], dtype=float)
    ndarray([0.9900990128517151], dtype=float)
    
    

ipython3.. code ::
        
    a = array([100], dtype=uint16)
    b = array([101], dtype=float)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([201.]), array([-1.]), array([10100.]), array([0.99009901]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.uint16)
    b = ulab.ndarray([101], dtype=ulab.float)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([201.0], dtype=float)
    ndarray([-1.0], dtype=float)
    ndarray([10100.0], dtype=float)
    ndarray([0.9900990128517151], dtype=float)
    
    

int16
~~~~~

ipython3.. code ::
        
    a = array([100], dtype=int16)
    b = array([101], dtype=uint8)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([201], dtype=int16),
     array([-1], dtype=int16),
     array([10100], dtype=int16),
     array([0.99009901]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.int16)
    b = ulab.ndarray([101], dtype=ulab.uint8)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([201], dtype=int16)
    ndarray([-1], dtype=int16)
    ndarray([10100], dtype=int16)
    ndarray([0.9900990128517151], dtype=float)
    
    

ipython3.. code ::
        
    a = array([100], dtype=int16)
    b = array([101], dtype=int8)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([201], dtype=int16),
     array([-1], dtype=int16),
     array([10100], dtype=int16),
     array([0.99009901]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.int16)
    b = ulab.ndarray([101], dtype=ulab.int8)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([201], dtype=int16)
    ndarray([-1], dtype=int16)
    ndarray([10100], dtype=int16)
    ndarray([0.9900990128517151], dtype=float)
    
    

ipython3.. code ::
        
    a = array([100], dtype=int16)
    b = array([101], dtype=uint16)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([201], dtype=int32),
     array([-1], dtype=int32),
     array([10100], dtype=int32),
     array([0.99009901]))



**While the results are correct, here we have float instead of int32**

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.int16)
    b = ulab.ndarray([101], dtype=ulab.uint16)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([201.0], dtype=float)
    ndarray([-1.0], dtype=float)
    ndarray([10100.0], dtype=float)
    ndarray([0.0], dtype=float)
    
    

ipython3.. code ::
        
    a = array([100], dtype=int16)
    b = array([101], dtype=int16)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([201], dtype=int16),
     array([-1], dtype=int16),
     array([10100], dtype=int16),
     array([0.99009901]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.int16)
    b = ulab.ndarray([101], dtype=ulab.int16)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([201], dtype=int16)
    ndarray([-1], dtype=int16)
    ndarray([10100], dtype=int16)
    ndarray([0], dtype=int16)
    
    

ipython3.. code ::
        
    a = array([100], dtype=int16)
    b = array([101], dtype=float)
    a+b, a-b, a*b, a/b




.. parsed-literal::

    (array([201.]), array([-1.]), array([10100.]), array([0.99009901]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([100], dtype=ulab.int16)
    b = ulab.ndarray([101], dtype=ulab.float)
    print(a+b)
    print(a-b)
    print(a*b)
    print(a/b)
.. parsed-literal::

    ndarray([201.0], dtype=float)
    ndarray([-1.0], dtype=float)
    ndarray([10100.0], dtype=float)
    ndarray([0.9900990128517151], dtype=float)
    
    

When in an operation the ``dtype`` of two arrays is different, the
result’s ``dtype`` will be decided by the following upcasting rules:

1. Operations with two ``ndarray``\ s of the same ``dtype`` preserve
   their ``dtype``, even when the results overflow.

2. if either of the operands is a float, the results is also a float

3. 

   -  ``uint8`` + ``int8`` => ``int16``,

   -  ``uint8`` + ``int16`` => ``int16``

   -  ``uint8`` + ``uint16`` => ``uint16``

   -  ``int8`` + ``int16`` => ``int16``

   -  ``int8`` + ``uint16`` => ``uint16`` (in numpy, it is a ``int32``)

   -  ``uint16`` + ``int16`` => ``float`` (in numpy, it is a ``int32``)

4. When the right hand side of a binary operator is a micropython
   variable, ``mp_obj_int``, or ``mp_obj_float``, then the result will
   be promoted to ``dtype`` ``float``. This is necessary, because a
   micropython integer can be 31 bites wide.

``numpy`` is also inconsistent in how it represents ``dtype``: as an
argument, it is denoted by the constants ``int8``, ``uint8``, etc.,
while a string will be returned, if the user asks for the type of an
array.

The upcasting rules are stipulated in a single C function,
``upcasting()``.

upcasting rules with scalars
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When a

ipython3.. code ::
        
    a = array([1, 2, 3], dtype=int8)
    b = a * 555
    a *= -555
    b.dtype, b, a.dtype, a




.. parsed-literal::

    (dtype('int16'),
     array([ 555, 1110, 1665], dtype=int16),
     dtype('int8'),
     array([-43, -86, 127], dtype=int8))



Binary operations
-----------------

In the standard binary operations, the operands are either two
``ndarray``\ s or an ``ndarray``, and a number. From the C standpoint,
these operations are probably the most difficult: the problem is that
two operands, each with 5 possible C types are added, multiplied,
subtracted, or divided, hence making the number of possible combinations
large. In order to mitigate the situation, we make use of macros: this
would make most of the code type-agnostic.

Also, when an operation involves a scalar, and an ``ndarray``, we will
turn the scalar into an ``ndarray`` of length 1. In this way, we can
reduce the code size of the binary handler by almost a factor of two.

ipython3.. code ::
        
    %%micropython -unix 1
    
    from ulab import ndarray, float
    
    a = ndarray([1, 2, 3], dtype=float)
    print(a + a)
    print(a * 5.0)
    print(a / 2)
    print(a - 10)
.. parsed-literal::

    ndarray([2.0, 4.0, 6.0], dtype=float)
    ndarray([5.0, 10.0, 15.0], dtype=float)
    ndarray([0.5, 1.0, 1.5], dtype=float)
    ndarray([-9.0, -8.0, -7.0], dtype=float)
    
    

in-place operators
~~~~~~~~~~~~~~~~~~

In-place operators preserve the type of the array’s type. Here are a
couple of caveats:

1. overflow obviously occurs
2. float can be added only to float type
3. true divide fails, except when the array is of type float

ipython3.. code ::
        
    a = array([1, 2, 3, 40], dtype=uint8)
    a += 220
    a




.. parsed-literal::

    array([221, 222, 223,   4], dtype=uint8)



ipython3.. code ::
        
    a = array([1, 2, 3, 40], dtype=int8)
    a += 220
    a




.. parsed-literal::

    array([-35, -34, -33,   4], dtype=int8)



ipython3.. code ::
        
    a = array([1, 2, 3, 40], dtype=uint16)
    a += 220
    a




.. parsed-literal::

    array([221, 222, 223, 260], dtype=uint16)



ipython3.. code ::
        
    a = array([1, 2, 3, 40], dtype=int16)
    a += 220
    a




.. parsed-literal::

    array([221, 222, 223, 260], dtype=int16)



ipython3.. code ::
        
    a = array([1, 2, 3, 40], dtype=float)
    a += 220
    a




.. parsed-literal::

    array([221., 222., 223., 260.])



ipython3.. code ::
        
    a = array([1, 2, 3, 40], dtype=uint8)
    a += 220.0
    a

::


    ---------------------------------------------------------------------------

    TypeError                                 Traceback (most recent call last)

    <ipython-input-406-658f10292bb1> in <module>
          1 a = array([1, 2, 3, 40], dtype=uint8)
    ----> 2 a += 220.0
          3 a


    TypeError: Cannot cast ufunc add output from dtype('float64') to dtype('uint8') with casting rule 'same_kind'


ipython3.. code ::
        
    a = array([1, 2, 3, 40], dtype=uint8)
    a /= 22
    a

::


    ---------------------------------------------------------------------------

    TypeError                                 Traceback (most recent call last)

    <ipython-input-407-1ed2746d8aeb> in <module>
          1 a = array([1, 2, 3, 40], dtype=uint8)
    ----> 2 a /= 22
          3 a


    TypeError: No loop matching the specified signature and casting
    was found for ufunc true_divide


ipython3.. code ::
        
    a = array([1, 2, 3, 40], dtype=float)
    a += 220
    a




.. parsed-literal::

    array([221., 222., 223., 260.])



ipython3.. code ::
        
    a = array([1, 2, 3, 4], dtype=int8)
    b = array([5, 10, 15, 20], dtype=float)
    a /= b
    a

::


    ---------------------------------------------------------------------------

    TypeError                                 Traceback (most recent call last)

    <ipython-input-413-80e572931886> in <module>
          1 a = array([1, 2, 3, 4], dtype=int8)
          2 b = array([5, 10, 15, 20], dtype=float)
    ----> 3 a /= b
          4 a


    TypeError: ufunc 'true_divide' output (typecode 'd') could not be coerced to provided output parameter (typecode 'b') according to the casting rule ''same_kind''


ipython3.. code ::
        
    a = array([1, 2, 3, 4], dtype=int8)
    b = array([5, 10, 15, 20], dtype=int8)
    a /= b
    a

::


    ---------------------------------------------------------------------------

    TypeError                                 Traceback (most recent call last)

    <ipython-input-414-a76603b93818> in <module>
          1 a = array([1, 2, 3, 4], dtype=int8)
          2 b = array([5, 10, 15, 20], dtype=int8)
    ----> 3 a /= b
          4 a


    TypeError: No loop matching the specified signature and casting
    was found for ufunc true_divide


ipython3.. code ::
        
    a = array([1, 2, 3, 4], dtype=int8)
    b = array([5, 10, 15, 100], dtype=int16)
    a *= b
    a




.. parsed-literal::

    array([   5,   20,   45, -112], dtype=int8)



ipython3.. code ::
        
    a = array([1, 2, 3, 4], dtype=int8)
    a **= 2
    a




.. parsed-literal::

    array([ 1,  4,  9, 16], dtype=int8)



Comparison operators
~~~~~~~~~~~~~~~~~~~~

These return list(s) of Booleans.

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([1, 2, 3, 4, 5, 6, 7, 8])
    a.reshape((1, 8))
    print(a < 4)
    print(a <= 4)
    print(a > 4)
    print(a >= 4)
.. parsed-literal::

    [True, True, True, False, False, False, False, False]
    [True, True, True, True, False, False, False, False]
    [False, False, False, False, True, True, True, True]
    [False, False, False, True, True, True, True, True]
    
    

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([[1, 2, 3, 4], [5, 6, 7, 8]])
    print(a < 4)
    print(a <= 4)
    print(a > 4)
    print(a >= 4)
.. parsed-literal::

    [[True, True, True, False], [False, False, False, False]]
    [[True, True, True, True], [False, False, False, False]]
    [[False, False, False, False], [True, True, True, True]]
    [[False, False, False, True], [True, True, True, True]]
    
    

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([1, 2, 3, 4, 4, 6, 7, 8])
    a.transpose()
    b = ulab.ndarray([8, 7, 6, 5, 4, 3, 2, 1])
    b.transpose()
    print(a < b)
    print(a <= b)
    print(a > b)
    print(a >= b)
.. parsed-literal::

    [True, True, True, True, False, False, False, False]
    [True, True, True, True, True, False, False, False]
    [False, False, False, False, False, True, True, True]
    [False, False, False, False, True, True, True, True]
    
    

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([[1, 2, 3, 4], [5, 6, 7, 8]])
    b = ulab.ndarray([[8, 7, 1, 1], [4, 3, 2, 1]], dtype=ulab.int8)
    print(a < b)
    print(a <= b)
    print(a > b)
    print(a >= b)
.. parsed-literal::

    [[True, True, False, False], [False, False, False, False]]
    [[True, True, False, False], [False, False, False, False]]
    [[False, False, True, True], [True, True, True, True]]
    [[False, False, True, True], [True, True, True, True]]
    
    

Simple running weighted average
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

With the subscription tools, a weighted running average can very easily
be implemented as follows:

ipython3.. code ::
        
    %%micropython -unix 1
    
    from ulab import ndarray, mean, roll
    
    # These are the weights; the last entry is the most dominant
    weight = ndarray([1, 2, 3, 4, 5]) 
    
    # initial array of samples
    samples = ndarray([0]*5)
    
    for i in range(5):
        # a new datum is inserted on the right hand side. This simply overwrites whatever was in the last slot
        samples[-1] = 2
        print(mean(samples*weight))
        # the data are shifted by one position to the left
        roll(samples, 1)
.. parsed-literal::

    2.0
    3.6
    4.8
    5.6
    6.0
    
    

Unary operators
---------------

At the moment, only ``len`` is implemented, which returns the number of
elements for one-dimensional arrays, and the length of the first axis
for matrices. One should consider other possibilities.

ipython3.. code ::
        
    %%micropython -unix 1
    
    from ulab import ndarray
    
    # initialise an array
    a = ndarray(range(10))
    print('1D array: ', a)
    
    # print out the array's length
    print('length of array: ', len(a))
.. parsed-literal::

    1D array:  ndarray([0.0, 1.0, 2.0, ..., 7.0, 8.0, 9.0], dtype=float)
    length of array:  10
    
    

ipython3.. code ::
        
    %%micropython -unix 1
    
    from ulab import ndarray
    
    # initialise a matrix
    a = ndarray([range(10), range(10), range(10)])
    print('2D array: \n', a)
    
    # print out the array's elements, one by one
    print('length of array: ', len(a))
.. parsed-literal::

    2D array: 
     ndarray([[0.0, 1.0, 2.0, ..., 7.0, 8.0, 9.0],
    	 [0.0, 1.0, 2.0, ..., 7.0, 8.0, 9.0],
    	 [0.0, 1.0, 2.0, ..., 7.0, 8.0, 9.0]], dtype=float)
    length of array:  3
    
    

Class methods: shape, size, rawsize, flatten
--------------------------------------------

ipython3.. code ::
        
    %%micropython -unix 1
    
    from ulab import ndarray
    
    # initialise an array
    a = ndarray(range(10))
    print('1D array: ', a)
    
    # print out the shape
    print('shape: ', a.shape())
    
    #print out the size
    print('size 0: ', a.size(0), '\nsize 1: ', a.size(1), '\nsize 2: ', a.size(2))
    
    #print out the raw size
    print('raw size: ', a.rawsize())
    
    # initialise a matrix
    a = ndarray([range(10), range(10), range(10)])
    print('\n2D array: \n', a)
    
    # print out the shape
    print('shape: ', a.shape())
    
    #print out the size
    print('size 0: ', a.size(0), '\nsize 1: ', a.size(1), '\nsize 2: ', a.size(2))
    
    #print out the raw size
    print('raw size: ', a.rawsize())
    
    #flattened array
    a = ndarray([range(3), range(3), range(3)])
    print('\n2D array: \n', a)
    print('flattened array: (C)', a.flatten(order='C'))
    print('flattened array: (F)', a.flatten(order='F'))
.. parsed-literal::

    1D array:  ndarray([0.0, 1.0, 2.0, ..., 7.0, 8.0, 9.0], dtype=float)
    shape:  (10, 1)
    size 0:  10 
    size 1:  10 
    size 2:  1
    raw size:  (10, 1, 40, 10, 4)
    
    2D array: 
     ndarray([[0.0, 1.0, 2.0, ..., 7.0, 8.0, 9.0],
    	 [0.0, 1.0, 2.0, ..., 7.0, 8.0, 9.0],
    	 [0.0, 1.0, 2.0, ..., 7.0, 8.0, 9.0]], dtype=float)
    shape:  (3, 10)
    size 0:  30 
    size 1:  3 
    size 2:  10
    raw size:  (3, 10, 120, 30, 4)
    
    2D array: 
     ndarray([[0.0, 1.0, 2.0],
    	 [0.0, 1.0, 2.0],
    	 [0.0, 1.0, 2.0]], dtype=float)
    flattened array: (C) ndarray([0.0, 1.0, 2.0, 0.0, 1.0, 2.0, 0.0, 1.0, 2.0], dtype=float)
    flattened array: (F) ndarray([0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0], dtype=float)
    
    

ndarray.h
---------

https://github.com/v923z/micropython-ulab/tree/master/code/ndarray.h

.. code:: cpp
        
    
    #ifndef _NDARRAY_
    #define _NDARRAY_
    
    #include "py/objarray.h"
    #include "py/binary.h"
    #include "py/objstr.h"
    #include "py/objlist.h"
    
    #define PRINT_MAX  10
    
    
    const mp_obj_type_t ulab_ndarray_type;
    
    #define CREATE_SINGLE_ITEM(outarray, type, typecode, value) do {\
        ndarray_obj_t *tmp = create_new_ndarray(1, 1, (typecode));\
        type *tmparr = (type *)tmp->array->items;\
        tmparr[0] = (type)(value);\
        (outarray) = MP_OBJ_FROM_PTR(tmp);\
    } while(0)
    
    /*  
        mp_obj_t row = mp_obj_new_list(n, NULL);
        mp_obj_list_t *row_ptr = MP_OBJ_TO_PTR(row);
        
        should work outside the loop, but it doesn't. Go figure! 
    */
    
    #define RUN_BINARY_LOOP(typecode, type_out, type_left, type_right, ol, or, op) do {\
        type_left *left = (type_left *)(ol)->array->items;\
        type_right *right = (type_right *)(or)->array->items;\
        uint8_t inc = 0;\
        if((or)->array->len > 1) inc = 1;\
        if(((op) == MP_BINARY_OP_ADD) || ((op) == MP_BINARY_OP_SUBTRACT) || ((op) == MP_BINARY_OP_MULTIPLY)) {\
            ndarray_obj_t *out = create_new_ndarray(ol->m, ol->n, typecode);\
            type_out *(odata) = (type_out *)out->array->items;\
            if((op) == MP_BINARY_OP_ADD) { for(size_t i=0, j=0; i < (ol)->array->len; i++, j+=inc) odata[i] = left[i] + right[j];}\
            if((op) == MP_BINARY_OP_SUBTRACT) { for(size_t i=0, j=0; i < (ol)->array->len; i++, j+=inc) odata[i] = left[i] - right[j];}\
            if((op) == MP_BINARY_OP_MULTIPLY) { for(size_t i=0, j=0; i < (ol)->array->len; i++, j+=inc) odata[i] = left[i] * right[j];}\
            return MP_OBJ_FROM_PTR(out);\
        } else if((op) == MP_BINARY_OP_TRUE_DIVIDE) {\
            ndarray_obj_t *out = create_new_ndarray(ol->m, ol->n, NDARRAY_FLOAT);\
            float *odata = (float *)out->array->items;\
            for(size_t i=0, j=0; i < (ol)->array->len; i++, j+=inc) odata[i] = (float)left[i]/(float)right[j];\
            return MP_OBJ_FROM_PTR(out);\
        } else if(((op) == MP_BINARY_OP_LESS) || ((op) == MP_BINARY_OP_LESS_EQUAL) ||  \
                 ((op) == MP_BINARY_OP_MORE) || ((op) == MP_BINARY_OP_MORE_EQUAL)) {\
            mp_obj_t out_list = mp_obj_new_list(0, NULL);\
            size_t m = (ol)->m, n = (ol)->n;\
            for(size_t i=0, r=0; i < m; i++, r+=inc) {\
                mp_obj_t row = mp_obj_new_list(n, NULL);\
                mp_obj_list_t *row_ptr = MP_OBJ_TO_PTR(row);\
                for(size_t j=0, s=0; j < n; j++, s+=inc) {\
                    row_ptr->items[j] = mp_const_false;\
                    if((op) == MP_BINARY_OP_LESS) {\
                        if(left[i*n+j] < right[r*n+s]) row_ptr->items[j] = mp_const_true;\
                    } else if((op) == MP_BINARY_OP_LESS_EQUAL) {\
                        if(left[i*n+j] <= right[r*n+s]) row_ptr->items[j] = mp_const_true;\
                    } else if((op) == MP_BINARY_OP_MORE) {\
                        if(left[i*n+j] > right[r*n+s]) row_ptr->items[j] = mp_const_true;\
                    } else if((op) == MP_BINARY_OP_MORE_EQUAL) {\
                        if(left[i*n+j] >= right[r*n+s]) row_ptr->items[j] = mp_const_true;\
                    }\
                }\
                if(m == 1) return row;\
                mp_obj_list_append(out_list, row);\
            }\
            return out_list;\
        }\
    } while(0)
    
    enum NDARRAY_TYPE {
        NDARRAY_UINT8 = 'B',
        NDARRAY_INT8 = 'b',
        NDARRAY_UINT16 = 'H', 
        NDARRAY_INT16 = 'h',
        NDARRAY_FLOAT = 'f',
    };
    
    typedef struct _ndarray_obj_t {
        mp_obj_base_t base;
        size_t m, n;
        size_t len;
        mp_obj_array_t *array;
        size_t bytes;
    } ndarray_obj_t;
    
    mp_obj_t mp_obj_new_ndarray_iterator(mp_obj_t , size_t , mp_obj_iter_buf_t *);
    
    float ndarray_get_float_value(void *, uint8_t , size_t );
    void fill_array_iterable(float *, mp_obj_t );
    
    void ndarray_print_row(const mp_print_t *, mp_obj_array_t *, size_t , size_t );
    void ndarray_print(const mp_print_t *, mp_obj_t , mp_print_kind_t );
    void ndarray_assign_elements(mp_obj_array_t *, mp_obj_t , uint8_t , size_t *);
    ndarray_obj_t *create_new_ndarray(size_t , size_t , uint8_t );
    
    mp_obj_t ndarray_copy(mp_obj_t );
    mp_obj_t ndarray_make_new(const mp_obj_type_t *, size_t , size_t , const mp_obj_t *);
    mp_obj_t ndarray_subscr(mp_obj_t , mp_obj_t , mp_obj_t );
    mp_obj_t ndarray_getiter(mp_obj_t , mp_obj_iter_buf_t *);
    mp_obj_t ndarray_binary_op(mp_binary_op_t , mp_obj_t , mp_obj_t );
    mp_obj_t ndarray_unary_op(mp_unary_op_t , mp_obj_t );
    
    mp_obj_t ndarray_shape(mp_obj_t );
    mp_obj_t ndarray_size(mp_obj_t , mp_obj_t );
    mp_obj_t ndarray_rawsize(mp_obj_t );
    mp_obj_t ndarray_flatten(size_t , const mp_obj_t *, mp_map_t *);
    
    #endif

ndarray.c
---------

https://github.com/v923z/micropython-ulab/tree/master/code/ndarray.c

.. code:: cpp
        
    
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "py/runtime.h"
    #include "py/binary.h"
    #include "py/obj.h"
    #include "py/objtuple.h"
    #include "ndarray.h"
    
    // This function is copied verbatim from objarray.c
    STATIC mp_obj_array_t *array_new(char typecode, size_t n) {
        int typecode_size = mp_binary_get_size('@', typecode, NULL);
        mp_obj_array_t *o = m_new_obj(mp_obj_array_t);
        // this step could probably be skipped: we are never going to store a bytearray per se
        #if MICROPY_PY_BUILTINS_BYTEARRAY && MICROPY_PY_ARRAY
        o->base.type = (typecode == BYTEARRAY_TYPECODE) ? &mp_type_bytearray : &mp_type_array;
        #elif MICROPY_PY_BUILTINS_BYTEARRAY
        o->base.type = &mp_type_bytearray;
        #else
        o->base.type = &mp_type_array;
        #endif
        o->typecode = typecode;
        o->free = 0;
        o->len = n;
        o->items = m_new(byte, typecode_size * o->len);
        return o;
    }
    
    float ndarray_get_float_value(void *data, uint8_t typecode, size_t index) {
        if(typecode == NDARRAY_UINT8) {
            return (float)((uint8_t *)data)[index];
        } else if(typecode == NDARRAY_INT8) {
            return (float)((int8_t *)data)[index];
        } else if(typecode == NDARRAY_UINT16) {
            return (float)((uint16_t *)data)[index];
        } else if(typecode == NDARRAY_INT16) {
            return (float)((int16_t *)data)[index];
        } else {
            return (float)((float_t *)data)[index];
        }
    }
    
    void fill_array_iterable(float *array, mp_obj_t iterable) {
        mp_obj_iter_buf_t x_buf;
        mp_obj_t x_item, x_iterable = mp_getiter(iterable, &x_buf);
        size_t i=0;
        while ((x_item = mp_iternext(x_iterable)) != MP_OBJ_STOP_ITERATION) {
            array[i] = (float)mp_obj_get_float(x_item);
            i++;
        }
    }
    
    void ndarray_print_row(const mp_print_t *print, mp_obj_array_t *data, size_t n0, size_t n) {
        mp_print_str(print, "[");
        size_t i;
        if(n < PRINT_MAX) { // if the array is short, print everything
            mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0), PRINT_REPR);
            for(i=1; i<n; i++) {
                mp_print_str(print, ", ");
                mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0+i), PRINT_REPR);
            }
        } else {
            mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0), PRINT_REPR);
            for(i=1; i<3; i++) {
                mp_print_str(print, ", ");
                mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0+i), PRINT_REPR);
            }
            mp_printf(print, ", ..., ");
            mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0+n-3), PRINT_REPR);
            for(size_t i=1; i<3; i++) {
                mp_print_str(print, ", ");
                mp_obj_print_helper(print, mp_binary_get_val_array(data->typecode, data->items, n0+n-3+i), PRINT_REPR);
            }
        }
        mp_print_str(print, "]");
    }
    
    void ndarray_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
        (void)kind;
        ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
        mp_print_str(print, "ndarray(");
        if((self->m == 1) || (self->n == 1)) {
            ndarray_print_row(print, self->array, 0, self->array->len);
        } else {
            // TODO: add vertical ellipses for the case, when self->m > PRINT_MAX
            mp_print_str(print, "[");
            ndarray_print_row(print, self->array, 0, self->n);
            for(size_t i=1; i < self->m; i++) {
                mp_print_str(print, ",\n\t ");
                ndarray_print_row(print, self->array, i*self->n, self->n);
            }
            mp_print_str(print, "]");
        }
        // TODO: print typecode
        if(self->array->typecode == NDARRAY_UINT8) {
            printf(", dtype=uint8)");
        } else if(self->array->typecode == NDARRAY_INT8) {
            printf(", dtype=int8)");
        } else if(self->array->typecode == NDARRAY_UINT16) {
            printf(", dtype=uint16)");
        } else if(self->array->typecode == NDARRAY_INT16) {
            printf(", dtype=int16)");
        } else if(self->array->typecode == NDARRAY_FLOAT) {
            printf(", dtype=float)");
        } 
    }
    
    void ndarray_assign_elements(mp_obj_array_t *data, mp_obj_t iterable, uint8_t typecode, size_t *idx) {
        // assigns a single row in the matrix
        mp_obj_t item;
        while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            mp_binary_set_val_array(typecode, data->items, (*idx)++, item);
        }
    }
    
    ndarray_obj_t *create_new_ndarray(size_t m, size_t n, uint8_t typecode) {
        // Creates the base ndarray with shape (m, n), and initialises the values to straight 0s
        ndarray_obj_t *ndarray = m_new_obj(ndarray_obj_t);
        ndarray->base.type = &ulab_ndarray_type;
        ndarray->m = m;
        ndarray->n = n;
        mp_obj_array_t *array = array_new(typecode, m*n);
        ndarray->bytes = m * n * mp_binary_get_size('@', typecode, NULL);
        // this should set all elements to 0, irrespective of the of the typecode (all bits are zero)
        // we could, perhaps, leave this step out, and initialise the array only, when needed
        memset(array->items, 0, ndarray->bytes); 
        ndarray->array = array;
        return ndarray;
    }
    
    mp_obj_t ndarray_copy(mp_obj_t self_in) {
        // returns a verbatim (shape and typecode) copy of self_in
        ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
        ndarray_obj_t *out = create_new_ndarray(self->m, self->n, self->array->typecode);
        memcpy(out->array->items, self->array->items, self->bytes);
        return MP_OBJ_FROM_PTR(out);
    }
    
    STATIC uint8_t ndarray_init_helper(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
            { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = NDARRAY_FLOAT } },
        };
        
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(1, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
        
        uint8_t dtype = args[1].u_int;
        return dtype;
    }
    
    mp_obj_t ndarray_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
        mp_arg_check_num(n_args, n_kw, 1, 2, true);
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        uint8_t dtype = ndarray_init_helper(n_args, args, &kw_args);
    
        size_t len1, len2=0, i=0;
        mp_obj_t len_in = mp_obj_len_maybe(args[0]);
        if (len_in == MP_OBJ_NULL) {
            mp_raise_ValueError("first argument must be an iterable");
        } else {
            len1 = MP_OBJ_SMALL_INT_VALUE(len_in);
        }
    
        // We have to figure out, whether the first element of the iterable is an iterable itself
        // Perhaps, there is a more elegant way of handling this
        mp_obj_iter_buf_t iter_buf1;
        mp_obj_t item1, iterable1 = mp_getiter(args[0], &iter_buf1);
        while ((item1 = mp_iternext(iterable1)) != MP_OBJ_STOP_ITERATION) {
            len_in = mp_obj_len_maybe(item1);
            if(len_in != MP_OBJ_NULL) { // indeed, this seems to be an iterable
                // Next, we have to check, whether all elements in the outer loop have the same length
                if(i > 0) {
                    if(len2 != MP_OBJ_SMALL_INT_VALUE(len_in)) {
                        mp_raise_ValueError("iterables are not of the same length");
                    }
                }
                len2 = MP_OBJ_SMALL_INT_VALUE(len_in);
                i++;
            }
        }
        // By this time, it should be established, what the shape is, so we can now create the array
        ndarray_obj_t *self = create_new_ndarray(len1, (len2 == 0) ? 1 : len2, dtype);
        iterable1 = mp_getiter(args[0], &iter_buf1);
        i = 0;
        if(len2 == 0) { // the first argument is a single iterable
            ndarray_assign_elements(self->array, iterable1, dtype, &i);
        } else {
            mp_obj_iter_buf_t iter_buf2;
            mp_obj_t iterable2; 
    
            while ((item1 = mp_iternext(iterable1)) != MP_OBJ_STOP_ITERATION) {
                iterable2 = mp_getiter(item1, &iter_buf2);
                ndarray_assign_elements(self->array, iterable2, dtype, &i);
            }
        }
        return MP_OBJ_FROM_PTR(self);
    }
    
    mp_obj_t ndarray_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
        // NOTE: this will work only on the flattened array!
        ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
        if (value == MP_OBJ_SENTINEL) { 
            // simply return the values at index, no assignment
            if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
                mp_bound_slice_t slice;
                mp_seq_get_fast_slice_indexes(self->array->len, index, &slice);
                // TODO: this won't work with index reversion!!!
                size_t len = (slice.stop - slice.start) / slice.step;
                ndarray_obj_t *out = create_new_ndarray(1, len, self->array->typecode);
                int _sizeof = mp_binary_get_size('@', self->array->typecode, NULL);
                uint8_t *indata = (uint8_t *)self->array->items;
                uint8_t *outdata = (uint8_t *)out->array->items;
                for(size_t i=0; i < len; i++) {
                    memcpy(outdata+(i*_sizeof), indata+(slice.start+i*slice.step)*_sizeof, _sizeof);
                }
                return MP_OBJ_FROM_PTR(out);
            }
            // we have a single index, return either a single number (arrays), or an array (matrices)
            int16_t idx = mp_obj_get_int(index);
            if(idx < 0) {
                idx = self->m > 1 ? self->m + idx : self->n + idx;
            }
            if(self->m > 1) { // we do have a matrix
                if(idx >= self->m) {
                    mp_raise_ValueError("index is out of range");
                }
                if(self->n == 1) { // the matrix is actually a column vector
                    return mp_binary_get_val_array(self->array->typecode, self->array->items, idx);
                }
                // return an array
                ndarray_obj_t *out = create_new_ndarray(1, self->n, self->array->typecode);
                int _sizeof = mp_binary_get_size('@', self->array->typecode, NULL);
                uint8_t *indata = (uint8_t *)self->array->items;
                uint8_t *outdata = (uint8_t *)out->array->items;
                memcpy(outdata, &indata[idx*self->n*_sizeof], self->n*_sizeof);
                return MP_OBJ_FROM_PTR(out);            
            }
            // since self->m == 1, we have a flat array, hence, we've got to return a single number
            if(idx >= self->n) {
                mp_raise_ValueError("index is out of range");
            }
            return mp_binary_get_val_array(self->array->typecode, self->array->items, idx);
        } else { 
            int16_t idx = mp_obj_get_int(index);
            if((self->m == 1) || (self->n == 1)) {
                if(idx < 0) {
                    idx = self->m > 1 ? self->m + idx : self->n + idx;
                }
                if((idx > self->m) && (idx > self->n)) {
                    mp_raise_ValueError("index is out of range");                
                }
                mp_binary_set_val_array(self->array->typecode, self->array->items, idx, value);
            } else { // do not deal with assignment, bail out, if the array is two-dimensional
                mp_raise_NotImplementedError("subcript assignment is not implemented for 2D arrays");
            }
        }
        return mp_const_none;
    }
    
    // itarray iterator
    
    mp_obj_t ndarray_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf) {
        return mp_obj_new_ndarray_iterator(o_in, 0, iter_buf);
    }
    
    typedef struct _mp_obj_ndarray_it_t {
        mp_obj_base_t base;
        mp_fun_1_t iternext;
        mp_obj_t ndarray;
        size_t cur;
    } mp_obj_ndarray_it_t;
    
    mp_obj_t ndarray_iternext(mp_obj_t self_in) {
        mp_obj_ndarray_it_t *self = MP_OBJ_TO_PTR(self_in);
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(self->ndarray);
        // TODO: in numpy, ndarrays are iterated with respect to the first axis. 
        size_t iter_end = 0;
        if((ndarray->m == 1) || (ndarray->n ==1)) {
            iter_end = ndarray->array->len;
        } else {
            iter_end = ndarray->m;
        }
        if(self->cur < iter_end) {
            if(ndarray->m == ndarray->array->len) { // we have a linear array
                // read the current value
                mp_obj_t value;
                value = mp_binary_get_val_array(ndarray->array->typecode, ndarray->array->items, self->cur);
                self->cur++;
                return value;
            } else { // we have a matrix, return the number of rows
                ndarray_obj_t *value = create_new_ndarray(1, ndarray->n, ndarray->array->typecode);
                // copy the memory content here
                uint8_t *tmp = (uint8_t *)ndarray->array->items;
                size_t strip_size = ndarray->n * mp_binary_get_size('@', ndarray->array->typecode, NULL);
                memcpy(value->array->items, &tmp[self->cur*strip_size], strip_size);
                self->cur++;
                return value;
            }
        } else {
            return MP_OBJ_STOP_ITERATION;
        }
    }
    
    mp_obj_t mp_obj_new_ndarray_iterator(mp_obj_t ndarray, size_t cur, mp_obj_iter_buf_t *iter_buf) {
        assert(sizeof(mp_obj_ndarray_it_t) <= sizeof(mp_obj_iter_buf_t));
        mp_obj_ndarray_it_t *o = (mp_obj_ndarray_it_t*)iter_buf;
        o->base.type = &mp_type_polymorph_iter;
        o->iternext = ndarray_iternext;
        o->ndarray = ndarray;
        o->cur = cur;
        return MP_OBJ_FROM_PTR(o);
    }
    
    mp_obj_t ndarray_shape(mp_obj_t self_in) {
        ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
        mp_obj_t tuple[2] = {
            mp_obj_new_int(self->m),
            mp_obj_new_int(self->n)
        };
        return mp_obj_new_tuple(2, tuple);
    }
    
    mp_obj_t ndarray_size(mp_obj_t self_in, mp_obj_t axis) {
        ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
        uint8_t ax = mp_obj_get_int(axis);
        if(ax == 0) {
            return mp_obj_new_int(self->array->len);
        } else if(ax == 1) {
            return mp_obj_new_int(self->m);
        } else if(ax == 2) {
            return mp_obj_new_int(self->n);
        } else {
            return mp_const_none;
        }
    }
    
    mp_obj_t ndarray_rawsize(mp_obj_t self_in) {
        // returns a 5-tuple with the 
        // 
        // 1. number of rows
        // 2. number of columns
        // 3. length of the storage (should be equal to the product of 1. and 2.)
        // 4. length of the data storage in bytes
        // 5. datum size in bytes
        ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
        mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(5, NULL));
        tuple->items[0] = MP_OBJ_NEW_SMALL_INT(self->m);
        tuple->items[1] = MP_OBJ_NEW_SMALL_INT(self->n);
        tuple->items[2] = MP_OBJ_NEW_SMALL_INT(self->bytes);
        tuple->items[3] = MP_OBJ_NEW_SMALL_INT(self->array->len);
        tuple->items[4] = MP_OBJ_NEW_SMALL_INT(mp_binary_get_size('@', self->array->typecode, NULL));
        return tuple;
    }
    
    mp_obj_t ndarray_flatten(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_order, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_QSTR(MP_QSTR_C)} },
        };
    
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
        mp_obj_t self_copy = ndarray_copy(pos_args[0]);
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(self_copy);
        
        GET_STR_DATA_LEN(args[0].u_obj, order, len);    
        if((len != 1) || ((memcmp(order, "C", 1) != 0) && (memcmp(order, "F", 1) != 0))) {
            mp_raise_ValueError("flattening order must be either 'C', or 'F'");        
        }
    
        // if order == 'C', we simply have to set m, and n, there is nothing else to do
        if(memcmp(order, "F", 1) == 0) {
            ndarray_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
            uint8_t _sizeof = mp_binary_get_size('@', self->array->typecode, NULL);
            // get the data of self_in: we won't need a temporary buffer for the transposition
            uint8_t *self_array = (uint8_t *)self->array->items;
            uint8_t *array = (uint8_t *)ndarray->array->items;
            size_t i=0;
            for(size_t n=0; n < self->n; n++) {
                for(size_t m=0; m < self->m; m++) {
                    memcpy(array+_sizeof*i, self_array+_sizeof*(m*self->n + n), _sizeof);
                    i++;
                }
            }        
        }
        ndarray->n = ndarray->array->len;
        ndarray->m = 1;
        return self_copy;
    }
    
    // Binary operations
    
    mp_obj_t ndarray_binary_op(mp_binary_op_t op, mp_obj_t lhs, mp_obj_t rhs) {
        // One of the operands is a scalar
        // TODO: conform to numpy with the upcasting
        // TODO: implement in-place operators
        mp_obj_t RHS = MP_OBJ_NULL;
        bool rhs_is_scalar = true;
        if(mp_obj_is_int(rhs)) {
            size_t ivalue = mp_obj_get_int(rhs);
            if((ivalue > 0) && (ivalue < 256)) {
                CREATE_SINGLE_ITEM(RHS, uint8_t, NDARRAY_UINT8, ivalue);
            }
            else if((ivalue > 0) && (ivalue < 65535)) {
                CREATE_SINGLE_ITEM(RHS, uint16_t, NDARRAY_UINT16, ivalue);
            }
            else if((ivalue < 0) && (ivalue > -128)) {
                CREATE_SINGLE_ITEM(RHS, int8_t, NDARRAY_INT8, ivalue);
            }
            else if((ivalue < 0) && (ivalue > -32767)) {
                CREATE_SINGLE_ITEM(RHS, int16_t, NDARRAY_INT16, ivalue);
            }
        } else if(mp_obj_is_float(rhs)) {
            float fvalue = mp_obj_get_float(rhs);        
            CREATE_SINGLE_ITEM(RHS, float, NDARRAY_FLOAT, fvalue);
        } else {
            RHS = rhs;
            rhs_is_scalar = false;
        }
        //else 
        if(mp_obj_is_type(lhs, &ulab_ndarray_type) && mp_obj_is_type(RHS, &ulab_ndarray_type)) { 
            // next, the ndarray stuff
            ndarray_obj_t *ol = MP_OBJ_TO_PTR(lhs);
            ndarray_obj_t *or = MP_OBJ_TO_PTR(RHS);
            if(!rhs_is_scalar && ((ol->m != or->m) || (ol->n != or->n))) {
                mp_raise_ValueError("operands could not be broadcast together");
            }
            // At this point, the operands should have the same shape
            switch(op) {
                case MP_BINARY_OP_EQUAL:
                    // Two arrays are equal, if their shape, typecode, and elements are equal
                    if((ol->m != or->m) || (ol->n != or->n) || (ol->array->typecode != or->array->typecode)) {
                        return mp_const_false;
                    } else {
                        size_t i = ol->bytes;
                        uint8_t *l = (uint8_t *)ol->array->items;
                        uint8_t *r = (uint8_t *)or->array->items;
                        while(i) { // At this point, we can simply compare the bytes, the type is irrelevant
                            if(*l++ != *r++) {
                                return mp_const_false;
                            }
                            i--;
                        }
                        return mp_const_true;
                    }
                    break;
                case MP_BINARY_OP_LESS:
                case MP_BINARY_OP_LESS_EQUAL:
                case MP_BINARY_OP_MORE:
                case MP_BINARY_OP_MORE_EQUAL:
                case MP_BINARY_OP_ADD:
                case MP_BINARY_OP_SUBTRACT:
                case MP_BINARY_OP_TRUE_DIVIDE:
                case MP_BINARY_OP_MULTIPLY:
                    // These are the upcasting rules
                    // float always becomes float
                    // operation on identical types preserves type
                    // uint8 + int8 => int16
                    // uint8 + int16 => int16
                    // uint8 + uint16 => uint16
                    // int8 + int16 => int16
                    // int8 + uint16 => uint16
                    // uint16 + int16 => float
                    // The parameters of RUN_BINARY_LOOP are 
                    // typecode of result, type_out, type_left, type_right, lhs operand, rhs operand, operator
                    if(ol->array->typecode == NDARRAY_UINT8) {
                        if(or->array->typecode == NDARRAY_UINT8) {
                            RUN_BINARY_LOOP(NDARRAY_UINT8, uint8_t, uint8_t, uint8_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_INT8) {
                            RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, uint8_t, int8_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_UINT16) {
                            RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint8_t, uint16_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_INT16) {
                            RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, uint8_t, int16_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_FLOAT) {
                            RUN_BINARY_LOOP(NDARRAY_FLOAT, float, uint8_t, float, ol, or, op);
                        }
                    } else if(ol->array->typecode == NDARRAY_INT8) {
                        if(or->array->typecode == NDARRAY_UINT8) {
                            RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int8_t, uint8_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_INT8) {
                            RUN_BINARY_LOOP(NDARRAY_INT8, int8_t, int8_t, int8_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_UINT16) {
                            RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int8_t, uint16_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_INT16) {
                            RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int8_t, int16_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_FLOAT) {
                            RUN_BINARY_LOOP(NDARRAY_FLOAT, float, int8_t, float, ol, or, op);
                        }                
                    } else if(ol->array->typecode == NDARRAY_UINT16) {
                        if(or->array->typecode == NDARRAY_UINT8) {
                            RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, uint8_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_INT8) {
                            RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, int8_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_UINT16) {
                            RUN_BINARY_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, uint16_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_INT16) {
                            RUN_BINARY_LOOP(NDARRAY_FLOAT, float, uint16_t, int16_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_FLOAT) {
                            RUN_BINARY_LOOP(NDARRAY_FLOAT, float, uint8_t, float, ol, or, op);
                        }
                    } else if(ol->array->typecode == NDARRAY_INT16) {
                        if(or->array->typecode == NDARRAY_UINT8) {
                            RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int16_t, uint8_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_INT8) {
                            RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int16_t, int8_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_UINT16) {
                            RUN_BINARY_LOOP(NDARRAY_FLOAT, float, int16_t, uint16_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_INT16) {
                            RUN_BINARY_LOOP(NDARRAY_INT16, int16_t, int16_t, int16_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_FLOAT) {
                            RUN_BINARY_LOOP(NDARRAY_FLOAT, float, uint16_t, float, ol, or, op);
                        }
                    } else if(ol->array->typecode == NDARRAY_FLOAT) {
                        if(or->array->typecode == NDARRAY_UINT8) {
                            RUN_BINARY_LOOP(NDARRAY_FLOAT, float, float, uint8_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_INT8) {
                            RUN_BINARY_LOOP(NDARRAY_FLOAT, float, float, int8_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_UINT16) {
                            RUN_BINARY_LOOP(NDARRAY_FLOAT, float, float, uint16_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_INT16) {
                            RUN_BINARY_LOOP(NDARRAY_FLOAT, float, float, int16_t, ol, or, op);
                        } else if(or->array->typecode == NDARRAY_FLOAT) {
                            RUN_BINARY_LOOP(NDARRAY_FLOAT, float, float, float, ol, or, op);
                        }
                    } else { // this should never happen
                        mp_raise_TypeError("wrong input type");
                    }
                    // this instruction should never be reached, but we have to make the compiler happy
                    return MP_OBJ_NULL; 
                default:
                    return MP_OBJ_NULL; // op not supported                                                        
            }
        } else {
            mp_raise_TypeError("wrong operand type on the right hand side");
        }
    }
    
    mp_obj_t ndarray_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
        ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
        ndarray_obj_t *ndarray = NULL;
        switch (op) {
            case MP_UNARY_OP_LEN: 
                if(self->m > 1) {
                    return mp_obj_new_int(self->m);
                } else {
                    return mp_obj_new_int(self->len);
                }
                break;
            
            case MP_UNARY_OP_INVERT:
                if(self->array->typecode == NDARRAY_FLOAT) {
                    mp_raise_ValueError("operation is not supported for given type");
                }
                // we can invert the content byte by byte, there is no need to distinguish 
                // between different typecodes
                ndarray = MP_OBJ_TO_PTR(ndarray_copy(self_in));
                uint8_t *array = (uint8_t *)ndarray->array->items;
                for(size_t i=0; i < self->bytes; i++) array[i] = ~array[i];
                return MP_OBJ_FROM_PTR(ndarray);
                break;
            
            case MP_UNARY_OP_NEGATIVE:
                ndarray = MP_OBJ_TO_PTR(ndarray_copy(self_in));
                if(self->array->typecode == NDARRAY_UINT8) {
                    uint8_t *array = (uint8_t *)ndarray->array->items;
                    for(size_t i=0; i < self->array->len; i++) array[i] = -array[i];
                } else if(self->array->typecode == NDARRAY_INT8) {
                    int8_t *array = (int8_t *)ndarray->array->items;
                    for(size_t i=0; i < self->array->len; i++) array[i] = -array[i];
                } else if(self->array->typecode == NDARRAY_UINT16) {                
                    uint16_t *array = (uint16_t *)ndarray->array->items;
                    for(size_t i=0; i < self->array->len; i++) array[i] = -array[i];
                } else if(self->array->typecode == NDARRAY_INT16) {
                    int16_t *array = (int16_t *)ndarray->array->items;
                    for(size_t i=0; i < self->array->len; i++) array[i] = -array[i];
                } else {
                    float *array = (float *)ndarray->array->items;
                    for(size_t i=0; i < self->array->len; i++) array[i] = -array[i];
                }
                return MP_OBJ_FROM_PTR(ndarray);
                break;
    
            case MP_UNARY_OP_POSITIVE:
                return ndarray_copy(self_in);
    
            case MP_UNARY_OP_ABS:
                if((self->array->typecode == NDARRAY_UINT8) || (self->array->typecode == NDARRAY_UINT16)) {
                    return ndarray_copy(self_in);
                }
                ndarray = MP_OBJ_TO_PTR(ndarray_copy(self_in));
                if((self->array->typecode == NDARRAY_INT8)) {
                    int8_t *array = (int8_t *)ndarray->array->items;
                    for(size_t i=0; i < self->array->len; i++) {
                        if(array[i] < 0) array[i] = -array[i];
                    }
                } else if((self->array->typecode == NDARRAY_INT16)) {
                    int16_t *array = (int16_t *)ndarray->array->items;
                    for(size_t i=0; i < self->array->len; i++) {
                        if(array[i] < 0) array[i] = -array[i];
                    }
                } else {
                    float *array = (float *)ndarray->array->items;
                    for(size_t i=0; i < self->array->len; i++) {
                        if(array[i] < 0) array[i] = -array[i];
                    }                
                }
                return MP_OBJ_FROM_PTR(ndarray);
                break;
            default: return MP_OBJ_NULL; // operator not supported
        }
    }

ipython3.. code ::
        
    a = array([1, 2, 3, 4])
    a < a[2]




.. parsed-literal::

    array([ True,  True, False, False])



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([1, -2, 3], dtype=ulab.int8)
    
    print(abs(a))
    
    a = ulab.ndarray([1, 2, 3], dtype=ulab.uint8)
    print(~a)
    
    a = ulab.ndarray([1, 2, 3], dtype=ulab.int8)
    print(~a)
.. parsed-literal::

    ndarray([1, 2, 3], dtype=int8)
    ndarray([254, 253, 252], dtype=uint8)
    ndarray([-2, -3, -4], dtype=int8)
    
    

ipython3.. code ::
        
    a = array([1, -2, 3], dtype=int8)
    print(-a, +a)
    
    a = array([1, 2, 3], dtype=uint8)
    print(-a, +a)
    
    a = array([1, 2, -3], dtype=float)
    print(-a, +a)
.. parsed-literal::

    [-1  2 -3] [ 1 -2  3]
    [255 254 253] [1 2 3]
    [-1. -2.  3.] [ 1.  2. -3.]

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([1, -2, 3], dtype=ulab.int8)
    print(-a, +a)
    
    a = ulab.ndarray([1, 2, 3], dtype=ulab.uint8)
    print(-a, +a)
    
    a = ulab.ndarray([1, 2, -3], dtype=ulab.float)
    print(-a, +a)
.. parsed-literal::

    ndarray([-1, 2, -3], dtype=int8) ndarray([1, -2, 3], dtype=int8)
    ndarray([255, 254, 253], dtype=uint8) ndarray([1, 2, 3], dtype=uint8)
    ndarray([-1.0, -2.0, 3.0], dtype=float) ndarray([1.0, 2.0, -3.0], dtype=float)
    
    

ipython3.. code ::
        
    a = array([1, 2, 3], dtype=int8)
    
    print(~a)
.. parsed-literal::

    [-2 -3 -4]

ipython3.. code ::
        
    a = array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
ipython3.. code ::
        
    a[0, 1]




.. parsed-literal::

    2



Linear algebra
==============

This module contains very basic matrix operators, such as transposing,
reshaping, inverting, and matrix multiplication. The actual inversion is
factored out into a helper function, so that the routine can be re-used
in other modules. (The ``polyfit`` function in ``poly.c`` uses that.)
Also note that inversion is based on the notion of a *small number*
(epsilon). During the computation of the inverse, a number is treated as
0, if its absolute value is smaller than epsilon. This precaution is
required, otherwise, one might run into singular matrices.

As in ``numpy``, ``inv`` is not a class method, but it should be applied
only on ``ndarray``\ s. This is why one has to check the argument type
at the beginning of the functions.

Examples
--------

Transpose of one- and two-dimensional arrays, .transpose()
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ipython3.. code ::
        
    %%micropython -unix 1
    
    from ulab import ndarray
    
    a = ndarray(range(10))
    print('1D array: ', a)
    print('shape of a: ', a.shape())
    
    a.transpose()
    print('\ntranspose of array: ', a)
    print('shape of a: ', a.shape())
    
    
    a = ndarray([[1, 2, 3, 4], [5, 6, 7, 8]])
    print('\n2D array: \n', a)
    print('shape of a: ', a.shape())
    
    a.transpose()
    print('\ntranspose of array: \n', a)
    print('shape of a: ', a.shape())
.. parsed-literal::

    1D array:  ndarray([0.0, 1.0, 2.0, ..., 7.0, 8.0, 9.0], dtype=float)
    shape of a:  (10, 1)
    
    transpose of array:  ndarray([0.0, 1.0, 2.0, ..., 7.0, 8.0, 9.0], dtype=float)
    shape of a:  (1, 10)
    
    2D array: 
     ndarray([[1.0, 2.0, 3.0, 4.0],
    	 [5.0, 6.0, 7.0, 8.0]], dtype=float)
    shape of a:  (2, 4)
    
    transpose of array: 
     ndarray([[1.0, 5.0],
    	 [2.0, 6.0],
    	 [3.0, 7.0],
    	 [4.0, 8.0]], dtype=float)
    shape of a:  (4, 2)
    
    

.reshape()
~~~~~~~~~~

ipython3.. code ::
        
    %%micropython -unix 1
    
    from ulab import ndarray
    
    a = ndarray(range(15))
    print('1D array: ', a)
    print('shape of a: ', a.shape())
    
    a.reshape((3, 5))
    print('\n2D array: \n', a)
    print('shape of a: ', a.shape())
.. parsed-literal::

    1D array:  ndarray([0.0, 1.0, 2.0, ..., 12.0, 13.0, 14.0], dtype=float)
    shape of a:  (15, 1)
    
    2D array: 
     ndarray([[0.0, 1.0, 2.0, 3.0, 4.0],
    	 [5.0, 6.0, 7.0, 8.0, 9.0],
    	 [10.0, 11.0, 12.0, 13.0, 14.0]], dtype=float)
    shape of a:  (3, 5)
    
    

inverse of a matrix (inv)
~~~~~~~~~~~~~~~~~~~~~~~~~

ipython3.. code ::
        
    %%micropython -unix 1
    
    from ulab import ndarray, inv
    
    a = ndarray([[1, 2], [3, 4]])
    print('2D matrix (a): \n', a)
    b = inv(a)
    print('\ninverse of a: \n', b)
.. parsed-literal::

    2D matrix (a): 
     ndarray([[1.0, 2.0],
    	 [3.0, 4.0]], dtype=float)
    
    inverse of a: 
     ndarray([[-2.0, 1.0],
    	 [1.5, -0.5]], dtype=float)
    
    

matrix multiplication (dot)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

With the ``dot`` function, we can now check, whether the inverse of the
matrix was correct:

ipython3.. code ::
        
    %%micropython -unix 1
    
    from ulab import ndarray, inv, dot
    
    
    a = ndarray([[1, 2], [3, 4]])
    print('2D matrix (a): \n', a)
    b = inv(a)
    print('\ninverse of a: \n', b)
    
    c = dot(a, b)
    print('\na multiplied by its inverse: \n', c)
.. parsed-literal::

    2D matrix (a): 
     ndarray([[1.0, 2.0],
    	 [3.0, 4.0]], dtype=float)
    
    inverse of a: 
     ndarray([[-2.0, 1.0],
    	 [1.5, -0.5]], dtype=float)
    
    a multiplied by its inverse: 
     ndarray([[1.0, 0.0],
    	 [0.0, 1.0]], dtype=float)
    
    

zeros, ones, eye
~~~~~~~~~~~~~~~~

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    print(ulab.zeros(3, dtype=ulab.int16))
    print(ulab.zeros((5, 3), dtype=ulab.float))
    
    print("\n====================\n");
    print(ulab.ones(3, dtype=ulab.int16))
    print(ulab.ones((5, 3), dtype=ulab.float))
    
    print("\n====================\n");
    print(ulab.eye(5, dtype=ulab.int16))
    print(ulab.eye(5, M=3, dtype=ulab.float))
    
    print(ulab.eye(5, k=1, dtype=ulab.uint8))
    print(ulab.eye(5, k=-3, dtype=ulab.uint8))
.. parsed-literal::

    ndarray([0, 0, 0], dtype=int16)
    ndarray([[0.0, 0.0, 0.0],
    	 [0.0, 0.0, 0.0],
    	 [0.0, 0.0, 0.0],
    	 [0.0, 0.0, 0.0],
    	 [0.0, 0.0, 0.0]], dtype=float)
    
    ====================
    
    ndarray([1, 1, 1], dtype=int16)
    ndarray([[1.0, 1.0, 1.0],
    	 [1.0, 1.0, 1.0],
    	 [1.0, 1.0, 1.0],
    	 [1.0, 1.0, 1.0],
    	 [1.0, 1.0, 1.0]], dtype=float)
    
    ====================
    
    ndarray([[1, 0, 0, 0, 0],
    	 [0, 1, 0, 0, 0],
    	 [0, 0, 1, 0, 0],
    	 [0, 0, 0, 1, 0],
    	 [0, 0, 0, 0, 1]], dtype=int16)
    ndarray([[1.0, 0.0, 0.0, 0.0, 0.0],
    	 [0.0, 1.0, 0.0, 0.0, 0.0],
    	 [0.0, 0.0, 1.0, 0.0, 0.0]], dtype=float)
    ndarray([[0, 1, 0, 0, 0],
    	 [0, 0, 1, 0, 0],
    	 [0, 0, 0, 1, 0],
    	 [0, 0, 0, 0, 1],
    	 [0, 0, 0, 0, 0]], dtype=uint8)
    ndarray([[0, 0, 0, 0, 0],
    	 [0, 0, 0, 0, 0],
    	 [0, 0, 0, 0, 0],
    	 [1, 0, 0, 0, 0],
    	 [0, 1, 0, 0, 0]], dtype=uint8)
    
    

linalg.h
--------

https://github.com/v923z/micropython-ulab/tree/master/code/linalg.h

.. code:: cpp
        
    
    #ifndef _LINALG_
    #define _LINALG_
    
    #include "ndarray.h"
    
    #define SWAP(t, a, b) { t tmp = a; a = b; b = tmp; }
    #define epsilon        1e-6
    
    mp_obj_t linalg_transpose(mp_obj_t );
    mp_obj_t linalg_reshape(mp_obj_t , mp_obj_t );
    bool linalg_invert_matrix(float *, size_t );
    mp_obj_t linalg_inv(mp_obj_t );
    mp_obj_t linalg_dot(mp_obj_t , mp_obj_t );
    mp_obj_t linalg_zeros(size_t , const mp_obj_t *, mp_map_t *);
    mp_obj_t linalg_ones(size_t , const mp_obj_t *, mp_map_t *);
    mp_obj_t linalg_eye(size_t , const mp_obj_t *, mp_map_t *);
    
    #endif

linalg.c
--------

https://github.com/v923z/micropython-ulab/tree/master/code/linalg.c

.. code:: cpp
        
    
    #include <stdlib.h>
    #include <string.h>
    #include "py/obj.h"
    #include "py/runtime.h"
    #include "py/misc.h"
    #include "linalg.h"
    
    mp_obj_t linalg_transpose(mp_obj_t self_in) {
        ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
        // the size of a single item in the array
        uint8_t _sizeof = mp_binary_get_size('@', self->array->typecode, NULL);
        
        // NOTE: In principle, we could simply specify the stride direction, and then we wouldn't 
        // even have to shuffle the elements. The downside of that approach is that we would have 
        // to implement two versions of the matrix multiplication and inversion functions
        
        // NOTE: 
        // if the matrices are square, we can simply swap items, but 
        // generic matrices can't be transposed in place, so we have to 
        // declare a temporary variable
        
        // NOTE: 
        //  In the old matrix, the coordinate (m, n) is m*self->n + n
        //  We have to assign this to the coordinate (n, m) in the new 
        //  matrix, i.e., to n*self->m + m
        
        // one-dimensional arrays can be transposed by simply swapping the dimensions
        if((self->m != 1) && (self->n != 1)) {
            uint8_t *c = (uint8_t *)self->array->items;
            // self->bytes is the size of the bytearray, irrespective of the typecode
            uint8_t *tmp = m_new(uint8_t, self->bytes);
            for(size_t m=0; m < self->m; m++) {
                for(size_t n=0; n < self->n; n++) {
                    memcpy(tmp+_sizeof*(n*self->m + m), c+_sizeof*(m*self->n + n), _sizeof);
                }
            }
            memcpy(self->array->items, tmp, self->bytes);
            m_del(uint8_t, tmp, self->bytes);
        } 
        SWAP(size_t, self->m, self->n);
        return mp_const_none;
    }
    
    mp_obj_t linalg_reshape(mp_obj_t self_in, mp_obj_t shape) {
        ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
        if(!MP_OBJ_IS_TYPE(shape, &mp_type_tuple) || (MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(shape)) != 2)) {
            mp_raise_ValueError("shape must be a 2-tuple");
        }
    
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t item, iterable = mp_getiter(shape, &iter_buf);
        uint16_t m, n;
        item = mp_iternext(iterable);
        m = mp_obj_get_int(item);
        item = mp_iternext(iterable);
        n = mp_obj_get_int(item);
        if(m*n != self->m*self->n) {
            // TODO: the proper error message would be "cannot reshape array of size %d into shape (%d, %d)"
            mp_raise_ValueError("cannot reshape array (incompatible input/output shape)");
        }
        self->m = m;
        self->n = n;
        return MP_OBJ_FROM_PTR(self);
    }
    
    bool linalg_invert_matrix(float *data, size_t N) {
        // returns true, of the inversion was successful, 
        // false, if the matrix is singular
        
        // initially, this is the unit matrix: the contents of this matrix is what 
        // will be returned after all the transformations
        float *unit = m_new(float, N*N);
    
        float elem = 1.0;
        // initialise the unit matrix
        memset(unit, 0, sizeof(float)*N*N);
        for(size_t m=0; m < N; m++) {
            memcpy(&unit[m*(N+1)], &elem, sizeof(float));
        }
        for(size_t m=0; m < N; m++){
            // this could be faster with ((c < epsilon) && (c > -epsilon))
            if(abs(data[m*(N+1)]) < epsilon) {
                m_del(float, unit, N*N);
                return false;
            }
            for(size_t n=0; n < N; n++){
                if(m != n){
                    elem = data[N*n+m] / data[m*(N+1)];
                    for(size_t k=0; k < N; k++){
                        data[N*n+k] -= elem * data[N*m+k];
                        unit[N*n+k] -= elem * unit[N*m+k];
                    }
                }
            }
        }
        for(size_t m=0; m < N; m++){ 
            elem = data[m*(N+1)];
            for(size_t n=0; n < N; n++){
                data[N*m+n] /= elem;
                unit[N*m+n] /= elem;
            }
        }
        memcpy(data, unit, sizeof(float)*N*N);
        m_del(float, unit, N*N);
        return true;
    }
    
    mp_obj_t linalg_inv(mp_obj_t o_in) {
        // since inv is not a class method, we have to inspect the input argument first
        if(!MP_OBJ_IS_TYPE(o_in, &ulab_ndarray_type)) {
            mp_raise_TypeError("only ndarrays can be inverted");
        }
        ndarray_obj_t *o = MP_OBJ_TO_PTR(o_in);
        if(!MP_OBJ_IS_TYPE(o_in, &ulab_ndarray_type)) {
            mp_raise_TypeError("only ndarray objects can be inverted");
        }
        if(o->m != o->n) {
            mp_raise_ValueError("only square matrices can be inverted");
        }
        ndarray_obj_t *inverted = create_new_ndarray(o->m, o->n, NDARRAY_FLOAT);
        float *data = (float *)inverted->array->items;
        mp_obj_t elem;
        for(size_t m=0; m < o->m; m++) { // rows first
            for(size_t n=0; n < o->n; n++) { // columns next
                // this could, perhaps, be done in single line... 
                // On the other hand, we probably spend little time here
                elem = mp_binary_get_val_array(o->array->typecode, o->array->items, m*o->n+n);
                data[m*o->n+n] = (float)mp_obj_get_float(elem);
            }
        }
        
        if(!linalg_invert_matrix(data, o->m)) {
            // TODO: I am not sure this is needed here. Otherwise, 
            // how should we free up the unused RAM of inverted?
            m_del(float, inverted->array->items, o->n*o->n);
            mp_raise_ValueError("input matrix is singular");
        }
        return MP_OBJ_FROM_PTR(inverted);
    }
    
    mp_obj_t linalg_dot(mp_obj_t _m1, mp_obj_t _m2) {
        // TODO: should the results be upcast?
        ndarray_obj_t *m1 = MP_OBJ_TO_PTR(_m1);
        ndarray_obj_t *m2 = MP_OBJ_TO_PTR(_m2);    
        if(m1->n != m2->m) {
            mp_raise_ValueError("matrix dimensions do not match");
        }
        ndarray_obj_t *out = create_new_ndarray(m1->m, m2->n, NDARRAY_FLOAT);
        float *outdata = (float *)out->array->items;
        float sum, v1, v2;
        for(size_t i=0; i < m1->n; i++) {
            for(size_t j=0; j < m2->m; j++) {
                sum = 0.0;
                for(size_t k=0; k < m1->m; k++) {
                    // (j, k) * (k, j)
                    v1 = ndarray_get_float_value(m1->array->items, m1->array->typecode, i*m1->n+k);
                    v2 = ndarray_get_float_value(m2->array->items, m2->array->typecode, k*m2->n+j);
                    sum += v1 * v2;
                }
                outdata[i*m1->m+j] = sum;
            }
        }
        return MP_OBJ_FROM_PTR(out);
    }
    
    mp_obj_t linalg_zeros_ones(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args, uint8_t kind) {
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} } ,
            { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = NDARRAY_FLOAT} },
        };
    
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
        
        uint8_t dtype = args[1].u_int;
        if(!mp_obj_is_int(args[0].u_obj) && !mp_obj_is_type(args[0].u_obj, &mp_type_tuple)) {
            mp_raise_TypeError("input argument must be an integer or a 2-tuple");
        }
        ndarray_obj_t *ndarray = NULL;
        if(mp_obj_is_int(args[0].u_obj)) {
            size_t n = mp_obj_get_int(args[0].u_obj);
            ndarray = create_new_ndarray(1, n, dtype);
        } else if(mp_obj_is_type(args[0].u_obj, &mp_type_tuple)) {
            mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(args[0].u_obj);
            if(tuple->len != 2) {
                mp_raise_TypeError("input argument must be an integer or a 2-tuple");            
            }
            ndarray = create_new_ndarray(mp_obj_get_int(tuple->items[0]), 
                                                      mp_obj_get_int(tuple->items[1]), dtype);
        }
        if(kind == 1) {
            mp_obj_t one = mp_obj_new_int(1);
            for(size_t i=0; i < ndarray->array->len; i++) {
                mp_binary_set_val_array(dtype, ndarray->array->items, i, one);
            }
        }
        return MP_OBJ_FROM_PTR(ndarray);
    }
    
    mp_obj_t linalg_zeros(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        return linalg_zeros_ones(n_args, pos_args, kw_args, 0);
    }
    
    mp_obj_t linalg_ones(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        return linalg_zeros_ones(n_args, pos_args, kw_args, 1);
    }
    
    mp_obj_t linalg_eye(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
            { MP_QSTR_M, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) } },
            { MP_QSTR_k, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },        
            { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = NDARRAY_FLOAT} },
        };
    
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
        size_t n = args[0].u_int, m;
        int16_t k = args[2].u_int;
        uint8_t dtype = args[3].u_int;
        if(args[1].u_rom_obj == mp_const_none) {
            m = n;
        } else {
            m = mp_obj_get_int(args[1].u_rom_obj);
        }
        
        ndarray_obj_t *ndarray = create_new_ndarray(m, n, dtype);
        mp_obj_t one = mp_obj_new_int(1);
        size_t i = 0;
        if((k >= 0) && (k < n)) {
            while(k < n) {
                mp_binary_set_val_array(dtype, ndarray->array->items, i*n+k, one);
                k++;
                i++;
            }
        } else if((k < 0) && (-k < m)) {
            k = -k;
            i = 0;
            while(k < m) {
                mp_binary_set_val_array(dtype, ndarray->array->items, k*n+i, one);
                k++;
                i++;
            }
        }
        return MP_OBJ_FROM_PTR(ndarray);
    }

Vectorising mathematical operations
===================================

General comments
----------------

The following module implements the common mathematical functions for
scalars, ndarrays (linear or matrix), and iterables. If the input
argument is a scalar, a scalar is returned (i.e., for such arguments,
these functions are identical to the functions in the ``math`` module),
while for ndarrays, and iterables, the return value is an ndarray of
type ``float``.

Examples
--------

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    # initialise an array
    a = ulab.ndarray([1, 2, 3, 4, 5])
    print('1D array: ', a)
    
    print('\nexponent of an array (range(5)): ', ulab.exp(range(5)))
    
    print('\nexponent of a scalar (2.0): ', ulab.exp(2.0))
    
    print('\n exponent of a 1D ndarray (a): ', ulab.exp(a))
    
    # initialise a matrix
    b = ulab.ndarray([[1, 2, 3], [4, 5, 6]])
    print('\n2D matrix: ', b)
    print('exponent of a 2D matrix (b): ', ulab.exp(b))
.. parsed-literal::

    1D array:  ndarray([1.0, 2.0, 3.0, 4.0, 5.0], dtype=float)
    
    exponent of an array (range(5)):  ndarray([1.0, 2.718281745910645, 7.389056205749512, 20.08553695678711, 54.59814834594727], dtype=float)
    
    exponent of a scalar (2.0):  7.38905609893065
    
     exponent of a 1D ndarray (a):  ndarray([2.718281745910645, 7.389056205749512, 20.08553695678711, 54.59814834594727, 148.4131622314453], dtype=float)
    
    2D matrix:  ndarray([[1.0, 2.0, 3.0],
    	 [4.0, 5.0, 6.0]], dtype=float)
    exponent of a 2D matrix (b):  ndarray([[2.718281745910645, 7.389056205749512, 20.08553695678711],
    	 [54.59814834594727, 148.4131622314453, 403.4288024902343]], dtype=float)
    
    

Note that ndarrays are linear arrays in memory, even if the ``shape`` of
the ndarray is a matrix. This means that we can treat both cases in a
*single* loop.

Since ``ndarray``\ s are iterable, we could treat ``ndarray``\ s,
``list``\ s, ``tuples``, and ``range``\ s on the same footing. However,
that would mean extra trips to a lot of functions, therefore, reading
out the values of the ``ndarray`` directly is probably significantly
faster.

vectorise.h
-----------

https://github.com/v923z/micropython-ulab/tree/master/code/vectorise.h

.. code:: cpp
        
    
    #ifndef _VECTORISE_
    #define _VECTORISE_
    
    #include "ndarray.h"
    
    mp_obj_t vectorise_acos(mp_obj_t );
    mp_obj_t vectorise_acosh(mp_obj_t );
    mp_obj_t vectorise_asin(mp_obj_t );
    mp_obj_t vectorise_asinh(mp_obj_t );
    mp_obj_t vectorise_atan(mp_obj_t );
    mp_obj_t vectorise_atanh(mp_obj_t );
    mp_obj_t vectorise_ceil(mp_obj_t );
    mp_obj_t vectorise_cos(mp_obj_t );
    mp_obj_t vectorise_erf(mp_obj_t );
    mp_obj_t vectorise_erfc(mp_obj_t );
    mp_obj_t vectorise_exp(mp_obj_t );
    mp_obj_t vectorise_expm1(mp_obj_t );
    mp_obj_t vectorise_floor(mp_obj_t );
    mp_obj_t vectorise_gamma(mp_obj_t );
    mp_obj_t vectorise_lgamma(mp_obj_t );
    mp_obj_t vectorise_log(mp_obj_t );
    mp_obj_t vectorise_log10(mp_obj_t );
    mp_obj_t vectorise_log2(mp_obj_t );
    mp_obj_t vectorise_sin(mp_obj_t );
    mp_obj_t vectorise_sinh(mp_obj_t );
    mp_obj_t vectorise_sqrt(mp_obj_t );
    mp_obj_t vectorise_tan(mp_obj_t );
    mp_obj_t vectorise_tanh(mp_obj_t );
    
    #endif

vectorise.c
-----------

https://github.com/v923z/micropython-ulab/tree/master/code/vectorise.c

.. code:: cpp
        
    
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include "py/runtime.h"
    #include "py/binary.h"
    #include "py/obj.h"
    #include "py/objarray.h"
    #include "vectorise.h"
    
    #ifndef MP_PI
    #define MP_PI MICROPY_FLOAT_CONST(3.14159265358979323846)
    #endif
    
    mp_obj_t vectorise_generic_vector(mp_obj_t o_in, mp_float_t (*f)(mp_float_t)) {
        // Return a single value, if o_in is not iterable
        if(mp_obj_is_float(o_in) || mp_obj_is_integer(o_in)) {
                return mp_obj_new_float(f(mp_obj_get_float(o_in)));
        }
        mp_float_t x;
        if(MP_OBJ_IS_TYPE(o_in, &ulab_ndarray_type)) {
            ndarray_obj_t *o = MP_OBJ_TO_PTR(o_in);
            ndarray_obj_t *out = create_new_ndarray(o->m, o->n, NDARRAY_FLOAT);
            float *datain = (float *)o->array->items;
            float *dataout = (float *)out->array->items;
            for(size_t i=0; i < o->array->len; i++) {
                dataout[i] = f(datain[i]);
            }
            return MP_OBJ_FROM_PTR(out);
        } else if(MP_OBJ_IS_TYPE(o_in, &mp_type_tuple) || MP_OBJ_IS_TYPE(o_in, &mp_type_list) || 
            MP_OBJ_IS_TYPE(o_in, &mp_type_range)) {
                mp_obj_array_t *o = MP_OBJ_TO_PTR(o_in);
                ndarray_obj_t *out = create_new_ndarray(1, o->len, NDARRAY_FLOAT);
                float *dataout = (float *)out->array->items;
                mp_obj_iter_buf_t iter_buf;
                mp_obj_t item, iterable = mp_getiter(o_in, &iter_buf);
                size_t i=0;
                while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
                    x = mp_obj_get_float(item);
                    dataout[i++] = f(x);
                }
            return MP_OBJ_FROM_PTR(out);
        }
        return mp_const_none;
    }
    
    
    #define MATH_FUN_1(py_name, c_name) \
        mp_obj_t vectorise_ ## py_name(mp_obj_t x_obj) { \
            return vectorise_generic_vector(x_obj, MICROPY_FLOAT_C_FUN(c_name)); \
        }
    
    // _degrees won't compile for the unix port
    /*
    mp_float_t _degreesf(mp_float_t x) {
        return(180*x/MP_PI);
    }
    
    MATH_FUN_1(degrees, _degrees);
    
    // _radians won't compile for the unix port
    mp_float_t _radiansf(mp_float_t x) {
        return(MP_PI*x/180.0);
    }
    
    MATH_FUN_1(radians, _radians);
    
    STATIC mp_float_t _fabsf(mp_float_t x) {
        return fabsf(x);
    }
    
    MATH_FUN_1(fabs, _fabs);
    */
    MATH_FUN_1(acos, acos);
    MATH_FUN_1(acosh, acosh);
    MATH_FUN_1(asin, asin);
    MATH_FUN_1(asinh, asinh);
    MATH_FUN_1(atan, atan);	
    MATH_FUN_1(atanh, atanh);
    MATH_FUN_1(ceil, ceil);
    MATH_FUN_1(cos, cos);
    MATH_FUN_1(erf, erf);
    MATH_FUN_1(erfc, erfc);
    MATH_FUN_1(exp, exp);
    MATH_FUN_1(expm1, expm1);
    MATH_FUN_1(floor, floor);
    MATH_FUN_1(gamma, tgamma);
    MATH_FUN_1(lgamma, lgamma);
    MATH_FUN_1(log, log);
    MATH_FUN_1(log10, log10);
    MATH_FUN_1(log2, log2);
    MATH_FUN_1(sin, sin);
    MATH_FUN_1(sinh, sinh);
    MATH_FUN_1(sqrt, sqrt);
    MATH_FUN_1(tan, tan);
    MATH_FUN_1(tanh, tanh);

Polynomials
===========

This module has two functions, ``polyval``, and ``polyfit``. The
background for ``polyfit`` can be found under
https://en.wikipedia.org/wiki/Polynomial_regression, and one can take
the matrix inversion function from ``linalg``.

Background
----------

An estimate, :math:`\beta`, for the coefficients of a polynomial fit can
be gotten from :raw-latex:`\begin{equation}
\vec{\beta} = (\mathbf{X}^T\mathbf{X})^{-1}\mathbf{X}^T \vec{y}
\end{equation}` where :math:`\vec{y}` are the dependent values, and the
matrix :math:`X` is constructed from the independent values as
:raw-latex:`\begin{equation}
X = \begin{pmatrix}
1 & x_1^2 & x_1^2 & ... & x_1^m 
\\
1 & x_2^2 & x_2^2 & ... & x_2^m 
\\
\vdots & \vdots & \vdots & \ddots & \vdots 
\\
1 & x_n^2 & x_n^2 & ... & x_n^m 
\end{pmatrix}
\end{equation}`

Note that the calculation of :math:`X^T` is trivial, and we need
:math:`X` only once, namely in the product :math:`X^TX`. We will save
RAM by storing only :math:`X^T`, and expressing :math:`X` from
:math:`X^T`, when we need it. The routine calculates the coefficients in
increasing order, therefore, before returning, we have to reverse the
array.

Examples
--------

polyval
~~~~~~~

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    p = [1, 1, 1, 0]
    x = [0, 1, 2, 3, 4]
    print('coefficients: ', p)
    print('independent values: ', x)
    print('\nvalues of p(x): ', ulab.polyval(p, x))
    
    # the same works with ndarrays
    a = ulab.ndarray(x)
    print('\nndarray (a): ', a)
    print('value of p(a): ', ulab.polyval(p, a))
.. parsed-literal::

    coefficients:  [1, 1, 1, 0]
    independent values:  [0, 1, 2, 3, 4]
    
    values of p(x):  ndarray([0.0, 3.0, 14.0, 39.0, 84.0], dtype=float)
    
    ndarray (a):  ndarray([0.0, 1.0, 2.0, 3.0, 4.0], dtype=float)
    value of p(a):  ndarray([0.0, 3.0, 14.0, 39.0, 84.0], dtype=float)
    
    

polyfit
~~~~~~~

First a perfect parabola with zero shift, and leading coefficient of 1.

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    x = ulab.ndarray([-3, -2, -1, 0, 1, 2, 3])
    y = ulab.ndarray([9, 4, 1, 0, 1, 4, 9])
    print('independent values: ', x)
    print('dependent values: ', y)
    
    print('fit values', ulab.polyfit(x, y, 2))
.. parsed-literal::

    independent values:  ndarray([-3.0, -2.0, -1.0, 0.0, 1.0, 2.0, 3.0], dtype=float)
    dependent values:  ndarray([9.0, 4.0, 1.0, 0.0, 1.0, 4.0, 9.0], dtype=float)
    fit values ndarray([1.00000011920929, 0.0, 0.0], dtype=float)
    
    

We can now take a more meaningful example: the data points scatter here:

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    x = ulab.ndarray([-3, -2, -1, 0, 1, 2, 3])
    y = ulab.ndarray([10, 5, 1, 0, 1, 4.2, 9.1])
    print('independent values: ', x)
    print('dependent values: ', y)
    
    print('fit values', ulab.polyfit(x, y, 2))
.. parsed-literal::

    independent values:  ndarray([-3.0, -2.0, -1.0, 0.0, 1.0, 2.0, 3.0], dtype=float)
    dependent values:  ndarray([10.0, 5.0, 1.0, 0.0, 1.0, 4.199999809265137, 9.100000381469727], dtype=float)
    fit values ndarray([1.065476179122925, -0.1535714119672775, 0.06666660308837891], dtype=float)
    
    

Finally, let us see, what this looks like in numpy:

ipython3.. code ::
        
    x = array([-3, -2, -1, 0, 1, 2, 3])
    y = array([10, 5, 1, 0, 1, 4.2, 9.1])
    print('independent values: ', x)
    print('dependent values: ', y)
    
    print('fit values: ', polyfit(x, y, 2))
.. parsed-literal::

    independent values:  [-3 -2 -1  0  1  2  3]
    dependent values:  [10.   5.   1.   0.   1.   4.2  9.1]
    fit values:  [ 1.06547619 -0.15357143  0.06666667]

Look at that!

poly.h
------

https://github.com/v923z/micropython-ulab/tree/master/code/poly.h

.. code:: cpp
        
    
    #ifndef _POLY_
    #define _POLY_
    
    mp_obj_t poly_polyval(mp_obj_t , mp_obj_t );
    mp_obj_t poly_polyfit(size_t  , const mp_obj_t *);
    
    #endif

poly.c
------

https://github.com/v923z/micropython-ulab/tree/master/code/poly.c

.. code:: cpp
        
    
    #include "py/obj.h"
    #include "py/runtime.h"
    #include "py/objarray.h"
    #include "ndarray.h"
    #include "linalg.h"
    #include "poly.h"
    
    
    bool object_is_nditerable(mp_obj_t o_in) {
        if(mp_obj_is_type(o_in, &ulab_ndarray_type) || 
          mp_obj_is_type(o_in, &mp_type_tuple) || 
          mp_obj_is_type(o_in, &mp_type_list) || 
          mp_obj_is_type(o_in, &mp_type_range)) {
            return true;
        }
        return false;
    }
    
    size_t get_nditerable_len(mp_obj_t o_in) {
        if(mp_obj_is_type(o_in, &ulab_ndarray_type)) {
            ndarray_obj_t *in = MP_OBJ_TO_PTR(o_in);
            return in->array->len;
        } else {
            return (size_t)mp_obj_get_int(mp_obj_len_maybe(o_in));
        }
    }
    
    mp_obj_t poly_polyval(mp_obj_t o_p, mp_obj_t o_x) {
        // TODO: return immediately, if o_p is not an iterable
        size_t m, n;
        if(MP_OBJ_IS_TYPE(o_x, &ulab_ndarray_type)) {
            ndarray_obj_t *ndx = MP_OBJ_TO_PTR(o_x);
            m = ndx->m;
            n = ndx->n;
        } else {
            mp_obj_array_t *ix = MP_OBJ_TO_PTR(o_x);
            m = 1;
            n = ix->len;
        }
        // polynomials are going to be of type float, except, when both 
        // the coefficients and the independent variable are integers
        ndarray_obj_t *out = create_new_ndarray(m, n, NDARRAY_FLOAT);
        mp_obj_iter_buf_t x_buf;
        mp_obj_t x_item, x_iterable = mp_getiter(o_x, &x_buf);
    
        mp_obj_iter_buf_t p_buf;
        mp_obj_t p_item, p_iterable;
    
        mp_float_t x, y;
        float *outf = (float *)out->array->items;
        uint8_t plen = mp_obj_get_int(mp_obj_len_maybe(o_p));
        float *p = m_new(float, plen);
        p_iterable = mp_getiter(o_p, &p_buf);
        uint16_t i = 0;    
        while((p_item = mp_iternext(p_iterable)) != MP_OBJ_STOP_ITERATION) {
            p[i] = (float)mp_obj_get_float(p_item);
            i++;
        }
        i = 0;
        while ((x_item = mp_iternext(x_iterable)) != MP_OBJ_STOP_ITERATION) {
            x = mp_obj_get_float(x_item);
            y = p[0];
            for(uint8_t j=0; j < plen-1; j++) {
                y *= x;
                y += p[j+1];
            }
            outf[i++] = y;
        }
        m_del(float, p, plen);
        return MP_OBJ_FROM_PTR(out);
    }
    
    mp_obj_t poly_polyfit(size_t  n_args, const mp_obj_t *args) {
        if((n_args != 2) && (n_args != 3)) {
            mp_raise_ValueError("number of arguments must be 2, or 3");
        }
        if(!object_is_nditerable(args[0])) {
            mp_raise_ValueError("input data must be an iterable");
        }
        uint16_t lenx, leny;
        uint8_t deg;
        float *x, *XT, *y, *prod;
    
        if(n_args == 2) { // only the y values are supplied
            // TODO: this is actually not enough: the first argument can very well be a matrix, 
            // in which case we are between the rock and a hard place
            leny = (uint16_t)mp_obj_get_int(mp_obj_len_maybe(args[0]));
            deg = (uint8_t)mp_obj_get_int(args[1]);
            if(leny < deg) {
                mp_raise_ValueError("more degrees of freedom than data points");
            }
            lenx = leny;
            x = m_new(float, lenx); // assume uniformly spaced data points
            for(size_t i=0; i < lenx; i++) {
                x[i] = i;
            }
            y = m_new(float, leny);
            fill_array_iterable(y, args[0]);
        } else if(n_args == 3) {
            lenx = (uint16_t)mp_obj_get_int(mp_obj_len_maybe(args[0]));
            leny = (uint16_t)mp_obj_get_int(mp_obj_len_maybe(args[0]));
            if(lenx != leny) {
                mp_raise_ValueError("input vectors must be of equal length");
            }
            deg = (uint8_t)mp_obj_get_int(args[2]);
            if(leny < deg) {
                mp_raise_ValueError("more degrees of freedom than data points");
            }
            x = m_new(float, lenx);
            fill_array_iterable(x, args[0]);
            y = m_new(float, leny);
            fill_array_iterable(y, args[1]);
        }
        
        // one could probably express X as a function of XT, 
        // and thereby save RAM, because X is used only in the product
        XT = m_new(float, (deg+1)*leny); // XT is a matrix of shape (deg+1, len) (rows, columns)
        for(uint8_t i=0; i < leny; i++) { // column index
            XT[i+0*lenx] = 1.0; // top row
            for(uint8_t j=1; j < deg+1; j++) { // row index
                XT[i+j*leny] = XT[i+(j-1)*leny]*x[i];
            }
        }
        
        prod = m_new(float, (deg+1)*(deg+1)); // the product matrix is of shape (deg+1, deg+1)
        float sum;
        for(uint16_t i=0; i < deg+1; i++) { // column index
            for(uint16_t j=0; j < deg+1; j++) { // row index
                sum = 0.0;
                for(size_t k=0; k < lenx; k++) {
                    // (j, k) * (k, i) 
                    // Note that the second matrix is simply the transpose of the first: 
                    // X(k, i) = XT(i, k) = XT[k*lenx+i]
                    sum += XT[j*lenx+k]*XT[i*lenx+k]; // X[k*(deg+1)+i];
                }
                prod[j*(deg+1)+i] = sum;
            }
        }
        if(!linalg_invert_matrix(prod, deg+1)) {
            // Although X was a Vandermonde matrix, whose inverse is guaranteed to exist, 
            // we bail out here, if prod couldn't be inverted: if the values in x are not all 
            // distinct, prod is singular
            m_del(float, XT, (deg+1)*lenx);
            m_del(float, x, lenx);
            m_del(float, y, lenx);
            m_del(float, prod, (deg+1)*(deg+1));
            mp_raise_ValueError("could not invert Vandermonde matrix");
        } 
        // at this point, we have the inverse of X^T * X
        // y is a column vector; x is free now, we can use it for storing intermediate values
        for(uint16_t i=0; i < deg+1; i++) { // row index
            sum = 0.0;
            for(uint16_t j=0; j < lenx; j++) { // column index
                sum += XT[i*lenx+j]*y[j];
            }
            x[i] = sum;
        }
        // XT is no longer needed
        m_del(float, XT, (deg+1)*leny);
        
        ndarray_obj_t *beta = create_new_ndarray(deg+1, 1, NDARRAY_FLOAT);
        float *betav = (float *)beta->array->items;
        // x[0..(deg+1)] contains now the product X^T * y; we can get rid of y
        m_del(float, y, leny);
        
        // now, we calculate beta, i.e., we apply prod = (X^T * X)^(-1) on x = X^T * y; x is a column vector now
        for(uint16_t i=0; i < deg+1; i++) {
            sum = 0.0;
            for(uint16_t j=0; j < deg+1; j++) {
                sum += prod[i*(deg+1)+j]*x[j];
            }
            betav[i] = sum;
        }
        m_del(float, x, lenx);
        m_del(float, prod, (deg+1)*(deg+1));
        for(uint8_t i=0; i < (deg+1)/2; i++) {
            // We have to reverse the array, for the leading coefficient comes first. 
            SWAP(float, betav[i], betav[deg-i]);
        }
        return MP_OBJ_FROM_PTR(beta);
    }

Fast Fourier transform
======================

The original idea of the implementation of the fast Fourier transform is
taken from Numerical recipes. The main modification is that the present
FFT kernel requires two input vectors of float type: one for the real
part, and one for the imaginary part, while in Numerical recipes, the
real and imaginary parts occupy alternating positions in the same array.

However, since ``ndarray`` cannot hold complex types, it makes sense to
starts with two separate vectors. This is especially true for our
particular case, since the data are most probably real, coming from an
ADC or similar. By separating the real and imaginary parts at the very
beginning, we can process *real* data by not providing the imaginary
part. If only one argument is supplied, it is assumed to be real, and
the imaginary part is automatically filled in.

Now, the implementation computes the transform in place. This means that
RAM space could be saved, if the old data are not required anymore. The
problem, however, is that the results are of type float, irrespective of
the input type. If one can somehow guarantee that the input type is also
float, then the old data can be overwritten. This is what happens in the
``spectrum`` function that overwrites the input array.

Examples
--------

Full FFT
~~~~~~~~

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([0, 1, 2, 3, 0, 1, 2, 3])
    re, im = ulab.fft(a)
    print('real part: ', re)
    print('imag part: ', im)
.. parsed-literal::

    real part:  ndarray([12.0, 0.0, -3.999999761581421, 0.0, -4.0, 0.0, -4.0, 0.0], dtype=float)
    imag part:  ndarray([0.0, 0.0, 3.999999523162842, 0.0, 0.0, 0.0, -3.999999523162842, 0.0], dtype=float)
    
    

The same Fourier transform on numpy:

ipython3.. code ::
        
    fft.fft([0, 1, 2, 3, 0, 1, 2, 3])




.. parsed-literal::

    array([12.+0.j,  0.+0.j, -4.+4.j,  0.+0.j, -4.+0.j,  0.+0.j, -4.-4.j,
            0.+0.j])



Spectrum
~~~~~~~~

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([0, 1, 2, 3, 0, 1, 2, 3])
    ulab.spectrum(a)
    print(a)
.. parsed-literal::

    ndarray([12.0, 0.0, 5.656853675842285, 0.0, 4.0, 0.0, 5.656853675842285, 0.0], dtype=float)
    
    

And watch this: if you need the spectrum, but do not want to overwrite
your data, you can do the following

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([0, 1, 2, 3, 0, 1, 2, 3])
    re, im = ulab.fft(a)
    print('spectrum: ', ulab.sqrt(re*re+im*im))
.. parsed-literal::

    spectrum:  ndarray([12.0, 0.0, 5.656853675842285, 0.0, 4.0, 0.0, 5.656853675842285, 0.0], dtype=float)
    
    

fft.h
-----

https://github.com/v923z/micropython-ulab/tree/master/code/fft.h

.. code:: cpp
        
    
    #ifndef _FFT_
    #define _FFT_
    
    #ifndef MP_PI
    #define MP_PI MICROPY_FLOAT_CONST(3.14159265358979323846)
    #endif
    
    #define SWAP(t, a, b) { t tmp = a; a = b; b = tmp; }
    
    mp_obj_t fft_fft(size_t , const mp_obj_t *);
    mp_obj_t fft_spectrum(mp_obj_t );
    #endif

fft.c
-----

https://github.com/v923z/micropython-ulab/tree/master/code/fft.c

.. code:: cpp
        
    
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "py/runtime.h"
    #include "py/binary.h"
    #include "py/obj.h"
    #include "py/objarray.h"
    #include "ndarray.h"
    #include "fft.h"
    
    void fft_kernel(float *real, float *imag, int n, int isign) {
        // This is basically a modification of four1 from Numerical Recipes
        // The main difference is that this function takes two arrays, one 
        // for the real, and one for the imaginary parts. 
        int j, m, mmax, istep;
        float tempr, tempi;
        float wtemp, wr, wpr, wpi, wi, theta;
    
        j = 0;
        for(int i = 0; i < n; i++) {
            if (j > i) {
                SWAP(float, real[i], real[j]);
                SWAP(float, imag[i], imag[j]);            
            }
            m = n >> 1;
            while (j >= m && m > 0) {
                j -= m;
                m >>= 1;
            }
            j += m;
        }
    
        mmax = 1;
        while (n > mmax) {
            istep = mmax << 1;
            theta = -1.0*isign*6.28318530717959/istep;
            wtemp = sinf(0.5 * theta);
            wpr = -2.0 * wtemp * wtemp;
            wpi = sinf(theta);
            wr = 1.0;
            wi = 0.0;
            for(m = 0; m < mmax; m++) {
                for(int i = m; i < n; i += istep) {
                    j = i + mmax;
                    tempr = wr * real[j] - wi * imag[j];
                    tempi = wr * imag[j] + wi * real[j];
                    real[j] = real[i] - tempr;
                    imag[j] = imag[i] - tempi;
                    real[i] += tempr;
                    imag[i] += tempi;
                }
                wtemp = wr;
                wr = wr*wpr - wi*wpi + wr;
                wi = wi*wpr + wtemp*wpi + wi;
            }
            mmax = istep;
        }
    }
    
    mp_obj_t fft_fft(size_t n_args, const mp_obj_t *args) {
        // TODO: return the absolute value, if keyword argument is specified
        // TODO: transform the data in place, if keyword argument is specified
        if(!MP_OBJ_IS_TYPE(args[0], &ulab_ndarray_type)) {
            mp_raise_NotImplementedError("FFT is defined for ndarrays only");
        } 
        if(n_args == 2) {
            if(!MP_OBJ_IS_TYPE(args[1], &ulab_ndarray_type)) {
                mp_raise_NotImplementedError("FFT is defined for ndarrays only");
            }
        }
        // Check if input is of length of power of 2
        ndarray_obj_t *re = MP_OBJ_TO_PTR(args[0]);
        uint16_t len = re->array->len;
        if((len & (len-1)) != 0) {
            mp_raise_ValueError("input array length must be power of 2");
        }
        
        ndarray_obj_t *out_re = create_new_ndarray(1, len, NDARRAY_FLOAT);
        float *data_re = (float *)out_re->array->items;
        
        if(re->array->typecode == NDARRAY_FLOAT) {
            memcpy((float *)out_re->array->items, (float *)re->array->items, re->bytes);
        } else {
            for(size_t i=0; i < len; i++) {
                data_re[i] = ndarray_get_float_value(re->array->items, re->array->typecode, i);
            }
        }
        ndarray_obj_t *out_im = create_new_ndarray(1, len, NDARRAY_FLOAT);
        float *data_im = (float *)out_im->array->items;
    
        if(n_args == 2) {
            ndarray_obj_t *im = MP_OBJ_TO_PTR(args[1]);
            if (re->array->len != im->array->len) {
                mp_raise_ValueError("real and imaginary parts must be of equal length");
            }
            if(im->array->typecode == NDARRAY_FLOAT) {
                memcpy((float *)out_im->array->items, (float *)im->array->items, im->bytes);
            } else {
                for(size_t i=0; i < len; i++) {
                    data_im[i] = ndarray_get_float_value(im->array->items, im->array->typecode, i);
                }
            }
        }    
        fft_kernel(data_re, data_im, len, 1);
        mp_obj_t tuple[2];
        tuple[0] = out_re;
        tuple[1] = out_im;
        return mp_obj_new_tuple(2, tuple);
    }
    
    mp_obj_t fft_spectrum(mp_obj_t oin) {
        // calculates the the spectrum of a single real ndarray in place
        if(!MP_OBJ_IS_TYPE(oin, &ulab_ndarray_type)) {
            mp_raise_NotImplementedError("FFT is defined for ndarrays only");
        }
        ndarray_obj_t *re = MP_OBJ_TO_PTR(oin);
        uint16_t len = re->array->len;
        if((re->m > 1) && (re->n > 1)) {
            mp_raise_ValueError("input data must be an array");
        }
        if((len & (len-1)) != 0) {
            mp_raise_ValueError("input array length must be power of 2");
        }
        if(re->array->typecode != NDARRAY_FLOAT) {
            mp_raise_TypeError("input array must be of type float");
        }
        float *data_re = (float *)re->array->items;
        float *data_im = m_new(float, len);
        fft_kernel(data_re, data_im, len, 1);
        for(size_t i=0; i < len; i++) {
            data_re[i] = sqrtf(data_re[i]*data_re[i] + data_im[i]*data_im[i]);
        }
        m_del(float, data_im, len);
        return mp_const_none;
    }

Numerical
=========

General comments
----------------

This section contains miscellaneous functions that did not fit in the
other submodules. These include ``linspace``, ``min/max``,
``argmin/argmax``, ``sum``, ``mean``, ``std``. These latter functions
work with iterables, or ndarrays. When the ndarray is two-dimensional,
an ``axis`` keyword can be supplied, in which case, the function returns
a vector, otherwise a scalar.

Since the return values of ``mean``, and ``std`` are most probably
floats, these functions return ndarrays of type float, while ``min/max``
and ``clip`` do not change the type, and ``argmin/argmax`` return
``uint8``, if the values are smaller than 255, otherwise, ``uint16``.

roll
~~~~

Note that at present, arrays are always rolled to the left, even when
the user specifies right. The reason for that is inner working of
``memcpy``: one can shift contiguous chunks to the left only. If one
tries to shift to the right, then the same value will be written into
the new array over and over again.

Examples
--------

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    print(ulab.linspace(0, 10, 11))
    print(ulab.sum([1, 2, 3]))
    
    a = ulab.ndarray([[1, 2, 3, 4, 5], [6, 7, 8, 9, 10], [55, 66, 77, 88, 99]], dtype=ulab.int8)
    print(a)
    ulab.roll(a, -1, axis=0)
    print(a)
    ulab.roll(a, 1, axis=1)
    print(a)
.. parsed-literal::

    ndarray([0.0, 1.0, 2.0, ..., 8.0, 9.0, 10.0], dtype=float)
    6.0
    ndarray([[1, 2, 3, 4, 5],
    	 [6, 7, 8, 9, 10],
    	 [55, 66, 77, 88, 99]], dtype=int8)
    ndarray([[55, 66, 77, 88, 99],
    	 [1, 2, 3, 4, 5],
    	 [6, 7, 8, 9, 10]], dtype=int8)
    ndarray([[66, 77, 88, 99, 55],
    	 [2, 3, 4, 5, 1],
    	 [7, 8, 9, 10, 6]], dtype=int8)
    
    

ipython3.. code ::
        
    linspace(0, 10, 11, endpoint=False, dtype=int8, retstep=True)




.. parsed-literal::

    (array([0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9], dtype=int8), 0.9090909090909091)



ipython3.. code ::
        
    a = array([0, 1, 2, 3], dtype=float)
    abs(a), -a




.. parsed-literal::

    (array([0., 1., 2., 3.]), array([-0., -1., -2., -3.]))



ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    print(ulab.linspace(0, 10, num=11, endpoint=True, retstep=True, dtype=ulab.int8))
    print(ulab.linspace(0, 10, num=11, endpoint=False, retstep=True, dtype=ulab.int16))
.. parsed-literal::

    (ndarray([0, 1, 2, ..., 8, 9, 10], dtype=int8), 1.0)
    (ndarray([0, 0, 1, ..., 7, 8, 9], dtype=int16), 0.9090909361839294)
    
    

ipython3.. code ::
        
    %%micropython -unix 1
    
    import ulab
    
    a = ulab.ndarray([0, 1, 2, -3], dtype=ulab.float)
    print(abs(a))
.. parsed-literal::

    ndarray([0.0, 1.0, 2.0, 3.0], dtype=float)
    
    

numerical.h
-----------

https://github.com/v923z/micropython-ulab/tree/master/code/numerical.h

.. code:: cpp
        
    
    #ifndef _NUMERICAL_
    #define _NUMERICAL_
    
    #include "ndarray.h"
    
    mp_obj_t numerical_linspace(size_t , const mp_obj_t *, mp_map_t *);
    mp_obj_t numerical_sum(size_t , const mp_obj_t *, mp_map_t *);
    mp_obj_t numerical_mean(size_t , const mp_obj_t *, mp_map_t *);
    mp_obj_t numerical_std(size_t , const mp_obj_t *, mp_map_t *);
    mp_obj_t numerical_min(size_t , const mp_obj_t *, mp_map_t *);
    mp_obj_t numerical_max(size_t , const mp_obj_t *, mp_map_t *);
    mp_obj_t numerical_argmin(size_t , const mp_obj_t *, mp_map_t *);
    mp_obj_t numerical_argmax(size_t , const mp_obj_t *, mp_map_t *);
    mp_obj_t numerical_roll(size_t , const mp_obj_t *, mp_map_t *);
    
    #endif

numerical.c
-----------

Parsing of arguments
~~~~~~~~~~~~~~~~~~~~

Since most of these functions operate on matrices along an axis, it
might make sense to factor out the parsing of arguments and keyword
arguments. The void function ``numerical_parse_args`` fills in the
pointer for the matrix/array, and the axis.

https://github.com/v923z/micropython-ulab/tree/master/code/numerical.c

.. code:: cpp
        
    
    #include <math.h>
    #include <stdlib.h>
    #include <string.h>
    #include "py/obj.h"
    #include "py/runtime.h"
    #include "py/builtin.h"
    #include "py/misc.h"
    #include "numerical.h"
    
    enum NUMERICAL_FUNCTION_TYPE {
        NUMERICAL_MIN,
        NUMERICAL_MAX,
        NUMERICAL_ARGMIN,
        NUMERICAL_ARGMAX,
        NUMERICAL_SUM,
        NUMERICAL_MEAN,
        NUMERICAL_STD,
    };
    
    mp_obj_t numerical_linspace(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) } },
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) } },
            { MP_QSTR_num, MP_ARG_INT, {.u_int = 50} },
            { MP_QSTR_endpoint, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_true_obj)} },
            { MP_QSTR_retstep, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_false_obj)} },
            { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = NDARRAY_FLOAT} },
        };
    
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(2, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
        uint16_t len = args[2].u_int;
        if(len < 2) {
            mp_raise_ValueError("number of points must be at least 2");
        }
        float value, step;
        value = mp_obj_get_float(args[0].u_obj);
        uint8_t typecode = args[5].u_int;
        if(args[3].u_obj == mp_const_true) step = (mp_obj_get_float(args[1].u_obj)-value)/(len-1);
        else step = (mp_obj_get_float(args[1].u_obj)-value)/len;
        ndarray_obj_t *ndarray = create_new_ndarray(1, len, typecode);
        if(typecode == NDARRAY_UINT8) {
            uint8_t *array = (uint8_t *)ndarray->array->items;
            for(size_t i=0; i < len; i++, value += step) array[i] = (uint8_t)value;
        } else if(typecode == NDARRAY_INT8) {
            int8_t *array = (int8_t *)ndarray->array->items;
            for(size_t i=0; i < len; i++, value += step) array[i] = (int8_t)value;
        } else if(typecode == NDARRAY_UINT16) {
            uint16_t *array = (uint16_t *)ndarray->array->items;
            for(size_t i=0; i < len; i++, value += step) array[i] = (uint16_t)value;
        } else if(typecode == NDARRAY_INT16) {
            int16_t *array = (int16_t *)ndarray->array->items;
            for(size_t i=0; i < len; i++, value += step) array[i] = (int16_t)value;
        } else {
            float *array = (float *)ndarray->array->items;
            for(size_t i=0; i < len; i++, value += step) array[i] = value;
        }
        if(args[4].u_obj == mp_const_false) {
            return MP_OBJ_FROM_PTR(ndarray);
        } else {
            mp_obj_t tuple[2];
            tuple[0] = ndarray;
            tuple[1] = mp_obj_new_float(step);
            return mp_obj_new_tuple(2, tuple);
        }
    }
    
    mp_obj_t numerical_sum_mean_std_array(mp_obj_t oin, uint8_t optype) {
        mp_float_t value, sum = 0.0, sq_sum = 0.0;
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t item, iterable = mp_getiter(oin, &iter_buf);
        mp_int_t len = mp_obj_get_int(mp_obj_len(oin));
        while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            value = mp_obj_get_float(item);
            sum += value;
            if(optype == NUMERICAL_STD) {
                sq_sum += value*value;
            }
        }
        if(optype ==  NUMERICAL_SUM) {
            return mp_obj_new_float(sum);
        } else if(optype == NUMERICAL_MEAN) {
            return mp_obj_new_float(sum/len);
        } else {
            sum /= len; // this is now the mean!
            return mp_obj_new_float(sqrtf((sq_sum/len-sum*sum)));
        }
    }
    
    STATIC mp_obj_t numerical_argmin_argmax_array(mp_obj_t o_in, mp_uint_t op, uint8_t type) {
        size_t idx = 0, best_idx = 0;
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable = mp_getiter(o_in, &iter_buf);
        mp_obj_t best_obj = MP_OBJ_NULL;
        mp_obj_t item;
        while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            if ((best_obj == MP_OBJ_NULL) || (mp_binary_op(op, item, best_obj) == mp_const_true)) {
                best_obj = item;
                best_idx = idx;
            }
            idx++;
        }
        if((type == NUMERICAL_ARGMIN) || (type == NUMERICAL_ARGMAX)) {
            return MP_OBJ_NEW_SMALL_INT(best_idx);
        } else {
            return best_obj;
        }
    }
    
    STATIC size_t numerical_argmin_argmax_single_line(void *data, size_t start, size_t stop, 
                                                      size_t stride, uint8_t typecode, uint8_t optype) {
        size_t best_idx = start;
        mp_float_t value, best_value = ndarray_get_float_value(data, typecode, start);
        
        for(size_t i=start; i < stop; i+=stride) {
            value = ndarray_get_float_value(data, typecode, i);
            if((optype == NUMERICAL_MIN) || (optype == NUMERICAL_ARGMIN)) {
                if(best_value > value) {
                    best_value = value;
                    best_idx = i;
                }
            } else if((optype == NUMERICAL_MAX) || (optype == NUMERICAL_ARGMAX)) {
                if(best_value < value) {
                    best_value = value;
                    best_idx = i;
                }
            }
        }
        return best_idx;
    }
    
    STATIC mp_obj_t numerical_argmin_argmax_matrix(mp_obj_t oin, mp_obj_t axis, uint8_t optype) {
        ndarray_obj_t *in = MP_OBJ_TO_PTR(oin);
        size_t best_idx;
        if((axis == mp_const_none) || (in->m == 1) || (in->n == 1)) { 
            // return the value for the flattened array
            best_idx = numerical_argmin_argmax_single_line(in->array->items, 0, 
                                                          in->array->len, 1, in->array->typecode, optype);
            if((optype == NUMERICAL_ARGMIN) || (optype == NUMERICAL_ARGMAX)) {
                return MP_OBJ_NEW_SMALL_INT(best_idx);
            } else {
                // TODO: do we have to do type conversion here, depending on the type of the input array?
                return mp_obj_new_float(ndarray_get_float_value(in->array->items, in->array->typecode, best_idx));
            }
        } else {
            uint8_t _axis = mp_obj_get_int(axis);
            size_t m = (_axis == 0) ? 1 : in->m;
            size_t n = (_axis == 0) ? in->n : 1;
            size_t len = in->array->len;
            // TODO: pass in->array->typcode to create_new_ndarray
            ndarray_obj_t *out = create_new_ndarray(m, n, NDARRAY_FLOAT);
    
            // TODO: these two cases could probably be combined in a more elegant fashion...
            if(_axis == 0) { // vertical
                for(size_t i=0; i < n; i++) {
                    best_idx = numerical_argmin_argmax_single_line(in->array->items, i, len, 
                                                                   n, in->array->typecode, optype);
                    if((optype == NUMERICAL_ARGMIN) || (optype == NUMERICAL_ARGMAX)) {
                        ((float_t *)out->array->items)[i] = (float)best_idx;
                    } else {
                        ((float_t *)out->array->items)[i] = ndarray_get_float_value(in->array->items, in->array->typecode, best_idx);
                    }
                }
            } else { // horizontal
                for(size_t i=0; i < m; i++) {
                    best_idx = numerical_argmin_argmax_single_line(in->array->items, i*in->n, 
                                                                   (i+1)*in->n, 1, in->array->typecode, optype);
                    if((optype == NUMERICAL_ARGMIN) || (optype == NUMERICAL_ARGMAX)) {
                        ((float_t *)out->array->items)[i] = (float)best_idx;
                    } else {
                        ((float_t *)out->array->items)[i] = ndarray_get_float_value(in->array->items, in->array->typecode, best_idx);
                    }
    
                }
            }
        return MP_OBJ_FROM_PTR(out);
        }
        return mp_const_none;
    }
    
    STATIC mp_float_t numerical_sum_mean_std_single_line(void *data, size_t start, size_t stop, 
                                                      size_t stride, uint8_t typecode, uint8_t optype) {
        
        mp_float_t sum = 0.0, sq_sum = 0.0, value;
        size_t len = 0;
        for(size_t i=start; i < stop; i+=stride, len++) {
            value = ndarray_get_float_value(data, typecode, i);        
            sum += value;
            if(optype == NUMERICAL_STD) {
                sq_sum += value*value;
            }
        }
        if(len == 0) {
            mp_raise_ValueError("data length is 0!");
        }
        if(optype ==  NUMERICAL_SUM) {
            return sum;
        } else if(optype == NUMERICAL_MEAN) {
            return sum/len;
        } else {
            sum /= len; // this is now the mean!
            return sqrtf((sq_sum/len-sum*sum));
        }
    }
    
    STATIC mp_obj_t numerical_sum_mean_std_matrix(mp_obj_t oin, mp_obj_t axis, uint8_t optype) {
        ndarray_obj_t *in = MP_OBJ_TO_PTR(oin);
        if((axis == mp_const_none) || (in->m == 1) || (in->n == 1)) { 
            // return the value for the flattened array
            return mp_obj_new_float(numerical_sum_mean_std_single_line(in->array->items, 0, 
                                                          in->array->len, 1, in->array->typecode, optype));
        } else {
            uint8_t _axis = mp_obj_get_int(axis);
            size_t m = (_axis == 0) ? 1 : in->m;
            size_t n = (_axis == 0) ? in->n : 1;
            size_t len = in->array->len;
            mp_float_t sms;
            // TODO: pass in->array->typcode to create_new_ndarray
            ndarray_obj_t *out = create_new_ndarray(m, n, NDARRAY_FLOAT);
    
            // TODO: these two cases could probably be combined in a more elegant fashion...
            if(_axis == 0) { // vertical
                for(size_t i=0; i < n; i++) {
                    sms = numerical_sum_mean_std_single_line(in->array->items, i, len, 
                                                                   n, in->array->typecode, optype);
                    ((float_t *)out->array->items)[i] = sms;
                }
            } else { // horizontal
                for(size_t i=0; i < m; i++) {
                    sms = numerical_sum_mean_std_single_line(in->array->items, i*in->n, 
                                                                   (i+1)*in->n, 1, in->array->typecode, optype);
                    ((float_t *)out->array->items)[i] = sms;
                }
            }
        return MP_OBJ_FROM_PTR(out);
        }
    }
    
    STATIC mp_obj_t numerical_function(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args, uint8_t type) {
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} } ,
            { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
        };
    
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(1, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
        
        mp_obj_t oin = args[0].u_obj;
        mp_obj_t axis = args[1].u_obj;
        if((axis != mp_const_none) && (mp_obj_get_int(axis) != 0) && (mp_obj_get_int(axis) != 1)) {
            // this seems to pass with False, and True...
            mp_raise_ValueError("axis must be None, 0, or 1");
        }
        
        if(MP_OBJ_IS_TYPE(oin, &mp_type_tuple) || MP_OBJ_IS_TYPE(oin, &mp_type_list) || 
            MP_OBJ_IS_TYPE(oin, &mp_type_range)) {
            switch(type) {
                case NUMERICAL_MIN:
                case NUMERICAL_ARGMIN:
                    return numerical_argmin_argmax_array(oin, MP_BINARY_OP_LESS, type);
                case NUMERICAL_MAX:
                case NUMERICAL_ARGMAX:
                    return numerical_argmin_argmax_array(oin, MP_BINARY_OP_MORE, type);
                case NUMERICAL_SUM:
                case NUMERICAL_MEAN:
                case NUMERICAL_STD:
                    return numerical_sum_mean_std_array(oin, type);
                default: // we should never reach this point, but whatever
                    return mp_const_none;
            }
        } else if(MP_OBJ_IS_TYPE(oin, &ulab_ndarray_type)) {
            switch(type) {
                case NUMERICAL_MIN:
                case NUMERICAL_MAX:
                case NUMERICAL_ARGMIN:
                case NUMERICAL_ARGMAX:
                    return numerical_argmin_argmax_matrix(oin, axis, type);
                case NUMERICAL_SUM:
                case NUMERICAL_MEAN:
                case NUMERICAL_STD:
                    return numerical_sum_mean_std_matrix(oin, axis, type);            
                default:
                    mp_raise_NotImplementedError("operation is not implemented on ndarrays");
            }
        } else {
            mp_raise_TypeError("input must be tuple, list, range, or ndarray");
        }
        return mp_const_none;
    }
    
    mp_obj_t numerical_min(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        return numerical_function(n_args, pos_args, kw_args, NUMERICAL_MIN);
    }
    
    mp_obj_t numerical_max(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        return numerical_function(n_args, pos_args, kw_args, NUMERICAL_MAX);
    }
    
    mp_obj_t numerical_argmin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        return numerical_function(n_args, pos_args, kw_args, NUMERICAL_ARGMIN);
    }
    
    mp_obj_t numerical_argmax(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        return numerical_function(n_args, pos_args, kw_args, NUMERICAL_ARGMAX);
    }
    
    mp_obj_t numerical_sum(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        return numerical_function(n_args, pos_args, kw_args, NUMERICAL_SUM);
    }
    
    mp_obj_t numerical_mean(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        return numerical_function(n_args, pos_args, kw_args, NUMERICAL_MEAN);
    }
    
    mp_obj_t numerical_std(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        return numerical_function(n_args, pos_args, kw_args, NUMERICAL_STD);
    }
    
    mp_obj_t numerical_roll(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        // TODO: replace memcpy by memmove
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) } },
            { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        };
    
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(2, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
        
        mp_obj_t oin = args[0].u_obj;
        int16_t shift = mp_obj_get_int(args[1].u_obj);
        int8_t axis = args[2].u_int;
        if((axis != 0) && (axis != 1)) {
            mp_raise_ValueError("axis must be None, 0, or 1");
        }
        ndarray_obj_t *in = MP_OBJ_TO_PTR(oin);
        uint8_t _sizeof = mp_binary_get_size('@', in->array->typecode, NULL);
        size_t len;
        int16_t _shift;
        uint8_t *data = (uint8_t *)in->array->items;
        // TODO: transpose the matrix, if axis == 0
        if(shift < 0) {
            _shift = -shift;
        } else {
            _shift = shift;
        }
        if(axis == 0) {
            len = in->m;
            // temporary buffer
            uint8_t *_data = m_new(uint8_t, _sizeof*len);
            
            _shift = _shift % len;
            if(shift < 0) _shift = len - _shift;
            _shift *= _sizeof;
            uint8_t *tmp = m_new(uint8_t, _shift);
    
            for(size_t n=0; n < in->n; n++) {
                for(size_t m=0; m < len; m++) {
                    // this loop should fill up the temporary buffer
                    memcpy(&_data[m*_sizeof], &data[(m*in->n+n)*_sizeof], _sizeof);
                }
                // now, the actual shift
                memcpy(tmp, _data, _shift);
                memcpy(_data, &_data[_shift], len*_sizeof-_shift);
                memcpy(&_data[len*_sizeof-_shift], tmp, _shift);
                for(size_t m=0; m < len; m++) {
                    // this loop should dump the content of the temporary buffer into data
                    memcpy(&data[(m*in->n+n)*_sizeof], &_data[m*_sizeof], _sizeof);
                }            
            }
            m_del(uint8_t, tmp, _shift);
            m_del(uint8_t, _data, _sizeof*len);
            return mp_const_none;
        }
        len = in->n;
        if((in->m == 1) || (in->n == 1)) {
            len = in->array->len;
        }
        _shift = _shift % len;
        if(shift < 0) _shift = len - _shift;
        // TODO: if(shift > len/2), we should move in the opposite direction. That would save RAM
        _shift *= _sizeof;
        uint8_t *tmp = m_new(uint8_t, _shift);
        for(size_t m=0; m < in->m; m++) {
            memcpy(tmp, &data[m*len*_sizeof], _shift);
            memcpy(&data[m*len*_sizeof], &data[m*len*_sizeof+_shift], len*_sizeof-_shift);
            memcpy(&data[(m+1)*len*_sizeof-_shift], tmp, _shift);
        }
        m_del(uint8_t, tmp, _shift);
        return mp_const_none;
    }

ulab module
===========

This module simply brings all components together, and does not contain
new function definitions.

ulab.c
------

https://github.com/v923z/micropython-ulab/tree/master/code/ulab.c

.. code:: cpp
        
    
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "py/runtime.h"
    #include "py/binary.h"
    #include "py/obj.h"
    #include "py/objarray.h" // this can in the future be dropped
    
    #include "ndarray.h"
    #include "linalg.h"
    #include "vectorise.h"
    #include "poly.h"
    #include "fft.h"
    #include "numerical.h"
    
    #define ULAB_VERSION 0.15
    
    typedef struct _mp_obj_float_t {
        mp_obj_base_t base;
        mp_float_t value;
    } mp_obj_float_t;
    
    mp_obj_float_t ulab_version = {{&mp_type_float}, ULAB_VERSION};
    
    MP_DEFINE_CONST_FUN_OBJ_1(ndarray_shape_obj, ndarray_shape);
    MP_DEFINE_CONST_FUN_OBJ_2(ndarray_size_obj, ndarray_size);
    MP_DEFINE_CONST_FUN_OBJ_1(ndarray_rawsize_obj, ndarray_rawsize);
    MP_DEFINE_CONST_FUN_OBJ_KW(ndarray_flatten_obj, 1, ndarray_flatten);
    
    MP_DEFINE_CONST_FUN_OBJ_1(linalg_transpose_obj, linalg_transpose);
    MP_DEFINE_CONST_FUN_OBJ_2(linalg_reshape_obj, linalg_reshape);
    MP_DEFINE_CONST_FUN_OBJ_1(linalg_inv_obj, linalg_inv);
    MP_DEFINE_CONST_FUN_OBJ_2(linalg_dot_obj, linalg_dot);
    MP_DEFINE_CONST_FUN_OBJ_KW(linalg_zeros_obj, 0, linalg_zeros);
    MP_DEFINE_CONST_FUN_OBJ_KW(linalg_ones_obj, 0, linalg_ones);
    MP_DEFINE_CONST_FUN_OBJ_KW(linalg_eye_obj, 0, linalg_eye);
    
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_acos_obj, vectorise_acos);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_acosh_obj, vectorise_acosh);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_asin_obj, vectorise_asin);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_asinh_obj, vectorise_asinh);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_atan_obj, vectorise_atan);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_atanh_obj, vectorise_atanh);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_ceil_obj, vectorise_ceil);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_cos_obj, vectorise_cos);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_erf_obj, vectorise_erf);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_erfc_obj, vectorise_erfc);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_exp_obj, vectorise_exp);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_expm1_obj, vectorise_expm1);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_floor_obj, vectorise_floor);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_gamma_obj, vectorise_gamma);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_lgamma_obj, vectorise_lgamma);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log_obj, vectorise_log);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log10_obj, vectorise_log10);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_log2_obj, vectorise_log2);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sin_obj, vectorise_sin);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sinh_obj, vectorise_sinh);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_sqrt_obj, vectorise_sqrt);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_tan_obj, vectorise_tan);
    MP_DEFINE_CONST_FUN_OBJ_1(vectorise_tanh_obj, vectorise_tanh);
    
    STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_linspace_obj, 2, numerical_linspace);
    STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_sum_obj, 1, numerical_sum);
    STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_mean_obj, 1, numerical_mean);
    STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_std_obj, 1, numerical_std);
    STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_min_obj, 1, numerical_min);
    STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_max_obj, 1, numerical_max);
    STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_argmin_obj, 1, numerical_argmin);
    STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_argmax_obj, 1, numerical_argmax);
    STATIC MP_DEFINE_CONST_FUN_OBJ_KW(numerical_roll_obj, 2, numerical_roll);
    
    STATIC MP_DEFINE_CONST_FUN_OBJ_2(poly_polyval_obj, poly_polyval);
    STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poly_polyfit_obj, 2, 3, poly_polyfit);
    
    STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fft_fft_obj, 1, 2, fft_fft);
    STATIC MP_DEFINE_CONST_FUN_OBJ_1(fft_spectrum_obj, fft_spectrum);
    
    STATIC const mp_rom_map_elem_t ulab_ndarray_locals_dict_table[] = {
        { MP_ROM_QSTR(MP_QSTR_shape), MP_ROM_PTR(&ndarray_shape_obj) },
        { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&ndarray_size_obj) },
        { MP_ROM_QSTR(MP_QSTR_rawsize), MP_ROM_PTR(&ndarray_rawsize_obj) },
        { MP_ROM_QSTR(MP_QSTR_flatten), MP_ROM_PTR(&ndarray_flatten_obj) },    
        { MP_ROM_QSTR(MP_QSTR_transpose), MP_ROM_PTR(&linalg_transpose_obj) },
        { MP_ROM_QSTR(MP_QSTR_reshape), MP_ROM_PTR(&linalg_reshape_obj) },
    };
    
    STATIC MP_DEFINE_CONST_DICT(ulab_ndarray_locals_dict, ulab_ndarray_locals_dict_table);
    
    const mp_obj_type_t ulab_ndarray_type = {
        { &mp_type_type },
        .name = MP_QSTR_ndarray,
        .print = ndarray_print,
        .make_new = ndarray_make_new,
        .subscr = ndarray_subscr,
        .getiter = ndarray_getiter,
        .unary_op = ndarray_unary_op,
        .binary_op = ndarray_binary_op,
        .locals_dict = (mp_obj_dict_t*)&ulab_ndarray_locals_dict,
    };
    
    STATIC const mp_map_elem_t ulab_globals_table[] = {
        { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_ulab) },
        { MP_ROM_QSTR(MP_QSTR___version__), MP_ROM_PTR(&ulab_version) },
        { MP_OBJ_NEW_QSTR(MP_QSTR_ndarray), (mp_obj_t)&ulab_ndarray_type },
        { MP_OBJ_NEW_QSTR(MP_QSTR_inv), (mp_obj_t)&linalg_inv_obj },
        { MP_ROM_QSTR(MP_QSTR_dot), (mp_obj_t)&linalg_dot_obj },
        { MP_ROM_QSTR(MP_QSTR_zeros), (mp_obj_t)&linalg_zeros_obj },
        { MP_ROM_QSTR(MP_QSTR_ones), (mp_obj_t)&linalg_ones_obj },
        { MP_ROM_QSTR(MP_QSTR_eye), (mp_obj_t)&linalg_eye_obj },    
        { MP_OBJ_NEW_QSTR(MP_QSTR_acos), (mp_obj_t)&vectorise_acos_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_acosh), (mp_obj_t)&vectorise_acosh_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_asin), (mp_obj_t)&vectorise_asin_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_asinh), (mp_obj_t)&vectorise_asinh_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_atan), (mp_obj_t)&vectorise_atan_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_atanh), (mp_obj_t)&vectorise_atanh_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_ceil), (mp_obj_t)&vectorise_ceil_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_cos), (mp_obj_t)&vectorise_cos_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_erf), (mp_obj_t)&vectorise_erf_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_erfc), (mp_obj_t)&vectorise_erfc_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_exp), (mp_obj_t)&vectorise_exp_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_expm1), (mp_obj_t)&vectorise_expm1_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_floor), (mp_obj_t)&vectorise_floor_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_gamma), (mp_obj_t)&vectorise_gamma_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_lgamma), (mp_obj_t)&vectorise_lgamma_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_log), (mp_obj_t)&vectorise_log_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_log10), (mp_obj_t)&vectorise_log10_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_log2), (mp_obj_t)&vectorise_log2_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_sin), (mp_obj_t)&vectorise_sin_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_sinh), (mp_obj_t)&vectorise_sinh_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_sqrt), (mp_obj_t)&vectorise_sqrt_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_tan), (mp_obj_t)&vectorise_tan_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_tanh), (mp_obj_t)&vectorise_tanh_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_linspace), (mp_obj_t)&numerical_linspace_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_sum), (mp_obj_t)&numerical_sum_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_mean), (mp_obj_t)&numerical_mean_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_std), (mp_obj_t)&numerical_std_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_min), (mp_obj_t)&numerical_min_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_max), (mp_obj_t)&numerical_max_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_argmin), (mp_obj_t)&numerical_argmin_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_argmax), (mp_obj_t)&numerical_argmax_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_roll), (mp_obj_t)&numerical_roll_obj },  
        { MP_OBJ_NEW_QSTR(MP_QSTR_polyval), (mp_obj_t)&poly_polyval_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_polyfit), (mp_obj_t)&poly_polyfit_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_fft), (mp_obj_t)&fft_fft_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_spectrum), (mp_obj_t)&fft_spectrum_obj },
        // class constants
        { MP_ROM_QSTR(MP_QSTR_uint8), MP_ROM_INT(NDARRAY_UINT8) },
        { MP_ROM_QSTR(MP_QSTR_int8), MP_ROM_INT(NDARRAY_INT8) },
        { MP_ROM_QSTR(MP_QSTR_uint16), MP_ROM_INT(NDARRAY_UINT16) },
        { MP_ROM_QSTR(MP_QSTR_int16), MP_ROM_INT(NDARRAY_INT16) },
        { MP_ROM_QSTR(MP_QSTR_float), MP_ROM_INT(NDARRAY_FLOAT) },
    };
    
    STATIC MP_DEFINE_CONST_DICT (
        mp_module_ulab_globals,
        ulab_globals_table
    );
    
    const mp_obj_module_t ulab_user_cmodule = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t*)&mp_module_ulab_globals,
    };
    
    MP_REGISTER_MODULE(MP_QSTR_ulab, ulab_user_cmodule, MODULE_ULAB_ENABLED);

makefile
--------

ipython3.. code ::
        
    %%writefile ../../../ulab/code/micropython.mk
    
    USERMODULES_DIR := $(USERMOD_DIR)
    
    # Add all C files to SRC_USERMOD.
    SRC_USERMOD += $(USERMODULES_DIR)/ndarray.c
    SRC_USERMOD += $(USERMODULES_DIR)/linalg.c
    SRC_USERMOD += $(USERMODULES_DIR)/vectorise.c
    SRC_USERMOD += $(USERMODULES_DIR)/poly.c
    SRC_USERMOD += $(USERMODULES_DIR)/fft.c
    SRC_USERMOD += $(USERMODULES_DIR)/numerical.c
    SRC_USERMOD += $(USERMODULES_DIR)/ulab.c
    
    # We can add our module folder to include paths if needed
    # This is not actually needed in this example.
    CFLAGS_USERMOD += -I$(USERMODULES_DIR)
.. parsed-literal::

    Overwriting ../../../ulab/code/micropython.mk

make
----

unix port
~~~~~~~~~

ipython3.. code ::
        
    %cd ../../../micropython/ports/unix/
.. parsed-literal::

    /home/v923z/sandbox/micropython/v1.11/micropython/ports/unix

.. code:: bash

    !make USER_C_MODULES=../../../ulab all
.. parsed-literal::

    Use make V=1 or set BUILD_VERBOSE in your environment to increase build verbosity.
    Including User C Module from ../../../ulab/code
    GEN build/genhdr/moduledefs.h
    GEN build/genhdr/qstr.i.last
    GEN build/genhdr/qstr.split
    GEN build/genhdr/qstrdefs.collected.h
    QSTR not updated
    CC ../../py/objmodule.c
    CC ../../../ulab/code/ndarray.c
    CC ../../../ulab/code/ulab.c
    LINK micropython
       text	   data	    bss	    dec	    hex	filename
       2085	   6862	      0	   8947	   22f3	build/build/frozen_mpy.o
          2	      0	      0	      2	      2	build/build/frozen.o
     477228	  57856	   2104	 537188	  83264	micropython

stm32 port
~~~~~~~~~~

ipython3.. code ::
        
    %cd ../../../micropython/ports/stm32/
.. parsed-literal::

    /home/v923z/sandbox/micropython/v1.11/micropython/ports/stm32

.. code:: bash

    !make BOARD=PYBV11 CROSS_COMPILE=../../../../compiler/bin/arm-none-eabi- USER_C_MODULES=../../../ulab all
Change log
==========

ipython3.. code ::
        
    %%writefile ../../../ulab/docs/ulab-change-log.md
    
    Tue, 8 Oct 2019
    
    version 0.15
    
        added inv, neg, pos, and abs unary operators to ndarray.c
        
    Mon, 7 Oct 2019
    
    version 0.14
    
        made the internal binary_op function tighter, and added keyword arguments to linspace
        
    Sat, 4 Oct 2019
    
    version 0.13
    
        added the <, <=, >, >= binary operators to ndarray
    
    Fri, 4 Oct 2019
    
    version 0.12
    
        added .flatten to ndarray, ones, zeros, and eye to linalg
    
    Thu, 3 Oct 2019
    
    version 0.11
        
        binary operators are now based on macros
.. parsed-literal::

    Overwriting ../../../ulab/docs/ulab-change-log.md

ipython3.. code ::
        
    
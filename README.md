# ulab

`ulab` is a `numpy`-like array manipulation library for `micropython` and `CircuitPython`.
The module is written in C, defines compact containers for numerical
data, and is fast. The library is a software-only standard `micropython` user module,
i.e., it has no hardware dependencies, and can be compiled for any platform. 
The `float` implementation of `micropython` (`float`, or `double`) is automatically detected.

# Supported functions


## ndarray

`ulab` implements `numpy`'s `ndarray` with the `==`, `!=`, `<`, `<=`, `>`, `>=`, `+`, `-`, `/`, `*`, and `**` binary operators, and the `len`, `~`, `-`, `+`, `abs` unary operators that operate element-wise. Type-aware `ndarray`s can be initialised from any `micropython` iterable, lists of iterables, or by means of the `ones`, `zeros`, `eye`, or `linspace` functions. 

`ndarray`s can be iterated on, and have a number of their own methods, such as `shape`, `reshape`, `transpose`, `size`, and `itemsize`.

## Modules

In addition to the `ndarray`'s operators and methods, seven modules define a great number of functions that can take `ndarray`s or `micropython` iterables as their arguments. If flash space is a concern, unnecessary sub-modules can be excluded from the compiled firmware with a pre-processor switch. 

### vector

The `vector` sub-module implements all functions of the `math` package (e.g., `acos`, `acosh`, ..., `tan`, `tanh`) of `micropython` for `ndarray`s and iterables.

### numerical

The `numerical` sub-module defines the `roll`, `flip`, `diff`, `sort` and `argsort` functions for `ndarray`s, and, in addition, the `min`, `max`, `argmin`, `argmax`, `sum`, `mean`, `std` functions that work with `ndarray`s, as well as generic one-dimensional iterables.

### linalg

The `linalg` sub-module implements functions for matrix inversion, dot product, and the calculation of the determinant, eigenvalues, eigenvectors, Cholesky decomposition, and trace. 

### poly

The `poly` sub-module defines the `polyval`, and `polyfit` functions from `numpy`.

### fft

The `fft` sub-module implements the fast Fourier transform, and its inverse for one-dimensional `ndarray`s.

### filter

The `filter` sub-module implements one-dimensional convolution.

### compare

The `compare` sub-module contains the implementation of the `minimum`, `maximum`, and `clip` functions.

### extras

The `extras` sub-module is meant as a user-extendable module, and currently implements the `spectrogram` function of `scipy`. 

# Finding help

Documentation can be found on [readthedocs](readthedocs.org/) under [micropython-ulab](https://micropython-ulab.readthedocs.io/en/latest), as well as at [circuitpython-ulab](
https://circuitpython.readthedocs.io/en/latest/shared-bindings/ulab/__init__.html). A number of practical examples are listed in the excellent [circuitpython-ulab](https://learn.adafruit.com/ulab-crunch-numbers-fast-with-circuitpython/overview) overview.

# Firmware

Firmware for pyboard.v.1.1, and PYBD_SF6 is released once in a while, and can be downloaded
from https://github.com/v923z/micropython-ulab/releases. Since a number of features can be
set in the firmware (threading, support for SD card, LEDs, user switch etc.), and it is
impossible to create something that suits everyone, these releases should only be used for
quick testing of `ulab`. Otherwise, compilation from the source is required with
the appropriate settings, which are usually defined in the `mpconfigboard.h` file of the port
in question.

`ulab` is also included in most development builds of `CircuitPython` for SAMD51 and nRF microcontrollers.

## Compiling

If you want to try the latest version of `ulab` on micropython, or your hardware is 
different to pyboard.v.1.1, or PYBD_SF6, the firmware can be compiled 
from the source by following these steps:

First, you have to clone the `micropython` repository by running

```
git clone https://github.com/micropython/micropython.git
```
on the command line. This will create a new repository with the name `micropython`. Staying there, clone the `ulab` repository with 

```
git clone https://github.com/v923z/micropython-ulab.git ulab
```

If you don't have the cross-compiler installed, your might want to do that now, for instance on Linux by executing

```
sudo apt-get install gcc-arm-none-eabi
```

If this step was successful, you can try to run the `make` command in the port's directory as
```
make BOARD=PYBV11 USER_C_MODULES=../../../ulab all
```
which will prepare the firmware for pyboard.v.11. Similarly, 
```
make BOARD=PYBD_SF6 USER_C_MODULES=../../../ulab all
```
will compile for the SF6 member of the PYBD series. If your target is `unix`, you don't need to specify the `BOARD` parameter.

Provided that you managed to compile the firmware, you would upload that by running either
```
dfu-util --alt 0 -D firmware.dfu
```
or 
```
python pydfu.py -u firmware.dfu
```

In case you got stuck somewhere in the process, a bit more detailed instructions can be found under https://github.com/micropython/micropython/wiki/Getting-Started, and https://github.com/micropython/micropython/wiki/Pyboard-Firmware-Update.

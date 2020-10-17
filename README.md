# ulab

`ulab` is a `numpy`-like array manipulation library for `micropython` and `CircuitPython`.
The module is written in C, defines compact containers for numerical data of one to four
dimensions, and is fast. The library is a software-only standard `micropython` user module,
i.e., it has no hardware dependencies, and can be compiled for any platform.
The `float` implementation of `micropython` (`float`, or `double`) is automatically detected.

# Supported functions


## ndarray

`ulab` implements `numpy`'s `ndarray` with the `==`, `!=`, `<`, `<=`, `>`, `>=`, `+`, `-`, `/`, `*`, `**`,
`+=`, `-=`, `*=`, `/=`, `**=` binary operators, and the `len`, `~`, `-`, `+`, `abs` unary operators that
operate element-wise. Type-aware `ndarray`s can be initialised from any `micropython` iterable, lists of
iterables, or by means of the `arange`, `concatenate`, `eye`, `full`, `linspace`, `logspace`, `ones`, or
`zeros`  functions.

`ndarray`s can be iterated on, and have a number of their own methods, such as `flatten`, `shape`,
`reshape`, `strides`, `transpose`, `size`, `tobytes`, and `itemsize`.

## Customising the firmware

In addition to the `ndarray`'s operators and methods, seven modules define a great number of functions that can
take `ndarray`s or `micropython` iterables as their arguments. If flash space is a concern, unnecessary sub-modules
or even individual functions can be excluded from the compiled firmware with pre-processor switches.

### approx

The `approx` sub-module contains the implementation of the `interp`, and `trapz` functions of `numpy`, and `newton`, `bisect`,
and `fmin` from `scipy`.

### compare

The `compare` sub-module contains the implementation of the `equal`, `not_equal`, `minimum`, `maximum`, and `clip` functions.

### fft

The `fft` sub-module implements the fast Fourier transform, and its inverse for one-dimensional `ndarray`s,
as well as the `spectrogram` function from `scipy`.

### filter

The `filter` sub-module implements `convolve` for one-dimensional convolution,
as well as the cascaded second-order sections filter, `sosfilt` from `scipy`.

### linalg

The `linalg` sub-module implements functions for matrix inversion, dot product, and the calculation of the
determinant, eigenvalues, eigenvectors, Cholesky decomposition, and trace.

### numerical

The `numerical` sub-module defines the `cross`, `diff`, `flip`, `roll`, `sort` and `argsort` functions for `ndarray`s, and,
in addition, the `min`, `max`, `argmin`, `argmax`, `sum`, `mean`, `std` functions that work with `ndarray`s, as
well as generic one-dimensional iterables.

### poly

The `poly` sub-module defines the `polyval`, and `polyfit` functions from `numpy`.

### vector

The `vector` sub-module implements all functions of `micropython`'s `math` package (e.g., `acos`, `acosh`, ..., `tan`, `tanh`),
and the `degrees` and `radians` for `ndarray`s and iterables. In addition, it also provided tools for vectorising generic,
user-defined `python` functions.

### user

The `user` sub-module is meant as a user-extendable module, and contains a dummy function only.

# Finding help

Documentation can be found on [readthedocs](https://readthedocs.org/) under
[micropython-ulab](https://micropython-ulab.readthedocs.io/en/latest),
as well as at [circuitpython-ulab](https://circuitpython.readthedocs.io/en/latest/shared-bindings/ulab/__init__.html).
A number of practical examples are listed in the excellent
[circuitpython-ulab](https://learn.adafruit.com/ulab-crunch-numbers-fast-with-circuitpython/overview) overview.

# Firmware

Compiled firmware for many hardware platforms can be downloaded from Roberto Colistete's
gitlab repository: for the [pyboard](https://gitlab.com/rcolistete/micropython-samples/-/tree/master/Pyboard/Firmware/), and
for [ESP8266](https://gitlab.com/rcolistete/micropython-samples/-/tree/master/ESP8266/Firmware).
Since a number of features can be set in the firmware (threading, support for SD card, LEDs, user switch etc.), and it is
impossible to create something that suits everyone, these releases should only be used for
quick testing of `ulab`. Otherwise, compilation from the source is required with
the appropriate settings, which are usually defined in the `mpconfigboard.h` file of the port
in question.

`ulab` is also included in the following compiled `micropython` variants and derivatives:

1. `CircuitPython` for SAMD51 and nRF microcontrollers https://github.com/adafruit/circuitpython
1. `MicroPython for K210` https://github.com/loboris/MicroPython_K210_LoBo
1. `MaixPy` https://github.com/sipeed/MaixPy
1. `OpenMV` https://github.com/openmv/openmv
1. `pycom` https://pycom.io/

## Compiling

If you want to try the latest version of `ulab` on `micropython` or one of its forks, the firmware can be compiled
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

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
iterables, or by means of the `arange`, `concatenate`, `diag`, `eye`, `frombuffer`, `full`, `linspace`, `logspace`, `ones`, or
`zeros`  functions.

`ndarray`s can be iterated on, and have a number of their own methods, such as `flatten`, `shape`,
`reshape`, `strides`, `transpose`, `size`, `tobytes`, and `itemsize`.


## Customising the firmware

In addition to the `ndarray` operators and methods, `ulab` defines a great number of functions that can
take `ndarray`s or `micropython` iterables as their arguments. If flash space is a concern, unnecessary functions
can be excluded from the compiled firmware with pre-processor switches. Most of the functions are parts of
`numpy`, but several are re-implementations of `scipy` features. For a full list of functions, see
[micropython-ulab](https://micropython-ulab.readthedocs.io/en/latest)!


## Usage

`ulab` sports a `numpy`-compatible interface, which makes porting of `CPython` code straightforward. The following
snippet should run equally well in `micropython`, or on a PC.

```python
try:
    from ulab import numpy as np
    from ulab import scipy as spy
except ImportError:
    import numpy as np
    import scipy as spy

x = np.array([1, 2, 3])
spy.special.erf(x)
```

# Finding help

Documentation can be found on [readthedocs](https://readthedocs.org/) under
[micropython-ulab](https://micropython-ulab.readthedocs.io/en/latest),
as well as at [circuitpython-ulab](https://circuitpython.readthedocs.io/en/latest/shared-bindings/ulab/__init__.html).
A number of practical examples are listed in Jeff Epler's excellent
[circuitpython-ulab](https://learn.adafruit.com/ulab-crunch-numbers-fast-with-circuitpython/overview) overview.

# Benchmarks

Representative numbers on performance can be found under [ulab samples](https://github.com/thiagofe/ulab_samples). 

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

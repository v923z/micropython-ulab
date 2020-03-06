# micropython-ulab

`ulab` is a `numpy`-like array manipulation library for micropython and circuitpython.
The module is written in C, defines compact containers for numerical
data, and is fast. The library is a software-only standard `micropython` user module,
i.e., it has no hardware dependencies, and can be compiled for any platform.
The `float` implementation of `micropython` (`float`, or `double`) is automatically detected.

Documentation can be found under https://micropython-ulab.readthedocs.io/en/latest/.

# Firmware

Firmware for pyboard.v.1.1, and PYBD_SF6 is released once in a while, and can be downloaded
from https://github.com/v923z/micropython-ulab/releases. Since a number of features can be
set in the firmware (threading, support for SD card, LEDs, user switch etc.), and it is
impossible to create something that suits everyone, these releases should only be used for
quick testing of `ulab`. Otherwise, compilation from the source is required with
the appropriate settings, which are usually defined in the `mpconfigboard.h` file of the port
in question.

Firmware is also included in most development builds of CircuitPython for SAMD51 and nRF microcontrollers.

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

# ulab

`ulab` is a `numpy`-like array manipulation library for [micropython](http://micropython.org/) and [CircuitPython](https://circuitpython.org/).
The module is written in C, defines compact containers for numerical data of one to four
dimensions, and is fast. The library is a software-only standard `micropython` user module,
i.e., it has no hardware dependencies, and can be compiled for any platform.
The `float` implementation of `micropython` (`float`, or `double`) is automatically detected.

# Supported functions


## ndarray

`ulab` implements `numpy`'s `ndarray` with the `==`, `!=`, `<`, `<=`, `>`, `>=`, `+`, `-`, `/`, `*`, `**`,
`+=`, `-=`, `*=`, `/=`, `**=` binary operators, and the `len`, `~`, `-`, `+`, `abs` unary operators that
operate element-wise. Type-aware `ndarray`s can be initialised from any `micropython` iterable, lists of
iterables via the `array` constructor, or by means of the `arange`, `concatenate`, `diag`, `eye`, 
`frombuffer`, `full`, `linspace`, `logspace`, `ones`, or `zeros`  functions.

`ndarray`s can be iterated on, and have a number of their own methods, such as `flatten`, `itemsize`, `reshape`,
`shape`, `size`, `strides`, `tobytes`, and `transpose`.


## Customising the firmware

In addition to the `ndarray` operators and methods, `ulab` defines a great number of functions that can
take `ndarray`s or `micropython` iterables as their arguments. Most of the functions have been ported from 
`numpy`, but several are re-implementations of `scipy` features. For a complete list, see
[micropython-ulab](https://micropython-ulab.readthedocs.io/en/latest)!

If flash space is a concern, unnecessary functions can be excluded from the compiled firmware with 
pre-processor switches. In addition, `ulab` also has options for trading execution speed for firmware size. 
A thorough discussion on how the firmware can be customised can be found in the 
[corresponding section](https://micropython-ulab.readthedocs.io/en/latest/ulab-intro.html#customising-the-firmware) 
of the user manual.


## Usage

`ulab` sports a `numpy/scipy`-compatible interface, which makes porting of `CPython` code straightforward. The following
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

### UNIX port

Simply clone the `ulab` repository with

```bash
git clone https://github.com/v923z/micropython-ulab.git ulab
```
and then run 

```bash
./build.sh
```
This command will clone `micropython`, and build the `unix` port automatically, as well as run the test scripts. If you want an interactive `unix` session, you can launch it in 

```bash
ulab/micropython/ports/unix
```

### STM-based boards

First, you have to clone the `micropython` repository by running

```bash
git clone https://github.com/micropython/micropython.git
```
on the command line. This will create a new repository with the name `micropython`. Staying there, clone the `ulab` repository with

```bash
git clone https://github.com/v923z/micropython-ulab.git ulab
```
If you don't have the cross-compiler installed, your might want to do that now, for instance on Linux by executing

```bash
sudo apt-get install gcc-arm-none-eabi
```

If this step was successful, you can try to run the `make` command in the port's directory as

```bash
make BOARD=PYBV11 USER_C_MODULES=../../../ulab all
```
which will prepare the firmware for pyboard.v.11. Similarly,

```bash
make BOARD=PYBD_SF6 USER_C_MODULES=../../../ulab all
```
will compile for the SF6 member of the PYBD series. If your target is `unix`, you don't need to specify the `BOARD` parameter.

Provided that you managed to compile the firmware, you would upload that by running either

```bash
dfu-util --alt 0 -D firmware.dfu
```
or

```bash
python pydfu.py -u firmware.dfu
```

In case you got stuck somewhere in the process, a bit more detailed instructions can be found under https://github.com/micropython/micropython/wiki/Getting-Started, and https://github.com/micropython/micropython/wiki/Pyboard-Firmware-Update.


### ESP32-based boards

```bash
cd $BUILD_DIR/micropython
git checkout b137d064e9e0bfebd2a59a9b312935031252e742
# choose micropython version - note v1.12 is incompatible with ulab
# and v1.13 is currently broken in some ways (on some platforms) https://github.com/BradenM/micropy-cli/issues/167
# - the patch is not live yet (should be in 1.14), but is at this commit
git submodule update --init
cd $BUILD_DIR/micropython/mpy-cross && make # build cross-compiler (required)

cd $BUILD_DIR/micropython/ports/esp32
make ESPIDF= # will display supported ESP-IDF commit hashes
# output should look like: """
# ...
# Supported git hash (v3.3): 9e70825d1e1cbf7988cf36981774300066580ea7
# Supported git hash (v4.0) (experimental): 4c81978a3e2220674a432a588292a4c860eef27b
```

Choose an ESPIDF version from one of the options printed by the previous command:

```bash
ESPIDF_VER=9e70825d1e1cbf7988cf36981774300066580ea7

# Download and prepare the SDK
git clone https://github.com/espressif/esp-idf.git $BUILD_DIR/esp-idf
cd $BUILD_DIR/esp-idf
git checkout $ESPIDF_VER
git submodule update --init --recursive # get idf submodules
pip install -r ./requirements.txt # install python reqs
```

Next, install the ESP32 compiler. If using an ESP-IDF version >= 4.x (chosen by `$ESPIDF_VER` above), this can be done by running `. $BUILD_DIR/esp-idf/install.sh`. Otherwise, (for version 3.x) run:

```bash
cd $BUILD_DIR

# for 64 bit linux
curl https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz | tar xvz

# for 32 bit
# curl https://dl.espressif.com/dl/xtensa-esp32-elf-linux32-1.22.0-80-g6c4433a-5.2.0.tar.gz | tar xvz

# don't worry about adding to path; we'll specify that later

# also, see https://docs.espressif.com/projects/esp-idf/en/v3.3.2/get-started for more info
```

We can now clone the `ulab` repository

```
git clone https://github.com/v923z/micropython-ulab $BUILD_DIR/ulab
```

Finally, build the firmware:

```bash
cd $BUILD_DIR/micropython/ports/esp32
# temporarily add esp32 compiler to path
export PATH=$BUILD_DIR/xtensa-esp32-elf/bin:$PATH
export ESPIDF=$BUILD_DIR/esp-idf # req'd by Makefile
export BOARD=GENERIC # options are dirs in ./boards
export USER_C_MODULES=$BUILD_DIR/ulab # include ulab in firmware

make submodules & make all
```

If it compiles without error, you can plug in your ESP32 via USB and then flash it with:

```bash
make erase && make deploy
```

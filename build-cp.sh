#!/bin/sh
set -e
nproc=$(python -c 'import multiprocessing; print(multiprocessing.cpu_count())')
HERE="$(dirname -- "$(readlink -f -- "${0}")" )"
[ -e circuitpython/py/py.mk ] || (git clone --depth 100 --branch 6.0.x https://github.com/adafruit/circuitpython && cd circuitpython && git submodule update --init)
rm -rf circuitpython/extmod/ulab; ln -s "$HERE" circuitpython/extmod/ulab
make -C circuitpython/mpy-cross -j$nproc
make -C circuitpython/ports/unix -j$nproc deplibs
make -C circuitpython/ports/unix -j$nproc

if ! env MICROPY_MICROPYTHON=circuitpython/ports/unix/micropython circuitpython/tests/run-tests -d tests; then
    for exp in *.exp; do
        testbase=$(basename $exp .exp);
        echo -e "\nFAILURE $testbase";
        diff -u $testbase.exp $testbase.out;
    done
    exit 1
fi

(cd circuitpython && sphinx-build -E -W -b html . _build/html)

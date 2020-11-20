#!/bin/sh
set -e
HERE="$(dirname -- "$(readlink -f -- "${0}")" )"
[ -e micropython/py/py.mk ] || git clone https://github.com/micropython/micropython
[ -e micropython/lib/libffi/autogen.sh ] || (cd micropython && git submodule update --init lib/libffi )
#git clone https://github.com/micropython/micropython
make -C micropython/mpy-cross -j$(nproc)
make -C micropython/ports/unix -j$(nproc) deplibs
make -C micropython/ports/unix -j$(nproc) USER_C_MODULES="${HERE}" DEBUG=1 STRIP=:


for dir in "numpy" "common"
do
	if ! env MICROPY_MICROPYTHON=micropython/ports/unix/micropython ./run-tests -d tests/"$dir"; then
		for exp in *.exp; do
			testbase=$(basename $exp .exp);
			echo -e "\nFAILURE $testbase";
			diff -u $testbase.exp $testbase.out;
		done
	fi
done

#!/bin/sh
set -e
# POSIX compliant version
readlinkf_posix() {
  [ "${1:-}" ] || return 1
  max_symlinks=40
  CDPATH='' # to avoid changing to an unexpected directory

  target=$1
  [ -e "${target%/}" ] || target=${1%"${1##*[!/]}"} # trim trailing slashes
  [ -d "${target:-/}" ] && target="$target/"

  cd -P . 2>/dev/null || return 1
  while [ "$max_symlinks" -ge 0 ] && max_symlinks=$((max_symlinks - 1)); do
    if [ ! "$target" = "${target%/*}" ]; then
      case $target in
        /*) cd -P "${target%/*}/" 2>/dev/null || break ;;
        *) cd -P "./${target%/*}" 2>/dev/null || break ;;
      esac
      target=${target##*/}
    fi

    if [ ! -L "$target" ]; then
      target="${PWD%/}${target:+/}${target}"
      printf '%s\n' "${target:-/}"
      return 0
    fi

    # `ls -dl` format: "%s %u %s %s %u %s %s -> %s\n",
    #   <file mode>, <number of links>, <owner name>, <group name>,
    #   <size>, <date and time>, <pathname of link>, <contents of link>
    # https://pubs.opengroup.org/onlinepubs/9699919799/utilities/ls.html
    link=$(ls -dl -- "$target" 2>/dev/null) || break
    target=${link#*" $target -> "}
  done
  return 1
}
NPROC=$(python -c 'import multiprocessing; print(multiprocessing.cpu_count())')
HERE="$(dirname -- "$(readlinkf_posix -- "${0}")" )"
[ -e circuitpython/py/py.mk ] || (git clone --no-recurse-submodules --depth 100 --branch 6.0.x https://github.com/adafruit/circuitpython && cd circuitpython && git submodule update --init lib/uzlib tools)
rm -rf circuitpython/extmod/ulab; ln -s "$HERE" circuitpython/extmod/ulab
make -C circuitpython/mpy-cross -j$NPROC
sed -e '/MICROPY_PY_UHASHLIB/s/1/0/' < circuitpython/ports/unix/mpconfigport.h > circuitpython/ports/unix/mpconfigport_ulab.h
# Work around circuitpython#3990
make -C circuitpython/ports/unix -j$NPROC DEBUG=1 MICROPY_PY_FFI=0 MICROPY_PY_BTREE=0 MICROPY_SSL_AXTLS=0 MICROPY_PY_USSL=0 CFLAGS_EXTRA='-DMP_CONFIGFILE="<mpconfigport_ulab.h>" -Wno-tautological-constant-out-of-range-compare' build/genhdr/qstrdefs.generated.h
make -k -C circuitpython/ports/unix -j$NPROC DEBUG=1 MICROPY_PY_FFI=0 MICROPY_PY_BTREE=0 MICROPY_SSL_AXTLS=0 MICROPY_PY_USSL=0 CFLAGS_EXTRA='-DMP_CONFIGFILE="<mpconfigport_ulab.h>" -Wno-tautological-constant-out-of-range-compare'

for dir in "circuitpy" "common"
do
	if ! env MICROPY_MICROPYTHON=circuitpython/ports/unix/micropython ./run-tests -d tests/"$dir"; then
		for exp in *.exp; do
			testbase=$(basename $exp .exp);
			echo -e "\nFAILURE $testbase";
			diff -u $testbase.exp $testbase.out;
		done
		exit 1
	fi
done

#(cd circuitpython && sphinx-build -E -W -b html . _build/html)

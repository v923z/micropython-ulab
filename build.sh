#!/bin/sh
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
NPROC=`python3 -c 'import multiprocessing; print(multiprocessing.cpu_count())'`
set -e
HERE="$(dirname -- "$(readlinkf_posix -- "${0}")" )"
[ -e micropython/py/py.mk ] || git clone --no-recurse-submodules https://github.com/micropython/micropython
[ -e micropython/lib/axtls/README ] || (cd micropython && git submodule update --init lib/axtls )
dims=${1-2}
make -C micropython/mpy-cross -j${NPROC}
make -C micropython/ports/unix -j${NPROC} axtls
make -C micropython/ports/unix -j${NPROC} USER_C_MODULES="${HERE}" DEBUG=1 STRIP=: MICROPY_PY_FFI=0 MICROPY_PY_BTREE=0 CFLAGS_EXTRA=-DULAB_MAX_DIMS=$dims BUILD=build-$dims PROG=micropython-$dims

bash test-common.sh "${dims}" "micropython/ports/unix/micropython-$dims"

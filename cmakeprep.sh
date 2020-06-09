#!/bin/bash
# Prepare CMake build environment.
# * Set target build target via environment var XC_INSTALL_DIR.
# * Set compiler toolchain via CC=(gcc|clang)
#

PREFIX=${XC_INSTALL_DIR:=/usr/local}


cwd=$(dirname $0)
[ -f ${cwd}/../.buildopts ] && . ${cwd}/../.buildopts             # source in from buildopts

# For bash 4.2 and above,
# make pipe result visible in calling shell and turn off job control
#unset x; shopt -s lastpipe; set +m

(
echo depspfx=${depspfx}
echo xcpfx=${xcpfx}
echo CC=${CC}
echo CXX=${CXX}
echo LIC=${LIC}
echo PREFIX=${PREFIX}
echo XC_INSTALL_DIR=${XC_INSTALL_DIR}
echo BUILD_TYPE=${BUILD_TYPE}
) > /tmp/xc.$$.cm.keys

utilpath=$(dirname $0)

python3 ${utilpath}/cmakeprep_args.py /tmp/xc.$$.cm.keys /tmp/xc.$$.cm.out

cmake -G 'Unix Makefiles' $(cat /tmp/xc.$$.cm.out) $*

rm -f /tmp/xc.$$.cm.*


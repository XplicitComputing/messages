#!/bin/bash

# Build and install Protocol Buffer library from Github source.
# Remove Ubuntu/Debian package first, if it exists.
# options:
#   --tag=      Checkout code at a tag on current branch (default to $TAG).
#   --prefix=   Install directory prefix (default to $PREFIX).
#   --dpfx=     Same as "--prefix=".
#   --noclean   Do not remove build directory after build and install.
#   --getmd5    Read archived file by tag, determine MD5 sum; do not install.
#   --md5=      Specify MD5 sum for tagged file.
#
# Examples:
#   ./install-protobuf.sh --noclean                 # Use default source, do not remove build.
#   ./install-protobuf.sh --prefix=/opt/xcompute    # Set install target to /opt/xcompute
#   ./install-protobuf.sh CXX=clang++               # Pass CXX through to package build.
# To install protobuf 3.20.3 on Linux, try this:
#   ./install-protobuf.sh --dpfx=$HOME/xcdeps --skipcheck --tag=3.20.3 \
#     --md5=a1e8f594f998576180ff1efa49007f54
# (Change prefix value as appropriate.)
# Use "--skipcheck" because included GoogleTests fail to come with Apple clang 14.
# But installed protobuf assets apparently work.
#

# requires Bash 3.0 (for i in {x..y})

RED='\033[1;31m'        # bold red
YLW='\033[1;33m'        # bold yellow
NC='\033[0m'            # No Color
YLW_WARNING="${RED}[${YLW}WARNING${RED}]${NC}"

GH_PROTOBUF_RLSEDL=https://github.com/protocolbuffers/protobuf/releases/download
PKG_PROTOBUFDEV=libprotobuf-dev                 # dev package on Ubuntu/Debian
PREFIX=/usr/local       # Install target prefix
CLEANUP=1
CFGARGS="CFLAGS=-fPIC CXXFLAGS=-fPIC"
#TAG=3.8.0               # Default protobuf release for early XC Messages
#PBMD5=cc4f50740430d9488312e5e48bc94d68  # v3.8.0
TAG=3.20.3
PBMD5=a1e8f594f998576180ff1efa49007f54  # v3.20.3
GETMD5=
SKIPCHECK=

while [ $# -gt 0 ]; do
    case "$1" in
      --tag=*)
        TAG="${1#*=}"
        ;;
      --dpfx=*|--prefix=*)
        PREFIX="${1#*=}"
        ;;
      --md5=*)
         PBMD5="${1#*=}"
        ;;
      --getmd5)
        GETMD5=y
        ;;
      --skipcheck)      # skip 'make check' for some toolchain version; GoogleTest has issues with them
        SKIPCHECK=y
        ;;
      --noclean)
        CLEANUP=
        ;;
      *)  # unknown argument; pass through
        CFGARGS="${CFGARGS} $1"
        ;;
    esac
    shift
done

pbtar=protobuf-all-${TAG}.tar.gz

if [ "${GETMD5}" = y ]; then
    echo Compute MD5 sum:
    wget -O- -q ${GH_PROTOBUF_RLSEDL}/v${TAG}/${pbtar} | openssl md5
    echo Completed.
    exit 0
fi

if [ \! -d ${PREFIX} ]; then
    echo Target install directory \"${PREFIX}\" does not exist.
    echo Please manually create directory with right ownership before continuing.
    exit 1
fi

echo Branch tag $TAG
echo Install target $PREFIX

echo "---- Installing Protocol Buffers ----"


getmd5() {
    local fname=$1
    retval=$(openssl md5 ${fname} |  awk '{print $2}')
}

protocvers() {
    retval=$(protoc --version | awk '{print $2}')
    echo protoc vers ${retval}
}

for fetchpass in {1..4}; do

    if [ -f ${pbtar} ]; then
        getmd5 ${pbtar}; pbtar_md5=${retval}
        if [ ${pbtar_md5} != ${PBMD5} ]; then
            matched=fail
        else
            matched=succ
        fi
    else
        matched=fail
    fi
    # matched=fail        # for testing
    if [ ${matched} = succ ]; then
        break
    else
        echo fetchpass ${fetchpass}...
        if [ -f ${pbtar} ]; then rm ${pbtar}; fi
        wget ${GH_PROTOBUF_RLSEDL}/v${TAG}/${pbtar}
    fi

done
#echo last fetchpass ${fetchpass}...
#echo matched ${matched}
if [ ${matched} = fail ]; then
    echo Unable to fetch protobuf source ${pbtar}
    exit 1
fi

# check for pre-existing incompatible protoc.
dpkg -l ${PKG_PROTOBUFDEV}
rc_dpkg=$?
if [ $rc_dpkg -eq 0 ]; then
    protocvers; pbvers=retval
    if [ ${pbvers} != ${TAG} ]; then
        echo -e ${YLW_WARNING}: a different version of ${YLW}${PKG_PROTOBUFDEV}${NC} is installed on your machine
        echo Currently using $(which protoc)
        echo Please make sure that the right version is found in PATH at build-time.
        sleep 3
        for count in 5 4 3 2 1; do
            echo -n ${count}'... '
            sleep 1
        done
        echo ' '
    fi
fi

tar -xf ${pbtar}
cd protobuf-${TAG}

./autogen.sh
./configure --prefix=${PREFIX} ${CFGARGS}
make -j$(nproc)
make check -j$(nproc)
if [ \! ${SKIPCHECK} ]; then
    make check -j${npcu}
else
    echo Skipping 'make check' for ${TAG}
fi

# check if target needs 'sudo'; execute according
xcuid=$(stat -c '%u' ${PREFIX})
if [ "${xcuid}_" = "0_" ]; then
    PRIV=sudo
    echo -e "Using ${YLW}sudo${NC} to install as ${YLW}root${NC}."
fi

set -x
${PRIV} make install
# ldconfig for libprotobuf needs 'autoconf --disable-shared';
# tells autoconf/libtool to not build shared libraries.
[ -n "${PRIV}" ] && ${PRIV} ldconfig
cd ..
set +x

if [ "$CLEANUP" ]; then
    echo "Cleaning..."
    cd ..
    rm -R protobuf-${TAG}
fi
export PATH=${PREFIX}/bin:$PATH
which protoc
protoc --version
if [ $rc_dpkg -eq 0 ]; then
    echo -e ${YLW_WARNING}: remember to avoid system installed version of ${YLW}${PKG_PROTOBUFDEV}${NC}
    dpkg -l ${PKG_PROTOBUFDEV} | grep protobuf
fi

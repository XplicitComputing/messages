#!/bin/bash

# Build and install Protocol Buffer library from Github source.
# Remove Ubuntu/Debian package first, if it exists.
# options:
#   --tag=      Checkout code at a tag on current branch (default to $TAG).
#   --notag     Checkout HEAD (latest); no tag.
#   --prefix=   Install directory prefix (default to $PREFIX)
#   --noclean   Do not remove build directory after build and install.
#
# Examples:
#   ./install-protobuf.sh --notag --noclean         # Use latest source, do not remove build.
#   ./install-protobuf.sh --prefix=/opt/xcompute    # Set install target to /opt/xcompute
#   ./install-protobuf.sh CXX=clang++               # Pass CXX through to package build.
#

RED='\033[1;31m'        # bold red
YLW='\033[1;33m'        # bold yellow
NC='\033[0m'            # No Color
YLW_WARNING="${RED}[${YLW}WARNING${RED}]${NC}"

PKG_PROTOBUFDEV=libprotobuf-dev                 # dev package on Ubuntu/Debian
TAG=3.8.0                                   # Known recent release tag
PREFIX=/usr/local   # Install target prefix
CLEANUP=1
CFGARGS="CFLAGS=-fPIC CXXFLAGS=-fPIC"

while [ $# -gt 0 ]; do
    case "$1" in
      --tag=*)
        TAG="${1#*=}"
        ;;
      --prefix=*)
        PREFIX="${1#*=}"
        ;;
      --notag)
        TAG=
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

if [ \! -d ${PREFIX} ]; then
    echo Target install directory \"${PREFIX}\" does not exist.
    echo Please manually create directory with right ownership before continuing.
    exit 1
fi

echo Branch tag $TAG
echo Install target $PREFIX

echo "---- Installing Protocol Buffers ----"

dpkg -l ${PKG_PROTOBUFDEV}
rc_dpkg=$?
if [ $rc_dpkg -eq 0 ]; then
    echo -e ${YLW_WARNING}: a different version of ${YLW}${PKG_PROTOBUFDEV}${NC} is installed on your machine
    echo Currently using $(which protoc)
    echo Please make sure that the right version is found in PATH at build-time.
    sleep 2
fi

cat <<- _end_text > protobuf.md5
cc4f50740430d9488312e5e48bc94d68  protobuf-all-3.8.0.tar.gz
_end_text
md5sum -c protobuf.md5
rc_md5=$?
if [ $rc_md5 -ne 0 ]; then
    wget https://github.com/protocolbuffers/protobuf/releases/download/v${TAG}/protobuf-all-${TAG}.tar.gz
fi
tar -xf protobuf-*.tar.gz
cd protobuf-${TAG}

./autogen.sh
./configure --prefix=${PREFIX} ${CFGARGS}
make -j$(nproc)
make check -j$(nproc)

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
    rm -R protobuf*/
fi
export PATH=${PREFIX}/bin:$PATH
which protoc
protoc --version
if [ $rc_dpkg -eq 0 ]; then
    echo -e ${YLW_WARNING}: remember to avoid system installed version of ${YLW}${PKG_PROTOBUFDEV}${NC}
    dpkg -l ${PKG_PROTOBUFDEV} | grep protobuf
fi

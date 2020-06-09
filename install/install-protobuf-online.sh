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

PKG_PROTOBUFDEV=libprotobuf-dev                 # dev package on Ubuntu/Debian
TAG=v3.12.3                                      # Known recent release tag
PREFIX=/usr/local   # Install target prefix
CLEANUP=1
CFGARGS=""

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
sleep 1

echo "It's best practice to first remove existing libprotobuf-dev..."
dpkg -l ${PKG_PROTOBUFDEV}
[ $? = 0 ] && sudo apt remove ${PKG_PROTOBUFDEV}
echo "Done prepping. Now unzipping..."
sleep 1

#unzip protobuf*.zip
[ -d protobuf ] || git clone https://github.com/protocolbuffers/protobuf.git
cd protobuf
[ "${TAG}" ] && git checkout ${TAG}
git submodule update --init --recursive
./autogen.sh
./configure --prefix=${PREFIX} ${CFGARGS}
make -j$(nproc)
make check -j$(nproc)

# check if target needs 'sudo'; execute according
xcuid=$(stat -c '%u' ${PREFIX})
if [ "${xcuid}_" = "0_" ]; then
    PRIV=sudo
    RED='\033[0;31m'
    NC='\033[0m' # No Color
    echo -e "Using ${RED}sudo${NC} to install as ${RED}root${NC}."
fi
set -x
${PRIV} make install
# ldconfig for libprotobuf needs 'autoconf --disable-shared';
# tells autoconf/libtool to not build shared libraries.
[ -n "${PRIV}" ] && ${PRIV} ldconfig
cd ..

if [ "$CLEANUP" ]; then
    echo "Cleaning..."
    rm -R protobuf*/
fi
export PATH=${PREFIX}/bin:$PATH
which protoc
protoc --version

#!/bin/bash

# Build and install Protocol Buffer library from Github source.
# Remove Ubuntu/Debian package first, if it exists.
# options:
#   --prefix=   Install directory prefix (default to $PREFIX)
#   --noclean   Do not remove build directory after build and install.
#
# Examples:
#   ./install-protobuf.sh --notag --noclean         # Use latest source, do not remove build.
#   ./install-protobuf.sh --prefix=/opt/xcompute    # Set install target to /opt/xcompute
#   ./install-protobuf.sh CXX=clang++               # Pass CXX through to package build.
#

PREFIX=/usr/local   # Install target prefix
CLEANUP=1

unzip protobuf-*.zip 
cd protobuf-*

./autogen.sh
./configure --prefix=${PREFIX} 
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
    rm -R protobuf-*/
fi
export PATH=${PREFIX}/bin:$PATH
which protoc
protoc --version

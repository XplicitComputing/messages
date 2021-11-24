#!/bin/bash
echo "* * * * * * * *  XPLICIT COMPUTING BUILD ENVIRONMENT (MESSAGES) * * * * * * * * "

dpfx=

getargs() {
    while [ $# -gt 0 ]; do
        case "$1" in
          --dpfx=*)                 # dependencies prefix
            dpfx="${1#*=}"
            ;;
          *)  # unknown argument; don't care.
            ;;
        esac
        shift
    done

    if [ -z "${dpfx}" ]; then
        echo missing "dependencies prefix: --dpfx=..."
        echo For final product build, try: --dpfx=/usr/local/xcompute
        echo For development builds, try : '--dpfx=${HOME}/xcompute'
        echo ... or deps dir of your choosing
        exit 1
    fi
}

getargs $*

echo "Installing autotools..."
#brew install clang-12 libstdc++-10-dev
brew install cmake automake autoconf autogen libtool checkinstall wget curl unzip
echo "Your c++ compiler should be at least clang++ 12, provided by Apple Xcode."

echo "Your current C compiler is:"
which cc
cc --version

echo "Your current C++ compiler is:"
which c++
c++ --version

read -p "Press [enter] to proceed with Protobuf3 installation..."
sudo ./install-protobuf.sh --prefix=${dpfx}

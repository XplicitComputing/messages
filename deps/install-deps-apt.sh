#!/bin/bash

dpfx=

preface() {
cat <<EOText
* * * * * * * *  XC-MESSAGES BUILD ENVIRONMENT * * * * * * * *

This script installs the Ubuntu or Debian packages needed to build
XC-Messages from source code.

The current build environment OS is assumed to be Ubuntu 20.04.
XC-Messages is currently built using the clang++-12 compiler to allow
exploitation of numerics-oriented x86-64 microarchitectures.
(CXXFLAGS="-march=x86-64-v3" is supported by the clang-12 and gcc-11
compiler families.)

Rather than using the package manager's version of Google Protobuf,
XC-Messages currently depends on and builds Protobuf version 3.8.0.
To allow multiple versions to exist, you can specify a target install
directory prefix by "--dpfx".

Ubuntu/Debian packages will be installed using "sudo".

EOText
}

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

preface
getargs $*

echo "Installing compiler and autotools..."
sudo apt install clang clang-12
sudo apt install cmake automake autoconf autogen libtool checkinstall wget curl unzip zlib1g-dev
echo "Your current C compiler is:"
which cc
cc --version
echo "Your current C++ compiler is:"
which c++
c++ --version
echo "Please set the C compiler to clang-12 before proceeding with XC builds:"
sudo update-alternatives --install /usr/bin/cc cc /usr/bin/clang-12 60
echo "Please set the C++ compiler to clang++-12:"
sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-12 60
echo "The current C++ compiler is:"
which c++
c++ --version
read -p "Press [enter] to proceed with Protobuf3 installation..."
./install-protobuf.sh --prefix=${dpfx}

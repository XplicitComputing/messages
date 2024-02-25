#!/bin/bash

dpfx=

preface() {
cat <<EOText
* * * * * * * *  XC-MESSAGES BUILD ENVIRONMENT * * * * * * * *

This script installs the macOS Homebrew packages needed to build
XC-Messages from source code.  The build environment is assumed to be:
macOS 12.6 Monterey or later.

For best performance, XC-Messages should be built using the
microarchitecture that best approaches the numerics support of the
target CPU. This is often done by:
  export CXXFLAGS="-march=<microarchitecture>"
Possible microarchitecture values include:
* 'haswell' (AVX2) or 'cascadelake' (AVX512) 
* 'x86-64-v3' (AVX2) or 'x86-64-v4' (AVX512)
* 'native' (what compiler deems appropriate for its host machine)

The recommended C++ toolchain is the default Apple toolchain for
the target release, such as Apple Clang 14 on macOS 12.6 Monterey or later;
these particular versions provide partial support for C++20.

Rather than using the package manager's version of Google Protobuf,
XC-Messages currently depends on and builds Protobuf version 3.20.3.
To allow multiple versions to exist, you can specify a target install
directory prefix by "--dpfx".

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
        echo For development builds, try : '--dpfx=${HOME}/xcdeps'
        echo ... or deps dir of your choosing
        exit 1
    fi
}

preface
getargs $*

echo "Installing compiler and autotools..."
brew install cmake automake autoconf autogen libtool wget curl unzip
echo "Your current C compiler is:"
which cc
cc --version
echo "Your current C++ compiler is:"
which c++
c++ --version
read -p "Press [enter] to proceed with Protobuf3 installation..."
./install-protobuf-brew.sh --prefix=${dpfx} --tag=3.20.3 --md5=a1e8f594f998576180ff1efa49007f54

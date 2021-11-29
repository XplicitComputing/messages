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

echo "Installing compiler and build tools..."
sudo apt install clang-12 libstdc++-10-dev
sudo apt install cmake automake autoconf autogen libtool checkinstall wget curl unzip

echo "Your current C compiler is:"
which cc
cc --version

echo "Your current C++ compiler is:"
which c++
c++ --version

echo "Attempting to set your default C/C++ compilers to clang-12 :"
sudo update-alternatives --install /usr/bin/cc cc /usr/bin/clang-12 60
sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-12 60

echo "Your current C++ compiler is:"
which c++
c++ --version

read -p "Press [enter] to proceed with Protobuf3 installation..."
./install-protobuf.sh --prefix=${dpfx}

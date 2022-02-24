#!/bin/bash
echo "* * * * * * * *  XPLICIT COMPUTING BUILD ENVIRONMENT (MESSAGES) * * * * * * * * "

dpfx=

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
    echo For development builds, try : '--dpfx=${HOME}/xcompute' or deps dir of your choosing
    exit 1
fi
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
sudo ./install-protobuf.sh --prefix=${dpfx}

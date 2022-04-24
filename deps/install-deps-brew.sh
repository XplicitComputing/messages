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
brew install cmake automake autoconf autogen libtool checkinstall wget curl unzip
echo "Your current C compiler is:"
which cc
cc --version
echo "Your current C++ compiler is:"
which c++
c++ --version
#echo "Consider setting your default compiler to clang-12 before proceeding with XC builds:"
#echo "Please set your default C compiler to clang-12 :"
#sudo update-alternatives --config cc
#sudo update-alternatives --install /usr/bin/cc cc /usr/bin/clang-12 60
#echo "Please set your default C++ compiler to clang++-12 :"
#sudo update-alternatives --config c++
#sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-12 60
#echo "Your current C++ compiler is:"
#which c++
#c++ --version
read -p "Press [enter] to proceed with Protobuf3 installation..."
./install-protobuf.sh --prefix=${dpfx}

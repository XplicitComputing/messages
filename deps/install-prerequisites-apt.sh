#!/bin/bash
echo "Installing compiler and autotools..."
sudo apt install clang++-10 cmake automake autoconf autogen libtool checkinstall wget curl unzip
echo "Your current C compiler is:"
which cc
cc --version
echo "Your current C++ compiler is:"
which c++
c++ --version
#echo "Consider setting your default compiler to clang-10 before proceeding with XC builds:"
#echo "sudo update-alternatives --config cc" 
#echo "sudo update-alternatives --config c++" 
#echo ", or manually overriding with soft link:" 
#echo "sudo ln -sf /usr/bin/clang-10 /usr/bin/cc"
#echo "sudo ln -sf /usr/bin/clang++-10 /usr/bin/c++"
echo "Please set your default C compiler to clang-10 :"
sudo update-alternatives --config cc
echo "Please set your default C++ compiler to clang++-10 :"
sudo update-alternatives --config c++

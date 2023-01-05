#!/bin/bash

while [ $# -gt 0 ]; do
    # echo arg $1
    case "$1" in
        TC=*)   # alternative to TOOLCHAIN env var
            TOOLCHAIN=${1/TC=/}
            ;;
        -q)     # quick/quiet mode, no prompts/no pause
            qq=qqq
            ;;
    esac
    shift
done

# GOOGLE_ROOT points to the root of where the following are found:
#   include/google/protobuf
#   lib/libprotobuf*
if [ "${GOOGLE_ROOT}" ]; then
    LPROTOFLAGS="-L${GOOGLE_ROOT}/lib"
fi

case "$TOOLCHAIN" in
    Clang|CLANG)
        CXX=clang++
        # CXXFLAGS="stdlib=libc++ -lc++abi -lsupc++ -lc++fs"     # for some systems
        ;;
    Gcc|GCC) CXX=g++
        ;;
    *) CXX=c++
        ;;
esac

if [ ! "${qq}" ];  then
    echo "compiling messages example app: 'hello_vector' ..."
    read -p "Press [enter] to continue."

    ${CXX} --version    # print compiler version
fi

ln -f ../../../bindings/cpp/vector.pb.h
ln -f ../../../lib/static/libxcmessages.a

set -x
# invoke CXX compiler on save_vector source to create app:
${CXX} ${CXXFLAGS} save_vector.cpp -o save_vector -L. -lxcmessages ${LPROTOFLAGS} -lprotobuf
# invoke CXX compiler on load_vector source to create app:
${CXX} ${CXXFLAGS} load_vector.cpp -o load_vector -L. -lxcmessages ${LPROTOFLAGS} -lprotobuf
set +x

if [ "${qq}" ]; then
    exit 0
fi

ls -l save_vector load_vector

cat <<-Build_Epilog
The apps will use the shared-object version of libprotobuf.
Your LD_LIBRARY_PATH has to be appropriately set to find libprotobuf.so.
If in doubt of where the apps are finding it, run these commands:
    ldd save_vector
    ldd load_vector

Build_Epilog

read -p "Press [enter] to close."

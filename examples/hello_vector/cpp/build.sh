#!/bin/bash


# ---- PROCESS CMDLINE --------------------------------------- #

quiet=false
force_quiet=false
while [ $# -gt 0 ]; do
    # echo arg $1
    case "$1" in
        TC=*)
            TOOLCHAIN=${1/TC=/}
            ;;
        -q)
            quiet=true
            force_quiet=false
            ;;
    esac
    shift
done


# ---- TRY TO INFER SOMETHING FROM USER'S ENVIRONMENT -------- #

if [[ -z "${TC}" ]]; then
    TC=${TOOLCHAIN}

    # we have a 'TOOLCHAIN' env.var set, then we are a non-default setup, and know what we want!
    #
    if [[ ! -z "${TC}" ]]; then
        quiet=true
        force_quiet=true
        if [[ "${TC}" == "Clang" ]]; then
            CXXFLAGS="-stdlib=libc++ -lc++abi -lsupc++ -lc++fs"
        fi
    fi
fi


# ---- COMPILER ---------------------------------------------- #

case "$TOOLCHAIN" in
    Clang|CLANG)
        CXX=clang++
        ;;
    Gcc|GCC) CXX=g++
        ;;
    *) CXX=c++
        ;;
esac


# ---- INFER LIBRARY PATH ------------------------------------ #

# GOOGLE_ROOT points to the root of where the following are found:
#   include/google/protobuf
#   lib/libprotobuf*
#
LD_LIB_PATH=.
if [[ ! -z "${GOOGLE_ROOT}" ]]; then
    LD_LIB_PATH="${LD_LIB_PATH}:${GOOGLE_ROOT}/lib"

elif [[ ! -z "${LD_LIBRARY_PATH}" ]]; then

    LD_LIB_PATH="${LD_LIB_PATH}:${LD_LIBRARY_PATH}"
fi
LD_LIB_PATH=-L`echo ${LD_LIB_PATH} | sed -e 's/:/ -L/g'`


# ---- OPTIONAL INFO ----------------------------------------- #

if [[ "${quiet}" == "false" ]];  then
    echo "compiling messages example app: 'hello_vector' ..."
    read -p "Press [enter] to continue."

    ${CXX} --version    # print compiler version
fi


# ---- SETUP & BUILD ----------------------------------------- #

ln -fs ../../../bindings/cpp/vector.pb.h
ln -fs ../../../lib/static/libxcmessages.a

if [[ "${force_quiet}" == "false" ]]; then
    set -x
fi

# save_vector:
${CXX} ${CXXFLAGS} save_vector.cpp -o save_vector ${LD_LIB_PATH} -lxcmessages -lprotobuf

# load_vector:
${CXX} ${CXXFLAGS} load_vector.cpp -o load_vector ${LD_LIB_PATH} -lxcmessages -lprotobuf

{ set +x; } 2>/dev/null


# ---- QUIT EARLY? ------------------------------------------ #

if [[ "${quiet}" == "true" ]]; then
    exit 0
fi


# ---- OPTIONAL INFO ---------------------------------------- #

ls -l save_vector load_vector

cat <<-Build_Epilog
The apps will use the shared-object version of libprotobuf.
Your LD_LIBRARY_PATH has to be appropriately set to find libprotobuf.so.
If in doubt of where the apps are finding it, run these commands:
    ldd save_vector
    ldd load_vector
Build_Epilog

read -p "Press [enter] to close."


# ----------------------------------------------------------- #

#!/bin/bash


LANGUAGES='cpp csharp java javascript objc python ruby php' # go dart julia perl c r rust scala swift

have_c=$(which protoc-c 2>/dev/null || echo false)
if [[ "${have_c}" != "false" ]]; then
    have_c=true
    LANGUAGES='c cpp csharp java javascript objc python ruby php' # go dart julia perl r rust scala swift
fi


if [ "${XC_BUILD_TREE}" = "" ]; then
    XC_BUILD_TREE=.
fi
BDIR=${XC_BUILD_TREE}/bindings


quiet=
cmpargs=
external_init=
while [ $# -gt 0 ]; do
    case "$1" in
        CC=*)
            cmpargs="${cmpargs} $1"
            ;;

        clean)
            if [[ -f Makefile ]]; then
                make clean
            fi
            rm -rf CMakeFiles CMakeCache.txt cmake_install.cmake Makefile
            rm -rf ${BDIR}/${LANGUAGES}
            exit 0
            ;;

        -e|--external)
            external_init=true
            ;;

        -q|--quiet)
            quiet=true
            ;;

        *)        # currently, nothing else passes through
            ;;
    esac
    shift
done



echo "Compiling protobuf message bindings for:  ${LANGUAGES}"
if [[ -z "${quiet}" ]]; then
    [ "$iact" = off ] || read -p "Press [enter] to continue."
fi



# prep cmake environment in current directory
if [[ -z "${external_init}" ]]; then

    # if .buildopts exists, use it
    [ -f .buildopts ] && . .buildopts

    cmake .

    # parallel build make - find ncpu
    ncpu=1 #default
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        ncpu=$(nproc)
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        ncpu=$(sysctl -n hw.ncpu)
    fi
    nrs=1           # CPUs reserved for non-build
    ncpu=$(( (ncpu>nrs)? ncpu-nrs : 1 ))

    # build the protocol buffers that are used by xcompute directly
    #utils/cmakeprep.sh ${cmpargs} .
    make -j${ncpu}
fi


# build the protobuf bindings that will be used by users and developers to integrate
EscBDIR=$(echo "${XC_BUILD_TREE}/bindings" | sed -e 's/\//\\\//g')
directories=`echo "${LANGUAGES}" | sed -e "s/[^ ]* */${EscBDIR}\/&/g"`
mkdir -p $directories
protoc  --proto_path=. \
        --cpp_out=${BDIR}/cpp \
        --python_out=${BDIR}/python \
        --java_out=${BDIR}/java \
        --js_out=${BDIR}/javascript,import_style=commonjs,binary:. \
        --ruby_out=${BDIR}/ruby \
        --objc_out=${BDIR}/objc \
        --csharp_out=${BDIR}/csharp \
        --php_out=${BDIR}/php \
        vector.proto setup.proto spatial.proto meta.proto

if [[ "${have_c}" == "true" ]]; then
protoc-c --proto_path=. \
         --c_out=${BDIR}/c \
         vector.proto setup.proto spatial.proto meta.proto
fi


if [[ -z "${quiet}" ]]; then
    [ "$iact" = off ] || read -p "Press [enter] to continue."
fi

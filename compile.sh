#!/bin/bash


LANGUAGES='cpp csharp java javascript objc php python ruby' # dart go julia perl r rust scala swift
PROTOFILES='vector.proto concept.proto spatial.proto meta.proto'
PB_STEMS=$(echo $PROTOFILES | sed s/\.proto//g)

have_c=$(which protoc-c 2>/dev/null || echo false)
if [[ "${have_c}" != "false" ]]; then
    have_c=true
    LANGUAGES="c ${LANGUAGES}"
fi


if [ "${MSGS_BUILD_TREE}" = "" ]; then
    MSGS_BUILD_TREE=.
fi
BNDR=${MSGS_BUILD_TREE}/bindings


iact=
external_init=
while [ $# -gt 0 ]; do
    case "$1" in
        clean)
            echo "cleaning messages..."
            if [[ -f Makefile ]]; then
                make clean
            fi
            rm -rf CMakeFiles CMakeCache.txt cmake_install.cmake Makefile
            for lang in ${LANGUAGES}; do
                rm -rf ${BNDR}/${lang}
            done
            [ -d ${BNDR} ] && rmdir ${BNDR}
            exit 0
            ;;

        -e|--external)
            external_init=true
            ;;

        -i|--interactive)
            iact=true
            ;;

        *)        # currently, nothing else passes through
            ;;
    esac
    shift
done



echo "Compiling protobuf message bindings for:  ${LANGUAGES}"
[ "${iact}" ] && read -p "Press [enter] to continue."

# prep cmake environment in current directory
if [[ -z "${external_init}" ]]; then

    echo == xcmessages CMAKE_FLAGS: ${CMAKE_FLAGS}
    cmake ${CMAKE_FLAGS} .

    # parallel build make - find ncpu
    ncpu=1 #default
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        ncpu=$(nproc)
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        ncpu=$(sysctl -n hw.ncpu)
    fi
    nrs=1           # CPUs reserved for non-build
    ncpu=$(( (ncpu>nrs)? ncpu-nrs : 1 ))

    for stem in ${PB_STEMS}; do
        protoc --cpp_out . -I . ${stem}.proto
        #sed --in-place 's \bfinal\b /*final*/ ' ${stem}.pb.h   # in case removing 'final' is desired.
    done
    rm -rf CMakeFiles CMakeCache.txt

    make -j${ncpu}
fi

# build the protobuf bindings that will be used by users and developers to integrate
EscBNDR=$(echo "${MSGS_BUILD_TREE}/bindings" | sed -e 's/\//\\\//g')
directories=`echo "${LANGUAGES}" | sed -e "s/[^ ]* */${EscBNDR}\/&/g"`
mkdir -p $directories

if [[ "${have_c}" == "true" ]]; then
protoc-c --proto_path=. \
         --c_out=${BNDR}/c \
         vector.proto concept.proto spatial.proto meta.proto
fi

protoc  --proto_path=. \
        --cpp_out=${BNDR}/cpp \
        --csharp_out=${BNDR}/csharp \
        --java_out=${BNDR}/java \
        --js_out=${BNDR}/javascript,import_style=commonjs,binary:. \
        --objc_out=${BNDR}/objc \
        --php_out=${BNDR}/php \
        --python_out=${BNDR}/python \
        --ruby_out=${BNDR}/ruby \
        vector.proto concept.proto spatial.proto meta.proto


[ "${iact}" ] && read -p "Press [enter] to continue."

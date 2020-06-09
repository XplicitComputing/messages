#!/bin/bash


languages='cpp csharp java javascript objc python ruby php' # go dart julia perl c r rust scala swift

echo "Compiling protocol buffer messages for: $languages"
[ "$iact" = off ] || read -p "Press [enter] to continue."

#prep cmake environment in current directory
do_clean() {
    make clean
    rm -rf CMakeFiles CMakeCache.txt cmake_install.cmake Makefile
    rm -rf bindings/${languages}
}

# if .buildopts exists, use it
[ -f .buildopts ] && . .buildopts

cmpargs=
while [ $# -gt 0 ]; do
    case "$1" in
      CC=*)
        cmpargs="${cmpargs} $1"
        ;;
      clean)
        do_clean
        exit 0
        ;;
      *)        # currently, nothing else passes through
        ;;
    esac
    shift
done
cmake .
# keep CPU in reserve for non-build
ncpu=$(nproc)
nrs=1           # CPUs reserved for non-build
ncpu=$(( (ncpu>nrs)? ncpu-nrs : 1 ))
#build the protocol buffers that are used by xcompute directly
#utils/cmakeprep.sh ${cmpargs} .
make -j${ncpu}

#build the protobuf bindings that will be used by users and developers to integrate
mkdir -p ${languages} #temporary directory for each language
protoc --cpp_out=cpp --python_out=python --java_out=java --js_out=javascript,import_style=commonjs,binary:. --ruby_out=ruby --objc_out=objc --csharp_out=csharp --php_out=php vector.proto system.proto spatial.proto meta.proto
mkdir -p bindings
for lang in $languages
do
	cp -rf $lang bindings/ #move under bindings
	rm -rf $lang
done


[ "$iact" = off ] || read -p "Press [enter] to continue."

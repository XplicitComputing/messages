#!/bin/bash


if [[ ! -z "${GOOGLE_ROOT}" ]]; then

    ln -fs ../../vector.pb.h

    if [[ "$TOOLCHAIN" == "Clang" ]]; then
        clang++ -stdlib=libc++ -lc++abi -lsupc++ -lc++fs save_vector.cpp -o save_vector -L../../lib/static -lxcmessages -L${GOOGLE_ROOT}/lib -lprotobuf
        clang++ -stdlib=libc++ -lc++abi -lsupc++ -lc++fs load_vector.cpp -o load_vector -L../../lib/static -lxcmessages -L${GOOGLE_ROOT}/lib -lprotobuf
    else
        g++ save_vector.cpp -o save_vector -L../../lib/static -lxcmessages -L${GOOGLE_ROOT}/lib -lprotobuf
        g++ load_vector.cpp -o load_vector -L../../lib/static -lxcmessages -L${GOOGLE_ROOT}/lib -lprotobuf
    fi

else

    echo "compiling messages example app: 'hello_vector' ..."
    read -p "Press [enter] to continue."

    # make links to required library and (for convenience)
    ln -f ../../lib/static/libxcmessages.a libxcmessages.a
    ln -f ../../vector.pb.h vector.pb.h

    # invoke c++ compiler on save source to create app:
    c++ save_vector.cpp -o save_vector libxcmessages.a -lprotobuf

    # invoke c++ compiler on load source to create app:
    c++ load_vector.cpp -o load_vector libxcmessages.a -lprotobuf

    read -p "Press [enter] to close."
fi

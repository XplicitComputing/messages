#!/bin/bash


if [[ ! -z "${GOOGLE_ROOT}" ]]; then

    ln -fs ../../vector.pb.h

    if [[ "$TOOLCHAIN" == "Clang" ]]; then
        clang++ -stdlib=libc++ -lc++abi -lsupc++ -lc++fs xco2csv.cpp -o xco2csv -L../.. -lxcmessages -L${GOOGLE_ROOT}/lib -lprotobuf
    else
        g++ xco2csv.cpp -o xco2csv -L../.. -lxcmessages -L${GOOGLE_ROOT}/lib -lprotobuf
    fi

else

    # make links to required library and (for convenience)
    ln -f ../../libxcmessages.a libxcmessages.a
    ln -f ../../vector.pb.h vector.pb.h

    # invoke c++ compiler on source to create app:
    c++ xco2csv.cpp -o xco2csv libxcmessages.a -lprotobuf
    read -p "Press [enter] to close."
fi

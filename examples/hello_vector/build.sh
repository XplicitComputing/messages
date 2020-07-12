#!/bin/bash


if [[ ! -z "${GOOGLE_ROOT}" ]]; then

    ln -fs ../../vector.pb.h

    c++ save_msg.cpp -o save_app -L../.. -lxcmessages -L${GOOGLE_ROOT}/lib -lprotobuf
    c++ load_msg.cpp -o load_app -L../.. -lxcmessages -L${GOOGLE_ROOT}/lib -lprotobuf

else

    # make links to required library and (for convenience)
    ln -f ../../libxcmessages.a libxcmessages.a
    ln -f ../../vector.pb.h vector.pb.h

    # invoke c++ compiler on save source to create app:
    c++ save_msg.cpp -o save_app libxcmessages.a -lprotobuf

    # invoke c++ compiler on load source to create app:
    c++ load_msg.cpp -o load_app libxcmessages.a -lprotobuf

    read -p "Press [enter] to close."
fi

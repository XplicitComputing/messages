#!/bin/bash

# make links to required library and header (for convenience)
ln -f ../../libxcmessages.a libxcmessages.a
ln -f ../../vector.pb.h vector.pb.h

# invoke c++ compiler on save source to create app:
c++ save_msg.cpp -o save_app libxcmessages.a -lprotobuf
# invoke c++ compiler on load source to create app:
c++ load_msg.cpp -o load_app libxcmessages.a -lprotobuf

read -p "Press [enter] to close."

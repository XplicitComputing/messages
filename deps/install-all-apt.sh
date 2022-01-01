#!/bin/bash

dpfx=

getargs() {
    while [ $# -gt 0 ]; do
        case "$1" in
          --dpfx=*)                 # dependencies prefix
            dpfx="${1#*=}"
            ;;
          *)  # unknown argument; don't care.
            ;;
        esac
        shift
    done

    if [ -z "${dpfx}" ]; then
        echo missing "dependencies prefix: --dpfx=..."
        echo For final product build, try: --dpfx=/usr/local/xcompute
        echo For development builds, try : '--dpfx=${HOME}/xcompute'
        echo ... or deps dir of your choosing
        exit 1
    fi
}

getargs $*

./install-deps-apt.sh --dpfx=${dpfx}
./install-plugin-c.sh
./install-plugin-go.sh
./install-plugin-dart.sh

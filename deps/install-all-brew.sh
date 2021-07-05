#!/bin/bash
sudo ./install-deps-brew.sh --dpfx=/usr/local
sudo ./install-protobuf.sh --prefix=/usr/local
sudo ./install-plugin-c.sh
sudo ./install-plugin-go.sh
sudo ./install-plugin-dart.sh

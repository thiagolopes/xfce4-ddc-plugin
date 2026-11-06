#!/bin/bash
set -ex

if [[ ! -d "./build/" ]]; then
    meson setup build --prefix=/usr/ --reconfigure
fi

meson compile -C build
meson install -C build
xfce4-panel -r

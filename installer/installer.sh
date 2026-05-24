#!/bin/bash
set -euo pipefail

read -rp "Enter your Oneshot 2016 installation path: " oneshot_path
read -rp "Enter path for installation: " install_path

mkdir -p $install_path
cp -r "$oneshot_path/*" "$install_path/"
cp -r "Sunshine/*" "$install_path/"

echo """[Desktop Entry]
Type=Application
Name=Oneshot: Sunshine
Comment=Oneshot mod
Exec=$install_path/oneshot
Icon=$install_path/icon.png
Categories=Game;
""" > $HOME/.local/share/applications/sunshine.desktop

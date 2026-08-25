#!/bin/sh
set -euo pipefail
curl --parallel --tcp-nodelay --tcp-fastopen --fail -s https://raw.githubusercontent.com/mdqinc/SDL_GameControllerDB/refs/heads/master/gamecontrollerdb.txt > assets/gamecontrollerdb.txt

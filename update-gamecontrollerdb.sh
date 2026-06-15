#!/bin/sh
set -euo pipefail
curl -s https://raw.githubusercontent.com/mdqinc/SDL_GameControllerDB/refs/heads/master/gamecontrollerdb.txt > assets/gamecontrollerdb.txt

#!/bin/bash
mapfile -d '' -t scripts < <(find "$2" -type f -name '*.rb' -print0)
mrbc -g -o "$1" "${scripts[@]}"

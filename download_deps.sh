#!/bin/bash
git submodule update --init --recursive --depth 10 --single-branch --jobs $(nproc)
git submodule foreach --recursive git pull

#!/bin/bash
git submodule update --init --recursive --single-branch --jobs $(nproc)
git submodule foreach --recursive git pull

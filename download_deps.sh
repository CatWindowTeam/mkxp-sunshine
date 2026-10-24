#!/bin/bash
git submodule update --init --recursive --depth 1 --single-branch
git submodule foreach --recursive git pull

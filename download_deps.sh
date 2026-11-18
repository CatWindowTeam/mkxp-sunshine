#!/bin/bash
git submodule update --init --recursive --single-branch
git submodule foreach --recursive git pull

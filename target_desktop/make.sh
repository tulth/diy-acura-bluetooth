#!/bin/bash
set -e
cmake -H"." \
      -B"./build"
make -C ./build $@

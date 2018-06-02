#!/bin/bash
set -e
teensy-loader-cli --mcu=mkl26z64 -v -s -w $@

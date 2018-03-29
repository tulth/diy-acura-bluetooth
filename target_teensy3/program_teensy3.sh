#!/bin/bash
set -e
teensy-loader-cli --mcu=mk20dx256 -v -s -w $@

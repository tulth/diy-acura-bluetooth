#!/bin/env python
from __future__ import print_function
import sys
import os


BIT_TIME = 3100
BIT_ZERO_LOW_TIME = 675
BIT_ONE_LOW_TIME = 1890
BIT_ONEVAL_THRESH_TIME = (BIT_ZERO_LOW_TIME + BIT_ONE_LOW_TIME) >> 1
BIT_LOW_TOO_LONG_TIME = (BIT_TIME + BIT_ONE_LOW_TIME) >> 1
NIBBLE_END_GAP_TIME = (BIT_TIME)
INTERBIT_TIMEOUT_TIME = (BIT_TIME)

MBUS_LOW_TOO_LONG_CODE = 16
MBUS_TIMEOUT_CODE = 17
MBUS_END_MSG_CODE = 18

if __name__ == '__main__':
    print("Library only.")

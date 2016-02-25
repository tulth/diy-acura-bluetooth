#!/bin/env python
from __future__ import print_function
import sys
import os
import ctypes

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

def nibbleSeq2Str(nibbleSeq):
    return "0x" + "".join(["{:x}".format(nibble) for nibble in nibbleSeq])

class circular_buffer(ctypes.Structure):
    _fields_ = [
        ("buffer", ctypes.c_void_p),
        ("bufferEnd", ctypes.c_void_p),
        ("capacity", ctypes.c_size_t),
        ("count", ctypes.c_size_t),
        ("elementSize", ctypes.c_size_t),
        ("head", ctypes.c_void_p),
        ("tail", ctypes.c_void_p),
        ]

    def __str__(self):
        return "circular_buffer: count {}".format(self.count)


class MbusPhyTxRxStruct(ctypes.Structure):
    _fields_ = [
        ("state", ctypes.c_uint8),
        ("bitShifter", ctypes.c_uint8),
        ("microSecTimeStamp", ctypes.c_ulong),
        ("byteFifo", circular_buffer),
        ]

    def __str__(self):
        return "state{} bitshftr{} usecTS{} {}".format(self.state, self.bitShifter,
                                                       self.microSecTimeStamp, self.byteFifo)


class MbusPhyStruct(ctypes.Structure):
    _fields_ = [
        ("tx", MbusPhyTxRxStruct),
        ("rx", MbusPhyTxRxStruct),
        ]

    def __str__(self):
        return "tx {} / rx {}".format(self.tx, self.rx)

if __name__ == '__main__':
    print("Library only.")

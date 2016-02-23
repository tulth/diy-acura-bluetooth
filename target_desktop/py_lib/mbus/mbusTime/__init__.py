#!/bin/env python
from __future__ import print_function
import sys
import os
import datetime
from .. import toggles

MAX_ARRAY_SIZE = 32


MBUS_LOW_TOO_LONG_CODE = 16
MBUS_TIMEOUT_CODE = 17
MBUS_END_MSG_CODE = 18

MBUS_TIME_FORMATS = ("pyTimeElapsed", "microsHex", "blank")

class MbusTime(object):
    def __init__(self, val, mbusTimeFormat="pyTimeElapsed"):
        super().__init__()
        assert mbusTimeFormat in MBUS_TIME_FORMATS
        assert isinstance(val, int)
        self.mbusTimeFormat = mbusTimeFormat
        self.val = int(val)
    
    def __str__(self):
        if self.mbusTimeFormat == "pyTimeElapsed":
            dtobj = datetime.timedelta(microseconds=self.val)
            return "{}".format(dtobj)
        elif self.mbusTimeFormat == "microsHex":
            return "{:x}".format(self.val)
        else:
            return ""

    def fromStr(self, argStr):
        """returns the unused portion of argStr!"""
        if self.mbusTimeFormat == "blank":
            return argStr
        argSplit = argStr.split(" ")
        print(argSplit)
        assert len(argSplit) > 0
        timeStr = argSplit[0]
        remainder = " ".join(argSplit[1:])
        if self.mbusTimeFormat == "pyTimeElapsed":
            self.val = datetime.datetime.strptime(timeStr, "%H:%M:%S.%f", ).microsecond
        elif self.mbusTimeFormat == "microsHex":
            self.val = int(timeStr, 16)
        return remainder

    def __add__(self, other):
        if isinstance(other, int):
            return MbusTime(self.val + val, mbusTimeFormat=self.mbusTimeFormat)
        elif isinstance(other, MbusTime):
            return MbusTime(self.val + other.val, mbusTimeFormat=self.mbusTimeFormat)
        else:
            raise NotImplementedError()


if __name__ == '__main__':
    print("library only!")
    sys.exit(1)

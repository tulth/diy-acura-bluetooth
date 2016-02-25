#!/bin/env python
from __future__ import print_function
import sys
import os
import datetime
from .. import toggles

MBUS_TIME_FORMATS = ("pyTimeElapsed", "microsHex", "microsInt", "blank")

TIMEZERO = datetime.datetime.strptime("0:0:00.0", "%H:%M:%S.%f", )

class MbusTime(object):

    def __init__(self, val=0, mbusTimeFormat="pyTimeElapsed"):
        super().__init__()
        assert mbusTimeFormat in MBUS_TIME_FORMATS
        assert isinstance(val, int)
        self.mbusTimeFormat = mbusTimeFormat
        self.val = int(val)

    def setMbusTimeFormat(self, mbusTimeFormat):
        self.mbusTimeFormat = mbusTimeFormat

    def __str__(self):
        return self.toStr()

    def toStr(self, mbusTimeFormat=None):
        if mbusTimeFormat is None:
            mbusTimeFormat = self.mbusTimeFormat
        if mbusTimeFormat == "pyTimeElapsed":
            dtobj = datetime.timedelta(microseconds=self.val)
            return "{}".format(dtobj)
        elif mbusTimeFormat == "microsInt":
            return "{:d}".format(self.val)
        elif mbusTimeFormat == "microsHex":
            return "{:x}".format(self.val)
        else:
            return ""

    def fromTime(self, other):
        self.val = other.val

    def fromInt(self, argInt):
        self.val = int(argInt)

    def fromStr(self, argStr):
        """returns the unused portion of argStr!"""
        if self.mbusTimeFormat == "blank":
            return argStr
        argSplit = argStr.split(" ")
        assert len(argSplit) > 0
        timeStr = argSplit[0]
        remainder = " ".join(argSplit[1:])
        if self.mbusTimeFormat == "pyTimeElapsed":
            val = datetime.datetime.strptime(timeStr, "%H:%M:%S.%f", ) - TIMEZERO
            self.val = int(1000000 * val.total_seconds())
        elif self.mbusTimeFormat == "microsInt":
            self.val = int(timeStr)
        elif self.mbusTimeFormat == "microsHex":
            self.val = int(timeStr, 16)
        return remainder

    def __add__(self, other):
        if isinstance(other, int):
            return MbusTime(self.val + other, mbusTimeFormat=self.mbusTimeFormat)
        elif isinstance(other, MbusTime):
            return MbusTime(self.val + other.val, mbusTimeFormat=self.mbusTimeFormat)
        else:
            raise NotImplementedError()

    def __sub__(self, other):
        if isinstance(other, int):
            return MbusTime(self.val - other, mbusTimeFormat=self.mbusTimeFormat)
        elif isinstance(other, MbusTime):
            return MbusTime(self.val - other.val, mbusTimeFormat=self.mbusTimeFormat)
        else:
            raise NotImplementedError()


def fromStr_MbusTime(argStr, mbusTimeFormat="microsHex"):
    obj = MbusTime(mbusTimeFormat)
    obj.fromStr(argStr)
    return obj


if __name__ == '__main__':
    print("library only!")
    sys.exit(1)

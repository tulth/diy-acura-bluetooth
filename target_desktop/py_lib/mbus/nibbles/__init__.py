#!/bin/env python
from __future__ import print_function
import sys
import os
import datetime
import string
import ctypes
from .. import toggles
from .. import *

MAX_ARRAY_SIZE = 32


class MbusRawNibbleListStruct(ctypes.Structure):
    _fields_ = [
        ("nibbles", ctypes.c_byte * MAX_ARRAY_SIZE),
        ("numNibbles", ctypes.c_uint8),
        ]

    def __init__(self, mbusTimeFormat="pyTimeElapsed"):
        super().__init__()
        self.time = mbusTime.MbusTime(val=0, mbusTimeFormat=mbusTimeFormat)
        self.numNibbles=0

    def nibbleSeq2Str(self, nibbleSeq):
        return "".join(["{:x}".format(nibble) for nibble in nibbleSeq])

    def __str__(self):
        valSeq = tuple([self.nibbles[num] for num in range(self.numNibbles)])
        valStr = self.nibbleSeq2Str(valSeq)
        return " ".join((str(self.time), str(valStr))).lstrip()

    def fromStr(self, argStr):
        valStr = self.time.fromStr(argStr)
        nibbleSeq = self.hexStr2NibbleSeq(valStr)
        assert len(nibbleSeq) < MAX_ARRAY_SIZE
        self.numNibbles = len(nibbleSeq)
        for idx, nibble in enumerate(nibbleSeq):
            self.nibbles[idx] = ctypes.c_byte(nibble)

    def append(self, argHexNibbleChar):
        nib = self.hexChar2Nibble(argHexNibbleChar)
        self.nibbles[self.numNibbles] = nib
        self.numNibbles += 1
        
    def hexChar2Nibble(self, argChar):
        if argChar in string.hexdigits:
            return (int(argChar, 16))
        elif argChar == 'L':
            return (MBUS_LOW_TOO_LONG_CODE)
        elif argChar == 'T':
            return (MBUS_TIMEOUT_CODE)
        else:
            return (0xFF)
        
    def hexStr2NibbleSeq(self, argStr):
        nibbleSeq = []
        for char in argStr:
            nibbleSeq.append(self.hexChar2Nibble(char))
        return nibbleSeq

    def toToggles(self, ):
        currentTimeUs = self.time - 4 * self.numNibbles * BIT_TIME + NIBBLE_END_GAP_TIME
        toggleList = toggles.ToggleList()
        for idx in range(self.numNibbles):
            nibble = int(self.nibbles[idx])
            for bit in "{:04b}".format(nibble):
                # print("{:10}: drop nibble{} bit{}".format(currentTimeUs, nibble, bit))
                toggleList.append(toggles.fromTimeValStr_ToggleElement(currentTimeUs, '0', "microsInt"))
                if bit == '0':
                    currentTimeUs += BIT_ZERO_LOW_TIME
                else:
                    currentTimeUs += BIT_ONE_LOW_TIME
                # print("{:10}: rise".format(currentTimeUs))
                toggleList.append(toggles.fromTimeValStr_ToggleElement(currentTimeUs, '1', "microsInt"))
                if bit == '0':
                    currentTimeUs += (BIT_TIME - BIT_ZERO_LOW_TIME)
                else:
                    currentTimeUs += (BIT_TIME - BIT_ONE_LOW_TIME)
        return toggleList


def fromStr_MbusRawNibbleListStruct(argStr, mbusTimeFormat="pyTimeElapsed"):
    obj = MbusRawNibbleListStruct(mbusTimeFormat)
    obj.fromStr(argStr)
    return obj


class MbusRawNibbleListStructList(list):

    def __init__(self, mbusTimeFormat="pyTimeElapsed"):
        self.mbusTimeFormat = mbusTimeFormat
        super().__init__()

    def fromFileName(self, fileName):
        with open(fileName, 'r') as fileHandle:
            self.fromFileHandle(fileHandle)

    def readLineFromFileHandle(self, fileHandle):
        line = None
        fileReadDone = False
        if fileHandle.tell() == os.fstat(fileHandle.fileno()).st_size:
            fileReadDone = True
        else:
            charList = [fileHandle.read(1)]
            while charList[-1] != '\n':
                if fileHandle.tell() == os.fstat(fileHandle.fileno()).st_size:
                    fileReadDone = True
                    break
                charList.append(fileHandle.read(1))
            line = "".join(charList)
        return line, fileReadDone

    def fromFileHandle(self, fileHandle):
        fileReadDone = False
        while not(fileReadDone):
            line, fileReadDone = self.readLineFromFileHandle(fileHandle)
            if line is not None:
                line = line.rstrip()
                self.append(fromStr_MbusRawNibbleListStruct(line, mbusTimeFormat=self.mbusTimeFormat))

    def toFileName(self, fileName):
        """if none, prints to stdout"""
        with (open(fileName, "w") if fileName is not None else sys.stdout) as fileHandle:
            self.toFileHandle(fileHandle)

    def toFileHandle(self, fileHandle):
        for togElem in self:
            fileHandle.write("{}\n".format(togElem))

    def __str__(self):
        resultList = []
        for elem in self:
            resultList.append(str(elem))
        return "\n".join(resultList)

    def toTogglesFileName(self, fileName):
        """if none, prints to stdout"""
        with (open(fileName, "w") if fileName is not None else sys.stdout) as fileHandle:
            self.toTogglesFileHandle(fileHandle)

    def toTogglesFileHandle(self, fileHandle):
        fileHandle.write("{}\n".format(self.toToggles()))

    def toToggles(self):
        toggleList = toggles.ToggleList()
        for nibbleList in self:
            toggleList.extend(nibbleList.toToggles())
        return toggleList

    def toVcdFileName(self, fileName):
        self.toToggles().toVcdFileName(fileName)
        toggleList = toggles.ToggleList()
        for nibbleList in self:
            toggleList.extend(nibbleList.toToggles())
        return toggleList


if __name__ == '__main__':
    print("library only!")
    sys.exit(1)

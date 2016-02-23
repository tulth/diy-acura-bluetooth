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

TIMEZERO = datetime.datetime.strptime("0:0:00.0", "%H:%M:%S.%f", )

class MbusRawNibbleListStruct(ctypes.Structure):
    _fields_ = [
        ("nibbles", ctypes.c_byte * ARRAY_SIZE),
        ("numNibbles", ctypes.c_uint8),
        ]

    def __init__(self, timeStampType="pyTimeElapsed"):
        assert timeStampType in ("pyTimeElapsed", "microsHex", None)
        self.timeStampType = timeStampType
        super().__init__()
        self.time = None

    def nibbleSeq2Str(self, nibbleSeq):
        return "0x" + "".join(["{:x}".format(nibble) for nibble in nibbleSeq])

    def __str__(self):
        valSeq = tuple([self.nibbleArray[num] for num in range(self.nibbleArrayLength)])
        valStr = nibbleSeq2Str(valSeq)
        if self.timeStampType == "pyTimeElapsed":
            return "{} {}".format(self.time, valStr)
        elif self.timeStampType == "microsHex":
            return "{} {}".format("{:x}".format(self.time), valStr)
        else:
            return "{}".format(valStr)
            
    def fromStr(self, argStr):
        if self.timeStampType is None:
            valStr = argStr
        else:
            timeStr, valStr = argStr.split(" ")
            if self.timeStampType == "pyTimeElapsed":
                self.time = datetime.datetime.strptime(timeStr, "%H:%M:%S.%f", ) - timeZero
            elif self.timeStampType == "microsHex":
                self.time = int(timeStr, 16)
        nibbleSeq = self.hexStr2NibbleSeq(valStr)
        assert len(nibbleSeq) < ARRAY_SIZE
        self.numNibbles = len(nibbleSeq)
        for idx, nibble in enumerate(nibbleSeq):
            self.nibbles[idx] = ctypes.c_byte(nibble)

    def hexStr2NibbleSeq(self, argStr):
        nibbleSeq = []
        for char in argStr:
            if char in string.hexdigits:
                nibbleSeq.append(int(char, 16))
            elif char == 'L':
                nibbleSeq.append(MBUS_LOW_TOO_LONG_CODE)
            elif char == 'T':
                nibbleSeq.append(MBUS_TIMEOUT_CODE)
            else:
                nibbleSeq.append(0xFF)
        return nibbleSeq

    def toToggles(self, startTime, ):
        currentTimeUs = startTime
        toggleList = ToggleList()
        for idx in range(self.numNibbles):
            nibble = int(self.nibbles[idx])
            for bit in "{:04b}".format(nibble):
                # print("{:10}: drop nibble{} bit{}".format(currentTimeUs, nibble, bit))
                toggleList.append((currentTimeUs, '0'))
                if bit == '0':
                    currentTimeUs += 600
                else:
                    currentTimeUs += 1800
                # print("{:10}: rise".format(currentTimeUs))
                toggleList.append((currentTimeUs, '1'))
                if bit == '0':
                    currentTimeUs += (3000 - 600)
                else:
                    currentTimeUs += (3000 - 1800)
        currentTimeUs += (3000 - 600)
        return toggleList
        

def fromStr_MbusRawNibbleListStruct(argStr, timeStampType="pyTimeElapsed"):
    obj = ToggleElement(timeStampType)
    obj.fromStr(argStr)
    return obj


class MbusRawNibbleListStructList(list):

    def __init__(self, timeStampType="pyTimeElapsed"):
        assert timeStampType in ("pyTimeElapsed", "microsHex", None)
        self.timeStampType = timeStampType
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
                self.append(fromStr_MbusRawNibbleListStruct(line, timeStampType=self.timeStampType))


    def toFileName(self, fileName):
        with open(fileName, 'w') as fileHandle:
            self.toFileHandle(fileHandle)

    def toFileHandle(self, fileHandle):
        for togElem in self:
            fileHandle.write("{}\n".format(togElem))

    def __str__(self):
        resultList = []
        for elem in self:
            resultList.append(str(elem))
        return "\n".join(resultList)


def simSend(timeStr, nibbleSeq):
    endTime = datetime.datetime.strptime(timeStr, "%H:%M:%S.%f", ) - timeZero
    currentTimeUs = round((endTime.total_seconds() - (4 * 3.e-3 * len(nibbleSeq)) - 3.5e-3)*1e6)
    # print("-" * 10 + "{} {} {} len{}".format(endTime, timeStr, nibbleSeq, len(nibbleSeq)))
    toggleList = []
    for nibble in nibbleSeq:
        # print("*"*5 + " {}".format(nibble))
        for bit in "{:04b}".format(nibble):
            # print("{:10}: drop nibble{} bit{}".format(currentTimeUs, nibble, bit))
            toggleList.append((currentTimeUs, '0'))
            if bit == '0':
                currentTimeUs += 600
            else:
                currentTimeUs += 1800
            # print("{:10}: rise".format(currentTimeUs))
            toggleList.append((currentTimeUs, '1'))
            if bit == '0':
                currentTimeUs += (3000 - 600)
            else:
                currentTimeUs += (3000 - 1800)
    currentTimeUs += (3000 - 600)
    return toggleList

if __name__ == '__main__':
    print("library only!")
    sys.exit(1)

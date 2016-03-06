#!/bin/env python
from __future__ import print_function
import sys
import os
import ctypes
from . import nibbles

BIT_TIME = 3100
BIT_ZERO_LOW_TIME = 675
BIT_ONE_LOW_TIME = 1890
BIT_ONEVAL_THRESH_TIME = (BIT_ZERO_LOW_TIME + BIT_ONE_LOW_TIME) >> 1
BIT_LOW_TOO_LONG_TIME = (BIT_TIME + BIT_ONE_LOW_TIME) >> 1
NIBBLE_END_GAP_TIME = (BIT_TIME)
INTERBIT_TIMEOUT_TIME = (BIT_TIME)

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


class MbusLinkStruct(ctypes.Structure):
    _fields_ = [
        ("rxMsgFifo", circular_buffer),
        ("txMsgFifo", circular_buffer),
        ("nibbles", nibbles.MbusRawNibbleListStruct),
        ]

    def __str__(self):
        return "tx {} / rx {}".format(self.tx, self.rx)


# ERROR IDS
ERR_ID_SIGNAL = 0x1
ERR_ID_LENGTH = 0x2
ERR_ID_CHECKSUM = 0x3
ERR_ID_DIRECTION = 0x4
ERR_ID_UNKNOWN_TYPE = 0x5

# MSG IDS
MSGTYPE_ping = 0x10
MSGTYPE_setPlayState = 0x11
MSGTYPE_headPowerOn = 0x12
MSGTYPE_setDiskTrack = 0x13
MSGTYPE_setMode = 0x14
MSGTYPE_pong = 0x20
MSGTYPE_ackWait = 0x21
MSGTYPE_cdPowerOn = 0x22
MSGTYPE_changing = 0x23
MSGTYPE_playState = 0x24
MSGTYPE_diskInfo = 0x25
MSGTYPE_unknownStatus = 0x26

# HEAD2CD MSG BODY
class MbusMsg_setPlayState_BodyStruct(ctypes.Structure):
    _fields_ = [
        ("resume", ctypes.c_bool),
        ("stop", ctypes.c_bool),
        ("scanStop", ctypes.c_bool),
        ("fastReverse", ctypes.c_bool),
        ("fastForward", ctypes.c_bool),
        ("pause", ctypes.c_bool),
        ("play", ctypes.c_bool),
        ]

    def __str__(self):
        argTuple = (self.resume, self.stop, self.scanStop,
                    self.fastReverse, self.fastForward, self.pause, self.play)
        fmtStr = "resume{:d} stop{:d} scanStop{:d} fastReverse{:d} fastForward{:d} pause{:d} play{:d}"
        return fmtStr.format(*argTuple)


class MbusMsg_setDiskTrack_BodyStruct(ctypes.Structure):
    _fields_ = [
        ("disk", ctypes.c_uint8),
        ("track", ctypes.c_uint8),
        ("pause", ctypes.c_bool),
        ("play", ctypes.c_bool),
        ("random", ctypes.c_bool),
        ]

    def __str__(self):
        argTuple = self.disk, self.track, self.pause, self.play, self.random
        fmtStr = "disk#{} track:{:02d} pause{:d} play{:d} random{:d}"
        return fmtStr.format(*argTuple)


class MbusMsg_setMode_BodyStruct(ctypes.Structure):
    _fields_ = [
        ("", ctypes.c_bool),
        ("repeatAll", ctypes.c_bool),
        ("repeatOne", ctypes.c_bool),
        ("introScan", ctypes.c_bool),
        ("random", ctypes.c_bool),
        ]

    def __str__(self):
        argTuple = (self.repeatAll, self.repeatOne, self.introScan, self.random, )
        fmtStr = "repeatAll{:d} repeatOne{:d} introScan{:d} random{:d}"
        return fmtStr.format(*argTuple)


# CD2HEAD MSG BODY
class MbusMsg_changing_BodyStruct(ctypes.Structure):
    _fields_ = [
        ("disk", ctypes.c_uint8),
        ("track", ctypes.c_uint8),
        ("eject", ctypes.c_bool),
        ("busy", ctypes.c_bool),
        ("repeatAll", ctypes.c_bool),
        ("repeatOne", ctypes.c_bool),
        ("random", ctypes.c_bool),
        ("done", ctypes.c_bool),
        ]

    def __str__(self):
        argTuple = (self.disk, self.track, self.eject, self.busy, self.repeatAll,
                    self.repeatOne, self.random, self.done, )
        fmtStr = "disk#{} Trk{:02d} eject{:d} busy{:d} repeatAll{:d} repeatOne{:d} random{:d} done{:d}"
        return fmtStr.format(*argTuple)


class MbusMsg_playState_BodyStruct(ctypes.Structure):
    _fields_ = [
        ("track", ctypes.c_uint8),
        ("index", ctypes.c_uint8),
        ("minute", ctypes.c_uint8),
        ("second", ctypes.c_uint8),
        ("repeatAll", ctypes.c_bool),
        ("repeatOne", ctypes.c_bool),
        ("introScan", ctypes.c_bool),
        ("random", ctypes.c_bool),
        ("stopped", ctypes.c_bool),
        ("paused", ctypes.c_bool),
        ("play", ctypes.c_bool),
        ]

    def __str__(self):
        argTuple = (self.track, self.index, self.minute, self.second, self.repeatAll,
                    self.repeatOne, self.introScan, self.random, self.stopped, self.paused, self.play, )
        fmtStr = ("Trk:Idx {:02}:{:02} min:sec {:02}:{:02} repeatAll{:d} " +
                  "repeatOne{:d} introScan{:d} random{:d} stop{:d} pause{:d} play{:d}")
        return fmtStr.format(*argTuple)


class MbusMsg_diskInfo_BodyStruct(ctypes.Structure):
    _fields_ = [
        ("disk", ctypes.c_uint8),
        ("tracks", ctypes.c_uint8),
        ("minutes", ctypes.c_uint8),
        ("seconds", ctypes.c_uint8),
        ("flags", ctypes.c_uint8),
        ]

    def __str__(self):
        argTuple = (self.disk, self.tracks, self.minutes, self.seconds, self.flags)
        fmtStr = "disk #{} tracks {:02d} min:sec {:02d}:{:02d} flags {:x}"
        return fmtStr.format(*argTuple)


#
class MbusMsgBodyUnion(ctypes.Union):
    _fields_ = [
        ("setPlayState", MbusMsg_setPlayState_BodyStruct),
        ("setDiskTrack", MbusMsg_setDiskTrack_BodyStruct),
        ("setMode", MbusMsg_setMode_BodyStruct),
        ("changing", MbusMsg_changing_BodyStruct),
        ("playState", MbusMsg_playState_BodyStruct),
        ("diskInfo", MbusMsg_diskInfo_BodyStruct),
        ]


class MbusMsgParsedStruct(ctypes.Structure):
    _fields_ = [
        ("directionH2C", ctypes.c_bool),
        ("msgType", ctypes.c_uint8),   # contains msg type or err type
        ("body", MbusMsgBodyUnion),
        ]

    def __str__(self):
        if self.directionH2C:
            dirStr = "H2C"
        else:
            dirStr = "C2H"
        bodyStr = ""
        if self.msgType == MSGTYPE_ping:
            # FUTURE: bodyStr = str(self.body.ping)
            msgTypeStr = "ping"
        elif self.msgType == MSGTYPE_setPlayState:
            bodyStr = str(self.body.setPlayState)
            msgTypeStr = "setPlayState"
        elif self.msgType == MSGTYPE_setDiskTrack:
            bodyStr = str(self.body.setDiskTrack)
            msgTypeStr = "setDiskTrack"
        elif self.msgType == MSGTYPE_setMode:
            bodyStr = str(self.body.setMode)
            msgTypeStr = "setMode"
        elif self.msgType == MSGTYPE_headPowerOn:
            # FUTURE: bodyStr = str(self.body.headPowerOn)
            msgTypeStr = "headPowerOn"
        elif self.msgType == MSGTYPE_pong:
            bodyStr = str(self.body.pong)
            msgTypeStr = "pong"
        elif self.msgType == MSGTYPE_ackWait:
            # FUTURE: bodyStr = str(self.body.ackWait)
            msgTypeStr = "ackWait"
        elif self.msgType == MSGTYPE_cdPowerOn:
            # FUTURE: bodyStr = str(self.body.cdPowerOn)
            msgTypeStr = "cdPowerOn"
        elif self.msgType == MSGTYPE_changing:
            bodyStr = str(self.body.changing)
            msgTypeStr = "changing"
        elif self.msgType == MSGTYPE_playState:
            bodyStr = str(self.body.playState)
            msgTypeStr = "playState"
        elif self.msgType == MSGTYPE_diskInfo:
            bodyStr = str(self.body.diskInfo)
            msgTypeStr = "diskInfo"
        elif self.msgType == MSGTYPE_unknownStatus:
            # FUTURE: bodyStr = str(self.body.unknownStatus)
            msgTypeStr = "unknownStatus"
        else:
            msgTypeStr = "STRIFY ERR"
        return "{} {:15} {}".format(dirStr, msgTypeStr, bodyStr)


class MbusMsgStruct(ctypes.Structure):
    _fields_ = [
        ("errId", ctypes.c_uint8),  # 0 = no error
        ("parsed", MbusMsgParsedStruct),   # if not an error, parsed contents here
        ("rawNibbles", nibbles.MbusRawNibbleListStruct),
        ]

    def __str__(self):
        if self.errId == ERR_ID_SIGNAL:
            content = "ERROR: Bad bus signaling"
        if self.errId == ERR_ID_LENGTH:
            content = "ERROR: Bad message length"
        elif self.errId == ERR_ID_CHECKSUM:
            content = "ERROR: CheckSum"
        elif self.errId == ERR_ID_DIRECTION:
            content = "ERROR: Unexpected Direction"
        elif self.errId == ERR_ID_UNKNOWN_TYPE:
            content = "ERROR: Unknown message type"
        else:
            content = str(self.parsed)
        return "{:18} {}".format(str(self.rawNibbles), content)

if __name__ == '__main__':
    print("Library only.")

#!/bin/env python
"""
"""
from __future__ import print_function
import sys
import os
import logging
import argparse
import ctypes
from . import *
from . import mbusTime
from . import nibbles
from . import toggles

libSoName = "build/libMbus.so"

def main(argv):
    cfg = parseArgs(argv)
    log = logging.getLogger()
    log.setLevel(logging.DEBUG)
    with (open(cfg.DESTINATION, "w") if cfg.DESTINATION else sys.stdout) as outFileHandle:
        # logHandler = logging.StreamHandler(outFileHandle)
        logHandler = logging.StreamHandler(sys.stdout)
        # logHandler.setFormatter(ElapsedFormatter())
        log.addHandler(logHandler)
        #
        if cfg.SOURCE is None:
            log.info("Reading from stdin")
        else:
            log.info("Reading from {}".format(cfg.SOURCE))
        nibbleListList = nibbles.MbusRawNibbleListList()
        nibbleListList.fromFileName(cfg.SOURCE)
        msgList = sendToLinkRx(nibbleListList)
        if cfg.DESTINATION is None:
            log.info("Writing to stdout")
        else:
            log.info("Writing to {}".format(cfg.DESTINATION))
        with (open(cfg.DESTINATION, "w") if cfg.DESTINATION is not None else sys.stdout) as fileHandle:
            for msg in msgList:
                fileHandle.write("{}\n".format(msg))


def sendToLinkRx(nibbleListList):
    libMbus = ctypes.cdll.LoadLibrary(libSoName)
    mbusLink = MbusLinkStruct()
    msgMemSize = 2
    rxMsgMem = (MbusRxMsgStruct * msgMemSize)()
    txMsgMem = (MbusTxMsgStruct * msgMemSize)()
    libMbus.mbus_link_init(ctypes.byref(mbusLink),
                           ctypes.byref(rxMsgMem),
                           ctypes.sizeof(rxMsgMem),
                           ctypes.byref(txMsgMem),
                           ctypes.sizeof(txMsgMem),
                           None  # phy tx byte fifo unused
                           )
    resultStrList = []
    for nibbleList in nibbleListList:
        nibbleSeq = tuple([nibbleList.nibbles[num] for num in range(nibbleList.numNibbles)])
        for nibble in nibbleSeq:
            libMbus.mbus_link_rx_push_nibble(ctypes.byref(mbusLink),
                                             ctypes.c_uint8(nibble))
        libMbus.mbus_link_rx_push_nibble(ctypes.byref(mbusLink),
                                         ctypes.c_uint8(nibbles.MBUS_END_MSG_CODE))
        while not libMbus.mbus_link_rx_is_empty(ctypes.byref(mbusLink)):
            mbusMsg = MbusRxMsgStruct()
            libMbus.mbus_link_rx_pop(ctypes.byref(mbusLink),
                                     ctypes.byref(mbusMsg))
            maxStrChar = 256
            strArg = (ctypes.c_char * maxStrChar)(*(b'X' * 256))
            strLen = libMbus.mbus_link_msgToStr(ctypes.byref(mbusMsg),
                                                ctypes.byref(strArg),
                                                ctypes.c_uint(len(strArg)),
                                                )
            resultStr = ""
            for strIdx in range(strLen - 1):  # don't print ending null
                resultStr += strArg[strIdx].decode("utf-8")
            resultStrList.append(resultStr)
    return resultStrList
    # # rxNibList = nibbles.MbusRawNibbleListStructList()
    # rxNibListList = nibbles.MbusRawNibbleListStructList(mbusTimeFormat="blank")
    # rxNibList = nibbles.MbusRawNibbleListStruct(mbusTimeFormat="blank")
    # driveMbusPinLo = ctypes.c_bool()
    # for lowLevelToggle in lowLevelToggleSeq:
    #     libMbus.mbus_link_update(ctypes.byref(mbusLink),
    #                             ctypes.c_ulong(lowLevelToggle[1]),
    #                             ctypes.c_bool(lowLevelToggle[0]),
    #                             ctypes.byref(driveMbusPinLo),
    #                             )
    #     if not libMbus.mbus_link_rx_is_empty(ctypes.byref(mbusLink)):
    #         while not libMbus.mbus_link_rx_is_empty(ctypes.byref(mbusLink)):
    #             newNib = libMbus.mbus_link_rx_pop(ctypes.byref(mbusLink))
    #             if newNib == MBUS_END_MSG_CODE:
    #                 # print("{}".format("".join(rxNibList)))
    #                 # print("{}".format(rxNibList))
    #                 rxNibListList.append(rxNibList)
    #                 rxNibList = nibbles.MbusRawNibbleListStruct(mbusTimeFormat="blank")
    #             else:
    #                 rxNibList.append(chr((libMbus.mbus_link_rxnibble2ascii(newNib))))
    # return rxNibListList

def parseArgs(argv):
    description = __doc__
    parser = argparse.ArgumentParser(description=description, formatter_class=argparse.RawDescriptionHelpFormatter)
    if len(argv) == 1:
        argv.append('-h')

    parser.add_argument('SOURCE',
                        help='File name of hex msgs to parse.',)
    parser.add_argument('DESTINATION', nargs='?',
                        help='Optional destination filename.  If omitted, prints to stdout.',)
    parser.add_argument('-f', '--overwrite-file',
                        help='Normally, if the destination file already exists, the program will terminate.  This option forces the overwrite of DESTINATION even if it exists.',
                        action='store_true',)
    parser.add_argument('-d', '--debug',
                        help='Turn on debug printing.',
                        action='store_true',)
    cfg = parser.parse_args()
    return cfg


if __name__ == '__main__':
    main(sys.argv)

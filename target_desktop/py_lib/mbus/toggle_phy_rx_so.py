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

libSoName="build/libMbus.so"

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
        toggleList = toggles.ToggleList()
        toggleList.fromFileName(cfg.SOURCE)
        lowLevelToggles = toggles.intervalizedToggles(toggleList, interval=200)
        nibbleListList = toggleSim(lowLevelToggles)
        if cfg.DESTINATION is None:
            log.info("Writing to stdout")
        else:
            log.info("Writing to {}".format(cfg.DESTINATION))
        nibbleListList.toFileName(cfg.DESTINATION)

def toggleSim(lowLevelToggleSeq):
    libMbus = ctypes.cdll.LoadLibrary(libSoName)
    mbusPhy = MbusPhyStruct()
    byteMemSize = 64
    rxByteMem = (ctypes.c_byte * byteMemSize)()
    txByteMem = (ctypes.c_byte * byteMemSize)()
    libMbus.mbus_phy_init(ctypes.byref(mbusPhy),
                          ctypes.byref(rxByteMem),
                          ctypes.c_size_t(byteMemSize),
                          ctypes.byref(txByteMem),
                          ctypes.c_size_t(byteMemSize),
                          )
    libMbus.mbus_phy_rx_enable(ctypes.byref(mbusPhy))
    # rxNibList = nibbles.MbusRawNibbleListStructList()
    rxNibListList = nibbles.MbusRawNibbleListStructList(mbusTimeFormat="blank")
    rxNibList = nibbles.MbusRawNibbleListStruct(mbusTimeFormat="blank")
    driveMbusPinLo = ctypes.c_bool()
    for lowLevelToggle in lowLevelToggleSeq:
        libMbus.mbus_phy_update(ctypes.byref(mbusPhy),
                                ctypes.c_ulong(lowLevelToggle[1]),
                                ctypes.c_bool(lowLevelToggle[0]),
                                ctypes.byref(driveMbusPinLo),
                                )
        if not libMbus.mbus_phy_rx_is_empty(ctypes.byref(mbusPhy)):
            while not libMbus.mbus_phy_rx_is_empty(ctypes.byref(mbusPhy)):
                newNib = libMbus.mbus_phy_rx_pop(ctypes.byref(mbusPhy))
                if newNib == nibbles.MBUS_END_MSG_CODE:
                    # print("{}".format("".join(rxNibList)))
                    # print("{}".format(rxNibList))
                    rxNibListList.append(rxNibList)
                    rxNibList = nibbles.MbusRawNibbleListStruct(mbusTimeFormat="blank")
                else:
                    rxNibList.append(chr((libMbus.mbus_phy_rxnibble2ascii(newNib))))
    return rxNibListList

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

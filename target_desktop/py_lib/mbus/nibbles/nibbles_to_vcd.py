#!/bin/env python
"""
"""
from __future__ import print_function
import sys
import os
import logging
import argparse
from . import *
from .. import mbusTime


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
        nibbleList = MbusRawNibbleListList()
        nibbleList.fromFileName(cfg.SOURCE)
        if cfg.DESTINATION is None:
            log.info("Writing to stdout")
        else:
            log.info("Writing to {}".format(cfg.DESTINATION))
        nibbleList.toVcdFileName(cfg.DESTINATION)

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

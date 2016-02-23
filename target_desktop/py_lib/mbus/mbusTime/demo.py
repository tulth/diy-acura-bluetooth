#!/bin/env python
"""
"""
from __future__ import print_function
import sys
import os
import logging
import argparse
from . import *

def main(argv):
    mbt0 = MbusTime(12, mbusTimeFormat="pyTimeElapsed")
    print("mtb0 >>{}<<".format(mbt0))
    mbt1 = MbusTime(12, mbusTimeFormat="microsHex")
    print("mtb1 >>{}<<".format(mbt1))
    mbt2 = MbusTime(12, mbusTimeFormat="blank")
    print("mtb2 >>{}<<".format(mbt2))
    print("type(mtb0) >>{}<<".format(type(mbt0)))
    print("type(mtb1) >>{}<<".format(type(mbt1)))
    print("mtb0 + mtb1 >>{}<<".format(mbt0 + mbt1))
    print("type(mtb0 + mtb1) >>{}<<".format(type(mbt0 + mbt1)))
    print("mtb1 + mtb0 >>{}<<".format(mbt1 + mbt0))
    print("type(mtb1 + mtb0) >>{}<<".format(type(mbt1 + mbt0)))
    #
    mbt0.fromStr('0:00:05.871401')
    print("mbt0.fromStr('0:00:05.871401'), mbt0 >>{}<<".format(mbt0))
    #
    mbt1.fromStr('FFFFFFFF')
    print("mbt1.fromStr('FFFFFFFF'), mbt1 >>{}<<".format(mbt1))
    #
    print("mtb0 + mtb1 >>{}<<".format(mbt0 + mbt1))
    print("mtb1 + mtb0 >>{}<<".format(mbt1 + mbt0))
    #
    rem = mbt0.fromStr('0:00:05.871401 stuff')
    print("mbt0.fromStr('0:00:05.871401'), mbt0 >>{}<< rem >>{}<<".format(mbt0, rem))
    #
    rem = mbt0.fromStr('0:00:05.871401 other stuff')
    print("mbt0.fromStr('0:00:05.871401'), mbt0 >>{}<< rem >>{}<<".format(mbt0, rem))


if __name__ == '__main__':
    main(sys.argv)

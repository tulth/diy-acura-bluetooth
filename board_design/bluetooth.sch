EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:diy
LIBS:diy-acura-bluetooth-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 6 6
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L RN-52-BREAKOUT U5
U 1 1 56F1857A
P 5200 2850
F 0 "U5" H 5200 3900 60  0000 C CNN
F 1 "RN-52-BREAKOUT" H 5200 1800 60  0000 C CNN
F 2 "diy-acura-bluetooth:RN-52-BREAKOUT" H 5200 2850 60  0001 C CNN
F 3 "" H 5200 2850 60  0000 C CNN
F 4 "Sparkfun" H 5200 2850 60  0001 C CNN "MFN"
F 5 "WRL-12849" H 5200 2850 60  0001 C CNN "MFP"
	1    5200 2850
	1    0    0    -1  
$EndComp
Text HLabel 4100 3800 0    60   Output ~ 0
3vIn
Wire Wire Line
	4100 3800 4400 3800
$Comp
L GND #PWR023
U 1 1 56F185E1
P 6100 3900
F 0 "#PWR023" H 6100 3650 50  0001 C CNN
F 1 "GND" H 6100 3750 50  0000 C CNN
F 2 "" H 6100 3900 50  0000 C CNN
F 3 "" H 6100 3900 50  0000 C CNN
	1    6100 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 3900 6100 3800
Wire Wire Line
	6100 3800 6000 3800
NoConn ~ 4400 1900
NoConn ~ 4400 2200
NoConn ~ 4400 3500
NoConn ~ 6000 3300
NoConn ~ 6000 3200
NoConn ~ 6000 3100
NoConn ~ 6000 3000
NoConn ~ 6000 2900
NoConn ~ 6000 2800
NoConn ~ 6000 2700
NoConn ~ 6000 2600
NoConn ~ 6000 2500
NoConn ~ 6000 2400
NoConn ~ 6000 2300
Text HLabel 4100 2000 0    60   Output ~ 0
connStatEventOut
Wire Wire Line
	4400 2000 4100 2000
Text HLabel 4100 2800 0    60   Input ~ 0
cmdLoIn
Wire Wire Line
	4100 2800 4400 2800
Text HLabel 4100 2900 0    60   BiDi ~ 0
usbProgD-
Text HLabel 4100 3000 0    60   Input ~ 0
usbProgD+
Wire Wire Line
	4100 2900 4400 2900
Wire Wire Line
	4100 3000 4400 3000
Text HLabel 4100 3700 0    60   Input ~ 0
powerEnIn
Wire Wire Line
	4100 3700 4400 3700
Text HLabel 4100 3400 0    60   Input ~ 0
uartRxIn
Text HLabel 4100 3300 0    60   Output ~ 0
uartTxOut
Wire Wire Line
	4100 3300 4400 3300
Wire Wire Line
	4100 3400 4400 3400
Text HLabel 4100 3100 0    60   Output ~ 0
uartRtsOut
Text HLabel 4100 3200 0    60   Input ~ 0
uartCtsIn
Wire Wire Line
	4100 3100 4400 3100
Wire Wire Line
	4400 3200 4100 3200
Text HLabel 6200 1900 2    60   Output ~ 0
SPK_L+
Text HLabel 6200 2000 2    60   Output ~ 0
SPK_R+
Wire Wire Line
	6200 2000 6000 2000
Wire Wire Line
	6000 1900 6200 1900
Text HLabel 6200 2100 2    60   Output ~ 0
SPK_L-
Wire Wire Line
	6000 2100 6200 2100
Text HLabel 6200 2200 2    60   Output ~ 0
SPK_R-
Wire Wire Line
	6200 2200 6000 2200
$EndSCHEMATC

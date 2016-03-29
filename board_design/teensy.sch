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
Sheet 5 6
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
L TEENSY-3.2 U4
U 1 1 56F17BD5
P 4150 2450
F 0 "U4" H 4150 3200 60  0000 C CNN
F 1 "TEENSY-3.2" H 4150 3000 60  0000 C CNN
F 2 "diy-acura-bluetooth:Teensy-3.1" H 4150 3000 60  0001 C CNN
F 3 "" H 4150 3000 60  0000 C CNN
F 4 "Sparkfun" H 4150 2450 60  0001 C CNN "MFN"
F 5 "DEV-13736" H 4150 2450 60  0001 C CNN "MFP"
	1    4150 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 1900 3300 1900
Text HLabel 2400 2000 0    60   Output ~ 0
rn52TxOut
Text HLabel 2400 1900 0    60   Input ~ 0
rn52RxIn
Wire Wire Line
	2400 2000 3300 2000
$Comp
L GND #PWR020
U 1 1 56F17C9D
P 4150 3700
F 0 "#PWR020" H 4150 3450 50  0001 C CNN
F 1 "GND" H 4150 3550 50  0000 C CNN
F 2 "" H 4150 3700 50  0000 C CNN
F 3 "" H 4150 3700 50  0000 C CNN
	1    4150 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 3700 4150 3600
$Comp
L GND #PWR021
U 1 1 56F17CB6
P 3100 1650
F 0 "#PWR021" H 3100 1400 50  0001 C CNN
F 1 "GND" H 3100 1500 50  0000 C CNN
F 2 "" H 3100 1650 50  0000 C CNN
F 3 "" H 3100 1650 50  0000 C CNN
	1    3100 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 1650 3100 1550
Wire Wire Line
	3300 1800 3200 1800
Wire Wire Line
	3200 1800 3200 1550
Wire Wire Line
	3200 1550 3100 1550
Text HLabel 5500 1800 2    60   Input ~ 0
5vIn
Wire Wire Line
	5500 1800 5000 1800
Text HLabel 5500 2000 2    60   Output ~ 0
3vOut
Wire Wire Line
	5500 2000 5000 2000
$Comp
L GND #PWR022
U 1 1 56F17CF5
P 5350 3550
F 0 "#PWR022" H 5350 3300 50  0001 C CNN
F 1 "GND" H 5350 3400 50  0000 C CNN
F 2 "" H 5350 3550 50  0000 C CNN
F 3 "" H 5350 3550 50  0000 C CNN
	1    5350 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 1900 5350 3550
Wire Wire Line
	5350 1900 5000 1900
Text HLabel 5500 2100 2    60   Input ~ 0
mbusSenseIn
Text HLabel 5500 2200 2    60   Output ~ 0
mbusDriveLoOut
Wire Wire Line
	5500 2200 5000 2200
Wire Wire Line
	5000 2100 5500 2100
NoConn ~ 3950 3600
NoConn ~ 4250 3600
NoConn ~ 4350 3600
Text HLabel 4050 3800 3    60   Output ~ 0
3vOut
Wire Wire Line
	4050 3600 4050 3800
NoConn ~ 5000 2500
NoConn ~ 5000 2600
NoConn ~ 5000 2700
NoConn ~ 5000 2800
NoConn ~ 5000 2900
NoConn ~ 5000 3000
NoConn ~ 5000 3100
NoConn ~ 3300 3100
NoConn ~ 3300 3000
NoConn ~ 3300 2900
NoConn ~ 3300 2800
NoConn ~ 3300 2700
NoConn ~ 3300 2600
Text HLabel 2400 2300 0    60   Output ~ 0
rn52CmdLoOut
Text HLabel 2400 2400 0    60   Input ~ 0
rn52ConnStatEventIn
Text HLabel 2400 2200 0    60   Input ~ 0
rn52RtsIn
Text HLabel 2400 2100 0    60   Input ~ 0
rn52CtsOut
Wire Wire Line
	2400 2200 3300 2200
Wire Wire Line
	2400 2100 3300 2100
Wire Wire Line
	2400 2300 3300 2300
Wire Wire Line
	2400 2400 3300 2400
Text HLabel 2400 2500 0    60   Output ~ 0
rn52PwrEnOut
Wire Wire Line
	2400 2500 3300 2500
Text HLabel 5500 2300 2    60   Output ~ 0
holdPowerOnOut
Wire Wire Line
	5500 2300 5000 2300
Text HLabel 5500 2400 2    60   Input ~ 0
accSenseIn
Wire Wire Line
	5500 2400 5000 2400
$EndSCHEMATC

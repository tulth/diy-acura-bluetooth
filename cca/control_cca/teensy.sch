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
LIBS:t_cap
LIBS:t_comm
LIBS:t_conn
LIBS:t_diode
LIBS:t_microcontroller
LIBS:t_module
LIBS:t_opamp
LIBS:t_power
LIBS:t_relay
LIBS:t_res
LIBS:t_sensor
LIBS:t_transistor
LIBS:mechanical
LIBS:control_cca-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 6 4
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
L GND #PWR021
U 1 1 588327DC
P 3100 1600
F 0 "#PWR021" H 3100 1350 50  0001 C CNN
F 1 "GND" H 3100 1450 50  0000 C CNN
F 2 "" H 3100 1600 50  0000 C CNN
F 3 "" H 3100 1600 50  0000 C CNN
	1    3100 1600
	1    0    0    -1  
$EndComp
Text HLabel 5500 1800 2    60   Input ~ 0
5vIn
Text HLabel 5500 2000 2    60   Output ~ 0
3vOut
Text HLabel 5500 2100 2    60   Input ~ 0
mbusSenseLowIn
Text HLabel 5500 2200 2    60   Output ~ 0
mbusDriveLowOut
Text HLabel 2400 2300 0    60   Output ~ 0
rn52CmdLoOut
Text HLabel 2400 2400 0    60   Input ~ 0
rn52ConnStatEventIn
Text HLabel 2400 2200 0    60   Input ~ 0
rn52RtsIn
Text HLabel 2400 2100 0    60   Output ~ 0
rn52CtsOut
Text HLabel 2400 2500 0    60   Output ~ 0
rn52PwrEnOut
Text HLabel 5500 2300 2    60   Output ~ 0
holdPowerOnOut
Text HLabel 5500 2400 2    60   Input ~ 0
accLowIn
$Comp
L TEENSY-LC U3
U 1 1 588334ED
P 4100 2500
F 0 "U3" H 4100 3300 60  0000 C CNN
F 1 "TEENSY-LC" H 4100 3100 60  0000 C CNN
F 2 "t_footprints:Teensy-3.2" H 4100 3100 60  0001 C CNN
F 3 "" H 4100 3100 60  0000 C CNN
	1    4100 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 2300 4900 2300
Wire Wire Line
	5500 2200 4900 2200
Wire Wire Line
	5500 2100 4900 2100
Wire Wire Line
	5500 2000 4900 2000
Wire Wire Line
	5500 1800 4900 1800
Wire Wire Line
	3100 1500 3100 1600
Wire Wire Line
	3200 1500 3100 1500
Wire Wire Line
	3200 1800 3200 1500
Wire Wire Line
	3300 1800 3200 1800
Wire Wire Line
	5500 2400 4900 2400
Wire Wire Line
	3300 2100 2400 2100
Wire Wire Line
	3300 2200 2400 2200
Wire Wire Line
	3300 2300 2400 2300
Wire Wire Line
	3300 2400 2400 2400
Wire Wire Line
	3300 2500 2400 2500
NoConn ~ 3300 2600
NoConn ~ 3300 2700
NoConn ~ 3300 2800
NoConn ~ 3300 2900
NoConn ~ 3300 3000
NoConn ~ 3300 3100
NoConn ~ 3900 3500
NoConn ~ 4100 3500
NoConn ~ 4000 3500
NoConn ~ 4200 3500
NoConn ~ 4300 3500
NoConn ~ 4900 3100
NoConn ~ 4900 3000
NoConn ~ 4900 2800
NoConn ~ 4900 2900
NoConn ~ 4900 2700
NoConn ~ 4900 2600
NoConn ~ 4900 2500
$Comp
L GND #PWR022
U 1 1 5882E9A0
P 5200 3300
F 0 "#PWR022" H 5200 3050 50  0001 C CNN
F 1 "GND" H 5200 3150 50  0000 C CNN
F 2 "" H 5200 3300 50  0000 C CNN
F 3 "" H 5200 3300 50  0000 C CNN
	1    5200 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 1900 5200 1900
Wire Wire Line
	5200 1900 5200 3300
Wire Wire Line
	3300 2000 2400 2000
Text HLabel 2400 2000 0    60   Output ~ 0
rn52TxOut
Text HLabel 2400 1900 0    60   Input ~ 0
rn52RxIn
Wire Wire Line
	2400 1900 3300 1900
$EndSCHEMATC

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
Sheet 1 6
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
L MBUS_CONN P1
U 1 1 56EF363F
P 900 3200
F 0 "P1" H 900 3700 60  0000 C CNN
F 1 "MBUS_CONN" H 900 2700 60  0000 C CNN
F 2 "diy-acura-bluetooth:mbus_conn" H 1050 2700 60  0001 C CNN
F 3 "" H 1050 2700 60  0000 C CNN
F 4 "Sullins" H 900 3200 60  0001 C CNN "MFN"
F 5 "PRPC040SBAN-M71RC" H 900 3200 60  0001 C CNN "MFP"
	1    900  3200
	1    0    0    -1  
$EndComp
$Sheet
S 2750 1400 650  550 
U 56EF6FF5
F0 "audio" 60
F1 "audio.sch" 60
F2 "R+" I R 3400 1600 60 
F3 "R-" I R 3400 1700 60 
F4 "L+" I R 3400 1800 60 
F5 "L-" I R 3400 1900 60 
F6 "right" O L 2750 1900 60 
F7 "left" O L 2750 1700 60 
F8 "Vcc" I L 2750 1500 60 
$EndSheet
$Sheet
S 2700 3900 1000 750 
U 56F10EEE
F0 "power" 60
F1 "power.sch" 60
F2 "battIn" I L 2700 4100 60 
F3 "accIn" I L 2700 4000 60 
F4 "SW12VOut" O R 3700 4000 60 
F5 "5VOut" O R 3700 4100 60 
F6 "holdPowerOnIn" I R 3700 4300 60 
F7 "accSenseOut" I R 3700 4400 60 
$EndSheet
Wire Wire Line
	1400 3500 2300 3500
Wire Wire Line
	1400 3400 2400 3400
$Comp
L GND #PWR01
U 1 1 56F14837
P 1500 3700
F 0 "#PWR01" H 1500 3450 50  0001 C CNN
F 1 "GND" H 1500 3550 50  0000 C CNN
F 2 "" H 1500 3700 50  0000 C CNN
F 3 "" H 1500 3700 50  0000 C CNN
	1    1500 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 2800 1500 3700
Wire Wire Line
	1500 3600 1400 3600
Wire Wire Line
	1500 3200 1400 3200
Connection ~ 1500 3600
Wire Wire Line
	1500 3000 1400 3000
Connection ~ 1500 3200
Wire Wire Line
	1500 2800 1400 2800
Connection ~ 1500 3000
Wire Wire Line
	2700 4000 2400 4000
Wire Wire Line
	2400 4000 2400 3400
Wire Wire Line
	2700 4100 2300 4100
Wire Wire Line
	2300 4100 2300 3500
Wire Wire Line
	3900 4000 3700 4000
Wire Wire Line
	3900 3400 3900 4000
$Comp
L +5V #PWR02
U 1 1 56F1507F
P 4200 3900
F 0 "#PWR02" H 4200 3750 50  0001 C CNN
F 1 "+5V" H 4200 4040 50  0000 C CNN
F 2 "" H 4200 3900 50  0000 C CNN
F 3 "" H 4200 3900 50  0000 C CNN
	1    4200 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 4100 4200 3900
$Comp
L +5V #PWR03
U 1 1 56F155EC
P 2600 1300
F 0 "#PWR03" H 2600 1150 50  0001 C CNN
F 1 "+5V" H 2600 1440 50  0000 C CNN
F 2 "" H 2600 1300 50  0000 C CNN
F 3 "" H 2600 1300 50  0000 C CNN
	1    2600 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2600 1500 2600 1300
Wire Wire Line
	2600 1500 2750 1500
$Sheet
S 4100 3200 1250 300 
U 56F157BE
F0 "mbus_if" 60
F1 "mbus_if.sch" 60
F2 "mbusBi" B L 4100 3300 60 
F3 "SW12VIn" I L 4100 3400 60 
F4 "mbusSenseOut" O R 5350 3300 60 
F5 "driveMbusLoIn" I R 5350 3400 60 
$EndSheet
Wire Wire Line
	4100 3300 1400 3300
Wire Wire Line
	3900 3400 4100 3400
$Sheet
S 6900 1650 2100 3450
U 56F178FE
F0 "teensy" 60
F1 "teensy.sch" 60
F2 "5vIn" I L 6900 4100 60 
F3 "rn52TxOut" O L 6900 2000 60 
F4 "rn52RxIn" I L 6900 2100 60 
F5 "3vOut" O L 6900 1700 60 
F6 "mbusSenseIn" I L 6900 3300 60 
F7 "mbusDriveLoOut" O L 6900 3400 60 
F8 "rn52CmdLoOut" O L 6900 2450 60 
F9 "rn52ConnStatEventIn" I L 6900 2550 60 
F10 "rn52RtsIn" I L 6900 2200 60 
F11 "rn52CtsOut" I L 6900 2300 60 
F12 "rn52PwrEnOut" O L 6900 1800 60 
F13 "holdPowerOnOut" O L 6900 4300 60 
F14 "accSenseIn" I L 6900 4400 60 
$EndSheet
Wire Wire Line
	2750 1700 2000 1700
Wire Wire Line
	2000 1700 2000 2900
Wire Wire Line
	2000 2900 1400 2900
Wire Wire Line
	2750 1900 2200 1900
Wire Wire Line
	2200 1900 2200 3100
Wire Wire Line
	2200 3100 1400 3100
Wire Wire Line
	4200 1600 3400 1600
Wire Wire Line
	3400 1700 4200 1700
Wire Wire Line
	4200 1800 3400 1800
Wire Wire Line
	4200 1900 3400 1900
Connection ~ 4200 4100
$Sheet
S 4200 1400 1300 1300
U 56F184EF
F0 "bluetooth" 60
F1 "bluetooth.sch" 60
F2 "usbProgD-" B R 5500 1600 60 
F3 "usbProgD+" I R 5500 1500 60 
F4 "SPK_L+" O L 4200 1800 60 
F5 "SPK_R+" O L 4200 1600 60 
F6 "SPK_L-" O L 4200 1900 60 
F7 "SPK_R-" O L 4200 1700 60 
F8 "3vIn" O R 5500 1700 60 
F9 "connStatEventOut" O R 5500 2550 60 
F10 "cmdLoIn" I R 5500 2450 60 
F11 "powerEnIn" I R 5500 1800 60 
F12 "uartRxIn" I R 5500 2000 60 
F13 "uartTxOut" O R 5500 2100 60 
F14 "uartRtsOut" O R 5500 2200 60 
F15 "uartCtsIn" I R 5500 2300 60 
$EndSheet
Wire Wire Line
	5350 3300 6900 3300
Wire Wire Line
	6900 3400 5350 3400
Wire Wire Line
	3700 4100 6900 4100
Wire Wire Line
	6900 4300 3700 4300
Wire Wire Line
	6900 4400 3700 4400
Wire Wire Line
	6900 2550 5500 2550
Wire Wire Line
	5500 2450 6900 2450
Wire Wire Line
	6900 2300 5500 2300
Wire Wire Line
	6900 2200 5500 2200
Wire Wire Line
	6900 2100 5500 2100
Wire Wire Line
	6900 2000 5500 2000
Wire Wire Line
	6900 1800 5500 1800
Wire Wire Line
	5500 1700 6900 1700
$Comp
L CONN_01X03 P2
U 1 1 56F26C55
P 6500 900
F 0 "P2" H 6500 1100 50  0000 C CNN
F 1 "CONN_01X03" V 6600 900 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 6500 900 50  0001 C CNN
F 3 "" H 6500 900 50  0000 C CNN
F 4 "Sullins" H 6500 900 60  0001 C CNN "MFN"
F 5 "PRPC040SAAN-RC" H 6500 900 60  0001 C CNN "MFP"
	1    6500 900 
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 800  5800 800 
Wire Wire Line
	5800 800  5800 1500
Wire Wire Line
	5800 1500 5500 1500
Wire Wire Line
	5500 1600 5900 1600
Wire Wire Line
	5900 1600 5900 900 
Wire Wire Line
	5900 900  6300 900 
$Comp
L GND #PWR04
U 1 1 56F2714E
P 6200 1100
F 0 "#PWR04" H 6200 850 50  0001 C CNN
F 1 "GND" H 6200 950 50  0000 C CNN
F 2 "" H 6200 1100 50  0000 C CNN
F 3 "" H 6200 1100 50  0000 C CNN
	1    6200 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 1000 6200 1000
Wire Wire Line
	6200 1000 6200 1100
Text Label 2200 1700 0    60   ~ 0
SPK_L
Text Label 2300 1900 0    60   ~ 0
SPK_R
Text Label 1800 3400 0    60   ~ 0
ACC
Text Label 1800 3500 0    60   ~ 0
BATT
Text Label 1800 3300 0    60   ~ 0
BUS
Text Label 3650 1600 0    60   ~ 0
SPK_R_P
Text Label 3650 1700 0    60   ~ 0
SPK_R_N
Text Label 3650 1800 0    60   ~ 0
SPK_L_P
Text Label 3650 1900 0    60   ~ 0
SPK_L_N
Text Label 5750 3300 0    60   ~ 0
mbusSense
Text Label 5750 3400 0    60   ~ 0
driveMbusLo
Text Label 4550 4300 0    60   ~ 0
holdPowerOn
Text Label 4550 4400 0    60   ~ 0
accSense
$Comp
L +3.3V #PWR05
U 1 1 57047CF7
P 6350 1600
F 0 "#PWR05" H 6350 1450 50  0001 C CNN
F 1 "+3.3V" H 6350 1740 50  0000 C CNN
F 2 "" H 6350 1600 50  0000 C CNN
F 3 "" H 6350 1600 50  0000 C CNN
	1    6350 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6350 1600 6350 1700
Connection ~ 6350 1700
Text Label 5950 1800 0    60   ~ 0
rn52PwrEn
Text Label 6050 2000 0    60   ~ 0
rn52Tx
Text Label 6050 2100 0    60   ~ 0
rn52Rx
Text Label 6050 2200 0    60   ~ 0
rn52Rts
Text Label 6050 2300 0    60   ~ 0
rn52Cts
Text Label 6050 2450 0    60   ~ 0
rn52CmdLo
Text Label 5800 2550 0    60   ~ 0
rn52ConnStatEvent
$EndSCHEMATC

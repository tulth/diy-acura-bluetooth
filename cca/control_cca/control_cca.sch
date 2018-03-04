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
Sheet 1 4
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 4200 3900 1400 500 
U 58A3B6E5
F0 "mbus_if" 60
F1 "mbus_if.sch" 60
F2 "sw12Vin" I L 4200 4000 60 
F3 "mbusBi" I L 4200 4300 60 
F4 "mbusSenseLowOut" O R 5600 4100 60 
F5 "mbusDriveLowIn" O R 5600 4200 60 
$EndSheet
$Sheet
S 2700 3000 1100 1100
U 58A3B8B4
F0 "power" 60
F1 "power.sch" 60
F2 "battIn" I L 2700 3100 60 
F3 "accIn" I L 2700 3200 60 
F4 "sw12VOut" O R 3800 4000 60 
F5 "5VOut" O R 3800 3200 60 
F6 "holdPowerOnIn" I R 3800 3300 60 
F7 "accLowOut" I R 3800 3400 60 
$EndSheet
$Sheet
S 6600 3100 1100 3300
U 58A3B94A
F0 "teensy" 60
F1 "teensy.sch" 60
F2 "5vIn" I L 6600 3200 60 
F3 "3vOut" O R 7700 3200 60 
F4 "mbusSenseLowIn" I L 6600 4100 60 
F5 "mbusDriveLowOut" O L 6600 4200 60 
F6 "rn52CmdLoOut" O L 6600 6200 60 
F7 "rn52ConnStatEventIn" I L 6600 6300 60 
F8 "rn52RtsIn" I L 6600 6100 60 
F9 "rn52PwrEnOut" O L 6600 5700 60 
F10 "holdPowerOnOut" O L 6600 3300 60 
F11 "accLowIn" I L 6600 3400 60 
F12 "rn52TxOut" O L 6600 5900 60 
F13 "rn52RxIn" I L 6600 5800 60 
F14 "rn52CtsOut" O L 6600 6000 60 
$EndSheet
$Comp
L MBUS_CONN P1
U 1 1 58A3C1A1
P 800 5800
F 0 "P1" H 800 6300 60  0000 C CNN
F 1 "MBUS_CONN" H 800 5300 60  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x09_Pitch2.54mm" H 950 5300 60  0001 C CNN
F 3 "" H 950 5300 60  0000 C CNN
	1    800  5800
	1    0    0    -1  
$EndComp
Connection ~ 11300 7800
$Comp
L +3.3V #PWR01
U 1 1 58A6DEA3
P 5500 5500
F 0 "#PWR01" H 5500 5350 50  0001 C CNN
F 1 "+3.3V" H 5500 5640 50  0000 C CNN
F 2 "" H 5500 5500 50  0000 C CNN
F 3 "" H 5500 5500 50  0000 C CNN
	1    5500 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 5500 5500 5600
Wire Wire Line
	5500 5600 5400 5600
$Comp
L +5V #PWR02
U 1 1 58A70825
P 2700 5300
F 0 "#PWR02" H 2700 5150 50  0001 C CNN
F 1 "+5V" H 2700 5440 50  0000 C CNN
F 2 "" H 2700 5300 50  0000 C CNN
F 3 "" H 2700 5300 50  0000 C CNN
	1    2700 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 5300 2700 5400
Wire Wire Line
	2700 5400 2800 5400
$Comp
L +5V #PWR03
U 1 1 58A70A38
P 6500 3100
F 0 "#PWR03" H 6500 2950 50  0001 C CNN
F 1 "+5V" H 6500 3240 50  0000 C CNN
F 2 "" H 6500 3100 50  0000 C CNN
F 3 "" H 6500 3100 50  0000 C CNN
	1    6500 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6500 3100 6500 3200
Wire Wire Line
	3800 3200 6600 3200
$Comp
L GND #PWR04
U 1 1 58A70F8D
P 2700 5800
F 0 "#PWR04" H 2700 5550 50  0001 C CNN
F 1 "GND" H 2700 5650 50  0000 C CNN
F 2 "" H 2700 5800 50  0000 C CNN
F 3 "" H 2700 5800 50  0000 C CNN
	1    2700 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 5700 2700 5700
Wire Wire Line
	2700 5700 2700 5800
$Comp
L +3.3V #PWR05
U 1 1 58A7192B
P 7800 3100
F 0 "#PWR05" H 7800 2950 50  0001 C CNN
F 1 "+3.3V" H 7800 3240 50  0000 C CNN
F 2 "" H 7800 3100 50  0000 C CNN
F 3 "" H 7800 3100 50  0000 C CNN
	1    7800 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 3100 7800 3200
Wire Wire Line
	7800 3200 7700 3200
Wire Wire Line
	6600 6200 5400 6200
Wire Wire Line
	6600 6300 5400 6300
Wire Wire Line
	6600 6100 5400 6100
Wire Wire Line
	6600 5800 5400 5800
Wire Wire Line
	6600 5900 5400 5900
Wire Wire Line
	6600 5700 5400 5700
Wire Wire Line
	6600 6000 5400 6000
Wire Wire Line
	2600 5700 1300 5700
Wire Wire Line
	1300 5500 2800 5500
$Comp
L GND #PWR06
U 1 1 58A78DFF
P 2000 6300
F 0 "#PWR06" H 2000 6050 50  0001 C CNN
F 1 "GND" H 2000 6150 50  0000 C CNN
F 2 "" H 2000 6300 50  0000 C CNN
F 3 "" H 2000 6300 50  0000 C CNN
	1    2000 6300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2000 5400 2000 6300
Wire Wire Line
	2000 6200 1300 6200
Wire Wire Line
	2000 5800 1300 5800
Connection ~ 2000 6200
Wire Wire Line
	2000 5600 1300 5600
Connection ~ 2000 5800
Wire Wire Line
	2000 5400 1300 5400
Connection ~ 2000 5600
Wire Wire Line
	3800 4000 4200 4000
Wire Wire Line
	1300 5900 2400 5900
Wire Wire Line
	2400 5900 2400 4300
Wire Wire Line
	2400 4300 4200 4300
Wire Wire Line
	1300 6000 2300 6000
Wire Wire Line
	2300 6000 2300 3200
Wire Wire Line
	2300 3200 2700 3200
Connection ~ 6500 3200
Wire Wire Line
	1300 6100 2200 6100
Wire Wire Line
	2200 6100 2200 3100
Wire Wire Line
	2200 3100 2700 3100
Wire Wire Line
	6600 3300 3800 3300
Wire Wire Line
	3800 3400 6600 3400
Wire Wire Line
	5600 4100 6600 4100
Wire Wire Line
	6600 4200 5600 4200
$Comp
L GND #PWR07
U 1 1 58A7EF16
P 5500 6500
F 0 "#PWR07" H 5500 6250 50  0001 C CNN
F 1 "GND" H 5500 6350 50  0000 C CNN
F 2 "" H 5500 6500 50  0000 C CNN
F 3 "" H 5500 6500 50  0000 C CNN
	1    5500 6500
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5400 6400 5500 6400
Wire Wire Line
	5500 6400 5500 6500
Text Label 1400 5500 0    60   ~ 0
SPK_L
Text Label 1400 5700 0    60   ~ 0
SPK_R
Text Label 1400 5900 0    60   ~ 0
BUS
Text Label 1400 6000 0    60   ~ 0
ACC
Text Label 1400 6100 0    60   ~ 0
BATT
$Comp
L Conn_01x09 J7
U 1 1 5A9B694A
P 5200 6000
F 0 "J7" H 5200 6500 50  0000 C CNN
F 1 "Conn_01x09" H 5200 5500 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x09_Pitch2.54mm" H 5200 6000 50  0001 C CNN
F 3 "" H 5200 6000 50  0001 C CNN
	1    5200 6000
	-1   0    0    -1  
$EndComp
$Comp
L Conn_01x04 J1
U 1 1 5A9B7BB7
P 3000 5500
F 0 "J1" H 3000 5700 50  0000 C CNN
F 1 "Conn_01x04" H 3000 5200 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch2.54mm" H 3000 5500 50  0001 C CNN
F 3 "" H 3000 5500 50  0001 C CNN
	1    3000 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2600 5700 2600 5600
Wire Wire Line
	2600 5600 2800 5600
$Comp
L Mounting_Hole MK1
U 1 1 5A9B3CC7
P 9350 1850
F 0 "MK1" H 9350 2050 50  0000 C CNN
F 1 "Mounting_Hole" H 9350 1975 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_ISO14580" H 9350 1850 50  0001 C CNN
F 3 "" H 9350 1850 50  0001 C CNN
	1    9350 1850
	1    0    0    -1  
$EndComp
$Comp
L Mounting_Hole MK2
U 1 1 5A9B58A8
P 9350 2250
F 0 "MK2" H 9350 2450 50  0000 C CNN
F 1 "Mounting_Hole" H 9350 2375 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_ISO14580" H 9350 2250 50  0001 C CNN
F 3 "" H 9350 2250 50  0001 C CNN
	1    9350 2250
	1    0    0    -1  
$EndComp
$Comp
L Mounting_Hole MK3
U 1 1 5A9B5965
P 9950 2050
F 0 "MK3" H 9950 2250 50  0000 C CNN
F 1 "Mounting_Hole" H 9950 2175 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_ISO14580" H 9950 2050 50  0001 C CNN
F 3 "" H 9950 2050 50  0001 C CNN
	1    9950 2050
	1    0    0    -1  
$EndComp
$Comp
L Mounting_Hole MK4
U 1 1 5A9B59F7
P 10700 1850
F 0 "MK4" H 10700 2050 50  0000 C CNN
F 1 "Mounting_Hole" H 10700 1975 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_ISO14580" H 10700 1850 50  0001 C CNN
F 3 "" H 10700 1850 50  0001 C CNN
	1    10700 1850
	1    0    0    -1  
$EndComp
$Comp
L Mounting_Hole MK5
U 1 1 5A9B5A70
P 10700 2250
F 0 "MK5" H 10700 2450 50  0000 C CNN
F 1 "Mounting_Hole" H 10700 2375 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3_ISO14580" H 10700 2250 50  0001 C CNN
F 3 "" H 10700 2250 50  0001 C CNN
	1    10700 2250
	1    0    0    -1  
$EndComp
$EndSCHEMATC

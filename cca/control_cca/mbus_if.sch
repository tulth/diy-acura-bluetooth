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
Sheet 3 4
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
L AS393P-E1 U1
U 2 1 5882F59E
P 4900 2900
F 0 "U1" H 5050 3050 50  0000 C CNN
F 1 "AS393P-E1" H 5150 2750 50  0000 C CNN
F 2 "Housings_DIP:DIP-8_W7.62mm_LongPads" V 5450 2900 50  0001 C CNN
F 3 "" H 4900 2900 50  0000 C CNN
	2    4900 2900
	1    0    0    -1  
$EndComp
$Comp
L R_thruhole R6
U 1 1 5882FA9D
P 4500 2400
F 0 "R6" H 4500 2480 50  0000 C CNN
F 1 "47.5K" H 4500 2300 50  0000 C CNN
F 2 "t_footprints:Resistor_Horizontal_RM10mm" H 4600 2250 50  0001 C CNN
F 3 "" V 4500 2300 50  0000 C CNN
	1    4500 2400
	1    0    0    -1  
$EndComp
$Comp
L R_thruhole R5
U 1 1 5882FB8F
P 4200 3300
F 0 "R5" H 4200 3380 50  0000 C CNN
F 1 "20K" H 4200 3200 50  0000 C CNN
F 2 "t_footprints:Resistor_Horizontal_RM10mm" H 4300 3150 50  0001 C CNN
F 3 "" V 4200 3200 50  0000 C CNN
	1    4200 3300
	0    1    1    0   
$EndComp
$Comp
L GND #PWR08
U 1 1 5882FBBF
P 4800 3300
F 0 "#PWR08" H 4800 3050 50  0001 C CNN
F 1 "GND" H 4800 3150 50  0000 C CNN
F 2 "" H 4800 3300 50  0000 C CNN
F 3 "" H 4800 3300 50  0000 C CNN
	1    4800 3300
	1    0    0    -1  
$EndComp
Text HLabel 1700 2100 0    60   Input ~ 0
sw12Vin
Text HLabel 1700 2800 0    60   BiDi ~ 0
mbusBi
Text HLabel 6300 2600 2    60   Output ~ 0
mbusSenseLowOut
$Comp
L GND #PWR09
U 1 1 5882FF09
P 5700 3200
F 0 "#PWR09" H 5700 2950 50  0001 C CNN
F 1 "GND" H 5700 3050 50  0000 C CNN
F 2 "" H 5700 3200 50  0000 C CNN
F 3 "" H 5700 3200 50  0000 C CNN
	1    5700 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 3200 4800 3300
Wire Wire Line
	5700 3100 5700 3200
Wire Wire Line
	5200 2900 5400 2900
Wire Wire Line
	6300 2600 5700 2600
Wire Wire Line
	5700 2600 5700 2700
Wire Wire Line
	4800 2100 4800 2600
Wire Wire Line
	4800 2400 4700 2400
Wire Wire Line
	1700 2100 4800 2100
Connection ~ 4800 2400
Wire Wire Line
	4600 2800 1700 2800
Wire Wire Line
	4600 3000 4200 3000
Wire Wire Line
	4200 2400 4200 3100
Wire Wire Line
	4200 2400 4300 2400
Connection ~ 4200 3000
$Comp
L GND #PWR010
U 1 1 5882FFF6
P 4200 3600
F 0 "#PWR010" H 4200 3350 50  0001 C CNN
F 1 "GND" H 4200 3450 50  0000 C CNN
F 2 "" H 4200 3600 50  0000 C CNN
F 3 "" H 4200 3600 50  0000 C CNN
	1    4200 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 3600 4200 3500
Wire Wire Line
	3500 4100 3500 2800
Connection ~ 3500 2800
$Comp
L GND #PWR011
U 1 1 58830045
P 3500 4600
F 0 "#PWR011" H 3500 4350 50  0001 C CNN
F 1 "GND" H 3500 4450 50  0000 C CNN
F 2 "" H 3500 4600 50  0000 C CNN
F 3 "" H 3500 4600 50  0000 C CNN
	1    3500 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 4600 3500 4500
Text HLabel 6300 4300 2    60   Output ~ 0
mbusDriveLowIn
Wire Wire Line
	6300 4300 3800 4300
$Comp
L BSS138N Q6
U 1 1 5883BB60
P 5600 2900
F 0 "Q6" H 5800 2975 50  0000 L CNN
F 1 "BSS138N" H 5800 2900 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 5800 2825 50  0001 L CIN
F 3 "" H 5600 2900 50  0000 L CNN
	1    5600 2900
	1    0    0    -1  
$EndComp
$Comp
L BSS138N Q5
U 1 1 5883BCB4
P 3600 4300
F 0 "Q5" H 3800 4375 50  0000 L CNN
F 1 "BSS138N" H 3800 4300 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 3800 4225 50  0001 L CIN
F 3 "" H 3600 4300 50  0000 L CNN
	1    3600 4300
	-1   0    0    -1  
$EndComp
$EndSCHEMATC

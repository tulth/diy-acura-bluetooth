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
Sheet 3 6
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
L R-78E5.0-0.5 U3
U 1 1 56F10F5D
P 4500 2900
F 0 "U3" H 4500 3200 60  0000 C CNN
F 1 "R-78E5.0-0.5" H 4500 3100 60  0000 C CNN
F 2 "diy-acura-bluetooth:R-78E5.0-0.5" H 4500 3100 60  0001 C CNN
F 3 "" H 4500 3100 60  0000 C CNN
F 4 "Recom" H 4500 2900 60  0001 C CNN "MFN"
F 5 "R-78E5.0-0.5" H 4500 2900 60  0001 C CNN "MFP"
	1    4500 2900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR012
U 1 1 56F112AE
P 4500 3300
F 0 "#PWR012" H 4500 3050 50  0001 C CNN
F 1 "GND" H 4500 3150 50  0000 C CNN
F 2 "" H 4500 3300 50  0000 C CNN
F 3 "" H 4500 3300 50  0000 C CNN
	1    4500 3300
	1    0    0    -1  
$EndComp
$Comp
L 1N4004 D1
U 1 1 56F11305
P 2350 2800
F 0 "D1" H 2350 2900 50  0000 C CNN
F 1 "1N4004" H 2350 2700 50  0000 C CNN
F 2 "Diodes_ThroughHole:Diode_DO-41_SOD81_Horizontal_RM10" H 2250 2800 50  0001 C CNN
F 3 "" H 2350 2800 50  0000 C CNN
F 4 "Diodes Incorporated" H 3500 1900 60  0001 C CNN "MFN"
F 5 "1N4004" H 3500 1900 60  0001 C CNN "MFP"
	1    2350 2800
	-1   0    0    1   
$EndComp
Text HLabel 1300 2800 0    60   Input ~ 0
battIn
Text HLabel 1300 3900 0    60   Input ~ 0
accIn
$Comp
L GND #PWR013
U 1 1 56F118B7
P 2500 4150
F 0 "#PWR013" H 2500 3900 50  0001 C CNN
F 1 "GND" H 2500 4000 50  0000 C CNN
F 2 "" H 2500 4150 50  0000 C CNN
F 3 "" H 2500 4150 50  0000 C CNN
	1    2500 4150
	1    0    0    -1  
$EndComp
$Comp
L 2N3904 Q3
U 1 1 56F1191A
P 3400 3900
F 0 "Q3" H 3700 3950 50  0000 R CNN
F 1 "2N3904" H 3400 3750 50  0000 R CNN
F 2 "TO_SOT_Packages_THT:TO-92_Molded_Wide_Oval" H 3600 4000 50  0001 C CNN
F 3 "" H 3400 3900 50  0000 C CNN
F 4 "On Semiconductor" H 3500 1900 60  0001 C CNN "MFN"
F 5 "2N3904" H 3500 1900 60  0001 C CNN "MFP"
	1    3400 3900
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR014
U 1 1 56F11AD9
P 3300 4150
F 0 "#PWR014" H 3300 3900 50  0001 C CNN
F 1 "GND" H 3300 4000 50  0000 C CNN
F 2 "" H 3300 4150 50  0000 C CNN
F 3 "" H 3300 4150 50  0000 C CNN
	1    3300 4150
	1    0    0    -1  
$EndComp
$Comp
L R R14
U 1 1 56F11BA1
P 3850 3900
F 0 "R14" V 3930 3900 50  0000 C CNN
F 1 "4.75K" V 3850 3900 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 3780 3900 50  0001 C CNN
F 3 "" H 3850 3900 50  0000 C CNN
F 4 "Yageo" V 2300 1600 60  0001 C CNN "MFN"
F 5 "MFR-25FBF52-4K75" V 2300 1600 60  0001 C CNN "MFP"
	1    3850 3900
	0    -1   -1   0   
$EndComp
$Comp
L CP C11
U 1 1 56F1227F
P 5200 3050
F 0 "C11" H 5225 3150 50  0000 L CNN
F 1 "100uF/10V" H 5225 2950 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D5_L11_P2" H 5238 2900 50  0001 C CNN
F 3 "" H 5200 3050 50  0000 C CNN
F 4 "Panasonic" H 5200 3050 60  0001 C CNN "MFN"
F 5 "ECA-1AM101" H 5200 3050 60  0001 C CNN "MFP"
	1    5200 3050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR015
U 1 1 56F12285
P 5200 3250
F 0 "#PWR015" H 5200 3000 50  0001 C CNN
F 1 "GND" H 5200 3100 50  0000 C CNN
F 2 "" H 5200 3250 50  0000 C CNN
F 3 "" H 5200 3250 50  0000 C CNN
	1    5200 3250
	1    0    0    -1  
$EndComp
Text HLabel 5600 800  2    60   Output ~ 0
SW12VOut
Text HLabel 5600 2800 2    60   Output ~ 0
5VOut
Text HLabel 5600 3900 2    60   Input ~ 0
holdPowerOnIn
$Comp
L AS393P-E1 U2
U 1 1 56F124B5
P 4400 1600
F 0 "U2" H 4550 1750 50  0000 C CNN
F 1 "AS393P-E1" H 4650 1450 50  0000 C CNN
F 2 "Housings_DIP:DIP-8_W7.62mm_LongPads" H 4400 1600 50  0001 C CNN
F 3 "" H 4400 1600 50  0000 C CNN
F 4 "Diodes Incorporated" H 4400 1600 60  0001 C CNN "MFN"
F 5 "AS393P-E1" H 4400 1600 60  0001 C CNN "MFP"
	1    4400 1600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR016
U 1 1 56F126D5
P 4300 1950
F 0 "#PWR016" H 4300 1700 50  0001 C CNN
F 1 "GND" H 4300 1800 50  0000 C CNN
F 2 "" H 4300 1950 50  0000 C CNN
F 3 "" H 4300 1950 50  0000 C CNN
	1    4300 1950
	1    0    0    -1  
$EndComp
Text HLabel 5500 1600 2    60   Input ~ 0
accSenseOut
$Comp
L R R13
U 1 1 56F12901
P 3800 1950
F 0 "R13" V 3880 1950 50  0000 C CNN
F 1 "47.5K" V 3800 1950 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 3730 1950 50  0001 C CNN
F 3 "" H 3800 1950 50  0000 C CNN
F 4 "Yageo" V 2300 1600 60  0001 C CNN "MFN"
F 5 "MFR-25FBF52-47K5" V 2300 1600 60  0001 C CNN "MFP"
	1    3800 1950
	1    0    0    -1  
$EndComp
$Comp
L R R15
U 1 1 56F1298C
P 4050 1200
F 0 "R15" V 4130 1200 50  0000 C CNN
F 1 "47.5K" V 4050 1200 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 3980 1200 50  0001 C CNN
F 3 "" H 4050 1200 50  0000 C CNN
F 4 "Yageo" V 2300 1600 60  0001 C CNN "MFN"
F 5 "MFR-25FBF52-47K5" V 2300 1600 60  0001 C CNN "MFP"
	1    4050 1200
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR017
U 1 1 56F12A3B
P 3800 2150
F 0 "#PWR017" H 3800 1900 50  0001 C CNN
F 1 "GND" H 3800 2000 50  0000 C CNN
F 2 "" H 3800 2150 50  0000 C CNN
F 3 "" H 3800 2150 50  0000 C CNN
	1    3800 2150
	1    0    0    -1  
$EndComp
$Comp
L R R12
U 1 1 56F4C395
P 2600 3150
F 0 "R12" V 2680 3150 50  0000 C CNN
F 1 "20K" V 2600 3150 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 2530 3150 50  0001 C CNN
F 3 "" H 2600 3150 50  0000 C CNN
F 4 "Yageo" V 2600 3150 60  0001 C CNN "MFN"
F 5 "MFR-25FBF52-20K" V 2600 3150 60  0001 C CNN "MFP"
	1    2600 3150
	-1   0    0    1   
$EndComp
$Comp
L IRFU9024NPBF Q2
U 1 1 56F9F590
P 2900 2900
F 0 "Q2" H 3200 2950 50  0000 R CNN
F 1 "IRFU9024NPBF" H 3600 2850 50  0000 R CNN
F 2 "diy-acura-bluetooth:TO-251AA-TPAD" H 3100 3000 50  0001 C CNN
F 3 "" H 2900 2900 50  0000 C CNN
F 4 "International Rectifier" H 2900 2900 60  0001 C CNN "MFN"
F 5 "IRFU9024NPBF" H 2900 2900 60  0001 C CNN "MFP"
	1    2900 2900
	0    1    -1   0   
$EndComp
$Comp
L R R19
U 1 1 5704874F
P 4100 4150
F 0 "R19" V 4180 4150 50  0000 C CNN
F 1 "4.75K" V 4100 4150 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 4030 4150 50  0001 C CNN
F 3 "" H 4100 4150 50  0000 C CNN
F 4 "Yageo" V 2550 1850 60  0001 C CNN "MFN"
F 5 "MFR-25FBF52-4K75" V 2550 1850 60  0001 C CNN "MFP"
	1    4100 4150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR018
U 1 1 57048826
P 4100 4350
F 0 "#PWR018" H 4100 4100 50  0001 C CNN
F 1 "GND" H 4100 4200 50  0000 C CNN
F 2 "" H 4100 4350 50  0000 C CNN
F 3 "" H 4100 4350 50  0000 C CNN
	1    4100 4350
	1    0    0    -1  
$EndComp
$Comp
L R R11
U 1 1 57048CAE
P 1700 4250
F 0 "R11" V 1780 4250 50  0000 C CNN
F 1 "47.5K" V 1700 4250 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 1630 4250 50  0001 C CNN
F 3 "" H 1700 4250 50  0000 C CNN
F 4 "Yageo" V 200 3900 60  0001 C CNN "MFN"
F 5 "MFR-25FBF52-47K5" V 200 3900 60  0001 C CNN "MFP"
	1    1700 4250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR019
U 1 1 57048CB5
P 1700 4450
F 0 "#PWR019" H 1700 4200 50  0001 C CNN
F 1 "GND" H 1700 4300 50  0000 C CNN
F 2 "" H 1700 4450 50  0000 C CNN
F 3 "" H 1700 4450 50  0000 C CNN
	1    1700 4450
	1    0    0    -1  
$EndComp
$Comp
L 5LN01SP Q1
U 1 1 57048EF1
P 2400 3900
F 0 "Q1" H 2700 3950 50  0000 R CNN
F 1 "5LN01SP" H 3050 3850 50  0000 R CNN
F 2 "TO_SOT_Packages_THT:TO-92_Molded_Wide_Oval" H 2600 4000 50  0001 C CNN
F 3 "" H 2400 3900 50  0000 C CNN
	1    2400 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 2800 4000 2800
Wire Wire Line
	4500 3250 4500 3300
Wire Wire Line
	2500 2800 2700 2800
Wire Wire Line
	1300 2800 2200 2800
Wire Wire Line
	2500 3700 2500 3400
Wire Wire Line
	2500 3400 3300 3400
Wire Wire Line
	2500 4100 2500 4150
Wire Wire Line
	3300 3400 3300 3700
Wire Wire Line
	2900 3400 2900 3100
Connection ~ 2900 3400
Wire Wire Line
	3300 4100 3300 4150
Wire Wire Line
	3600 3900 3700 3900
Wire Wire Line
	2600 3300 2600 3400
Connection ~ 2600 3400
Wire Wire Line
	2600 3000 2600 2800
Connection ~ 2600 2800
Wire Wire Line
	5200 3200 5200 3250
Wire Wire Line
	5000 2800 5600 2800
Wire Wire Line
	5200 2800 5200 2900
Wire Wire Line
	3300 2800 3300 800 
Wire Wire Line
	3300 800  5600 800 
Connection ~ 3300 2800
Connection ~ 5200 2800
Wire Wire Line
	5600 3900 4000 3900
Wire Wire Line
	4300 1900 4300 1950
Wire Wire Line
	4100 1700 3800 1700
Wire Wire Line
	3800 1200 3800 1800
Connection ~ 3800 1700
Wire Wire Line
	3800 2100 3800 2150
Wire Wire Line
	4300 800  4300 1300
Wire Wire Line
	4700 1600 5500 1600
Connection ~ 4300 800 
Connection ~ 4300 1200
Wire Wire Line
	4300 1200 4200 1200
Wire Wire Line
	3900 1200 3800 1200
Wire Wire Line
	4100 1500 1600 1500
Wire Wire Line
	1600 1500 1600 3900
Wire Wire Line
	3100 2700 3150 2700
Wire Wire Line
	3150 2700 3150 2800
Connection ~ 3150 2800
Wire Wire Line
	4100 3900 4100 4000
Connection ~ 4100 3900
Wire Wire Line
	4100 4300 4100 4350
Wire Wire Line
	1700 4400 1700 4450
Wire Wire Line
	1700 3900 1700 4100
Wire Wire Line
	1300 3900 2200 3900
Connection ~ 1600 3900
Connection ~ 1700 3900
$EndSCHEMATC

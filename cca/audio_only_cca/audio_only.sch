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
LIBS:audio_only-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 2
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
S 3100 1300 1200 1300
U 58832742
F0 "audio" 60
F1 "../bt_cca/audio.sch" 60
F2 "SPK_L+" I L 3100 2200 60 
F3 "SPK_L-" I L 3100 2300 60 
F4 "SPK_R+" I L 3100 2400 60 
F5 "SPK_R-" I L 3100 2500 60 
F6 "spk_right" O R 4300 2400 60 
F7 "spk_left" O R 4300 2200 60 
F8 "Vcc" I R 4300 1400 60 
F9 "MIC_L+" O L 3100 1700 60 
F10 "MIC_R+" O L 3100 1500 60 
F11 "MIC_L-" O L 3100 1800 60 
F12 "MIC_R-" O L 3100 1600 60 
F13 "M_BIAS" I L 3100 1900 60 
F14 "gnd" I R 4300 1500 60 
$EndSheet
$Comp
L GND #PWR01
U 1 1 58840E44
P 5300 2600
F 0 "#PWR01" H 5300 2350 50  0001 C CNN
F 1 "GND" H 5300 2450 50  0000 C CNN
F 2 "" H 5300 2600 50  0000 C CNN
F 3 "" H 5300 2600 50  0000 C CNN
	1    5300 2600
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5400 2500 5300 2500
Wire Wire Line
	5300 2500 5300 2600
Wire Wire Line
	5400 2300 4700 2300
Wire Wire Line
	5400 2400 4300 2400
Wire Wire Line
	4700 2300 4700 2200
Wire Wire Line
	4700 2200 4300 2200
$Comp
L GND #PWR02
U 1 1 58841645
P 5300 1200
F 0 "#PWR02" H 5300 950 50  0001 C CNN
F 1 "GND" H 5300 1050 50  0000 C CNN
F 2 "" H 5300 1200 50  0000 C CNN
F 3 "" H 5300 1200 50  0000 C CNN
	1    5300 1200
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5400 1100 5300 1100
Wire Wire Line
	5300 900  5300 1200
Wire Wire Line
	4900 1400 4300 1400
Wire Wire Line
	4900 900  4900 1400
Wire Wire Line
	4900 1000 5400 1000
Wire Wire Line
	3100 2200 1900 2200
Wire Wire Line
	1900 2200 1900 800 
Wire Wire Line
	1900 800  1300 800 
Wire Wire Line
	3100 2400 1800 2400
Wire Wire Line
	1800 2400 1800 900 
Wire Wire Line
	1800 900  1300 900 
Wire Wire Line
	3100 2300 1700 2300
Wire Wire Line
	1700 2300 1700 1000
Wire Wire Line
	1700 1000 1300 1000
Wire Wire Line
	3100 2500 1600 2500
Wire Wire Line
	1600 2500 1600 1100
Wire Wire Line
	1600 1100 1300 1100
Wire Wire Line
	1400 2800 2100 2800
Wire Wire Line
	2100 2800 2100 1600
Wire Wire Line
	2100 1600 3100 1600
Wire Wire Line
	1400 2900 2200 2900
Wire Wire Line
	2200 2900 2200 1800
Wire Wire Line
	2200 1800 3100 1800
Wire Wire Line
	1400 3000 2300 3000
Wire Wire Line
	2300 3000 2300 1500
Wire Wire Line
	2300 1500 3100 1500
Wire Wire Line
	1400 3100 2400 3100
Wire Wire Line
	2400 3100 2400 1700
Wire Wire Line
	2400 1700 3100 1700
Wire Wire Line
	1400 3200 2500 3200
Wire Wire Line
	2500 3200 2500 1900
Wire Wire Line
	2500 1900 3100 1900
$Comp
L PWR_FLAG #FLG03
U 1 1 588430D0
P 4900 900
F 0 "#FLG03" H 4900 995 50  0001 C CNN
F 1 "PWR_FLAG" H 4900 1080 50  0000 C CNN
F 2 "" H 4900 900 50  0000 C CNN
F 3 "" H 4900 900 50  0000 C CNN
	1    4900 900 
	1    0    0    -1  
$EndComp
Connection ~ 4900 1000
$Comp
L GND #PWR04
U 1 1 58843E32
P 4500 2700
F 0 "#PWR04" H 4500 2450 50  0001 C CNN
F 1 "GND" H 4500 2550 50  0000 C CNN
F 2 "" H 4500 2700 50  0000 C CNN
F 3 "" H 4500 2700 50  0000 C CNN
	1    4500 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 2700 4500 1500
Wire Wire Line
	4500 1500 4300 1500
$Comp
L PWR_FLAG #FLG05
U 1 1 5884411F
P 5300 900
F 0 "#FLG05" H 5300 995 50  0001 C CNN
F 1 "PWR_FLAG" H 5300 1080 50  0000 C CNN
F 2 "" H 5300 900 50  0000 C CNN
F 3 "" H 5300 900 50  0000 C CNN
	1    5300 900 
	1    0    0    -1  
$EndComp
Connection ~ 5300 1100
$Comp
L TCONN_01X02 P3
U 1 1 58852B8F
P 5600 1000
F 0 "P3" H 5600 1100 50  0000 C CNN
F 1 "TCONN_01X02" V 5700 950 50  0000 C CNN
F 2 "t_footprints:Pin_Header_Straight_1X02" H 5600 950 50  0001 C CNN
F 3 "" H 5600 950 50  0000 C CNN
	1    5600 1000
	1    0    0    -1  
$EndComp
$Comp
L TCONN_01X03 P4
U 1 1 588530A3
P 5600 2400
F 0 "P4" H 5600 2600 50  0000 C CNN
F 1 "TCONN_01X03" V 5700 2400 50  0000 C CNN
F 2 "t_footprints:Socket_Strip_Angled_1x03" H 5600 2400 50  0001 C CNN
F 3 "" H 5600 2400 50  0000 C CNN
	1    5600 2400
	1    0    0    -1  
$EndComp
$Comp
L TCONN_01X04 P1
U 1 1 588537AC
P 1100 900
F 0 "P1" H 1100 1100 50  0000 C CNN
F 1 "TCONN_01X04" V 1200 850 50  0000 C CNN
F 2 "t_footprints:Socket_Strip_Angled_1x04" H 1100 850 50  0001 C CNN
F 3 "" H 1100 850 50  0000 C CNN
	1    1100 900 
	-1   0    0    -1  
$EndComp
$Comp
L TCONN_01X05 P2
U 1 1 58854089
P 1200 3000
F 0 "P2" H 1200 3300 50  0000 C CNN
F 1 "TCONN_01X05" V 1300 3000 50  0000 C CNN
F 2 "Socket_Strip_Angled_1x05" H 1200 3000 50  0001 C CNN
F 3 "" H 1200 3000 50  0000 C CNN
	1    1200 3000
	-1   0    0    -1  
$EndComp
$EndSCHEMATC

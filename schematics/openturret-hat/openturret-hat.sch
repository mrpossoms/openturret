EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
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
L Driver_Motor:L293D U0
U 1 1 5EB772E1
P 7950 2200
F 0 "U0" H 7950 3381 50  0000 C CNN
F 1 "L293D" H 7950 3290 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 8200 1450 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/l293.pdf" H 7650 2900 50  0001 C CNN
	1    7950 2200
	1    0    0    -1  
$EndComp
$Comp
L Driver_Motor:L293D U1
U 1 1 5EB7E57E
P 7950 4600
F 0 "U1" H 7950 5781 50  0000 C CNN
F 1 "L293D" H 7950 5690 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 8200 3850 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/l293.pdf" H 7650 5300 50  0001 C CNN
	1    7950 4600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5EB8037B
P 7950 3100
F 0 "#PWR0101" H 7950 2850 50  0001 C CNN
F 1 "GND" H 7955 2927 50  0000 C CNN
F 2 "" H 7950 3100 50  0001 C CNN
F 3 "" H 7950 3100 50  0001 C CNN
	1    7950 3100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5EB80A96
P 7950 5500
F 0 "#PWR0102" H 7950 5250 50  0001 C CNN
F 1 "GND" H 7955 5327 50  0000 C CNN
F 2 "" H 7950 5500 50  0001 C CNN
F 3 "" H 7950 5500 50  0001 C CNN
	1    7950 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7950 3100 7850 3100
Wire Wire Line
	7750 3100 7750 3000
Wire Wire Line
	7950 3100 8050 3100
Wire Wire Line
	8150 3100 8150 3000
Connection ~ 7950 3100
Wire Wire Line
	8050 3000 8050 3100
Connection ~ 8050 3100
Wire Wire Line
	8050 3100 8150 3100
Wire Wire Line
	7850 3000 7850 3100
Connection ~ 7850 3100
Wire Wire Line
	7850 3100 7750 3100
Wire Wire Line
	7950 5500 7850 5500
Wire Wire Line
	7750 5500 7750 5400
Wire Wire Line
	7950 5500 8050 5500
Wire Wire Line
	8150 5500 8150 5400
Connection ~ 7950 5500
Wire Wire Line
	8050 5400 8050 5500
Connection ~ 8050 5500
Wire Wire Line
	8050 5500 8150 5500
Wire Wire Line
	7850 5400 7850 5500
Connection ~ 7850 5500
Wire Wire Line
	7850 5500 7750 5500
$Comp
L Device:R R0
U 1 1 5EB854AF
P 7300 1350
F 0 "R0" H 7370 1396 50  0000 L CNN
F 1 "10K" H 7370 1305 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 7230 1350 50  0001 C CNN
F 3 "~" H 7300 1350 50  0001 C CNN
	1    7300 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 1200 7850 1200
Wire Wire Line
	7850 1200 7300 1200
Connection ~ 7850 1200
Wire Wire Line
	7300 1500 7300 2000
Wire Wire Line
	7300 2000 7450 2000
Wire Wire Line
	7300 2000 7300 2600
Wire Wire Line
	7300 2600 7450 2600
Connection ~ 7300 2000
Wire Wire Line
	7300 2600 7300 4400
Wire Wire Line
	7300 4400 7450 4400
Connection ~ 7300 2600
Wire Wire Line
	7300 4400 7300 5000
Wire Wire Line
	7300 5000 7450 5000
Connection ~ 7300 4400
Wire Wire Line
	7850 3600 8050 3600
Text GLabel 7100 1800 0    50   Input ~ 0
M0-coil0-1
Text GLabel 7100 2200 0    50   Input ~ 0
M0-coil1-0
Text GLabel 7100 2400 0    50   Input ~ 0
M0-coil1-1
Text GLabel 7100 4000 0    50   Input ~ 0
M1-coil0-0
Text GLabel 7100 4200 0    50   Input ~ 0
M1-coil0-1
Text GLabel 7100 4600 0    50   Input ~ 0
M1-coil1-0
Text GLabel 7100 4800 0    50   Input ~ 0
M1-coil1-1
Wire Wire Line
	7500 2200 7450 2200
Wire Wire Line
	7450 1600 7100 1600
Wire Wire Line
	7100 1800 7450 1800
Wire Wire Line
	7450 2200 7100 2200
Connection ~ 7450 2200
Wire Wire Line
	7100 2400 7450 2400
Wire Wire Line
	7450 4000 7100 4000
Wire Wire Line
	7100 4200 7450 4200
Wire Wire Line
	7450 4600 7100 4600
Wire Wire Line
	7100 4800 7450 4800
Connection ~ 7300 1200
Connection ~ 7850 3600
$Comp
L power:GND #PWR0103
U 1 1 5EB9796B
P 6200 1500
F 0 "#PWR0103" H 6200 1250 50  0001 C CNN
F 1 "GND" H 6205 1327 50  0000 C CNN
F 2 "" H 6200 1500 50  0001 C CNN
F 3 "" H 6200 1500 50  0001 C CNN
	1    6200 1500
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:AMS1117-5.0 U2
U 1 1 5EB9353C
P 6200 1200
F 0 "U2" H 6200 1442 50  0000 C CNN
F 1 "AMS1117-5.0" H 6200 1351 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 6200 1400 50  0001 C CNN
F 3 "http://www.advanced-monolithic.com/pdf/ds1117.pdf" H 6300 950 50  0001 C CNN
	1    6200 1200
	1    0    0    -1  
$EndComp
Text GLabel 7100 1600 0    50   Input ~ 0
M0-coil0-0
Wire Wire Line
	6500 1200 7300 1200
$Comp
L power:GND #PWR0104
U 1 1 5EBA48F8
P 6200 3900
F 0 "#PWR0104" H 6200 3650 50  0001 C CNN
F 1 "GND" H 6205 3727 50  0000 C CNN
F 2 "" H 6200 3900 50  0001 C CNN
F 3 "" H 6200 3900 50  0001 C CNN
	1    6200 3900
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:AMS1117-5.0 U3
U 1 1 5EBA48FE
P 6200 3600
F 0 "U3" H 6200 3842 50  0000 C CNN
F 1 "AMS1117-5.0" H 6200 3751 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 6200 3800 50  0001 C CNN
F 3 "http://www.advanced-monolithic.com/pdf/ds1117.pdf" H 6300 3350 50  0001 C CNN
	1    6200 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6500 3600 7850 3600
$Comp
L power:VDC #PWR0105
U 1 1 5EBAAEE5
P 5900 1200
F 0 "#PWR0105" H 5900 1100 50  0001 C CNN
F 1 "VDC" V 5900 1429 50  0000 L CNN
F 2 "" H 5900 1200 50  0001 C CNN
F 3 "" H 5900 1200 50  0001 C CNN
	1    5900 1200
	0    -1   -1   0   
$EndComp
$Comp
L power:VDC #PWR0106
U 1 1 5EBACC9C
P 5900 3600
F 0 "#PWR0106" H 5900 3500 50  0001 C CNN
F 1 "VDC" V 5900 3829 50  0000 L CNN
F 2 "" H 5900 3600 50  0001 C CNN
F 3 "" H 5900 3600 50  0001 C CNN
	1    5900 3600
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Raspberry_Pi_2_3 J1
U 1 1 5EB83C47
P 2750 3200
F 0 "J1" H 2750 4681 50  0000 C CNN
F 1 "Raspberry_Pi_2_3" H 2750 4590 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" H 2750 3200 50  0001 C CNN
F 3 "https://www.raspberrypi.org/documentation/hardware/raspberrypi/schematics/rpi_SCH_3bplus_1p0_reduced.pdf" H 2750 3200 50  0001 C CNN
	1    2750 3200
	1    0    0    -1  
$EndComp
$EndSCHEMATC

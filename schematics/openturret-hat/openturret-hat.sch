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
	7850 1200 7550 1200
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
$Comp
L power:GND #PWR0103
U 1 1 5EB9796B
P 5150 1600
F 0 "#PWR0103" H 5150 1350 50  0001 C CNN
F 1 "GND" H 5155 1427 50  0000 C CNN
F 2 "" H 5150 1600 50  0001 C CNN
F 3 "" H 5150 1600 50  0001 C CNN
	1    5150 1600
	1    0    0    -1  
$EndComp
Text GLabel 7100 1600 0    50   Input ~ 0
M0-coil0-0
$Comp
L power:VDC #PWR0105
U 1 1 5EBAAEE5
P 4850 1150
F 0 "#PWR0105" H 4850 1050 50  0001 C CNN
F 1 "VDC" V 4850 1379 50  0000 L CNN
F 2 "" H 4850 1150 50  0001 C CNN
F 3 "" H 4850 1150 50  0001 C CNN
	1    4850 1150
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Raspberry_Pi_2_3 J1
U 1 1 5EB83C47
P 2150 3500
F 0 "J1" H 2150 4981 50  0000 C CNN
F 1 "Raspberry_Pi_2_3" H 2150 4890 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" H 2150 3500 50  0001 C CNN
F 3 "https://www.raspberrypi.org/documentation/hardware/raspberrypi/schematics/rpi_SCH_3bplus_1p0_reduced.pdf" H 2150 3500 50  0001 C CNN
	1    2150 3500
	1    0    0    -1  
$EndComp
Text GLabel 5850 3350 2    50   Input ~ 0
M0-coil0-0
Text GLabel 5850 3450 2    50   Input ~ 0
M0-coil0-1
Text GLabel 5850 3550 2    50   Input ~ 0
M0-coil1-0
Text GLabel 5850 3150 2    50   Input ~ 0
M0-coil1-1
$Comp
L Regulator_Linear:L7805 U3
U 1 1 5EBB398D
P 5150 1150
F 0 "U3" H 5150 1392 50  0000 C CNN
F 1 "L7805" H 5150 1301 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 5175 1000 50  0001 L CIN
F 3 "http://www.st.com/content/ccc/resource/technical/document/datasheet/41/4f/b3/b0/12/d4/47/88/CD00000444.pdf/files/CD00000444.pdf/jcr:content/translations/en.CD00000444.pdf" H 5150 1100 50  0001 C CNN
	1    5150 1150
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0104
U 1 1 5EBC7838
P 5850 1150
F 0 "#PWR0104" H 5850 1000 50  0001 C CNN
F 1 "VCC" V 5867 1278 50  0000 L CNN
F 2 "" H 5850 1150 50  0001 C CNN
F 3 "" H 5850 1150 50  0001 C CNN
	1    5850 1150
	0    1    1    0   
$EndComp
$Comp
L power:VCC #PWR0106
U 1 1 5EBC813C
P 7550 1200
F 0 "#PWR0106" H 7550 1050 50  0001 C CNN
F 1 "VCC" H 7567 1373 50  0000 C CNN
F 2 "" H 7550 1200 50  0001 C CNN
F 3 "" H 7550 1200 50  0001 C CNN
	1    7550 1200
	1    0    0    -1  
$EndComp
Connection ~ 7550 1200
Wire Wire Line
	7550 1200 7300 1200
$Comp
L power:VCC #PWR0107
U 1 1 5EBC8C69
P 7550 3600
F 0 "#PWR0107" H 7550 3450 50  0001 C CNN
F 1 "VCC" H 7567 3773 50  0000 C CNN
F 2 "" H 7550 3600 50  0001 C CNN
F 3 "" H 7550 3600 50  0001 C CNN
	1    7550 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	7550 3600 7850 3600
$Comp
L MCU_Microchip_ATtiny:ATtiny24A-PU U2
U 1 1 5EBF24A5
P 5050 3050
F 0 "U2" H 4521 3096 50  0000 R CNN
F 1 "ATtiny24A-PU" H 4521 3005 50  0000 R CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 5050 3050 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/doc8183.pdf" H 5050 3050 50  0001 C CNN
	1    5050 3050
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0108
U 1 1 5EBF53A8
P 5050 2150
F 0 "#PWR0108" H 5050 2000 50  0001 C CNN
F 1 "VCC" H 5067 2323 50  0000 C CNN
F 2 "" H 5050 2150 50  0001 C CNN
F 3 "" H 5050 2150 50  0001 C CNN
	1    5050 2150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 5EBF6FAF
P 5050 3950
F 0 "#PWR0109" H 5050 3700 50  0001 C CNN
F 1 "GND" H 5055 3777 50  0000 C CNN
F 2 "" H 5050 3950 50  0001 C CNN
F 3 "" H 5050 3950 50  0001 C CNN
	1    5050 3950
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x03_Odd_Even J2
U 1 1 5EBF7BFA
P 3950 1650
F 0 "J2" H 4000 1967 50  0000 C CNN
F 1 "Conn_02x03_Odd_Even" H 4000 1876 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" H 3950 1650 50  0001 C CNN
F 3 "~" H 3950 1650 50  0001 C CNN
	1    3950 1650
	1    0    0    -1  
$EndComp
Text GLabel 5850 2950 2    50   Output ~ 0
MISO
Wire Wire Line
	5850 2950 5650 2950
$Comp
L power:GND #PWR0110
U 1 1 5EBFCA9B
P 4350 1750
F 0 "#PWR0110" H 4350 1500 50  0001 C CNN
F 1 "GND" H 4355 1577 50  0000 C CNN
F 2 "" H 4350 1750 50  0001 C CNN
F 3 "" H 4350 1750 50  0001 C CNN
	1    4350 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4350 1750 4250 1750
Text GLabel 5850 2850 2    50   Input ~ 0
SCK
Wire Wire Line
	5850 2850 5650 2850
Text GLabel 3700 1550 0    50   Input ~ 0
MISO
Wire Wire Line
	3750 1550 3700 1550
Text GLabel 3700 1650 0    50   Output ~ 0
SCK
Wire Wire Line
	3700 1650 3750 1650
Text GLabel 4350 1650 2    50   Output ~ 0
MOSI
Wire Wire Line
	4350 1650 4250 1650
Text GLabel 5850 3050 2    50   Input ~ 0
MOSI
Wire Wire Line
	5850 3050 5650 3050
Text GLabel 5850 3650 2    50   Input ~ 0
RST
Wire Wire Line
	5850 3650 5650 3650
Text GLabel 3700 1750 0    50   Output ~ 0
RST
Wire Wire Line
	3700 1750 3750 1750
$Comp
L power:VCC #PWR0111
U 1 1 5EC15286
P 4350 1550
F 0 "#PWR0111" H 4350 1400 50  0001 C CNN
F 1 "VCC" V 4367 1678 50  0000 L CNN
F 2 "" H 4350 1550 50  0001 C CNN
F 3 "" H 4350 1550 50  0001 C CNN
	1    4350 1550
	0    1    1    0   
$EndComp
Wire Wire Line
	4350 1550 4250 1550
Wire Wire Line
	5850 3150 5650 3150
Wire Wire Line
	5650 3350 5850 3350
Wire Wire Line
	5850 3450 5650 3450
Wire Wire Line
	5650 3550 5850 3550
Text GLabel 5850 2450 2    50   Output ~ 0
M1-coil0-0
Text GLabel 5850 2550 2    50   Output ~ 0
M1-coil0-1
Text GLabel 5850 2650 2    50   Output ~ 0
M1-coil1-0
Text GLabel 5850 2750 2    50   Output ~ 0
M1-coil1-1
Wire Wire Line
	5850 2450 5650 2450
Wire Wire Line
	5650 2550 5850 2550
Wire Wire Line
	5650 2650 5850 2650
Wire Wire Line
	5850 2750 5650 2750
$Comp
L Device:CP1_Small C1
U 1 1 5EC5022A
P 5650 1250
F 0 "C1" H 5741 1296 50  0000 L CNN
F 1 "CP1_Small" H 5741 1205 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D5.0mm_P2.50mm" H 5650 1250 50  0001 C CNN
F 3 "~" H 5650 1250 50  0001 C CNN
	1    5650 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 1150 5650 1150
Connection ~ 5650 1150
Wire Wire Line
	5650 1150 5850 1150
Wire Wire Line
	5150 1450 5150 1500
Wire Wire Line
	5650 1350 5650 1500
Wire Wire Line
	5650 1500 5150 1500
Connection ~ 5150 1500
Wire Wire Line
	5150 1500 5150 1600
Text GLabel 3200 3900 2    50   Output ~ 0
MOSI
Text GLabel 3200 3800 2    50   Input ~ 0
MISO
Wire Wire Line
	3200 3800 2950 3800
Wire Wire Line
	2950 3900 3200 3900
Text GLabel 3200 4000 2    50   Output ~ 0
SCK
Wire Wire Line
	2950 4000 3200 4000
Wire Wire Line
	1750 4800 1850 4800
Wire Wire Line
	1950 4800 1850 4800
Connection ~ 1850 4800
Wire Wire Line
	1950 4800 2050 4800
Connection ~ 1950 4800
Wire Wire Line
	2050 4800 2150 4800
Connection ~ 2050 4800
Wire Wire Line
	2150 4800 2250 4800
Connection ~ 2150 4800
Wire Wire Line
	2250 4800 2350 4800
Connection ~ 2250 4800
Wire Wire Line
	2350 4800 2450 4800
Connection ~ 2350 4800
$Comp
L power:GND #PWR0112
U 1 1 5EC880D2
P 2450 4800
F 0 "#PWR0112" H 2450 4550 50  0001 C CNN
F 1 "GND" H 2455 4627 50  0000 C CNN
F 2 "" H 2450 4800 50  0001 C CNN
F 3 "" H 2450 4800 50  0001 C CNN
	1    2450 4800
	1    0    0    -1  
$EndComp
Connection ~ 2450 4800
$Comp
L power:VCC #PWR0113
U 1 1 5EC8896C
P 1950 2000
F 0 "#PWR0113" H 1950 1850 50  0001 C CNN
F 1 "VCC" H 1967 2173 50  0000 C CNN
F 2 "" H 1950 2000 50  0001 C CNN
F 3 "" H 1950 2000 50  0001 C CNN
	1    1950 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 2200 1950 2200
Wire Wire Line
	1950 2200 1950 2000
Connection ~ 1950 2200
Text GLabel 8600 1600 2    50   Output ~ 0
M0-coil0-a
Text GLabel 8600 1800 2    50   Output ~ 0
M0-coil0-b
Text GLabel 8600 2200 2    50   Output ~ 0
M0-coil1-a
Text GLabel 8600 2400 2    50   Output ~ 0
M0-coil1-b
Wire Wire Line
	8450 1600 8600 1600
Wire Wire Line
	8600 1800 8450 1800
Wire Wire Line
	8450 2200 8600 2200
Wire Wire Line
	8600 2400 8450 2400
Text GLabel 8600 4000 2    50   Output ~ 0
M1-coil0-a
Text GLabel 8600 4200 2    50   Output ~ 0
M1-coil0-b
Wire Wire Line
	8600 4000 8450 4000
Wire Wire Line
	8450 4200 8600 4200
Text GLabel 8600 4600 2    50   Output ~ 0
M1-coil1-a
Text GLabel 8600 4800 2    50   Output ~ 0
M1-coil1-b
Wire Wire Line
	8450 4600 8600 4600
Wire Wire Line
	8600 4800 8450 4800
$Comp
L power:VDC #PWR?
U 1 1 5ECBE4F7
P 8050 3600
F 0 "#PWR?" H 8050 3500 50  0001 C CNN
F 1 "VDC" H 8050 3875 50  0000 C CNN
F 2 "" H 8050 3600 50  0001 C CNN
F 3 "" H 8050 3600 50  0001 C CNN
	1    8050 3600
	1    0    0    -1  
$EndComp
$Comp
L power:VDC #PWR?
U 1 1 5ECBF674
P 8050 1200
F 0 "#PWR?" H 8050 1100 50  0001 C CNN
F 1 "VDC" H 8050 1475 50  0000 C CNN
F 2 "" H 8050 1200 50  0001 C CNN
F 3 "" H 8050 1200 50  0001 C CNN
	1    8050 1200
	1    0    0    -1  
$EndComp
$EndSCHEMATC

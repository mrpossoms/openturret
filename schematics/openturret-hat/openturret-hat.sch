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
P 5350 2350
F 0 "U0" H 5350 3531 50  0000 C CNN
F 1 "L293D" H 5350 3440 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 5600 1600 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/l293.pdf" H 5050 3050 50  0001 C CNN
	1    5350 2350
	1    0    0    -1  
$EndComp
$Comp
L Driver_Motor:L293D U1
U 1 1 5EB7E57E
P 5350 4750
F 0 "U1" H 5350 5931 50  0000 C CNN
F 1 "L293D" H 5350 5840 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 5600 4000 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/l293.pdf" H 5050 5450 50  0001 C CNN
	1    5350 4750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EB8037B
P 5350 3250
F 0 "#PWR?" H 5350 3000 50  0001 C CNN
F 1 "GND" H 5355 3077 50  0000 C CNN
F 2 "" H 5350 3250 50  0001 C CNN
F 3 "" H 5350 3250 50  0001 C CNN
	1    5350 3250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EB80A96
P 5350 5650
F 0 "#PWR?" H 5350 5400 50  0001 C CNN
F 1 "GND" H 5355 5477 50  0000 C CNN
F 2 "" H 5350 5650 50  0001 C CNN
F 3 "" H 5350 5650 50  0001 C CNN
	1    5350 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 3250 5250 3250
Wire Wire Line
	5150 3250 5150 3150
Wire Wire Line
	5350 3250 5450 3250
Wire Wire Line
	5550 3250 5550 3150
Connection ~ 5350 3250
Wire Wire Line
	5450 3150 5450 3250
Connection ~ 5450 3250
Wire Wire Line
	5450 3250 5550 3250
Wire Wire Line
	5250 3150 5250 3250
Connection ~ 5250 3250
Wire Wire Line
	5250 3250 5150 3250
Wire Wire Line
	5350 5650 5250 5650
Wire Wire Line
	5150 5650 5150 5550
Wire Wire Line
	5350 5650 5450 5650
Wire Wire Line
	5550 5650 5550 5550
Connection ~ 5350 5650
Wire Wire Line
	5450 5550 5450 5650
Connection ~ 5450 5650
Wire Wire Line
	5450 5650 5550 5650
Wire Wire Line
	5250 5550 5250 5650
Connection ~ 5250 5650
Wire Wire Line
	5250 5650 5150 5650
$Comp
L Device:R R0
U 1 1 5EB854AF
P 4700 1500
F 0 "R0" H 4770 1546 50  0000 L CNN
F 1 "10K" H 4770 1455 50  0000 L CNN
F 2 "" V 4630 1500 50  0001 C CNN
F 3 "~" H 4700 1500 50  0001 C CNN
	1    4700 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 1350 5250 1350
Wire Wire Line
	5250 1350 4700 1350
Connection ~ 5250 1350
Wire Wire Line
	4700 1650 4700 2150
Wire Wire Line
	4700 2150 4850 2150
Wire Wire Line
	4700 2150 4700 2750
Wire Wire Line
	4700 2750 4850 2750
Connection ~ 4700 2150
Wire Wire Line
	4700 2750 4700 4550
Wire Wire Line
	4700 4550 4850 4550
Connection ~ 4700 2750
Wire Wire Line
	4700 4550 4700 5150
Wire Wire Line
	4700 5150 4850 5150
Connection ~ 4700 4550
Wire Wire Line
	5250 3750 5450 3750
Text GLabel 4500 1950 0    50   Input ~ 0
M0-coil0-1
Text GLabel 4500 2350 0    50   Input ~ 0
M0-coil1-0
Text GLabel 4500 2550 0    50   Input ~ 0
M0-coil1-1
Text GLabel 4500 4150 0    50   Input ~ 0
M1-coil0-0
Text GLabel 4500 4350 0    50   Input ~ 0
M1-coil0-1
Text GLabel 4500 4750 0    50   Input ~ 0
M1-coil1-0
Text GLabel 4500 4950 0    50   Input ~ 0
M1-coil1-1
Wire Wire Line
	4900 2350 4850 2350
Wire Wire Line
	4850 1750 4500 1750
Wire Wire Line
	4500 1950 4850 1950
Wire Wire Line
	4850 2350 4500 2350
Connection ~ 4850 2350
Wire Wire Line
	4500 2550 4850 2550
Wire Wire Line
	4850 4150 4500 4150
Wire Wire Line
	4500 4350 4850 4350
Wire Wire Line
	4850 4750 4500 4750
Wire Wire Line
	4500 4950 4850 4950
Connection ~ 4700 1350
Connection ~ 5250 3750
$Comp
L power:GND #PWR?
U 1 1 5EB9796B
P 3600 1650
F 0 "#PWR?" H 3600 1400 50  0001 C CNN
F 1 "GND" H 3605 1477 50  0000 C CNN
F 2 "" H 3600 1650 50  0001 C CNN
F 3 "" H 3600 1650 50  0001 C CNN
	1    3600 1650
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:AMS1117-5.0 U?
U 1 1 5EB9353C
P 3600 1350
F 0 "U?" H 3600 1592 50  0000 C CNN
F 1 "AMS1117-5.0" H 3600 1501 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 3600 1550 50  0001 C CNN
F 3 "http://www.advanced-monolithic.com/pdf/ds1117.pdf" H 3700 1100 50  0001 C CNN
	1    3600 1350
	1    0    0    -1  
$EndComp
Text GLabel 4500 1750 0    50   Input ~ 0
M0-coil0-0
Wire Wire Line
	3900 1350 4700 1350
$Comp
L power:GND #PWR?
U 1 1 5EBA48F8
P 3600 4050
F 0 "#PWR?" H 3600 3800 50  0001 C CNN
F 1 "GND" H 3605 3877 50  0000 C CNN
F 2 "" H 3600 4050 50  0001 C CNN
F 3 "" H 3600 4050 50  0001 C CNN
	1    3600 4050
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:AMS1117-5.0 U?
U 1 1 5EBA48FE
P 3600 3750
F 0 "U?" H 3600 3992 50  0000 C CNN
F 1 "AMS1117-5.0" H 3600 3901 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 3600 3950 50  0001 C CNN
F 3 "http://www.advanced-monolithic.com/pdf/ds1117.pdf" H 3700 3500 50  0001 C CNN
	1    3600 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 3750 5250 3750
$Comp
L power:VDC #PWR?
U 1 1 5EBAAEE5
P 3300 1350
F 0 "#PWR?" H 3300 1250 50  0001 C CNN
F 1 "VDC" V 3300 1579 50  0000 L CNN
F 2 "" H 3300 1350 50  0001 C CNN
F 3 "" H 3300 1350 50  0001 C CNN
	1    3300 1350
	0    -1   -1   0   
$EndComp
$Comp
L power:VDC #PWR?
U 1 1 5EBACC9C
P 3300 3750
F 0 "#PWR?" H 3300 3650 50  0001 C CNN
F 1 "VDC" V 3300 3979 50  0000 L CNN
F 2 "" H 3300 3750 50  0001 C CNN
F 3 "" H 3300 3750 50  0001 C CNN
	1    3300 3750
	0    -1   -1   0   
$EndComp
$EndSCHEMATC

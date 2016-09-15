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
LIBS:moteino
LIBS:button-cache
EELAYER 25 0
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
L GND #PWR2
U 1 1 57DAD49F
P 2500 4650
F 0 "#PWR2" H 2500 4400 50  0001 C CNN
F 1 "GND" H 2500 4500 50  0000 C CNN
F 2 "" H 2500 4650 50  0000 C CNN
F 3 "" H 2500 4650 50  0000 C CNN
	1    2500 4650
	1    0    0    -1  
$EndComp
$Comp
L moteino U1
U 1 1 57DADA9A
P 4100 2650
F 0 "U1" H 4250 2900 60  0000 R CNN
F 1 "moteino" H 4250 2800 60  0000 R CNN
F 2 "" H 4100 2650 60  0001 C CNN
F 3 "" H 4100 2650 60  0001 C CNN
	1    4100 2650
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH SW1
U 1 1 57DADAD1
P 2500 3300
F 0 "SW1" H 2650 3410 50  0000 C CNN
F 1 "SW_PUSH" H 2500 3220 50  0000 C CNN
F 2 "" H 2500 3300 50  0000 C CNN
F 3 "" H 2500 3300 50  0000 C CNN
	1    2500 3300
	0    -1   -1   0   
$EndComp
$Comp
L SPEAKER SP1
U 1 1 57DADB38
P 3450 3850
F 0 "SP1" H 3350 4100 50  0000 C CNN
F 1 "PIEZO ELEMENT" H 3350 3600 50  0000 C CNN
F 2 "" H 3450 3850 50  0000 C CNN
F 3 "" H 3450 3850 50  0000 C CNN
	1    3450 3850
	-1   0    0    1   
$EndComp
Wire Wire Line
	4100 2750 2500 2750
Wire Wire Line
	2500 2500 2500 3000
Wire Wire Line
	2500 3600 2500 4650
Wire Wire Line
	4100 3150 3750 3150
Wire Wire Line
	3750 3150 3750 3750
Wire Wire Line
	3750 3950 3750 4400
Wire Wire Line
	3750 4400 2500 4400
Connection ~ 2500 4400
$Comp
L LED D1
U 1 1 57DADC0A
P 4600 4350
F 0 "D1" H 4600 4450 50  0000 C CNN
F 1 "LED" H 4600 4250 50  0000 C CNN
F 2 "" H 4600 4350 50  0000 C CNN
F 3 "" H 4600 4350 50  0000 C CNN
	1    4600 4350
	-1   0    0    1   
$EndComp
Wire Wire Line
	4100 3450 3950 3450
Wire Wire Line
	3950 3450 3950 4350
Wire Wire Line
	3950 4350 4400 4350
$Comp
L GND #PWR3
U 1 1 57DADC66
P 5500 4700
F 0 "#PWR3" H 5500 4450 50  0001 C CNN
F 1 "GND" H 5500 4550 50  0000 C CNN
F 2 "" H 5500 4700 50  0000 C CNN
F 3 "" H 5500 4700 50  0000 C CNN
	1    5500 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 4350 5000 4350
$Comp
L +5V #PWR4
U 1 1 57DADCB3
P 6450 2450
F 0 "#PWR4" H 6450 2300 50  0001 C CNN
F 1 "+5V" H 6450 2590 50  0000 C CNN
F 2 "" H 6450 2450 50  0000 C CNN
F 3 "" H 6450 2450 50  0000 C CNN
	1    6450 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 2950 6450 2950
Wire Wire Line
	6450 2950 6450 2450
$Comp
L GND #PWR6
U 1 1 57DADCFF
P 7100 3100
F 0 "#PWR6" H 7100 2850 50  0001 C CNN
F 1 "GND" H 7100 2950 50  0000 C CNN
F 2 "" H 7100 3100 50  0000 C CNN
F 3 "" H 7100 3100 50  0000 C CNN
	1    7100 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 2850 7100 2850
Wire Wire Line
	7100 2850 7100 3100
$Comp
L +3.3V #PWR5
U 1 1 57DADD30
P 6750 2700
F 0 "#PWR5" H 6750 2550 50  0001 C CNN
F 1 "+3.3V" H 6750 2840 50  0000 C CNN
F 2 "" H 6750 2700 50  0000 C CNN
F 3 "" H 6750 2700 50  0000 C CNN
	1    6750 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 3050 6750 3050
Wire Wire Line
	6750 3050 6750 2700
NoConn ~ 6100 3150
NoConn ~ 6100 3250
NoConn ~ 6100 3350
NoConn ~ 6100 3450
NoConn ~ 6100 3550
NoConn ~ 6100 3650
NoConn ~ 6100 3750
NoConn ~ 6100 3850
NoConn ~ 5400 1850
NoConn ~ 5300 1850
NoConn ~ 5200 1850
NoConn ~ 5100 1850
NoConn ~ 5000 1850
NoConn ~ 4900 1850
NoConn ~ 4100 2650
NoConn ~ 4100 2850
NoConn ~ 4100 2950
NoConn ~ 4100 3050
NoConn ~ 4100 3250
NoConn ~ 4100 3350
NoConn ~ 4100 3550
NoConn ~ 4100 3650
NoConn ~ 4100 3750
NoConn ~ 4100 3850
$Comp
L R R1
U 1 1 57DADE37
P 2500 2350
F 0 "R1" V 2580 2350 50  0000 C CNN
F 1 "10K" V 2500 2350 50  0000 C CNN
F 2 "" V 2430 2350 50  0000 C CNN
F 3 "" H 2500 2350 50  0000 C CNN
F 4 "~" H 2500 2350 60  0001 C CNN "Characteristics"
F 5 "~" H 2500 2350 60  0001 C CNN "Description"
F 6 "~" H 2500 2350 60  0001 C CNN "Package ID"
F 7 "~" H 2500 2350 60  0001 C CNN "Source"
F 8 "~" H 2500 2350 60  0001 C CNN "Critical"
F 9 "~" H 2500 2350 60  0001 C CNN "Notes"
F 10 "~" H 2500 2350 60  0001 C CNN "MFP"
F 11 "~" H 2500 2350 60  0001 C CNN "supplier1_name"
F 12 "~" H 2500 2350 60  0001 C CNN "supplier2_name"
F 13 "~" H 2500 2350 60  0001 C CNN "supplier1_link"
F 14 "~" H 2500 2350 60  0001 C CNN "supplier2_link"
F 15 "~" H 2500 2350 60  0001 C CNN "MFN"
	1    2500 2350
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR1
U 1 1 57DADE5A
P 2500 2000
F 0 "#PWR1" H 2500 1850 50  0001 C CNN
F 1 "+3.3V" H 2500 2140 50  0000 C CNN
F 2 "" H 2500 2000 50  0000 C CNN
F 3 "" H 2500 2000 50  0000 C CNN
	1    2500 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 2000 2500 2200
Connection ~ 2500 2750
$Comp
L R R?
U 1 1 57DADEFB
P 5150 4350
F 0 "R?" V 5230 4350 50  0000 C CNN
F 1 "560R" V 5150 4350 50  0000 C CNN
F 2 "" V 5080 4350 50  0000 C CNN
F 3 "" H 5150 4350 50  0000 C CNN
	1    5150 4350
	0    1    1    0   
$EndComp
Wire Wire Line
	5300 4350 5500 4350
Wire Wire Line
	5500 4350 5500 4700
$EndSCHEMATC

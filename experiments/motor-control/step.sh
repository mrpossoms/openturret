#!/usr/bin/env bash

coil0_0=18
coil0_1=23
coil1_0=24
coil1_1=25

if [ $1 -gt 0 ]; then
tc0=$coil0_0
tc1=$coil0_1
coil0_0=$coil1_0
coil0_1=$coil1_1
coil1_0=$tc0
coil1_1=$tc1
fi

a=1
b=0

for i in $(seq 1 $2); do

#sleep 1

./gpio.sh $coil0_0 $a
./gpio.sh $coil0_1 $b

./gpio.sh $coil1_0 $b
./gpio.sh $coil1_1 $a

#sleep 1

./gpio.sh $coil0_0 $b
./gpio.sh $coil0_1 $a

./gpio.sh $coil1_0 $a
./gpio.sh $coil1_1 $b

done

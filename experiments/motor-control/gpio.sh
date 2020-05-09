#!/usr/bin/env bash

gpio_path=/sys/class/gpio/gpio$1

echo $1 > /sys/class/gpio/export
echo out > $gpio_path/direction
echo $2 > $gpio_path/value

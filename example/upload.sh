#!/bin/bash

DEVICE=$1
TARGET=$2
SERIALPORT=$3
#BAUDRATE=115200
BAUDRATE=57600

avrdude -Cavrdude.conf -v -p$DEVICE -carduino -P$SERIALPORT -b$BAUDRATE -D -Uflash:w:$TARGET:i

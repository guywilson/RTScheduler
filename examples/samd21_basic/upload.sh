#!/bin/bash

PORT=$1
BINFILE=$2

stty 1200 < $PORT

sleep 1

/Users/guy/Library/Arduino15/packages/arduino/tools/bossac/1.7.0-arduino3/bossac -i -d -p $PORT -U true -e -w -v $BINFILE -R

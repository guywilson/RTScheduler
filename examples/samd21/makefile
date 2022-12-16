###############################################################################
#                                                                             #
# MAKEFILE for real-time scheduler example - Runs on an Arduino, tested on    #
# an Arduino Nano and Uno                                                     #
#                                                                             #
# Guy Wilson (c) 2019                                                         #
#                                                                             #
###############################################################################

PROJNAME=rtssamd

# Target device
DEVICE=cortex-m0plus

# Target architecture size
ARCHSIZE=32

# What is our target
ELF=$(PROJNAME).elf
HEX=$(PROJNAME).hex
TARGET=$(PROJNAME).bin

# Build output directory
BUILD=build

# Source directory
SRC=src

# Arduino base directory
ARDUINOBASE=/Users/guy/Library/Arduino15/packages/arduino
CMSISBASE=$(ARDUINOBASE)/tools/CMSIS
SAMDBASE=$(ARDUINOBASE)/hardware/samd/1.8.11

# Port we use to upload the target to the device
UPLOADPORT=/dev/cu.usbmodem101

# Tools
CC=arm-none-eabi-gcc
LINKER=arm-none-eabi-g++
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump
SIZETOOL=arm-none-eabi-size
UPLOADTOOL=/Users/guy/Library/Arduino15/packages/arduino/tools/bossac/1.7.0-arduino3/bossac

# Upload flags
UPLOADFLAGS=-i -d -e -w -v -R -p

# Include dir flags
INCLUDEFLAGS=-I/Users/guy/development/RTScheduler/sched/samd21/src -I$(CMSISBASE)/4.5.0/CMSIS/Include/ -I$(ARDUINOBASE)/tools/CMSIS-Atmel/1.2.0/CMSIS/Device/ATMEL/ -I$(ARDUINOBASE)/tools/CMSIS-Atmel/1.2.0/CMSIS/Device/ATMEL/samd21/include/ -I$(SAMDBASE)/cores/arduino/api/deprecated -I$(SAMDBASE)/cores/arduino/api/deprecated-avr-comp -I$(SAMDBASE)/cores/arduino -I$(SAMDBASE)/variants/mkrwan1300

# C compiler flags
CFLAGS=-c -mcpu=$(DEVICE) -mthumb -g -Os -Wall -std=gnu11 -ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500 -DF_CPU=48000000L -DARDUINO=10813 -DARDUINO_SAMD_MKRWAN1310 -DARDUINO_ARCH_SAMD -DUSE_ARDUINO_MKR_PIN_LAYOUT -D__SAMD21G18A__ -DUSB_VID=0x2341 -DUSB_PID=0x8059 -DUSBCON "-DUSB_MANUFACTURER=\"Arduino LLC\"" "-DUSB_PRODUCT=\"Arduino MKR WAN 1310\"" -DUSE_BQ24195L_PMIC -DVERY_LOW_POWER $(INCLUDEFLAGS)

# Library paths
LIBDIRS=-L$(CMSISBASE)/4.5.0/CMSIS/Lib/GCC -L/Users/guy/development/arduino-core/samd21/lib -L/Users/guy/development/RTScheduler/sched/samd21/lib

# External Libraries
EXTLIBS=-larm_cortexM0l_math -lcore-samd21 -lsched

# Linker flags
LFLAGS=-Wl,--gc-sections -T$(SAMDBASE)/variants/mkrwan1300/linker_scripts/gcc/flash_with_bootloader.ld --specs=nano.specs --specs=nosys.specs -mcpu=cortex-m0plus -mthumb $(LIBDIRS) -Wl,--check-sections -Wl,--gc-sections -Wl,--warn-common -Wl,--warn-section-align

# Hex file creator flags
OBJCOPYFLAGS=-O ihex -R .eeprom

#Obj dump flags
OBJDUMPFLAGS=-I $(SRC) -f -s -l -S

# Size tool flags
SFLAGS=-A

# Object files
OBJFILES=$(BUILD)/led_utils.o $(BUILD)/heartbeat.o $(BUILD)/rtc_samd21.o $(BUILD)/error_samd21.o $(BUILD)/main.o

# Target
all: $(TARGET)

###############################################################################
#
# Project files
#
###############################################################################
$(BUILD)/led_utils.o: $(SRC)/led_utils.c $(SRC)/led_utils.h
	$(CC) $(CFLAGS) -o $(BUILD)/led_utils.o $(SRC)/led_utils.c

$(BUILD)/heartbeat.o: $(SRC)/heartbeat.c $(SRC)/heartbeat.h $(SRC)/led_utils.h $(SRC)/taskdef.h
	$(CC) $(CFLAGS) -o $(BUILD)/heartbeat.o $(SRC)/heartbeat.c

$(BUILD)/error_samd21.o: $(SRC)/error_samd21.c $(SRC)/led_utils.h
	$(CC) $(CFLAGS) -o $(BUILD)/error_samd21.o $(SRC)/error_samd21.c

$(BUILD)/rtc_samd21.o: $(SRC)/rtc_samd21.c $(SRC)/rtc_samd21.h
	$(CC) $(CFLAGS) -o $(BUILD)/rtc_samd21.o $(SRC)/rtc_samd21.c

$(BUILD)/main.o: $(SRC)/main.c $(SRC)/heartbeat.h $(SRC)/taskdef.h $(SRC)/led_utils.h $(SRC)/rtc_samd21.h
	$(CC) $(CFLAGS) -o $(BUILD)/main.o $(SRC)/main.c

###############################################################################
#
# Link it all together into an ELF format file
#
###############################################################################
$(BUILD)/$(ELF): $(OBJFILES)
	$(LINKER) $(LFLAGS) -o $(BUILD)/$(ELF) $(OBJFILES) -Wl,--start-group -lm $(EXTLIBS) -Wl,--end-group
	$(OBJDUMP) $(OBJDUMPFLAGS) $(BUILD)/$(ELF) > $(PROJNAME).s

###############################################################################
#
# Create the target
#
###############################################################################
$(TARGET): $(BUILD)/$(ELF)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD)/$(ELF) $(HEX)
	$(OBJCOPY) -O binary $(BUILD)/$(ELF) $(TARGET)
	$(SIZETOOL) $(SFLAGS) $(BUILD)/$(ELF)

###############################################################################
#
# Upload to the device, use 'make install' to envoke
#
###############################################################################
install: $(TARGET)
	stty 1200 < $(UPLOADPORT)
	sleep 1
	$(UPLOADTOOL) $(UPLOADFLAGS) $(UPLOADPORT) $(TARGET)
	
clean: 
	rm $(BUILD)/*
	rm $(PROJNAME).s
	rm $(HEX)
	rm $(TARGET)

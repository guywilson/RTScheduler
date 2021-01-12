###############################################################################
#                                                                             #
# MAKEFILE for real-time scheduler example - Runs on an Arduino, tested on    #
# an Arduino Nano and Uno                                                     #
#                                                                             #
# Guy Wilson (c) 2019                                                         #
#                                                                             #
###############################################################################

PROJNAME=rtsavr

# Target device
DEVICE=atmega328p

# Target board
BOARD=ARDUINO_AVR_NANO

# Target architecture size
ARCHSIZE=8

# What is our target
ELF=$(PROJNAME).elf
TARGET=$(PROJNAME).hex

# Build output directory
BUILD=build

# Source directory
SRC=src

# Scheduler source directory
SCHEDSRC=src/sched

# Port we use to upload the target to the device
UPLOADPORT=/dev/cu.wchusbserial1410
#UPLOADPORT=/dev/cu.usbmodem1411

# C compiler
CC=avr-gcc

# Linker
LINKER=avr-gcc

# Hex file creator
OBJCOPY=avr-objcopy

# Assembly dump tool
OBJDUMP=avr-objdump

# Size tool
SIZETOOL=avr-size

# Upload tool
UPLOADTOOL=./upload.sh

# C compiler flags
CFLAGS=-c -Os -Wall -ffunction-sections -fdata-sections -mmcu=$(DEVICE) -DF_CPU=16000000L -DARDUINO=10804 -D$(BOARD) -DARDUINO_ARCH_AVR -DARCH_SIZE=$(ARCHSIZE)

# Linker flags
LFLAGS=-fuse-linker-plugin -Wl,--gc-sections -mmcu=$(DEVICE)

# Hex file creator flags
OBJCOPYFLAGS=-O ihex -R .eeprom

#Obj dump flags
OBJDUMPFLAGS=-I $(SRC) -f -s -l -S

# Size tool flags
SFLAGS=-C --mcu=$(DEVICE)

# Object files
OBJFILES=$(BUILD)/led_utils.o $(BUILD)/heartbeat.o $(BUILD)/rtc_atmega328p.o $(BUILD)/error_atmega328p.o $(BUILD)/main.o

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

$(BUILD)/rtc_atmega328p.o: $(SRC)/rtc_atmega328p.c $(SRC)/rtc_atmega328p.h
	$(CC) $(CFLAGS) -o $(BUILD)/rtc_atmega328p.o $(SRC)/rtc_atmega328p.c

$(BUILD)/error_atmega328p.o: $(SRC)/error_atmega328p.c $(SRC)/led_utils.h
	$(CC) $(CFLAGS) -o $(BUILD)/error_atmega328p.o $(SRC)/error_atmega328p.c

$(BUILD)/main.o: $(SRC)/main.c $(SRC)/heartbeat.h $(SRC)/taskdef.h $(SRC)/led_utils.h $(SRC)/rtc_atmega328p.h
	$(CC) $(CFLAGS) -o $(BUILD)/main.o $(SRC)/main.c

###############################################################################
#
# Link it all together into an ELF format file
#
###############################################################################
$(BUILD)/$(ELF): $(OBJFILES)
	$(LINKER) $(LFLAGS) -o $(BUILD)/$(ELF) $(OBJFILES) -lm -lsched
	$(OBJDUMP) $(OBJDUMPFLAGS) $(BUILD)/$(ELF) > $(PROJNAME).s

###############################################################################
#
# Create the target
#
###############################################################################
$(TARGET): $(BUILD)/$(ELF)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD)/$(ELF) $(TARGET)
	$(SIZETOOL) $(SFLAGS) $(BUILD)/$(ELF)

###############################################################################
#
# Upload to the device, use 'make install' to envoke
#
###############################################################################
install: $(TARGET)
	$(UPLOADTOOL) $(DEVICE) $(TARGET) $(UPLOADPORT)
	
clean: 
	rm $(BUILD)/*
	rm $(PROJNAME).s
	rm $(TARGET)

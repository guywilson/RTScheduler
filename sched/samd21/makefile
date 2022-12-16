###############################################################################
#                                                                             #
# MAKEFILE for rtwc - A weather station with a real-time scheduler            #
#                                                                             #
# Guy Wilson (c) 2018                                                         #
#                                                                             #
###############################################################################

PROJNAME=libsched

# Target device
DEVICE=cortex-m0plus

# Target architecture size
ARCHSIZE=8

# What is our target
TARGET=$(PROJNAME).a

# Version number
MAJOR_VERSION=1
MINOR_VERSION=5

# Build output directory
BUILD=build

# Source directory
SRC=src

# Library directory
LIBDIR=lib

# Version template dir
VERSION_DIR=template

# C compiler
CC=arm-none-eabi-gcc

# Linker
LIBTOOL=arm-none-eabi-ar

# Version builder
VBUILD=vbuild

# C compiler flags
CFLAGS=-c -mcpu=$(DEVICE) -mthumb -g -Os -Wall -std=gnu11 -ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500 -DF_CPU=48000000L -DARDUINO=10813 -DARDUINO_SAMD_MKRWAN1310 -DARDUINO_ARCH_SAMD -DUSE_ARDUINO_MKR_PIN_LAYOUT -D__SAMD21G18A__ -DUSB_VID=0x2341 -DUSB_PID=0x8059 -DUSBCON "-DUSB_MANUFACTURER=\"Arduino LLC\"" "-DUSB_PRODUCT=\"Arduino MKR WAN 1310\"" -DUSE_BQ24195L_PMIC -DVERY_LOW_POWER $(INCLUDEFLAGS)

# Lib tool flags
LIBFLAGS=rcs

# Scheduler object files
OBJFILES=$(BUILD)/scheduler.o $(BUILD)/version.o

# Target
all: $(TARGET)

###############################################################################
#
# Scheduler files
#
###############################################################################
$(BUILD)/scheduler.o: $(SRC)/scheduler.c $(SRC)/scheduler.h $(SRC)/schederr.h
	$(CC) $(CFLAGS) -o $(BUILD)/scheduler.o $(SRC)/scheduler.c

$(BUILD)/version.o: $(SRC)/version.c
	$(CC) $(CFLAGS) -o $(BUILD)/version.o $(SRC)/version.c

###############################################################################
#
# Create the library
#
###############################################################################
$(TARGET): $(OBJFILES)
	$(LIBTOOL) $(LIBFLAGS) $(LIBDIR)/$(TARGET) $(OBJFILES)

version:
	$(VBUILD) -incfile scheduler.ver -template template/version.c.template -out $(SRC)/version.c -major $(MAJOR_VERSION) -minor $(MINOR_VERSION)

install:
	cp $(LIBDIR)/$(TARGET) /usr/lib
	cp $(SRC)/*.h /usr/include

clean:
	rm $(BUILD)/*
	rm $(LIBDIR)/$(TARGET)

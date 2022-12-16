###############################################################################
#                                                                             #
# MAKEFILE for rtwc - A weather station with a real-time scheduler            #
#                                                                             #
# Guy Wilson (c) 2018                                                         #
#                                                                             #
###############################################################################

PROJNAME=libsched

# Target device
DEVICE=atmega328p

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
CC=avr-gcc

# Linker
LIBTOOL=avr-ar

# Version builder
VBUILD=vbuild

# C compiler flags
CFLAGS=-c -O1 -Wall -ffunction-sections -fdata-sections -mmcu=$(DEVICE) -DF_CPU=16000000L -DARDUINO=10804 -DARDUINO_ARCH_AVR -DARCH_SIZE=$(ARCHSIZE)

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
	cp $(LIBDIR)/$(TARGET) $${HOME}/lib
	cp $(SRC)/*.h $${HOME}/include
	
clean: 
	rm $(BUILD)/*
	rm $(LIBDIR)/$(TARGET)

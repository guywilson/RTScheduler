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

# Build output directory
BUILD=build

# Source directory
SRC=src

# Library directory
LIBDIR=lib

# C compiler
CC=avr-gcc

# Linker
LIBTOOL=avr-ar

# C compiler flags
CFLAGS=-c -O1 -Wall -ffunction-sections -fdata-sections -mmcu=$(DEVICE) -DF_CPU=16000000L -DARDUINO=10804 -DARDUINO_ARCH_AVR -DARCH_SIZE=$(ARCHSIZE)

# Lib tool flags
LIBFLAGS=rcs

# Scheduler object files
OBJFILES=$(BUILD)/scheduler.o

# Target
all: $(TARGET)

###############################################################################
#
# Scheduler files
#
###############################################################################
$(BUILD)/scheduler.o: $(SRC)/scheduler.c $(SRC)/scheduler.h $(SRC)/schederr.h
	$(CC) $(CFLAGS) -o $(BUILD)/scheduler.o $(SRC)/scheduler.c

###############################################################################
#
# Create the library
#
###############################################################################
$(TARGET): $(OBJFILES)
	$(LIBTOOL) $(LIBFLAGS) $(LIBDIR)/$(TARGET) $(OBJFILES)

install:
	cp $(LIBDIR)/$(TARGET) /usr/local/Cellar/avr-gcc/9.1.0/lib/avr-gcc/9/gcc/avr/9.1.0
	cp $(SRC)/*.h /usr/local/Cellar/avr-gcc/9.1.0/lib/avr-gcc/9/gcc/avr/9.1.0/include
	
clean: 
	rm $(BUILD)/*
	rm $(LIBDIR)/$(TARGET)

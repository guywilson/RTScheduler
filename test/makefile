###############################################################################
#                                                                             #
# MAKEFILE for real-time scheduler example - Runs on an Arduino, tested on    #
# an Arduino Nano and Uno                                                     #
#                                                                             #
# Guy Wilson (c) 2019                                                         #
#                                                                             #
###############################################################################

PROJNAME=rtstest

# Target architecture size
ARCHSIZE=8

# What is our target
TARGET=$(PROJNAME)

# Build output directory
BUILD=build

# Source directory
SRC=src

# Scheduler source directory
SCHEDSRC=../sched/src

# C compiler
CC=gcc

# Linker
LINKER=gcc

# C compiler flags
CFLAGS=-c -O1 -Wall -DF_CPU=16000000L -DARCH_SIZE=$(ARCHSIZE) -DUNIT_TEST_MODE

# Object files
OBJFILES=$(BUILD)/test.o $(BUILD)/sched_utils.o $(BUILD)/scheduler.o

# Target
all: $(TARGET)

###############################################################################
#
# Project files
#
###############################################################################
$(BUILD)/test.o: $(SRC)/test.c $(SCHEDSRC)/scheduler.h
	$(CC) $(CFLAGS) -o $(BUILD)/test.o $(SRC)/test.c

$(BUILD)/sched_utils.o: $(SRC)/sched_utils.c $(SCHEDSRC)/scheduler.h
	$(CC) $(CFLAGS) -o $(BUILD)/sched_utils.o $(SRC)/sched_utils.c

$(BUILD)/scheduler.o: $(SCHEDSRC)/scheduler.c $(SCHEDSRC)/scheduler.h
	$(CC) $(CFLAGS) -o $(BUILD)/scheduler.o $(SCHEDSRC)/scheduler.c

###############################################################################
#
# Link it all together into an ELF format file
#
###############################################################################
$(TARGET): $(OBJFILES)
	$(LINKER) -lpthread -lstdc++ -o $(TARGET) $(OBJFILES)

clean: 
	rm $(BUILD)/*
	rm $(TARGET)

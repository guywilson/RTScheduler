###############################################################################
#                                                                             #
# MAKEFILE for real-time scheduler example - Runs on an Arduino, tested on    #
# an Arduino Nano and Uno                                                     #
#                                                                             #
# Guy Wilson (c) 2019                                                         #
#                                                                             #
###############################################################################

PROJNAME=rtstest

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
CPP=g++

# Linker
LINKER=g++

# C compiler flags
CFLAGS=-c -O1 -Wall -DUNIT_TEST_MODE

# Object files
OBJFILES=$(BUILD)/test.o $(BUILD)/strutils.o $(BUILD)/currenttime.o $(BUILD)/logger.o $(BUILD)/sched_utils.o $(BUILD)/scheduler.o

# Target
all: $(TARGET)

###############################################################################
#
# Project files
#
###############################################################################
$(BUILD)/test.o: $(SRC)/test.cpp $(SCHEDSRC)/scheduler.h
	$(CPP) $(CFLAGS) -o $(BUILD)/test.o $(SRC)/test.cpp

$(BUILD)/sched_utils.o: $(SRC)/sched_utils.cpp $(SRC)/sched_utils.h $(SCHEDSRC)/scheduler.h
	$(CPP) $(CFLAGS) -o $(BUILD)/sched_utils.o $(SRC)/sched_utils.cpp

$(BUILD)/currenttime.o: $(SRC)/currenttime.cpp $(SRC)/currenttime.h
	$(CPP) $(CFLAGS) -o $(BUILD)/currenttime.o $(SRC)/currenttime.cpp

$(BUILD)/logger.o: $(SRC)/logger.cpp $(SRC)/logger.h
	$(CPP) $(CFLAGS) -o $(BUILD)/logger.o $(SRC)/logger.cpp

$(BUILD)/strutils.o: $(SRC)/strutils.c $(SRC)/strutils.h
	$(CC) $(CFLAGS) -o $(BUILD)/strutils.o $(SRC)/strutils.c

$(BUILD)/scheduler.o: $(SCHEDSRC)/scheduler.c $(SCHEDSRC)/scheduler.h
	$(CC) $(CFLAGS) -o $(BUILD)/scheduler.o $(SCHEDSRC)/scheduler.c

###############################################################################
#
# Link it all together into an ELF format file
#
###############################################################################
$(TARGET): $(OBJFILES)
	$(LINKER) -lpthread -o $(TARGET) $(OBJFILES)

clean: 
	rm $(BUILD)/*
	rm $(TARGET)

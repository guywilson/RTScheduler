###############################################################################
#                                                                             #
# MAKEFILE for vbuild tool                                                    #
#                                                                             #
# � Guy Wilson 2019                                                           #
#                                                                             #
###############################################################################

# Source directory
SOURCE=src

# Build output directory
BUILD=build

# What is our target
TARGET=vbuild

# C compiler
C=gcc

# Linker
LINKER=gcc

# C compiler flags (Release)
CFLAGS=-c -Wall

# Object files (in linker ',' seperated format)
OBJFILES=$(BUILD)/vbuild.o

# Target
all: $(TARGET)

# Compile C source files
#
$(BUILD)/vbuild.o: $(SOURCE)/vbuild.c
	$(C) $(CFLAGS) -o $(BUILD)/vbuild.o $(SOURCE)/vbuild.c

$(TARGET): $(OBJFILES)
	$(LINKER) -o $(TARGET) $(OBJFILES)

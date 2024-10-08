# Project Name
TARGET = sardinia

# Sources
CPP_SOURCES = sardinia.cpp

# Library Locations
LIBDAISY_DIR = /Users/dominicfrugoli/Desktop/ProgrammingCode/DaisyCode/DaisyExamples/libDaisy
DAISYSP_DIR = /Users/dominicfrugoli/Desktop/ProgrammingCode/DaisyCode/DaisyExamples/DaisySP

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

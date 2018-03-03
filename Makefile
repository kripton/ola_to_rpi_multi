#############################################################################
# Makefile for building: ola_to_rpi_multi
# Generated by qmake (3.1) (Qt 5.9.4)
# Project:  ola_to_rpi_multi.pro
# Template: app
# Command: /usr/lib64/qt5/bin/qmake -o Makefile ola_to_rpi_multi.pro
#############################################################################

MAKEFILE      = Makefile

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = 
CFLAGS        = -pipe -O2 -pthread -Wall -W -fPIC $(DEFINES)
CXXFLAGS      = -pipe -O2 -std=gnu++11 -pthread -Wall -W -fPIC $(DEFINES)
INCPATH       = -I.
DEL_FILE      = rm -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
COPY          = cp -f
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
TAR           = tar -cf
COMPRESS      = gzip -9f
DISTNAME      = ola_to_rpi_multi1.0.0
LINK          = g++
LFLAGS        = -Wl,-O1
LIBS          = $(SUBLIBS) -lpigpio -lola -lolacommon -lprotobuf -pthread -lpthread 
AR            = ar cqs
RANLIB        = 
SED           = sed
STRIP         = strip

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp \
		OlaToRpiMulti.cpp \
		RpiDmxOutput.cpp 
OBJECTS       = main.o \
		OlaToRpiMulti.o \
		RpiDmxOutput.o

TARGET        = ola_to_rpi_multi


first: all
####### Build rules

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

all: Makefile $(TARGET)

clean: compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core

####### Compile

main.o: main.cpp OlaToRpiMulti.hpp \
		RpiDmxOutput.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

OlaToRpiMulti.o: OlaToRpiMulti.cpp OlaToRpiMulti.hpp \
		RpiDmxOutput.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o OlaToRpiMulti.o OlaToRpiMulti.cpp

RpiDmxOutput.o: RpiDmxOutput.cpp RpiDmxOutput.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o RpiDmxOutput.o RpiDmxOutput.cpp

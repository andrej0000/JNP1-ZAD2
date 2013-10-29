CPPCOMPILER = g++
CPPFLAGS = -std=c++11
CCOMPILER = gcc
CFLAGS = -Wall
EXT = .o

ifeq ($(debuglevel), 1)
	CPPFLAGS += DDEBUG
	EXT = .dbg.o
else
	CFLAGS += -02	
endif

CPP = $(CPPCOMPILER) $(CFLAGS) $(CPPFLAGS)
CC = $(CCOMPILER) $(CFLAGS)

growingnet$(EXT): growingnet.h growingnet.cc network.o
	$(CPP) growingnet.cc -c -o growingnet$(EXT)

network$(EXT): network.h network.cc
	$(CPP) network.cc -c -o network$(EXT)

network_test1: network$(EXT) growingnet$(EXT) network_test1.c
	$(CC) -c network_test1.c -o network_test1.o
	$(CXX) network_test1.o network$(EXT) growingnet$(EXT) -o network_test1

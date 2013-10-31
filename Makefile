CPPCOMPILER = g++
CPPFLAGS = -std=c++11
CCOMPILER = gcc
CFLAGS = -Wall
EXT = .rls.o
NEXT = .dbg.o
.PHONY = clean

ifeq ($(debuglevel), 1)
	CPPFLAGS += -DDEBUG
	EXT = .dbg.o
	NEXT = .rls.dbg
else
	CFLAGS += -O2
endif

CPP = $(CPPCOMPILER) $(CFLAGS) $(CPPFLAGS)
CC = $(CCOMPILER) $(CFLAGS)

ALL: switch network.o growingnet.o

switch:
	echo $(EXT)
	rm -f growingnet.o network.o

growingnet$(EXT): growingnet.h growingnet.cc network$(EXT)
	$(CPP) growingnet.cc -c -o growingnet$(EXT)

network$(EXT): network.h network.cc
	$(CPP) network.cc -c -o network$(EXT)

growingnet.o: growingnet$(EXT) network$(EXT)
	cp growingnet$(EXT) growingnet.o

network.o: network$(EXT)
	cp network$(EXT) network.o

network_test1: network.o growingnet.o network_test1.c
	$(CC) -c network_test1.c -o network_test1.o
	$(CXX) network_test1.o network.o growingnet.o -o network_test1

clean:
	-rm *.o

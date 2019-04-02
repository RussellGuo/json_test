CROSS=~/bin/arm-linux-androideabi-4.7/bin/arm-linux-androideabi-
CC=$(CROSS)gcc
CPLUSPLUS=$(CROSS)g++
INCLUDES=-Iinclude
LIBS=-Llib -L.
CFLAGS=-O2 -s  $(INCLUDES) -std=c99 -Wall
CPPFLAGS=-O2 -s  $(INCLUDES) -std=c++11 -Wall

SRC_FILES = demo.cpp

all: mtts libpcm.so

mtts: demo.cpp libpcm.so
	$(CPLUSPLUS) $(CPPFLAGS) $(LIBS) -o $@ $(SRC_FILES) -lmtts -lsndfile -lpcm -ltinyalsa -ldl


libpcm.so: pcm_play.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $< -shared -ltinyalsa -ldl

clean:
	rm -f mtts *.o libpcm.so

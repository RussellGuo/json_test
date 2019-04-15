CROSS=~/bin/arm-linux-androideabi-4.7/bin/arm-linux-androideabi-
CC=$(CROSS)gcc
CPLUSPLUS=$(CROSS)g++
AR=$(CROSS)ar
INCLUDES=-Iinclude
LIBS=-Llib -L.
CFLAGS=-O2 -s  $(INCLUDES) -std=c99 -Wall
CPPFLAGS=-O2 -s  $(INCLUDES) -std=c++11 -Wall

all: demo demo-pcm libmtts_c.so

libmtts_c.so: mtts_c.cpp
	$(CPLUSPLUS) $(CPPFLAGS) -o $@ mtts_c.cpp -shared $(LIBS) -lmtts -ldl -lsndfile

demo: demo.c libmtts_c.so
	$(CC) $(CFLAGS) -o $@ demo.c $(LIBS) -lmtts_c -lpcm_48k

demo-pcm: demo-pcm.c relay_pcm_play.c
	$(CC) $(CFLAGS) -o $@ $< relay_pcm_play.c $(LIBS) -lpcm_48k

clean:
	rm -f libmtts_c.so *.o demo demo-pcm

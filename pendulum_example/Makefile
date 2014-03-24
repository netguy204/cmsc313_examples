OBJS=

BINS=\
	main.bin

CFLAGS+=-std=c99 -Wall
LDFLAGS+=`sdl2-config --static-libs`

PLATFORM:=$(shell uname)

ifneq ($(PLATFORM), Darwin)
LDFLAGS+=-lGL
endif

%.bin: %.o $(OBJS)
	$(CC) -o $@ $< $(OBJS) $(CFLAGS) $(LDFLAGS)

all: $(BINS)

clean:
	rm -f $(OBJS) $(BINS)

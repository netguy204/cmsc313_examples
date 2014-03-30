OBJS=

BINS=\
	main.bin

BINOBJS=$(patsubst %.bin, %.o, $(BINS))

CFLAGS+=-std=c99 -Wall -g
LDFLAGS+=`sdl2-config --static-libs`

PLATFORM:=$(shell uname)

ifneq ($(PLATFORM), Darwin)
LDFLAGS+=-lGL
endif

all: $(BINS)

%.bin: %.o $(OBJS)
	$(CC) -o $@ $< $(OBJS) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(BINS)

.PRECIOUS: $(OBJS) $(BINOBJS)

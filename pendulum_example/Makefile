OBJS=

BINS=\
	main.bin

CFLAGS+=-std=c99
LDFLAGS+=`sdl2-config --static-libs`

%.bin: %.o $(OBJS)
	$(CC) -o $@ $< $(OBJS) $(CFLAGS) $(LDFLAGS)

all: $(BINS)

clean:
	rm -f $(OBJS) $(BINS)

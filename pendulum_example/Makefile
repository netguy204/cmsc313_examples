OBJS=

BINS=\
	main.bin

CFLAGS+=
LDFLAGS+=`sdl2-config --static-libs`

%.bin: %.o $(OBJS)
	$(CC) -o $@ $< $(OBJS) $(CFLAGS) $(LDFLAGS)

all: $(BINS)

clean:
	rm -f $(OBJS) $(BINS)

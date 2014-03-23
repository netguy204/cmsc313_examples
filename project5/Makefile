OBJS=\
	frac_heap.o \
	frac_ops.o

BINS=\
	test1.bin

BIN_OBJS=$(patsubst %.bin, %.o, $(BINS))

CFLAGS+=-m32 -std=c99
LDFLAGS+=-lm

%.bin: %.o $(OBJS)
	$(CC) -o $@ $< $(OBJS) $(CFLAGS) $(LDFLAGS)

all: $(BINS)

test: test1.bin
	./test.sh

clean:
	rm -f $(OBJS) $(BINS) $(BIN_OBJS) output.txt

.dummy: $(OBJS) $(BIN_OBJS)

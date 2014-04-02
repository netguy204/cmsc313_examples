OBJS=\
	circular.o \
	unittest.o

TEST_SRC:=$(wildcard test*.c)
BINS:=$(patsubst %.c, %.bin, $(TEST_SRC))
BIN_OBJS:=$(patsubst %.bin, %.o, $(BINS))

CFLAGS+=-m32 -std=c99 -g
LDFLAGS+=-lm

all: $(BINS)

%.bin: %.o $(OBJS)
	$(CC) -o $@ $< $(OBJS) $(CFLAGS) $(LDFLAGS)

test: test1.bin test2.bin test3.bin
	./test.sh

clean:
	rm -f $(OBJS) $(BINS) $(BIN_OBJS) *.output

.dummy: $(OBJS) $(BIN_OBJS)

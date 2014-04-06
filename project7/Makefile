OBJS=\
	strrev.o \
	strrepl.o \
	unittest.o

TEST_SRC:=$(wildcard test*.c)
BINS:=$(patsubst %.c, %.bin, $(TEST_SRC))
BIN_OBJS:=$(patsubst %.bin, %.o, $(BINS))

CFLAGS+=-m32 -std=c99 -g
LDFLAGS+=-lm

all: $(BINS)

%.bin: %.o $(OBJS)
	$(CC) -o $@ $< $(OBJS) $(CFLAGS) $(LDFLAGS)

%.o: %.asm
	nasm -f elf -g -F stabs $<

test: test1.bin
	$(foreach tname, $(BINS), ./$(tname) && ) echo "Done"

clean:
	rm -f $(OBJS) $(BINS) $(BIN_OBJS) *.output

.dummy: $(OBJS) $(BIN_OBJS)

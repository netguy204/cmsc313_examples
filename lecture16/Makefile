OBJS=\
	poly.o \
	poly_hack.o

# adjust as appropriate for OSX
PLATFORM:=$(shell uname)
ifeq ($(PLATFORM),Darwin)
CFLAGS=-m32 -g -std=c99 -Wl,-no_pie
NASM=nasm -f macho -g -D "C(x)=x"
else
CFLAGS=-m32 -g -std=c99
NASM=nasm -f elf -g -F stabs -D "\"C(x)=x\""
endif

%.o: %.asm
	$(NASM) $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

poly_test: poly_test.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o poly_test

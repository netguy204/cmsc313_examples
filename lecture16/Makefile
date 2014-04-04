OBJS=\
	poly.o \
	poly_hack.o

CFLAGS=-m32 -g -std=c99
%.o: %.asm
	nasm -f elf -g -F stabs $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

poly_test: poly_test.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o poly_test

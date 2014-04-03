%.o: %.asm
	nasm -f elf -g -F stabs $<

%.o: %.c
	$(CC) -m32 -g -c $<

poly: poly.o poly_hack.o
	$(CC) -m32 -g -o $@ $^

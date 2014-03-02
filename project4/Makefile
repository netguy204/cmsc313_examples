NASM=nasm -f elf -g -F stabs
OBJS=proj4.o library.o
CC?=gcc -m32

proj3: $(OBJS)
	$(LD) -o $@ $(OBJS) -melf_i386

test: proj4
	./test.sh

clean:
	rm -f *.o proj3 output.txt

%.o: %.asm
	$(NASM) $<

%.o: %.c
	$(CC) -g -o $@ -c $<

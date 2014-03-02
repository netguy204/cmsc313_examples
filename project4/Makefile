NASM=nasm -f elf -g -F stabs
OBJS=proj4.o driver.o library.o

proj4: $(OBJS)
	$(LD) -o $@ $(OBJS) -melf_i386

test: proj4
	./test.sh

clean:
	rm -f *.o proj4 output.txt

%.o: %.asm
	$(NASM) $<

%.o: %.c
	$(CC) -m32 -g -o $@ -c $<

NASM=nasm -f elf -g -F stabs
OBJS=proj3.o driver.o

proj3: $(OBJS)
	$(LD) -o $@ $(OBJS) -melf_i386

test: proj3
	./test.sh

clean:
	rm -f *.o proj3 output.txt

%.o: %.asm
	$(NASM) $<

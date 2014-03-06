OBJS = \
	utils.o

BINS = \
	calc.bin \
	infix-to-postfix.bin

%.o: %.c
	$(CC) -std=c99 -g -o $@ -c $<

%.bin: %.o $(OBJS)
	$(CC) -std=c99 -g -o $@ $< $(OBJS)

all: $(BINS)

clean:
	rm -f *.o $(BINS)

.phony: $(OBJS) $(patsubst %.bin, %.o, $(BINS))

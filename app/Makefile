TARG=$(shell gcc -dumpmachine)

SRCS=$(wildcard src/*.c)
OBJS=$(patsubst %.c,%.c.o,$(SRCS))
CFLAGS+=-g -O0
LINK+=-lm
INC=-Ideps/vidi.h/inc

bin/$(TARG):
	mkdir -p $@

%.c.o: %.c bin/$(TARG)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

bin/$(TARG)/tracker: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LINK)
	

all: bin/$(TARG)/tracker
	@echo "Built tracker"


.PHONY: clean
clean: 
	rm src/*.o
	rm -rf bin

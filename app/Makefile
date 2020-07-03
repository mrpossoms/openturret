TARG=$(shell gcc -dumpmachine)

SRCS=$(wildcard src/*.c)
OBJS=$(patsubst %.c,%.c.o,$(SRCS))
# CFLAGS+=-O1
INC=-Ideps/vidi.h/inc

bin/$(TARG):
	mkdir -p $@

%.c.o: %.c bin/$(TARG)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

bin/$(TARG)/tracker: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	

all: bin/$(TARG)/tracker
	@echo "Built tracker"

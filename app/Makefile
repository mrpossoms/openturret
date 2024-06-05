TARG=$(shell ${CC} -dumpmachine)
USER=pi
HOSTNAME=ot

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

flash: bin/$(TARG)/tracker
	scp $< $(USER)@$(HOSTNAME):~

run: flash
	ssh -t $(USER)@$(HOSTNAME) "./tracker /dev/video0 -d"

.PHONY: clean flash run
clean: 
	rm src/*.o
	rm -rf bin

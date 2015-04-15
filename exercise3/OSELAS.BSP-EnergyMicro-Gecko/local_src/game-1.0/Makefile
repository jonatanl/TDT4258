#
# Simple development makefile
#

#
# add more flags here,
# but don't remove the "+="
#
CFLAGS += -O2 -g -Wall -std=c99 #-Wsign-compare -Wfloat-equal -Wformat-security #-Werror
CPPFLAGS +=
LDFLAGS +=


all: game
	
game: input.o logic.o draw.o

*.o: *.c
	arm-cortexm3-uclinuxeabi-gcc -c $@

clean:
	-rm -f game

install:


.PHONY: all install clean

# end of development makefile

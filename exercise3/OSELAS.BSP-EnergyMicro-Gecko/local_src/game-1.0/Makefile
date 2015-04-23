#
# Simple development makefile
#

#
# add more flags here,
# but don't remove the "+="
#
CFLAGS += -O2 -g -Wall -std=c99 #-Wsign-compare -Wfloat-equal -Wformat-security #-Werror
CPPFLAGS +=
LDFLAGS += -lrt


all: game
	
game: draw.o input.o logic.o debug.o spaceship.o util.o geometry.o asteroids.o projectiles.o

%.o: %.c
	arm-cortexm3-uclinuxeabi-gcc $(CFLAGS) -c $< -o $@

clean:
	-rm -f game

install:


.PHONY: all install clean

# end of development makefile

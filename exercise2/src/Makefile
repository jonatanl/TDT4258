# Example Makefile
#
# Exercise 2, TDT4258

CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

CFLAGS=-mcpu=cortex-m3 -mthumb -g -std=c99 -Wall
LDFLAGS=--specs=nosys.specs -mcpu=cortex-m3 -mthumb -g -lgcc -lc -lefm32gg -L../lib
ASFLAGS=-mcpu=cortex-m3 -mthumb -g
LINKERSCRIPT=../lib/efm32gg.ld

ex2.bin : ex2.elf
	${OBJCOPY} -O binary $< $@

ex2.elf : ex2.o dac.o gpio.o interrupt_handlers.o letimer.o arraysound.o synth.o synthSongs.o sleepControl.o effects.o
	${LD} -T ${LINKERSCRIPT} $^ -o $@ ${LDFLAGS} 

%.o : %.c
	${CC} ${CFLAGS} -c $< -o $@

synthSongs.c : noteGenerator/tetris.json
	python noteGenerator/generateSong.py noteGenerator/tetris.json noteGenerator/mario.json

.PHONY : upload
upload :
	-start-eACommander.sh -r --address 0x00000000 -f "ex2.bin" -r

.PHONY : songs
songs :
	python noteGenerator/generateSong.py noteGenerator/tetris.json noteGenerator/mario.json

.PHONY : clean
clean :
	-rm -rf *.o *.elf *.bin *.hex 

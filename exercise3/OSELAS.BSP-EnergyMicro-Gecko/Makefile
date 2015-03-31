# Makefile for exercise 3, TDT4258
#
# Simple usage:
# 	make compile	# compile all sources
# 	make rootfs		# create the rootfs image
# 	make flash		# flash the rootfs image to the efm32gg
.PHONY: compile image flash

compile: \
		platform-energymicro-efm32gg-dk3750/build-target/game/game \
		platform-energymicro-efm32gg-dk3750/build-target/driver-gamepad-1.0/driver-gamepad.o

rootfs: \
		platform-energymicro-efm32gg-dk3750/images/root.romfs
		
flash: rootfs
	ptxdist test flash-rootfs


platform-energymicro-efm32gg-dk3750/build-target/game/game: \
		local_src/game-1.0/game.c
	test ! -f platform-energymicro-efm32gg-dk3750/state/game.compile || ptxdist drop game.compile
	ptxdist compile game

platform-energymicro-efm32gg-dk3750/build-target/driver-gamepad-1.0/driver-gamepad.o: \
		local_src/driver-gamepad-1.0/driver-gamepad.c
	test ! -f platform-energymicro-efm32gg-dk3750/state/driver-gamepad.compile || ptxdist drop driver-gamepad.compile
	ptxdist compile driver-gamepad

platform-energymicro-efm32gg-dk3750/images/root.romfs: \
		platform-energymicro-efm32gg-dk3750/build-target/game/game \
		platform-energymicro-efm32gg-dk3750/build-target/driver-gamepad-1.0/driver-gamepad.o
	ptxdist targetinstall game driver-gamepad
	ptxdist image root.romfs

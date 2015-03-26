# Makefile for exercise 3, TDT4258

game :
	ptxdist clean game
	ptxdist compile game
	ptxdist targetinstall game
	ptxdist image root.romfs
	ptxdist test flash-rootfs

driver-gamepad :
	ptxdist clean driver-gamepad
	ptxdist compile driver-gamepad
	ptxdist targetinstall driver-gamepad
	ptxdist images
	ptxdist test flash-rootfs
	

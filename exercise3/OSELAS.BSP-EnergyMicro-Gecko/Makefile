# Makefile for exercise 3, TDT4258

game :
	ptxdist drop game compile
	ptxdist compile game
	ptxdist targetinstall game
	ptxdist image root.romfs
	ptxdist test flash-rootfs


export QEMU=qemu-system-i386
export SRC_DIR=src
export BUILD_DIR=$(abspath build)
export BOCHS=bochs
export TOOLS_DIR=tools	

export TOOLCHAIN = /home/samar/MYOS/Toolchain/i686-elf
export TOOLCHAIN_BIN = $(TOOLCHAIN)/bin
export TOOLCHAIN_PREFIX = i686-elf

export TARGET_CC = $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-gcc
export TARGET_LD = $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-gcc
export TARGET_CFLAGS = -std=c99 -g
export TARGET_LINKFLAGS =
export TARGET_LIBS =

export TARGET_ASM = nasm
export TARGET_ASMFLAGS = 


export ASMFLAGS = 
export ASM = nasm
export CC = gcc
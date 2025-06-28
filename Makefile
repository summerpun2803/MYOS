ASM=nasm
QEMU=qemu-system-i386
SRC_DIR=src
BUILD_DIR=build
BOCHS=bochs
CC=gcc
TOOLS_DIR=tools


CC16 = /usr/bin/watcom/binl/wcc
LD16 = /usr/bin/watcom/binl/wlink

.PHONY: all floppy_image kernel bootloader clean always debug test

all: floppy_image tools_fat

#
# Floppy image
#
floppy_image: $(BUILD_DIR)/main_floppy.img

$(BUILD_DIR)/main_floppy.img: bootloader kernel
	dd if=/dev/zero of=$(BUILD_DIR)/main_floppy.img bs=512 count=2880 conv=notrunc
	mkfs.fat -F 12 -n "NBOS" $(BUILD_DIR)/main_floppy.img
	dd if=$(BUILD_DIR)/stage1.bin of=$(BUILD_DIR)/main_floppy.img conv=notrunc
	mcopy -i build/main_floppy.img build/stage2.bin ::stage2.bin
	mcopy -i build/main_floppy.img build/kernel.bin ::kernel.bin
	mcopy -i $(BUILD_DIR)/main_floppy.img test.txt "::test.txt"
	mmd -i $(BUILD_DIR)/main_floppy.img "::mydir"
	mcopy -i $(BUILD_DIR)/main_floppy.img test.txt "::mydir/test.txt"


#
# Bootloader
#
bootloader: stage1 stage2

stage1 : always
	$(MAKE) -C $(SRC_DIR)/bootloader/part1 BUILD_DIR=$(abspath $(BUILD_DIR))

stage2 : always
	$(MAKE) -C $(SRC_DIR)/bootloader/part2 BUILD_DIR=$(abspath $(BUILD_DIR))
#
# Kernel
#
kernel: always
	$(MAKE) -C $(SRC_DIR)/kernel

#
# Tools
#
tools_fat: $(BUILD_DIR)/fat
$(BUILD_DIR)/fat: always $(TOOLS_DIR)/fat.c
	mkdir -p $(BUILD_DIR)/tools
	$(CC) -g -o $(BUILD_DIR)/tools/fat $(TOOLS_DIR)/fat.c

#
# Always
#
always:
	mkdir -p $(BUILD_DIR)

#
# Clean
#
clean:
	rm -rf $(BUILD_DIR)/*
	$(MAKE) -C $(SRC_DIR)/bootloader/part2 clean

#
# Debugging and running
#
run:
	${QEMU} -fda ${BUILD_DIR}/main_floppy.img
	
debug:
	${BOCHS} -f bochs_config

test:
	$(BUILD_DIR)/tools/fat $(BUILD_DIR)/main_floppy.img "TEST    TXT"


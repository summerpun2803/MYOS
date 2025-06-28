ASM = nasm
QEMU = qemu-system-i386
BUILD_DIR = build
SRC_DIR = src

image = $(BUILD_DIR)/floppy.img

$(BUILD_DIR)/floppy.img: $(BUILD_DIR)/boot.bin always
	cp $(BUILD_DIR)/boot.bin $(BUILD_DIR)/floppy.img
	truncate -s 1440k $(BUILD_DIR)/floppy.img

$(BUILD_DIR)/boot.bin: $(SRC_DIR)/main.asm always
	$(ASM) $(SRC_DIR)/main.asm -f bin -o $(BUILD_DIR)/boot.bin 

run:
	$(QEMU) -fda $(BUILD_DIR)/floppy.img

always:
	mkdir -p $(BUILD_DIR)

debug:
	bochs -f bochs_config

.PHONY: image run
ASM ?= nasm
BUILD_DIR ?= ../../../build
SRC = boot.asm
OUTPUT = $(BUILD_DIR)/stage1.bin  # or stage2.bin in part2

.PHONY: all clean

all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(ASM) -f bin $(SRC) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)

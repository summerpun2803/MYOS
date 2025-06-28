ASM ?= nasm
BUILD_DIR ?= ../../build
SRC = kernel.asm
OUTPUT = $(BUILD_DIR)/kernel.bin 

.PHONY: all clean

all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(ASM) $(SRC) -f bin -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)

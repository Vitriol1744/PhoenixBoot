BOOTLOADER_BIN := PhoenixBoot.bin

SRC_DIR := ./src
BUILD_DIR := ./build

CXX := clang++
AS := nasm
LD := ld

CXX_FLAGS := -std=c++20 -m32 -Wall -Wextra -Wpedantic -ffreestanding -nostdlib -fno-pic -fno-stack-protector -mno-red-zone -masm=intel -mno-sse -mno-sse2 -mno-mmx -mno-80387 -fno-exceptions -fno-rtti
AS_FLAGS := -felf32
LD_FLAGS := -nostdlib -no-pie -melf_i386 -s -T linker.ld

SRCS := $(shell find $(SRC_DIR)/stage2 -name '*.cpp' -or -name '*.asm')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/$(BOOTLOADER_BIN): $(BUILD_DIR)/stage2.bin
	mkdir -p $(dir $@)
	$(AS) -i$(BUILD_DIR) -i$(SRC_DIR) $(SRC_DIR)/stage1.asm -o $@

$(BUILD_DIR)/stage2.bin: $(OBJS) linker.ld
	$(LD) $(OBJS) -o $@ $(LD_FLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(BUILD_DIR)/%.asm.o: %.asm
	mkdir -p $(dir $@)
	$(AS) $(AS_FLAGS) $< -o $@

rebuild:
	$(MAKE) clean
	$(MAKE)

qemu: $(BUILD_DIR)/$(BOOTLOADER_BIN)
	qemu-system-x86_64 -drive format=raw,file=$<

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
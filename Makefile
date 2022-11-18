BOOTLOADER_BIN := PhoenixBoot.bin

SRC_DIR := ./src
BUILD_DIR := ./build

CXX := clang++
AS := nasm
LD := ld

CXX_DEFINES := PH_DEBUG PH_ARCH=PH_ARCH_X86_64
CXX_DEF_FLAGS := $(addprefix -D,$(CXX_DEFINES))

CXX_INC_DIRS := src/stage2
CXX_INC_FLAGS := $(addprefix -I,$(CXX_INC_DIRS))
AS_INC_DIRS := src/stage2/Arch/x86
AS_INC_FLAGS := $(addprefix -i,$(AS_INC_DIRS))

CXX_WARN_LEVELS := all extra pedantic
CXX_WARN_FLAGS := $(addprefix -W,$(CXX_WARN_LEVELS))

INTERNAL_CXX_FLAGS := -std=c++20 -ffreestanding -nostdlib -fno-pic -fno-stack-protector -mno-red-zone -mno-sse -mno-sse2 -mno-mmx -mno-80387 -fno-exceptions -fno-rtti
CXX_FLAGS := $(CXX_DEF_FLAGS) $(CXX_INC_FLAGS) $(CXX_WARN_FLAGS) $(INTERNAL_CXX_FLAGS) -mabi=sysv -march=x86-64 -target x86_64-none -m32 -masm=intel
AS_FLAGS := $(AS_INC_FLAGS) -felf32
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
	qemu-system-x86_64 $(QEMU_FLAGS) -drive format=raw,file=$<

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
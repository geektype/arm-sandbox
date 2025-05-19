AS=arm-none-eabi-as
CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
GDB=arm-none-eabi-gdb

ASFLAGS = -g
CFLAGS = -g -c -ffreestanding -std=c11 -O0
LDFLAGS = -T $(SOURCE_DIR)/link.ld

QEMU_FLAGS = -M virt -m 128M -kernel $(TARGET) -nographic -S -s

SOURCE_DIR = src
BUILD_DIR=build

TARGET=$(BUILD_DIR)/main.elf


OBJ_DIR=$(BUILD_DIR)/obj

SRCS := $(shell find $(SOURCE_DIR) -type f -name '*.c')
ASRCS := $(shell find $(SOURCE_DIR) -type f -name '*.s')

HEADER_DIRS := $(shell find $(SOURCE_DIR) -type f -name '*.h' -exec dirname {} \; | sort -u)
INCLUDE_FLAGS := $(patsubst %, -I%, $(HEADER_DIRS))

OBJS := $(SRCS:$(SOURCE_DIR)/%.c=$(OBJ_DIR)/%.o) $(ASRCS:$(SOURCE_DIR)/%.s=$(OBJ_DIR)/%.o)


$(TARGET): $(OBJS)
	@$(LD) $(LDFLAGS) -o $@ $^


$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.s | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $^

$(OBJ_DIR):
	mkdir -p $@

.PHONY: build run debug gen-commands clean

build: $(TARGET)

run: $(TARGET)
	qemu-system-arm $(QEMU_FLAGS)
debug: $(TARGET)
	$(GDB) $(TARGET) -ex "a"
gen-commands:
	make -Bnw | python3 scripts/gen_compile_commands.py	

clean:
	rm -fr $(BUILD_DIR)


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
ARCHIVE=$(BUILD_DIR)/$(LIB_NAME).a

ASRCS := $(wildcard $(SOURCE_DIR)/*.s)
CSRCS := $(wildcard $(SOURCE_DIR)/*.c)

OBJS := $(addprefix $(OBJ_DIR)/, $(notdir $(ASRCS:.s=.o) $(CSRCS:.c=.o)))

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^


$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.s | $(OBJ_DIR)
	$(AS) $(ASFLAGS) -o $@ $^

$(OBJ_DIR):
	mkdir -p $@

.PHONY: build run debug clean

build: $(TARGET)

run: $(TARGET)
	qemu-system-arm $(QEMU_FLAGS)
debug: $(TARGET)
	$(GDB) $(TARGET) -ex "target remote localhost:1234"

clean:
	rm -fr $(BUILD_DIR)


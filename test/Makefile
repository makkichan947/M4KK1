# M4KK1 Dynamic Linker Test Makefile

CC = gcc
CFLAGS = -Wall -Wextra -I../sys/src/include -nostdlib -ffreestanding -m32
LDFLAGS = -Ttext 0x100000

# 源文件
SRCS = ldso_test.c

# 目标文件
OBJS = $(SRCS:.c=.o)
TARGET = ldso_test

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

# 运行测试（需要QEMU或其他模拟器）
test: $(TARGET)
	@echo "Test program built successfully"
	@echo "To run the test, you need to:"
	@echo "1. Build the kernel with dynamic linker support"
	@echo "2. Run the kernel in QEMU or another emulator"
	@echo "3. Load this test program as a user-space application"
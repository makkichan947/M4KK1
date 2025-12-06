# M4KK1 MLang Compiler Makefile
# 构建MLang独特编译器

# 编译器和标志
CC := gcc
CFLAGS := -Wall -Wextra -O2 -g -Iinclude -I../include -std=c99
LDFLAGS := -lm

# 源代码文件
mlang_src := $(wildcard src/*.c src/*/*.c)

# 目标文件名
MLANG := mlang
MLANG_OPT := mlang-opt
MLANG_CODEGEN := mlang-codegen

# MLang编译器对象文件
mlang_y := $(patsubst %.c,%.o,$(mlang_src))

# 默认目标
.PHONY: all
all: $(MLANG)

# 构建主编译器
$(MLANG): src/simple.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# 清理目标
.PHONY: clean
clean:
	rm -f $(mlang_y) $(MLANG) $(MLANG_OPT) $(MLANG_CODEGEN)

# 安装目标
.PHONY: install
install: all
	@echo "安装MLang编译器..."
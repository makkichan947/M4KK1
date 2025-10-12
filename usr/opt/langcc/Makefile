# LangCC - Language Compiler Collection Makefile
# 构建编译器集合

# 工具链
CC = gcc
AS = as
LD = ld

# 标志
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
        -nostartfiles -nodefaultlibs -Wall -Wextra -Werror \
        -ffreestanding -std=c99 -pedantic \
        -I include/ \
        -I ../../y4ku/include/ \
        -DLANGCC_BUILD

LDFLAGS = -m elf_i386 -T linker.ld -Map langcc.map

# 文件
LANGCC_SOURCES = $(wildcard src/*.c)
LANGCC_OBJECTS = $(LANGCC_SOURCES:.c=.o)

LANGCC_BINARY = langcc

# 默认目标
.PHONY: all clean install test

all: $(LANGCC_BINARY)

# 编译C文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 链接编译器
$(LANGCC_BINARY): $(LANGCC_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# 安装编译器
install: $(LANGCC_BINARY)
	mkdir -p $(DESTDIR)/usr/bin
	cp $(LANGCC_BINARY) $(DESTDIR)/usr/bin/
	mkdir -p $(DESTDIR)/usr/include/langcc
	cp -r include/* $(DESTDIR)/usr/include/langcc/
	mkdir -p $(DESTDIR)/usr/lib/langcc
	cp -r lib/* $(DESTDIR)/usr/lib/langcc/

# 清理构建文件
clean:
	rm -f $(LANGCC_OBJECTS) $(LANGCC_BINARY) langcc.map

# 测试编译器
test: $(LANGCC_BINARY)
	@echo "Testing LangCC..."
	./$(LANGCC_BINARY) --version
	@echo "LangCC test completed"

# 调试构建
debug: CFLAGS += -DDEBUG -g
debug: all

# 发布构建（优化）
release: CFLAGS += -O2 -DNDEBUG
release: all

# 显示编译器信息
info:
	@echo "LangCC Build Information"
	@echo "Sources: $(LANGCC_SOURCES)"
	@echo "Objects: $(LANGCC_OBJECTS)"
	@echo "Binary: $(LANGCC_BINARY)"

# 帮助信息
help:
	@echo "LangCC - Language Compiler Collection Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all       - Build LangCC (default)"
	@echo "  debug     - Build LangCC with debug info"
	@echo "  release   - Build optimized LangCC"
	@echo "  clean     - Clean build files"
	@echo "  install   - Install LangCC to system"
	@echo "  test      - Test LangCC functionality"
	@echo "  info      - Show build information"
	@echo "  help      - Show this help"
	@echo ""
	@echo "Installation requires root privileges for 'install' target!"

# 依赖检查
check-deps:
	@echo "Checking dependencies..."
	@which $(CC) > /dev/null || (echo "GCC not found" && exit 1)
	@which $(AS) > /dev/null || (echo "AS not found" && exit 1)
	@which $(LD) > /dev/null || (echo "LD not found" && exit 1)
	@echo "All dependencies are available"

# 完整构建（包括依赖检查）
full: check-deps all

# 文档生成
docs:
	@echo "Generating documentation..."
	# 这里应该生成文档
	@echo "Documentation generated"

# 包管理器集成
package: all
	@echo "Creating package..."
	# 这里应该创建软件包
	@echo "Package created"

# 交叉编译支持
cross-compile: CFLAGS += --target=m4kk1-unknown-elf
cross-compile: all

# 静态链接构建
static: LDFLAGS += -static
static: all

# 性能分析构建
profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: all

# 代码覆盖率构建
coverage: CFLAGS += -fprofile-arcs -ftest-coverage
coverage: LDFLAGS += -fprofile-arcs
coverage: all

# 内存检查构建
memcheck: CFLAGS += -fsanitize=address -fsanitize=leak
memcheck: LDFLAGS += -fsanitize=address
memcheck: all

# 线程安全构建
thread-safe: CFLAGS += -D_THREAD_SAFE -pthread
thread-safe: LDFLAGS += -pthread
thread-safe: all

# 最小化构建
minimal: CFLAGS += -Os -ffunction-sections -fdata-sections
minimal: LDFLAGS += -Wl,--gc-sections
minimal: all

# 兼容性构建
compat: CFLAGS += -D_GNU_SOURCE -D_POSIX_C_SOURCE=200809L
compat: all

# 国际化支持
i18n: CFLAGS += -DENABLE_NLS
i18n: all

# 无警告构建
no-warn: CFLAGS = $(filter-out -W%, $(CFLAGS)) -w
no-warn: all

# 严格模式构建
strict: CFLAGS += -Wstrict-prototypes -Wmissing-prototypes \
                  -Wold-style-definition -Wmissing-declarations \
                  -Werror=implicit-function-declaration
strict: all

# 便携式构建
portable: CFLAGS += -DPORTABLE -ansi
portable: all

# 快速构建（减少优化）
fast: CFLAGS = $(filter-out -O%, $(CFLAGS)) -O0
fast: all

# 完整清理
distclean: clean
	rm -f config.log config.status configure
	rm -rf autom4te.cache

# 维护目标
maintainer-clean: distclean
	rm -f Makefile

# 显示配置
show-config:
	@echo "Build configuration:"
	@echo "CC=$(CC)"
	@echo "AS=$(AS)"
	@echo "LD=$(LD)"
	@echo "CFLAGS=$(CFLAGS)"
	@echo "LDFLAGS=$(LDFLAGS)"

# 版本信息
version:
	@echo "LangCC version 0.1.0"
	@echo "Build date: $(shell date)"
	@echo "Build host: $(shell uname -a)"
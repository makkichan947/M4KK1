# M4KK1 Copland窗口服务器Makefile
# 构建Wayland风格的窗口服务器

# 编译器和编译选项
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2 -g -Iinclude -I../include
CXXFLAGS = -Wall -Wextra -O2 -g -std=c++17 -Iinclude -I../include
LDFLAGS = -lpthread -lm -lrt

# 源代码文件
C_SRCS = src/main.c src/server.c src/client.c src/test.c
CXX_SRCS =
ASM_SRCS =

# 目标文件
C_OBJS = $(C_SRCS:.c=.o)
CXX_OBJS = $(CXX_SRCS:.cpp=.o)
ASM_OBJS = $(ASM_SRCS:.asm=.o)
OBJS = $(C_OBJS) $(CXX_OBJS) $(ASM_OBJS)

# 可执行文件
TARGET = copland
TEST_TARGET = copland-test

# 安装路径
PREFIX = /usr
BINDIR = $(PREFIX)/bin
DATADIR = $(PREFIX)/share/copland
CONFDIR = $(PREFIX)/etc/copland

# 版本信息
VERSION = 1.0.0
RELEASE = 1

# 默认目标
.PHONY: all clean install uninstall dist test

all: $(TARGET)

test: $(TEST_TARGET)

# 链接目标文件
$(TARGET): src/main.o src/server.o src/client.o
	$(CC) src/main.o src/server.o src/client.o -o $(TARGET) $(LDFLAGS)

# 链接测试目标文件
$(TEST_TARGET): src/test.o src/client.o src/server.o
	$(CC) src/test.o src/client.o src/server.o -o $(TEST_TARGET) $(LDFLAGS)

# 编译C源文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 编译C++源文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译汇编文件
%.o: %.asm
	nasm -f elf64 $< -o $@

# 清理构建文件
clean:
	rm -f $(OBJS) $(TARGET)
	rm -f *.tar.gz *.rpm *.deb

# 安装到系统
install: $(TARGET)
	# 创建安装目录
	install -d $(BINDIR)
	install -d $(DATADIR)/protocols
	install -d $(DATADIR)/clients
	install -d $(CONFDIR)

	# 安装可执行文件
	install -m 755 $(TARGET) $(BINDIR)/

	# 安装配置文件
	install -m 644 config/copland.conf $(CONFDIR)/ 2>/dev/null || true
	install -m 644 protocols/* $(DATADIR)/protocols/ 2>/dev/null || true
	install -m 755 clients/* $(DATADIR)/clients/ 2>/dev/null || true

	# 安装文档
	install -d $(PREFIX)/share/doc/copland
	install -m 644 README.md $(PREFIX)/share/doc/copland/ 2>/dev/null || true
	install -m 644 docs/design.md $(PREFIX)/share/doc/copland/ 2>/dev/null || true

	# 创建符号链接
	ln -sf copland $(BINDIR)/wayland-server 2>/dev/null || true

	@echo "Copland窗口服务器已安装到 $(BINDIR)/copland"

# 卸载
uninstall:
	rm -f $(BINDIR)/copland
	rm -f $(BINDIR)/wayland-server
	rm -rf $(DATADIR)
	rm -rf $(CONFDIR)
	rm -rf $(PREFIX)/share/doc/copland
	@echo "Copland窗口服务器已卸载"

# 创建分发包
dist: clean
	tar -czf copland-$(VERSION).tar.gz \
		Makefile \
		README.md \
		src/ \
		include/ \
		docs/ \
		config/ \
		protocols/ \
		clients/

# RPM包构建
rpm: dist
	rpmbuild -ta copland-$(VERSION).tar.gz

# DEB包构建
deb: dist
	dpkg-buildpackage -us -uc

# 开发模式构建
dev: CFLAGS += -DDEBUG -g3 -O0
dev: all

# 发布模式构建
release: CFLAGS += -DNDEBUG -O3
release: all

# 静态链接构建
static: LDFLAGS += -static
static: all

# 交叉编译
cross: CC = x86_64-m4kk1-linux-gcc
cross: all

# 依赖检查
depend:
	makedepend -Y -Iinclude $(C_SRCS) $(CXX_SRCS)

# 显示帮助
help:
	@echo "可用目标:"
	@echo "  all       - 构建窗口服务器 (默认)"
	@echo "  test      - 构建测试程序"
	@echo "  clean     - 清理构建文件"
	@echo "  install   - 安装到系统"
	@echo "  uninstall - 从系统卸载"
	@echo "  dist      - 创建分发包"
	@echo "  dev       - 开发模式构建"
	@echo "  release   - 发布模式构建"
	@echo "  static    - 静态链接构建"
	@echo "  cross     - 交叉编译"
	@echo "  depend    - 生成依赖信息"
	@echo "  help      - 显示此帮助"

# 包含依赖文件
-include .depend
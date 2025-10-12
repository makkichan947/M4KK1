# M4KK1 M4man手册系统Makefile
# 构建TUI界面的手册浏览器

# 编译器和编译选项
CC = langcc
CXX = langcc++
CFLAGS = -Wall -Wextra -O2 -g -Iinclude -I../include
CXXFLAGS = -Wall -Wextra -O2 -g -std=c++17 -Iinclude -I../include
LDFLAGS = -lncurses -lpthread -lm

# 源代码文件
C_SRCS = src/main.c src/ui.c src/document.c src/parser.c src/renderer.c 
		\src/search.c src/bookmark.c src/history.c src/config.c src/utils.c
CXX_SRCS =
ASM_SRCS =

# 目标文件
C_OBJS = $(C_SRCS:.c=.o)
CXX_OBJS = $(CXX_SRCS:.cpp=.o)
ASM_OBJS = $(ASM_SRCS:.asm=.o)
OBJS = $(C_OBJS) $(CXX_OBJS) $(ASM_OBJS)

# 可执行文件
TARGET = m4man

# 安装路径
PREFIX = /usr
BINDIR = $(PREFIX)/bin
DATADIR = $(PREFIX)/share/m4man
MANDIR = $(PREFIX)/share/man

# 版本信息
VERSION = 1.0.0
RELEASE = 1

# 默认目标
.PHONY: all clean install uninstall dist

all: $(TARGET)

# 链接目标文件
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

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
	install -d $(DATADIR)/themes
	install -d $(DATADIR)/parsers
	install -d $(MANDIR)/man1

	# 安装可执行文件
	install -m 755 $(TARGET) $(BINDIR)/

	# 安装配置文件
	install -m 644 config/m4man.conf $(DATADIR)/ 2>/dev/null || true
	install -m 644 themes/* $(DATADIR)/themes/ 2>/dev/null || true
	install -m 755 parsers/* $(DATADIR)/parsers/ 2>/dev/null || true

	# 安装手册页
	install -m 644 docs/m4man.1 $(MANDIR)/man1/ 2>/dev/null || true

	# 安装文档
	install -d $(PREFIX)/share/doc/m4man
	install -m 644 README.md $(PREFIX)/share/doc/m4man/ 2>/dev/null || true
	install -m 644 docs/design.md $(PREFIX)/share/doc/m4man/ 2>/dev/null || true

	# 创建符号链接
	ln -sf m4man $(BINDIR)/man 2>/dev/null || true

	@echo "M4man手册浏览器已安装到 $(BINDIR)/m4man"

# 卸载
uninstall:
	rm -f $(BINDIR)/m4man
	rm -f $(BINDIR)/man
	rm -rf $(DATADIR)
	rm -f $(MANDIR)/man1/m4man.1
	rm -rf $(PREFIX)/share/doc/m4man
	@echo "M4man手册浏览器已卸载"

# 创建分发包
dist: clean
	tar -czf m4man-$(VERSION).tar.gz \
		Makefile \
		README.md \
		src/ \
		include/ \
		docs/ \
		config/ \
		themes/ \
		parsers/

# RPM包构建
rpm: dist
	rpmbuild -ta m4man-$(VERSION).tar.gz

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
	@echo "  all       - 构建手册浏览器 (默认)"
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
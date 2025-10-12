# M4KK1 Rubidium桌面环境Makefile
# 构建平铺式桌面环境

# 编译器和编译选项
CC = langcc
CXX = langcc++
CFLAGS = -Wall -Wextra -O2 -g -Iinclude -I../include
CXXFLAGS = -Wall -Wextra -O2 -g -std=c++17 -Iinclude -I../include
LDFLAGS = -lwayland-client -lxkbcommon -lpthread -lm

# 源代码文件
C_SRCS = src/main.c src/desktop.c src/window_manager.c src/workspace.c \
         src/layout_engine.c src/animation_engine.c src/input_manager.c \
         src/renderer.c src/config.c src/theme.c src/keybindings.c \
         src/window_rules.c src/utils.c
CXX_SRCS =
ASM_SRCS =

# 目标文件
C_OBJS = $(C_SRCS:.c=.o)
CXX_OBJS = $(CXX_SRCS:.cpp=.o)
ASM_OBJS = $(ASM_SRCS:.asm=.o)
OBJS = $(C_OBJS) $(CXX_OBJS) $(ASM_OBJS)

# 可执行文件
TARGET = rubidium

# 安装路径
PREFIX = /usr
BINDIR = $(PREFIX)/bin
DATADIR = $(PREFIX)/share/rubidium
CONFDIR = $(PREFIX)/etc/rubidium

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
	install -d $(DATADIR)/layouts
	install -d $(DATADIR)/plugins
	install -d $(CONFDIR)

	# 安装可执行文件
	install -m 755 $(TARGET) $(BINDIR)/

	# 安装配置文件
	install -m 644 config/rubidium.conf $(CONFDIR)/ 2>/dev/null || true
	install -m 644 themes/* $(DATADIR)/themes/ 2>/dev/null || true
	install -m 644 layouts/* $(DATADIR)/layouts/ 2>/dev/null || true
	install -m 755 plugins/* $(DATADIR)/plugins/ 2>/dev/null || true

	# 安装桌面文件
	install -d $(PREFIX)/share/xsessions
	install -m 644 desktop/rubidium.desktop $(PREFIX)/share/xsessions/ 2>/dev/null || true

	# 安装文档
	install -d $(PREFIX)/share/doc/rubidium
	install -m 644 README.md $(PREFIX)/share/doc/rubidium/ 2>/dev/null || true
	install -m 644 docs/design.md $(PREFIX)/share/doc/rubidium/ 2>/dev/null || true

	# 创建符号链接
	ln -sf rubidium $(BINDIR)/rubidium-wm 2>/dev/null || true

	@echo "Rubidium桌面环境已安装到 $(BINDIR)/rubidium"

# 卸载
uninstall:
	rm -f $(BINDIR)/rubidium
	rm -f $(BINDIR)/rubidium-wm
	rm -rf $(DATADIR)
	rm -rf $(CONFDIR)
	rm -f $(PREFIX)/share/xsessions/rubidium.desktop
	rm -rf $(PREFIX)/share/doc/rubidium
	@echo "Rubidium桌面环境已卸载"

# 创建分发包
dist: clean
	tar -czf rubidium-$(VERSION).tar.gz \
		Makefile \
		README.md \
		src/ \
		include/ \
		docs/ \
		config/ \
		themes/ \
		layouts/ \
		plugins/ \
		desktop/

# RPM包构建
rpm: dist
	rpmbuild -ta rubidium-$(VERSION).tar.gz

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
	@echo "  all       - 构建桌面环境 (默认)"
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
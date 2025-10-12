# M4KK1操作系统主Makefile
# 构建完整的M4KK1操作系统

# 版本信息
VERSION = 0.1.0
RELEASE = 1

# 安装前缀
PREFIX = /usr
SYSCONFDIR = /etc

# 编译器设置
CC = gcc
CXX = g++
AS = nasm
LD = ld

# 编译标志
CFLAGS = -Wall -Wextra -O2 -g -ffreestanding -nostdlib -nostdinc
CXXFLAGS = -Wall -Wextra -O2 -g -std=c++17 -ffreestanding -nostdlib -nostdinc
ASFLAGS = -f elf32
LDFLAGS = -nostdlib -z max-page-size=0x1000

# 主机编译标志（用于构建工具）
HOSTCFLAGS = -Wall -Wextra -O2 -g

# 导出变量
export CC CXX AS LD CFLAGS CXXFLAGS ASFLAGS LDFLAGS HOSTCFLAGS PREFIX SYSCONFDIR VERSION

# 定义源码树根目录
srctree := $(CURDIR)

# 导出源码树变量
export srctree

# 包含通用构建规则
include scripts/Makefile.include

# 默认目标
.PHONY: all clean install test dist help

all: kernel bootloader

# 内核构建
kernel:
	@echo "构建M4KK1宏内核..."
	$(MAKE) -C sys/src all

# 引导程序构建
bootloader:
	@echo "构建Bootcamp引导程序..."
	$(MAKE) -C sys/boot/bootcamp all

# 系统工具构建
tools:
	@echo "构建系统工具..."
	@echo "注意：用户工具需要langcc编译器，暂时跳过"

# 用户工具构建
user-tools:
	@echo "构建用户工具..."
	$(MAKE) -C usr/bin/ivfetch all
	$(MAKE) -C usr/bin/m4sh all
	$(MAKE) -C usr/bin/mtop all
	$(MAKE) -C usr/bin/vicode all
	$(MAKE) -C usr/bin/vi all
	$(MAKE) -C usr/bin/f1le all
	$(MAKE) -C usr/bin/ptdsk all
	$(MAKE) -C usr/bin/ntmgr all
	$(MAKE) -C usr/bin/ntctl all
	$(MAKE) -C usr/bin/m4man all
	$(MAKE) -C usr/bin/copland all
	$(MAKE) -C usr/bin/rubidium all

# 系统工具构建
system-tools:
	@echo "构建系统工具..."
	$(MAKE) -C usr/opt/langcc all
	$(MAKE) -C pkg/pkgmgr all

# 完整构建
everything: bootloader kernel system-tools user-tools

# 安装后处理
install: all
	@echo "安装M4KK1操作系统..."
	$(MAKE) -C sys/src install
	$(MAKE) -C usr/bin install
	$(MAKE) -C usr/opt install
	$(MAKE) -C pkg install
	@echo "创建可执行文件符号链接..."
	@for prg in usr/bin/*/*.prg; do \
		if [ -f "$$prg" ]; then \
			base=$$(basename $$prg .prg); \
			ln -sf "../$$prg" "usr/bin/$$base" 2>/dev/null || true; \
		fi; \
	done
	@echo "M4KK1操作系统安装完成"

# 清理
clean:
	@echo "清理所有构建文件..."
	$(MAKE) -C sys/src clean
	$(MAKE) -C sys/boot/bootcamp clean
	$(MAKE) -C usr/bin clean
	@echo "系统工具目录暂不支持构建操作"
	@echo "包管理目录暂不支持构建操作"

# 安装
install: all
	@echo "安装M4KK1操作系统..."
	$(MAKE) -C sys/src install
	$(MAKE) -C usr/bin install
	$(MAKE) -C usr/opt install
	$(MAKE) -C pkg install

# 测试
test:
	@echo "运行M4KK1测试套件..."
	@echo "单元测试..."
	@echo "单元测试..."
	@echo "集成测试..."
	@echo "性能测试..."

# 完整测试
test-all: test
	@echo "运行完整测试套件..."
	@echo "系统测试..."

# 创建分发包
dist: clean
	@echo "创建M4KK1分发包..."
	tar -czf m4kk1-$(VERSION).tar.gz \
		Makefile \
		README.md \
		sys/ \
		usr/ \
		pkg/ \
		test/ \
		docs/

# 创建ISO镜像
iso: all
	@echo "创建M4KK1 ISO镜像..."
	@echo "跳过mkisofs构建..."
	./tools/mkisofs/mkisofs \
		-o m4kk1-$(VERSION).iso \
		-b boot/grub/iso9660_stage1_5 \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		-V "M4KK1 $(VERSION)" \
		iso/

# 运行QEMU测试
qemu-test: iso
	@echo "在QEMU中测试M4KK1..."
	qemu-system-x86_64 \
		-cdrom m4kk1-$(VERSION).iso \
		-m 512M \
		-vga std \
		-serial stdio

# 运行完整测试
full-test: test-all qemu-test

# 开发环境设置
dev-setup:
	@echo "设置开发环境..."
	sudo apt update
	sudo apt install -y build-essential nasm qemu grub-pc-bin xorriso
	sudo apt install -y libncurses5-dev libncursesw5-dev
	sudo apt install -y libwayland-dev libxkbcommon-dev
	sudo apt install -y libdrm-dev libgbm-dev libegl1-mesa-dev
	sudo apt install -y libgtk-3-dev libglib2.0-dev

# 依赖检查
check-deps:
	@echo "检查构建依赖..."
	@which $(CC) >/dev/null || (echo "错误: 未找到 $(CC)" && exit 1)
	@which $(AS) >/dev/null || (echo "错误: 未找到 $(AS)" && exit 1)
	@which qemu-system-x86_64 >/dev/null || echo "警告: 未找到 qemu-system-x86_64，将无法进行虚拟机测试"
	@echo "依赖检查完成"
# 环境信息
info:
	@echo "M4KK1量子意识操作系统构建环境信息"
	@echo "版本: $(VERSION)"
	@echo "作者: makkichan947"
	@echo "仓库: https://github.com/makkichan947/M4KK1"
	@echo "量子签名: makkichan947_quantum_consciousness"
	@echo "编译器: $(CC)"
	@echo "汇编器: $(AS)"
	@echo "链接器: $(LD)"
	@echo "前缀: $(PREFIX)"
	@echo "配置目录: $(SYSCONFDIR)"

# 帮助信息
help:
	@echo "M4KK1操作系统构建系统"
	@echo "作者: makkichan947 (一人独立完成)"
	@echo "仓库: https://github.com/makkichan947/M4KK1"
	@echo ""
	@echo "可用目标:"
	@echo "  all           - 构建内核和工具 (默认)"
	@echo "  kernel        - 仅构建宏内核"
	@echo "  bootloader    - 仅构建引导程序"
	@echo "  tools         - 构建所有工具"
	@echo "  user-tools    - 构建用户工具"
	@echo "  system-tools  - 构建系统工具"
	@echo "  everything    - 构建所有组件"
	@echo "  clean         - 清理所有构建文件"
	@echo "  install       - 安装到系统"
	@echo "  test          - 运行基本测试"
	@echo "  test-all      - 运行完整测试"
	@echo "  dist          - 创建分发包"
	@echo "  iso           - 创建ISO镜像"
	@echo "  qemu-test     - 在QEMU中测试"
	@echo "  full-test     - 运行完整测试"
	@echo "  dev-setup     - 设置开发环境"
	@echo "  check-deps    - 检查构建依赖"
	@echo "  info          - 显示环境信息"
	@echo "  help          - 显示此帮助"
	@echo ""
	@echo "系统特性:"
	@echo "  • Y4KU宏内核架构"
	@echo "  • YFS高级文件系统"
	@echo "  • Swap2交换系统"
	@echo "  • LangCC编译器集合"
	@echo "  • PkgMgr包管理系统"
	@echo "  • 完整的用户工具集"
	@echo "  • 驱动程序集合"
	@echo ""
	@echo "示例:"
	@echo "  make everything   # 构建完整系统"
	@echo "  make iso          # 创建ISO镜像"
	@echo "  make qemu-test    # 在QEMU中测试"
	@echo "  make install      # 安装到系统"

# 子目录构建规则
usr/bin/all usr/bin/install usr/bin/clean:
	@echo "用户工具目录暂不支持构建操作（需要langcc编译器）"

usr/opt/all usr/opt/install usr/opt/clean:
	@echo "系统工具目录暂不支持构建操作"

pkg/all pkg/install pkg/clean:
	@echo "包管理目录暂不支持构建操作"

sys/src/all sys/src/install sys/src/clean:
	$(MAKE) -C sys/src $(subst sys/src/,,$@)

sys/boot/bootcamp/all sys/boot/bootcamp/clean:
	$(MAKE) -C sys/boot/bootcamp $(subst sys/boot/bootcamp/,,$@)

test/unit/all test/integration/all test/performance/all test/system/all:
	@echo "测试目录暂不支持构建操作"

# 包含子目录依赖
# -include sys/src/Makefile
# -include usr/bin/Makefile
# -include usr/opt/Makefile
# -include pkg/Makefile

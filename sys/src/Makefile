# M4KK1 Kernel Makefile
# 主内核构建脚本，参考Linux内核构建系统

# 导出所有变量
export

# 包含主配置
# include scripts/Kconfig

# 包含通用构建规则
include scripts/Makefile.include

# 默认目标
.PHONY: all clean install help menuconfig
all: kernel

# 内核构建目标
kernel: scripts/mkversion
	@echo "构建M4KK1宏内核..."
	$(MAKE) -C drivers all
	$(MAKE) -C arch/m4kk1 all
	$(MAKE) -C mm all
	$(MAKE) -C fs all
	$(MAKE) -C lib all
	$(MAKE) -C init all
	$(MAKE) -C kernel all

# 链接最终内核
	@echo "链接最终内核..."
	$(LD) $(LDFLAGS) -o m4kk1.krn \
		init/entry.o \
		arch/m4kk1/*.o \
		drivers/drivers.a \
		drivers/keyboard/keyboard.a \
		drivers/mouse/mouse.a \
		mm/mm.a \
		fs/fs.a \
		kernel/*.o \
		lib/lib.a

# 清理目标
clean:
	@echo "清理内核构建文件..."
	$(MAKE) -C kernel clean
	$(MAKE) -C arch/m4kk1 clean
	$(MAKE) -C drivers clean
	$(MAKE) -C fs clean
	$(MAKE) -C mm clean
	$(MAKE) -C init clean
	$(MAKE) -C lib clean
	rm -f scripts/mkversion

# 安装目标
install: all
	@echo "安装内核..."
	$(MAKE) -C arch/m4kk1 install

# 帮助目标
help:
	@echo "M4KK1内核构建系统"
	@echo "可用目标:"
	@echo "  all       - 构建整个内核 (默认)"
	@echo "  clean     - 清理构建文件"
	@echo "  install   - 安装内核"
	@echo "  help      - 显示此帮助信息"

# 菜单配置目标
menuconfig:
	@echo "启动内核配置界面..."
	@# 这里可以集成配置界面工具

# 版本信息生成（使用隐式规则）
scripts/mkversion: scripts/mkversion.c
	@echo "生成版本信息..."
	gcc -Wall -Wextra -O2 -g -I/usr/include -I/usr/local/include -o $@ $<

# 包含子目录Makefile
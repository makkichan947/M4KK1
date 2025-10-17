# M4KK1 Multi-Architecture Build System
# 多架构构建系统

# 默认架构
DEFAULT_ARCH := x86_64

# 支持的架构
SUPPORTED_ARCHES := x86_64 x86 arm arm64 powerpc riscv

# 导出变量
export

# 包含通用构建规则
include ../../scripts/Makefile.include

# 默认目标
.PHONY: all
all: kernel

# 帮助目标
.PHONY: help
help:
	@echo "M4KK1多架构操作系统构建系统"
	@echo "支持的架构: $(SUPPORTED_ARCHES)"
	@echo ""
	@echo "可用目标:"
	@echo "  all              - 构建默认架构 ($(DEFAULT_ARCH))"
	@echo "  kernel           - 构建内核"
	@echo "  clean            - 清理构建文件"
	@echo "  help             - 显示此帮助信息"
	@echo "  list-arch        - 列出支持的架构"
	@echo ""
	@echo "架构特定目标:"
	@for arch in $(SUPPORTED_ARCHES); do \
		echo "  $$arch           - 构建$$arch架构"; \
		echo "  $$arch-clean     - 清理$$arch架构"; \
		echo "  $$arch-install   - 安装$$arch架构"; \
	done

# 列出支持的架构
.PHONY: list-arch
list-arch:
	@echo "支持的架构:"
	@for arch in $(SUPPORTED_ARCHES); do \
		echo "  $$arch"; \
	done

# 内核构建目标
.PHONY: kernel
kernel: ../../scripts/mkversion
	@echo "构建M4KK1内核 ($(DEFAULT_ARCH))..."
	$(MAKE) -C ../../sys/src arch-$(DEFAULT_ARCH)

# 架构特定目标
.PHONY: $(SUPPORTED_ARCHES)
$(SUPPORTED_ARCHES):
	@echo "构建M4KK1内核 ($@)..."
	$(MAKE) -C ../../sys/src arch-$@

# 架构特定清理目标
.PHONY: $(addsuffix -clean, $(SUPPORTED_ARCHES))
$(addsuffix -clean, $(SUPPORTED_ARCHES)):
	arch=$(patsubst %-clean,%,$@); \
	echo "清理M4KK1内核 ($$arch)..."; \
	$(MAKE) -C ../../sys/src $$arch-clean

# 架构特定安装目标
.PHONY: $(addsuffix -install, $(SUPPORTED_ARCHES))
$(addsuffix -install, $(SUPPORTED_ARCHES)):
	arch=$(patsubst %-install,%,$@); \
	echo "安装M4KK1内核 ($$arch)..."; \
	$(MAKE) -C ../../sys/src $$arch-install

# 清理目标
.PHONY: clean
clean:
	@echo "清理M4KK1构建文件..."
	$(MAKE) -C ../../sys/src clean
	rm -f ../../scripts/mkversion

# 安装目标
.PHONY: install
install: kernel
	@echo "安装M4KK1内核..."
	$(MAKE) -C ../../sys/src install

# 版本信息生成
../../scripts/mkversion: ../../scripts/mkversion.c
	@echo "生成版本信息..."
	gcc -Wall -Wextra -O2 -g -I/usr/include -I/usr/local/include -o $@ $<

# 包含子目录Makefile
include ../../sys/src/Makefile
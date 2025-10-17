# M4KK1 Multi-Architecture Operating System
# 主构建系统 - 支持多架构

# 默认架构
DEFAULT_ARCH := x86_64

# 支持的架构
SUPPORTED_ARCHES := x86_64 x86 arm arm64 powerpc riscv

# 导出变量
export

# 包含通用构建规则
include scripts/Makefile.include

# 默认目标：显示帮助信息
.PHONY: all
all: help

# 帮助目标
.PHONY: help
help:
	@echo "========================================"
	@echo "M4KK1多架构操作系统构建系统"
	@echo "========================================"
	@echo ""
	@echo "快速构建命令："
	@echo "  make x86_64      - 构建x86_64架构"
	@echo "  make arm64       - 构建ARM64架构"
	@echo "  make riscv       - 构建RISC-V架构"
	@echo "  make multarch    - 构建所有架构"
	@echo "  make iso         - 创建多架构ISO镜像"
	@echo "  make clean       - 清理构建文件"
	@echo ""
	@echo "高级选项："
	@echo "  cd tools/build && make help"
	@echo ""
	@echo "支持的架构: x86_64, x86, arm, arm64, powerpc, riscv"
	@echo "========================================"

# 列出支持的架构
.PHONY: list-arch
list-arch:
	@echo "支持的架构:"
	@for arch in $(SUPPORTED_ARCHES); do \
		echo "  $$arch"; \
	done

# 架构特定目标 - 委托给tools/build目录
.PHONY: x86_64 x86 arm arm64 powerpc riscv multarch
x86_64 x86 arm arm64 powerpc riscv multarch:
	@$(MAKE) -C tools/build $@

# 清理目标
.PHONY: clean
clean:
	@$(MAKE) -C tools/build clean

# 其他目标
.PHONY: kernel install list-arch iso
kernel install list-arch iso:
	@$(MAKE) -C tools/build $@

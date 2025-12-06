# M4KK1 Virtualization System Makefile
# 构建M4KVM虚拟化系统

# 源代码文件
m4kvm_src := $(wildcard src/*.c)

# 目标文件名
M4KVM := m4kvm

# 包含通用构建规则
include ../../../scripts/Makefile.include

# 虚拟化系统对象文件
m4kvm_y := $(patsubst %.c,%.o,$(m4kvm_src))

# 默认目标
.PHONY: all
all: $(M4KVM)

# 构建虚拟化系统
$(M4KVM): $(m4kvm_y)
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

# 清理目标
.PHONY: clean
clean:
	rm -f $(m4kvm_y) $(M4KVM)

# 安装目标
.PHONY: install
install: $(M4KVM)
	@echo "安装虚拟化系统..."
# M4KK1 System Monitor Makefile
# 构建M4ktop系统监控工具

# 源代码文件
m4ktop_src := $(wildcard src/*.c)

# 目标文件名
M4KTOP := m4ktop

# 包含通用构建规则
include ../../../scripts/Makefile.include

# 监控工具对象文件
m4ktop_y := $(patsubst %.c,%.o,$(m4ktop_src))

# 默认目标
.PHONY: all
all: $(M4KTOP)

# 构建系统监控工具
$(M4KTOP): $(m4ktop_y)
	$(CC) $(CFLAGS) $^ -o $@ -lm

# 清理目标
.PHONY: clean
clean:
	rm -f $(m4ktop_y) $(M4KTOP)

# 安装目标
.PHONY: install
install: $(M4KTOP)
	@echo "安装系统监控工具..."
# M4KK1 ProcPS Makefile
# 进程和系统工具构建脚本

# 包含通用构建规则
include ../../../scripts/Makefile.include

# 工具列表
TOOLS := ps uname free df

# 目标文件
TOOL_OBJS := $(addsuffix .o, $(TOOLS))
TOOL_BINS := $(TOOLS)

# 默认目标
.PHONY: all
all: $(TOOL_BINS)

# 构建规则
$(TOOL_BINS): %: %.o
	$(CC) $(LDFLAGS) -o $@ $< $(LDLIBS)

# 编译规则
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# 清理目标
.PHONY: clean
clean:
	rm -f $(TOOL_OBJS) $(TOOL_BINS)

# 安装目标
.PHONY: install
install: all
	@echo "安装系统监控工具..."
	mkdir -p $(DESTDIR)/usr/bin
	for tool in $(TOOL_BINS); do \
		cp $$tool $(DESTDIR)/usr/bin/; \
	done

# 帮助目标
.PHONY: help
help:
	@echo "M4KK1系统监控工具构建系统"
	@echo "可用目标:"
	@echo "  all     - 构建所有工具"
	@echo "  clean   - 清理构建文件"
	@echo "  install - 安装工具"
	@echo "  help    - 显示此帮助"
	@echo ""
	@echo "工具列表: $(TOOLS)"
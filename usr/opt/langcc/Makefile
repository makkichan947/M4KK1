# M4KK1 LangCC Compiler Collection Makefile
# 构建LangCC编译器集合

# 源代码文件
langcc_src := $(wildcard src/*.c src/*/*.c)

# 目标文件名
LANGCC := langcc
LANGPP := langpp
LANGASM := langasm

# 包含通用构建规则
include ../../../scripts/Makefile.include

# 编译器对象文件
langcc_y := $(patsubst %.c,%.o,$(langcc_src))

# 默认目标
.PHONY: all
all: $(LANGCC) $(LANGPP) $(LANGASM)

# 构建主编译器
$(LANGCC): $(langcc_y)
	$(CC) $(CFLAGS) $^ -o $@

# 清理目标
.PHONY: clean
clean:
	rm -f $(langcc_y) $(LANGCC) $(LANGPP) $(LANGASM)

# 安装目标
.PHONY: install
install: all
	@echo "安装LangCC编译器集合..."
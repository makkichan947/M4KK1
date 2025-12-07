# M4KK1 MLang Compiler Makefile
# 构建MLang独特编译器

# 编译器和标志
CC := gcc
CFLAGS := -Wall -Wextra -O2 -g -Iinclude -I../include -std=c99
LDFLAGS := -lm

# 源代码文件
mlang_src := src/compiler_corrected_fixed.c src/utils_fixed.c src/main.c

# 目标文件名
MLANG := mlang
MLANG_TEST_FRAMEWORK := mlang-test-framework

# MLang编译器对象文件
mlang_o := $(patsubst %.c,%.o,$(mlang_src))

# 默认目标
.PHONY: all
all: $(MLANG)

# 构建主编译器
$(MLANG): $(mlang_src)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 构建测试框架
$(MLANG_TEST_FRAMEWORK): $(mlang_src)
	$(CC) $(CFLAGS) -o $@ ../test/mlang_test_framework.c $^ $(LDFLAGS)

# 编译单个源文件
%.o: %.c
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

# 清理目标
.PHONY: clean
clean:
	rm -f $(mlang_o) $(MLANG) $(MLANG_TEST_FRAMEWORK)

# 安装目标
.PHONY: install
install: all
	@echo "安装MLang编译器..."
	cp $(MLANG) /usr/local/bin/ 2>/dev/null || echo "需要sudo权限安装到/usr/local/bin"

# 运行测试
.PHONY: test
test: $(MLANG_TEST_FRAMEWORK)
	@echo "运行MLang测试框架..."
	./$(MLANG_TEST_FRAMEWORK)

# 编译测试链
.PHONY: test-chain
test-chain: $(MLANG)
	@echo "编译并运行MLang测试链..."
	./$(MLANG) /workspace/test/mlang_test_chain.mlang -o /tmp/mlang_test_chain.out
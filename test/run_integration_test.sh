#!/bin/bash

# M4KK1 系统综合测试运行脚本

echo "M4KK1 系统综合测试"
echo "=================="

# 检查测试文件是否存在
if [ ! -f "integration_test.c" ]; then
    echo "错误: integration_test.c 文件不存在"
    exit 1
fi

# 编译测试程序
echo "编译综合测试程序..."
make integration_test

if [ $? -ne 0 ]; then
    echo "编译失败"
    exit 1
fi

# 检查编译产物
if [ ! -f "integration_test" ]; then
    echo "错误: 编译产物 integration_test 不存在"
    exit 1
fi

echo "编译成功，开始运行测试..."

# 运行测试程序
echo "执行综合测试..."
./integration_test

echo "测试完成"
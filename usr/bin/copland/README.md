# Copland窗口服务器

Copland是M4KK1操作系统中的轻量级、自研窗口服务器，采用原创协议设计，基于现代渲染管线架构。

## 特性

- **轻量级协议**: 自主设计的Copland协议，专为高性能和低延迟优化
- **现代渲染管线**: 基于Vulkan的渲染架构，支持硬件加速
- **零拷贝缓冲区**: 高效的共享内存管理，减少数据复制开销
- **异步事件处理**: 高性能输入输出系统，支持多客户端并发
- **实时性能监控**: 内置统计和监控功能，便于调试和优化

## 架构设计

### 核心组件

1. **Copland协议** (`include/copland-protocol.h`)
   - 定义消息格式和数据结构
   - 窗口管理相关类型定义
   - 事件和请求类型枚举

2. **服务器核心** (`src/server.c`)
   - 窗口管理（创建、销毁、移动、调整大小）
   - 表面合成和渲染协调
   - 现代缓冲区管理（零拷贝共享内存）
   - 异步输入事件分发

3. **客户端库** (`src/client.c`)
   - 客户端API接口
   - 协议编解码
   - 窗口操作函数

### 协议特性

- **消息类型**: 支持窗口操作、表面管理、缓冲区操作、输入事件等
- **窗口类型**: 顶级窗口、弹出窗口、临时窗口、拖拽窗口、子表面、光标窗口
- **缓冲区格式**: ARGB8888、XRGB8888、RGB565、NV12、YUV420
- **输入设备**: 键盘、指针、触摸设备支持

## 编译和安装

### 依赖要求

- GCC 编译器
- POSIX线程库
- 实时库（可选）

### 构建

```bash
# 构建主程序
make

# 构建测试程序
make test

# 开发模式构建
make dev

# 发布模式构建
make release
```

### 安装

```bash
# 安装到系统
sudo make install

# 卸载
sudo make uninstall
```

## 使用方法

### 启动服务器

```bash
# 基本启动
copland

# 指定配置文件
copland -c /etc/copland.conf

# 守护进程模式
copland -d

# 详细输出模式
copland -V

# 显示帮助
copland -h
```

### 客户端开发

```c
#include "copland.h"

/* 创建客户端 */
copland_client_t *client = copland_client_create();

/* 连接到服务器 */
copland_client_connect(client, "/tmp/copland.sock");

/* 创建窗口 */
uint32_t window_id = copland_client_create_window(
    client, 0, COPLAND_WINDOW_TOPLEVEL,
    100, 100, 800, 600, "我的窗口");

/* 发送输入事件 */
copland_client_send_button_event(client, 1, true, timestamp);

/* 清理资源 */
copland_client_destroy(client);
```

### 测试

```bash
# 运行集成测试
./copland-test

# 或直接运行
make test && ./copland-test
```

## 文件结构

```
usr/bin/copland/
├── include/
│   ├── copland.h           # 主接口头文件
│   └── copland-protocol.h  # 协议定义头文件
├── src/
│   ├── main.c              # 主程序入口
│   ├── server.c            # 服务器核心实现
│   ├── client.c            # 客户端库实现
│   └── test.c              # 测试程序
├── docs/
│   └── design.md           # 设计文档
├── config/
│   └── copland.conf        # 配置文件
├── clients/                # 示例客户端
├── protocols/              # 协议文档
└── Makefile               # 构建脚本
```

## 性能特性

- **低延迟**: 异步事件处理，最小化响应时间
- **高吞吐**: 零拷贝缓冲区管理，减少内存操作
- **可扩展**: 模块化设计，支持动态加载
- **监控**: 内置性能统计，便于调试和优化

## 调试和监控

服务器支持多种调试功能：

- 详细日志输出 (`-V` 参数)
- 实时性能统计 (`SIGUSR2` 信号)
- 配置热重载 (`SIGUSR1` 信号)

## 作者

makkichan947 - 独立完成

## 许可证

专为M4KK1操作系统开发，版权所有。
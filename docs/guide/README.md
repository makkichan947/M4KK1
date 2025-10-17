# M4KK1 用户指南

欢迎使用M4KK1操作系统！本指南将帮助您了解如何使用这个独特的操作系统。

## 快速开始

### 系统要求
- 支持的处理器架构：x86_64, x86, ARM64, ARM, PowerPC, RISC-V
- 最小内存：64MB
- 推荐内存：256MB或更高
- 存储空间：至少100MB

### 安装M4KK1

#### 从ISO镜像安装
1. 下载M4KK1多架构ISO镜像
2. 使用支持的引导加载程序启动
3. 选择合适的架构版本
4. 按照安装向导完成安装

#### 虚拟机安装
```bash
# 使用QEMU运行M4KK1
qemu-system-x86_64 -cdrom m4kk1-multarch.iso -m 256M
```

## 系统使用

### 登录
M4KK1启动后会显示登录提示符：
```
M4KK1 login:
```

输入用户名和密码进行登录。

### 基本命令

#### 文件操作
```bash
# 列出文件
ls

# 切换目录
cd /usr/bin

# 复制文件
cp file1.txt file2.txt

# 移动文件
mv old.txt new.txt

# 删除文件
rm file.txt
```

#### 进程管理
```bash
# 查看进程
ps

# 结束进程
kill 1234

# 查看系统信息
uname -a
```

#### 网络操作
```bash
# 查看网络接口
ifconfig

# 测试网络连接
ping 192.168.1.1
```

### 文本编辑

#### 使用vi编辑器
```bash
# 编辑文件
vi filename.txt

# 基本操作：
# i - 插入模式
# Esc - 命令模式
# :wq - 保存并退出
# :q! - 强制退出
```

### 程序开发

#### 编译C程序
```bash
# 编译
m4k-gcc hello.c -o hello

# 运行程序
./hello
```

#### 编译汇编程序
```bash
# 编译
m4k-as program.asm -o program.o

# 链接
m4k-ld program.o -o program
```

## 系统管理

### 用户管理
```bash
# 添加用户
useradd username

# 设置密码
passwd username

# 删除用户
userdel username
```

### 文件权限
```bash
# 更改文件权限
chmod 755 filename

# 更改文件所有者
chown user:group filename
```

### 系统监控
```bash
# 查看内存使用
free

# 查看磁盘使用
df

# 查看系统负载
uptime
```

## 高级特性

### 多架构支持
M4KK1支持在不同处理器架构上运行：

- **x86_64**: 高性能计算
- **ARM64**: 移动和嵌入式
- **RISC-V**: 开源硬件平台
- **PowerPC**: 大型机应用

### 独特文件系统
M4KK1使用YFS文件系统，提供：
- 高级元数据支持
- 内置加密功能
- 快照和版本控制
- 压缩和重复数据删除

### 安全特性
- 基于能力的访问控制
- 独特的安全标识符
- 强制访问控制策略
- 加密文件系统

## 故障排除

### 常见问题

#### 启动问题
- 确保使用支持的引导加载程序
- 检查系统是否满足最小要求
- 验证ISO镜像完整性

#### 性能问题
- 增加内存分配
- 检查磁盘I/O性能
- 监控系统负载

#### 兼容性问题
注意：M4KK1故意与现有系统不兼容。如果需要兼容性，请考虑其他操作系统。

### 获取帮助

#### 文档
- 架构文档：`docs/arch/`
- API文档：`docs/api/`
- 开发指南：`docs/guide/`

#### 社区支持
- 官方论坛：https://m4kk1.org/forum
- IRC频道：#m4kk1 on irc.libera.chat
- 邮件列表：dev@m4kk1.org

## 开发指南

### 构建系统
```bash
# 构建特定架构
make x86_64

# 构建所有架构
make multarch

# 创建ISO镜像
make iso
```

### 运行测试
```bash
# 运行所有测试
make test

# 运行特定测试
./test_program
```

### 调试
```bash
# 使用GDB调试
m4k-gdb program

# 查看系统日志
dmesg

# 检查进程状态
ps aux
```

## 许可证和法律

M4KK1是一个开源项目，但采用独特的许可证模式。请查看LICENSE文件了解详细条款。

**重要声明**：M4KK1故意设计为与现有操作系统不兼容。此设计是刻意的，旨在提供独特的计算环境。

## 贡献指南

欢迎社区贡献！请遵循以下步骤：

1. Fork项目仓库
2. 创建功能分支
3. 实现更改
4. 运行测试
5. 提交拉取请求

### 贡献要求
- 遵循M4KK1的独特设计理念
- 维护与现有系统的非兼容性
- 编写适当的文档
- 包含测试用例

## 版本历史

- **v0.2.0 (当前)**: 多架构支持，M4K ABI
- **v0.1.0**: 初始x86版本

## 致谢

感谢所有为M4KK1项目做出贡献的开发者和用户！

---

*本指南最后更新于2025年10月。如有问题，请参考最新文档。*
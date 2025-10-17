# M4KK1 API 文档

本文档描述了M4KK1操作系统的应用程序接口（API）。

## 系统调用API

M4KK1使用独特的中断号0x4D进行系统调用，区别于标准Linux的0x80。

### 基本系统调用

#### m4k_exit(status)
退出当前进程
- **系统调用号**: M4K_SYS_EXIT (0xM4K00001)
- **参数**: status - 退出状态码
- **返回值**: 不返回

```c
long m4k_exit(int status) {
    return m4k_syscall1(M4K_SYS_EXIT, status);
}
```

#### m4k_read(fd, buf, count)
从文件描述符读取数据
- **系统调用号**: M4K_SYS_READ (0xM4K00003)
- **参数**:
  - fd: 文件描述符
  - buf: 缓冲区指针
  - count: 读取字节数
- **返回值**: 实际读取的字节数，错误返回负数

#### m4k_write(fd, buf, count)
向文件描述符写入数据
- **系统调用号**: M4K_SYS_WRITE (0xM4K00004)
- **参数**:
  - fd: 文件描述符
  - buf: 缓冲区指针
  - count: 写入字节数
- **返回值**: 实际写入的字节数，错误返回负数

#### m4k_open(pathname, flags)
打开文件
- **系统调用号**: M4K_SYS_OPEN (0xM4K00005)
- **参数**:
  - pathname: 文件路径
  - flags: 打开标志
- **返回值**: 文件描述符，错误返回负数

#### m4k_close(fd)
关闭文件描述符
- **系统调用号**: M4K_SYS_CLOSE (0xM4K00006)
- **参数**: fd - 文件描述符
- **返回值**: 0表示成功，负数表示错误

### 进程管理

#### m4k_fork()
创建子进程
- **系统调用号**: M4K_SYS_FORK (0xM4K00002)
- **返回值**: 子进程返回0，父进程返回子进程ID

#### m4k_exec(filename, argv, envp)
执行程序
- **系统调用号**: M4K_SYS_EXEC (0xM4K00007)
- **参数**:
  - filename: 可执行文件路径
  - argv: 参数列表
  - envp: 环境变量列表
- **返回值**: 不返回（成功）或错误码

### 内存管理

#### m4k_mmap(addr, length, prot, flags, fd, offset)
映射内存
- **系统调用号**: M4K_SYS_MMAP (0xM4K00008)
- **参数**:
  - addr: 映射地址（可为NULL）
  - length: 映射长度
  - prot: 保护标志
  - flags: 映射标志
  - fd: 文件描述符
  - offset: 文件偏移
- **返回值**: 映射地址，错误返回负数

#### m4k_munmap(addr, length)
取消内存映射
- **系统调用号**: M4K_SYS_MUNMAP (0xM4K00009)
- **参数**:
  - addr: 映射地址
  - length: 映射长度
- **返回值**: 0表示成功，负数表示错误

### 标志位定义

#### 文件打开标志
```c
#define M4K_O_RDONLY    0x00000001  /* 只读 */
#define M4K_O_WRONLY    0x00000002  /* 只写 */
#define M4K_O_RDWR      0x00000004  /* 读写 */
#define M4K_O_CREAT     0x00000100  /* 创建文件 */
#define M4K_O_EXCL      0x00000200  /* 排他创建 */
#define M4K_O_TRUNC     0x00001000  /* 截断文件 */
#define M4K_O_APPEND    0x00002000  /* 追加模式 */
#define M4K_O_NONBLOCK  0x00004000  /* 非阻塞 */
```

#### 内存保护标志
```c
#define M4K_PROT_NONE   0x00        /* 无访问 */
#define M4K_PROT_READ   0x01        /* 可读 */
#define M4K_PROT_WRITE  0x02        /* 可写 */
#define M4K_PROT_EXEC   0x04        /* 可执行 */
```

#### 内存映射标志
```c
#define M4K_MAP_SHARED     0x01     /* 共享映射 */
#define M4K_MAP_PRIVATE    0x02     /* 私有映射 */
#define M4K_MAP_FIXED      0x10     /* 固定地址 */
#define M4K_MAP_ANONYMOUS  0x20     /* 匿名映射 */
```

## 库函数API

### 字符串操作

#### m4k_strlen(str)
计算字符串长度
```c
size_t m4k_strlen(const char *str);
```

#### m4k_strcpy(dst, src)
复制字符串
```c
char *m4k_strcpy(char *dst, const char *src);
```

#### m4k_strcmp(s1, s2)
比较字符串
```c
int m4k_strcmp(const char *s1, const char *s2);
```

### 内存操作

#### m4k_memcpy(dst, src, n)
复制内存
```c
void *m4k_memcpy(void *dst, const void *src, size_t n);
```

#### m4k_memset(s, c, n)
设置内存
```c
void *m4k_memset(void *s, int c, size_t n);
```

#### m4k_memcmp(s1, s2, n)
比较内存
```c
int m4k_memcmp(const void *s1, const void *s2, size_t n);
```

### 输入输出

#### m4k_printf(format, ...)
格式化输出
```c
int m4k_printf(const char *format, ...);
```

#### m4k_scanf(format, ...)
格式化输入
```c
int m4k_scanf(const char *format, ...);
```

## 错误码

M4KK1使用独特的错误码范围：

- **成功**: 0
- **通用错误**: 0xM4K00000 - 0xM4K0FFFF
- **权限错误**: 0xM4K10000 - 0xM4K1FFFF
- **文件错误**: 0xM4K20000 - 0xM4K2FFFF
- **内存错误**: 0xM4K30000 - 0xM4K3FFFF
- **进程错误**: 0xM4K40000 - 0xM4K4FFFF

## 示例程序

### 简单Hello World
```c
#include <m4k_syscall.h>

int main() {
    const char *msg = "Hello, M4KK1!\n";
    m4k_write(1, msg, 14);  // 标准输出
    m4k_exit(0);
    return 0;
}
```

### 文件操作
```c
#include <m4k_syscall.h>

int main() {
    // 打开文件
    int fd = m4k_open("test.txt", M4K_O_RDONLY);
    if (fd < 0) {
        return 1;
    }

    // 读取文件
    char buf[256];
    long n = m4k_read(fd, buf, sizeof(buf));
    if (n > 0) {
        m4k_write(1, buf, n);  // 输出到标准输出
    }

    // 关闭文件
    m4k_close(fd);
    m4k_exit(0);
    return 0;
}
```

## 编译和链接

### 编译用户程序
```bash
# 编译
m4k-gcc -c hello.c -o hello.o

# 链接
m4k-ld hello.o -o hello -lc
```

### 静态链接
```bash
m4k-ld hello.o -o hello --static -lc
```

### 动态链接
```bash
m4k-ld hello.o -o hello -lc
```

## 调试支持

### 调试信息
M4KK1提供独特的调试接口：
- 栈追踪
- 内存使用分析
- 系统调用追踪
- 性能监控

### 调试工具
- m4k-gdb: M4KK1调试器
- m4k-strace: 系统调用追踪
- m4k-ltrace: 库函数调用追踪
- m4k-valgrind: 内存调试工具

## 最佳实践

1. **错误处理**: 始终检查系统调用返回值
2. **资源管理**: 正确释放分配的资源
3. **权限检查**: 验证文件和目录权限
4. **缓冲区安全**: 防止缓冲区溢出
5. **线程安全**: 在多线程程序中注意同步

## 版本历史

- **v0.2.0**: 添加多架构支持，引入M4K ABI
- **v0.1.0**: 初始版本，x86架构支持

## 许可证

M4KK1 API遵循M4KK1操作系统的许可证条款。
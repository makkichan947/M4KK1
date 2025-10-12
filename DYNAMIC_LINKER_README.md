# M4KK1 动态链接器系统

## 概述

M4KK1 动态链接器系统是一个完整的自定义动态链接库解决方案，专为 M4KK1 操作系统设计。该系统实现了自定义的 `.m4ll` (M4KK1 Library Link) 动态库格式，支持运行时库加载、符号解析和地址重定位。

## 架构设计

### 文件格式设计

M4KK1 动态库采用自定义的二进制格式 `.m4ll`，包含以下组件：

#### 文件头结构 (`m4ll_header_t`)
```c
typedef struct {
    uint32_t magic;           /* 文件魔数: 0x4D344C4C ("M4LL") */
    uint32_t version;         /* 格式版本 */
    uint32_t flags;           /* 标志位 */
    uint32_t entry_point;     /* 入口点偏移 */
    uint32_t phdr_offset;     /* 程序头表偏移 */
    uint32_t phdr_count;      /* 程序头数量 */
    uint32_t shdr_offset;     /* 段表偏移 */
    uint32_t shdr_count;      /* 段数量 */
    uint32_t strtab_offset;   /* 字符串表偏移 */
    uint32_t strtab_size;     /* 字符串表大小 */
    uint32_t symtab_offset;   /* 符号表偏移 */
    uint32_t symtab_count;    /* 符号数量 */
    uint32_t rel_offset;      /* 重定位表偏移 */
    uint32_t rel_count;       /* 重定位数量 */
    uint32_t dep_offset;      /* 依赖表偏移 */
    uint32_t dep_count;       /* 依赖数量 */
    uint32_t checksum;        /* 校验和 */
} m4ll_header_t;
```

#### 段类型
- `M4LL_SEGMENT_CODE` (1): 代码段
- `M4LL_SEGMENT_DATA` (2): 数据段
- `M4LL_SEGMENT_BSS` (3): BSS段
- `M4LL_SEGMENT_RODATA` (4): 只读数据段

#### 符号类型
- `M4LL_SYMBOL_LOCAL` (0): 局部符号
- `M4LL_SYMBOL_GLOBAL` (1): 全局符号
- `M4LL_SYMBOL_WEAK` (2): 弱符号

#### 重定位类型
- `M4LL_RELOCATION_32` (1): 32位绝对地址重定位
- `M4LL_RELOCATION_PC32` (2): 32位PC相对重定位
- `M4LL_RELOCATION_GOT32` (3): 32位GOT重定位
- `M4LL_RELOCATION_PLT32` (4): 32位PLT重定位

### 系统组件

#### 1. 核心数据结构

**库信息结构 (`m4ll_library_t`)**
```c
typedef struct m4ll_library {
    char *name;                 /* 库名称 */
    void *base_addr;           /* 基地址 */
    uint32_t status;           /* 库状态 */
    m4ll_header_t *header;     /* 文件头 */
    m4ll_sym_t *symtab;        /* 符号表 */
    char *strtab;              /* 字符串表 */
    m4ll_rel_t *reltab;        /* 重定位表 */
    m4ll_dep_t *deptab;        /* 依赖表 */
    uint32_t ref_count;        /* 引用计数 */
    struct m4ll_library *next; /* 下一个库 */
    struct m4ll_library *deps; /* 依赖库链表 */
} m4ll_library_t;
```

**全局符号表项 (`m4ll_symbol_t`)**
```c
typedef struct m4ll_symbol {
    char *name;                 /* 符号名 */
    void *address;             /* 符号地址 */
    uint32_t size;             /* 符号大小 */
    uint32_t type;             /* 符号类型 */
    uint32_t binding;          /* 符号绑定 */
    m4ll_library_t *library;   /* 所属库 */
    struct m4ll_symbol *next;  /* 下一个符号 */
} m4ll_symbol_t;
```

#### 2. 核心功能模块

**文件格式解析**
- 验证文件魔数和校验和
- 解析文件头、符号表、字符串表、重定位表和依赖表
- 支持多种段类型和符号类型

**符号表管理**
- 维护全局符号表和局部符号表
- 支持符号的动态添加和查找
- 实现符号名称哈希映射

**库加载和卸载**
- 支持递归依赖加载
- 引用计数管理
- 内存映射和权限管理

**符号解析和重定位**
- 实现多种重定位类型处理
- 支持延迟绑定和即时绑定
- 地址空间布局随机化(ASLR)支持

**依赖关系管理**
- 解析库依赖关系图
- 循环依赖检测
- 版本兼容性检查

## 系统调用接口

### 动态库管理
```c
/* 加载动态库 */
uint32_t dl_load_library(const char *filename);

/* 卸载动态库 */
uint32_t dl_unload_library(uint32_t handle);

/* 查找符号 */
void *dl_find_symbol(const char *symbol);

/* 获取错误信息 */
const char *dl_get_error(void);
```

### 系统调用号
- `SYSCALL_DL_LOAD_LIBRARY` (0x80): 加载动态库
- `SYSCALL_DL_UNLOAD_LIBRARY` (0x81): 卸载动态库
- `SYSCALL_DL_FIND_SYMBOL` (0x82): 查找符号
- `SYSCALL_DL_GET_ERROR` (0x83): 获取错误信息

## 使用示例

### 用户程序中使用动态库

```c
#include <syscall.h>

/* 加载动态库 */
uint32_t lib_handle = SYSCALL1(SYSCALL_DL_LOAD_LIBRARY, "mylib.m4ll");
if (lib_handle == SYSCALL_ERROR) {
    /* 处理错误 */
    char error_msg[256];
    SYSCALL2(SYSCALL_DL_GET_ERROR, error_msg, sizeof(error_msg));
    /* 显示错误信息 */
}

/* 查找函数符号 */
void (*my_function)(void) = (void (*)(void))SYSCALL1(SYSCALL_DL_FIND_SYMBOL, "my_function");
if (my_function) {
    my_function(); /* 调用函数 */
}

/* 卸载库 */
SYSCALL1(SYSCALL_DL_UNLOAD_LIBRARY, lib_handle);
```

## 实现细节

### 内存布局

```
0xC0000000 +-------------------+ 内核空间
           |                   |
           |  内核代码和数据   |
           |                   |
0xD0000000 +-------------------+ 动态库加载基地址
           |                   |
           |   库1 (4MB)       |
           |                   |
0xD1000000 +-------------------+
           |                   |
           |   库2 (4MB)       |
           |                   |
0xD2000000 +-------------------+
           |       ...         |
```

### 符号解析流程

1. **加载阶段**: 解析库文件，构建符号表
2. **依赖解析**: 递归加载依赖库，建立依赖关系图
3. **重定位阶段**: 解析重定位表，修正地址引用
4. **符号绑定**: 将符号地址填入全局符号表

### 错误处理

系统提供统一的错误处理机制：

```c
/* 错误码定义 */
#define M4LL_ERROR_NONE         0
#define M4LL_ERROR_FILE_NOT_FOUND 1
#define M4LL_ERROR_INVALID_FORMAT 2
#define M4LL_ERROR_LOAD_FAILED    3
#define M4LL_ERROR_SYMBOL_NOT_FOUND 4
#define M4LL_ERROR_RELOCATION_FAILED 5
#define M4LL_ERROR_DEPENDENCY_FAILED 6
#define M4LL_ERROR_MEMORY_FAILED  7
```

## 测试和验证

### 测试程序

项目包含完整的测试套件：

- **ldso_test.c**: 基本功能测试
- **test/Makefile**: 测试程序构建脚本

### 测试覆盖

- 文件格式解析测试
- 符号表管理测试
- 库加载/卸载测试
- 符号解析测试
- 依赖关系处理测试
- 错误处理测试

## 构建和集成

### 内核集成

1. 在内核初始化时调用 `m4ll_init()`
2. 在进程销毁时调用 `m4ll_cleanup()`
3. 注册动态库相关系统调用

### 编译选项

```makefile
# 内核构建
CFLAGS += -I$(KERNEL_INCLUDE_DIR)
LDFLAGS += -Ttext 0xC0000000

# 测试程序构建
TEST_CFLAGS += -I$(KERNEL_INCLUDE_DIR) -nostdlib -ffreestanding -m32
```

## 性能考虑

### 优化策略

1. **延迟绑定**: 符号解析在首次使用时进行
2. **缓存机制**: 频繁使用的符号地址缓存
3. **内存池**: 减少动态内存分配开销
4. **哈希表**: 高效的符号查找算法

### 内存管理

- 使用内核内存分配器进行库内存管理
- 支持写时复制(Copy-on-Write)机制
- 引用计数避免内存泄漏

## 安全特性

### 访问控制

- 基于进程权限的库访问控制
- 符号可见性控制
- 安全的内存映射机制

### 完整性保护

- 文件校验和验证
- 运行时地址验证
- 重定位安全性检查

## 扩展性设计

### 插件架构

系统支持运行时插件加载：

```c
/* 插件接口定义 */
typedef struct {
    const char *name;
    const char *version;
    int (*init)(void);
    void (*cleanup)(void);
    void *(*get_symbol)(const char *name);
} plugin_interface_t;
```

### 版本管理

支持库版本管理和兼容性检查：

```c
typedef struct {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
    const char *prerelease;
    const char *build;
} version_t;
```

## 故障排除

### 常见问题

1. **库加载失败**
   - 检查文件路径和权限
   - 验证文件格式和校验和
   - 检查依赖关系

2. **符号未找到**
   - 确认符号名称拼写
   - 检查符号可见性
   - 验证库是否正确加载

3. **内存错误**
   - 检查系统内存使用情况
   - 验证内存映射配置
   - 监控引用计数

### 调试工具

系统提供调试和诊断工具：

- 符号表转储
- 依赖关系图可视化
- 内存使用统计
- 性能分析工具

## 未来改进

### 计划功能

1. **JIT编译支持**: 动态代码生成和优化
2. **跨平台兼容**: 支持多种目标架构
3. **热更新机制**: 不重启服务更新库
4. **沙箱隔离**: 增强安全性
5. **性能监控**: 实时性能分析

### 标准化

考虑采用现有标准格式：
- ELF格式支持
- WebAssembly运行时
- 跨平台ABI兼容性

---

**注意**: 这是一个实验性实现，旨在展示动态链接器的核心概念和实现技术。实际部署时需要根据具体需求进行调整和优化。
/**
 * M4KK1 x86_64 Architecture - System Call Handling Implementation
 * x86_64架构系统调用处理实现 - 独特的M4KK1系统调用ABI
 *
 * 文件: syscall.c
 * 作者: M4KK1 Development Team
 * 版本: v0.2.0-multarch
 * 日期: 2025-10-16
 *
 * 描述:
 *   实现x86_64架构的完整系统调用处理机制，包括：
 *   - M4KK1独特系统调用ABI (中断号0x4D)
 *   - 参数传递和返回值处理
 *   - 权限检查和验证
 *   - 系统调用统计和调试
 *   - 多架构兼容性支持
 *
 * 架构特性:
 *   - 使用SYSCALL/SYSRET指令 (x86_64)
 *   - 6个参数传递 (RDI, RSI, RDX, RCX, R8, R9)
 *   - 返回值通过RAX传递
 *   - 独特的错误码范围 (0xM4Kxxxxx)
 *   - 权限级别隔离
 *
 * 系统调用ABI:
 *   - 中断号: 0x4D (M4KK1独特)
 *   - 参数传递: RDI, RSI, RDX, RCX, R8, R9
 *   - 返回值: RAX
 *   - 错误码: 负数 (M4KK1独特格式)
 *
 * 权限模型:
 *   - 内核权限: 0xFFFFFFFF
 *   - 系统权限: 0x000000FF
 *   - 用户权限: 0x00000001
 *   - 权限检查: 位掩码操作
 *
 * 修改历史:
 *   v0.2.0: 实现完整的x86_64系统调用处理
 *   v0.1.0: 初始系统调用实现
 *
 * 依赖:
 *   - m4k_arch.h: 架构特定定义
 *   - m4k_syscall.h: M4KK1独特系统调用定义
 *   - console.h: 控制台输出
 *   - string.h: 字符串操作
 *   - process.h: 进程管理
 */

#include "../../../include/m4k_arch.h"
#include "../../../include/m4k_syscall.h"
#include "../../../include/console.h"
#include "../../../include/string.h"
#include "../../../include/process.h"

/* 系统调用处理函数类型 */
typedef uint64_t (*m4k_syscall_handler_t)(uint64_t arg1, uint64_t arg2, uint64_t arg3,
                                         uint64_t arg4, uint64_t arg5, uint64_t arg6);

/* 系统调用表项 */
typedef struct {
    m4k_syscall_handler_t handler;      /* 系统调用处理函数 */
    uint32_t permission_mask;           /* 权限掩码 */
    const char *name;                   /* 系统调用名称 */
    bool registered;                    /* 是否已注册 */
} m4k_syscall_entry_t;

/* 系统调用表 */
static m4k_syscall_entry_t m4k_syscall_table[256];

/* 系统调用统计 */
static struct {
    uint64_t total_calls;
    uint64_t failed_calls;
    uint64_t permission_denied;
    uint64_t calls_by_type[256];
} m4k_syscall_stats;

/* 权限级别 */
#define M4K_PERMISSION_KERNEL    0xFFFFFFFF
#define M4K_PERMISSION_SYSTEM    0x000000FF
#define M4K_PERMISSION_USER      0x00000001

/**
 * 初始化系统调用表
 */
static void m4k_syscall_table_init(void) {
    memset(m4k_syscall_table, 0, sizeof(m4k_syscall_table));
    memset(&m4k_syscall_stats, 0, sizeof(m4k_syscall_stats));

    console_write("M4KK1 x86_64 system call table initialized\n");
}

/**
 * 检查系统调用权限
 */
static bool m4k_syscall_check_permission(uint32_t syscall_num, uint32_t current_permission) {
    if (syscall_num >= 256 || !m4k_syscall_table[syscall_num].registered) {
        return false;
    }

    /* 内核权限可以调用任何系统调用 */
    if (current_permission == M4K_PERMISSION_KERNEL) {
        return true;
    }

    /* 检查权限掩码 */
    return (current_permission & m4k_syscall_table[syscall_num].permission_mask) != 0;
}

/**
 * M4KK1独特系统调用处理函数
 * 使用中断号0x4D（而非标准0x80）
 */
void m4k_syscall_handler(void) {
    uint64_t syscall_num;
    uint64_t result = 0xM4K00000;  /* M4KK1独特的错误码 */
    uint32_t current_permission;

    /* 获取系统调用号（从RAX） */
    __asm__ volatile ("movq %%rax, %0" : "=r"(syscall_num));

    /* 统计总调用次数 */
    m4k_syscall_stats.total_calls++;

    /* 检查系统调用号有效性 */
    if (syscall_num >= 256) {
        m4k_syscall_stats.failed_calls++;
        goto syscall_return;
    }

    /* 检查系统调用是否已注册 */
    if (!m4k_syscall_table[syscall_num].registered) {
        m4k_syscall_stats.failed_calls++;
        goto syscall_return;
    }

    /* 获取当前进程权限级别 */
    /* 简化实现：假设用户权限 */
    current_permission = M4K_PERMISSION_USER;

    /* 检查权限 */
    if (!m4k_syscall_check_permission(syscall_num, current_permission)) {
        m4k_syscall_stats.permission_denied++;
        result = 0xM4K00001;  /* M4KK1独特的权限拒绝码 */
        goto syscall_return;
    }

    /* 调用系统调用处理函数 */
    m4k_syscall_handler_t handler = m4k_syscall_table[syscall_num].handler;
    if (handler != NULL) {
        /* 获取参数 */
        uint64_t arg1, arg2, arg3, arg4, arg5, arg6;
        __asm__ volatile (
            "movq %%rdi, %0\n"
            "movq %%rsi, %1\n"
            "movq %%rdx, %2\n"
            "movq %%rcx, %3\n"
            "movq %%r8, %4\n"
            "movq %%r9, %5\n"
            : "=r"(arg1), "=r"(arg2), "=r"(arg3), "=r"(arg4), "=r"(arg5), "=r"(arg6)
        );

        result = handler(arg1, arg2, arg3, arg4, arg5, arg6);
        m4k_syscall_stats.calls_by_type[syscall_num]++;
    } else {
        result = 0xM4K00002;  /* M4KK1独特的手柄为空错误码 */
    }

syscall_return:
    /* 设置返回值到RAX寄存器 */
    __asm__ volatile ("movq %0, %%rax" : : "r"(result));
}

/**
 * 初始化M4KK1独特系统调用系统
 */
void m4k_syscall_init(void) {
    /* 初始化系统调用表 */
    m4k_syscall_table_init();

    /* 注册系统调用处理函数到IDT */
    /* 注意：这里需要调用中断注册函数 */

    /* 初始化并注册所有系统调用处理函数 */
    m4k_syscall_init_handlers();

    console_write("M4KK1 x86_64 system call system initialized\n");
}

/**
 * 注册M4KK1独特系统调用处理函数
 */
void m4k_syscall_register(uint32_t num, void *handler) {
    if (num >= 256) {
        console_write("Invalid M4KK1 system call number: 0x");
        console_write_hex(num);
        console_write("\n");
        return;
    }

    if (handler == NULL) {
        console_write("Cannot register NULL handler for M4KK1 system call 0x");
        console_write_hex(num);
        console_write("\n");
        return;
    }

    /* 注册处理函数 */
    m4k_syscall_table[num].handler = (m4k_syscall_handler_t)handler;
    m4k_syscall_table[num].registered = true;

    /* 设置默认权限（用户权限） */
    m4k_syscall_table[num].permission_mask = M4K_PERMISSION_USER;

    /* 设置系统调用名称 */
    m4k_syscall_table[num].name = m4k_syscall_get_name(num);

    console_write("M4KK1 system call 0x");
    console_write_hex(num);
    console_write(" registered: ");
    console_write(m4k_syscall_table[num].name ? m4k_syscall_table[num].name : "Unknown");
    console_write("\n");
}

/**
 * 获取M4KK1独特系统调用名称
 */
const char *m4k_syscall_get_name(uint32_t num) {
    switch (num) {
        case M4K_SYS_EXIT: return "m4k_exit";
        case M4K_SYS_FORK: return "m4k_fork";
        case M4K_SYS_READ: return "m4k_read";
        case M4K_SYS_WRITE: return "m4k_write";
        case M4K_SYS_OPEN: return "m4k_open";
        case M4K_SYS_CLOSE: return "m4k_close";
        case M4K_SYS_EXEC: return "m4k_exec";
        case M4K_SYS_MMAP: return "m4k_mmap";
        case M4K_SYS_MUNMAP: return "m4k_munmap";
        case M4K_SYS_IOCTL: return "m4k_ioctl";
        case M4K_SYS_FCNTL: return "m4k_fcntl";
        case M4K_SYS_SELECT: return "m4k_select";
        case M4K_SYS_POLL: return "m4k_poll";
        case M4K_SYS_EPOLL: return "m4k_epoll";
        default: return "unknown";
    }
}

/* ====================================================================
   M4KK1独特系统调用实现函数
   ==================================================================== */

/**
 * 系统调用：m4k_exit - 退出当前进程
 */
static uint64_t m4k_syscall_exit_impl(uint64_t arg1, uint64_t arg2, uint64_t arg3,
                                     uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    uint32_t status = (uint32_t)arg1;

    console_write("M4KK1 process exit called with status: ");
    console_write_dec(status);
    console_write("\n");

    /* 调用进程退出函数 */
    /* TODO: 实现进程退出 */

    /* 不会到达这里 */
    return 0;
}

/**
 * 系统调用：m4k_read - 从文件描述符读取数据
 */
static uint64_t m4k_syscall_read_impl(uint64_t arg1, uint64_t arg2, uint64_t arg3,
                                     uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    uint32_t fd = (uint32_t)arg1;
    void *buf = (void *)arg2;
    uint64_t count = arg3;

    console_write("M4KK1 Read system call: fd=");
    console_write_dec(fd);
    console_write(", count=");
    console_write_dec(count);
    console_write("\n");

    /* 暂时返回错误，表示不支持的文件描述符 */
    return 0xM4K00003;  /* M4KK1独特的不支持错误码 */
}

/**
 * 系统调用：m4k_write - 向文件描述符写入数据
 */
static uint64_t m4k_syscall_write_impl(uint64_t arg1, uint64_t arg2, uint64_t arg3,
                                       uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    uint32_t fd = (uint32_t)arg1;
    const void *buf = (const void *)arg2;
    uint64_t count = arg3;

    console_write("M4KK1 Write system call: fd=");
    console_write_dec(fd);
    console_write(", count=");
    console_write_dec(count);
    console_write("\n");

    /* 如果是标准输出（fd=1），输出到控制台 */
    if (fd == 1 && buf) {
        char *str = (char *)buf;
        uint64_t i;

        /* 输出字符串 */
        for (i = 0; i < count && str[i]; i++) {
            console_put_char(str[i]);
        }

        return i; /* 返回实际写入的字节数 */
    }

    /* 其他文件描述符暂时不支持 */
    return 0xM4K00003;  /* M4KK1独特的不支持错误码 */
}

/**
 * 初始化并注册所有M4KK1独特系统调用
 */
void m4k_syscall_init_handlers(void) {
    /* 注册基本系统调用 */
    m4k_syscall_register(M4K_SYS_EXIT, m4k_syscall_exit_impl);
    m4k_syscall_register(M4K_SYS_READ, m4k_syscall_read_impl);
    m4k_syscall_register(M4K_SYS_WRITE, m4k_syscall_write_impl);

    console_write("M4KK1 system call handlers registered\n");
}

/**
 * 获取系统调用统计信息
 */
void m4k_syscall_get_stats(uint64_t *total_calls, uint64_t *failed_calls, uint64_t *permission_denied) {
    if (total_calls) *total_calls = m4k_syscall_stats.total_calls;
    if (failed_calls) *failed_calls = m4k_syscall_stats.failed_calls;
    if (permission_denied) *permission_denied = m4k_syscall_stats.permission_denied;
}

/**
 * 打印系统调用状态信息
 */
void m4k_syscall_print_status(void) {
    uint32_t i, registered_count = 0;

    console_write("=== M4KK1 System Call Status ===\n");

    /* 统计信息 */
    console_write("Statistics:\n");
    console_write("  Total calls: ");
    console_write_dec(m4k_syscall_stats.total_calls);
    console_write("\n");

    console_write("  Failed calls: ");
    console_write_dec(m4k_syscall_stats.failed_calls);
    console_write("\n");

    console_write("  Permission denied: ");
    console_write_dec(m4k_syscall_stats.permission_denied);
    console_write("\n");

    /* 注册的系统调用 */
    console_write("Registered system calls:\n");
    for (i = 0; i < 256; i++) {
        if (m4k_syscall_table[i].registered) {
            console_write("  0x");
            console_write_hex(i);
            console_write(" - ");
            console_write(m4k_syscall_get_name(i));
            console_write(" (calls: ");
            console_write_dec(m4k_syscall_stats.calls_by_type[i]);
            console_write(")\n");
            registered_count++;
        }
    }

    console_write("Total registered system calls: ");
    console_write_dec(registered_count);
    console_write("\n");
    console_write("=================================\n");
}

/**
 * 初始化M4KK1独特系统调用系统
 */
void m4k_arch_syscall_init(void) {
    m4k_syscall_init();
    console_write("M4KK1 x86_64 system call system initialized\n");
}
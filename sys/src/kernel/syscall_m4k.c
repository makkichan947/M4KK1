/**
 * M4KK1 System Call Implementation - M4K ABI
 * 独特的M4KK1系统调用实现，使用M4K ABI确保不兼容性
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <console.h>
#include <kernel.h>
#include <process.h>
#include <m4k_syscall.h>
#include <idt.h>
#include <ldso.h>

/**
 * M4KK1独特系统调用处理函数类型
 */
typedef uint32_t (*m4k_syscall_handler_t)(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                         uint32_t arg4, uint32_t arg5);

/**
 * M4KK1独特系统调用表项
 */
typedef struct {
    m4k_syscall_handler_t handler;      /* 系统调用处理函数 */
    uint32_t permission_mask;           /* 权限掩码 */
    const char *name;                   /* 系统调用名称 */
    bool registered;                    /* 是否已注册 */
} m4k_syscall_entry_t;

/**
 * M4KK1独特系统调用表
 * 使用M4K_前缀的系统调用号
 */
static m4k_syscall_entry_t m4k_syscall_table[256];

/**
 * M4KK1独特系统调用统计信息
 */
static struct {
    uint32_t total_calls;           /* 总调用次数 */
    uint32_t failed_calls;          /* 失败调用次数 */
    uint32_t permission_denied;     /* 权限拒绝次数 */
} m4k_syscall_stats;

/**
 * M4KK1独特权限级别
 */
#define M4K_PERMISSION_KERNEL    0xFFFFFFFF  /* 内核权限 */
#define M4K_PERMISSION_USER      0x00000001  /* 用户权限 */
#define M4K_PERMISSION_SYSTEM    0x000000FF  /* 系统权限 */

/**
 * 初始化M4KK1独特系统调用表
 */
static void m4k_syscall_table_init(void) {
    memset(m4k_syscall_table, 0, sizeof(m4k_syscall_table));
    memset(&m4k_syscall_stats, 0, sizeof(m4k_syscall_stats));

    KLOG_INFO("M4KK1 system call table initialized");
}

/**
 * 检查M4KK1独特系统调用权限
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
 * 这是中断0x4D的中断处理函数（M4KK1独特的中断号）
 */
void m4k_syscall_handler(void) {
    uint32_t syscall_num;
    uint32_t result = 0xM4K00000;  /* M4KK1独特的错误码 */
    uint32_t saved_registers[6];    /* 保存的寄存器 */

    /* 获取系统调用号（从EAX，使用M4KK1独特的方式） */
    __asm__ volatile ("movl %%eax, %0" : "=r"(syscall_num));

    /* 统计总调用次数 */
    m4k_syscall_stats.total_calls++;

    KLOG_DEBUG("M4KK1 system call invoked: 0x");
    console_write_hex(syscall_num);
    console_write("\n");

    /* 检查系统调用号有效性 */
    if (syscall_num >= 256) {
        KLOG_WARN("Invalid M4KK1 system call number: 0x");
        console_write_hex(syscall_num);
        console_write("\n");
        m4k_syscall_stats.failed_calls++;
        goto m4k_syscall_return;
    }

    /* 检查系统调用是否已注册 */
    if (!m4k_syscall_table[syscall_num].registered) {
        KLOG_WARN("Unregistered M4KK1 system call: 0x");
        console_write_hex(syscall_num);
        console_write("\n");
        m4k_syscall_stats.failed_calls++;
        goto m4k_syscall_return;
    }

    /* 获取当前进程权限级别 */
    process_t *current_process = process_get_current();
    uint32_t current_permission = (current_process != NULL) ?
        M4K_PERMISSION_USER : M4K_PERMISSION_KERNEL;

    /* 检查权限 */
    if (!m4k_syscall_check_permission(syscall_num, current_permission)) {
        KLOG_WARN("Permission denied for M4KK1 system call 0x");
        console_write_hex(syscall_num);
        console_write(" (process: ");
        if (current_process) {
            console_write_hex(current_process->pid);
        } else {
            console_write("kernel");
        }
        console_write(")\n");

        m4k_syscall_stats.permission_denied++;
        result = 0xM4K00001;  /* M4KK1独特的权限拒绝码 */
        goto m4k_syscall_return;
    }

    /* 保存寄存器状态 */
    __asm__ volatile (
        "movl %%ebx, 0(%0)\n"
        "movl %%ecx, 4(%0)\n"
        "movl %%edx, 8(%0)\n"
        "movl %%esi, 12(%0)\n"
        "movl %%edi, 16(%0)\n"
        "movl %%ebp, 20(%0)\n"
        : : "r"(saved_registers) : "memory"
    );

    /* 调用系统调用处理函数 */
    m4k_syscall_handler_t handler = m4k_syscall_table[syscall_num].handler;
    if (handler != NULL) {
        /* 从寄存器获取参数 */
        uint32_t arg1, arg2, arg3, arg4, arg5;
        __asm__ volatile (
            "movl %%ebx, %0\n"
            "movl %%ecx, %1\n"
            "movl %%edx, %2\n"
            "movl %%esi, %3\n"
            "movl %%edi, %4\n"
            : "=r"(arg1), "=r"(arg2), "=r"(arg3), "=r"(arg4), "=r"(arg5)
        );

        result = handler(arg1, arg2, arg3, arg4, arg5);

        KLOG_DEBUG("M4KK1 system call 0x");
        console_write_hex(syscall_num);
        console_write(" returned: 0x");
        console_write_hex(result);
        console_write("\n");
    } else {
        KLOG_ERROR("M4KK1 system call handler is NULL for 0x");
        console_write_hex(syscall_num);
        console_write("\n");
        m4k_syscall_stats.failed_calls++;
        result = 0xM4K00002;  /* M4KK1独特的手柄为空错误码 */
    }

m4k_syscall_return:
    /* 设置返回值到EAX寄存器 */
    __asm__ volatile ("movl %0, %%eax" : : "r"(result));

    /* 恢复寄存器状态 */
    __asm__ volatile (
        "movl 0(%0), %%ebx\n"
        "movl 4(%0), %%ecx\n"
        "movl 8(%0), %%edx\n"
        "movl 12(%0), %%esi\n"
        "movl 16(%0), %%edi\n"
        "movl 20(%0), %%ebp\n"
        : : "r"(saved_registers) : "memory"
    );
}

/**
 * 初始化M4KK1独特系统调用系统
 */
void m4k_syscall_init(void) {
    /* 初始化系统调用表 */
    m4k_syscall_table_init();

    /* 注册M4KK1独特系统调用中断处理函数 */
    idt_register_handler(0x4D, m4k_syscall_handler);

    /* 初始化并注册所有系统调用处理函数 */
    m4k_syscall_init_handlers();

    KLOG_INFO("M4KK1 system call system initialized");
}

/**
 * 注册M4KK1独特系统调用处理函数
 */
void m4k_syscall_register(uint32_t num, void *handler) {
    if (num >= 256) {
        KLOG_ERROR("Invalid M4KK1 system call number for registration: 0x");
        console_write_hex(num);
        console_write("\n");
        return;
    }

    if (handler == NULL) {
        KLOG_ERROR("Cannot register NULL handler for M4KK1 system call 0x");
        console_write_hex(num);
        console_write("\n");
        return;
    }

    /* 注册处理函数 */
    m4k_syscall_table[num].handler = (m4k_syscall_handler_t)handler;
    m4k_syscall_table[num].registered = true;

    /* 设置默认权限（用户权限） */
    m4k_syscall_table[num].permission_mask = M4K_PERMISSION_USER;

    /* 设置系统调用名称（如果已知） */
    m4k_syscall_table[num].name = m4k_syscall_get_name(num);

    KLOG_INFO("M4KK1 system call 0x");
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
static uint32_t m4k_syscall_exit_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                     uint32_t arg4, uint32_t arg5) {
    uint32_t status = arg1;

    KLOG_INFO("M4KK1 process exit called with status: ");
    console_write_dec(status);
    console_write("\n");

    /* 调用进程退出函数 */
    process_exit();

    /* 不会到达这里 */
    return 0;
}

/**
 * 系统调用：m4k_read - 从文件描述符读取数据
 */
static uint32_t m4k_syscall_read_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                     uint32_t arg4, uint32_t arg5) {
    uint32_t fd = arg1;
    void *buf = (void *)arg2;
    uint32_t count = arg3;

    KLOG_DEBUG("M4KK1 Read system call: fd=");
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
static uint32_t m4k_syscall_write_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                      uint32_t arg4, uint32_t arg5) {
    uint32_t fd = arg1;
    const void *buf = (const void *)arg2;
    uint32_t count = arg3;

    KLOG_DEBUG("M4KK1 Write system call: fd=");
    console_write_dec(fd);
    console_write(", count=");
    console_write_dec(count);
    console_write("\n");

    /* 如果是标准输出（fd=1），输出到控制台 */
    if (fd == 1 && buf) {
        char *str = (char *)buf;
        uint32_t i;

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

    KLOG_INFO("M4KK1 system call handlers registered");
}
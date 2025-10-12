/**
 * M4KK1 System Call Implementation
 * 系统调用机制实现
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <console.h>
#include <kernel.h>
#include <process.h>
#include <syscall.h>
#include <idt.h>
#include <ldso.h>

/**
 * 系统调用处理函数类型
 */
typedef uint32_t (*syscall_handler_t)(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                    uint32_t arg4, uint32_t arg5);

/**
 * 系统调用表项
 */
typedef struct {
    syscall_handler_t handler;      /* 系统调用处理函数 */
    uint32_t permission_mask;       /* 权限掩码 */
    const char *name;               /* 系统调用名称 */
    bool registered;                /* 是否已注册 */
} syscall_entry_t;

/**
 * 获取系统调用名称（前向声明）
 */
const char *syscall_get_name(uint32_t num);

/**
 * 系统调用上下文
 */
typedef struct {
    uint32_t eax;                   /* 系统调用号 */
    uint32_t ebx;                   /* 参数1 */
    uint32_t ecx;                   /* 参数2 */
    uint32_t edx;                   /* 参数3 */
    uint32_t esi;                   /* 参数4 */
    uint32_t edi;                   /* 参数5 */
    uint32_t ebp;                   /* 栈基指针 */
    uint32_t esp;                   /* 栈指针 */
    uint32_t eflags;                /* 标志寄存器 */
    uint32_t cr3;                   /* 页目录基地址 */
} syscall_context_t;

/**
 * 系统调用表
 * 支持最多256个系统调用（0x00-0xFF）
 */
static syscall_entry_t syscall_table[256];

/**
 * 系统调用统计信息
 */
static struct {
    uint32_t total_calls;           /* 总调用次数 */
    uint32_t failed_calls;          /* 失败调用次数 */
    uint32_t permission_denied;     /* 权限拒绝次数 */
} syscall_stats;

/**
 * 当前进程权限级别
 */
#define PERMISSION_LEVEL_KERNEL  0xFFFFFFFF  /* 内核权限 */
#define PERMISSION_LEVEL_USER    0x00000001  /* 用户权限 */
#define PERMISSION_LEVEL_SYSTEM  0x000000FF  /* 系统权限 */

/**
 * 初始化系统调用表
 */
static void syscall_table_init(void) {
    memset(syscall_table, 0, sizeof(syscall_table));
    memset(&syscall_stats, 0, sizeof(syscall_stats));

    KLOG_INFO("System call table initialized");
}

/**
 * 检查系统调用权限
 */
static bool syscall_check_permission(uint32_t syscall_num, uint32_t current_permission) {
    if (syscall_num >= 256 || !syscall_table[syscall_num].registered) {
        return false;
    }

    /* 内核权限可以调用任何系统调用 */
    if (current_permission == PERMISSION_LEVEL_KERNEL) {
        return true;
    }

    /* 检查权限掩码 */
    return (current_permission & syscall_table[syscall_num].permission_mask) != 0;
}

/**
 * 保存寄存器状态
 */
static void syscall_save_registers(uint32_t *regs) {
    __asm__ volatile (
        "movl %%ebx, 0(%0)\n"
        "movl %%ecx, 4(%0)\n"
        "movl %%edx, 8(%0)\n"
        "movl %%esi, 12(%0)\n"
        "movl %%edi, 16(%0)\n"
        "movl %%ebp, 20(%0)\n"
        : : "r"(regs) : "memory"
    );
}

/**
 * 恢复寄存器状态
 */
static void syscall_restore_registers(uint32_t *regs) {
    __asm__ volatile (
        "movl 0(%0), %%ebx\n"
        "movl 4(%0), %%ecx\n"
        "movl 8(%0), %%edx\n"
        "movl 12(%0), %%esi\n"
        "movl 16(%0), %%edi\n"
        "movl 20(%0), %%ebp\n"
        : : "r"(regs) : "memory"
    );
}

/**
 * 系统调用处理函数
 * 这是中断0x80的中断处理函数
 */
void syscall_handler(void) {
    uint32_t syscall_num;
    uint32_t result = SYSCALL_ERROR;
    uint32_t saved_registers[6]; /* ebx, ecx, edx, esi, edi, ebp */

    /* 获取系统调用号（从EAX） */
    __asm__ volatile ("movl %%eax, %0" : "=r"(syscall_num));

    /* 统计总调用次数 */
    syscall_stats.total_calls++;

    KLOG_DEBUG("System call invoked: 0x");
    console_write_hex(syscall_num);
    console_write("\n");

    /* 检查系统调用号有效性 */
    if (syscall_num >= 256) {
        KLOG_WARN("Invalid system call number: 0x");
        console_write_hex(syscall_num);
        console_write("\n");
        syscall_stats.failed_calls++;
        goto syscall_return;
    }

    /* 检查系统调用是否已注册 */
    if (!syscall_table[syscall_num].registered) {
        KLOG_WARN("Unregistered system call: 0x");
        console_write_hex(syscall_num);
        console_write("\n");
        syscall_stats.failed_calls++;
        goto syscall_return;
    }

    /* 获取当前进程权限级别 */
    process_t *current_process = process_get_current();
    uint32_t current_permission = (current_process != NULL) ?
        PERMISSION_LEVEL_USER : PERMISSION_LEVEL_KERNEL;

    /* 检查权限 */
    if (!syscall_check_permission(syscall_num, current_permission)) {
        KLOG_WARN("Permission denied for system call 0x");
        console_write_hex(syscall_num);
        console_write(" (process: ");
        if (current_process) {
            console_write_hex(current_process->pid);
        } else {
            console_write("kernel");
        }
        console_write(")\n");

        syscall_stats.permission_denied++;
        result = SYSCALL_ERROR;
        goto syscall_return;
    }

    /* 保存寄存器状态 */
    syscall_save_registers(saved_registers);

    /* 调用系统调用处理函数 */
    syscall_handler_t handler = syscall_table[syscall_num].handler;
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

        KLOG_DEBUG("System call 0x");
        console_write_hex(syscall_num);
        console_write(" returned: 0x");
        console_write_hex(result);
        console_write("\n");
    } else {
        KLOG_ERROR("System call handler is NULL for 0x");
        console_write_hex(syscall_num);
        console_write("\n");
        syscall_stats.failed_calls++;
        result = SYSCALL_ERROR;
    }

syscall_return:
    /* 设置返回值到EAX寄存器 */
    __asm__ volatile ("movl %0, %%eax" : : "r"(result));

    /* 恢复寄存器状态 */
    syscall_restore_registers(saved_registers);
}

/**
 * 初始化并注册所有系统调用
 */
void syscall_init_handlers(void);

/**
 * 初始化系统调用系统
 */
void syscall_init(void) {
    /* 初始化系统调用表 */
    syscall_table_init();

    /* 注册系统调用中断处理函数 */
    idt_register_handler(0x80, syscall_handler);

    /* 初始化并注册所有系统调用处理函数 */
    syscall_init_handlers();

    KLOG_INFO("System call system initialized");
}

/**
 * 注册系统调用处理函数
 */
void syscall_register(uint32_t num, void *handler) {
    if (num >= 256) {
        KLOG_ERROR("Invalid system call number for registration: 0x");
        console_write_hex(num);
        console_write("\n");
        return;
    }

    if (handler == NULL) {
        KLOG_ERROR("Cannot register NULL handler for system call 0x");
        console_write_hex(num);
        console_write("\n");
        return;
    }

    /* 注册处理函数 */
    syscall_table[num].handler = (syscall_handler_t)handler;
    syscall_table[num].registered = true;

    /* 设置默认权限（用户权限） */
    syscall_table[num].permission_mask = PERMISSION_LEVEL_USER;

    /* 设置系统调用名称（如果已知） */
    syscall_table[num].name = syscall_get_name(num);

    KLOG_INFO("System call 0x");
    console_write_hex(num);
    console_write(" registered: ");
    console_write(syscall_table[num].name ? syscall_table[num].name : "Unknown");
    console_write("\n");
}

/**
 * 注销系统调用处理函数
 */
void syscall_unregister(uint32_t num) {
    if (num >= 256) {
        KLOG_ERROR("Invalid system call number for unregistration: 0x");
        console_write_hex(num);
        console_write("\n");
        return;
    }

    if (!syscall_table[num].registered) {
        KLOG_WARN("System call 0x");
        console_write_hex(num);
        console_write(" is not registered\n");
        return;
    }

    /* 注销处理函数 */
    syscall_table[num].handler = NULL;
    syscall_table[num].registered = false;
    syscall_table[num].name = NULL;
    syscall_table[num].permission_mask = 0;

    KLOG_INFO("System call 0x");
    console_write_hex(num);
    console_write(" unregistered\n");
}

/**
 * 执行系统调用
 */
uint32_t syscall_execute(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3,
                        uint32_t arg4, uint32_t arg5) {
    uint32_t result;

    /* 检查系统调用是否已注册 */
    if (num >= 256 || !syscall_table[num].registered) {
        KLOG_ERROR("Cannot execute unregistered system call: 0x");
        console_write_hex(num);
        console_write("\n");
        return SYSCALL_ERROR;
    }

    /* 通过中断调用系统调用 */
    __asm__ volatile (
        "int $0x80\n"
        : "=a"(result)
        : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4), "D"(arg5)
    );

    return result;
}

/**
 * 获取系统调用名称
 */
const char *syscall_get_name(uint32_t num) {
    switch (num) {
        case SYSCALL_EXIT: return "exit";
        case SYSCALL_FORK: return "fork";
        case SYSCALL_READ: return "read";
        case SYSCALL_WRITE: return "write";
        case SYSCALL_OPEN: return "open";
        case SYSCALL_CLOSE: return "close";
        case SYSCALL_WAITPID: return "waitpid";
        case SYSCALL_EXECVE: return "execve";
        case SYSCALL_GETPID: return "getpid";
        case SYSCALL_GETPPID: return "getppid";
        case SYSCALL_BRK: return "brk";
        case SYSCALL_MMAP: return "mmap";
        case SYSCALL_MUNMAP: return "munmap";
        case SYSCALL_MPROTECT: return "mprotect";
        case SYSCALL_MSYNC: return "msync";
        case SYSCALL_GETCWD: return "getcwd";
        case SYSCALL_CHDIR: return "chdir";
        case SYSCALL_MKDIR: return "mkdir";
        case SYSCALL_RMDIR: return "rmdir";
        case SYSCALL_LINK: return "link";
        case SYSCALL_UNLINK: return "unlink";
        case SYSCALL_RENAME: return "rename";
        case SYSCALL_STAT: return "stat";
        case SYSCALL_FSTAT: return "fstat";
        case SYSCALL_LSTAT: return "lstat";
        case SYSCALL_ACCESS: return "access";
        case SYSCALL_CHMOD: return "chmod";
        case SYSCALL_CHOWN: return "chown";
        case SYSCALL_UTIME: return "utime";
        case SYSCALL_TIME: return "time";
        case SYSCALL_TIMES: return "times";
        case SYSCALL_GETUID: return "getuid";
        case SYSCALL_GETGID: return "getgid";
        case SYSCALL_SETUID: return "setuid";
        case SYSCALL_SETGID: return "setgid";
        case SYSCALL_GETEUID: return "geteuid";
        case SYSCALL_GETEGID: return "getegid";
        case SYSCALL_SETEUID: return "seteuid";
        case SYSCALL_SETEGID: return "setegid";
        case SYSCALL_PIPE: return "pipe";
        case SYSCALL_DUP: return "dup";
        case SYSCALL_DUP2: return "dup2";
        case SYSCALL_SELECT: return "select";
        case SYSCALL_POLL: return "poll";
        case SYSCALL_EPOLL_CREATE: return "epoll_create";
        case SYSCALL_EPOLL_CTL: return "epoll_ctl";
        case SYSCALL_EPOLL_WAIT: return "epoll_wait";
        case SYSCALL_SOCKET: return "socket";
        case SYSCALL_BIND: return "bind";
        case SYSCALL_LISTEN: return "listen";
        case SYSCALL_ACCEPT: return "accept";
        case SYSCALL_CONNECT: return "connect";
        case SYSCALL_SEND: return "send";
        case SYSCALL_RECV: return "recv";
        case SYSCALL_SENDTO: return "sendto";
        case SYSCALL_RECVFROM: return "recvfrom";
        case SYSCALL_SHUTDOWN: return "shutdown";
        case SYSCALL_SETSOCKOPT: return "setsockopt";
        case SYSCALL_GETSOCKOPT: return "getsockopt";
        case SYSCALL_IOCTL: return "ioctl";
        case SYSCALL_FCNTL: return "fcntl";
        case SYSCALL_READDIR: return "readdir";
        case SYSCALL_TELLDIR: return "telldir";
        case SYSCALL_SEEKDIR: return "seekdir";
        case SYSCALL_CLOSEDIR: return "closedir";
        case SYSCALL_OPENDIR: return "opendir";
        case SYSCALL_MKNOD: return "mknod";
        case SYSCALL_MKFIFO: return "mkfifo";
        case SYSCALL_TRUNCATE: return "truncate";
        case SYSCALL_FTRUNCATE: return "ftruncate";
        case SYSCALL_GETDENTS: return "getdents";
        case SYSCALL_SYNC: return "sync";
        case SYSCALL_FSYNC: return "fsync";
        case SYSCALL_FDATASYNC: return "fdatasync";
        case SYSCALL_MLOCK: return "mlock";
        case SYSCALL_MUNLOCK: return "munlock";
        case SYSCALL_MLOCKALL: return "mlockall";
        case SYSCALL_MUNLOCKALL: return "munlockall";
        case SYSCALL_NANOSLEEP: return "nanosleep";
        case SYSCALL_CLOCK_GETTIME: return "clock_gettime";
        case SYSCALL_CLOCK_SETTIME: return "clock_settime";
        case SYSCALL_CLOCK_GETRES: return "clock_getres";
        case SYSCALL_SCHED_YIELD: return "sched_yield";
        case SYSCALL_SCHED_SETSCHEDULER: return "sched_setscheduler";
        case SYSCALL_SCHED_GETSCHEDULER: return "sched_getscheduler";
        case SYSCALL_SCHED_SETPARAM: return "sched_setparam";
        case SYSCALL_SCHED_GETPARAM: return "sched_getparam";
        case SYSCALL_SCHED_SETAFFINITY: return "sched_setaffinity";
        case SYSCALL_SCHED_GETAFFINITY: return "sched_getaffinity";
        case SYSCALL_PRLIMIT64: return "prlimit64";
        case SYSCALL_GETRUSAGE: return "getrusage";
        case SYSCALL_GETTIMEOFDAY: return "gettimeofday";
        case SYSCALL_SETTIMEOFDAY: return "settimeofday";
        case SYSCALL_ADJTIMEX: return "adjtimex";
        case SYSCALL_TIMER_CREATE: return "timer_create";
        case SYSCALL_TIMER_DELETE: return "timer_delete";
        case SYSCALL_TIMER_SETTIME: return "timer_settime";
        case SYSCALL_TIMER_GETTIME: return "timer_gettime";
        case SYSCALL_TIMER_GETOVERRUN: return "timer_getoverrun";
        case SYSCALL_KILL: return "kill";
        case SYSCALL_TKILL: return "tkill";
        case SYSCALL_TGKILL: return "tgkill";
        case SYSCALL_SIGACTION: return "sigaction";
        case SYSCALL_SIGPROCMASK: return "sigprocmask";
        case SYSCALL_SIGPENDING: return "sigpending";
        case SYSCALL_SIGSUSPEND: return "sigsuspend";
        case SYSCALL_SIGTIMEDWAIT: return "sigtimedwait";
        case SYSCALL_SIGRETURN: return "sigreturn";
        case SYSCALL_REBOOT: return "reboot";
        case SYSCALL_KEXEC_LOAD: return "kexec_load";
        case SYSCALL_EXIT_GROUP: return "exit_group";
        case SYSCALL_WAIT4: return "wait4";
        case SYSCALL_CLONE: return "clone";
        case SYSCALL_VFORK: return "vfork";
        case SYSCALL_UNAME: return "uname";
        case SYSCALL_SEMGET: return "semget";
        case SYSCALL_SEMOP: return "semop";
        case SYSCALL_SEMCTL: return "semctl";
        case SYSCALL_SEMTIMEDOP: return "semtimedop";
        case SYSCALL_MSGGET: return "msgget";
        case SYSCALL_MSGSND: return "msgsnd";
        case SYSCALL_MSGRCV: return "msgrcv";
        case SYSCALL_MSGCTL: return "msgctl";
        case SYSCALL_SHMGET: return "shmget";
        case SYSCALL_SHMAT: return "shmat";
        case SYSCALL_SHMDT: return "shmdt";
        case SYSCALL_SHMCTL: return "shmctl";
        case SYSCALL_DL_LOAD_LIBRARY: return "dl_load_library";
        case SYSCALL_DL_UNLOAD_LIBRARY: return "dl_unload_library";
        case SYSCALL_DL_FIND_SYMBOL: return "dl_find_symbol";
        case SYSCALL_DL_GET_ERROR: return "dl_get_error";
        default: return "unknown";
    }
}

/**
 * 获取系统调用统计信息
 */
void syscall_get_stats(uint32_t *total_calls, uint32_t *failed_calls, uint32_t *permission_denied) {
    if (total_calls) *total_calls = syscall_stats.total_calls;
    if (failed_calls) *failed_calls = syscall_stats.failed_calls;
    if (permission_denied) *permission_denied = syscall_stats.permission_denied;
}

/**
 * 设置系统调用权限掩码
 */
void syscall_set_permission(uint32_t num, uint32_t permission_mask) {
    if (num >= 256) {
        KLOG_ERROR("Invalid system call number for permission setting: 0x");
        console_write_hex(num);
        console_write("\n");
        return;
    }

    if (!syscall_table[num].registered) {
        KLOG_WARN("Setting permission for unregistered system call 0x");
        console_write_hex(num);
        console_write("\n");
    }

    syscall_table[num].permission_mask = permission_mask;

    KLOG_INFO("Permission mask set for system call 0x");
    console_write_hex(num);
    console_write(" to 0x");
    console_write_hex(permission_mask);
    console_write("\n");
}

/**
 * 检查系统调用是否已注册
 */
bool syscall_is_registered(uint32_t num) {
    return (num < 256) && syscall_table[num].registered;
}

/**
 * 获取系统调用处理函数
 */
syscall_handler_t syscall_get_handler(uint32_t num) {
    if (num >= 256 || !syscall_table[num].registered) {
        return NULL;
    }
    return syscall_table[num].handler;
}

/**
 * 打印系统调用状态信息
 */
void syscall_print_status(void) {
    uint32_t i, registered_count = 0;

    KLOG_INFO("=== System Call Status ===");

    /* 统计信息 */
    KLOG_INFO("Statistics:");
    KLOG_INFO("  Total calls: ");
    console_write_dec(syscall_stats.total_calls);
    console_write("\n");

    KLOG_INFO("  Failed calls: ");
    console_write_dec(syscall_stats.failed_calls);
    console_write("\n");

    KLOG_INFO("  Permission denied: ");
    console_write_dec(syscall_stats.permission_denied);
    console_write("\n");

    /* 注册的系统调用 */
    KLOG_INFO("Registered system calls:");
    for (i = 0; i < 256; i++) {
        if (syscall_table[i].registered) {
            KLOG_INFO("  0x");
            console_write_hex(i);
            console_write(" - ");
            console_write(syscall_get_name(i));
            console_write(" (handler: 0x");
            console_write_hex((uint32_t)syscall_table[i].handler);
            console_write(")\n");
            registered_count++;
        }
    }

    KLOG_INFO("Total registered system calls: ");
    console_write_dec(registered_count);
    console_write("\n");
    KLOG_INFO("=========================");
}

/* ====================================================================
   系统调用实现函数
   ==================================================================== */

/**
 * 系统调用：exit - 退出当前进程
 */
static uint32_t syscall_exit_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                 uint32_t arg4, uint32_t arg5) {
    uint32_t status = arg1;

    KLOG_INFO("Process exit called with status: ");
    console_write_dec(status);
    console_write("\n");

    /* 调用进程退出函数 */
    process_exit();

    /* 不会到达这里 */
    return 0;
}

/**
 * 系统调用：fork - 创建子进程
 */
static uint32_t syscall_fork_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                 uint32_t arg4, uint32_t arg5) {
    process_t *child_process;

    KLOG_INFO("Fork system call invoked\n");

    /* 创建子进程 */
    child_process = process_create("child", PROCESS_PRIORITY_NORMAL);
    if (!child_process) {
        KLOG_ERROR("Failed to create child process in fork\n");
        return SYSCALL_ERROR;
    }

    /* 在子进程中返回0，在父进程中返回子进程PID */
    if (child_process->pid != process_get_current()->pid) {
        /* 这是子进程 */
        return 0;
    } else {
        /* 这是父进程 */
        return child_process->pid;
    }
}

/**
 * 系统调用：getpid - 获取当前进程ID
 */
static uint32_t syscall_getpid_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                   uint32_t arg4, uint32_t arg5) {
    uint32_t pid = process_get_pid();

    KLOG_DEBUG("GetPID system call: returning ");
    console_write_dec(pid);
    console_write("\n");

    return pid;
}

/**
 * 系统调用：getppid - 获取父进程ID
 */
static uint32_t syscall_getppid_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                    uint32_t arg4, uint32_t arg5) {
    uint32_t ppid = process_get_ppid();

    KLOG_DEBUG("GetPPID system call: returning ");
    console_write_dec(ppid);
    console_write("\n");

    return ppid;
}

/**
 * 系统调用：read - 从文件描述符读取数据
 */
static uint32_t syscall_read_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                 uint32_t arg4, uint32_t arg5) {
    uint32_t fd = arg1;
    void *buf = (void *)arg2;
    uint32_t count = arg3;

    KLOG_DEBUG("Read system call: fd=");
    console_write_dec(fd);
    console_write(", count=");
    console_write_dec(count);
    console_write("\n");

    /* 暂时返回错误，表示不支持的文件描述符 */
    return SYSCALL_ERROR;
}

/**
 * 系统调用：write - 向文件描述符写入数据
 */
static uint32_t syscall_write_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                  uint32_t arg4, uint32_t arg5) {
    uint32_t fd = arg1;
    const void *buf = (const void *)arg2;
    uint32_t count = arg3;

    KLOG_DEBUG("Write system call: fd=");
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
    return SYSCALL_ERROR;
}

/**
 * 系统调用：open - 打开文件
 */
static uint32_t syscall_open_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                 uint32_t arg4, uint32_t arg5) {
    const char *pathname = (const char *)arg1;
    uint32_t flags = arg2;

    KLOG_DEBUG("Open system call: pathname=");
    if (pathname) {
        console_write(pathname);
    } else {
        console_write("(null)");
    }
    console_write(", flags=");
    console_write_hex(flags);
    console_write("\n");

    /* 暂时不支持，返回错误 */
    return SYSCALL_ERROR;
}

/**
 * 系统调用：close - 关闭文件描述符
 */
static uint32_t syscall_close_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                  uint32_t arg4, uint32_t arg5) {
    uint32_t fd = arg1;

    KLOG_DEBUG("Close system call: fd=");
    console_write_dec(fd);
    console_write("\n");

    /* 暂时不支持，返回错误 */
    return SYSCALL_ERROR;
}

/**
 * 系统调用：execve - 执行程序
 */
static uint32_t syscall_execve_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                   uint32_t arg4, uint32_t arg5) {
    const char *filename = (const char *)arg1;
    char *const *argv = (char *const *)arg2;
    char *const *envp = (char *const *)arg3;

    KLOG_DEBUG("Execve system call: filename=");
    if (filename) {
        console_write(filename);
    } else {
        console_write("(null)");
    }
    console_write("\n");

    /* 暂时不支持，返回错误 */
    return SYSCALL_ERROR;
}

/**
 * 系统调用：waitpid - 等待子进程
 */
static uint32_t syscall_waitpid_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                    uint32_t arg4, uint32_t arg5) {
    uint32_t pid = arg1;
    void *status = (void *)arg2;
    uint32_t options = arg3;

    KLOG_DEBUG("Waitpid system call: pid=");
    console_write_dec(pid);
    console_write(", options=");
    console_write_hex(options);
    console_write("\n");

    /* 暂时不支持，返回错误 */
    return SYSCALL_ERROR;
}

/**
 * 系统调用：brk - 设置程序中断点
 */
static uint32_t syscall_brk_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                uint32_t arg4, uint32_t arg5) {
    uint32_t addr = arg1;

    KLOG_DEBUG("Brk system call: addr=0x");
    console_write_hex(addr);
    console_write("\n");

    /* 暂时不支持，返回错误 */
    return SYSCALL_ERROR;
}

/**
 * 系统调用：getcwd - 获取当前工作目录
 */
static uint32_t syscall_getcwd_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                   uint32_t arg4, uint32_t arg5) {
    char *buf = (char *)arg1;
    uint32_t size = arg2;

    KLOG_DEBUG("Getcwd system call: buf=0x");
    console_write_hex((uint32_t)buf);
    console_write(", size=");
    console_write_dec(size);
    console_write("\n");

    /* 暂时不支持，返回错误 */
    return SYSCALL_ERROR;
}

/**
 * 系统调用：chdir - 改变当前工作目录
 */
static uint32_t syscall_chdir_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                  uint32_t arg4, uint32_t arg5) {
    const char *path = (const char *)arg1;

    KLOG_DEBUG("Chdir system call: path=");
    if (path) {
        console_write(path);
    } else {
        console_write("(null)");
    }
    console_write("\n");

    /* 暂时不支持，返回错误 */
    return SYSCALL_ERROR;
}

/**
 * 系统调用：time - 获取时间
 */
static uint32_t syscall_time_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                 uint32_t arg4, uint32_t arg5) {
    void *tloc = (void *)arg1;

    KLOG_DEBUG("Time system call\n");

    /* 返回模拟的时间戳 */
    uint32_t current_time = 1234567890; /* 模拟时间戳 */

    if (tloc) {
        *(uint32_t *)tloc = current_time;
    }

    return current_time;
}

/**
 * 系统调用：uname - 获取系统信息
 */
static uint32_t syscall_uname_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                  uint32_t arg4, uint32_t arg5) {
    void *buf = (void *)arg1;

    KLOG_DEBUG("Uname system call\n");

    /* 暂时不支持，返回错误 */
    return SYSCALL_ERROR;
}

/**
 * 系统调用：reboot - 重启系统
 */
static uint32_t syscall_reboot_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                   uint32_t arg4, uint32_t arg5) {
    uint32_t magic1 = arg1;
    uint32_t magic2 = arg2;
    uint32_t cmd = arg3;

    KLOG_INFO("Reboot system call: magic1=0x");
    console_write_hex(magic1);
    console_write(", magic2=0x");
    console_write_hex(magic2);
    console_write(", cmd=");
    console_write_dec(cmd);
    console_write("\n");

    /* 检查魔数 */
    if (magic1 == 0x01234567 && magic2 == 0x89ABCDEF) {
        KLOG_INFO("Rebooting system...\n");

        /* 执行重启 */
        __asm__ volatile (
            "movl $0x64, %eax\n"  /* 魔数 */
            "outb %al, $0xFE\n"   /* QEMU重启端口 */
        );

        /* 如果上面的方法不起作用，尝试键盘控制器重启 */
        __asm__ volatile (
            "movb $0x02, %al\n"
            "outb %al, $0x64\n"
            "movb $0xFE, %al\n"
            "outb %al, $0x64\n"
        );

        return 0;
    }

    return SYSCALL_ERROR;
}

/**
 * 初始化并注册所有系统调用
 */
void syscall_init_handlers(void) {
    /* 注册基本系统调用 */
    syscall_register(SYSCALL_EXIT, syscall_exit_impl);
    syscall_register(SYSCALL_FORK, syscall_fork_impl);
    syscall_register(SYSCALL_READ, syscall_read_impl);
    syscall_register(SYSCALL_WRITE, syscall_write_impl);
    syscall_register(SYSCALL_OPEN, syscall_open_impl);
    syscall_register(SYSCALL_CLOSE, syscall_close_impl);
    syscall_register(SYSCALL_WAITPID, syscall_waitpid_impl);
    syscall_register(SYSCALL_EXECVE, syscall_execve_impl);
    syscall_register(SYSCALL_GETPID, syscall_getpid_impl);
    syscall_register(SYSCALL_GETPPID, syscall_getppid_impl);
    syscall_register(SYSCALL_BRK, syscall_brk_impl);
    syscall_register(SYSCALL_GETCWD, syscall_getcwd_impl);
    syscall_register(SYSCALL_CHDIR, syscall_chdir_impl);
    syscall_register(SYSCALL_TIME, syscall_time_impl);
    syscall_register(SYSCALL_UNAME, syscall_uname_impl);
    syscall_register(SYSCALL_REBOOT, syscall_reboot_impl);

    /* 注册动态链接器相关系统调用 */
    /* TODO: 实现动态链接器系统调用 */
    /* syscall_register(SYSCALL_DL_LOAD_LIBRARY, syscall_dl_load_library_impl); */
    /* syscall_register(SYSCALL_DL_UNLOAD_LIBRARY, syscall_dl_unload_library_impl); */
    /* syscall_register(SYSCALL_DL_FIND_SYMBOL, syscall_dl_find_symbol_impl); */
    /* syscall_register(SYSCALL_DL_GET_ERROR, syscall_dl_get_error_impl); */

    KLOG_INFO("System call handlers registered");
}

/* ====================================================================
    动态链接器系统调用实现函数
    ==================================================================== */

/**
 * 系统调用：dl_load_library - 加载动态库
 */
static uint32_t syscall_dl_load_library_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                           uint32_t arg4, uint32_t arg5) {
    const char *filename = (const char *)arg1;

    KLOG_DEBUG("DL Load Library system call");
    if (filename) {
        console_write(filename);
    } else {
        console_write("(null)");
    }
    console_write("\n");

    if (!filename) {
        return SYSCALL_ERROR;
    }

    /* 调用动态链接器加载库 */
    m4ll_library_t *lib;
    if (m4ll_load_library(filename, &lib) < 0) {
        KLOG_ERROR("Failed to load library");
        return SYSCALL_ERROR;
    }

    /* 返回库句柄（这里暂时用库地址作为句柄） */
    return (uint32_t)lib;
}

/**
 * 系统调用：dl_unload_library - 卸载动态库
 */
static uint32_t syscall_dl_unload_library_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                             uint32_t arg4, uint32_t arg5) {
    m4ll_library_t *lib = (m4ll_library_t *)arg1;

    KLOG_DEBUG("DL Unload Library system call");
    console_write_hex((uint32_t)lib);
    console_write("\n");

    if (!lib) {
        return SYSCALL_ERROR;
    }

    /* 调用动态链接器卸载库 */
    if (m4ll_unload_library(lib) < 0) {
        KLOG_ERROR("Failed to unload library");
        return SYSCALL_ERROR;
    }

    return SYSCALL_SUCCESS;
}

/**
 * 系统调用：dl_find_symbol - 查找符号
 */
static uint32_t syscall_dl_find_symbol_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                          uint32_t arg4, uint32_t arg5) {
    const char *symbol = (const char *)arg1;

    KLOG_DEBUG("DL Find Symbol system call");
    if (symbol) {
        console_write(symbol);
    } else {
        console_write("(null)");
    }
    console_write("\n");

    if (!symbol) {
        return SYSCALL_ERROR;
    }

    /* 调用动态链接器查找符号 */
    void *address = m4ll_find_symbol(symbol);
    if (!address) {
        KLOG_DEBUG("Symbol not found");
        return SYSCALL_ERROR;
    }

    return (uint32_t)address;
}

/**
 * 系统调用：dl_get_error - 获取错误信息
 */
static uint32_t syscall_dl_get_error_impl(uint32_t arg1, uint32_t arg2, uint32_t arg3,
                                        uint32_t arg4, uint32_t arg5) {
    char *buf = (char *)arg1;
    uint32_t size = arg2;

    KLOG_DEBUG("DL Get Error system call");
    console_write_hex((uint32_t)buf);
    console_write(", size=");
    console_write_dec(size);
    console_write("\n");

    if (!buf || size == 0) {
        return SYSCALL_ERROR;
    }

    /* 复制错误信息到用户缓冲区 */
    size_t error_len = strlen(m4ll_error_msg);
    if (error_len >= size) {
        error_len = size - 1;
    }

    memcpy(buf, m4ll_error_msg, error_len);
    buf[error_len] = '\0';

    return error_len;
}
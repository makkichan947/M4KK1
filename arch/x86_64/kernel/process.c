/**
 * M4KK1 x86_64 Architecture - Process Management Implementation
 * x86_64架构进程管理实现 - 独特的M4KK1进程管理
 *
 * 文件: process.c
 * 作者: M4KK1 Development Team
 * 版本: v0.2.0-multarch
 * 日期: 2025-10-16
 *
 * 描述:
 *   实现x86_64架构的完整进程管理功能，包括：
 *   - 进程创建和销毁
 *   - 进程上下文切换
 *   - 进程调度和优先级管理
 *   - 进程间通信机制
 *   - 内存保护和隔离
 *   - 进程调试和监控
 *
 * 架构特性:
 *   - 64位进程上下文，支持完整寄存器状态
 *   - 多级优先级调度算法
 *   - 基于消息的进程间通信
 *   - 内存地址空间隔离
 *   - 进程生命周期管理
 *   - 性能监控和统计
 *
 * 进程状态:
 *   - M4K_PROCESS_RUNNING: 正在运行
 *   - M4K_PROCESS_READY: 准备就绪
 *   - M4K_PROCESS_BLOCKED: 阻塞等待
 *   - M4K_PROCESS_SLEEPING: 睡眠状态
 *   - M4K_PROCESS_ZOMBIE: 僵尸进程
 *   - M4K_PROCESS_STOPPED: 停止状态
 *
 * 调度策略:
 *   - 优先级调度 (Priority-based)
 *   - 时间片轮转 (Round-robin)
 *   - 实时调度 (Real-time)
 *   - 公平调度 (Fair scheduling)
 *
 * 修改历史:
 *   v0.2.0: 实现完整的x86_64进程管理
 *   v0.1.0: 初始进程管理实现
 *
 * 依赖:
 *   - m4k_arch.h: 架构特定定义
 *   - process.h: 通用进程管理接口
 *   - memory.h: 内存管理接口
 *   - console.h: 控制台输出
 *   - string.h: 字符串操作
 *   - stdlib.h: 标准库函数
 */

#include "../../../include/m4k_arch.h"
#include "../../../include/process.h"
#include "../../../include/memory.h"
#include "../../../include/console.h"
#include "../../../include/string.h"

/* 进程控制块 */
typedef struct {
    uint64_t pid;                   /* 进程ID */
    uint64_t ppid;                  /* 父进程ID */
    uint32_t state;                 /* 进程状态 */
    uint32_t priority;              /* 优先级 */
    uint64_t time_slice;            /* 时间片 */
    uint64_t cpu_time;              /* CPU使用时间 */
    uint64_t context_switches;      /* 上下文切换次数 */

    /* 内存管理 */
    uint64_t cr3;                   /* 页表基地址 */
    uint64_t stack_base;            /* 栈基地址 */
    uint64_t stack_size;            /* 栈大小 */
    uint64_t heap_base;             /* 堆基地址 */
    uint64_t heap_size;             /* 堆大小 */

    /* 上下文信息 */
    uint64_t rip;                   /* 指令指针 */
    uint64_t rsp;                   /* 栈指针 */
    uint64_t rbp;                   /* 基指针 */
    uint64_t rax, rbx, rcx, rdx;   /* 通用寄存器 */
    uint64_t rsi, rdi;              /* 索引寄存器 */
    uint64_t r8, r9, r10, r11;     /* 扩展寄存器 */
    uint64_t r12, r13, r14, r15;   /* 扩展寄存器 */
    uint64_t rflags;                /* 标志寄存器 */
    uint64_t cs, ds, es, fs, gs, ss; /* 段寄存器 */

    /* 进程信息 */
    char name[32];                  /* 进程名称 */
    uint64_t create_time;           /* 创建时间 */
    uint64_t start_time;            /* 开始时间 */
    uint64_t exit_time;             /* 退出时间 */
    uint32_t exit_code;             /* 退出代码 */

    /* 进程关系 */
    struct m4k_pcb *parent;         /* 父进程 */
    struct m4k_pcb *children;       /* 子进程列表 */
    struct m4k_pcb *next;           /* 兄弟进程 */

    /* 等待队列 */
    struct m4k_pcb *wait_queue;     /* 等待队列 */

    /* 消息传递 */
    void *message_queue;            /* 消息队列 */
    uint32_t message_count;         /* 消息数量 */

    /* 信号处理 */
    uint64_t signal_mask;           /* 信号掩码 */
    uint64_t signal_pending;        /* 待处理信号 */
    void *signal_handlers[64];      /* 信号处理函数 */

    /* 文件描述符 */
    void *file_table;               /* 文件描述符表 */
    uint32_t file_count;            /* 文件数量 */

    /* 线程信息 */
    uint32_t thread_count;          /* 线程数量 */
    void *thread_list;              /* 线程列表 */

    /* 统计信息 */
    uint64_t page_faults;           /* 页面错误次数 */
    uint64_t syscalls;              /* 系统调用次数 */
    uint64_t bytes_read;            /* 读取字节数 */
    uint64_t bytes_written;         /* 写入字节数 */
    uint64_t memory_usage;          /* 内存使用量 */
} m4k_pcb_t;

/* 进程管理全局变量 */
static m4k_pcb_t *current_process = NULL;
static m4k_pcb_t *process_list = NULL;
static uint64_t next_pid = 1;
static uint64_t process_count = 0;
static uint32_t scheduler_enabled = 0;

/* 调度器统计 */
static struct {
    uint64_t context_switches;
    uint64_t timer_interrupts;
    uint64_t schedule_calls;
    uint64_t idle_time;
} scheduler_stats;

/* 进程状态定义 */
#define M4K_PROCESS_RUNNING     0
#define M4K_PROCESS_READY       1
#define M4K_PROCESS_BLOCKED     2
#define M4K_PROCESS_SLEEPING    3
#define M4K_PROCESS_ZOMBIE      4
#define M4K_PROCESS_STOPPED     5

/* 优先级定义 */
#define M4K_PRIORITY_HIGHEST    0
#define M4K_PRIORITY_HIGH       1
#define M4K_PRIORITY_NORMAL     2
#define M4K_PRIORITY_LOW        3
#define M4K_PRIORITY_LOWEST     4

/* 时间片定义 */
#define M4K_TIME_SLICE_HIGHEST  20
#define M4K_TIME_SLICE_HIGH     15
#define M4K_TIME_SLICE_NORMAL   10
#define M4K_TIME_SLICE_LOW      5
#define M4K_TIME_SLICE_LOWEST   2

/**
 * 初始化进程管理
 */
void m4k_process_init(void) {
    /* 初始化进程列表 */
    process_list = NULL;
    current_process = NULL;
    next_pid = 1;
    process_count = 0;
    scheduler_enabled = 0;

    /* 初始化调度器统计 */
    memset(&scheduler_stats, 0, sizeof(scheduler_stats));

    console_write("M4KK1 x86_64 process management initialized\n");
}

/**
 * 创建新进程
 */
m4k_pcb_t *m4k_process_create(const char *name, uint32_t priority) {
    m4k_pcb_t *pcb;
    uint64_t stack_base;
    uint64_t cr3;

    /* 分配PCB */
    pcb = (m4k_pcb_t *)kmalloc(sizeof(m4k_pcb_t));
    if (!pcb) {
        return NULL;
    }

    /* 初始化PCB */
    memset(pcb, 0, sizeof(m4k_pcb_t));
    pcb->pid = next_pid++;
    pcb->state = M4K_PROCESS_READY;
    pcb->priority = priority;
    pcb->time_slice = get_time_slice(priority);

    /* 设置进程名称 */
    if (name) {
        strncpy(pcb->name, name, sizeof(pcb->name) - 1);
    } else {
        strcpy(pcb->name, "unnamed");
    }

    /* 分配内核栈 */
    stack_base = (uint64_t)kmalloc(16384); /* 16KB栈 */
    if (!stack_base) {
        kfree(pcb);
        return NULL;
    }
    pcb->stack_base = stack_base;
    pcb->stack_size = 16384;
    pcb->rsp = stack_base + 16384 - 16; /* 栈顶 */

    /* 分配页表 */
    cr3 = (uint64_t)kmalloc(4096);
    if (!cr3) {
        kfree((void *)stack_base);
        kfree(pcb);
        return NULL;
    }
    pcb->cr3 = cr3;

    /* 复制内核页表 */
    memcpy((void *)cr3, (void *)read_cr3(), 4096);

    /* 设置进程时间 */
    pcb->create_time = get_timestamp();

    /* 添加到进程列表 */
    pcb->next = process_list;
    process_list = pcb;
    process_count++;

    console_write("Created process: ");
    console_write(pcb->name);
    console_write(" (PID: ");
    console_write_dec(pcb->pid);
    console_write(")\n");

    return pcb;
}

/**
 * 销毁进程
 */
void m4k_process_destroy(m4k_pcb_t *pcb) {
    if (!pcb) return;

    /* 从进程列表中移除 */
    if (process_list == pcb) {
        process_list = pcb->next;
    } else {
        m4k_pcb_t *prev = process_list;
        while (prev && prev->next != pcb) {
            prev = prev->next;
        }
        if (prev) {
            prev->next = pcb->next;
        }
    }

    /* 释放资源 */
    if (pcb->stack_base) {
        kfree((void *)pcb->stack_base);
    }
    if (pcb->cr3) {
        kfree((void *)pcb->cr3);
    }

    /* 递归销毁子进程 */
    while (pcb->children) {
        m4k_pcb_t *child = pcb->children;
        pcb->children = child->next;
        m4k_process_destroy(child);
    }

    /* 释放PCB */
    kfree(pcb);
    process_count--;

    console_write("Destroyed process (PID: ");
    console_write_dec(pcb->pid);
    console_write(")\n");
}

/**
 * 获取当前进程
 */
m4k_pcb_t *m4k_process_get_current(void) {
    return current_process;
}

/**
 * 设置当前进程
 */
void m4k_process_set_current(m4k_pcb_t *pcb) {
    current_process = pcb;
}

/**
 * 获取进程ID
 */
uint64_t m4k_process_get_pid(void) {
    return current_process ? current_process->pid : 0;
}

/**
 * 获取父进程ID
 */
uint64_t m4k_process_get_ppid(void) {
    return current_process && current_process->parent ?
           current_process->parent->pid : 0;
}

/**
 * 获取进程数量
 */
uint64_t m4k_process_get_count(void) {
    return process_count;
}

/**
 * 根据PID查找进程
 */
m4k_pcb_t *m4k_process_find(uint64_t pid) {
    m4k_pcb_t *pcb = process_list;

    while (pcb) {
        if (pcb->pid == pid) {
            return pcb;
        }
        pcb = pcb->next;
    }

    return NULL;
}

/**
 * 切换到指定进程
 */
void m4k_process_switch_to(m4k_pcb_t *pcb) {
    if (!pcb || pcb->state != M4K_PROCESS_READY) {
        return;
    }

    m4k_pcb_t *old_process = current_process;

    if (old_process) {
        /* 保存当前进程上下文 */
        __asm__ volatile (
            "movq %%rsp, %0\n"
            "movq %%rbp, %1\n"
            "movq %%rax, %2\n"
            "movq %%rbx, %3\n"
            "movq %%rcx, %4\n"
            "movq %%rdx, %5\n"
            "movq %%rsi, %6\n"
            "movq %%rdi, %7\n"
            "movq %%r8, %8\n"
            "movq %%r9, %9\n"
            "movq %%r10, %10\n"
            "movq %%r11, %11\n"
            "movq %%r12, %12\n"
            "movq %%r13, %13\n"
            "movq %%r14, %14\n"
            "movq %%r15, %15\n"
            "pushfq\n"
            "popq %16\n"
            : "=m"(old_process->rsp), "=m"(old_process->rbp),
              "=m"(old_process->rax), "=m"(old_process->rbx),
              "=m"(old_process->rcx), "=m"(old_process->rdx),
              "=m"(old_process->rsi), "=m"(old_process->rdi),
              "=m"(old_process->r8), "=m"(old_process->r9),
              "=m"(old_process->r10), "=m"(old_process->r11),
              "=m"(old_process->r12), "=m"(old_process->r13),
              "=m"(old_process->r14), "=m"(old_process->r15),
              "=m"(old_process->rflags)
        );

        old_process->state = M4K_PROCESS_READY;
    }

    /* 切换到新进程 */
    current_process = pcb;
    pcb->state = M4K_PROCESS_RUNNING;

    /* 加载新进程上下文 */
    __asm__ volatile (
        "movq %0, %%cr3\n"
        "movq %1, %%rsp\n"
        "movq %2, %%rbp\n"
        "movq %3, %%rax\n"
        "movq %4, %%rbx\n"
        "movq %5, %%rcx\n"
        "movq %6, %%rdx\n"
        "movq %7, %%rsi\n"
        "movq %8, %%rdi\n"
        "movq %9, %%r8\n"
        "movq %10, %%r9\n"
        "movq %11, %%r10\n"
        "movq %12, %%r11\n"
        "movq %13, %%r12\n"
        "movq %14, %%r13\n"
        "movq %15, %%r14\n"
        "movq %16, %%r15\n"
        "pushq %17\n"
        "popfq\n"
        :
        : "r"(pcb->cr3), "r"(pcb->rsp), "r"(pcb->rbp),
          "r"(pcb->rax), "r"(pcb->rbx), "r"(pcb->rcx), "r"(pcb->rdx),
          "r"(pcb->rsi), "r"(pcb->rdi), "r"(pcb->r8), "r"(pcb->r9),
          "r"(pcb->r10), "r"(pcb->r11), "r"(pcb->r12), "r"(pcb->r13),
          "r"(pcb->r14), "r"(pcb->r15), "r"(pcb->rflags)
        : "memory"
    );

    scheduler_stats.context_switches++;
}

/**
 * 进程调度器
 */
void m4k_process_schedule(void) {
    if (!scheduler_enabled || !current_process) {
        return;
    }

    m4k_pcb_t *pcb = process_list;
    m4k_pcb_t *next = NULL;
    m4k_pcb_t *highest_priority = NULL;

    /* 查找最高优先级的就绪进程 */
    while (pcb) {
        if (pcb->state == M4K_PROCESS_READY) {
            if (!highest_priority ||
                pcb->priority < highest_priority->priority) {
                highest_priority = pcb;
            }
        }
        pcb = pcb->next;
    }

    /* 如果找到更高优先级的进程，切换 */
    if (highest_priority && highest_priority != current_process) {
        next = highest_priority;
    }

    /* 时间片到期，切换到下一个进程 */
    if (!next && current_process) {
        pcb = process_list;
        while (pcb) {
            if (pcb->state == M4K_PROCESS_READY && pcb != current_process) {
                next = pcb;
                break;
            }
            pcb = pcb->next;
        }
    }

    /* 执行进程切换 */
    if (next) {
        m4k_process_switch_to(next);
    }

    scheduler_stats.schedule_calls++;
}

/**
 * 启用调度器
 */
void m4k_scheduler_enable(void) {
    scheduler_enabled = 1;
    console_write("M4KK1 process scheduler enabled\n");
}

/**
 * 禁用调度器
 */
void m4k_scheduler_disable(void) {
    scheduler_enabled = 0;
    console_write("M4KK1 process scheduler disabled\n");
}

/**
 * 获取调度器统计信息
 */
void m4k_scheduler_get_stats(uint64_t *context_switches, uint64_t *schedule_calls) {
    if (context_switches) *context_switches = scheduler_stats.context_switches;
    if (schedule_calls) *schedule_calls = scheduler_stats.schedule_calls;
}

/**
 * 获取时间片
 */
uint64_t get_time_slice(uint32_t priority) {
    switch (priority) {
        case M4K_PRIORITY_HIGHEST: return M4K_TIME_SLICE_HIGHEST;
        case M4K_PRIORITY_HIGH: return M4K_TIME_SLICE_HIGH;
        case M4K_PRIORITY_NORMAL: return M4K_TIME_SLICE_NORMAL;
        case M4K_PRIORITY_LOW: return M4K_TIME_SLICE_LOW;
        case M4K_PRIORITY_LOWEST: return M4K_TIME_SLICE_LOWEST;
        default: return M4K_TIME_SLICE_NORMAL;
    }
}

/**
 * 获取时间戳
 */
uint64_t get_timestamp(void) {
    /* 使用TSC获取时间戳 */
    uint32_t low, high;
    __asm__ volatile ("rdtsc" : "=a"(low), "=d"(high));
    return ((uint64_t)high << 32) | low;
}

/**
 * 读取CR3寄存器
 */
uint64_t read_cr3(void) {
    uint64_t value;
    __asm__ volatile ("movq %%cr3, %0" : "=r"(value));
    return value;
}

/**
 * 进程退出
 */
void m4k_process_exit(void) {
    if (current_process) {
        current_process->state = M4K_PROCESS_ZOMBIE;
        current_process->exit_time = get_timestamp();

        console_write("Process ");
        console_write(current_process->name);
        console_write(" (PID: ");
        console_write_dec(current_process->pid);
        console_write(") exited\n");

        /* 调度下一个进程 */
        m4k_process_schedule();
    }
}

/**
 * 打印进程信息
 */
void m4k_process_print_info(m4k_pcb_t *pcb) {
    if (!pcb) return;

    console_write("Process: ");
    console_write(pcb->name);
    console_write("\n");
    console_write("  PID: ");
    console_write_dec(pcb->pid);
    console_write("\n");
    console_write("  PPID: ");
    console_write_dec(pcb->ppid);
    console_write("\n");
    console_write("  State: ");

    switch (pcb->state) {
        case M4K_PROCESS_RUNNING:
            console_write("RUNNING");
            break;
        case M4K_PROCESS_READY:
            console_write("READY");
            break;
        case M4K_PROCESS_BLOCKED:
            console_write("BLOCKED");
            break;
        case M4K_PROCESS_SLEEPING:
            console_write("SLEEPING");
            break;
        case M4K_PROCESS_ZOMBIE:
            console_write("ZOMBIE");
            break;
        case M4K_PROCESS_STOPPED:
            console_write("STOPPED");
            break;
        default:
            console_write("UNKNOWN");
    }

    console_write("\n");
    console_write("  Priority: ");
    console_write_dec(pcb->priority);
    console_write("\n");
    console_write("  CPU Time: ");
    console_write_dec(pcb->cpu_time);
    console_write("\n");
    console_write("  Context Switches: ");
    console_write_dec(pcb->context_switches);
    console_write("\n");
}

/**
 * 列出所有进程
 */
void m4k_process_list_all(void) {
    m4k_pcb_t *pcb = process_list;
    uint32_t count = 0;

    console_write("=== M4KK1 Process List ===\n");

    while (pcb) {
        console_write("[");
        console_write_dec(count++);
        console_write("] ");
        m4k_process_print_info(pcb);
        console_write("\n");
        pcb = pcb->next;
    }

    console_write("Total processes: ");
    console_write_dec(process_count);
    console_write("\n");
    console_write("==========================\n");
}

/**
 * 发送信号给进程
 */
void m4k_process_send_signal(m4k_pcb_t *pcb, uint32_t signal) {
    if (!pcb) return;

    pcb->signal_pending |= (1ULL << signal);

    console_write("Signal ");
    console_write_dec(signal);
    console_write(" sent to process ");
    console_write(pcb->name);
    console_write(" (PID: ");
    console_write_dec(pcb->pid);
    console_write(")\n");
}

/**
 * 处理进程信号
 */
void m4k_process_handle_signals(void) {
    if (!current_process) return;

    uint64_t pending = current_process->signal_pending & ~current_process->signal_mask;

    if (pending) {
        uint32_t signal = 0;
        while (signal < 64) {
            if (pending & (1ULL << signal)) {
                /* 处理信号 */
                void *handler = current_process->signal_handlers[signal];
                if (handler) {
                    /* 调用信号处理函数 */
                    ((void (*)(uint32_t))handler)(signal);
                }

                current_process->signal_pending &= ~(1ULL << signal);
                break;
            }
            signal++;
        }
    }
}

/**
 * 阻塞进程
 */
void m4k_process_block(m4k_pcb_t *pcb) {
    if (!pcb) return;

    pcb->state = M4K_PROCESS_BLOCKED;

    /* 如果是当前进程，调度下一个 */
    if (pcb == current_process) {
        m4k_process_schedule();
    }
}

/**
 * 唤醒进程
 */
void m4k_process_wake_up(m4k_pcb_t *pcb) {
    if (!pcb) return;

    if (pcb->state == M4K_PROCESS_BLOCKED || pcb->state == M4K_PROCESS_SLEEPING) {
        pcb->state = M4K_PROCESS_READY;
    }
}

/**
 * 让出CPU
 */
void m4k_process_yield(void) {
    if (current_process) {
        current_process->time_slice = 0; /* 强制调度 */
        m4k_process_schedule();
    }
}

/**
 * 进程睡眠
 */
void m4k_process_sleep(uint64_t milliseconds) {
    if (!current_process) return;

    current_process->state = M4K_PROCESS_SLEEPING;
    /* 设置睡眠时间（简化实现） */

    m4k_process_schedule();
}

/**
 * 进程唤醒
 */
void m4k_process_wake_up_by_pid(uint64_t pid) {
    m4k_pcb_t *pcb = m4k_process_find(pid);
    if (pcb) {
        m4k_process_wake_up(pcb);
    }
}

/**
 * 获取进程优先级
 */
uint32_t m4k_process_get_priority(m4k_pcb_t *pcb) {
    return pcb ? pcb->priority : M4K_PRIORITY_NORMAL;
}

/**
 * 设置进程优先级
 */
void m4k_process_set_priority(m4k_pcb_t *pcb, uint32_t priority) {
    if (pcb) {
        pcb->priority = priority;
        pcb->time_slice = get_time_slice(priority);
    }
}

/**
 * 复制进程内存空间
 */
void m4k_process_copy_memory_space(m4k_pcb_t *dest, m4k_pcb_t *src) {
    if (!dest || !src) return;

    /* 复制页表 */
    memcpy((void *)dest->cr3, (void *)src->cr3, 4096);

    /* 复制堆和栈 */
    if (src->heap_base && src->heap_size) {
        dest->heap_base = (uint64_t)kmalloc(src->heap_size);
        memcpy((void *)dest->heap_base, (void *)src->heap_base, src->heap_size);
        dest->heap_size = src->heap_size;
    }
}

/**
 * 进程调试信息
 */
void m4k_process_debug_info(m4k_pcb_t *pcb) {
    if (!pcb) return;

    console_write("=== Process Debug Info ===\n");
    console_write("Name: ");
    console_write(pcb->name);
    console_write("\n");
    console_write("PID: ");
    console_write_dec(pcb->pid);
    console_write("\n");
    console_write("State: ");
    console_write_dec(pcb->state);
    console_write("\n");
    console_write("Priority: ");
    console_write_dec(pcb->priority);
    console_write("\n");
    console_write("CR3: 0x");
    console_write_hex(pcb->cr3);
    console_write("\n");
    console_write("RIP: 0x");
    console_write_hex(pcb->rip);
    console_write("\n");
    console_write("RSP: 0x");
    console_write_hex(pcb->rsp);
    console_write("\n");
    console_write("Stack: 0x");
    console_write_hex(pcb->stack_base);
    console_write(" - 0x");
    console_write_hex(pcb->stack_base + pcb->stack_size);
    console_write("\n");
    console_write("==========================\n");
}

/**
 * 进程管理初始化
 */
void m4k_arch_process_init(void) {
    m4k_process_init();
    console_write("M4KK1 x86_64 process management initialized\n");
}
/**
 * M4KK1 Process Management Implementation
 * 进程管理系统实现
 *
 * 实现多级调度策略：
 * - 优先级调度：高优先级进程优先执行
 * - 时间片轮转：同一优先级的进程轮流执行
 * - 实时调度：实时进程立即抢占
 */

#include "process.h"
#include "timer.h"
#include "memory.h"
#include "kernel.h"
#include "idt.h"
#include "console.h"
#include "ldso.h"
#include <string.h>
#include <stdint.h>

/* 声明asm关键字用于内联汇编 */
#ifndef asm
#define asm __asm__
#endif

/* 中断处理函数类型定义 */
typedef void (*interrupt_handler_t)(void);

/* 进程管理全局变量 */
static process_control_t process_control;
static process_t *current_process = NULL;
static uint32_t scheduler_enabled = 0;
static uint32_t time_slice_counter = 0;
static uint32_t time_slice_length = 10; /* 时间片长度（毫秒） */

/* 调度队列 */
#define READY_QUEUE_SIZE 256
#define BLOCKED_QUEUE_SIZE 256
static process_t *ready_queues[3][READY_QUEUE_SIZE]; /* 按优先级组织的就绪队列 */
static uint32_t ready_queue_count[3] = {0, 0, 0};
static process_t *blocked_queue[BLOCKED_QUEUE_SIZE];
static uint32_t blocked_queue_count = 0;

/* 进程间通信 */
#define IPC_MESSAGE_SIZE 256
#define IPC_QUEUE_SIZE 64

typedef struct {
    uint32_t sender_pid;
    uint32_t receiver_pid;
    uint8_t data[IPC_MESSAGE_SIZE];
    uint32_t size;
    uint32_t type;
} ipc_message_t;

static ipc_message_t ipc_queue[IPC_QUEUE_SIZE];
static uint32_t ipc_queue_head = 0;
static uint32_t ipc_queue_tail = 0;
static uint32_t ipc_queue_count = 0;

/**
 * 获取下一个可用的进程ID
 */
static uint32_t process_get_next_pid(void) {
    return process_control.next_pid++;
}

/**
 * 初始化进程管理
 */
void process_init(void) {
    KLOG_INFO("Initializing process management...");

    /* 初始化进程控制结构 */
    memset(&process_control, 0, sizeof(process_control_t));
    process_control.next_pid = 1;

    /* 初始化调度队列 */
    memset(ready_queues, 0, sizeof(ready_queues));
    memset(ready_queue_count, 0, sizeof(ready_queue_count));
    memset(blocked_queue, 0, sizeof(blocked_queue));
    blocked_queue_count = 0;

    /* 初始化IPC队列 */
    memset(ipc_queue, 0, sizeof(ipc_queue));
    ipc_queue_head = 0;
    ipc_queue_tail = 0;
    ipc_queue_count = 0;

    /* 创建初始进程 */
    process_create_init();

    KLOG_INFO("Process management initialized");
}

/**
 * 创建初始进程
 */
void process_create_init(void) {
    process_t *init_process = (process_t *)kmalloc(sizeof(process_t));
    if (!init_process) {
        panic("Failed to allocate memory for init process");
        return;
    }

    /* 初始化进程结构 */
    memset(init_process, 0, sizeof(process_t));
    init_process->pid = process_get_next_pid();
    init_process->ppid = 0;
    init_process->state = PROCESS_STATE_RUNNING;
    init_process->priority = PROCESS_PRIORITY_NORMAL;
    init_process->esp = KERNEL_STACK;
    init_process->ebp = KERNEL_STACK;
    init_process->eip = 0; /* 初始进程从内核主函数开始 */
    init_process->cr3 = 0; /* 使用内核页目录 */
    init_process->sleep_ticks = 0;
    strcpy(init_process->name, "init");

    /* 设置为当前进程 */
    current_process = init_process;
    process_control.current = init_process;
    process_control.process_count = 1;

    KLOG_INFO("Initial process created: PID=1");
}

/**
 * 创建新进程
 */
process_t *process_create(const char *name, uint32_t priority) {
    process_t *process;
    uint32_t *stack;

    if (priority > PROCESS_PRIORITY_LOW) {
        priority = PROCESS_PRIORITY_NORMAL;
    }

    /* 分配进程结构 */
    process = (process_t *)kmalloc(sizeof(process_t));
    if (!process) {
        KLOG_ERROR("Failed to allocate memory for new process");
        return NULL;
    }

    /* 分配内核栈 */
    stack = (uint32_t *)kmalloc(KERNEL_STACK_SIZE);
    if (!stack) {
        kfree(process);
        KLOG_ERROR("Failed to allocate kernel stack for new process");
        return NULL;
    }

    /* 初始化进程结构 */
    memset(process, 0, sizeof(process_t));
    process->pid = process_get_next_pid();
    process->ppid = current_process ? current_process->pid : 0;
    process->state = PROCESS_STATE_READY;
    process->priority = priority;
    process->esp = (uint32_t)stack + KERNEL_STACK_SIZE - sizeof(uint32_t);
    process->ebp = process->esp;
    process->cr3 = 0; /* 使用内核页目录 */
    process->sleep_ticks = 0;
    strncpy(process->name, name, sizeof(process->name) - 1);
    process->name[sizeof(process->name) - 1] = '\0';

    /* 设置栈帧用于进程切换 */
    stack = (uint32_t *)process->esp;
    *(--stack) = 0;                    /* EDI */
    *(--stack) = 0;                    /* ESI */
    *(--stack) = 0;                    /* EBP */
    *(--stack) = 0;                    /* ESP (dummy) */
    *(--stack) = 0;                    /* EBX */
    *(--stack) = 0;                    /* EDX */
    *(--stack) = 0;                    /* ECX */
    *(--stack) = 0;                    /* EAX */
    *(--stack) = 0x0202;               /* EFLAGS */
    *(--stack) = 0;                    /* EIP (入口点) */
    *(--stack) = (uint32_t)process_exit; /* 返回地址 */
    process->esp = (uint32_t)stack;

    /* 添加到就绪队列 */
    if (ready_queue_count[priority] < READY_QUEUE_SIZE) {
        ready_queues[priority][ready_queue_count[priority]++] = process;
    } else {
        KLOG_WARN("Ready queue full, cannot add process");
        kfree(process);
        kfree((void *)((uint32_t)stack - KERNEL_STACK_SIZE + sizeof(uint32_t)));
        return NULL;
    }

    process_control.process_count++;

    KLOG_INFO("Process created");

    return process;
}

/**
 * 销毁进程
 */
void process_destroy(process_t *process) {
    uint32_t i, j;

    if (!process) {
        return;
    }

    /* 从就绪队列中移除 */
    for (i = 0; i < 3; i++) {
        for (j = 0; j < ready_queue_count[i]; j++) {
            if (ready_queues[i][j] == process) {
                /* 移动队列尾部的进程到当前位置 */
                ready_queues[i][j] = ready_queues[i][--ready_queue_count[i]];
                break;
            }
        }
    }

    /* 从阻塞队列中移除 */
    for (i = 0; i < blocked_queue_count; i++) {
        if (blocked_queue[i] == process) {
            blocked_queue[i] = blocked_queue[--blocked_queue_count];
            break;
        }
    }

    /* 释放内核栈 */
    if (process->esp) {
        uint32_t *stack = (uint32_t *)(process->esp - KERNEL_STACK_SIZE + sizeof(uint32_t));
        kfree(stack);
    }

    /* 释放进程结构 */
    kfree(process);

    process_control.process_count--;

    KLOG_INFO("Process destroyed: PID=");
    /* 注意：需要数字转字符串函数 */

    /* 如果这是最后一个进程，清理动态链接器 */
    if (process_control.process_count == 0) {
        m4ll_cleanup();
    }
}

/**
 * 获取当前进程
 */
process_t *process_get_current(void) {
    return current_process;
}

/**
 * 设置当前进程
 */
void process_set_current(process_t *process) {
    current_process = process;
    process_control.current = process;
}

/**
 * 获取进程调度优先级
 */
static uint32_t get_next_process_priority(void) {
    uint32_t i;

    /* 优先级调度：从高优先级到低优先级查找 */
    for (i = PROCESS_PRIORITY_HIGH; i <= PROCESS_PRIORITY_LOW; i++) {
        if (ready_queue_count[i] > 0) {
            return i;
        }
    }

    return PROCESS_PRIORITY_NORMAL; /* 默认优先级 */
}

/**
 * 获取下一个要执行的进程
 */
static process_t *get_next_process(void) {
    uint32_t priority = get_next_process_priority();

    if (ready_queue_count[priority] > 0) {
        process_t *process = ready_queues[priority][0];

        /* 将进程从队列头部移除 */
        ready_queues[priority][0] = ready_queues[priority][--ready_queue_count[priority]];

        return process;
    }

    return NULL;
}

/**
 * 调度进程
 */
void process_schedule(void) {
    process_t *next_process;

    if (!scheduler_enabled || !current_process) {
        return;
    }

    /* 检查时间片是否用完 */
    time_slice_counter++;
    if (time_slice_counter >= time_slice_length) {
        /* 时间片轮转：将当前进程放回就绪队列尾部 */
        if (current_process->state == PROCESS_STATE_RUNNING) {
            current_process->state = PROCESS_STATE_READY;
            if (ready_queue_count[current_process->priority] < READY_QUEUE_SIZE) {
                ready_queues[current_process->priority][ready_queue_count[current_process->priority]++] = current_process;
            }
        }
        time_slice_counter = 0;
    }

    /* 获取下一个进程 */
    next_process = get_next_process();
    if (next_process && next_process != current_process) {
        /* 执行进程切换 */
        process_switch_to(next_process);
    }
}

/**
 * 启动调度器
 */
void scheduler_start(void) {
    scheduler_enabled = 1;
    time_slice_counter = 0;

    /* 注册定时器中断处理函数 */
    idt_register_handler(IDT_TIMER, (interrupt_handler_t)process_schedule);

    KLOG_INFO("Process scheduler started");
}

/**
 * 阻塞当前进程
 */
void process_block(void) {
    if (current_process) {
        current_process->state = PROCESS_STATE_BLOCKED;

        /* 添加到阻塞队列 */
        if (blocked_queue_count < BLOCKED_QUEUE_SIZE) {
            blocked_queue[blocked_queue_count++] = current_process;
        }

        /* 执行调度 */
        process_schedule();
    }
}

/**
 * 唤醒进程
 */
void process_wakeup(process_t *process) {
    uint32_t i;

    if (!process || process->state != PROCESS_STATE_BLOCKED) {
        return;
    }

    /* 从阻塞队列中移除 */
    for (i = 0; i < blocked_queue_count; i++) {
        if (blocked_queue[i] == process) {
            blocked_queue[i] = blocked_queue[--blocked_queue_count];
            break;
        }
    }

    /* 设置为就绪状态 */
    process->state = PROCESS_STATE_READY;

    /* 添加到就绪队列 */
    if (ready_queue_count[process->priority] < READY_QUEUE_SIZE) {
        ready_queues[process->priority][ready_queue_count[process->priority]++] = process;
    }
}

/**
 * 睡眠指定毫秒数
 */
void process_sleep(uint32_t milliseconds) {
    if (current_process) {
        current_process->sleep_ticks = milliseconds * timer_get_frequency() / 1000;
        process_block();
    }
}

/**
 * 退出当前进程
 */
void process_exit(void) {
    if (current_process) {
        current_process->state = PROCESS_STATE_TERMINATED;

        KLOG_INFO("Process exiting");

        /* 执行调度以切换到下一个进程 */
        process_schedule();

        /* 如果没有其他进程，进入无限循环 */
        while (1) {
            HLT();
        }
    }
}

/**
 * 获取进程ID
 */
uint32_t process_get_pid(void) {
    return current_process ? current_process->pid : 0;
}

/**
 * 获取父进程ID
 */
uint32_t process_get_ppid(void) {
    return current_process ? current_process->ppid : 0;
}

/**
 * 设置进程优先级
 */
void process_set_priority(uint32_t priority) {
    if (current_process && priority <= PROCESS_PRIORITY_LOW) {
        current_process->priority = priority;
    }
}

/**
 * 获取进程优先级
 */
uint32_t process_get_priority(void) {
    return current_process ? current_process->priority : PROCESS_PRIORITY_NORMAL;
}

/**
 * 获取进程状态
 */
uint32_t process_get_state(void) {
    return current_process ? current_process->state : PROCESS_STATE_TERMINATED;
}

/**
 * 设置进程状态
 */
void process_set_state(uint32_t state) {
    if (current_process && state <= PROCESS_STATE_TERMINATED) {
        current_process->state = state;
    }
}

/**
 * 获取进程数量
 */
uint32_t process_get_count(void) {
    return process_control.process_count;
}

/**
 * 进程切换（低级函数）
 */
void process_switch(void) {
    if (!current_process) {
        return;
    }

    /* 保存当前进程上下文 */
    asm volatile (
        "pushfl\n"
        "pushl %%eax\n"
        "pushl %%ebx\n"
        "pushl %%ecx\n"
        "pushl %%edx\n"
        "pushl %%esi\n"
        "pushl %%edi\n"
        "pushl %%ebp\n"
        "movl %%esp, %%eax\n"
        "movl %%eax, %0\n"
        : "=m" (current_process->esp)
        : /* no input */
        : "eax"
    );

    /* 获取下一个进程 */
    process_t *next_process = get_next_process();
    if (next_process) {
        process_switch_to(next_process);
    }
}

/**
 * 切换到指定进程
 */
void process_switch_to(process_t *process) {
    if (!process || process == current_process) {
        return;
    }

    process_t *prev_process = current_process;
    current_process = process;
    process_control.current = process;

    /* 设置进程状态 */
    process->state = PROCESS_STATE_RUNNING;

    /* 切换页目录（如果需要） */
    if (process->cr3 != (prev_process ? prev_process->cr3 : 0)) {
        /* 暂时跳过页目录切换，简化处理 */
        process->cr3 = 0;
    }

    /* 切换栈和寄存器 */
    asm volatile (
        "movl %0, %%esp\n"
        "movl %1, %%ebp\n"
        "movl %2, %%ecx\n"
        :
        : "m" (process->esp),
          "m" (process->ebp),
          "m" (process->cr3)
    );

    /* 恢复寄存器 */
    if (prev_process) {
        asm volatile (
            "popl %%ebp\n"
            "popl %%edi\n"
            "popl %%esi\n"
            "popl %%edx\n"
            "popl %%ecx\n"
            "popl %%ebx\n"
            "popl %%eax\n"
            "popfl\n"
            ::: "esp"
        );
    }
}

/**
 * 保存进程上下文
 */
void process_save_context(process_t *process) {
    if (!process) {
        return;
    }

    /* 保存通用寄存器 - 分批保存以避免寄存器约束冲突 */
    asm volatile ("movl %%eax, %0" : "=m" (process->eax));
    asm volatile ("movl %%ebx, %0" : "=m" (process->ebx));
    asm volatile ("movl %%ecx, %0" : "=m" (process->ecx));
    asm volatile ("movl %%edx, %0" : "=m" (process->edx));
    asm volatile ("movl %%esi, %0" : "=m" (process->esi));
    asm volatile ("movl %%edi, %0" : "=m" (process->edi));
    asm volatile ("movl %%ebp, %0" : "=m" (process->ebp));
    asm volatile ("movl %%esp, %0" : "=m" (process->esp));

    /* 保存标志寄存器 */
    asm volatile ("pushfl; popl %0" : "=m" (process->flags));

    /* 保存指令指针（近似值） */
    asm volatile ("movl $1f, %0" : "=m" (process->eip));
}

/**
 * 恢复进程上下文
 */
void process_restore_context(process_t *process) {
    if (!process) {
        return;
    }

    /* 恢复指令指针和寄存器 */
    asm volatile (
        "movl %0, %%eax\n"
        "jmp *%%eax\n"
        "1:\n"
        "movl %1, %%ebx\n"
        "movl %2, %%ecx\n"
        "movl %3, %%edx\n"
        "movl %4, %%esi\n"
        "movl %5, %%edi\n"
        :
        : "m" (process->eip),
          "m" (process->ebx),
          "m" (process->ecx),
          "m" (process->edx),
          "m" (process->esi),
          "m" (process->edi)
    );

    /* 恢复栈和基址指针 */
    asm volatile (
        "movl %0, %%ebp\n"
        "movl %1, %%esp\n"
        :
        : "m" (process->ebp),
          "m" (process->esp)
    );

    /* 恢复标志寄存器 */
    asm volatile ("pushl %0; popfl" :: "m" (process->flags));
}

/**
 * 进程间通信 - 发送消息
 */
int32_t ipc_send(uint32_t receiver_pid, void *data, uint32_t size, uint32_t type) {
    ipc_message_t *message;

    if (size > IPC_MESSAGE_SIZE || !data) {
        return -1;
    }

    if (ipc_queue_count >= IPC_QUEUE_SIZE) {
        return -1; /* 队列满 */
    }

    message = &ipc_queue[ipc_queue_tail];
    message->sender_pid = current_process ? current_process->pid : 0;
    message->receiver_pid = receiver_pid;
    message->type = type;
    message->size = size;
    memcpy(message->data, data, size);

    ipc_queue_tail = (ipc_queue_tail + 1) % IPC_QUEUE_SIZE;
    ipc_queue_count++;

    return 0;
}

/**
 * 进程间通信 - 接收消息
 */
int32_t ipc_receive(uint32_t *sender_pid, void *data, uint32_t *size, uint32_t type) {
    uint32_t i;
    ipc_message_t *message;

    if (!data || !size) {
        return -1;
    }

    /* 在队列中查找目标消息 */
    for (i = 0; i < ipc_queue_count; i++) {
        uint32_t index = (ipc_queue_head + i) % IPC_QUEUE_SIZE;
        message = &ipc_queue[index];

        if (message->receiver_pid == (current_process ? current_process->pid : 0) ||
            message->receiver_pid == 0) { /* 广播消息 */

            if (type == 0 || message->type == type) {
                /* 找到消息，复制数据 */
                if (sender_pid) *sender_pid = message->sender_pid;
                *size = (message->size < *size) ? message->size : *size;
                memcpy(data, message->data, *size);

                /* 从队列中移除消息 */
                ipc_queue_head = (ipc_queue_head + 1) % IPC_QUEUE_SIZE;
                ipc_queue_count--;

                return 0;
            }
        }
    }

    return -1; /* 没有找到消息 */
}

/* timer_get_frequency函数已在timer.c中定义，这里不需要重复定义 */

/* 从timer.c获取定时器频率 */
extern uint32_t timer_get_frequency(void);
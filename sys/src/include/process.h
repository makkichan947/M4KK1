/**
 * M4KK1 Process Management Header
 * 进程管理函数定义
 */

#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <stdint.h>

/**
 * 进程状态
 */
#define PROCESS_STATE_RUNNING   0
#define PROCESS_STATE_READY     1
#define PROCESS_STATE_BLOCKED   2
#define PROCESS_STATE_TERMINATED 3

/**
 * 进程优先级
 */
#define PROCESS_PRIORITY_HIGH   0
#define PROCESS_PRIORITY_NORMAL 1
#define PROCESS_PRIORITY_LOW    2

/**
 * 进程结构
 */
typedef struct process {
    uint32_t pid;
    uint32_t ppid;
    uint32_t state;
    uint32_t priority;
    uint32_t esp;
    uint32_t ebp;
    uint32_t eip;
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t flags;
    uint32_t cr3;
    uint32_t sleep_ticks;
    char name[32];
    struct process *next;
} process_t;

/**
 * 进程控制块
 */
typedef struct {
    process_t *current;
    process_t *ready_queue;
    process_t *blocked_queue;
    uint32_t process_count;
    uint32_t next_pid;
} process_control_t;

/**
 * 初始化进程管理
 */
void process_init(void);

/**
 * 创建初始进程
 */
void process_create_init(void);

/**
 * 创建新进程
 */
process_t *process_create(const char *name, uint32_t priority);

/**
 * 销毁进程
 */
void process_destroy(process_t *process);

/**
 * 获取当前进程
 */
process_t *process_get_current(void);

/**
 * 设置当前进程
 */
void process_set_current(process_t *process);

/**
 * 调度进程
 */
void process_schedule(void);

/**
 * 启动调度器
 */
void scheduler_start(void);

/**
 * 阻塞当前进程
 */
void process_block(void);

/**
 * 唤醒进程
 */
void process_wakeup(process_t *process);

/**
 * 睡眠指定毫秒数
 */
void process_sleep(uint32_t milliseconds);

/**
 * 退出当前进程
 */
void process_exit(void);

/**
 * 获取进程ID
 */
uint32_t process_get_pid(void);

/**
 * 获取父进程ID
 */
uint32_t process_get_ppid(void);

/**
 * 设置进程优先级
 */
void process_set_priority(uint32_t priority);

/**
 * 获取进程优先级
 */
uint32_t process_get_priority(void);

/**
 * 获取进程状态
 */
uint32_t process_get_state(void);

/**
 * 设置进程状态
 */
void process_set_state(uint32_t state);

/**
 * 获取进程数量
 */
uint32_t process_get_count(void);

/**
 * 进程切换
 */
void process_switch(void);

/**
 * 保存进程上下文
 */
void process_save_context(process_t *process);

/**
 * 恢复进程上下文
 */
void process_restore_context(process_t *process);

/**
 * 进程间通信 - 发送消息
 */
int32_t ipc_send(uint32_t receiver_pid, void *data, uint32_t size, uint32_t type);

/**
 * 进程间通信 - 接收消息
 */
int32_t ipc_receive(uint32_t *sender_pid, void *data, uint32_t *size, uint32_t type);

/**
 * 切换到指定进程（内部函数）
 */
void process_switch_to(process_t *process);

/**
 * 获取定时器频率（内部函数）
 */
uint32_t timer_get_frequency(void);

#endif /* __PROCESS_H__ */
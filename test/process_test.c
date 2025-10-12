/**
 * 进程管理系统测试程序
 */

#include "../sys/src/include/process.h"
#include "../sys/src/include/console.h"
#include "../sys/src/include/timer.h"
#include <stdint.h>

void test_process(void) {
    console_write("Test process running!\n");
    process_sleep(1000); /* 睡眠1秒 */
    console_write("Test process finished!\n");
}

void process_test_main(void) {
    process_t *test_proc;
    uint32_t pid;

    console_write("=== 进程管理系统测试 ===\n");

    /* 初始化进程管理 */
    process_init();

    /* 创建测试进程 */
    test_proc = process_create("test_process", PROCESS_PRIORITY_NORMAL);
    if (test_proc) {
        console_write("测试进程创建成功\n");
    } else {
        console_write("测试进程创建失败\n");
        return;
    }

    /* 获取当前进程信息 */
    pid = process_get_pid();
    console_write("当前进程PID: ");
    console_write_dec(pid);
    console_write("\n");

    /* 测试进程优先级 */
    process_set_priority(PROCESS_PRIORITY_HIGH);
    console_write("设置优先级为高优先级\n");

    /* 启动调度器 */
    scheduler_start();

    /* 测试进程间通信 */
    uint32_t data = 42;
    if (ipc_send(test_proc->pid, &data, sizeof(data), 1) == 0) {
        console_write("IPC消息发送成功\n");
    } else {
        console_write("IPC消息发送失败\n");
    }

    console_write("进程测试完成\n");
}
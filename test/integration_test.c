/**
 * M4KK1 系统综合测试程序
 * 验证所有已实现的系统功能
 */

#include "../sys/src/include/timer.h"
#include "../sys/src/include/process.h"
#include "../sys/src/include/syscall.h"
#include "../sys/src/include/console.h"
#include "../sys/src/include/ldso.h"
#include "../sys/src/include/memory.h"
#include <stdint.h>
#include <stdbool.h>

/* 测试统计信息 */
typedef struct {
    uint32_t total_tests;
    uint32_t passed_tests;
    uint32_t failed_tests;
    uint32_t start_time;
    uint32_t end_time;
} test_stats_t;

/* 全局测试统计 */
static test_stats_t g_test_stats;

/* 测试结果宏 */
#define TEST_START(name) do { \
    console_write("测试: " name "... "); \
    g_test_stats.total_tests++; \
} while(0)

#define TEST_PASS() do { \
    console_write("通过\n"); \
    g_test_stats.passed_tests++; \
} while(0)

#define TEST_FAIL(reason) do { \
    console_write("失败 - " reason "\n"); \
    g_test_stats.failed_tests++; \
} while(0)

#define TEST_ASSERT(condition, message) do { \
    if (condition) { \
        TEST_PASS(); \
    } else { \
        TEST_FAIL(message); \
    } \
} while(0)

/* 定时器测试回调函数 */
static uint32_t g_alarm_count = 0;
static void alarm_callback(void) {
    g_alarm_count++;
    console_write("闹钟触发！计数: ");
    console_write_dec(g_alarm_count);
    console_write("\n");
}

/* 进程间通信测试数据 */
#define IPC_TEST_MESSAGE "Hello from IPC test!"
#define IPC_TEST_TYPE 0x1234

/**
 * 定时器系统测试
 */
void test_timer_system(void) {
    console_write("\n=== 定时器系统测试 ===\n");

    /* 测试定时器初始化 */
    TEST_START("定时器初始化");
    timer_init(1000); /* 1kHz频率 */
    TEST_PASS();

    /* 测试时钟计数 */
    TEST_START("时钟计数获取");
    uint32_t ticks1 = timer_get_ticks();
    timer_wait(100); /* 等待100ms */
    uint32_t ticks2 = timer_get_ticks();
    TEST_ASSERT(ticks2 > ticks1, "时钟计数应该递增");
    console_write("时钟计数: ");
    console_write_dec(ticks1);
    console_write(" -> ");
    console_write_dec(ticks2);
    console_write("\n");

    /* 测试系统运行时间 */
    TEST_START("系统运行时间");
    uint32_t uptime = timer_get_uptime();
    TEST_ASSERT(uptime > 0, "系统运行时间应该大于0");
    console_write("系统运行时间: ");
    console_write_dec(uptime);
    console_write("ms\n");

    /* 测试睡眠功能 */
    TEST_START("睡眠功能");
    uint32_t start_time = timer_get_uptime();
    timer_wait(200); /* 睡眠200ms */
    uint32_t elapsed = timer_get_uptime() - start_time;
    TEST_ASSERT(elapsed >= 200, "睡眠时间应该至少200ms");
    console_write("睡眠耗时: ");
    console_write_dec(elapsed);
    console_write("ms\n");

    /* 测试闹钟功能 */
    TEST_START("闹钟功能");
    g_alarm_count = 0;
    uint32_t alarm_id = timer_create_alarm(500, alarm_callback); /* 500ms闹钟 */
    TEST_ASSERT(alarm_id > 0, "闹钟创建应该成功");

    /* 等待闹钟触发 */
    timer_wait(600);
    TEST_ASSERT(g_alarm_count > 0, "闹钟应该被触发");
    console_write("闹钟触发次数: ");
    console_write_dec(g_alarm_count);
    console_write("\n");

    /* 销毁闹钟 */
    int32_t destroy_result = timer_destroy_alarm(alarm_id);
    TEST_ASSERT(destroy_result == 0, "闹钟销毁应该成功");

    /* 测试纳秒级睡眠 */
    TEST_START("纳秒级睡眠");
    uint64_t nano_start = timer_get_nanoseconds();
    timer_nsleep(1000000); /* 睡眠1ms */
    uint64_t nano_elapsed = timer_get_nanoseconds() - nano_start;
    TEST_ASSERT(nano_elapsed >= 1000000, "纳秒睡眠应该至少1ms");
    console_write("纳秒睡眠耗时: ");
    console_write_dec(nano_elapsed / 1000000);
    console_write("ms\n");

    console_write("定时器系统测试完成\n");
}

/**
 * 进程管理系统测试
 */
void test_process_system(void) {
    console_write("\n=== 进程管理系统测试 ===\n");

    /* 初始化进程管理 */
    TEST_START("进程管理初始化");
    process_init();
    TEST_PASS();

    /* 创建初始进程 */
    TEST_START("初始进程创建");
    process_create_init();
    TEST_PASS();

    /* 获取当前进程信息 */
    TEST_START("当前进程信息");
    uint32_t pid = process_get_pid();
    uint32_t ppid = process_get_ppid();
    TEST_ASSERT(pid > 0, "进程ID应该大于0");
    console_write("当前PID: ");
    console_write_dec(pid);
    console_write(", 父进程PID: ");
    console_write_dec(ppid);
    console_write("\n");

    /* 测试进程优先级 */
    TEST_START("进程优先级设置");
    uint32_t old_priority = process_get_priority();
    process_set_priority(PROCESS_PRIORITY_HIGH);
    uint32_t new_priority = process_get_priority();
    TEST_ASSERT(new_priority == PROCESS_PRIORITY_HIGH, "优先级设置失败");
    process_set_priority(old_priority); /* 恢复原优先级 */
    console_write("优先级测试完成\n");

    /* 测试进程状态 */
    TEST_START("进程状态管理");
    uint32_t state = process_get_state();
    TEST_ASSERT(state == PROCESS_STATE_RUNNING, "初始状态应该是运行状态");
    console_write("进程状态: ");
    console_write_dec(state);
    console_write("\n");

    /* 测试进程计数 */
    TEST_START("进程计数");
    uint32_t count = process_get_count();
    TEST_ASSERT(count > 0, "进程数量应该大于0");
    console_write("当前进程数量: ");
    console_write_dec(count);
    console_write("\n");

    /* 测试进程间通信 */
    TEST_START("进程间通信");
    char ipc_buffer[256];
    uint32_t sender_pid;
    uint32_t message_size;

    /* 发送测试消息 */
    uint32_t send_result = ipc_send(pid, IPC_TEST_MESSAGE, sizeof(IPC_TEST_MESSAGE), IPC_TEST_TYPE);
    TEST_ASSERT(send_result == 0, "IPC发送应该成功");

    /* 接收测试消息 */
    uint32_t recv_result = ipc_receive(&sender_pid, ipc_buffer, &message_size, IPC_TEST_TYPE);
    TEST_ASSERT(recv_result == 0, "IPC接收应该成功");
    TEST_ASSERT(message_size == sizeof(IPC_TEST_MESSAGE), "消息大小应该匹配");

    /* 验证消息内容 */
    if (recv_result == 0 && message_size == sizeof(IPC_TEST_MESSAGE)) {
        ipc_buffer[message_size] = '\0'; /* 添加字符串结束符 */
        TEST_ASSERT(ipc_buffer[0] == 'H', "消息内容应该正确");
        console_write("IPC消息内容: ");
        console_write(ipc_buffer);
        console_write("\n");
    }

    console_write("进程管理系统测试完成\n");
}

/**
 * 系统调用测试
 */
void test_syscall_system(void) {
    console_write("\n=== 系统调用测试 ===\n");

    /* 测试基础系统调用 */
    TEST_START("基础系统调用");
    uint32_t current_pid = SYSCALL1(SYSCALL_GETPID, 0);
    uint32_t parent_pid = SYSCALL1(SYSCALL_GETPPID, 0);
    TEST_ASSERT(current_pid > 0, "getpid应该返回有效PID");
    console_write("PID: ");
    console_write_dec(current_pid);
    console_write(", PPID: ");
    console_write_dec(parent_pid);
    console_write("\n");

    /* 测试时间系统调用 */
    TEST_START("时间系统调用");
    uint32_t sys_time = SYSCALL1(SYSCALL_TIME, 0);
    TEST_ASSERT(sys_time > 0, "time应该返回有效时间戳");
    console_write("系统时间: ");
    console_write_dec(sys_time);
    console_write("\n");

    /* 测试写系统调用 */
    TEST_START("写系统调用");
    const char *test_msg = "系统调用测试消息\n";
    uint32_t write_result = SYSCALL3(SYSCALL_WRITE, 1, (uint32_t)test_msg, 20);
    TEST_ASSERT(write_result == 20, "写操作应该成功");
    console_write("写操作结果: ");
    console_write_dec(write_result);
    console_write("字节\n");

    /* 测试内存分配系统调用 */
    TEST_START("内存分配系统调用");
    uint32_t brk_addr = SYSCALL1(SYSCALL_BRK, 0);
    TEST_ASSERT(brk_addr > 0, "brk应该返回有效地址");
    console_write("当前brk地址: 0x");
    console_write_hex(brk_addr);
    console_write("\n");

    /* 测试fork系统调用 */
    TEST_START("fork系统调用");
    uint32_t child_pid = SYSCALL1(SYSCALL_FORK, 0);
    if (child_pid == 0) {
        /* 子进程 */
        uint32_t child_check_pid = SYSCALL1(SYSCALL_GETPID, 0);
        TEST_ASSERT(child_check_pid > 0, "子进程应该有有效PID");
        console_write("子进程PID: ");
        console_write_dec(child_check_pid);
        console_write("\n");

        /* 子进程退出 */
        SYSCALL1(SYSCALL_EXIT, 0);
    } else {
        /* 父进程 */
        TEST_ASSERT(child_pid > 0, "fork应该返回子进程PID");
        console_write("父进程: 子进程PID = ");
        console_write_dec(child_pid);
        console_write("\n");

        /* 等待子进程 */
        SYSCALL3(SYSCALL_WAITPID, child_pid, 0, 0);
    }

    console_write("系统调用测试完成\n");
}

/**
 * 动态链接库测试
 */
void test_ldso_system(void) {
    console_write("\n=== 动态链接库测试 ===\n");

    /* 初始化动态链接器 */
    TEST_START("动态链接器初始化");
    int init_result = m4ll_init();
    TEST_ASSERT(init_result == 0, "动态链接器初始化应该成功");
    console_write("初始化结果: ");
    console_write_dec(init_result);
    console_write("\n");

    /* 测试符号查找 */
    TEST_START("符号查找");
    void *console_addr = m4ll_find_symbol("console_write");
    TEST_ASSERT(console_addr != NULL, "应该能找到console_write符号");
    console_write("console_write地址: 0x");
    console_write_hex((uint32_t)console_addr);
    console_write("\n");

    /* 测试库加载（预期失败，因为没有测试库文件） */
    TEST_START("库加载测试");
    m4ll_library_t *test_lib = NULL;
    int load_result = m4ll_load_library("nonexistent.m4ll", &test_lib);
    TEST_ASSERT(load_result != 0, "不存在的库文件应该加载失败");
    TEST_ASSERT(test_lib == NULL, "失败时库指针应该为NULL");
    console_write("库加载结果（预期失败）: ");
    console_write_dec(load_result);
    console_write("\n");

    /* 测试错误处理 */
    TEST_START("错误处理");
    extern int m4ll_errno;
    extern char m4ll_error_msg[256];

    /* 故意制造错误 */
    m4ll_load_library(NULL, &test_lib);
    TEST_ASSERT(m4ll_errno != M4LL_ERROR_NONE, "应该产生错误");
    console_write("错误码: ");
    console_write_dec(m4ll_errno);
    console_write(", 错误信息: ");
    console_write(m4ll_error_msg);
    console_write("\n");

    /* 清理动态链接器 */
    TEST_START("动态链接器清理");
    m4ll_cleanup();
    TEST_PASS();

    console_write("动态链接库测试完成\n");
}

/**
 * 系统集成测试
 */
void test_system_integration(void) {
    console_write("\n=== 系统集成测试 ===\n");

    /* 测试多系统协同工作 */
    TEST_START("多系统协同测试");

    /* 创建多个进程，每个进程都有定时器 */
    process_t *processes[3];
    uint32_t pids[3];

    for (int i = 0; i < 3; i++) {
        char proc_name[16];
        char num_str[4];
        strcpy(proc_name, "test_proc_");
        num_str[0] = '0' + i;
        num_str[1] = '\0';
        strcat(proc_name, num_str);

        processes[i] = process_create(proc_name, PROCESS_PRIORITY_NORMAL);
        TEST_ASSERT(processes[i] != NULL, "进程创建应该成功");

        pids[i] = processes[i]->pid;
        console_write("创建进程: ");
        console_write(proc_name);
        console_write(", PID: ");
        console_write_dec(pids[i]);
        console_write("\n");
    }

    /* 测试进程间通信和定时器结合 */
    TEST_START("IPC与定时器结合测试");

    /* 发送定时消息 */
    for (int i = 0; i < 3; i++) {
        char msg[32];
        strcpy(msg, "来自进程的消息 ");
        char num_str[4];
        num_str[0] = '0' + i;
        num_str[1] = '\0';
        strcat(msg, num_str);

        uint32_t send_result = ipc_send(pids[i], msg, strlen(msg) + 1, IPC_TEST_TYPE + i);
        TEST_ASSERT(send_result == 0, "IPC发送应该成功");
    }

    /* 接收并验证消息 */
    for (int i = 0; i < 3; i++) {
        char recv_buffer[256];
        uint32_t sender_pid;
        uint32_t msg_size;

        uint32_t recv_result = ipc_receive(&sender_pid, recv_buffer, &msg_size, IPC_TEST_TYPE + i);
        TEST_ASSERT(recv_result == 0, "IPC接收应该成功");

        recv_buffer[msg_size] = '\0';
        console_write("接收到消息: ");
        console_write(recv_buffer);
        console_write(" 来自PID: ");
        console_write_dec(sender_pid);
        console_write("\n");
    }

    /* 测试系统调用在多进程环境下的稳定性 */
    TEST_START("多进程系统调用稳定性");

    for (int i = 0; i < 3; i++) {
        uint32_t test_pid = SYSCALL1(SYSCALL_GETPID, 0);
        TEST_ASSERT(test_pid > 0, "系统调用应该在多进程环境下正常工作");

        uint32_t test_time = SYSCALL1(SYSCALL_TIME, 0);
        TEST_ASSERT(test_time > 0, "时间系统调用应该正常工作");
    }

    /* 清理测试进程 */
    TEST_START("测试进程清理");
    for (int i = 0; i < 3; i++) {
        if (processes[i]) {
            process_destroy(processes[i]);
        }
    }
    TEST_PASS();

    console_write("系统集成测试完成\n");
}

/**
 * 生成测试报告
 */
void generate_test_report(void) {
    console_write("\n=== 测试报告 ===\n");

    g_test_stats.end_time = timer_get_uptime();
    uint32_t total_time = g_test_stats.end_time - g_test_stats.start_time;

    console_write("测试统计:\n");
    console_write("总测试数: ");
    console_write_dec(g_test_stats.total_tests);
    console_write("\n通过: ");
    console_write_dec(g_test_stats.passed_tests);
    console_write("\n失败: ");
    console_write_dec(g_test_stats.failed_tests);
    console_write("\n成功率: ");

    if (g_test_stats.total_tests > 0) {
        uint32_t success_rate = (g_test_stats.passed_tests * 100) / g_test_stats.total_tests;
        console_write_dec(success_rate);
        console_write("%\n");
    }

    console_write("总耗时: ");
    console_write_dec(total_time);
    console_write("ms\n");

    /* 详细错误诊断 */
    if (g_test_stats.failed_tests > 0) {
        console_write("\n错误诊断:\n");

        /* 这里可以添加更详细的错误分析 */
        console_write("- 检查系统初始化是否正确完成\n");
        console_write("- 验证硬件设备是否正常工作\n");
        console_write("- 检查内存分配是否充足\n");
        console_write("- 确认进程调度器是否正常运行\n");
    }

    console_write("\n=== 测试完成 ===\n");
}

/**
 * 主测试函数
 */
int main(void) {
    console_write("M4KK1 系统综合测试开始\n");
    console_write("========================\n");

    /* 初始化测试统计 */
    g_test_stats.total_tests = 0;
    g_test_stats.passed_tests = 0;
    g_test_stats.failed_tests = 0;
    g_test_stats.start_time = timer_get_uptime();

    /* 执行各项测试 */
    test_timer_system();
    test_process_system();
    test_syscall_system();
    test_ldso_system();
    test_system_integration();

    /* 生成测试报告 */
    generate_test_report();

    console_write("所有测试执行完成！\n");

    /* 正常退出 */
    SYSCALL1(SYSCALL_EXIT, 0);
    return 0;
}
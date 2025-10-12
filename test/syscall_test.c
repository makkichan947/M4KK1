/**
 * M4KK1 System Call Test Program
 * 系统调用测试程序
 */

#include <syscall.h>
#include <console.h>
#include <stdint.h>

/**
 * 测试程序入口点
 */
int main(void) {
    uint32_t pid, ppid;
    uint32_t result;

    console_write("=== M4KK1 System Call Test ===\n");

    /* 测试getpid系统调用 */
    console_write("Testing getpid()...\n");
    pid = SYSCALL1(SYSCALL_GETPID, 0);
    console_write("Current PID: ");
    console_write_dec(pid);
    console_write("\n");

    /* 测试getppid系统调用 */
    console_write("Testing getppid()...\n");
    ppid = SYSCALL1(SYSCALL_GETPPID, 0);
    console_write("Parent PID: ");
    console_write_dec(ppid);
    console_write("\n");

    /* 测试write系统调用 */
    console_write("Testing write()...\n");
    const char *test_msg = "Hello from system call test!\n";
    result = SYSCALL3(SYSCALL_WRITE, 1, (uint32_t)test_msg, 30);
    console_write("Write result: ");
    console_write_dec(result);
    console_write("\n");

    /* 测试time系统调用 */
    console_write("Testing time()...\n");
    uint32_t current_time = SYSCALL1(SYSCALL_TIME, 0);
    console_write("Current time: ");
    console_write_dec(current_time);
    console_write("\n");

    /* 测试fork系统调用 */
    console_write("Testing fork()...\n");
    uint32_t child_pid = SYSCALL1(SYSCALL_FORK, 0);
    if (child_pid == 0) {
        /* 子进程 */
        console_write("Child process: My PID is ");
        pid = SYSCALL1(SYSCALL_GETPID, 0);
        console_write_dec(pid);
        console_write("\n");

        /* 子进程退出 */
        SYSCALL1(SYSCALL_EXIT, 0);
    } else {
        /* 父进程 */
        console_write("Parent process: Child PID is ");
        console_write_dec(child_pid);
        console_write("\n");

        /* 等待子进程 */
        SYSCALL3(SYSCALL_WAITPID, child_pid, 0, 0);
    }

    console_write("=== System Call Test Complete ===\n");

    /* 正常退出 */
    SYSCALL1(SYSCALL_EXIT, 0);

    return 0;
}
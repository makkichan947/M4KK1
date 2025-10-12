/**
 * M4KK1 System Call Header
 * 系统调用函数定义
 */

#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <stdint.h>

/**
 * 系统调用号
 */
#define SYSCALL_EXIT        0x01
#define SYSCALL_FORK        0x02
#define SYSCALL_READ        0x03
#define SYSCALL_WRITE       0x04
#define SYSCALL_OPEN        0x05
#define SYSCALL_CLOSE       0x06
#define SYSCALL_WAITPID     0x07
#define SYSCALL_EXECVE      0x08
#define SYSCALL_GETPID      0x09
#define SYSCALL_GETPPID     0x0A
#define SYSCALL_BRK         0x0B
#define SYSCALL_MMAP        0x0C
#define SYSCALL_MUNMAP      0x0D
#define SYSCALL_MPROTECT    0x0E
#define SYSCALL_MSYNC       0x0F
#define SYSCALL_GETCWD      0x10
#define SYSCALL_CHDIR       0x11
#define SYSCALL_MKDIR       0x12
#define SYSCALL_RMDIR       0x13
#define SYSCALL_LINK        0x14
#define SYSCALL_UNLINK      0x15
#define SYSCALL_RENAME      0x16
#define SYSCALL_STAT        0x17
#define SYSCALL_FSTAT       0x18
#define SYSCALL_LSTAT       0x19
#define SYSCALL_ACCESS      0x1A
#define SYSCALL_CHMOD       0x1B
#define SYSCALL_CHOWN       0x1C
#define SYSCALL_UTIME       0x1D
#define SYSCALL_TIME        0x1E
#define SYSCALL_TIMES       0x1F
#define SYSCALL_GETUID      0x20
#define SYSCALL_GETGID      0x21
#define SYSCALL_SETUID      0x22
#define SYSCALL_SETGID      0x23
#define SYSCALL_GETEUID     0x24
#define SYSCALL_GETEGID     0x25
#define SYSCALL_SETEUID     0x26
#define SYSCALL_SETEGID     0x27
#define SYSCALL_PIPE        0x28
#define SYSCALL_DUP         0x29
#define SYSCALL_DUP2        0x2A
#define SYSCALL_SELECT      0x2B
#define SYSCALL_POLL        0x2C
#define SYSCALL_EPOLL_CREATE 0x2D
#define SYSCALL_EPOLL_CTL   0x2E
#define SYSCALL_EPOLL_WAIT  0x2F
#define SYSCALL_SOCKET      0x30
#define SYSCALL_BIND        0x31
#define SYSCALL_LISTEN      0x32
#define SYSCALL_ACCEPT      0x33
#define SYSCALL_CONNECT     0x34
#define SYSCALL_SEND        0x35
#define SYSCALL_RECV        0x36
#define SYSCALL_SENDTO      0x37
#define SYSCALL_RECVFROM    0x38
#define SYSCALL_SHUTDOWN    0x39
#define SYSCALL_SETSOCKOPT  0x3A
#define SYSCALL_GETSOCKOPT  0x3B
#define SYSCALL_IOCTL       0x3C
#define SYSCALL_FCNTL       0x3D
#define SYSCALL_READDIR     0x3E
#define SYSCALL_TELLDIR     0x3F
#define SYSCALL_SEEKDIR     0x40
#define SYSCALL_CLOSEDIR    0x41
#define SYSCALL_OPENDIR     0x42
#define SYSCALL_MKNOD       0x43
#define SYSCALL_MKFIFO      0x44
#define SYSCALL_TRUNCATE    0x45
#define SYSCALL_FTRUNCATE   0x46
#define SYSCALL_GETDENTS    0x47
#define SYSCALL_SYNC        0x48
#define SYSCALL_FSYNC       0x49
#define SYSCALL_FDATASYNC   0x4A
#define SYSCALL_MLOCK       0x4B
#define SYSCALL_MUNLOCK     0x4C
#define SYSCALL_MLOCKALL    0x4D
#define SYSCALL_MUNLOCKALL  0x4E
#define SYSCALL_NANOSLEEP   0x4F
#define SYSCALL_CLOCK_GETTIME 0x50
#define SYSCALL_CLOCK_SETTIME 0x51
#define SYSCALL_CLOCK_GETRES  0x52
#define SYSCALL_SCHED_YIELD   0x53
#define SYSCALL_SCHED_SETSCHEDULER 0x54
#define SYSCALL_SCHED_GETSCHEDULER 0x55
#define SYSCALL_SCHED_SETPARAM    0x56
#define SYSCALL_SCHED_GETPARAM    0x57
#define SYSCALL_SCHED_SETAFFINITY 0x58
#define SYSCALL_SCHED_GETAFFINITY 0x59
#define SYSCALL_PRLIMIT64         0x5A
#define SYSCALL_GETRUSAGE         0x5B
#define SYSCALL_GETTIMEOFDAY      0x5C
#define SYSCALL_SETTIMEOFDAY      0x5D
#define SYSCALL_ADJTIMEX          0x5E
#define SYSCALL_TIMER_CREATE      0x5F
#define SYSCALL_TIMER_DELETE      0x60
#define SYSCALL_TIMER_SETTIME     0x61
#define SYSCALL_TIMER_GETTIME     0x62
#define SYSCALL_TIMER_GETOVERRUN  0x63
#define SYSCALL_KILL              0x64
#define SYSCALL_TKILL             0x65
#define SYSCALL_TGKILL            0x66
#define SYSCALL_SIGACTION         0x67
#define SYSCALL_SIGPROCMASK       0x68
#define SYSCALL_SIGPENDING        0x69
#define SYSCALL_SIGSUSPEND        0x6A
#define SYSCALL_SIGTIMEDWAIT      0x6B
#define SYSCALL_SIGRETURN         0x6C
#define SYSCALL_REBOOT            0x6D
#define SYSCALL_KEXEC_LOAD        0x6E
#define SYSCALL_EXIT_GROUP        0x6F
#define SYSCALL_WAIT4             0x70
#define SYSCALL_CLONE             0x71
#define SYSCALL_VFORK             0x72
#define SYSCALL_UNAME             0x73
#define SYSCALL_SEMGET            0x74
#define SYSCALL_SEMOP             0x75
#define SYSCALL_SEMCTL            0x76
#define SYSCALL_SEMTIMEDOP        0x77
#define SYSCALL_MSGGET            0x78
#define SYSCALL_MSGSND            0x79
#define SYSCALL_MSGRCV            0x7A
#define SYSCALL_MSGCTL            0x7B
#define SYSCALL_SHMGET            0x7C
#define SYSCALL_SHMAT             0x7D
#define SYSCALL_SHMDT             0x7E
#define SYSCALL_SHMCTL            0x7F

/* 动态链接器相关系统调用 */
#define SYSCALL_DL_LOAD_LIBRARY   0x80
#define SYSCALL_DL_UNLOAD_LIBRARY 0x81
#define SYSCALL_DL_FIND_SYMBOL    0x82
#define SYSCALL_DL_GET_ERROR      0x83

/**
 * 系统调用返回值
 */
#define SYSCALL_SUCCESS 0
#define SYSCALL_ERROR   (-1)

/**
 * 初始化系统调用
 */
void syscall_init(void);

/**
 * 系统调用处理函数
 */
void syscall_handler(void);

/**
 * 注册系统调用处理函数
 */
void syscall_register(uint32_t num, void *handler);

/**
 * 执行系统调用
 */
uint32_t syscall_execute(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);

/**
 * 系统调用宏
 */
#define SYSCALL0(num) syscall_execute(num, 0, 0, 0, 0, 0)
#define SYSCALL1(num, arg1) syscall_execute(num, (uint32_t)arg1, 0, 0, 0, 0)
#define SYSCALL2(num, arg1, arg2) syscall_execute(num, (uint32_t)arg1, (uint32_t)arg2, 0, 0, 0)
#define SYSCALL3(num, arg1, arg2, arg3) syscall_execute(num, (uint32_t)arg1, (uint32_t)arg2, (uint32_t)arg3, 0, 0)
#define SYSCALL4(num, arg1, arg2, arg3, arg4) syscall_execute(num, (uint32_t)arg1, (uint32_t)arg2, (uint32_t)arg3, (uint32_t)arg4, 0)
#define SYSCALL5(num, arg1, arg2, arg3, arg4, arg5) syscall_execute(num, (uint32_t)arg1, (uint32_t)arg2, (uint32_t)arg3, (uint32_t)arg4, (uint32_t)arg5)

#endif /* __SYSCALL_H__ */
/**
 * M4KK1 ps - Report process status
 * 报告进程状态
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_PROCESSES 1024
#define MAX_CMDLINE_LEN 4096

/* 进程信息结构 */
typedef struct {
    pid_t pid;
    pid_t ppid;
    char state;
    unsigned long utime;
    unsigned long stime;
    long priority;
    long nice;
    unsigned long vsize;
    unsigned long rss;
    char cmdline[MAX_CMDLINE_LEN];
} process_info_t;

/* 选项标志 */
static int show_all = 0;
static int show_long = 0;
static int show_user = 0;
static int show_full = 0;

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 ps - Report process status\n");
    printf("用法: ps [选项]\n");
    printf("\n");
    printf("选项:\n");
    printf("  -a, --all          显示所有进程\n");
    printf("  -l, --long         长格式显示\n");
    printf("  -u, --user         显示用户相关信息\n");
    printf("  -f, --full         完整格式显示\n");
    printf("  --help             显示此帮助信息\n");
}

/* 读取进程命令行 */
int read_cmdline(pid_t pid, char* cmdline, size_t size) {
    char path[256];
    int fd;
    ssize_t n;

    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
    fd = open(path, O_RDONLY);
    if (fd == -1) {
        return -1;
    }

    n = read(fd, cmdline, size - 1);
    close(fd);

    if (n <= 0) {
        return -1;
    }

    cmdline[n] = '\0';

    /* 将null字符替换为空格 */
    for (int i = 0; i < n - 1; i++) {
        if (cmdline[i] == '\0') {
            cmdline[i] = ' ';
        }
    }

    return 0;
}

/* 读取进程状态 */
int read_process_stat(pid_t pid, process_info_t* info) {
    char path[256];
    FILE* fp;
    char buffer[1024];

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    fp = fopen(path, "r");
    if (!fp) {
        return -1;
    }

    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        fclose(fp);
        return -1;
    }
    fclose(fp);

    /* 解析stat文件 */
    /* 格式: pid (comm) state ppid ... */
    char* token = strtok(buffer, " ");
    if (!token) return -1;
    info->pid = atoi(token);

    /* 跳过进程名（包含在括号中） */
    token = strtok(NULL, ")");
    if (!token) return -1;
    token = strtok(NULL, " ");
    if (!token) return -1;

    /* 状态 */
    info->state = token[0];

    /* PPID */
    token = strtok(NULL, " ");
    if (!token) return -1;
    info->ppid = atoi(token);

    /* 跳过一些字段到达utime */
    for (int i = 0; i < 11; i++) {
        token = strtok(NULL, " ");
        if (!token) return -1;
    }

    /* utime */
    token = strtok(NULL, " ");
    if (!token) return -1;
    info->utime = atol(token);

    /* stime */
    token = strtok(NULL, " ");
    if (!token) return -1;
    info->stime = atol(token);

    /* priority */
    token = strtok(NULL, " ");
    if (!token) return -1;
    info->priority = atol(token);

    /* nice */
    token = strtok(NULL, " ");
    if (!token) return -1;
    info->nice = atol(token);

    /* 跳到vsize */
    for (int i = 0; i < 7; i++) {
        token = strtok(NULL, " ");
        if (!token) return -1;
    }

    /* vsize */
    token = strtok(NULL, " ");
    if (!token) return -1;
    info->vsize = atol(token);

    /* rss */
    token = strtok(NULL, " ");
    if (!token) return -1;
    info->rss = atol(token);

    return 0;
}

/* 获取进程列表 */
int get_process_list(process_info_t* processes, int max_processes) {
    DIR* dir;
    struct dirent* entry;
    int count = 0;

    dir = opendir("/proc");
    if (!dir) {
        perror("无法打开/proc目录");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL && count < max_processes) {
        /* 检查是否为进程目录（纯数字） */
        char* endptr;
        pid_t pid = strtol(entry->d_name, &endptr, 10);
        if (*endptr != '\0') {
            continue; /* 不是进程目录 */
        }

        /* 读取进程信息 */
        if (read_process_stat(pid, &processes[count]) == 0) {
            /* 读取命令行 */
            if (read_cmdline(pid, processes[count].cmdline,
                           sizeof(processes[count].cmdline)) != 0) {
                strcpy(processes[count].cmdline, "[unknown]");
            }

            count++;
        }
    }

    closedir(dir);
    return count;
}

/* 显示进程信息（标准格式） */
void display_standard(process_info_t* proc) {
    printf("%5d %c %8lu %s\n",
           proc->pid,
           proc->state,
           proc->utime + proc->stime,
           proc->cmdline);
}

/* 显示进程信息（长格式） */
void display_long(process_info_t* proc) {
    printf("%5d %5d %c %3ld %3ld %8lu %8lu %8lu %s\n",
           proc->pid,
           proc->ppid,
           proc->state,
           proc->priority,
           proc->nice,
           proc->utime,
           proc->stime,
           proc->vsize / 1024, /* KB */
           proc->cmdline);
}

/* 显示进程信息（用户格式） */
void display_user(process_info_t* proc) {
    printf("%-8s %5d %5d %3ld %3ld %8lu %8lu %s\n",
           "root",  /* 暂时使用root */
           proc->pid,
           proc->ppid,
           proc->priority,
           proc->nice,
           proc->utime + proc->stime,
           proc->vsize / 1024,
           proc->cmdline);
}

/* 显示进程信息（完整格式） */
void display_full(process_info_t* proc) {
    printf("%-8s %5d %5d %3ld %3ld %c %8lu %8lu %8lu %s\n",
           "root",  /* 暂时使用root */
           proc->pid,
           proc->ppid,
           proc->priority,
           proc->nice,
           proc->state,
           proc->utime,
           proc->stime,
           proc->vsize / 1024,
           proc->cmdline);
}

/* 主函数 */
int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;
    process_info_t processes[MAX_PROCESSES];
    int process_count;

    static struct option long_options[] = {
        {"all", no_argument, 0, 'a'},
        {"long", no_argument, 0, 'l'},
        {"user", no_argument, 0, 'u'},
        {"full", no_argument, 0, 'f'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "aluf", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'a':
                show_all = 1;
                break;
            case 'l':
                show_long = 1;
                break;
            case 'u':
                show_user = 1;
                break;
            case 'f':
                show_full = 1;
                break;
            case 'h':
                show_help();
                return 0;
            default:
                fprintf(stderr, "无效选项。使用 --help 查看帮助。\n");
                return 1;
        }
    }

    /* 获取进程列表 */
    process_count = get_process_list(processes, MAX_PROCESSES);
    if (process_count < 0) {
        return 1;
    }

    /* 显示表头 */
    if (show_long) {
        printf("%5s %5s %1s %3s %3s %8s %8s %8s %s\n",
               "PID", "PPID", "S", "PRI", "NI", "UTIME", "STIME", "VSZ", "CMD");
    } else if (show_user) {
        printf("%-8s %5s %5s %3s %3s %8s %8s %s\n",
               "USER", "PID", "PPID", "PRI", "NI", "TIME", "VSZ", "CMD");
    } else if (show_full) {
        printf("%-8s %5s %5s %3s %3s %1s %8s %8s %8s %s\n",
               "USER", "PID", "PPID", "PRI", "NI", "S", "UTIME", "STIME", "VSZ", "CMD");
    } else {
        printf("%5s %1s %8s %s\n",
               "PID", "S", "TIME", "CMD");
    }

    /* 显示进程信息 */
    for (int i = 0; i < process_count; i++) {
        if (show_long) {
            display_long(&processes[i]);
        } else if (show_user) {
            display_user(&processes[i]);
        } else if (show_full) {
            display_full(&processes[i]);
        } else {
            display_standard(&processes[i]);
        }
    }

    return 0;
}
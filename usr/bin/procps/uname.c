/**
 * M4KK1 uname - Print system information
 * 打印系统信息
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/utsname.h>

#define KERNEL_NAME "M4KK1"
#define KERNEL_RELEASE "1.0.0"
#define KERNEL_VERSION "M4KK1-Y4KU"
#define MACHINE "i386"
#define PROCESSOR "i386"
#define HARDWARE_PLATFORM "i386"
#define OPERATING_SYSTEM "M4KK1"

/* 选项标志 */
static int show_all = 0;
static int show_kernel_name = 0;
static int show_nodename = 0;
static int show_kernel_release = 0;
static int show_kernel_version = 0;
static int show_machine = 0;
static int show_processor = 0;
static int show_hardware_platform = 0;
static int show_operating_system = 0;

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 uname - Print system information\n");
    printf("用法: uname [选项]\n");
    printf("\n");
    printf("选项:\n");
    printf("  -a, --all              显示所有信息\n");
    printf("  -s, --kernel-name      显示内核名称\n");
    printf("  -n, --nodename         显示网络节点主机名\n");
    printf("  -r, --kernel-release   显示内核发行版\n");
    printf("  -v, --kernel-version   显示内核版本\n");
    printf("  -m, --machine          显示硬件类型\n");
    printf("  -p, --processor        显示处理器类型\n");
    printf("  -i, --hardware-platform 显示硬件平台\n");
    printf("  -o, --operating-system 显示操作系统\n");
    printf("  --help                 显示此帮助信息\n");
}

/* 获取系统信息 */
void get_system_info(char* info[9]) {
    struct utsname uts;

    /* 尝试使用系统调用获取信息 */
    if (uname(&uts) == 0) {
        info[0] = strdup(uts.sysname);      /* 内核名称 */
        info[1] = strdup(uts.nodename);     /* 节点名 */
        info[2] = strdup(uts.release);      /* 内核发行版 */
        info[3] = strdup(uts.version);      /* 内核版本 */
        info[4] = strdup(uts.machine);      /* 硬件类型 */
        info[5] = strdup(uts.machine);      /* 处理器类型 */
        info[6] = strdup(uts.machine);      /* 硬件平台 */
        info[7] = strdup(uts.sysname);      /* 操作系统 */
        info[8] = NULL;
    } else {
        /* 使用默认的M4KK1信息 */
        info[0] = strdup(KERNEL_NAME);
        info[1] = strdup("m4kk1-host");
        info[2] = strdup(KERNEL_RELEASE);
        info[3] = strdup(KERNEL_VERSION);
        info[4] = strdup(MACHINE);
        info[5] = strdup(PROCESSOR);
        info[6] = strdup(HARDWARE_PLATFORM);
        info[7] = strdup(OPERATING_SYSTEM);
        info[8] = NULL;
    }
}

/* 释放系统信息 */
void free_system_info(char* info[9]) {
    for (int i = 0; i < 8; i++) {
        if (info[i]) {
            free(info[i]);
        }
    }
}

/* 主函数 */
int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;
    char* system_info[9];
    int first_output = 1;

    static struct option long_options[] = {
        {"all", no_argument, 0, 'a'},
        {"kernel-name", no_argument, 0, 's'},
        {"nodename", no_argument, 0, 'n'},
        {"kernel-release", no_argument, 0, 'r'},
        {"kernel-version", no_argument, 0, 'v'},
        {"machine", no_argument, 0, 'm'},
        {"processor", no_argument, 0, 'p'},
        {"hardware-platform", no_argument, 0, 'i'},
        {"operating-system", no_argument, 0, 'o'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "asnrvmpio", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'a':
                show_all = 1;
                break;
            case 's':
                show_kernel_name = 1;
                break;
            case 'n':
                show_nodename = 1;
                break;
            case 'r':
                show_kernel_release = 1;
                break;
            case 'v':
                show_kernel_version = 1;
                break;
            case 'm':
                show_machine = 1;
                break;
            case 'p':
                show_processor = 1;
                break;
            case 'i':
                show_hardware_platform = 1;
                break;
            case 'o':
                show_operating_system = 1;
                break;
            case 'h':
                show_help();
                return 0;
            default:
                fprintf(stderr, "无效选项。使用 --help 查看帮助。\n");
                return 1;
        }
    }

    /* 获取系统信息 */
    get_system_info(system_info);

    /* 如果没有指定选项，默认显示内核名称 */
    if (!show_all && !show_kernel_name && !show_nodename && !show_kernel_release &&
        !show_kernel_version && !show_machine && !show_processor &&
        !show_hardware_platform && !show_operating_system) {
        show_kernel_name = 1;
    }

    /* 显示所有信息 */
    if (show_all) {
        printf("%s %s %s %s %s\n",
               system_info[0], /* 内核名称 */
               system_info[1], /* 节点名 */
               system_info[2], /* 内核发行版 */
               system_info[3], /* 内核版本 */
               system_info[4]  /* 机器类型 */
        );
    } else {
        /* 显示指定的信息 */
        if (show_kernel_name) {
            if (!first_output) printf(" ");
            printf("%s", system_info[0]);
            first_output = 0;
        }
        if (show_nodename) {
            if (!first_output) printf(" ");
            printf("%s", system_info[1]);
            first_output = 0;
        }
        if (show_kernel_release) {
            if (!first_output) printf(" ");
            printf("%s", system_info[2]);
            first_output = 0;
        }
        if (show_kernel_version) {
            if (!first_output) printf(" ");
            printf("%s", system_info[3]);
            first_output = 0;
        }
        if (show_machine) {
            if (!first_output) printf(" ");
            printf("%s", system_info[4]);
            first_output = 0;
        }
        if (show_processor) {
            if (!first_output) printf(" ");
            printf("%s", system_info[5]);
            first_output = 0;
        }
        if (show_hardware_platform) {
            if (!first_output) printf(" ");
            printf("%s", system_info[6]);
            first_output = 0;
        }
        if (show_operating_system) {
            if (!first_output) printf(" ");
            printf("%s", system_info[7]);
            first_output = 0;
        }
        printf("\n");
    }

    /* 释放资源 */
    free_system_info(system_info);

    return 0;
}
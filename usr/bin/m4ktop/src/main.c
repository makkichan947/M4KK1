/**
 * M4KK1 System Monitor - Main Program
 * 系统监控工具主程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <math.h>

#include "m4ktop.h"

/* 程序版本 */
#define M4KTOP_VERSION "1.0.0"

/* 显示模式 */
#define MODE_TEXT 1
#define MODE_GRAPHICAL 2

/* 刷新间隔（秒） */
#define DEFAULT_INTERVAL 2

/* 全局配置 */
typedef struct {
    int mode;
    int interval;
    int show_cpu;
    int show_memory;
    int show_processes;
    int show_network;
    int show_disk;
    int show_system;
} m4ktop_config_t;

/* 显示帮助信息 */
void show_help(const char *program_name) {
    printf("M4KK1 System Monitor v%s\n", M4KTOP_VERSION);
    printf("用法: %s [选项]\n", program_name);
    printf("\n");
    printf("选项:\n");
    printf("  -t, --text             文本模式显示\n");
    printf("  -g, --graphical        图形模式显示（默认）\n");
    printf("  -i, --interval <sec>   刷新间隔（秒，默认: %d）\n", DEFAULT_INTERVAL);
    printf("  -c, --cpu              显示CPU信息\n");
    printf("  -m, --memory           显示内存信息\n");
    printf("  -p, --processes        显示进程信息\n");
    printf("  -n, --network          显示网络信息\n");
    printf("  -d, --disk             显示磁盘信息\n");
    printf("  -s, --system           显示系统信息\n");
    printf("  -a, --all              显示所有信息\n");
    printf("  -v, --version          显示版本\n");
    printf("  -h, --help             显示此帮助\n");
    printf("\n");
    printf("示例:\n");
    printf("  %s                     # 默认显示所有信息\n", program_name);
    printf("  %s -i 1 -c -m         # 每秒显示CPU和内存\n", program_name);
    printf("  %s --all               # 显示所有信息\n", program_name);
}

/* 显示版本信息 */
void show_version(void) {
    printf("M4KK1 System Monitor v%s\n", M4KTOP_VERSION);
    printf("Copyright (C) 2025 M4KK1 Development Team\n");
    printf("License: GPL-3.0\n");
}

/* 解析命令行参数 */
void parse_arguments(int argc, char *argv[], m4ktop_config_t *config) {
    int opt;

    /* 设置默认值 */
    config->mode = MODE_GRAPHICAL;
    config->interval = DEFAULT_INTERVAL;
    config->show_cpu = 0;
    config->show_memory = 0;
    config->show_processes = 0;
    config->show_network = 0;
    config->show_disk = 0;
    config->show_system = 0;

    while ((opt = getopt(argc, argv, "tgci:mpnsdvh")) != -1) {
        switch (opt) {
            case 't':
                config->mode = MODE_TEXT;
                break;
            case 'g':
                config->mode = MODE_GRAPHICAL;
                break;
            case 'i':
                config->interval = atoi(optarg);
                if (config->interval < 1) config->interval = 1;
                break;
            case 'c':
                config->show_cpu = 1;
                break;
            case 'm':
                config->show_memory = 1;
                break;
            case 'p':
                config->show_processes = 1;
                break;
            case 'n':
                config->show_network = 1;
                break;
            case 'd':
                config->show_disk = 1;
                break;
            case 's':
                config->show_system = 1;
                break;
            case 'a':
                config->show_cpu = 1;
                config->show_memory = 1;
                config->show_processes = 1;
                config->show_network = 1;
                config->show_disk = 1;
                config->show_system = 1;
                break;
            case 'v':
                show_version();
                exit(0);
            case 'h':
                show_help(argv[0]);
                exit(0);
            default:
                show_help(argv[0]);
                exit(1);
        }
    }

    /* 如果没有指定显示内容，显示所有 */
    if (!config->show_cpu && !config->show_memory && !config->show_processes &&
        !config->show_network && !config->show_disk && !config->show_system) {
        config->show_cpu = 1;
        config->show_memory = 1;
        config->show_processes = 1;
        config->show_network = 1;
        config->show_disk = 1;
        config->show_system = 1;
    }
}

/* 获取CPU使用率 */
double get_cpu_usage(void) {
    static uint64_t prev_total = 0;
    static uint64_t prev_idle = 0;
    uint64_t total = 0;
    uint64_t idle = 0;
    double usage = 0.0;

    /* 读取/proc/stat获取CPU统计 */
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) {
        return 0.0;
    }

    char line[256];
    if (fgets(line, sizeof(line), fp)) {
        uint64_t user, nice, system, idle_time, iowait, irq, softirq;

        sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu",
               &user, &nice, &system, &idle_time, &iowait, &irq, &softirq);

        idle = idle_time + iowait;
        total = user + nice + system + idle + irq + softirq;

        if (prev_total > 0) {
            uint64_t total_diff = total - prev_total;
            uint64_t idle_diff = idle - prev_idle;
            usage = (1.0 - (double)idle_diff / total_diff) * 100.0;
        }

        prev_total = total;
        prev_idle = idle;
    }

    fclose(fp);
    return usage;
}

/* 获取内存信息 */
void get_memory_info(uint64_t *total, uint64_t *free, uint64_t *used) {
    *total = 0;
    *free = 0;
    *used = 0;

    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line + 9, "%llu", total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line + 8, "%llu", free);
        } else if (strncmp(line, "MemAvailable:", 13) == 0) {
            uint64_t available;
            sscanf(line + 13, "%llu", &available);
            *used = *total - available;
            break;
        }
    }

    fclose(fp);

    if (*used == 0 && *total > 0) {
        *used = *total - *free;
    }
}

/* 获取进程信息 */
int get_process_count(void) {
    DIR *dir;
    struct dirent *ent;
    int count = 0;

    dir = opendir("/proc");
    if (!dir) {
        return 0;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_DIR) {
            char *endptr;
            strtol(ent->d_name, &endptr, 10);
            if (*endptr == '\0') { /* 是数字目录 */
                count++;
            }
        }
    }

    closedir(dir);
    return count;
}

/* 获取系统负载 */
void get_load_average(double *load1, double *load5, double *load15) {
    *load1 = 0.0;
    *load5 = 0.0;
    *load15 = 0.0;

    FILE *fp = fopen("/proc/loadavg", "r");
    if (!fp) {
        return;
    }

    fscanf(fp, "%lf %lf %lf", load1, load5, load15);
    fclose(fp);
}

/* 获取网络统计 */
void get_network_stats(uint64_t *rx_bytes, uint64_t *tx_bytes) {
    *rx_bytes = 0;
    *tx_bytes = 0;

    FILE *fp = fopen("/proc/net/dev", "r");
    if (!fp) {
        return;
    }

    char line[256];
    /* 跳过标题行 */
    fgets(line, sizeof(line), fp);
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        char interface[32];
        uint64_t rx, tx;

        sscanf(line, "%s %llu %*u %*u %*u %*u %*u %*u %*u %llu",
               interface, &rx, &tx);

        /* 只统计非lo接口 */
        if (strcmp(interface, "lo:") != 0) {
            *rx_bytes += rx;
            *tx_bytes += tx;
        }
    }

    fclose(fp);
}

/* 获取磁盘使用情况 */
void get_disk_usage(const char *path, uint64_t *total, uint64_t *used, uint64_t *free) {
    struct statvfs stat;

    if (statvfs(path, &stat) == 0) {
        *total = (uint64_t)stat.f_blocks * stat.f_frsize;
        *free = (uint64_t)stat.f_bavail * stat.f_frsize;
        *used = *total - *free;
    } else {
        *total = 0;
        *used = 0;
        *free = 0;
    }
}

/* 显示系统信息 */
void display_system_info(void) {
    printf("=== M4KK1 System Information ===\n");

    /* 系统基本信息 */
    printf("System: M4KK1 Operating System\n");
    printf("Kernel: v0.2.0-multarch\n");
    printf("Architecture: x86_64\n");
    printf("Uptime: Unknown\n");

    /* CPU信息 */
    printf("CPU: Unknown\n");
    printf("Cores: 1\n");

    /* 内存信息 */
    uint64_t mem_total, mem_free, mem_used;
    get_memory_info(&mem_total, &mem_free, &mem_used);
    printf("Memory: %llu MB total, %llu MB used, %llu MB free\n",
           mem_total / 1024 / 1024, mem_used / 1024 / 1024, mem_free / 1024 / 1024);

    /* 进程信息 */
    int process_count = get_process_count();
    printf("Processes: %d running\n", process_count);

    printf("================================\n");
}

/* 显示CPU信息 */
void display_cpu_info(void) {
    double cpu_usage = get_cpu_usage();
    double load1, load5, load15;

    get_load_average(&load1, &load5, &load15);

    printf("CPU Usage: %.1f%%\n", cpu_usage);
    printf("Load Average: %.2f, %.2f, %.2f\n", load1, load5, load15);
}

/* 显示内存信息 */
void display_memory_info(void) {
    uint64_t total, free, used;
    get_memory_info(&total, &free, &used);

    printf("Memory: %llu MB / %llu MB (%.1f%%)\n",
           used / 1024 / 1024, total / 1024 / 1024,
           (double)used / total * 100.0);
}

/* 显示进程信息 */
void display_process_info(void) {
    int count = get_process_count();
    printf("Processes: %d\n", count);
}

/* 显示网络信息 */
void display_network_info(void) {
    uint64_t rx_bytes, tx_bytes;
    get_network_stats(&rx_bytes, &tx_bytes);

    printf("Network: RX %llu bytes, TX %llu bytes\n",
           rx_bytes, tx_bytes);
}

/* 显示磁盘信息 */
void display_disk_info(void) {
    uint64_t total, used, free;
    get_disk_usage("/", &total, &used, &free);

    printf("Disk (/): %llu MB / %llu MB (%.1f%%)\n",
           used / 1024 / 1024, total / 1024 / 1024,
           total > 0 ? (double)used / total * 100.0 : 0.0);
}

/* 清屏 */
void clear_screen(void) {
    printf("\033[2J\033[H");
}

/* 主循环 */
void main_loop(m4ktop_config_t *config) {
    while (1) {
        if (config->mode == MODE_GRAPHICAL) {
            clear_screen();
        }

        printf("M4KK1 System Monitor (更新间隔: %ds)\n", config->interval);
        printf("=====================================\n");

        if (config->show_system) {
            display_system_info();
        } else {
            if (config->show_cpu) {
                display_cpu_info();
            }

            if (config->show_memory) {
                display_memory_info();
            }

            if (config->show_processes) {
                display_process_info();
            }

            if (config->show_network) {
                display_network_info();
            }

            if (config->show_disk) {
                display_disk_info();
            }
        }

        printf("\n按 Ctrl+C 退出...\n");

        sleep(config->interval);
    }
}

/* 主函数 */
int main(int argc, char *argv[]) {
    m4ktop_config_t config;

    /* 解析命令行参数 */
    parse_arguments(argc, argv, &config);

    /* 检查是否需要显示帮助 */
    if (argc == 1) {
        show_help(argv[0]);
        return 0;
    }

    /* 运行主循环 */
    main_loop(&config);

    return 0;
}
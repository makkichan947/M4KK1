/*
 * M4KK1 Ptdsk分区工具 - 主程序入口
 * 先进的图形化分区管理工具
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <locale.h>

#include "ptdsk.h"

/* 全局应用状态 */
static app_state_t *global_app = NULL;

/* 信号处理函数 */
void signal_handler(int signo)
{
    if (global_app) {
        switch (signo) {
            case SIGTERM:
            case SIGINT:
                /* 优雅地退出应用 */
                ptdsk_quit(global_app);
                break;
            case SIGUSR1:
                /* 刷新设备信息 */
                if (global_app->current_device) {
                    device_refresh_info(global_app->current_device);
                }
                break;
            case SIGUSR2:
                /* 保存当前状态 */
                if (global_app->config_file) {
                    config_save(global_app, global_app->config_file);
                }
                break;
        }
    }
}

/* 设置信号处理 */
void setup_signal_handlers(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sa.sa_flags = SA_RESTART;

    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
}

/* 打印帮助信息 */
void print_help(const char *program_name)
{
    printf("M4KK1 Ptdsk分区工具 %s\n", PTDSK_VERSION_STRING);
    printf("用法: %s [选项] [设备...]\n", program_name);
    printf("\n选项:\n");
    printf("  -h, --help              显示此帮助信息\n");
    printf("  -v, --version           显示版本信息\n");
    printf("  -c, --config <文件>     指定配置文件\n");
    printf("  -b, --backup <目录>     指定备份目录\n");
    printf("  -r, --readonly          只读模式\n");
    printf("  -e, --expert            专家模式\n");
    printf("  -l, --list              列出所有设备\n");
    printf("  -s, --scan              扫描设备\n");
    printf("  --no-gui               命令行模式\n");
    printf("\n设备:\n");
    printf("  /dev/sda               指定要操作的设备\n");
    printf("  /dev/sdb               多个设备用空格分隔\n");
    printf("\n示例:\n");
    printf("  %s                     启动图形界面\n", program_name);
    printf("  %s /dev/sda            操作指定设备\n", program_name);
    printf("  %s -l                  列出所有设备\n", program_name);
    printf("  %s -r /dev/sda         只读模式操作设备\n", program_name);
    printf("  %s --no-gui /dev/sda   命令行模式操作设备\n", program_name);
}

/* 打印版本信息 */
void print_version(void)
{
    printf("M4KK1 Ptdsk分区工具 %s\n", PTDSK_VERSION_STRING);
    printf("先进的图形化分区管理工具\n");
    printf("专为M4KK1操作系统优化\n");
    printf("支持GPT/MBR分区表、硬盘监控、坏道检测\n");
}

/* 解析命令行参数 */
typedef struct {
    char **devices;         /* 设备列表 */
    size_t device_count;    /* 设备数量 */
    char *config_file;      /* 配置文件 */
    char *backup_dir;       /* 备份目录 */
    bool readonly_mode;     /* 只读模式 */
    bool expert_mode;       /* 专家模式 */
    bool list_devices;      /* 列出设备 */
    bool scan_devices;      /* 扫描设备 */
    bool no_gui;            /* 无GUI模式 */
} options_t;

options_t *parse_options(int argc, char *argv[])
{
    static options_t opts;
    int opt;

    memset(&opts, 0, sizeof(opts));

    /* 命令行选项定义 */
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {"config", required_argument, 0, 'c'},
        {"backup", required_argument, 0, 'b'},
        {"readonly", no_argument, 0, 'r'},
        {"expert", no_argument, 0, 'e'},
        {"list", no_argument, 0, 'l'},
        {"scan", no_argument, 0, 's'},
        {"no-gui", no_argument, 0, 1},
        {0, 0, 0, 0}
    };

    /* 解析选项 */
    while ((opt = getopt_long(argc, argv, "hvc:b:rels", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_help(argv[0]);
                exit(0);
                break;
            case 'v':
                print_version();
                exit(0);
                break;
            case 'c':
                opts.config_file = strdup(optarg);
                break;
            case 'b':
                opts.backup_dir = strdup(optarg);
                break;
            case 'r':
                opts.readonly_mode = true;
                break;
            case 'e':
                opts.expert_mode = true;
                break;
            case 'l':
                opts.list_devices = true;
                break;
            case 's':
                opts.scan_devices = true;
                break;
            case 1:
                opts.no_gui = true;
                break;
            case '?':
                /* 未知选项，getopt_long会打印错误信息 */
                exit(1);
                break;
        }
    }

    /* 解析位置参数（设备） */
    for (int i = optind; i < argc; i++) {
        opts.device_count++;
        opts.devices = realloc(opts.devices, opts.device_count * sizeof(char*));
        opts.devices[opts.device_count - 1] = strdup(argv[i]);
    }

    return &opts;
}

/* 清理选项 */
void cleanup_options(options_t *opts)
{
    if (opts->config_file) free(opts->config_file);
    if (opts->backup_dir) free(opts->backup_dir);

    for (size_t i = 0; i < opts->device_count; i++) {
        free(opts->devices[i]);
    }
    if (opts->devices) free(opts->devices);
}

/* 列出设备信息 */
void list_devices(void)
{
    device_t **devices;
    size_t count;

    printf("扫描设备中...\n");
    devices = device_scan();
    if (!devices) {
        printf("未找到任何设备\n");
        return;
    }

    /* 计算设备数量 */
    for (count = 0; devices[count]; count++);

    printf("\n找到 %zu 个设备:\n\n", count);
    printf("%-12s %-8s %-10s %-8s %-10s %s\n",
           "设备", "类型", "大小", "健康", "分区数", "型号");
    printf("%-12s %-8s %-10s %-8s %-10s %s\n",
           "----------", "--------", "----------", "--------", "----------", "----------------");

    for (size_t i = 0; i < count; i++) {
        device_t *dev = devices[i];
        char size_str[16];
        const char *type_str = device_type_to_string(dev->type);
        const char *health_str = health_status_to_string(dev->health);

        snprintf(size_str, sizeof(size_str), "%.1fG",
                (double)dev->size / (1024*1024*1024));

        printf("%-12s %-8s %-10s %-8s %-10zu %s\n",
               dev->path, type_str, size_str, health_str,
               dev->partition_count, dev->model);

        /* 显示分区信息 */
        if (dev->partition_count > 0) {
            partition_t *part = dev->partitions;
            while (part) {
                printf("  %-10s %-8s %-8s %s\n",
                       part->name,
                       filesystem_type_to_string(part->fs_type),
                       part->is_mounted ? "已挂载" : "未挂载",
                       part->mount_point ? part->mount_point : "");
                part = part->next;
            }
        }
    }

    printf("\n");

    /* 释放设备列表 */
    for (size_t i = 0; i < count; i++) {
        device_close(devices[i]);
    }
    free(devices);
}

/* 扫描设备 */
void scan_devices(void)
{
    printf("开始设备扫描...\n");

    /* 这里实现设备扫描逻辑 */
    printf("设备扫描完成\n");
}

/* 初始化应用 */
bool app_initialize(app_state_t *app, options_t *opts)
{
    /* 设置语言环境 */
    setlocale(LC_ALL, "");

    /* 设置配置 */
    if (opts->config_file) {
        app->config_file = strdup(opts->config_file);
        if (!config_load(app, opts->config_file)) {
            fprintf(stderr, "警告: 无法加载配置文件 %s，使用默认配置\n", opts->config_file);
        }
    }

    if (opts->backup_dir) {
        app->backup_dir = strdup(opts->backup_dir);
    }

    app->readonly_mode = opts->readonly_mode;
    app->expert_mode = opts->expert_mode;

    /* 扫描设备 */
    if (opts->list_devices) {
        list_devices();
        return false; /* 不进入主循环 */
    }

    if (opts->scan_devices) {
        scan_devices();
        return false; /* 不进入主循环 */
    }

    /* 打开指定设备 */
    for (size_t i = 0; i < opts->device_count; i++) {
        device_t *device = device_open(opts->devices[i]);
        if (device) {
            app->devices[app->device_count] = device;
            app->device_count++;

            if (!app->current_device) {
                app->current_device = device;
            }
        } else {
            fprintf(stderr, "警告: 无法打开设备 %s\n", opts->devices[i]);
        }
    }

    /* 如果没有指定设备，扫描所有设备 */
    if (app->device_count == 0) {
        device_t **devices = device_scan();
        if (devices) {
            size_t i = 0;
            while (devices[i] && i < MAX_DEVICES) {
                app->devices[i] = devices[i];
                i++;
            }
            app->device_count = i;

            if (i > 0) {
                app->current_device = app->devices[0];
            }

            free(devices);
        }
    }

    return true;
}

/* 主函数 */
int main(int argc, char *argv[])
{
    int exit_code = 0;
    options_t *opts;

    /* 解析命令行参数 */
    opts = parse_options(argc, argv);

    /* 创建应用状态 */
    global_app = ptdsk_create();
    if (!global_app) {
        fprintf(stderr, "无法创建应用状态\n");
        cleanup_options(opts);
        return 1;
    }

    /* 设置信号处理 */
    setup_signal_handlers();

    /* 初始化应用 */
    if (!ptdsk_init(global_app)) {
        fprintf(stderr, "应用初始化失败\n");
        ptdsk_destroy(global_app);
        cleanup_options(opts);
        return 1;
    }

    /* 初始化应用配置和数据 */
    if (!app_initialize(global_app, opts)) {
        /* 如果返回false，表示只需要执行命令行操作，不进入主循环 */
        ptdsk_destroy(global_app);
        cleanup_options(opts);
        return 0;
    }

    /* 如果是无GUI模式，执行命令行操作 */
    if (opts->no_gui) {
        printf("命令行模式暂未实现\n");
        ptdsk_destroy(global_app);
        cleanup_options(opts);
        return 0;
    }

    /* 运行主界面 */
    ptdsk_run(global_app);

    /* 保存配置 */
    if (global_app->config_file) {
        config_save(global_app, global_app->config_file);
    }

    /* 清理资源 */
    ptdsk_destroy(global_app);
    cleanup_options(opts);

    return exit_code;
}
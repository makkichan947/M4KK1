/*
 * M4KK1 Ntmgr网络管理器 - 主程序入口
 * TUI界面的网络管理器
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <locale.h>

#include "ntmgr.h"

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
                ntmgr_quit(global_app);
                break;
            case SIGUSR1:
                /* 刷新网络状态 */
                if (global_app->config) {
                    /* 重新扫描网络接口 */
                    interface_scan();
                }
                break;
            case SIGUSR2:
                /* 保存当前配置 */
                if (global_app->config_file) {
                    config_save(global_app->config, global_app->config_file);
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
    printf("M4KK1 Ntmgr网络管理器 %s\n", NTMGR_VERSION_STRING);
    printf("用法: %s [选项]\n", program_name);
    printf("\n选项:\n");
    printf("  -h, --help              显示此帮助信息\n");
    printf("  -v, --version           显示版本信息\n");
    printf("  -c, --config <文件>     指定配置文件\n");
    printf("  -r, --readonly          只读模式\n");
    printf("  -e, --expert            专家模式\n");
    printf("  --no-gui               命令行模式\n");
    printf("\n示例:\n");
    printf("  %s                     启动TUI界面\n", program_name);
    printf("  %s -c /etc/ntmgr.conf  使用指定配置文件启动\n", program_name);
    printf("  %s -r                  以只读模式启动\n", program_name);
    printf("  %s --no-gui            命令行模式启动\n", program_name);
}

/* 打印版本信息 */
void print_version(void)
{
    printf("M4KK1 Ntmgr网络管理器 %s\n", NTMGR_VERSION_STRING);
    printf("先进的TUI网络管理器\n");
    printf("专为M4KK1操作系统优化\n");
    printf("支持网络接口、VPN、代理、防火墙管理\n");
}

/* 解析命令行参数 */
typedef struct {
    char *config_file;      /* 配置文件 */
    bool readonly_mode;     /* 只读模式 */
    bool expert_mode;       /* 专家模式 */
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
        {"readonly", no_argument, 0, 'r'},
        {"expert", no_argument, 0, 'e'},
        {"no-gui", no_argument, 0, 1},
        {0, 0, 0, 0}
    };

    /* 解析选项 */
    while ((opt = getopt_long(argc, argv, "hvc:re", long_options, NULL)) != -1) {
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
            case 'r':
                opts.readonly_mode = true;
                break;
            case 'e':
                opts.expert_mode = true;
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

    return &opts;
}

/* 清理选项 */
void cleanup_options(options_t *opts)
{
    if (opts->config_file) free(opts->config_file);
}

/* 初始化应用 */
bool app_initialize(app_state_t *app, options_t *opts)
{
    /* 设置语言环境 */
    setlocale(LC_ALL, "");

    /* 设置配置 */
    if (opts->config_file) {
        app->config_file = strdup(opts->config_file);
        if (!config_load(app->config, opts->config_file)) {
            fprintf(stderr, "警告: 无法加载配置文件 %s，使用默认配置\n", opts->config_file);
        }
    }

    /* 设置模式 */
    if (opts->readonly_mode) {
        /* 设置只读模式标志 */
    }

    if (opts->expert_mode) {
        /* 设置专家模式标志 */
    }

    /* 扫描网络接口 */
    app->interfaces = interface_scan();
    if (app->interfaces) {
        /* 计算接口数量 */
        for (app->interface_count = 0; app->interfaces[app->interface_count]; app->interface_count++);
    }

    /* 加载VPN连接 */
    /* 实现VPN连接加载逻辑 */

    /* 加载代理服务器 */
    /* 实现代理服务器加载逻辑 */

    /* 加载防火墙规则 */
    /* 实现防火墙规则加载逻辑 */

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
    global_app = ntmgr_create();
    if (!global_app) {
        fprintf(stderr, "无法创建应用状态\n");
        cleanup_options(opts);
        return 1;
    }

    /* 设置信号处理 */
    setup_signal_handlers();

    /* 初始化应用 */
    if (!ntmgr_init(global_app)) {
        fprintf(stderr, "应用初始化失败\n");
        ntmgr_destroy(global_app);
        cleanup_options(opts);
        return 1;
    }

    /* 初始化应用配置和数据 */
    if (!app_initialize(global_app, opts)) {
        fprintf(stderr, "应用配置初始化失败\n");
        ntmgr_destroy(global_app);
        cleanup_options(opts);
        return 1;
    }

    /* 如果是无GUI模式，使用命令行界面 */
    if (opts->no_gui) {
        printf("命令行模式暂未实现\n");
        ntmgr_destroy(global_app);
        cleanup_options(opts);
        return 0;
    }

    /* 运行TUI主界面 */
    ntmgr_run(global_app);

    /* 保存配置 */
    if (global_app->config_file) {
        config_save(global_app->config, global_app->config_file);
    }

    /* 清理资源 */
    ntmgr_destroy(global_app);
    cleanup_options(opts);

    return exit_code;
}
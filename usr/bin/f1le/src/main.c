/*
 * M4KK1 F1le文件管理器 - 主程序入口
 * 基于yazi设计的高性能终端文件管理器
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <locale.h>

#include "f1le.h"
#include "ui.h"

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
                f1le_quit(global_app);
                break;
            case SIGWINCH:
                /* 窗口大小改变，重新布局界面 */
                if (global_app->config) {
                    /* 重新计算界面布局 */
                    ui_calculate_layout(NULL, (rect_t){0, 0, 0, 0}); /* 需要实现 */
                }
                break;
            case SIGCONT:
                /* 继续运行，刷新界面 */
                if (global_app->config) {
                    ui_refresh(NULL); /* 需要实现 */
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
    sigaction(SIGWINCH, &sa, NULL);
    sigaction(SIGCONT, &sa, NULL);
}

/* 打印帮助信息 */
void print_help(const char *program_name)
{
    printf("M4KK1 F1le文件管理器 %s\n", F1LE_VERSION_STRING);
    printf("用法: %s [选项] [目录...]\n", program_name);
    printf("\n选项:\n");
    printf("  -h, --help              显示此帮助信息\n");
    printf("  -v, --version           显示版本信息\n");
    printf("  -c, --config <文件>     指定配置文件\n");
    printf("  -t, --theme <主题>      指定主题\n");
    printf("  -l, --layout <布局>     指定布局 (single/double)\n");
    printf("  -r, --readonly          只读模式\n");
    printf("  --no-plugins           禁用插件\n");
    printf("\n示例:\n");
    printf("  %s                     启动文件管理器\n", program_name);
    printf("  %s /home /tmp          在指定目录启动\n", program_name);
    printf("  %s -t dark -l double   使用暗色主题和双面板布局\n", program_name);
    printf("  %s --no-plugins /var   禁用插件并浏览/var目录\n", program_name);
}

/* 打印版本信息 */
void print_version(void)
{
    printf("M4KK1 F1le文件管理器 %s\n", F1LE_VERSION_STRING);
    printf("基于yazi设计理念的现代化文件管理器\n");
    printf("专为M4KK1操作系统优化\n");
}

/* 解析命令行参数 */
typedef struct {
    char **directories;     /* 目录列表 */
    size_t dir_count;       /* 目录数量 */
    char *config_file;      /* 配置文件 */
    char *theme;            /* 主题名称 */
    char *layout;           /* 布局类型 */
    bool readonly;          /* 只读模式 */
    bool no_plugins;        /* 禁用插件 */
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
        {"theme", required_argument, 0, 't'},
        {"layout", required_argument, 0, 'l'},
        {"readonly", no_argument, 0, 'r'},
        {"no-plugins", no_argument, 0, 1},
        {0, 0, 0, 0}
    };

    /* 解析选项 */
    while ((opt = getopt_long(argc, argv, "hvc:t:l:r", long_options, NULL)) != -1) {
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
            case 't':
                opts.theme = strdup(optarg);
                break;
            case 'l':
                opts.layout = strdup(optarg);
                break;
            case 'r':
                opts.readonly = true;
                break;
            case 1:
                opts.no_plugins = true;
                break;
            case '?':
                /* 未知选项，getopt_long会打印错误信息 */
                exit(1);
                break;
        }
    }

    /* 解析位置参数（目录） */
    for (int i = optind; i < argc; i++) {
        opts.dir_count++;
        opts.directories = realloc(opts.directories, opts.dir_count * sizeof(char*));
        opts.directories[opts.dir_count - 1] = strdup(argv[i]);
    }

    /* 如果没有指定目录，使用当前目录 */
    if (opts.dir_count == 0) {
        opts.dir_count = 1;
        opts.directories = malloc(sizeof(char*));
        opts.directories[0] = strdup(".");
    }

    return &opts;
}

/* 清理选项 */
void cleanup_options(options_t *opts)
{
    if (opts->config_file) free(opts->config_file);
    if (opts->theme) free(opts->theme);
    if (opts->layout) free(opts->layout);

    for (size_t i = 0; i < opts->dir_count; i++) {
        free(opts->directories[i]);
    }
    if (opts->directories) free(opts->directories);
}

/* 初始化应用 */
bool app_initialize(app_state_t *app, options_t *opts)
{
    /* 设置语言环境 */
    setlocale(LC_ALL, "");

    /* 加载配置 */
    if (opts->config_file) {
        if (!config_load(app->config, opts->config_file)) {
            fprintf(stderr, "警告: 无法加载配置文件 %s，使用默认配置\n", opts->config_file);
        }
    }

    /* 设置主题 */
    if (opts->theme) {
        /* 设置主题，需要实现 */
    }

    /* 设置布局 */
    if (opts->layout) {
        /* 设置布局，需要实现 */
    }

    /* 设置只读模式 */
    if (opts->readonly) {
        /* 设置只读标志，需要实现 */
    }

    /* 禁用插件 */
    if (opts->no_plugins) {
        /* 禁用插件系统，需要实现 */
    }

    /* 初始化书签和历史记录 */
    bookmark_load(app, "bookmarks.txt");
    history_load(app, "history.txt");

    /* 创建初始标签页 */
    for (size_t i = 0; i < opts->dir_count; i++) {
        if (app->tab_count < MAX_TABS) {
            tab_t *tab = tab_create(opts->directories[i], opts->directories[i]);
            if (tab) {
                app->tabs[app->tab_count] = tab;
                app->tab_count++;
            }
        }
    }

    /* 设置活动标签页 */
    if (app->tab_count > 0) {
        app->tabs[0]->active = true;
        app->active_tab = 0;
    }

    return true;
}

/* 主循环 */
void main_loop(app_state_t *app)
{
    int ch;

    while (app->running) {
        /* 刷新界面 */
        ui_refresh(NULL); /* 需要实现 */

        /* 读取键盘输入 */
        ch = getchar();

        /* 处理特殊按键 */
        switch (ch) {
            case 27: /* ESC */
                /* 处理ESC序列 */
                break;
            case 10: /* Enter */
                /* 处理回车键 */
                break;
            case 9: /* Tab */
                /* 处理Tab键 */
                break;
            case 127: /* Backspace */
                /* 处理退格键 */
                break;
            default:
                /* 处理普通字符 */
                ui_handle_key(NULL, ch); /* 需要实现 */
                break;
        }
    }
}

/* 主函数 */
int main(int argc, char *argv[])
{
    int exit_code = 0;
    options_t *opts;

    /* 解析命令行参数 */
    opts = parse_options(argc, argv);

    /* 创建应用状态 */
    global_app = f1le_create();
    if (!global_app) {
        fprintf(stderr, "无法创建应用状态\n");
        cleanup_options(opts);
        return 1;
    }

    /* 设置信号处理 */
    setup_signal_handlers();

    /* 初始化应用 */
    if (!f1le_init(global_app)) {
        fprintf(stderr, "应用初始化失败\n");
        f1le_destroy(global_app);
        cleanup_options(opts);
        return 1;
    }

    /* 初始化应用配置和数据 */
    if (!app_initialize(global_app, opts)) {
        fprintf(stderr, "应用配置初始化失败\n");
        f1le_destroy(global_app);
        cleanup_options(opts);
        return 1;
    }

    /* 运行主循环 */
    main_loop(global_app);

    /* 保存状态 */
    bookmark_save(global_app, "bookmarks.txt");
    history_save(global_app, "history.txt");

    /* 清理资源 */
    f1le_destroy(global_app);
    cleanup_options(opts);

    return exit_code;
}
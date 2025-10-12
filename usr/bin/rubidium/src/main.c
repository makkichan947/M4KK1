/*
 * M4KK1 Rubidium桌面环境 - 主程序入口
 * 平铺式桌面环境主程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <locale.h>

#include "rubidium.h"

/* 全局桌面环境实例 */
static desktop_environment_t *global_desktop = NULL;

/* 信号处理函数 */
void signal_handler(int signo)
{
    if (global_desktop) {
        switch (signo) {
            case SIGTERM:
            case SIGINT:
                /* 优雅地退出桌面环境 */
                rubidium_quit(global_desktop);
                break;
            case SIGUSR1:
                /* 重新加载配置 */
                if (global_desktop->config) {
                    config_reload(global_desktop->config);
                }
                break;
            case SIGUSR2:
                /* 显示统计信息 */
                desktop_print_stats(global_desktop);
                break;
            case SIGHUP:
                /* 重新加载主题 */
                if (global_desktop->theme) {
                    theme_reload(global_desktop->theme);
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
    sigaction(SIGHUP, &sa, NULL);
}

/* 打印帮助信息 */
void print_help(const char *program_name)
{
    printf("M4KK1 Rubidium桌面环境 %s\n", RUBIDIUM_VERSION_STRING);
    printf("用法: %s [选项]\n", program_name);
    printf("\n选项:\n");
    printf("  -h, --help              显示此帮助信息\n");
    printf("  -v, --version           显示版本信息\n");
    printf("  -c, --config <文件>     指定配置文件\n");
    printf("  -t, --theme <主题>      指定主题\n");
    printf("  -l, --layout <布局>     指定默认布局\n");
    printf("  -V, --verbose           详细输出\n");
    printf("  -d, --daemon            守护进程模式\n");
    printf("  --no-animation         禁用动画\n");
    printf("  --dry-run              试运行模式\n");
    printf("\n示例:\n");
    printf("  %s                      启动桌面环境\n", program_name);
    printf("  %s -c ~/.config/rubidium.conf 使用指定配置启动\n", program_name);
    printf("  %s -t dark -l master_stack  使用暗色主题和主从布局\n", program_name);
    printf("  %s --no-animation       禁用动画启动\n", program_name);
}

/* 打印版本信息 */
void print_version(void)
{
    printf("M4KK1 Rubidium桌面环境 %s\n", RUBIDIUM_VERSION_STRING);
    printf("先进的平铺式桌面环境\n");
    printf("专为M4KK1操作系统优化\n");
    printf("支持动态工作区、丰富动画、高度定制\n");
}

/* 解析命令行参数 */
typedef struct {
    char *config_file;      /* 配置文件 */
    char *theme;            /* 主题名称 */
    char *layout;           /* 布局类型 */
    bool verbose;           /* 详细输出 */
    bool daemon_mode;       /* 守护进程模式 */
    bool dry_run;           /* 试运行模式 */
    bool no_animation;      /* 禁用动画 */
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
        {"verbose", no_argument, 0, 'V'},
        {"daemon", no_argument, 0, 'd'},
        {"dry-run", no_argument, 0, 1},
        {"no-animation", no_argument, 0, 2},
        {0, 0, 0, 0}
    };

    /* 解析选项 */
    while ((opt = getopt_long(argc, argv, "hvc:t:l:V", long_options, NULL)) != -1) {
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
            case 'V':
                opts.verbose = true;
                break;
            case 'd':
                opts.daemon_mode = true;
                break;
            case 1:
                opts.dry_run = true;
                break;
            case 2:
                opts.no_animation = true;
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
    if (opts->theme) free(opts->theme);
    if (opts->layout) free(opts->layout);
}

/* 守护进程化 */
void daemonize(void)
{
    pid_t pid;

    /* 创建子进程 */
    pid = fork();
    if (pid < 0) {
        perror("创建守护进程失败");
        exit(1);
    }

    if (pid > 0) {
        /* 父进程退出 */
        exit(0);
    }

    /* 子进程继续运行 */

    /* 创建新会话 */
    if (setsid() < 0) {
        perror("创建会话失败");
        exit(1);
    }

    /* 改变工作目录 */
    if (chdir("/") < 0) {
        perror("改变工作目录失败");
        exit(1);
    }

    /* 重定向标准输入输出 */
    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    /* 设置umask */
    umask(0);
}

/* 初始化桌面环境 */
bool desktop_initialize(desktop_environment_t *desktop, options_t *opts)
{
    /* 设置语言环境 */
    setlocale(LC_ALL, "");

    /* 加载配置 */
    if (opts->config_file) {
        if (!config_load(desktop->config, opts->config_file)) {
            fprintf(stderr, "警告: 无法加载配置文件 %s，使用默认配置\n", opts->config_file);
        }
    }

    /* 设置主题 */
    if (opts->theme) {
        if (desktop->theme) {
            theme_set_name(desktop->theme, opts->theme);
        }
    }

    /* 设置布局 */
    if (opts->layout) {
        workspace_layout_t layout = workspace_layout_from_string(opts->layout);
        if (layout != WORKSPACE_LAYOUT_MASTER_STACK) {
            desktop->config->default_layout = layout;
        }
    }

    /* 禁用动画 */
    if (opts->no_animation) {
        desktop->config->animations_enabled = false;
    }

    /* 试运行模式检查 */
    if (opts->dry_run) {
        printf("试运行模式：检查配置和环境...\n");

        /* 检查Wayland显示服务器 */
        if (!getenv("WAYLAND_DISPLAY")) {
            printf("警告: 未检测到Wayland显示服务器\n");
        }

        /* 检查图形驱动 */
        if (access("/dev/dri/card0", R_OK) != 0) {
            printf("警告: 无法访问图形设备\n");
        }

        /* 检查权限 */
        if (geteuid() != 0) {
            printf("注意: 未以root权限运行，可能影响某些功能\n");
        }

        printf("试运行检查完成\n");
        return false; /* 不进入主循环 */
    }

    /* 初始化渲染系统 */
    if (!renderer_init(desktop->renderer)) {
        fprintf(stderr, "渲染系统初始化失败\n");
        return false;
    }

    /* 初始化合成器 */
    if (!compositor_init(desktop->compositor)) {
        fprintf(stderr, "合成器初始化失败\n");
        renderer_cleanup(desktop->renderer);
        return false;
    }

    /* 初始化输入管理器 */
    if (!input_manager_init(desktop->input_mgr)) {
        fprintf(stderr, "输入管理器初始化失败\n");
        compositor_cleanup(desktop->compositor);
        renderer_cleanup(desktop->renderer);
        return false;
    }

    /* 创建默认工作区 */
    workspace_t *default_workspace = workspace_create(1, "Default", desktop->config->default_layout);
    if (!default_workspace) {
        fprintf(stderr, "创建默认工作区失败\n");
        input_manager_cleanup(desktop->input_mgr);
        compositor_cleanup(desktop->compositor);
        renderer_cleanup(desktop->renderer);
        return false;
    }

    desktop->workspaces[desktop->workspace_count] = default_workspace;
    desktop->workspace_count++;
    desktop->active_workspace = default_workspace;

    /* 加载键盘快捷键 */
    if (!keybindings_load(desktop->keybindings, desktop->config)) {
        fprintf(stderr, "警告: 键盘快捷键加载失败\n");
    }

    /* 加载窗口规则 */
    if (!window_rules_load(desktop->config->window_rules, desktop->config)) {
        fprintf(stderr, "警告: 窗口规则加载失败\n");
    }

    /* 启动动画引擎 */
    if (desktop->config->animations_enabled) {
        animation_engine_start();
    }

    if (opts->verbose) {
        printf("Rubidium桌面环境初始化完成\n");
        printf("布局: %s\n", workspace_layout_to_string(desktop->config->default_layout));
        printf("主题: %s\n", desktop->theme ? desktop->theme->name : "default");
        printf("动画: %s\n", desktop->config->animations_enabled ? "启用" : "禁用");
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

    /* 创建桌面环境实例 */
    global_desktop = rubidium_create();
    if (!global_desktop) {
        fprintf(stderr, "无法创建桌面环境实例\n");
        cleanup_options(opts);
        return 1;
    }

    /* 设置信号处理 */
    setup_signal_handlers();

    /* 守护进程化 */
    if (opts->daemon_mode) {
        daemonize();
    }

    /* 初始化桌面环境 */
    if (!rubidium_init(global_desktop)) {
        fprintf(stderr, "桌面环境初始化失败\n");
        rubidium_destroy(global_desktop);
        cleanup_options(opts);
        return 1;
    }

    /* 初始化桌面环境配置和组件 */
    if (!desktop_initialize(global_desktop, opts)) {
        /* 如果返回false，表示只需要执行特殊操作，不进入主循环 */
        rubidium_destroy(global_desktop);
        cleanup_options(opts);
        return 0;
    }

    /* 运行桌面环境主循环 */
    rubidium_run(global_desktop);

    /* 保存配置 */
    if (global_desktop->config) {
        config_save(global_desktop->config, global_desktop->config->file_path);
    }

    /* 清理资源 */
    rubidium_destroy(global_desktop);
    cleanup_options(opts);

    return exit_code;
}
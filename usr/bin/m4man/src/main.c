/*
 * M4KK1 M4man手册系统 - 主程序入口
 * TUI界面的手册浏览器
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <locale.h>

#include "m4man.h"

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
                m4man_quit(global_app);
                break;
            case SIGWINCH:
                /* 窗口大小改变，重新布局界面 */
                /* 重新计算界面布局 */
                break;
            case SIGUSR1:
                /* 刷新文档显示 */
                /* 重新渲染当前页面 */
                break;
            case SIGUSR2:
                /* 保存当前状态 */
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
    sigaction(SIGWINCH, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
}

/* 打印帮助信息 */
void print_help(const char *program_name)
{
    printf("M4KK1 M4man手册浏览器 %s\n", M4MAN_VERSION_STRING);
    printf("用法: %s [选项] [文档...]\n", program_name);
    printf("\n选项:\n");
    printf("  -h, --help              显示此帮助信息\n");
    printf("  -v, --version           显示版本信息\n");
    printf("  -c, --config <文件>     指定配置文件\n");
    printf("  -t, --theme <主题>      指定主题\n");
    printf("  -f, --fullscreen        全屏模式\n");
    printf("  -r, --raw               原始模式（不渲染）\n");
    printf("  -s, --search <查询>     搜索指定内容\n");
    printf("\n文档:\n");
    printf("  man:ls                  指定man页面\n");
    printf("  info:gcc                指定info文档\n");
    printf("  /path/to/file.md        指定Markdown文件\n");
    printf("  /path/to/file.html      指定HTML文件\n");
    printf("\n示例:\n");
    printf("  %s                      启动手册浏览器\n", program_name);
    printf("  %s man:ls               浏览ls命令手册\n", program_name);
    printf("  %s -s \"printf\"          搜索包含printf的手册\n", program_name);
    printf("  %s -f /usr/share/doc/README.md 全屏浏览文档\n", program_name);
}

/* 打印版本信息 */
void print_version(void)
{
    printf("M4KK1 M4man手册浏览器 %s\n", M4MAN_VERSION_STRING);
    printf("先进的TUI手册浏览系统\n");
    printf("专为M4KK1操作系统优化\n");
    printf("支持多格式文档浏览、搜索、书签管理\n");
}

/* 解析命令行参数 */
typedef struct {
    char **documents;       /* 文档列表 */
    size_t document_count;  /* 文档数量 */
    char *config_file;      /* 配置文件 */
    char *theme;            /* 主题名称 */
    char *search_query;     /* 搜索查询 */
    bool fullscreen;        /* 全屏模式 */
    bool raw_mode;          /* 原始模式 */
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
        {"fullscreen", no_argument, 0, 'f'},
        {"raw", no_argument, 0, 'r'},
        {"search", required_argument, 0, 's'},
        {0, 0, 0, 0}
    };

    /* 解析选项 */
    while ((opt = getopt_long(argc, argv, "hvc:t:frs:", long_options, NULL)) != -1) {
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
            case 'f':
                opts.fullscreen = true;
                break;
            case 'r':
                opts.raw_mode = true;
                break;
            case 's':
                opts.search_query = strdup(optarg);
                break;
            case '?':
                /* 未知选项，getopt_long会打印错误信息 */
                exit(1);
                break;
        }
    }

    /* 解析位置参数（文档） */
    for (int i = optind; i < argc; i++) {
        opts.document_count++;
        opts.documents = realloc(opts.documents, opts.document_count * sizeof(char*));
        opts.documents[opts.document_count - 1] = strdup(argv[i]);
    }

    return &opts;
}

/* 清理选项 */
void cleanup_options(options_t *opts)
{
    if (opts->config_file) free(opts->config_file);
    if (opts->theme) free(opts->theme);
    if (opts->search_query) free(opts->search_query);

    for (size_t i = 0; i < opts->document_count; i++) {
        free(opts->documents[i]);
    }
    if (opts->documents) free(opts->documents);
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

    /* 设置主题 */
    if (opts->theme) {
        if (app->config) {
            free(app->config->theme);
            app->config->theme = strdup(opts->theme);
        }
    }

    /* 设置显示模式 */
    if (opts->fullscreen) {
        app->display_mode = DISPLAY_MODE_FULLSCREEN;
    } else if (opts->raw_mode) {
        app->display_mode = DISPLAY_MODE_RAW;
    }

    /* 加载书签和历史记录 */
    bookmark_load(app, "bookmarks.txt");
    history_load(app, "history.txt");

    /* 打开指定文档 */
    for (size_t i = 0; i < opts->document_count; i++) {
        document_t *doc = document_open(opts->documents[i]);
        if (doc) {
            app->documents[app->document_count] = doc;
            app->document_count++;

            if (!app->current_document) {
                app->current_document = doc;
            }
        } else {
            fprintf(stderr, "警告: 无法打开文档 %s\n", opts->documents[i]);
        }
    }

    /* 执行搜索 */
    if (opts->search_query) {
        size_t result_count;
        app->search_results = search_all_documents(opts->search_query, SEARCH_TYPE_TEXT, &result_count);
        app->search_result_count = result_count;

        if (result_count > 0) {
            /* 跳转到第一个搜索结果 */
            search_goto_result(app, app->search_results[0]);
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
    global_app = m4man_create();
    if (!global_app) {
        fprintf(stderr, "无法创建应用状态\n");
        cleanup_options(opts);
        return 1;
    }

    /* 设置信号处理 */
    setup_signal_handlers();

    /* 初始化应用 */
    if (!m4man_init(global_app)) {
        fprintf(stderr, "应用初始化失败\n");
        m4man_destroy(global_app);
        cleanup_options(opts);
        return 1;
    }

    /* 初始化应用配置和数据 */
    if (!app_initialize(global_app, opts)) {
        fprintf(stderr, "应用配置初始化失败\n");
        m4man_destroy(global_app);
        cleanup_options(opts);
        return 1;
    }

    /* 运行主界面 */
    m4man_run(global_app);

    /* 保存状态 */
    if (global_app->config_file) {
        config_save(global_app->config, global_app->config_file);
    }
    bookmark_save(global_app, "bookmarks.txt");
    history_save(global_app, "history.txt");

    /* 清理资源 */
    m4man_destroy(global_app);
    cleanup_options(opts);

    return exit_code;
}
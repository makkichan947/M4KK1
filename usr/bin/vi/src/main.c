/*
 * M4KK1 VI编辑器 - 主程序入口
 * 基于经典Vim的现代化文本编辑器
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>

#include "editor.h"
#include "buffer.h"
#include "display.h"
#include "input.h"

/* 全局编辑器实例 */
static editor_t *global_editor = NULL;

/* 信号处理函数 */
void signal_handler(int signo)
{
    if (global_editor) {
        switch (signo) {
            case SIGTERM:
            case SIGINT:
                /* 优雅地退出编辑器 */
                editor_quit(global_editor);
                exit(0);
                break;
            case SIGWINCH:
                /* 窗口大小改变，重新初始化显示 */
                display_resize(0, 0); /* 重新检测终端大小 */
                editor_refresh(global_editor);
                break;
            case SIGCONT:
                /* 继续运行，刷新显示 */
                editor_refresh(global_editor);
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
    printf("M4KK1 VI 编辑器 %s\n", VI_VERSION_STRING);
    printf("用法: %s [选项] [文件...]\n", program_name);
    printf("\n选项:\n");
    printf("  -h, --help              显示此帮助信息\n");
    printf("  -v, --version           显示版本信息\n");
    printf("  -c <命令>               执行指定命令后启动\n");
    printf("  -o <文件>               指定输出文件\n");
    printf("  -R                      只读模式\n");
    printf("  -n                      不使用交换文件\n");
    printf("  -u <vimrc>              使用指定配置文件\n");
    printf("  -N                      兼容模式\n");
    printf("  +<行号>                 跳转到指定行\n");
    printf("  +/<模式>                搜索指定模式\n");
    printf("\n示例:\n");
    printf("  %s file.txt             编辑文件\n", program_name);
    printf("  %s -c \"set nu\" file.txt 设置行号后编辑\n", program_name);
    printf("  %s +10 file.txt         跳转到第10行编辑\n", program_name);
    printf("  %s file1.txt file2.txt  编辑多个文件\n", program_name);
}

/* 打印版本信息 */
void print_version(void)
{
    printf("M4KK1 VI 编辑器 %s\n", VI_VERSION_STRING);
    printf("基于经典Vim的现代化实现\n");
    printf("专为M4KK1操作系统优化\n");
}

/* 解析命令行参数 */
typedef struct {
    char **files;           /* 文件列表 */
    size_t file_count;      /* 文件数量 */
    char *command;          /* 执行命令 */
    char *vimrc;            /* 配置文件 */
    char *output;           /* 输出文件 */
    long line_number;       /* 跳转行号 */
    char *search_pattern;   /* 搜索模式 */
    bool readonly;          /* 只读模式 */
    bool no_swap;           /* 不使用交换文件 */
    bool compatible;        /* 兼容模式 */
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
        {"command", required_argument, 0, 'c'},
        {"output", required_argument, 0, 'o'},
        {"vimrc", required_argument, 0, 'u'},
        {"readonly", no_argument, 0, 'R'},
        {"noswap", no_argument, 0, 'n'},
        {"compatible", no_argument, 0, 'N'},
        {0, 0, 0, 0}
    };

    /* 解析选项 */
    while ((opt = getopt_long(argc, argv, "hvc:o:u:RnN", long_options, NULL)) != -1) {
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
                opts.command = strdup(optarg);
                break;
            case 'o':
                opts.output = strdup(optarg);
                break;
            case 'u':
                opts.vimrc = strdup(optarg);
                break;
            case 'R':
                opts.readonly = true;
                break;
            case 'n':
                opts.no_swap = true;
                break;
            case 'N':
                opts.compatible = true;
                break;
            case '?':
                /* 未知选项，getopt_long会打印错误信息 */
                exit(1);
                break;
        }
    }

    /* 解析位置参数（行号和搜索模式） */
    for (int i = optind; i < argc; i++) {
        if (argv[i][0] == '+') {
            if (argv[i][1] == '/') {
                /* 搜索模式 */
                opts.search_pattern = strdup(argv[i] + 2);
            } else {
                /* 行号 */
                char *endptr;
                opts.line_number = strtol(argv[i] + 1, &endptr, 10);
                if (*endptr != '\0' || opts.line_number <= 0) {
                    fprintf(stderr, "无效的行号: %s\n", argv[i] + 1);
                    exit(1);
                }
            }
        } else {
            /* 文件名 */
            opts.file_count++;
            opts.files = realloc(opts.files, opts.file_count * sizeof(char*));
            opts.files[opts.file_count - 1] = strdup(argv[i]);
        }
    }

    return &opts;
}

/* 清理选项 */
void cleanup_options(options_t *opts)
{
    if (opts->command) free(opts->command);
    if (opts->vimrc) free(opts->vimrc);
    if (opts->output) free(opts->output);
    if (opts->search_pattern) free(opts->search_pattern);

    for (size_t i = 0; i < opts->file_count; i++) {
        free(opts->files[i]);
    }
    if (opts->files) free(opts->files);
}

/* 主函数 */
int main(int argc, char *argv[])
{
    int exit_code = 0;
    options_t *opts;

    /* 解析命令行参数 */
    opts = parse_options(argc, argv);

    /* 创建编辑器实例 */
    global_editor = editor_create();
    if (!global_editor) {
        fprintf(stderr, "无法创建编辑器实例\n");
        cleanup_options(opts);
        return 1;
    }

    /* 设置信号处理 */
    setup_signal_handlers();

    /* 初始化编辑器 */
    if (!editor_init(global_editor)) {
        fprintf(stderr, "编辑器初始化失败\n");
        editor_destroy(global_editor);
        cleanup_options(opts);
        return 1;
    }

    /* 加载文件 */
    for (size_t i = 0; i < opts->file_count; i++) {
        buffer_t *buffer = buffer_create_from_file(opts->files[i]);
        if (buffer) {
            /* 设置只读模式（如果指定） */
            if (opts->readonly) {
                buffer->readonly = true;
            }

            /* 跳转到指定行号 */
            if (opts->line_number > 0) {
                buffer_goto_line(buffer, opts->line_number - 1);
            }

            /* 搜索指定模式 */
            if (opts->search_pattern) {
                buffer_search(buffer, opts->search_pattern, 0, true);
            }
        }
    }

    /* 执行启动命令 */
    if (opts->command) {
        editor_execute_command(global_editor, opts->command);
    }

    /* 主编辑循环 */
    while (global_editor->mode != MODE_EXIT) {
        key_event_t *event;

        /* 读取键盘输入 */
        event = input_read_key();
        if (!event) {
            continue;
        }

        /* 处理输入事件 */
        if (!editor_process_key(global_editor, event->key)) {
            /* 处理特殊按键 */
            switch (event->key) {
                case KEY_ESCAPE:
                    input_handle_escape(global_editor);
                    break;
                case KEY_ENTER:
                    input_handle_enter(global_editor);
                    break;
                case KEY_TAB:
                    input_handle_tab(global_editor);
                    break;
                case KEY_BACKSPACE:
                    input_handle_backspace(global_editor);
                    break;
                case KEY_ARROW_UP:
                case KEY_ARROW_DOWN:
                case KEY_ARROW_LEFT:
                case KEY_ARROW_RIGHT:
                    input_handle_arrows(global_editor, event->key);
                    break;
                case KEY_F1 ... KEY_F12:
                    input_handle_function_keys(global_editor, event->key);
                    break;
                default:
                    /* 普通字符处理 */
                    if (event->key >= 32 && event->key < 127) {
                        /* 根据当前模式处理字符 */
                        switch (global_editor->mode) {
                            case MODE_INSERT:
                                buffer_insert_char(global_editor->current_buffer,
                                                 global_editor->current_buffer->cursor.offset,
                                                 event->key);
                                buffer_move_cursor(global_editor->current_buffer, 0, 1);
                                break;
                            case MODE_COMMAND:
                                /* 命令模式字符处理 */
                                break;
                            default:
                                /* 其他模式忽略字符输入 */
                                break;
                        }
                    }
                    break;
            }
        }

        /* 刷新显示 */
        editor_refresh(global_editor);

        /* 释放事件 */
        if (event->text) free(event->text);
        free(event);
    }

    /* 保存文件（如果有修改） */
    if (opts->output) {
        buffer_save_file(global_editor->current_buffer, opts->output);
    }

    /* 清理资源 */
    editor_destroy(global_editor);
    cleanup_options(opts);

    return exit_code;
}
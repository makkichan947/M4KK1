/**
 * M4KK1 cat - Concatenate and display files
 * 连接并显示文件内容
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>

#define BUFFER_SIZE 8192

/* 选项标志 */
static int show_line_numbers = 0;
static int show_ends = 0;
static int show_tabs = 0;
static int show_nonprinting = 0;
static int squeeze_blank = 0;
static int number_nonblank = 0;

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 cat - Concatenate and display files\n");
    printf("用法: cat [选项] [文件...]\n");
    printf("\n");
    printf("选项:\n");
    printf("  -n, --number         显示行号\n");
    printf("  -b, --number-nonblank 只对非空行显示行号\n");
    printf("  -E, --show-ends      在每行末尾显示$\n");
    printf("  -T, --show-tabs      将制表符显示为^I\n");
    printf("  -v, --show-nonprinting 显示非打印字符\n");
    printf("  -s, --squeeze-blank   将连续的空行压缩为一行\n");
    printf("  --help               显示此帮助信息\n");
}

/* 处理单个字符 */
void process_char(char c, int* line_num, int* blank_count, int is_blank_line) {
    /* 显示行号 */
    if (show_line_numbers && *line_num == 1) {
        printf("%6d\t", *line_num);
        (*line_num)++;
    } else if (number_nonblank && !is_blank_line && *line_num == 1) {
        printf("%6d\t", *line_num);
        (*line_num)++;
    }

    /* 处理制表符 */
    if (show_tabs && c == '\t') {
        printf("^I");
        return;
    }

    /* 处理非打印字符 */
    if (show_nonprinting && (c < 32 || c == 127) && c != '\t' && c != '\n') {
        if (c == 127) {
            printf("^?");
        } else {
            printf("^%c", c + 64);
        }
        return;
    }

    /* 正常输出字符 */
    putchar(c);

    /* 处理行结束 */
    if (c == '\n') {
        if (show_ends) {
            printf("$");
        }
        printf("\n");

        /* 重置行号状态 */
        *line_num = 1;

        /* 处理空行压缩 */
        if (squeeze_blank && is_blank_line) {
            (*blank_count)++;
            if (*blank_count > 1) {
                return; /* 跳过额外的空行 */
            }
        } else {
            *blank_count = 0;
        }
    }
}

/* 处理文件 */
int process_file(const char* filename) {
    FILE* file;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    int line_num = 1;
    int blank_count = 0;
    int in_blank_line = 1;

    if (strcmp(filename, "-") == 0) {
        file = stdin;
    } else {
        file = fopen(filename, "r");
        if (!file) {
            fprintf(stderr, "cat: %s: %s\n", filename, strerror(errno));
            return 1;
        }
    }

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            char c = buffer[i];
            int is_blank = (c == ' ' || c == '\t');

            if (c == '\n') {
                process_char(c, &line_num, &blank_count, in_blank_line);
                in_blank_line = 1;
            } else {
                if (!is_blank) {
                    in_blank_line = 0;
                }
                process_char(c, &line_num, &blank_count, in_blank_line);
            }
        }
    }

    if (file != stdin) {
        fclose(file);
    }

    return 0;
}

/* 主函数 */
int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"number", no_argument, 0, 'n'},
        {"number-nonblank", no_argument, 0, 'b'},
        {"show-ends", no_argument, 0, 'E'},
        {"show-tabs", no_argument, 0, 'T'},
        {"show-nonprinting", no_argument, 0, 'v'},
        {"squeeze-blank", no_argument, 0, 's'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "nbETvsh", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'n':
                show_line_numbers = 1;
                break;
            case 'b':
                number_nonblank = 1;
                break;
            case 'E':
                show_ends = 1;
                break;
            case 'T':
                show_tabs = 1;
                break;
            case 'v':
                show_nonprinting = 1;
                break;
            case 's':
                squeeze_blank = 1;
                break;
            case 'h':
                show_help();
                return 0;
            default:
                fprintf(stderr, "无效选项。使用 --help 查看帮助。\n");
                return 1;
        }
    }

    /* 如果没有指定文件，读取标准输入 */
    if (optind >= argc) {
        return process_file("-");
    }

    /* 处理所有指定的文件 */
    int exit_code = 0;
    for (int i = optind; i < argc; i++) {
        if (process_file(argv[i]) != 0) {
            exit_code = 1;
        }
    }

    return exit_code;
}
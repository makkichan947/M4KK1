/**
 * M4KK1 head - Output the first part of files
 * 输出文件的前部分
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>

#define DEFAULT_LINES 10
#define BUFFER_SIZE 8196

/* 选项标志 */
static int show_lines = DEFAULT_LINES;
static int show_bytes = 0;
static int quiet = 0;
static int verbose = 0;

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 head - Output the first part of files\n");
    printf("用法: head [选项] [文件...]\n");
    printf("\n");
    printf("选项:\n");
    printf("  -c, --bytes=[-]NUM       显示前NUM个字节\n");
    printf("  -n, --lines=[-]NUM       显示前NUM行（默认10行）\n");
    printf("  -q, --quiet              不显示文件名表头\n");
    printf("  -v, --verbose            总是显示文件名表头\n");
    printf("  --help                   显示此帮助信息\n");
}

/* 显示文件内容 */
int display_file(const char* filename, int multiple_files) {
    FILE* file;
    char buffer[BUFFER_SIZE];
    int lines_displayed = 0;
    int bytes_displayed = 0;
    int show_header = 0;

    /* 决定是否显示文件名表头 */
    if (multiple_files && !quiet) {
        show_header = 1;
    }
    if (verbose && !quiet) {
        show_header = 1;
    }

    if (strcmp(filename, "-") == 0) {
        file = stdin;
    } else {
        file = fopen(filename, "r");
        if (!file) {
            fprintf(stderr, "head: 无法打开 '%s': %s\n", filename, strerror(errno));
            return 1;
        }
    }

    /* 显示文件名表头 */
    if (show_header && strcmp(filename, "-") != 0) {
        printf("==> %s <==\n", filename);
    }

    /* 按行显示 */
    if (!show_bytes) {
        while (lines_displayed < show_lines && fgets(buffer, sizeof(buffer), file)) {
            fputs(buffer, stdout);
            lines_displayed++;
        }
    } else {
        /* 按字节显示 */
        size_t bytes_to_read;
        while (bytes_displayed < show_bytes) {
            if (show_bytes - bytes_displayed < BUFFER_SIZE) {
                bytes_to_read = show_bytes - bytes_displayed;
            } else {
                bytes_to_read = BUFFER_SIZE;
            }

            size_t bytes_read = fread(buffer, 1, bytes_to_read, file);
            if (bytes_read == 0) {
                break;
            }

            fwrite(buffer, 1, bytes_read, stdout);
            bytes_displayed += bytes_read;
        }
    }

    if (file != stdin) {
        fclose(file);
    }

    /* 多个文件之间添加空行 */
    if (multiple_files && show_header) {
        printf("\n");
    }

    return 0;
}

/* 解析数字参数 */
int parse_number(const char* str, int* result) {
    char* endptr;
    long num;

    if (*str == '-') {
        /* 负数表示从文件末尾开始 */
        num = strtol(str + 1, &endptr, 10);
        if (*endptr != '\0') {
            return -1;
        }
        *result = -num;
    } else {
        num = strtol(str, &endptr, 10);
        if (*endptr != '\0') {
            return -1;
        }
        *result = num;
    }

    return 0;
}

/* 主函数 */
int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"bytes", required_argument, 0, 'c'},
        {"lines", required_argument, 0, 'n'},
        {"quiet", no_argument, 0, 'q'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "c:n:qv", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'c':
                if (parse_number(optarg, &show_bytes) != 0) {
                    fprintf(stderr, "head: 无效的字节数: '%s'\n", optarg);
                    return 1;
                }
                break;
            case 'n':
                if (parse_number(optarg, &show_lines) != 0) {
                    fprintf(stderr, "head: 无效的行数: '%s'\n", optarg);
                    return 1;
                }
                break;
            case 'q':
                quiet = 1;
                verbose = 0;
                break;
            case 'v':
                verbose = 1;
                quiet = 0;
                break;
            case 'h':
                show_help();
                return 0;
            default:
                fprintf(stderr, "无效选项。使用 --help 查看帮助。\n");
                return 1;
        }
    }

    /* 检查参数冲突 */
    if (show_bytes && show_lines != DEFAULT_LINES) {
        fprintf(stderr, "head: 不能同时指定字节数和行数\n");
        return 1;
    }

    /* 处理文件 */
    if (optind >= argc) {
        /* 从标准输入读取 */
        display_file("-", 0);
    } else {
        /* 处理指定的文件 */
        int multiple_files = (argc - optind > 1);
        for (int i = optind; i < argc; i++) {
            display_file(argv[i], multiple_files);
        }
    }

    return 0;
}
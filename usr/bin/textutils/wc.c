/**
 * M4KK1 wc - Print newline, word, and byte counts for each file
 * 打印每个文件的换行符、单词和字节计数
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <errno.h>

#define BUFFER_SIZE 8192

/* 统计信息结构 */
typedef struct {
    unsigned long lines;
    unsigned long words;
    unsigned long bytes;
    unsigned long chars;
    unsigned long max_line_length;
} count_info_t;

/* 选项标志 */
static int count_lines = 0;
static int count_words = 0;
static int count_bytes = 0;
static int count_chars = 0;
static int count_max_line = 0;

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 wc - Print newline, word, and byte counts for each file\n");
    printf("用法: wc [选项] [文件...]\n");
    printf("\n");
    printf("选项:\n");
    printf("  -c, --bytes      显示字节数\n");
    printf("  -m, --chars      显示字符数\n");
    printf("  -l, --lines      显示行数\n");
    printf("  -w, --words      显示单词数\n");
    printf("  -L, --max-line-length 显示最长行的长度\n");
    printf("  --help           显示此帮助信息\n");
    printf("\n");
    printf("如果不指定选项，则显示行数、单词数和字节数。\n");
}

/* 统计文件内容 */
int count_file(const char* filename, count_info_t* info) {
    FILE* file;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    int in_word = 0;
    unsigned long current_line_length = 0;

    memset(info, 0, sizeof(count_info_t));

    if (strcmp(filename, "-") == 0) {
        file = stdin;
    } else {
        file = fopen(filename, "r");
        if (!file) {
            fprintf(stderr, "wc: %s: %s\n", filename, strerror(errno));
            return 1;
        }
    }

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        info->bytes += bytes_read;

        for (size_t i = 0; i < bytes_read; i++) {
            char c = buffer[i];
            info->chars++;

            if (c == '\n') {
                info->lines++;
                if (current_line_length > info->max_line_length) {
                    info->max_line_length = current_line_length;
                }
                current_line_length = 0;
                in_word = 0;
            } else {
                current_line_length++;

                /* 统计单词 */
                if (isspace(c)) {
                    in_word = 0;
                } else if (!in_word) {
                    in_word = 1;
                    info->words++;
                }
            }
        }
    }

    /* 处理最后一行（如果没有换行符结尾） */
    if (current_line_length > 0) {
        if (current_line_length > info->max_line_length) {
            info->max_line_length = current_line_length;
        }
        /* 如果文件以非换行符结尾，我们需要计算这一行 */
        info->lines++; /* 实际上这行没有换行符，但算作一行 */
    }

    if (file != stdin) {
        fclose(file);
    }

    return 0;
}

/* 显示统计信息 */
void display_counts(const count_info_t* info, const char* filename, int multiple_files) {
    if (count_lines) {
        printf("%8lu ", info->lines);
    }
    if (count_words) {
        printf("%8lu ", info->words);
    }
    if (count_chars) {
        printf("%8lu ", info->chars);
    }
    if (count_bytes) {
        printf("%8lu ", info->bytes);
    }
    if (count_max_line) {
        printf("%8lu ", info->max_line_length);
    }

    /* 如果没有指定任何选项，显示默认的三列 */
    if (!count_lines && !count_words && !count_bytes && !count_chars && !count_max_line) {
        printf("%8lu %8lu %8lu ", info->lines, info->words, info->bytes);
    }

    if (filename && strcmp(filename, "-") != 0) {
        printf("%s\n", filename);
    } else {
        printf("\n");
    }
}

/* 显示总计 */
void display_total(const count_info_t* total, int file_count) {
    if (count_lines) {
        printf("%8lu ", total->lines);
    }
    if (count_words) {
        printf("%8lu ", total->words);
    }
    if (count_chars) {
        printf("%8lu ", total->chars);
    }
    if (count_bytes) {
        printf("%8lu ", total->bytes);
    }
    if (count_max_line) {
        printf("%8lu ", total->max_line_length);
    }

    if (!count_lines && !count_words && !count_bytes && !count_chars && !count_max_line) {
        printf("%8lu %8lu %8lu ", total->lines, total->words, total->bytes);
    }

    printf("total\n");
}

/* 主函数 */
int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;
    count_info_t total_info;
    int file_count = 0;

    static struct option long_options[] = {
        {"bytes", no_argument, 0, 'c'},
        {"chars", no_argument, 0, 'm'},
        {"lines", no_argument, 0, 'l'},
        {"words", no_argument, 0, 'w'},
        {"max-line-length", no_argument, 0, 'L'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "cmlwL", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'c':
                count_bytes = 1;
                break;
            case 'm':
                count_chars = 1;
                break;
            case 'l':
                count_lines = 1;
                break;
            case 'w':
                count_words = 1;
                break;
            case 'L':
                count_max_line = 1;
                break;
            case 'h':
                show_help();
                return 0;
            default:
                fprintf(stderr, "无效选项。使用 --help 查看帮助。\n");
                return 1;
        }
    }

    memset(&total_info, 0, sizeof(total_info));

    /* 处理文件 */
    if (optind >= argc) {
        /* 从标准输入读取 */
        count_info_t info;
        if (count_file("-", &info) == 0) {
            display_counts(&info, NULL, 0);
        }
    } else {
        /* 处理指定的文件 */
        for (int i = optind; i < argc; i++) {
            count_info_t info;

            if (count_file(argv[i], &info) == 0) {
                display_counts(&info, argv[i], argc - optind > 1);

                /* 累加总计 */
                total_info.lines += info.lines;
                total_info.words += info.words;
                total_info.bytes += info.bytes;
                total_info.chars += info.chars;
                if (info.max_line_length > total_info.max_line_length) {
                    total_info.max_line_length = info.max_line_length;
                }

                file_count++;
            }
        }

        /* 显示总计（多个文件时） */
        if (file_count > 1) {
            display_total(&total_info, file_count);
        }
    }

    return 0;
}
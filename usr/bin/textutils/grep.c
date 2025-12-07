/**
 * M4KK1 grep - Print lines matching a pattern
 * 打印匹配模式的行
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <regex.h>
#include <errno.h>

#define MAX_LINE_LEN 4096
#define MAX_PATTERN_LEN 1024

/* 选项标志 */
static int ignore_case = 0;
static int invert_match = 0;
static int line_numbers = 0;
static int quiet = 0;
static int count_only = 0;
static int files_with_matches = 0;
static int files_without_matches = 0;
static int no_filename = 0;
static int recursive = 0;
static int fixed_strings = 0;

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 grep - Print lines matching a pattern\n");
    printf("用法: grep [选项] PATTERN [文件...]\n");
    printf("\n");
    printf("选项:\n");
    printf("  -i, --ignore-case         忽略大小写\n");
    printf("  -v, --invert-match        反转匹配\n");
    printf("  -n, --line-number         显示行号\n");
    printf("  -q, --quiet               安静模式\n");
    printf("  -c, --count               只显示匹配行数\n");
    printf("  -l, --files-with-matches  只显示包含匹配的文件名\n");
    printf("  -L, --files-without-match 只显示不包含匹配的文件名\n");
    printf("  -h, --no-filename         不显示文件名\n");
    printf("  -F, --fixed-strings       固定字符串匹配\n");
    printf("  --help                    显示此帮助信息\n");
}

/* 编译正则表达式 */
regex_t* compile_pattern(const char* pattern) {
    regex_t* regex;
    int flags = REG_EXTENDED | REG_NOSUB;

    if (ignore_case) {
        flags |= REG_ICASE;
    }

    regex = malloc(sizeof(regex_t));
    if (!regex) {
        perror("malloc");
        return NULL;
    }

    if (regcomp(regex, pattern, flags) != 0) {
        fprintf(stderr, "grep: 无效的正则表达式: %s\n", pattern);
        free(regex);
        return NULL;
    }

    return regex;
}

/* 处理单个文件 */
int process_file(const char* filename, const char* pattern, regex_t* regex, int multiple_files) {
    FILE* file;
    char line[MAX_LINE_LEN];
    int line_num = 0;
    int match_count = 0;
    int has_matches = 0;

    if (strcmp(filename, "-") == 0) {
        file = stdin;
    } else {
        file = fopen(filename, "r");
        if (!file) {
            if (!quiet) {
                fprintf(stderr, "grep: %s: %s\n", filename, strerror(errno));
            }
            return 0;
        }
    }

    /* 处理每一行 */
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        int is_match = 0;

        /* 检查是否匹配 */
        if (fixed_strings) {
            /* 固定字符串匹配 */
            if (ignore_case) {
                is_match = (strcasestr(line, pattern) != NULL);
            } else {
                is_match = (strstr(line, pattern) != NULL);
            }
        } else {
            /* 正则表达式匹配 */
            is_match = (regexec(regex, line, 0, NULL, 0) == 0);
        }

        /* 应用反转匹配 */
        if (invert_match) {
            is_match = !is_match;
        }

        if (is_match) {
            has_matches = 1;
            match_count++;

            /* 根据选项决定输出 */
            if (!count_only && !files_with_matches && !files_without_matches) {
                if (multiple_files && !no_filename) {
                    printf("%s:", filename);
                }
                if (line_numbers) {
                    printf("%d:", line_num);
                }
                printf("%s", line);
                /* fgets已经包含换行符 */
            }
        }
    }

    if (file != stdin) {
        fclose(file);
    }

    /* 输出统计信息 */
    if (count_only) {
        if (multiple_files && !no_filename) {
            printf("%s:", filename);
        }
        printf("%d\n", match_count);
    }

    if (files_with_matches && has_matches) {
        printf("%s\n", filename);
    }

    if (files_without_matches && !has_matches) {
        printf("%s\n", filename);
    }

    return has_matches ? 1 : 0;
}

/* 主函数 */
int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;
    char* pattern = NULL;
    regex_t* regex = NULL;
    int exit_code = 1; /* 默认无匹配 */

    static struct option long_options[] = {
        {"ignore-case", no_argument, 0, 'i'},
        {"invert-match", no_argument, 0, 'v'},
        {"line-number", no_argument, 0, 'n'},
        {"quiet", no_argument, 0, 'q'},
        {"count", no_argument, 0, 'c'},
        {"files-with-matches", no_argument, 0, 'l'},
        {"files-without-match", no_argument, 0, 'L'},
        {"no-filename", no_argument, 0, 'h'},
        {"fixed-strings", no_argument, 0, 'F'},
        {"help", no_argument, 0, 'H'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "ivnqclLhF", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'i':
                ignore_case = 1;
                break;
            case 'v':
                invert_match = 1;
                break;
            case 'n':
                line_numbers = 1;
                break;
            case 'q':
                quiet = 1;
                break;
            case 'c':
                count_only = 1;
                break;
            case 'l':
                files_with_matches = 1;
                break;
            case 'L':
                files_without_matches = 1;
                break;
            case 'h':
                no_filename = 1;
                break;
            case 'F':
                fixed_strings = 1;
                break;
            case 'H':
                show_help();
                return 0;
            default:
                fprintf(stderr, "无效选项。使用 --help 查看帮助。\n");
                return 2;
        }
    }

    /* 检查参数 */
    if (optind >= argc) {
        fprintf(stderr, "grep: 需要指定搜索模式\n");
        fprintf(stderr, "使用 'grep --help' 查看更多信息。\n");
        return 2;
    }

    pattern = argv[optind++];
    int multiple_files = (optind < argc - 1) || (optind == argc && strcmp(argv[optind-1], "-") != 0);

    /* 编译正则表达式（如果不是固定字符串） */
    if (!fixed_strings) {
        regex = compile_pattern(pattern);
        if (!regex) {
            return 2;
        }
    }

    /* 处理文件 */
    if (optind >= argc) {
        /* 从标准输入读取 */
        exit_code = process_file("-", pattern, regex, 0);
    } else {
        /* 处理指定的文件 */
        for (int i = optind; i < argc; i++) {
            int result = process_file(argv[i], pattern, regex, multiple_files);
            if (result) {
                exit_code = 0; /* 找到匹配 */
            }
        }
    }

    /* 清理资源 */
    if (regex) {
        regfree(regex);
        free(regex);
    }

    return exit_code;
}
/**
 * M4KK1 tail - Output the last part of files
 * 输出文件的最后部分
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <sys/stat.h>

#define DEFAULT_LINES 10
#define BUFFER_SIZE 8196
#define MAX_LINES 10000

/* 行缓冲结构 */
typedef struct {
    char** lines;
    int count;
    int capacity;
} line_buffer_t;

/* 选项标志 */
static int show_lines = DEFAULT_LINES;
static int show_bytes = 0;
static int follow = 0;
static int quiet = 0;
static int verbose = 0;

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 tail - Output the last part of files\n");
    printf("用法: tail [选项] [文件...]\n");
    printf("\n");
    printf("选项:\n");
    printf("  -c, --bytes=[+]NUM       显示最后NUM个字节\n");
    printf("  -n, --lines=[+]NUM       显示最后NUM行（默认10行）\n");
    printf("  -f, --follow             跟踪文件变化\n");
    printf("  -q, --quiet              不显示文件名表头\n");
    printf("  -v, --verbose            总是显示文件名表头\n");
    printf("  --help                   显示此帮助信息\n");
}

/* 初始化行缓冲 */
line_buffer_t* line_buffer_create(int capacity) {
    line_buffer_t* buffer = malloc(sizeof(line_buffer_t));
    if (!buffer) return NULL;

    buffer->lines = malloc(sizeof(char*) * capacity);
    if (!buffer->lines) {
        free(buffer);
        return NULL;
    }

    buffer->count = 0;
    buffer->capacity = capacity;
    return buffer;
}

/* 销毁行缓冲 */
void line_buffer_destroy(line_buffer_t* buffer) {
    if (!buffer) return;

    for (int i = 0; i < buffer->count; i++) {
        free(buffer->lines[i]);
    }
    free(buffer->lines);
    free(buffer);
}

/* 添加行到缓冲 */
void line_buffer_add(line_buffer_t* buffer, const char* line) {
    if (buffer->count >= buffer->capacity) {
        /* 移除最旧的行 */
        free(buffer->lines[0]);
        memmove(buffer->lines, buffer->lines + 1, sizeof(char*) * (buffer->capacity - 1));
        buffer->count--;
    }

    buffer->lines[buffer->count] = strdup(line);
    if (buffer->lines[buffer->count]) {
        buffer->count++;
    }
}

/* 显示缓冲中的行 */
void line_buffer_display(line_buffer_t* buffer) {
    for (int i = 0; i < buffer->count; i++) {
        fputs(buffer->lines[i], stdout);
    }
}

/* 显示文件最后N行 */
int display_file_tail_lines(const char* filename, int multiple_files) {
    FILE* file;
    line_buffer_t* buffer;
    char line[BUFFER_SIZE];
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
            fprintf(stderr, "tail: 无法打开 '%s': %s\n", filename, strerror(errno));
            return 1;
        }
    }

    buffer = line_buffer_create(show_lines > 0 ? show_lines : -show_lines);
    if (!buffer) {
        if (file != stdin) fclose(file);
        return 1;
    }

    /* 读取所有行到缓冲 */
    while (fgets(line, sizeof(line), file)) {
        line_buffer_add(buffer, line);
    }

    if (file != stdin) {
        fclose(file);
    }

    /* 显示文件名表头 */
    if (show_header && strcmp(filename, "-") != 0) {
        printf("==> %s <==\n", filename);
    }

    /* 显示缓冲中的行 */
    line_buffer_display(buffer);

    line_buffer_destroy(buffer);

    /* 多个文件之间添加空行 */
    if (multiple_files && show_header) {
        printf("\n");
    }

    return 0;
}

/* 显示文件最后N字节 */
int display_file_tail_bytes(const char* filename, int multiple_files) {
    FILE* file;
    char* buffer;
    long file_size;
    long bytes_to_read;
    int show_header = 0;

    /* 决定是否显示文件名表头 */
    if (multiple_files && !quiet) {
        show_header = 1;
    }
    if (verbose && !quiet) {
        show_header = 1;
    }

    if (strcmp(filename, "-") == 0) {
        /* 从标准输入读取，暂时不支持 */
        fprintf(stderr, "tail: 从标准输入按字节显示暂不支持\n");
        return 1;
    }

    file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "tail: 无法打开 '%s': %s\n", filename, strerror(errno));
        return 1;
    }

    /* 获取文件大小 */
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);

    if (show_bytes > 0) {
        bytes_to_read = show_bytes;
    } else {
        bytes_to_read = -show_bytes;
    }

    if (bytes_to_read > file_size) {
        bytes_to_read = file_size;
    }

    /* 分配缓冲 */
    buffer = malloc(bytes_to_read + 1);
    if (!buffer) {
        fclose(file);
        return 1;
    }

    /* 读取最后N字节 */
    fseek(file, -bytes_to_read, SEEK_END);
    size_t bytes_read = fread(buffer, 1, bytes_to_read, file);
    buffer[bytes_read] = '\0';

    fclose(file);

    /* 显示文件名表头 */
    if (show_header) {
        printf("==> %s <==\n", filename);
    }

    /* 输出内容 */
    fwrite(buffer, 1, bytes_read, stdout);

    free(buffer);

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

    if (*str == '+') {
        /* 正数表示从文件开头开始 */
        num = strtol(str + 1, &endptr, 10);
        if (*endptr != '\0') {
            return -1;
        }
        *result = num;
    } else if (*str == '-') {
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
        {"follow", no_argument, 0, 'f'},
        {"quiet", no_argument, 0, 'q'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "c:n:fvq", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'c':
                if (parse_number(optarg, &show_bytes) != 0) {
                    fprintf(stderr, "tail: 无效的字节数: '%s'\n", optarg);
                    return 1;
                }
                break;
            case 'n':
                if (parse_number(optarg, &show_lines) != 0) {
                    fprintf(stderr, "tail: 无效的行数: '%s'\n", optarg);
                    return 1;
                }
                break;
            case 'f':
                follow = 1;
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
        fprintf(stderr, "tail: 不能同时指定字节数和行数\n");
        return 1;
    }

    /* 处理文件 */
    if (optind >= argc) {
        /* 从标准输入读取 */
        display_file_tail_lines("-", 0);
    } else {
        /* 处理指定的文件 */
        int multiple_files = (argc - optind > 1);
        for (int i = optind; i < argc; i++) {
            if (show_bytes) {
                display_file_tail_bytes(argv[i], multiple_files);
            } else {
                display_file_tail_lines(argv[i], multiple_files);
            }
        }
    }

    return 0;
}
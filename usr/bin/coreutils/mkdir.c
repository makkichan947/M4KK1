/**
 * M4KK1 mkdir - Make directories
 * 创建目录
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <getopt.h>

#define MAX_PATH_LEN 4096

/* 选项标志 */
static int create_parents = 0;
static int verbose = 0;
static int mode = 0755;

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 mkdir - Make directories\n");
    printf("用法: mkdir [选项] 目录...\n");
    printf("\n");
    printf("选项:\n");
    printf("  -p, --parents     创建父目录（如需要）\n");
    printf("  -v, --verbose     详细输出\n");
    printf("  -m, --mode=模式   设置目录权限（八进制）\n");
    printf("  --help            显示此帮助信息\n");
}

/* 创建单个目录 */
int create_directory(const char* path) {
    struct stat st;

    /* 检查目录是否已存在 */
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            /* 目录已存在，不是错误 */
            if (verbose) {
                printf("mkdir: 目录 '%s' 已存在\n", path);
            }
            return 0;
        } else {
            /* 存在同名文件 */
            fprintf(stderr, "mkdir: 无法创建目录 '%s': 文件已存在\n", path);
            return 1;
        }
    }

    /* 创建目录 */
    if (mkdir(path, mode) == -1) {
        fprintf(stderr, "mkdir: 无法创建目录 '%s': %s\n", path, strerror(errno));
        return 1;
    }

    if (verbose) {
        printf("mkdir: 创建目录 '%s'\n", path);
    }

    return 0;
}

/* 递归创建目录 */
int create_directory_recursive(const char* path) {
    char temp[MAX_PATH_LEN];
    char* p;
    struct stat st;
    int result = 0;

    /* 复制路径 */
    strncpy(temp, path, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';

    /* 逐级创建目录 */
    for (p = temp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';

            /* 检查这一级目录是否存在 */
            if (stat(temp, &st) != 0) {
                /* 目录不存在，创建它 */
                if (mkdir(temp, mode) == -1) {
                    fprintf(stderr, "mkdir: 无法创建目录 '%s': %s\n", temp, strerror(errno));
                    result = 1;
                    *p = '/';
                    break;
                }

                if (verbose) {
                    printf("mkdir: 创建目录 '%s'\n", temp);
                }
            } else if (!S_ISDIR(st.st_mode)) {
                /* 存在同名文件 */
                fprintf(stderr, "mkdir: 无法创建目录 '%s': 文件已存在\n", temp);
                result = 1;
                *p = '/';
                break;
            }

            *p = '/';
        }
    }

    /* 创建最终目录 */
    if (result == 0) {
        result = create_directory(path);
    }

    return result;
}

/* 主函数 */
int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"parents", no_argument, 0, 'p'},
        {"verbose", no_argument, 0, 'v'},
        {"mode", required_argument, 0, 'm'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "pvm:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'p':
                create_parents = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'm':
                mode = strtol(optarg, NULL, 8);
                break;
            case 'h':
                show_help();
                return 0;
            default:
                fprintf(stderr, "无效选项。使用 --help 查看帮助。\n");
                return 1;
        }
    }

    /* 检查参数 */
    if (optind >= argc) {
        fprintf(stderr, "mkdir: 缺少操作数\n");
        fprintf(stderr, "使用 'mkdir --help' 查看更多信息。\n");
        return 1;
    }

    /* 处理所有目录参数 */
    int exit_code = 0;
    for (int i = optind; i < argc; i++) {
        int result;
        if (create_parents) {
            result = create_directory_recursive(argv[i]);
        } else {
            result = create_directory(argv[i]);
        }

        if (result != 0) {
            exit_code = 1;
        }
    }

    return exit_code;
}
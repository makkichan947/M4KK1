/**
 * M4KK1 pwd - Print working directory
 * 显示当前工作目录
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>

#define MAX_PATH_LEN PATH_MAX

/* 选项标志 */
static int logical = 0;
static int physical = 0;

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 pwd - Print working directory\n");
    printf("用法: pwd [选项]\n");
    printf("\n");
    printf("选项:\n");
    printf("  -L, --logical   使用逻辑路径（默认，通过符号链接）\n");
    printf("  -P, --physical  使用物理路径（解析符号链接）\n");
    printf("  --help          显示此帮助信息\n");
}

/* 获取当前工作目录 */
char* get_current_directory(void) {
    char* cwd;
    size_t size = MAX_PATH_LEN;

    cwd = malloc(size);
    if (!cwd) {
        perror("malloc");
        return NULL;
    }

    while (1) {
        if (getcwd(cwd, size) != NULL) {
            return cwd;
        }

        if (errno != ERANGE) {
            perror("getcwd");
            free(cwd);
            return NULL;
        }

        /* 缓冲区太小，扩大它 */
        size *= 2;
        char* new_cwd = realloc(cwd, size);
        if (!new_cwd) {
            perror("realloc");
            free(cwd);
            return NULL;
        }
        cwd = new_cwd;
    }
}

/* 主函数 */
int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"logical", no_argument, 0, 'L'},
        {"physical", no_argument, 0, 'P'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "LP", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'L':
                logical = 1;
                physical = 0;
                break;
            case 'P':
                physical = 1;
                logical = 0;
                break;
            case 'h':
                show_help();
                return 0;
            default:
                fprintf(stderr, "无效选项。使用 --help 查看帮助。\n");
                return 1;
        }
    }

    /* 检查多余的参数 */
    if (optind < argc) {
        fprintf(stderr, "pwd: 多余的操作数 '%s'\n", argv[optind]);
        fprintf(stderr, "使用 'pwd --help' 查看更多信息。\n");
        return 1;
    }

    /* 获取当前工作目录 */
    char* cwd = get_current_directory();
    if (!cwd) {
        return 1;
    }

    /* 输出结果 */
    printf("%s\n", cwd);
    free(cwd);

    return 0;
}
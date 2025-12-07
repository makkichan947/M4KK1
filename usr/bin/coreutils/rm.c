/**
 * M4KK1 rm - Remove files or directories
 * 删除文件或目录
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_PATH_LEN 4096

/* 选项标志 */
static int force = 0;
static int interactive = 0;
static int recursive = 0;
static int verbose = 0;
static int one_fs = 0;
static int preserve_root = 1;

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 rm - Remove files or directories\n");
    printf("用法: rm [选项] 文件...\n");
    printf("\n");
    printf("选项:\n");
    printf("  -f, --force           强制删除，不提示确认\n");
    printf("  -i, --interactive     删除前提示确认\n");
    printf("  -r, --recursive       递归删除目录及其内容\n");
    printf("  -v, --verbose         详细输出\n");
    printf("  --one-file-system     跳过不同文件系统的目录\n");
    printf("  --no-preserve-root    不保护根目录（危险）\n");
    printf("  --preserve-root       保护根目录（默认）\n");
    printf("  --help                显示此帮助信息\n");
}

/* 获取用户确认 */
int get_confirmation(const char* path) {
    if (!interactive) return 1;

    printf("rm: 删除 '%s'? ", path);
    fflush(stdout);

    char response[10];
    if (fgets(response, sizeof(response), stdin) == NULL) {
        return 0;
    }

    return (response[0] == 'y' || response[0] == 'Y');
}

/* 删除单个文件 */
int remove_file(const char* path) {
    struct stat st;

    /* 获取文件信息 */
    if (lstat(path, &st) == -1) {
        if (!force) {
            fprintf(stderr, "rm: 无法访问 '%s': %s\n", path, strerror(errno));
            return 1;
        }
        return 0; /* 强制模式下忽略错误 */
    }

    /* 检查是否为根目录 */
    if (preserve_root && strcmp(path, "/") == 0) {
        fprintf(stderr, "rm: 无法删除根目录\n");
        return 1;
    }

    /* 获取用户确认 */
    if (!force && interactive) {
        if (!get_confirmation(path)) {
            return 0; /* 用户取消 */
        }
    }

    /* 删除文件 */
    int result;
    if (S_ISDIR(st.st_mode)) {
        if (!recursive) {
            fprintf(stderr, "rm: 无法删除 '%s': 是目录\n", path);
            return 1;
        }
        result = rmdir(path);
    } else {
        result = unlink(path);
    }

    if (result == -1) {
        if (!force) {
            fprintf(stderr, "rm: 无法删除 '%s': %s\n", path, strerror(errno));
            return 1;
        }
    } else if (verbose) {
        printf("rm: 删除 '%s'\n", path);
    }

    return 0;
}

/* 递归删除目录 */
int remove_directory_recursive(const char* path) {
    DIR* dir;
    struct dirent* entry;
    char fullpath[MAX_PATH_LEN];
    int result = 0;

    dir = opendir(path);
    if (!dir) {
        if (!force) {
            fprintf(stderr, "rm: 无法打开目录 '%s': %s\n", path, strerror(errno));
        }
        return force ? 0 : 1;
    }

    /* 遍历目录内容 */
    while ((entry = readdir(dir)) != NULL) {
        /* 跳过.和.. */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        /* 构建完整路径 */
        if (snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name) >= sizeof(fullpath)) {
            fprintf(stderr, "rm: 路径太长: %s/%s\n", path, entry->d_name);
            result = 1;
            continue;
        }

        /* 递归删除 */
        struct stat st;
        if (lstat(fullpath, &st) == -1) {
            if (!force) {
                fprintf(stderr, "rm: 无法访问 '%s': %s\n", fullpath, strerror(errno));
                result = 1;
            }
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            if (remove_directory_recursive(fullpath) != 0) {
                result = 1;
            }
        } else {
            if (remove_file(fullpath) != 0) {
                result = 1;
            }
        }
    }

    closedir(dir);

    /* 删除目录本身 */
    if (result == 0) {
        result = remove_file(path);
    }

    return result;
}

/* 主函数 */
int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"force", no_argument, 0, 'f'},
        {"interactive", no_argument, 0, 'i'},
        {"recursive", no_argument, 0, 'r'},
        {"verbose", no_argument, 0, 'v'},
        {"one-file-system", no_argument, 0, 1},
        {"no-preserve-root", no_argument, 0, 2},
        {"preserve-root", no_argument, 0, 3},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "firv", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'f':
                force = 1;
                interactive = 0; /* 强制模式覆盖交互模式 */
                break;
            case 'i':
                if (!force) { /* 只有非强制模式下才启用交互 */
                    interactive = 1;
                }
                break;
            case 'r':
                recursive = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 1: /* --one-file-system */
                one_fs = 1;
                break;
            case 2: /* --no-preserve-root */
                preserve_root = 0;
                break;
            case 3: /* --preserve-root */
                preserve_root = 1;
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
        fprintf(stderr, "rm: 缺少操作数\n");
        fprintf(stderr, "使用 'rm --help' 查看更多信息。\n");
        return 1;
    }

    /* 处理所有文件参数 */
    int exit_code = 0;
    for (int i = optind; i < argc; i++) {
        struct stat st;

        if (lstat(argv[i], &st) == -1) {
            if (!force) {
                fprintf(stderr, "rm: 无法访问 '%s': %s\n", argv[i], strerror(errno));
                exit_code = 1;
            }
            continue;
        }

        int result;
        if (S_ISDIR(st.st_mode) && recursive) {
            result = remove_directory_recursive(argv[i]);
        } else {
            result = remove_file(argv[i]);
        }

        if (result != 0) {
            exit_code = 1;
        }
    }

    return exit_code;
}
/**
 * M4KK1 mv - Move (rename) files
 * 移动（重命名）文件
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>

#define MAX_PATH_LEN 4096

/* 选项标志 */
static int force = 0;
static int interactive = 0;
static int verbose = 0;
static int backup = 0;
static int update = 0;

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 mv - Move (rename) files\n");
    printf("用法: mv [选项] 源文件... 目标\n");
    printf("\n");
    printf("选项:\n");
    printf("  -f, --force           强制移动，覆盖现有文件\n");
    printf("  -i, --interactive     覆盖前提示确认\n");
    printf("  -v, --verbose         详细输出\n");
    printf("  --backup              创建备份\n");
    printf("  -u, --update          只在源文件较新时移动\n");
    printf("  --help                显示此帮助信息\n");
}

/* 获取用户确认 */
int get_confirmation(const char* dest) {
    if (!interactive) return 1;

    printf("mv: 覆盖 '%s'? ", dest);
    fflush(stdout);

    char response[10];
    if (fgets(response, sizeof(response), stdin) == NULL) {
        return 0;
    }

    return (response[0] == 'y' || response[0] == 'Y');
}

/* 移动单个文件 */
int move_file(const char* src, const char* dest) {
    struct stat src_st, dest_st;
    int dest_exists = 0;

    /* 获取源文件信息 */
    if (stat(src, &src_st) == -1) {
        fprintf(stderr, "mv: 无法访问 '%s': %s\n", src, strerror(errno));
        return 1;
    }

    /* 检查目标文件是否存在 */
    dest_exists = (stat(dest, &dest_st) == 0);

    /* 检查是否需要更新 */
    if (update && dest_exists) {
        if (src_st.st_mtime <= dest_st.st_mtime) {
            return 0; /* 源文件不比目标文件新 */
        }
    }

    /* 获取用户确认 */
    if (!force && dest_exists && interactive) {
        if (!get_confirmation(dest)) {
            return 0; /* 用户取消 */
        }
    }

    /* 尝试重命名 */
    if (rename(src, dest) == 0) {
        if (verbose) {
            printf("mv: 移动 '%s' -> '%s'\n", src, dest);
        }
        return 0;
    }

    /* 重命名失败，可能需要跨文件系统移动 */
    if (errno == EXDEV) {
        /* 跨文件系统，需要复制然后删除 */
        fprintf(stderr, "mv: 跨文件系统移动暂不支持: %s -> %s\n", src, dest);
        return 1;
    } else {
        if (!force) {
            fprintf(stderr, "mv: 无法移动 '%s' 到 '%s': %s\n", src, dest, strerror(errno));
            return 1;
        }
    }

    return 0;
}

/* 主函数 */
int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"force", no_argument, 0, 'f'},
        {"interactive", no_argument, 0, 'i'},
        {"verbose", no_argument, 0, 'v'},
        {"backup", no_argument, 0, 'b'},
        {"update", no_argument, 0, 'u'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "fivbu", long_options, &option_index)) != -1) {
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
            case 'v':
                verbose = 1;
                break;
            case 'b':
                backup = 1;
                break;
            case 'u':
                update = 1;
                break;
            case 'h':
                show_help();
                return 0;
            default:
                fprintf(stderr, "无效选项。使用 --help 查看帮助。\n");
                return 1;
        }
    }

    /* 检查参数数量 */
    int num_args = argc - optind;
    if (num_args < 2) {
        fprintf(stderr, "mv: 缺少操作数\n");
        fprintf(stderr, "使用 'mv --help' 查看更多信息。\n");
        return 1;
    }

    /* 获取源文件和目标 */
    char** sources = &argv[optind];
    char* dest = argv[argc - 1];
    int num_sources = num_args - 1;

    /* 检查目标是否为目录 */
    struct stat dest_st;
    int dest_is_dir = (stat(dest, &dest_st) == 0 && S_ISDIR(dest_st.st_mode));

    /* 如果有多个源文件，目标必须是目录 */
    if (num_sources > 1 && !dest_is_dir) {
        fprintf(stderr, "mv: 目标 '%s' 不是目录\n", dest);
        return 1;
    }

    /* 处理所有源文件 */
    int exit_code = 0;
    for (int i = 0; i < num_sources; i++) {
        char final_dest[MAX_PATH_LEN];
        struct stat src_st;

        /* 获取源文件信息 */
        if (stat(sources[i], &src_st) == -1) {
            fprintf(stderr, "mv: 无法访问 '%s': %s\n", sources[i], strerror(errno));
            exit_code = 1;
            continue;
        }

        /* 确定最终目标路径 */
        if (dest_is_dir) {
            char* src_copy = strdup(sources[i]);
            char* basename = basename(src_copy);
            snprintf(final_dest, sizeof(final_dest), "%s/%s", dest, basename);
            free(src_copy);
        } else {
            strncpy(final_dest, dest, sizeof(final_dest));
        }

        /* 执行移动 */
        if (move_file(sources[i], final_dest) != 0) {
            exit_code = 1;
        }
    }

    return exit_code;
}
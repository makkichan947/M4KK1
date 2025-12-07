/**
 * M4KK1 cp - Copy files and directories
 * 复制文件和目录
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>
#include <dirent.h>

#define BUFFER_SIZE 8192
#define MAX_PATH_LEN 4096

/* 选项标志 */
static int force = 0;
static int interactive = 0;
static int recursive = 0;
static int verbose = 0;
static int preserve = 0;
static int backup = 0;
static int update = 0;
static int symbolic_link = 0;
static int hard_link = 0;

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 cp - Copy files and directories\n");
    printf("用法: cp [选项] 源文件... 目标\n");
    printf("\n");
    printf("选项:\n");
    printf("  -f, --force           强制复制，覆盖现有文件\n");
    printf("  -i, --interactive     覆盖前提示确认\n");
    printf("  -r, --recursive       递归复制目录\n");
    printf("  -v, --verbose         详细输出\n");
    printf("  -p, --preserve        保留文件属性\n");
    printf("  --backup              创建备份\n");
    printf("  -u, --update          只在源文件较新时复制\n");
    printf("  -s, --symbolic-link   创建符号链接而不是复制\n");
    printf("  --help                显示此帮助信息\n");
}

/* 获取用户确认 */
int get_confirmation(const char* dest) {
    if (!interactive) return 1;

    printf("cp: 覆盖 '%s'? ", dest);
    fflush(stdout);

    char response[10];
    if (fgets(response, sizeof(response), stdin) == NULL) {
        return 0;
    }

    return (response[0] == 'y' || response[0] == 'Y');
}

/* 复制文件内容 */
int copy_file_content(int src_fd, int dest_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;

    while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            return -1;
        }
    }

    return bytes_read == 0 ? 0 : -1;
}

/* 复制单个文件 */
int copy_file(const char* src, const char* dest) {
    int src_fd, dest_fd;
    struct stat st;
    int result = 0;

    /* 获取源文件信息 */
    if (stat(src, &st) == -1) {
        fprintf(stderr, "cp: 无法访问 '%s': %s\n", src, strerror(errno));
        return 1;
    }

    /* 检查是否需要更新 */
    if (update) {
        struct stat dest_st;
        if (stat(dest, &dest_st) == 0) {
            if (st.st_mtime <= dest_st.st_mtime) {
                return 0; /* 源文件不比目标文件新 */
            }
        }
    }

    /* 检查目标文件是否存在 */
    if (!force && access(dest, F_OK) == 0) {
        if (interactive) {
            if (!get_confirmation(dest)) {
                return 0; /* 用户取消 */
            }
        } else if (!force) {
            fprintf(stderr, "cp: 无法创建常规文件 '%s': 文件已存在\n", dest);
            return 1;
        }
    }

    /* 创建符号链接 */
    if (symbolic_link) {
        char link_target[MAX_PATH_LEN];
        ssize_t len = readlink(src, link_target, sizeof(link_target) - 1);
        if (len == -1) {
            fprintf(stderr, "cp: 无法读取符号链接 '%s': %s\n", src, strerror(errno));
            return 1;
        }
        link_target[len] = '\0';

        if (symlink(link_target, dest) == -1) {
            fprintf(stderr, "cp: 无法创建符号链接 '%s': %s\n", dest, strerror(errno));
            return 1;
        }

        if (verbose) {
            printf("cp: 创建符号链接 '%s' -> '%s'\n", dest, link_target);
        }
        return 0;
    }

    /* 创建硬链接 */
    if (hard_link) {
        if (link(src, dest) == -1) {
            fprintf(stderr, "cp: 无法创建硬链接 '%s': %s\n", dest, strerror(errno));
            return 1;
        }

        if (verbose) {
            printf("cp: 创建硬链接 '%s' -> '%s'\n", dest, src);
        }
        return 0;
    }

    /* 打开源文件 */
    src_fd = open(src, O_RDONLY);
    if (src_fd == -1) {
        fprintf(stderr, "cp: 无法打开 '%s' 进行读取: %s\n", src, strerror(errno));
        return 1;
    }

    /* 创建目标文件 */
    int flags = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t mode = st.st_mode;

    if (preserve) {
        /* 保留原始权限 */
    } else {
        mode = 0644; /* 默认权限 */
    }

    dest_fd = open(dest, flags, mode);
    if (dest_fd == -1) {
        fprintf(stderr, "cp: 无法创建常规文件 '%s': %s\n", dest, strerror(errno));
        close(src_fd);
        return 1;
    }

    /* 复制文件内容 */
    result = copy_file_content(src_fd, dest_fd);

    /* 关闭文件 */
    close(src_fd);
    close(dest_fd);

    if (result == -1) {
        fprintf(stderr, "cp: 复制失败 '%s' -> '%s'\n", src, dest);
        return 1;
    }

    /* 保留文件属性 */
    if (preserve) {
        /* 设置修改时间等 */
    }

    if (verbose) {
        printf("cp: 复制 '%s' -> '%s'\n", src, dest);
    }

    return 0;
}

/* 复制目录 */
int copy_directory(const char* src, const char* dest) {
    DIR* dir;
    struct dirent* entry;
    char src_path[MAX_PATH_LEN];
    char dest_path[MAX_PATH_LEN];
    struct stat st;

    /* 创建目标目录 */
    if (mkdir(dest, 0755) == -1 && errno != EEXIST) {
        fprintf(stderr, "cp: 无法创建目录 '%s': %s\n", dest, strerror(errno));
        return 1;
    }

    dir = opendir(src);
    if (!dir) {
        fprintf(stderr, "cp: 无法打开目录 '%s': %s\n", src, strerror(errno));
        return 1;
    }

    /* 复制目录内容 */
    while ((entry = readdir(dir)) != NULL) {
        /* 跳过.和.. */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        /* 构建路径 */
        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);

        /* 获取文件信息 */
        if (stat(src_path, &st) == -1) {
            fprintf(stderr, "cp: 无法访问 '%s': %s\n", src_path, strerror(errno));
            closedir(dir);
            return 1;
        }

        /* 递归复制 */
        if (S_ISDIR(st.st_mode)) {
            if (copy_directory(src_path, dest_path) != 0) {
                closedir(dir);
                return 1;
            }
        } else {
            if (copy_file(src_path, dest_path) != 0) {
                closedir(dir);
                return 1;
            }
        }
    }

    closedir(dir);

    if (verbose) {
        printf("cp: 复制目录 '%s' -> '%s'\n", src, dest);
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
        {"recursive", no_argument, 0, 'r'},
        {"verbose", no_argument, 0, 'v'},
        {"preserve", no_argument, 0, 'p'},
        {"backup", no_argument, 0, 'b'},
        {"update", no_argument, 0, 'u'},
        {"symbolic-link", no_argument, 0, 's'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "firvpbus", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'f':
                force = 1;
                interactive = 0;
                break;
            case 'i':
                if (!force) {
                    interactive = 1;
                }
                break;
            case 'r':
                recursive = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'p':
                preserve = 1;
                break;
            case 'b':
                backup = 1;
                break;
            case 'u':
                update = 1;
                break;
            case 's':
                symbolic_link = 1;
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
        fprintf(stderr, "cp: 缺少操作数\n");
        fprintf(stderr, "使用 'cp --help' 查看更多信息。\n");
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
        fprintf(stderr, "cp: 目标 '%s' 不是目录\n", dest);
        return 1;
    }

    /* 处理所有源文件 */
    int exit_code = 0;
    for (int i = 0; i < num_sources; i++) {
        char final_dest[MAX_PATH_LEN];
        struct stat src_st;

        /* 获取源文件信息 */
        if (stat(sources[i], &src_st) == -1) {
            fprintf(stderr, "cp: 无法访问 '%s': %s\n", sources[i], strerror(errno));
            exit_code = 1;
            continue;
        }

        /* 确定最终目标路径 */
        if (dest_is_dir) {
            char* basename = strrchr(sources[i], '/');
            basename = basename ? basename + 1 : sources[i];
            snprintf(final_dest, sizeof(final_dest), "%s/%s", dest, basename);
        } else {
            strncpy(final_dest, dest, sizeof(final_dest));
        }

        /* 执行复制 */
        int result;
        if (S_ISDIR(src_st.st_mode)) {
            if (!recursive) {
                fprintf(stderr, "cp: 省略目录 '%s'\n", sources[i]);
                exit_code = 1;
                continue;
            }
            result = copy_directory(sources[i], final_dest);
        } else {
            result = copy_file(sources[i], final_dest);
        }

        if (result != 0) {
            exit_code = 1;
        }
    }

    return exit_code;
}
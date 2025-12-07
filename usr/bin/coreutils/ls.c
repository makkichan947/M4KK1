/**
 * M4KK1 ls - List directory contents
 * 列出目录内容命令
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <getopt.h>

#define MAX_PATH_LEN 4096

/* 颜色输出 */
#define COLOR_RESET   "\x1b[0m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"

/* 选项标志 */
static int show_all = 0;
static int show_long = 0;
static int show_hidden = 0;
static int show_color = 0;
static int show_human = 0;
static int show_time = 0;
static int sort_reverse = 0;
static int sort_time = 0;

/* 文件类型颜色 */
const char* get_file_color(mode_t mode) {
    if (!show_color) return "";

    if (S_ISDIR(mode)) return COLOR_BLUE;
    if (S_ISLNK(mode)) return COLOR_CYAN;
    if (S_ISCHR(mode) || S_ISBLK(mode)) return COLOR_YELLOW;
    if (S_ISFIFO(mode)) return COLOR_MAGENTA;
    if (S_ISSOCK(mode)) return COLOR_MAGENTA;
    if (mode & S_IXUSR) return COLOR_GREEN;

    return "";
}

/* 格式化文件权限 */
void format_permissions(mode_t mode, char* buf) {
    buf[0] = S_ISDIR(mode) ? 'd' :
             S_ISCHR(mode) ? 'c' :
             S_ISBLK(mode) ? 'b' :
             S_ISLNK(mode) ? 'l' :
             S_ISFIFO(mode) ? 'p' :
             S_ISSOCK(mode) ? 's' : '-';

    buf[1] = (mode & S_IRUSR) ? 'r' : '-';
    buf[2] = (mode & S_IWUSR) ? 'w' : '-';
    buf[3] = (mode & S_IXUSR) ? 'x' : '-';
    buf[4] = (mode & S_IRGRP) ? 'r' : '-';
    buf[5] = (mode & S_IWGRP) ? 'w' : '-';
    buf[6] = (mode & S_IXGRP) ? 'x' : '-';
    buf[7] = (mode & S_IROTH) ? 'r' : '-';
    buf[8] = (mode & S_IWOTH) ? 'w' : '-';
    buf[9] = (mode & S_IXOTH) ? 'x' : '-';
    buf[10] = '\0';
}

/* 格式化文件大小 */
void format_size(off_t size, char* buf) {
    if (show_human) {
        const char* units[] = {"", "K", "M", "G", "T"};
        int unit = 0;
        double sz = size;

        while (sz >= 1024 && unit < 4) {
            sz /= 1024;
            unit++;
        }

        if (unit == 0) {
            sprintf(buf, "%lld", (long long)size);
        } else {
            sprintf(buf, "%.1f%s", sz, units[unit]);
        }
    } else {
        sprintf(buf, "%lld", (long long)size);
    }
}

/* 显示长格式 */
void display_long(const char* path, const char* name) {
    char fullpath[MAX_PATH_LEN];
    struct stat st;
    char perm[11];
    char size_str[32];
    char time_str[32];
    struct tm *tm;

    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);

    if (lstat(fullpath, &st) == -1) {
        perror(fullpath);
        return;
    }

    /* 权限 */
    format_permissions(st.st_mode, perm);

    /* 大小 */
    format_size(st.st_size, size_str);

    /* 时间 */
    tm = localtime(&st.st_mtime);
    if (show_time) {
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);
    } else {
        strftime(time_str, sizeof(time_str), "%b %d %H:%M", tm);
    }

    printf("%s %3lu %-8s %-8s %8s %s %s%s%s\n",
           perm,
           st.st_nlink,
           "root",  /* 暂时使用root */
           "root",  /* 暂时使用root */
           size_str,
           time_str,
           get_file_color(st.st_mode),
           name,
           COLOR_RESET);
}

/* 显示短格式 */
void display_short(const char* name, mode_t mode) {
    printf("%s%s%s  ", get_file_color(mode), name, show_color ? COLOR_RESET : "");
}

/* 比较函数用于排序 */
int compare_entries(const void* a, const void* b) {
    const struct dirent* da = *(const struct dirent**)a;
    const struct dirent* db = *(const struct dirent**)b;

    if (sort_reverse) {
        return strcmp(db->d_name, da->d_name);
    } else {
        return strcmp(da->d_name, db->d_name);
    }
}

/* 列出目录内容 */
void list_directory(const char* path) {
    DIR* dir;
    struct dirent* entry;
    struct dirent** entries = NULL;
    int count = 0;
    int i;

    dir = opendir(path);
    if (!dir) {
        perror(path);
        return;
    }

    /* 读取所有条目 */
    while ((entry = readdir(dir)) != NULL) {
        /* 跳过隐藏文件（除非指定-a） */
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }

        entries = realloc(entries, sizeof(struct dirent*) * (count + 1));
        entries[count] = malloc(sizeof(struct dirent));
        memcpy(entries[count], entry, sizeof(struct dirent));
        count++;
    }

    closedir(dir);

    /* 排序 */
    qsort(entries, count, sizeof(struct dirent*), compare_entries);

    /* 显示 */
    if (show_long) {
        for (i = 0; i < count; i++) {
            display_long(path, entries[i]->d_name);
            free(entries[i]);
        }
    } else {
        int cols = 0;
        for (i = 0; i < count; i++) {
            display_short(entries[i]->d_name, 0); /* 暂时不支持颜色 */
            cols++;
            if (cols >= 8) { /* 每行8个 */
                printf("\n");
                cols = 0;
            }
            free(entries[i]);
        }
        if (cols > 0) printf("\n");
    }

    free(entries);
}

/* 显示文件信息 */
void display_file(const char* path) {
    char* dir = strdup(path);
    char* name = strrchr(dir, '/');

    if (name) {
        *name = '\0';
        name++;
        if (show_long) {
            display_long(dir[0] ? dir : ".", name);
        } else {
            printf("%s\n", name);
        }
    } else {
        if (show_long) {
            display_long(".", path);
        } else {
            printf("%s\n", path);
        }
    }

    free(dir);
}

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 ls - List directory contents\n");
    printf("用法: ls [选项] [文件...]\n");
    printf("\n");
    printf("选项:\n");
    printf("  -a, --all          显示所有文件，包括以.开头的隐藏文件\n");
    printf("  -l, --long         使用长格式显示\n");
    printf("  -h, --human        以人类可读格式显示文件大小\n");
    printf("  --color[=WHEN]     控制是否使用颜色输出\n");
    printf("  -r, --reverse      反向排序\n");
    printf("  -t, --time         按修改时间排序\n");
    printf("  --help             显示此帮助信息\n");
}

/* 主函数 */
int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;
    char* path = ".";

    static struct option long_options[] = {
        {"all", no_argument, 0, 'a'},
        {"long", no_argument, 0, 'l'},
        {"human", no_argument, 0, 'h'},
        {"color", optional_argument, 0, 'C'},
        {"reverse", no_argument, 0, 'r'},
        {"time", no_argument, 0, 't'},
        {"help", no_argument, 0, 'H'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "alhrtC::", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'a':
                show_all = 1;
                break;
            case 'l':
                show_long = 1;
                break;
            case 'h':
                show_human = 1;
                break;
            case 'C':
                show_color = 1;
                break;
            case 'r':
                sort_reverse = 1;
                break;
            case 't':
                sort_time = 1;
                break;
            case 'H':
                show_help();
                return 0;
            default:
                fprintf(stderr, "无效选项。使用 --help 查看帮助。\n");
                return 1;
        }
    }

    /* 处理剩余参数 */
    if (optind < argc) {
        int i;
        for (i = optind; i < argc; i++) {
            struct stat st;
            if (stat(argv[i], &st) == -1) {
                perror(argv[i]);
                continue;
            }

            if (S_ISDIR(st.st_mode)) {
                if (argc - optind > 1) {
                    printf("%s:\n", argv[i]);
                }
                list_directory(argv[i]);
                if (i < argc - 1) printf("\n");
            } else {
                display_file(argv[i]);
            }
        }
    } else {
        list_directory(".");
    }

    return 0;
}
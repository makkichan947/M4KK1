/**
 * M4KK1 Package Management System - Core Implementation
 * 包管理系统核心实现
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>

#include "package.h"

/* 包数据库文件 */
#define PACKAGE_DB_PATH "/var/lib/pkgmgr/packages.db"
#define PACKAGE_DB_BACKUP "/var/lib/pkgmgr/packages.db.bak"

/* 包安装根目录 */
#define PACKAGE_ROOT "/usr/local"

/* 包状态 */
#define PKG_STATE_INSTALLED 1
#define PKG_STATE_REMOVED   0
#define PKG_STATE_BROKEN    -1

/* 依赖关系类型 */
#define DEP_TYPE_REQUIRED   1
#define DEP_TYPE_OPTIONAL   2
#define DEP_TYPE_CONFLICTS  3

/* 全局包数据库 */
static package_db_t package_db;

/* 工具函数 */
static char *str_trim(char *str) {
    char *end;

    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}

static int str_split(char *str, char delim, char **tokens, int max_tokens) {
    int count = 0;
    char *token = strtok(str, &delim);

    while (token && count < max_tokens) {
        tokens[count++] = str_trim(token);
        token = strtok(NULL, &delim);
    }

    return count;
}

static int file_exists(const char *path) {
    return access(path, F_OK) == 0;
}

static int dir_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int mkdir_p(const char *path) {
    char *path_copy = strdup(path);
    char *p = path_copy;
    int result = 0;

    if (!path_copy) return -1;

    /* 跳过开头的斜杠 */
    if (*p == '/') p++;

    while (*p) {
        if (*p == '/') {
            *p = '\0';
            if (!dir_exists(path_copy) && mkdir(path_copy, 0755) != 0) {
                result = -1;
                break;
            }
            *p = '/';
        }
        p++;
    }

    if (result == 0 && !dir_exists(path)) {
        result = mkdir(path, 0755);
    }

    free(path_copy);
    return result;
}

/* 包数据库操作 */
static int package_db_load(void) {
    FILE *fp = fopen(PACKAGE_DB_PATH, "r");
    if (!fp) {
        /* 数据库不存在，创建空的 */
        memset(&package_db, 0, sizeof(package_db));
        return 0;
    }

    /* 读取包数量 */
    if (fread(&package_db.count, sizeof(uint32_t), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    /* 读取包信息 */
    for (uint32_t i = 0; i < package_db.count && i < MAX_PACKAGES; i++) {
        package_info_t *pkg = &package_db.packages[i];

        if (fread(pkg, sizeof(package_info_t), 1, fp) != 1) {
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);
    return 0;
}

static int package_db_save(void) {
    FILE *fp;
    char dir_path[256];

    /* 确保目录存在 */
    snprintf(dir_path, sizeof(dir_path), "%s", PACKAGE_DB_PATH);
    char *last_slash = strrchr(dir_path, '/');
    if (last_slash) {
        *last_slash = '\0';
        mkdir_p(dir_path);
        *last_slash = '/';
    }

    /* 备份旧数据库 */
    if (file_exists(PACKAGE_DB_PATH)) {
        rename(PACKAGE_DB_PATH, PACKAGE_DB_BACKUP);
    }

    /* 写入新数据库 */
    fp = fopen(PACKAGE_DB_PATH, "w");
    if (!fp) {
        /* 恢复备份 */
        if (file_exists(PACKAGE_DB_BACKUP)) {
            rename(PACKAGE_DB_BACKUP, PACKAGE_DB_PATH);
        }
        return -1;
    }

    /* 写入包数量 */
    if (fwrite(&package_db.count, sizeof(uint32_t), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    /* 写入包信息 */
    for (uint32_t i = 0; i < package_db.count; i++) {
        package_info_t *pkg = &package_db.packages[i];

        if (fwrite(pkg, sizeof(package_info_t), 1, fp) != 1) {
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);
    return 0;
}

static package_info_t *package_db_find(const char *name) {
    for (uint32_t i = 0; i < package_db.count; i++) {
        if (strcmp(package_db.packages[i].name, name) == 0) {
            return &package_db.packages[i];
        }
    }
    return NULL;
}

static int package_db_add(package_info_t *pkg) {
    if (package_db.count >= MAX_PACKAGES) {
        return -1;
    }

    if (package_db_find(pkg->name)) {
        return -1; /* 包已存在 */
    }

    memcpy(&package_db.packages[package_db.count], pkg, sizeof(package_info_t));
    package_db.count++;

    return 0;
}

static int package_db_remove(const char *name) {
    for (uint32_t i = 0; i < package_db.count; i++) {
        if (strcmp(package_db.packages[i].name, name) == 0) {
            /* 移动后面的包 */
            memmove(&package_db.packages[i],
                   &package_db.packages[i + 1],
                   (package_db.count - i - 1) * sizeof(package_info_t));
            package_db.count--;
            return 0;
        }
    }
    return -1;
}

/* 包文件操作 */
static int package_extract_files(const char *package_path, const char *install_prefix) {
    /* 这里应该实现tar/zip解压功能 */
    /* 暂时只创建目录结构 */

    char command[512];
    snprintf(command, sizeof(command), "tar -tzf \"%s\" | head -20", package_path);

    FILE *fp = popen(command, "r");
    if (!fp) {
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        str_trim(line);
        if (line[0] == '/') {
            /* 绝对路径，转换为相对于安装前缀的路径 */
            char file_path[512];
            snprintf(file_path, sizeof(file_path), "%s%s", install_prefix, line);

            /* 确保目录存在 */
            char *last_slash = strrchr(file_path, '/');
            if (last_slash) {
                *last_slash = '\0';
                mkdir_p(file_path);
                *last_slash = '/';
            }
        }
    }

    pclose(fp);
    return 0;
}

static int package_remove_files(const char *package_name) {
    package_info_t *pkg = package_db_find(package_name);
    if (!pkg) {
        return -1;
    }

    /* 这里应该实现文件删除功能 */
    /* 暂时只更新数据库状态 */

    pkg->state = PKG_STATE_REMOVED;
    return 0;
}

/* 依赖关系检查 */
static int package_check_dependencies(package_info_t *pkg) {
    /* 检查必需依赖 */
    for (int i = 0; i < pkg->dep_count && i < MAX_DEPENDENCIES; i++) {
        package_dependency_t *dep = &pkg->dependencies[i];

        if (dep->type == DEP_TYPE_REQUIRED) {
            package_info_t *dep_pkg = package_db_find(dep->name);
            if (!dep_pkg || dep_pkg->state != PKG_STATE_INSTALLED) {
                printf("Error: Missing required dependency: %s\n", dep->name);
                return -1;
            }
        }
    }

    return 0;
}

/* 包管理API实现 */
int package_init(void) {
    /* 确保包数据库目录存在 */
    mkdir_p("/var/lib/pkgmgr");

    /* 加载包数据库 */
    if (package_db_load() != 0) {
        printf("Warning: Failed to load package database, creating new one\n");
        memset(&package_db, 0, sizeof(package_db));
    }

    return 0;
}

int package_install(const char *package_path, int force) {
    package_info_t pkg_info;
    char metadata_path[512];

    printf("Installing package: %s\n", package_path);

    /* 读取包元数据 */
    snprintf(metadata_path, sizeof(metadata_path), "%s/METADATA", package_path);
    if (!file_exists(metadata_path)) {
        printf("Error: Invalid package format\n");
        return -1;
    }

    /* 解析包元数据（简化实现） */
    memset(&pkg_info, 0, sizeof(pkg_info));
    strcpy(pkg_info.name, "unknown");
    pkg_info.state = PKG_STATE_INSTALLED;

    /* 检查依赖关系 */
    if (!force && package_check_dependencies(&pkg_info) != 0) {
        printf("Error: Dependency check failed\n");
        return -1;
    }

    /* 提取文件 */
    if (package_extract_files(package_path, PACKAGE_ROOT) != 0) {
        printf("Error: Failed to extract package files\n");
        return -1;
    }

    /* 添加到数据库 */
    if (package_db_add(&pkg_info) != 0) {
        printf("Error: Failed to add package to database\n");
        return -1;
    }

    /* 保存数据库 */
    if (package_db_save() != 0) {
        printf("Warning: Failed to save package database\n");
    }

    printf("Package installed successfully\n");
    return 0;
}

int package_remove(const char *package_name, int force) {
    package_info_t *pkg = package_db_find(package_name);

    if (!pkg) {
        printf("Error: Package not found: %s\n", package_name);
        return -1;
    }

    if (pkg->state != PKG_STATE_INSTALLED) {
        printf("Error: Package is not installed: %s\n", package_name);
        return -1;
    }

    printf("Removing package: %s\n", package_name);

    /* 检查是否有其他包依赖此包 */
    if (!force) {
        for (uint32_t i = 0; i < package_db.count; i++) {
            package_info_t *other_pkg = &package_db.packages[i];
            if (other_pkg == pkg) continue;

            for (int j = 0; j < other_pkg->dep_count; j++) {
                if (strcmp(other_pkg->dependencies[j].name, package_name) == 0) {
                    printf("Error: Package is required by: %s\n", other_pkg->name);
                    return -1;
                }
            }
        }
    }

    /* 移除文件 */
    if (package_remove_files(package_name) != 0) {
        printf("Error: Failed to remove package files\n");
        return -1;
    }

    /* 从数据库移除 */
    if (package_db_remove(package_name) != 0) {
        printf("Error: Failed to remove package from database\n");
        return -1;
    }

    /* 保存数据库 */
    if (package_db_save() != 0) {
        printf("Warning: Failed to save package database\n");
    }

    printf("Package removed successfully\n");
    return 0;
}

int package_info(const char *package_name) {
    package_info_t *pkg = package_db_find(package_name);

    if (!pkg) {
        printf("Package not found: %s\n", package_name);
        return -1;
    }

    printf("Package: %s\n", pkg->name);
    printf("Version: %s\n", pkg->version);
    printf("Description: %s\n", pkg->description);
    printf("State: %s\n", pkg->state == PKG_STATE_INSTALLED ? "installed" : "removed");
    printf("Install Date: %s", ctime(&pkg->install_time));

    if (pkg->dep_count > 0) {
        printf("Dependencies:\n");
        for (int i = 0; i < pkg->dep_count; i++) {
            printf("  %s (%s)\n",
                   pkg->dependencies[i].name,
                   pkg->dependencies[i].type == DEP_TYPE_REQUIRED ? "required" : "optional");
        }
    }

    return 0;
}

int package_list(void) {
    printf("Installed packages:\n");
    printf("%-20s %-10s %-15s %s\n", "Name", "Version", "State", "Description");
    printf("%s\n", "------------------------------------------------------------");

    for (uint32_t i = 0; i < package_db.count; i++) {
        package_info_t *pkg = &package_db.packages[i];

        printf("%-20s %-10s %-15s %s\n",
               pkg->name,
               pkg->version,
               pkg->state == PKG_STATE_INSTALLED ? "installed" : "removed",
               pkg->description);
    }

    return 0;
}

int package_search(const char *pattern) {
    printf("Searching for packages matching: %s\n", pattern);

    int found = 0;
    for (uint32_t i = 0; i < package_db.count; i++) {
        package_info_t *pkg = &package_db.packages[i];

        if (strstr(pkg->name, pattern) || strstr(pkg->description, pattern)) {
            printf("  %s - %s\n", pkg->name, pkg->description);
            found++;
        }
    }

    if (found == 0) {
        printf("No packages found matching: %s\n", pattern);
    }

    return 0;
}

int package_update(const char *package_name) {
    package_info_t *pkg = package_db_find(package_name);

    if (!pkg) {
        printf("Error: Package not found: %s\n", package_name);
        return -1;
    }

    printf("Updating package: %s\n", package_name);

    /* 这里应该实现包更新逻辑 */
    printf("Package update not yet implemented\n");

    return 0;
}

int package_cleanup(void) {
    printf("Cleaning up package cache...\n");

    /* 清理下载的包文件 */
    system("rm -rf /var/cache/pkgmgr/*");

    printf("Cleanup completed\n");
    return 0;
}

void package_print_stats(void) {
    uint32_t installed_count = 0;
    uint32_t total_size = 0;

    for (uint32_t i = 0; i < package_db.count; i++) {
        if (package_db.packages[i].state == PKG_STATE_INSTALLED) {
            installed_count++;
            total_size += package_db.packages[i].size;
        }
    }

    printf("Package Statistics:\n");
    printf("  Total packages: %u\n", package_db.count);
    printf("  Installed packages: %u\n", installed_count);
    printf("  Total size: %u KB\n", total_size / 1024);
    printf("  Database path: %s\n", PACKAGE_DB_PATH);
}
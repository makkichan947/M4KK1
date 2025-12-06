/**
 * M4KK1 Package Management System - Header Definitions
 * 包管理系统头文件定义
 */

#ifndef _M4K_PACKAGE_H
#define _M4K_PACKAGE_H

#include <stdint.h>
#include <time.h>

/* 常量定义 */
#define MAX_PACKAGES 4096
#define MAX_PACKAGE_NAME 128
#define MAX_PACKAGE_VERSION 64
#define MAX_PACKAGE_DESCRIPTION 512
#define MAX_DEPENDENCIES 64
#define MAX_FILES 1024

/* 包状态 */
#define PKG_STATE_INSTALLED 1
#define PKG_STATE_REMOVED   0
#define PKG_STATE_BROKEN    -1

/* 依赖关系类型 */
#define DEP_TYPE_REQUIRED   1
#define DEP_TYPE_OPTIONAL   2
#define DEP_TYPE_CONFLICTS  3

/* 依赖关系结构 */
typedef struct {
    char name[MAX_PACKAGE_NAME];
    char version_constraint[64];
    uint32_t type;              /* DEP_TYPE_* */
} package_dependency_t;

/* 包信息结构 */
typedef struct {
    char name[MAX_PACKAGE_NAME];
    char version[MAX_PACKAGE_VERSION];
    char description[MAX_PACKAGE_DESCRIPTION];
    char maintainer[128];
    char architecture[32];
    uint32_t state;
    uint64_t size;              /* 包大小（字节） */
    time_t install_time;        /* 安装时间 */
    char checksum[64];          /* 校验和 */
    uint32_t dep_count;         /* 依赖数量 */
    package_dependency_t dependencies[MAX_DEPENDENCIES]; /* 依赖列表 */
} package_info_t;

/* 文件信息结构 */
typedef struct {
    char path[256];
    uint32_t type;              /* 文件类型 */
    uint32_t mode;              /* 文件权限 */
    uint64_t size;              /* 文件大小 */
    char checksum[64];          /* 文件校验和 */
} package_file_t;

/* 包数据库结构 */
typedef struct {
    uint32_t count;
    package_info_t packages[MAX_PACKAGES];
} package_db_t;

/* API函数声明 */
int package_init(void);
int package_install(const char *package_path, int force);
int package_remove(const char *package_name, int force);
int package_update(const char *package_name);
int package_info(const char *package_name);
int package_list(void);
int package_search(const char *pattern);
int package_cleanup(void);
void package_print_stats(void);

/* 包查询函数 */
package_info_t *package_find(const char *name);
int package_is_installed(const char *name);
int package_get_size(const char *name);

/* 依赖关系管理 */
int package_resolve_dependencies(char **packages, int package_count);
int package_check_conflicts(package_info_t *pkg);

/* 包验证 */
int package_verify_checksum(const char *package_path);
int package_verify_signature(const char *package_path);

/* 包创建工具 */
int package_create(const char *package_name, const char *version,
                  const char *description, char **files, int file_count);
int package_add_dependency(const char *package_path, const char *dep_name,
                          const char *version_constraint, int type);
int package_add_file(const char *package_path, const char *file_path,
                    const char *package_file_path);

/* 包仓库管理 */
int package_repo_add(const char *name, const char *url);
int package_repo_remove(const char *name);
int package_repo_update(const char *name);
int package_repo_list(void);

/* 高级功能 */
int package_downgrade(const char *package_name, const char *version);
int package_verify_system(void);
int package_autoremove(void);
int package_autoclean(void);

#endif /* _M4K_PACKAGE_H */
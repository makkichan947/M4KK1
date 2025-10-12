/**
 * PkgMgr - M4KK1包管理系统主头文件
 * 定义包管理系统的核心数据结构和接口
 */

#ifndef _PKGMGR_H
#define _PKGMGR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

/**
 * PkgMgr魔数
 */
#define PKGMGR_MAGIC 0x504B4D47    /* "PKMG" */

/**
 * PkgMgr版本
 */
#define PKGMGR_VERSION_MAJOR 0
#define PKGMGR_VERSION_MINOR 1
#define PKGMGR_VERSION_PATCH 0

/**
 * 操作类型
 */
#define PKGMGR_OP_INSTALL     0
#define PKGMGR_OP_REMOVE      1
#define PKGMGR_OP_UPGRADE     2
#define PKGMGR_OP_QUERY       3
#define PKGMGR_OP_SYNC        4
#define PKGMGR_OP_CLEAN       5

/**
 * 包状态
 */
#define PKGMGR_STATUS_INSTALLED   0
#define PKGMGR_STATUS_AVAILABLE   1
#define PKGMGR_STATUS_OUTDATED    2
#define PKGMGR_STATUS_ORPHANED    3
#define PKGMGR_STATUS_CONFLICT    4

/**
 * 依赖类型
 */
#define PKGMGR_DEP_REQUIRED       0
#define PKGMGR_DEP_OPTIONAL       1
#define PKGMGR_DEP_MAKE          2
#define PKGMGR_DEP_CHECK         3

/**
 * 钩子类型
 */
#define PKGMGR_HOOK_PRE_INSTALL   0
#define PKGMGR_HOOK_POST_INSTALL  1
#define PKGMGR_HOOK_PRE_UPGRADE   2
#define PKGMGR_HOOK_POST_UPGRADE  3
#define PKGMGR_HOOK_PRE_REMOVE    4
#define PKGMGR_HOOK_POST_REMOVE   5

/**
 * 包信息结构
 */
typedef struct {
    char *name;                  /* 包名 */
    char *version;               /* 版本 */
    char *release;               /* 发布号 */
    char *description;           /* 描述 */
    char *architecture;          /* 架构 */
    char *maintainer;            /* 维护者 */
    char *url;                   /* 项目URL */
    char *license;               /* 许可证 */
    char *group;                 /* 组 */
    char *packager;              /* 打包者 */
    char *md5sum;                /* MD5校验和 */
    char *sha256sum;             /* SHA256校验和 */
    uint64_t size;               /* 包大小 */
    uint64_t install_size;       /* 安装大小 */
    time_t build_date;           /* 构建日期 */
    time_t install_date;         /* 安装日期 */
    uint32_t install_reason;     /* 安装原因 */
    char **depends;              /* 依赖列表 */
    uint32_t depends_count;      /* 依赖数量 */
    char **optdepends;           /* 可选依赖 */
    uint32_t optdepends_count;   /* 可选依赖数量 */
    char **conflicts;            /* 冲突包 */
    uint32_t conflicts_count;    /* 冲突数量 */
    char **provides;             /* 提供的虚拟包 */
    uint32_t provides_count;     /* 提供数量 */
    char **replaces;             /* 替换的包 */
    uint32_t replaces_count;     /* 替换数量 */
    char **files;                /* 文件列表 */
    uint32_t files_count;        /* 文件数量 */
    char **backup;               /* 备份文件 */
    uint32_t backup_count;       /* 备份数量 */
    char **scripts;              /* 脚本列表 */
    uint32_t scripts_count;      /* 脚本数量 */
    uint32_t status;             /* 包状态 */
    void *metadata;              /* 元数据 */
} pkgmgr_package_t;

/**
 * 仓库信息结构
 */
typedef struct {
    char *name;                  /* 仓库名 */
    char *url;                   /* 仓库URL */
    uint32_t priority;           /* 优先级 */
    bool enabled;                /* 启用状态 */
    time_t last_sync;            /* 最后同步时间 */
    char *siglevel;              /* 签名级别 */
    char **servers;              /* 服务器列表 */
    uint32_t server_count;       /* 服务器数量 */
} pkgmgr_repo_t;

/**
 * 事务结构
 */
typedef struct {
    uint32_t type;               /* 操作类型 */
    pkgmgr_package_t **packages; /* 包列表 */
    uint32_t package_count;      /* 包数量 */
    char *reason;                /* 操作原因 */
    time_t timestamp;            /* 时间戳 */
    uint32_t flags;              /* 标志位 */
    void *data;                  /* 附加数据 */
} pkgmgr_transaction_t;

/**
 * 配置结构
 */
typedef struct {
    char *root_dir;              /* 根目录 */
    char *db_path;               /* 数据库路径 */
    char *cache_dir;             /* 缓存目录 */
    char *log_file;              /* 日志文件 */
    uint32_t log_level;          /* 日志级别 */
    bool color_output;           /* 彩色输出 */
    bool verbose;                /* 详细输出 */
    bool debug;                  /* 调试模式 */
    bool no_confirm;             /* 无需确认 */
    bool force;                  /* 强制操作 */
    char **ignore_packages;      /* 忽略包列表 */
    uint32_t ignore_count;       /* 忽略数量 */
    char **hold_packages;        /* 保持包列表 */
    uint32_t hold_count;         /* 保持数量 */
    pkgmgr_repo_t **repos;       /* 仓库列表 */
    uint32_t repo_count;         /* 仓库数量 */
} pkgmgr_config_t;

/**
 * 上下文结构
 */
typedef struct {
    uint32_t magic;              /* 魔数 */
    uint32_t version;            /* 版本 */
    pkgmgr_config_t *config;     /* 配置 */
    void *db;                    /* 数据库连接 */
    void *cache;                 /* 缓存管理器 */
    void *network;               /* 网络管理器 */
    uint32_t error_count;        /* 错误计数 */
    uint32_t warning_count;      /* 警告计数 */
    bool dry_run;                /* 试运行模式 */
    char *current_operation;     /* 当前操作 */
} pkgmgr_context_t;

/**
 * 函数声明
 */

/* 上下文管理 */
int pkgmgr_init(pkgmgr_context_t *ctx);
int pkgmgr_cleanup(pkgmgr_context_t *ctx);
int pkgmgr_load_config(pkgmgr_context_t *ctx, const char *config_file);
int pkgmgr_save_config(pkgmgr_context_t *ctx, const char *config_file);

/* 包操作 */
int pkgmgr_install(pkgmgr_context_t *ctx, const char *package_name);
int pkgmgr_remove(pkgmgr_context_t *ctx, const char *package_name, bool cascade);
int pkgmgr_upgrade(pkgmgr_context_t *ctx, const char *package_name);
int pkgmgr_query(pkgmgr_context_t *ctx, const char *package_name, pkgmgr_package_t *package);
int pkgmgr_search(pkgmgr_context_t *ctx, const char *keyword, pkgmgr_package_t **packages, uint32_t *count);

/* 仓库管理 */
int pkgmgr_sync_repos(pkgmgr_context_t *ctx);
int pkgmgr_add_repo(pkgmgr_context_t *ctx, const char *name, const char *url, uint32_t priority);
int pkgmgr_remove_repo(pkgmgr_context_t *ctx, const char *name);
int pkgmgr_enable_repo(pkgmgr_context_t *ctx, const char *name);
int pkgmgr_disable_repo(pkgmgr_context_t *ctx, const char *name);

/* 依赖处理 */
int pkgmgr_resolve_dependencies(pkgmgr_context_t *ctx, const char *package_name,
                               pkgmgr_package_t **deps, uint32_t *dep_count);
int pkgmgr_check_conflicts(pkgmgr_context_t *ctx, pkgmgr_package_t *package,
                          pkgmgr_package_t **conflicts, uint32_t *conflict_count);
int pkgmgr_find_orphans(pkgmgr_context_t *ctx, pkgmgr_package_t **orphans, uint32_t *orphan_count);

/* 事务管理 */
int pkgmgr_transaction_init(pkgmgr_context_t *ctx, uint32_t type, pkgmgr_transaction_t **trans);
int pkgmgr_transaction_add_package(pkgmgr_transaction_t *trans, pkgmgr_package_t *package);
int pkgmgr_transaction_prepare(pkgmgr_context_t *ctx, pkgmgr_transaction_t *trans);
int pkgmgr_transaction_commit(pkgmgr_context_t *ctx, pkgmgr_transaction_t *trans);
int pkgmgr_transaction_rollback(pkgmgr_context_t *ctx, pkgmgr_transaction_t *trans);

/* 钩子系统 */
int pkgmgr_hook_execute(pkgmgr_context_t *ctx, uint32_t hook_type,
                       pkgmgr_package_t *package, const char *root_dir);
int pkgmgr_hook_register(pkgmgr_context_t *ctx, uint32_t hook_type,
                        const char *script_path, const char *package_name);

/* 数据库操作 */
int pkgmgr_db_open(pkgmgr_context_t *ctx, const char *db_path);
int pkgmgr_db_close(pkgmgr_context_t *ctx);
int pkgmgr_db_add_package(pkgmgr_context_t *ctx, pkgmgr_package_t *package);
int pkgmgr_db_remove_package(pkgmgr_context_t *ctx, const char *package_name);
int pkgmgr_db_find_package(pkgmgr_context_t *ctx, const char *package_name, pkgmgr_package_t *package);
int pkgmgr_db_list_packages(pkgmgr_context_t *ctx, pkgmgr_package_t **packages, uint32_t *count);

/* 缓存管理 */
int pkgmgr_cache_clean(pkgmgr_context_t *ctx);
int pkgmgr_cache_download(pkgmgr_context_t *ctx, const char *package_name, const char *cache_dir);
int pkgmgr_cache_verify(pkgmgr_context_t *ctx, const char *package_name);

/* 日志系统 */
int pkgmgr_log_init(pkgmgr_context_t *ctx, const char *log_file, uint32_t level);
int pkgmgr_log_write(pkgmgr_context_t *ctx, uint32_t level, const char *message);
int pkgmgr_log_close(pkgmgr_context_t *ctx);

/* 工具函数 */
uint32_t pkgmgr_get_version(void);
const char *pkgmgr_get_version_string(void);
int pkgmgr_package_compare(pkgmgr_package_t *pkg1, pkgmgr_package_t *pkg2);
int pkgmgr_version_compare(const char *ver1, const char *ver2);
char *pkgmgr_format_size(uint64_t size);
char *pkgmgr_format_time(time_t timestamp);

/* 错误处理 */
typedef enum {
    PKGMGR_OK = 0,
    PKGMGR_ERROR_INVALID_ARG = -1,
    PKGMGR_ERROR_NOT_FOUND = -2,
    PKGMGR_ERROR_ALREADY_EXISTS = -3,
    PKGMGR_ERROR_PERMISSION_DENIED = -4,
    PKGMGR_ERROR_DISK_FULL = -5,
    PKGMGR_ERROR_NETWORK_ERROR = -6,
    PKGMGR_ERROR_CHECKSUM_ERROR = -7,
    PKGMGR_ERROR_DEPENDENCY_ERROR = -8,
    PKGMGR_ERROR_CONFLICT_ERROR = -9,
    PKGMGR_ERROR_TRANSACTION_ERROR = -10,
    PKGMGR_ERROR_HOOK_ERROR = -11,
    PKGMGR_ERROR_DATABASE_ERROR = -12,
    PKGMGR_ERROR_CACHE_ERROR = -13,
    PKGMGR_ERROR_CONFIG_ERROR = -14,
    PKGMGR_ERROR_INTERNAL_ERROR = -15
} pkgmgr_error_t;

const char *pkgmgr_error_string(pkgmgr_error_t error);

/* 内存管理 */
void *pkgmgr_malloc(size_t size);
void pkgmgr_free(void *ptr);
void *pkgmgr_realloc(void *ptr, size_t size);
char *pkgmgr_strdup(const char *str);

/* 字符串处理 */
int pkgmgr_strcmp(const char *s1, const char *s2);
size_t pkgmgr_strlen(const char *str);
char *pkgmgr_strcpy(char *dest, const char *src);
char *pkgmgr_strcat(char *dest, const char *src);
char *pkgmgr_strtok(char *str, const char *delim);

/* 文件操作 */
int pkgmgr_file_exists(const char *filename);
int pkgmgr_file_read(const char *filename, uint8_t **buffer, uint32_t *size);
int pkgmgr_file_write(const char *filename, const uint8_t *buffer, uint32_t size);
int pkgmgr_file_copy(const char *src, const char *dst);

/* 路径处理 */
char *pkgmgr_path_join(const char *dir, const char *file);
char *pkgmgr_path_normalize(const char *path);
bool pkgmgr_path_is_absolute(const char *path);

/* 校验和计算 */
uint32_t pkgmgr_checksum_crc32(const void *data, uint32_t size);
uint32_t pkgmgr_checksum_md5(const void *data, uint32_t size);
uint32_t pkgmgr_checksum_sha256(const void *data, uint32_t size);

/* 版本信息宏 */
#define PKGMGR_VERSION \
    (PKGMGR_VERSION_MAJOR << 16 | PKGMGR_VERSION_MINOR << 8 | PKGMGR_VERSION_PATCH)

#define PKGMGR_VERSION_STRING \
    "PkgMgr " \
    STRINGIFY(PKGMGR_VERSION_MAJOR) "." \
    STRINGIFY(PKGMGR_VERSION_MINOR) "." \
    STRINGIFY(PKGMGR_VERSION_PATCH)

/* 字符串化宏 */
#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

#endif /* _PKGMGR_H */
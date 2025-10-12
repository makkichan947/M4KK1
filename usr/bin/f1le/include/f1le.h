/*
 * M4KK1 F1le文件管理器 - 主接口头文件
 * 定义文件管理器的核心数据结构和接口
 */

#ifndef __F1LE_H__
#define __F1LE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>

/* 版本信息 */
#define F1LE_VERSION_MAJOR 0
#define F1LE_VERSION_MINOR 1
#define F1LE_VERSION_PATCH 0
#define F1LE_VERSION_STRING "0.1.0"

/* 常量定义 */
#define MAX_PATH_LENGTH     4096
#define MAX_NAME_LENGTH     256
#define MAX_FILES_PER_DIR   10000
#define MAX_TABS            10
#define MAX_BOOKMARKS       100
#define MAX_HISTORY         1000

/* 文件类型枚举 */
typedef enum {
    FILE_TYPE_REGULAR = 0,   /* 普通文件 */
    FILE_TYPE_DIRECTORY = 1, /* 目录 */
    FILE_TYPE_SYMLINK = 2,   /* 符号链接 */
    FILE_TYPE_BLOCK = 3,     /* 块设备 */
    FILE_TYPE_CHAR = 4,      /* 字符设备 */
    FILE_TYPE_FIFO = 5,      /* FIFO */
    FILE_TYPE_SOCKET = 6,    /* 套接字 */
    FILE_TYPE_UNKNOWN = 7    /* 未知类型 */
} file_type_t;

/* 排序方式枚举 */
typedef enum {
    SORT_NAME = 0,           /* 按名称排序 */
    SORT_SIZE = 1,           /* 按大小排序 */
    SORT_MTIME = 2,          /* 按修改时间排序 */
    SORT_ATIME = 3,          /* 按访问时间排序 */
    SORT_TYPE = 4,           /* 按类型排序 */
    SORT_EXTENSION = 5       /* 按扩展名排序 */
} sort_type_t;

/* 显示模式枚举 */
typedef enum {
    DISPLAY_LIST = 0,        /* 列表显示 */
    DISPLAY_ICONS = 1,       /* 图标显示 */
    DISPLAY_TREE = 2,        /* 树状显示 */
    DISPLAY_DETAILS = 3      /* 详细显示 */
} display_mode_t;

/* 操作模式枚举 */
typedef enum {
    MODE_NORMAL = 0,         /* 普通模式 */
    MODE_SELECT = 1,         /* 选择模式 */
    MODE_SEARCH = 2,         /* 搜索模式 */
    MODE_COMMAND = 3,        /* 命令模式 */
    MODE_INPUT = 4           /* 输入模式 */
} operation_mode_t;

/* 文件信息结构 */
typedef struct file_info {
    char name[MAX_NAME_LENGTH];     /* 文件名 */
    char path[MAX_PATH_LENGTH];     /* 完整路径 */
    size_t size;                    /* 文件大小 */
    time_t mtime;                   /* 修改时间 */
    time_t atime;                   /* 访问时间 */
    time_t ctime;                   /* 创建时间 */
    mode_t mode;                    /* 文件模式 */
    uid_t uid;                      /* 用户ID */
    gid_t gid;                      /* 组ID */
    file_type_t type;               /* 文件类型 */
    char *mime_type;                /* MIME类型 */
    char *symlink_target;           /* 符号链接目标 */
    bool is_hidden;                 /* 是否隐藏 */
    bool is_selected;               /* 是否选中 */
    void *userdata;                 /* 用户数据 */
} file_info_t;

/* 文件列表结构 */
typedef struct file_list {
    file_info_t **files;            /* 文件数组 */
    size_t count;                   /* 文件数量 */
    size_t capacity;                /* 容量 */
    size_t current_index;           /* 当前索引 */
    sort_type_t sort_type;          /* 排序方式 */
    bool sort_reverse;              /* 逆序排序 */
    char *filter_pattern;           /* 过滤模式 */
    display_mode_t display_mode;    /* 显示模式 */
} file_list_t;

/* 标签页结构 */
typedef struct tab {
    char name[MAX_NAME_LENGTH];     /* 标签名 */
    char path[MAX_PATH_LENGTH];     /* 当前路径 */
    file_list_t *file_list;         /* 文件列表 */
    size_t scroll_offset;           /* 滚动偏移 */
    bool active;                    /* 是否活动 */
} tab_t;

/* 书签结构 */
typedef struct bookmark {
    char name[MAX_NAME_LENGTH];     /* 书签名称 */
    char path[MAX_PATH_LENGTH];     /* 书签路径 */
    time_t created;                 /* 创建时间 */
} bookmark_t;

/* 历史记录结构 */
typedef struct history_entry {
    char path[MAX_PATH_LENGTH];     /* 路径 */
    time_t timestamp;               /* 时间戳 */
} history_entry_t;

/* 应用配置结构 */
typedef struct config {
    /* 显示设置 */
    display_mode_t default_display_mode; /* 默认显示模式 */
    bool show_hidden_files;         /* 显示隐藏文件 */
    bool show_icons;                /* 显示图标 */
    bool show_preview;              /* 显示预览 */
    size_t preview_max_size;        /* 预览最大大小 */

    /* 行为设置 */
    bool confirm_delete;            /* 确认删除 */
    bool use_trash;                 /* 使用回收站 */
    bool auto_cd;                   /* 自动进入目录 */
    size_t max_history;             /* 最大历史记录数 */
    size_t max_tabs;                /* 最大标签页数 */

    /* 界面设置 */
    size_t left_panel_width;        /* 左侧面板宽度 */
    size_t right_panel_width;       /* 右侧面板宽度 */
    size_t preview_width;           /* 预览面板宽度 */
    char *theme;                    /* 主题名称 */
    char *icon_set;                 /* 图标集合 */

    /* 快捷键设置 */
    char **keybindings;             /* 快捷键绑定 */

    /* 插件设置 */
    char **enabled_plugins;         /* 启用的插件 */
} config_t;

/* 应用状态结构 */
typedef struct app_state {
    tab_t *tabs[MAX_TABS];          /* 标签页数组 */
    size_t tab_count;               /* 标签页数量 */
    size_t active_tab;              /* 活动标签页 */

    bookmark_t *bookmarks[MAX_BOOKMARKS]; /* 书签数组 */
    size_t bookmark_count;          /* 书签数量 */

    history_entry_t *history[MAX_HISTORY]; /* 历史记录数组 */
    size_t history_count;           /* 历史记录数量 */
    size_t history_index;           /* 历史记录索引 */

    config_t *config;               /* 配置 */
    operation_mode_t mode;          /* 操作模式 */

    char *status_message;           /* 状态消息 */
    size_t message_timeout;         /* 消息显示时间 */

    void *plugin_manager;           /* 插件管理器 */
    void *preview_manager;          /* 预览管理器 */
    void *task_manager;             /* 任务管理器 */

    bool running;                   /* 运行状态 */
} app_state_t;

/* 函数声明 */

/* 应用初始化和清理 */
app_state_t *f1le_create(void);
void f1le_destroy(app_state_t *app);
bool f1le_init(app_state_t *app);
void f1le_run(app_state_t *app);
void f1le_quit(app_state_t *app);

/* 文件列表管理 */
file_list_t *file_list_create(void);
void file_list_destroy(file_list_t *list);
bool file_list_load_directory(file_list_t *list, const char *path);
bool file_list_sort(file_list_t *list, sort_type_t type, bool reverse);
bool file_list_filter(file_list_t *list, const char *pattern);
file_info_t *file_list_get_current(const file_list_t *list);
file_info_t *file_list_get_by_index(const file_list_t *list, size_t index);

/* 标签页管理 */
tab_t *tab_create(const char *name, const char *path);
void tab_destroy(tab_t *tab);
bool tab_set_path(tab_t *tab, const char *path);
bool tab_goto_parent(tab_t *tab);
bool tab_goto_child(tab_t *tab, const char *name);

/* 书签管理 */
bool bookmark_add(app_state_t *app, const char *name, const char *path);
bool bookmark_remove(app_state_t *app, const char *name);
bookmark_t *bookmark_find(app_state_t *app, const char *name);
void bookmark_load(app_state_t *app, const char *filename);
void bookmark_save(app_state_t *app, const char *filename);

/* 历史记录管理 */
void history_add(app_state_t *app, const char *path);
const char *history_get(app_state_t *app, int offset);
void history_load(app_state_t *app, const char *filename);
void history_save(app_state_t *app, const char *filename);

/* 配置管理 */
config_t *config_create(void);
void config_destroy(config_t *config);
bool config_load(config_t *config, const char *filename);
bool config_save(config_t *config, const char *filename);
void config_set_default(config_t *config);

/* 文件操作 */
bool file_copy(const char *src, const char *dst);
bool file_move(const char *src, const char *dst);
bool file_delete(const char *path);
bool file_rename(const char *old_path, const char *new_path);
bool file_mkdir(const char *path, mode_t mode);
bool file_chmod(const char *path, mode_t mode);
bool file_chown(const char *path, uid_t uid, gid_t gid);

/* 搜索功能 */
file_info_t **file_search(const char *path, const char *pattern, size_t *count);
file_info_t **file_search_content(const char *path, const char *pattern, size_t *count);

/* 预览功能 */
char *file_preview_text(const char *path, size_t max_lines);
char *file_preview_image(const char *path, size_t max_width, size_t max_height);
char *file_preview_archive(const char *path);

/* 工具函数 */
const char *file_type_to_string(file_type_t type);
const char *file_size_to_string(size_t size);
const char *file_time_to_string(time_t time);
bool file_is_text(const char *path);
bool file_is_image(const char *path);
bool file_is_archive(const char *path);
bool file_is_executable(const char *path);

/* 错误处理 */
typedef enum {
    F1LE_OK = 0,                 /* 成功 */
    F1LE_ERROR_PERMISSION = 1,   /* 权限错误 */
    F1LE_ERROR_NOT_FOUND = 2,    /* 文件不存在 */
    F1LE_ERROR_IO = 3,           /* I/O错误 */
    F1LE_ERROR_MEMORY = 4,       /* 内存错误 */
    F1LE_ERROR_INVALID_ARG = 5,  /* 无效参数 */
    F1LE_ERROR_NOT_SUPPORTED = 6 /* 不支持的操作 */
} f1le_error_t;

const char *f1le_error_to_string(f1le_error_t error);

#endif /* __F1LE_H__ */
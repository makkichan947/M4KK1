/*
 * M4KK1 VI编辑器 - 编辑器核心接口
 * 定义编辑器的主要数据结构和函数接口
 */

#ifndef __VI_EDITOR_H__
#define __VI_EDITOR_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

/* 编辑器版本信息 */
#define VI_VERSION_MAJOR 8
#define VI_VERSION_MINOR 2
#define VI_VERSION_PATCH 0
#define VI_VERSION_STRING "8.2"

/* 编辑器模式枚举 */
typedef enum {
    MODE_NORMAL = 0,    /* 普通模式 */
    MODE_INSERT = 1,    /* 插入模式 */
    MODE_VISUAL = 2,    /* 可视模式 */
    MODE_COMMAND = 3,   /* 命令模式 */
    MODE_REPLACE = 4,   /* 替换模式 */
    MODE_OPERATOR = 5   /* 操作符模式 */
} editor_mode_t;

/* 缓冲区类型枚举 */
typedef enum {
    BUFFER_TYPE_FILE = 0,    /* 文件缓冲区 */
    BUFFER_TYPE_SCRATCH = 1, /* 临时缓冲区 */
    BUFFER_TYPE_HELP = 2,    /* 帮助缓冲区 */
    BUFFER_TYPE_DIR = 3      /* 目录缓冲区 */
} buffer_type_t;

/* 光标位置结构 */
typedef struct {
    size_t line;        /* 行号 (0-based) */
    size_t col;         /* 列号 (0-based) */
    size_t offset;      /* 在文件中的字节偏移 */
} cursor_t;

/* 缓冲区结构 */
typedef struct buffer {
    char *data;              /* 文本数据 */
    size_t size;             /* 当前大小 */
    size_t capacity;         /* 容量 */
    char *filename;          /* 文件名 */
    bool modified;           /* 修改标志 */
    bool readonly;           /* 只读标志 */
    buffer_type_t type;      /* 缓冲区类型 */
    cursor_t cursor;         /* 光标位置 */
    struct buffer *next;     /* 下一个缓冲区 */
    struct buffer *prev;     /* 上一个缓冲区 */
} buffer_t;

/* 窗口结构 */
typedef struct window {
    buffer_t *buffer;        /* 关联的缓冲区 */
    cursor_t cursor;         /* 窗口光标位置 */
    size_t top_line;         /* 顶部显示行 */
    size_t left_col;         /* 左侧显示列 */
    size_t rows;             /* 窗口行数 */
    size_t cols;             /* 窗口列数 */
    bool active;             /* 是否活动窗口 */
    struct window *next;     /* 下一个窗口 */
    struct window *prev;     /* 上一个窗口 */
} window_t;

/* 编辑器配置结构 */
typedef struct {
    bool show_line_numbers;  /* 显示行号 */
    bool show_status_bar;    /* 显示状态栏 */
    bool show_tab_line;      /* 显示标签栏 */
    bool auto_indent;        /* 自动缩进 */
    bool expand_tab;         /* 展开Tab为空格 */
    size_t tab_size;         /* Tab大小 */
    size_t shift_width;      /* 缩进宽度 */
    bool hl_search;          /* 高亮搜索结果 */
    bool inc_search;         /* 增量搜索 */
    bool ignore_case;        /* 忽略大小写 */
    bool smart_case;         /* 智能大小写 */
    char *colorscheme;       /* 颜色主题 */
} config_t;

/* 编辑器主结构 */
typedef struct {
    buffer_t *buffers;       /* 缓冲区列表 */
    window_t *windows;       /* 窗口列表 */
    buffer_t *current_buffer; /* 当前缓冲区 */
    window_t *current_window; /* 当前窗口 */
    editor_mode_t mode;      /* 当前模式 */
    config_t *config;        /* 配置 */
    char *status_message;    /* 状态消息 */
    size_t message_timeout;  /* 消息显示时间 */
    void *userdata;          /* 用户数据 */
} editor_t;

/* 函数声明 */

/* 编辑器初始化和清理 */
editor_t *editor_create(void);
void editor_destroy(editor_t *editor);
bool editor_init(editor_t *editor);
void editor_quit(editor_t *editor);

/* 缓冲区管理 */
buffer_t *buffer_create(const char *filename);
buffer_t *buffer_create_scratch(void);
void buffer_destroy(buffer_t *buffer);
bool buffer_load_file(buffer_t *buffer, const char *filename);
bool buffer_save_file(buffer_t *buffer, const char *filename);
bool buffer_insert_char(buffer_t *buffer, size_t pos, char ch);
bool buffer_delete_char(buffer_t *buffer, size_t pos);
bool buffer_insert_text(buffer_t *buffer, size_t pos, const char *text, size_t len);
bool buffer_delete_text(buffer_t *buffer, size_t pos, size_t len);

/* 窗口管理 */
window_t *window_create(editor_t *editor, buffer_t *buffer);
void window_destroy(window_t *window);
void window_resize(window_t *window, size_t rows, size_t cols);
void window_scroll(window_t *window, int lines);

/* 编辑操作 */
bool editor_insert_mode(editor_t *editor);
bool editor_normal_mode(editor_t *editor);
bool editor_visual_mode(editor_t *editor);
bool editor_command_mode(editor_t *editor);
bool editor_process_key(editor_t *editor, int key);
bool editor_execute_command(editor_t *editor, const char *cmd);

/* 显示和渲染 */
void editor_refresh(editor_t *editor);
void editor_draw_status_bar(editor_t *editor);
void editor_draw_line_numbers(editor_t *editor, window_t *window);

/* 配置管理 */
config_t *config_create(void);
void config_destroy(config_t *config);
bool config_load(config_t *config, const char *filename);
bool config_save(config_t *config, const char *filename);

/* 搜索和替换 */
size_t *editor_search(editor_t *editor, const char *pattern, size_t start_pos);
bool editor_replace(editor_t *editor, const char *pattern, const char *replacement);

/* 撤销和重做 */
bool editor_undo(editor_t *editor);
bool editor_redo(editor_t *editor);

/* 宏录制和回放 */
bool editor_start_macro(editor_t *editor, char reg);
bool editor_stop_macro(editor_t *editor, char reg);
bool editor_play_macro(editor_t *editor, char reg);

/* 插件系统接口 */
typedef struct plugin plugin_t;
plugin_t *plugin_load(const char *filename);
void plugin_unload(plugin_t *plugin);
bool plugin_call(plugin_t *plugin, const char *func, void *data);

#endif /* __VI_EDITOR_H__ */
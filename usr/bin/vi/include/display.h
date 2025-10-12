/*
 * M4KK1 VI编辑器 - 显示管理接口
 * 定义显示系统和渲染相关功能
 */

#ifndef __VI_DISPLAY_H__
#define __VI_DISPLAY_H__

#include "editor.h"
#include "buffer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 颜色定义 */
#define COLOR_BLACK     0
#define COLOR_RED       1
#define COLOR_GREEN     2
#define COLOR_YELLOW    3
#define COLOR_BLUE      4
#define COLOR_MAGENTA   5
#define COLOR_CYAN      6
#define COLOR_WHITE     7
#define COLOR_BRIGHT    8

/* 属性定义 */
#define ATTR_NORMAL     0
#define ATTR_BOLD       1
#define ATTR_DIM        2
#define ATTR_UNDERLINE  4
#define ATTR_BLINK      5
#define ATTR_REVERSE    7
#define ATTR_STANDOUT   8

/* 颜色对结构 */
typedef struct {
    short fg;           /* 前景色 */
    short bg;           /* 背景色 */
    short attr;         /* 属性 */
} color_pair_t;

/* 主题结构 */
typedef struct {
    char *name;         /* 主题名称 */
    color_pair_t colors[256]; /* 颜色表 */
    color_pair_t syntax_colors[128]; /* 语法高亮颜色 */
} theme_t;

/* 语法元素类型 */
typedef enum {
    SYN_NONE = 0,       /* 无 */
    SYN_KEYWORD,        /* 关键字 */
    SYN_TYPE,           /* 类型 */
    SYN_FUNCTION,       /* 函数 */
    SYN_STRING,         /* 字符串 */
    SYN_COMMENT,        /* 注释 */
    SYN_CONSTANT,       /* 常量 */
    SYN_NUMBER,         /* 数字 */
    SYN_OPERATOR,       /* 操作符 */
    SYN_DELIMITER,      /* 分隔符 */
    SYN_IDENTIFIER,     /* 标识符 */
    SYN_PREPROCESSOR,   /* 预处理器 */
    SYN_ERROR,          /* 错误 */
    SYN_TODO,           /* 待办事项 */
    SYN_SEARCH,         /* 搜索结果 */
    SYN_SELECTION,      /* 选择区域 */
    SYN_LINENUMBER,     /* 行号 */
    SYN_STATUS,         /* 状态栏 */
    SYN_CURSOR,         /* 光标 */
    SYN_VISUAL,         /* 可视模式 */
    SYN_MATCH,          /* 匹配项 */
    SYN_SPELL,          /* 拼写检查 */
} syntax_element_t;

/* 高亮规则结构 */
typedef struct highlight_rule {
    syntax_element_t element;   /* 语法元素 */
    char *pattern;              /* 匹配模式 */
    bool regex;                 /* 是否正则表达式 */
    struct highlight_rule *next; /* 下一个规则 */
} highlight_rule_t;

/* 语法文件结构 */
typedef struct {
    char *name;                 /* 语法名称 */
    char *extensions[32];       /* 文件扩展名 */
    highlight_rule_t *rules;     /* 高亮规则 */
    color_pair_t *colors;       /* 颜色映射 */
} syntax_file_t;

/* 显示单元结构 */
typedef struct display_cell {
    char ch;                    /* 字符 */
    color_pair_t color;         /* 颜色 */
    short attr;                 /* 属性 */
} display_cell_t;

/* 状态栏信息结构 */
typedef struct {
    char *left_text;            /* 左侧文本 */
    char *right_text;           /* 右侧文本 */
    color_pair_t color;         /* 颜色 */
    bool active;                /* 是否活动 */
} status_info_t;

/* 窗口显示信息 */
typedef struct {
    size_t first_line;          /* 首行 */
    size_t last_line;           /* 末行 */
    size_t first_col;           /* 首列 */
    size_t last_col;            /* 末列 */
    size_t cursor_line;         /* 光标行 */
    size_t cursor_col;          /* 光标列 */
    bool line_wrap;             /* 行换行 */
    bool show_break;            /* 显示换行符 */
} window_view_t;

/* 函数声明 */

/* 显示系统初始化 */
bool display_init(void);
void display_cleanup(void);
bool display_resize(size_t rows, size_t cols);

/* 主题管理 */
theme_t *theme_create(const char *name);
void theme_destroy(theme_t *theme);
bool theme_load(theme_t *theme, const char *filename);
bool theme_save(const theme_t *theme, const char *filename);
theme_t *theme_get_default(void);

/* 语法高亮 */
syntax_file_t *syntax_load(const char *filename);
void syntax_destroy(syntax_file_t *syntax);
syntax_element_t syntax_highlight_line(const syntax_file_t *syntax, const char *line, color_pair_t *colors);
syntax_file_t *syntax_detect(const char *filename);

/* 窗口渲染 */
void window_render(window_t *window, display_cell_t *screen, size_t rows, size_t cols);
void window_draw_text(window_t *window, size_t row, size_t col, const char *text, color_pair_t color);
void window_draw_cursor(window_t *window, display_cell_t *screen);

/* 状态栏渲染 */
void status_bar_render(const status_info_t *info, display_cell_t *screen, size_t cols);
status_info_t *status_info_create(void);
void status_info_destroy(status_info_t *info);
void status_info_update(status_info_t *info, const editor_t *editor);

/* 行号渲染 */
void line_numbers_render(const buffer_t *buffer, size_t first_line, size_t last_line,
                        display_cell_t *screen, size_t row, size_t width);

/* 屏幕刷新 */
void display_refresh(const editor_t *editor);
void display_clear(void);
void display_flush(void);

/* 颜色管理 */
color_pair_t color_make(short fg, short bg, short attr);
void color_apply(const color_pair_t *color);

/* 光标管理 */
void cursor_move(size_t row, size_t col);
void cursor_hide(void);
void cursor_show(void);
void cursor_set_shape(int shape);

/* 终端操作 */
void terminal_save(void);
void terminal_restore(void);
bool terminal_check_size(size_t *rows, size_t *cols);

#endif /* __VI_DISPLAY_H__ */
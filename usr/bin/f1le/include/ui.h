/*
 * M4KK1 F1le文件管理器 - 用户界面接口
 * 定义用户界面组件和渲染系统
 */

#ifndef __F1LE_UI_H__
#define __F1LE_UI_H__

#include "f1le.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 颜色定义 */
#define COLOR_BLACK         0
#define COLOR_RED           1
#define COLOR_GREEN         2
#define COLOR_YELLOW        3
#define COLOR_BLUE          4
#define COLOR_MAGENTA       5
#define COLOR_CYAN          6
#define COLOR_WHITE         7
#define COLOR_BRIGHT_BLACK  8
#define COLOR_BRIGHT_RED    9
#define COLOR_BRIGHT_GREEN  10
#define COLOR_BRIGHT_YELLOW 11
#define COLOR_BRIGHT_BLUE   12
#define COLOR_BRIGHT_MAGENTA 13
#define COLOR_BRIGHT_CYAN   14
#define COLOR_BRIGHT_WHITE  15

/* 颜色对结构 */
typedef struct {
    short foreground;       /* 前景色 */
    short background;       /* 背景色 */
    int attributes;         /* 属性（粗体、下划线等） */
} color_pair_t;

/* 位置和尺寸结构 */
typedef struct {
    size_t x;               /* X坐标 */
    size_t y;               /* Y坐标 */
    size_t width;           /* 宽度 */
    size_t height;          /* 高度 */
} rect_t;

/* 组件基类结构 */
typedef struct component {
    rect_t bounds;          /* 边界矩形 */
    bool visible;           /* 是否可见 */
    bool focused;           /* 是否获得焦点 */
    color_pair_t colors;    /* 颜色方案 */
    void *userdata;         /* 用户数据 */

    /* 虚函数表 */
    void (*draw)(struct component *comp);
    bool (*handle_key)(struct component *comp, int key);
    void (*handle_resize)(struct component *comp, rect_t new_bounds);
    void (*destroy)(struct component *comp);
} component_t;

/* 文件列表组件 */
typedef struct {
    component_t base;       /* 基类 */
    file_list_t *file_list; /* 文件列表 */
    size_t scroll_offset;   /* 滚动偏移 */
    size_t selected_index;  /* 选中索引 */
    display_mode_t display_mode; /* 显示模式 */
} file_list_component_t;

/* 预览组件 */
typedef struct {
    component_t base;       /* 基类 */
    file_info_t *current_file; /* 当前预览文件 */
    char *preview_data;     /* 预览数据 */
    size_t preview_size;    /* 预览数据大小 */
    preview_type_t preview_type; /* 预览类型 */
} preview_component_t;

/* 状态栏组件 */
typedef struct {
    component_t base;       /* 基类 */
    char *left_text;        /* 左侧文本 */
    char *right_text;       /* 右侧文本 */
    char *center_text;      /* 中央文本 */
    int progress;           /* 进度百分比 */
} status_bar_component_t;

/* 标签栏组件 */
typedef struct {
    component_t base;       /* 基类 */
    tab_t **tabs;           /* 标签页数组 */
    size_t tab_count;       /* 标签页数量 */
    size_t active_tab;      /* 活动标签页 */
} tab_bar_component_t;

/* 主界面结构 */
typedef struct {
    component_t *components; /* 组件列表 */
    size_t component_count;  /* 组件数量 */

    file_list_component_t *left_panel;   /* 左侧面板 */
    file_list_component_t *right_panel;  /* 右侧面板 */
    preview_component_t *preview_panel;  /* 预览面板 */
    status_bar_component_t *status_bar;  /* 状态栏 */
    tab_bar_component_t *tab_bar;        /* 标签栏 */

    rect_t screen_size;      /* 屏幕尺寸 */
    operation_mode_t mode;   /* 操作模式 */
    char *status_message;    /* 状态消息 */
} ui_t;

/* 函数声明 */

/* UI系统初始化 */
ui_t *ui_create(void);
void ui_destroy(ui_t *ui);
bool ui_init(ui_t *ui);
void ui_refresh(ui_t *ui);

/* 组件管理 */
component_t *component_create(size_t width, size_t height);
void component_destroy(component_t *comp);
void component_move(component_t *comp, size_t x, size_t y);
void component_resize(component_t *comp, size_t width, size_t height);
void component_set_colors(component_t *comp, color_pair_t colors);

/* 文件列表组件 */
file_list_component_t *file_list_component_create(rect_t bounds, file_list_t *file_list);
void file_list_component_destroy(file_list_component_t *comp);
void file_list_component_set_display_mode(file_list_component_t *comp, display_mode_t mode);
void file_list_component_scroll(file_list_component_t *comp, int lines);

/* 预览组件 */
preview_component_t *preview_component_create(rect_t bounds);
void preview_component_destroy(preview_component_t *comp);
bool preview_component_set_file(preview_component_t *comp, file_info_t *file);
void preview_component_clear(preview_component_t *comp);

/* 状态栏组件 */
status_bar_component_t *status_bar_component_create(rect_t bounds);
void status_bar_component_destroy(status_bar_component_t *comp);
void status_bar_component_set_text(status_bar_component_t *comp,
                                  const char *left, const char *center, const char *right);
void status_bar_component_set_progress(status_bar_component_t *comp, int progress);

/* 标签栏组件 */
tab_bar_component_t *tab_bar_component_create(rect_t bounds);
void tab_bar_component_destroy(tab_bar_component_t *comp);
bool tab_bar_component_add_tab(tab_bar_component_t *comp, const char *name, const char *path);
bool tab_bar_component_remove_tab(tab_bar_component_t *comp, size_t index);
void tab_bar_component_set_active_tab(tab_bar_component_t *comp, size_t index);

/* 布局管理 */
void ui_layout_panels(ui_t *ui);
void ui_calculate_layout(ui_t *ui, rect_t screen_size);

/* 渲染系统 */
void ui_draw(ui_t *ui);
void ui_draw_panel(ui_t *ui, file_list_component_t *panel, rect_t bounds);
void ui_draw_preview(ui_t *ui, preview_component_t *preview, rect_t bounds);
void ui_draw_status_bar(ui_t *ui, status_bar_component_t *status, rect_t bounds);
void ui_draw_tab_bar(ui_t *ui, tab_bar_component_t *tabs, rect_t bounds);

/* 输入处理 */
bool ui_handle_key(ui_t *ui, int key);
bool ui_handle_resize(ui_t *ui, size_t width, size_t height);

/* 主题系统 */
bool ui_load_theme(ui_t *ui, const char *theme_name);
void ui_set_default_theme(ui_t *ui);
color_pair_t ui_get_color_pair(ui_t *ui, const char *name);

/* 图标系统 */
const char *ui_get_file_icon(file_type_t type, const char *extension);
void ui_load_icon_set(const char *icon_set);

/* 光标管理 */
void ui_move_cursor(ui_t *ui, size_t x, size_t y);
void ui_show_cursor(ui_t *ui);
void ui_hide_cursor(ui_t *ui);

#endif /* __F1LE_UI_H__ */
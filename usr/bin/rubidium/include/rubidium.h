/*
 * M4KK1 Rubidium桌面环境 - 主接口头文件
 * 定义平铺式桌面环境的核心数据结构和接口
 */

#ifndef __RUBIDIUM_H__
#define __RUBIDIUM_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>

/* 版本信息 */
#define RUBIDIUM_VERSION_MAJOR 1
#define RUBIDIUM_VERSION_MINOR 0
#define RUBIDIUM_VERSION_PATCH 0
#define RUBIDIUM_VERSION_STRING "1.0.0"

/* 常量定义 */
#define MAX_WORKSPACES      32
#define MAX_WINDOWS         1024
#define MAX_ANIMATIONS      256
#define MAX_KEYBINDINGS     512
#define MAX_WINDOW_RULES    128

/* 布局方向枚举 */
typedef enum {
    LAYOUT_HORIZONTAL = 0,  /* 水平布局 */
    LAYOUT_VERTICAL = 1,    /* 垂直布局 */
    LAYOUT_STACKED = 2,     /* 堆叠布局 */
    LAYOUT_FLOATING = 3     /* 浮动布局 */
} layout_direction_t;

/* 工作区布局类型枚举 */
typedef enum {
    WORKSPACE_LAYOUT_MASTER_STACK = 0, /* 主从布局 */
    WORKSPACE_LAYOUT_DWINDLE = 1,      /* 二进制布局 */
    WORKSPACE_LAYOUT_SPIRAL = 2,       /* 螺旋布局 */
    WORKSPACE_LAYOUT_GRID = 3,         /* 网格布局 */
    WORKSPACE_LAYOUT_MONOCLE = 4       /* 单窗口布局 */
} workspace_layout_t;

/* 窗口类型枚举 */
typedef enum {
    WINDOW_TYPE_NORMAL = 0,     /* 普通窗口 */
    WINDOW_TYPE_DIALOG = 1,     /* 对话框 */
    WINDOW_TYPE_SPLASH = 2,     /* 启动画面 */
    WINDOW_TYPE_NOTIFICATION = 3, /* 通知窗口 */
    WINDOW_TYPE_DOCK = 4,       /* 停靠栏 */
    WINDOW_TYPE_PANEL = 5       /* 面板 */
} window_type_t;

/* 窗口状态枚举 */
typedef enum {
    WINDOW_STATE_ACTIVE = 0,    /* 活动状态 */
    WINDOW_STATE_INACTIVE = 1,  /* 非活动状态 */
    WINDOW_STATE_MINIMIZED = 2, /* 最小化 */
    WINDOW_STATE_MAXIMIZED = 3, /* 最大化 */
    WINDOW_STATE_FULLSCREEN = 4, /* 全屏 */
    WINDOW_STATE_FLOATING = 5   /* 浮动状态 */
} window_state_t;

/* 动画类型枚举 */
typedef enum {
    ANIMATION_FADE = 0,     /* 淡入淡出 */
    ANIMATION_SLIDE = 1,    /* 滑动 */
    ANIMATION_SCALE = 2,    /* 缩放 */
    ANIMATION_ROTATE = 3,   /* 旋转 */
    ANIMATION_BOUNCE = 4,   /* 弹跳 */
    ANIMATION_CUSTOM = 5    /* 自定义动画 */
} animation_type_t;

/* 缓动函数枚举 */
typedef enum {
    EASING_LINEAR = 0,      /* 线性 */
    EASING_EASE_IN = 1,     /* 缓入 */
    EASING_EASE_OUT = 2,    /* 缓出 */
    EASING_EASE_IN_OUT = 3, /* 缓入缓出 */
    EASING_BOUNCE = 4,      /* 弹跳 */
    EASING_ELASTIC = 5      /* 弹性 */
} easing_function_t;

/* 窗口结构 */
typedef struct window {
    uint32_t id;            /* 窗口ID */
    char *title;            /* 窗口标题 */
    char *class;            /* 窗口类名 */
    char *instance;         /* 窗口实例名 */

    window_type_t type;     /* 窗口类型 */
    window_state_t state;   /* 窗口状态 */

    /* 几何信息 */
    int32_t x, y;           /* 窗口位置 */
    uint32_t width, height; /* 窗口大小 */
    uint32_t min_width, min_height; /* 最小尺寸 */
    uint32_t max_width, max_height; /* 最大尺寸 */

    /* 布局信息 */
    layout_direction_t layout_dir; /* 布局方向 */
    struct window_node *node;     /* 布局节点 */

    /* 表面和缓冲区 */
    surface_t *surface;     /* 窗口表面 */
    buffer_t *buffer;       /* 窗口缓冲区 */

    /* 动画信息 */
    animation_t *animation; /* 当前动画 */

    /* 属性 */
    bool decorated;         /* 是否装饰 */
    bool resizable;         /* 是否可调整大小 */
    bool minimizable;       /* 是否可最小化 */
    bool maximizable;       /* 是否可最大化 */
    bool fullscreenable;    /* 是否可全屏 */

    /* 元数据 */
    pid_t pid;              /* 应用程序进程ID */
    char *app_id;           /* 应用程序ID */
    time_t created;         /* 创建时间 */
    time_t last_focused;    /* 最后聚焦时间 */

    struct window *next;    /* 下一个窗口 */
    void *userdata;         /* 用户数据 */
} window_t;

/* 工作区结构 */
typedef struct workspace {
    uint32_t id;            /* 工作区ID */
    char *name;             /* 工作区名称 */
    workspace_layout_t layout; /* 布局类型 */

    /* 窗口管理 */
    window_t **windows;     /* 窗口列表 */
    size_t window_count;    /* 窗口数量 */
    window_t *active_window; /* 活动窗口 */

    /* 布局信息 */
    struct window_node *root; /* 布局树根节点 */
    layout_direction_t direction; /* 主布局方向 */

    /* 视觉属性 */
    char *background;       /* 背景图片 */
    char *background_color; /* 背景颜色 */
    uint32_t gap_size;      /* 窗口间隙 */
    uint32_t border_width;  /* 边框宽度 */

    /* 动画设置 */
    animation_config_t animations; /* 动画配置 */

    /* 状态 */
    bool active;            /* 是否活动 */
    bool urgent;            /* 是否紧急 */
    time_t last_activity;   /* 最后活动时间 */

    struct workspace *next; /* 下一个工作区 */
} workspace_t;

/* 布局节点结构 */
typedef struct window_node {
    layout_direction_t direction; /* 布局方向 */
    double ratio;           /* 分割比例 */

    /* 几何信息 */
    double x, y;            /* 相对位置 */
    double width, height;   /* 相对大小 */

    /* 内容 */
    window_t *window;       /* 关联窗口 */
    struct window_node **children; /* 子节点列表 */
    size_t child_count;     /* 子节点数量 */

    /* 父子关系 */
    struct window_node *parent; /* 父节点 */

    /* 属性 */
    bool resizable;         /* 是否可调整大小 */
    double min_ratio;       /* 最小比例 */
    double max_ratio;       /* 最大比例 */
} window_node_t;

/* 动画结构 */
typedef struct animation {
    animation_type_t type;  /* 动画类型 */
    easing_function_t easing; /* 缓动函数 */

    /* 时间信息 */
    double duration;        /* 持续时间 */
    double delay;           /* 延迟时间 */
    double start_time;      /* 开始时间 */

    /* 状态 */
    bool active;            /* 是否活动 */
    bool finished;          /* 是否完成 */
    double progress;        /* 进度 (0.0-1.0) */

    /* 变换信息 */
    vec2_t start_position;  /* 起始位置 */
    vec2_t end_position;    /* 结束位置 */
    vec2_t start_size;      /* 起始大小 */
    vec2_t end_size;        /* 结束大小 */

    /* 关联对象 */
    window_t *window;       /* 关联窗口 */
    workspace_t *workspace; /* 关联工作区 */

    struct animation *next; /* 下一个动画 */
    void *userdata;         /* 用户数据 */
} animation_t;

/* 键盘快捷键结构 */
typedef struct keybinding {
    uint32_t modifiers;     /* 修饰键 */
    uint32_t key;           /* 按键 */
    char *command;          /* 命令 */
    char *description;      /* 描述 */

    struct keybinding *next; /* 下一个快捷键 */
} keybinding_t;

/* 窗口规则结构 */
typedef struct window_rule {
    char *class_pattern;    /* 类名模式 */
    char *title_pattern;    /* 标题模式 */
    char *app_id_pattern;   /* 应用ID模式 */

    /* 规则动作 */
    workspace_t *target_workspace; /* 目标工作区 */
    layout_direction_t layout_dir; /* 布局方向 */
    window_state_t initial_state; /* 初始状态 */
    bool floating;          /* 浮动窗口 */

    /* 匹配选项 */
    bool regex_match;       /* 正则匹配 */
    bool case_sensitive;    /* 大小写敏感 */

    struct window_rule *next; /* 下一个规则 */
} window_rule_t;

/* 配置结构 */
typedef struct config {
    /* 通用设置 */
    char *mod_key;          /* 修饰键 */
    uint32_t border_width;  /* 边框宽度 */
    uint32_t gap_size;      /* 间隙大小 */
    bool animation_enabled; /* 启用动画 */

    /* 布局设置 */
    workspace_layout_t default_layout; /* 默认布局 */
    double master_ratio;    /* 主区域比例 */
    uint32_t master_count;  /* 主窗口数量 */
    layout_direction_t orientation; /* 方向 */

    /* 动画设置 */
    bool animations_enabled; /* 动画启用 */
    double animation_speed; /* 动画速度 */
    easing_function_t default_easing; /* 默认缓动 */

    /* 键盘快捷键 */
    keybinding_t **keybindings; /* 快捷键列表 */
    size_t keybinding_count; /* 快捷键数量 */

    /* 窗口规则 */
    window_rule_t **window_rules; /* 窗口规则列表 */
    size_t window_rule_count; /* 规则数量 */

    /* 主题设置 */
    char *theme;            /* 主题名称 */
    color_t *colors;        /* 颜色表 */
    font_t *fonts;          /* 字体设置 */
} config_t;

/* 桌面环境结构 */
typedef struct desktop_environment {
    /* 核心组件 */
    window_t **windows;     /* 窗口列表 */
    size_t window_count;    /* 窗口数量 */
    window_t *active_window; /* 活动窗口 */

    workspace_t **workspaces; /* 工作区列表 */
    size_t workspace_count; /* 工作区数量 */
    workspace_t *active_workspace; /* 活动工作区 */

    /* 动画系统 */
    animation_t **animations; /* 动画列表 */
    size_t animation_count; /* 动画数量 */

    /* 配置和主题 */
    config_t *config;       /* 配置 */
    theme_t *theme;         /* 当前主题 */

    /* 输入管理 */
    keybinding_t **keybindings; /* 键盘快捷键 */
    input_manager_t *input_mgr; /* 输入管理器 */

    /* 渲染系统 */
    renderer_t *renderer;   /* 渲染器 */
    compositor_t *compositor; /* 合成器 */

    /* 状态 */
    bool running;           /* 运行状态 */
    bool locked;            /* 锁定状态 */

    /* 统计信息 */
    desktop_stats_t stats;  /* 统计信息 */

    pthread_mutex_t mutex;  /* 互斥锁 */
} desktop_environment_t;

/* 函数声明 */

/* 桌面环境管理 */
desktop_environment_t *rubidium_create(void);
void rubidium_destroy(desktop_environment_t *desktop);
bool rubidium_init(desktop_environment_t *desktop);
void rubidium_run(desktop_environment_t *desktop);
void rubidium_quit(desktop_environment_t *desktop);

/* 窗口管理 */
window_t *window_create(uint32_t id, const char *title, window_type_t type);
void window_destroy(window_t *window);
bool window_set_geometry(window_t *window, int32_t x, int32_t y, uint32_t width, uint32_t height);
bool window_set_state(window_t *window, window_state_t state);
bool window_focus(window_t *window);
bool window_add_to_workspace(window_t *window, workspace_t *workspace);

/* 工作区管理 */
workspace_t *workspace_create(uint32_t id, const char *name, workspace_layout_t layout);
void workspace_destroy(workspace_t *workspace);
bool workspace_add_window(workspace_t *workspace, window_t *window);
bool workspace_remove_window(workspace_t *workspace, window_t *window);
bool workspace_switch_to(workspace_t *workspace);
bool workspace_set_layout(workspace_t *workspace, workspace_layout_t layout);

/* 布局管理 */
window_node_t *layout_create_node(layout_direction_t direction);
void layout_destroy_node(window_node_t *node);
bool layout_split_node(window_node_t *node, layout_direction_t direction, double ratio);
bool layout_add_window(window_node_t *node, window_t *window);
bool layout_calculate_layout(window_node_t *root, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

/* 动画管理 */
animation_t *animation_create(animation_type_t type, double duration);
void animation_destroy(animation_t *animation);
bool animation_start(animation_t *animation, window_t *window);
bool animation_stop(animation_t *animation);
bool animation_update(animation_t *animation, double delta_time);

/* 键盘快捷键管理 */
keybinding_t *keybinding_create(uint32_t modifiers, uint32_t key, const char *command);
void keybinding_destroy(keybinding_t *keybinding);
bool keybinding_register(desktop_environment_t *desktop, keybinding_t *keybinding);
bool keybinding_unregister(desktop_environment_t *desktop, uint32_t modifiers, uint32_t key);
keybinding_t *keybinding_find(desktop_environment_t *desktop, uint32_t modifiers, uint32_t key);

/* 窗口规则管理 */
window_rule_t *window_rule_create(const char *pattern, window_rule_action_t action);
void window_rule_destroy(window_rule_t *rule);
bool window_rule_matches(window_rule_t *rule, window_t *window);
bool window_rule_apply(window_rule_t *rule, window_t *window);

/* 配置管理 */
config_t *config_create(void);
void config_destroy(config_t *config);
bool config_load(config_t *config, const char *filename);
bool config_save(config_t *config, const char *filename);
void config_set_default(config_t *config);

/* 工具函数 */
const char *layout_direction_to_string(layout_direction_t direction);
const char *workspace_layout_to_string(workspace_layout_t layout);
const char *window_type_to_string(window_type_t type);
const char *window_state_to_string(window_state_t state);
const char *animation_type_to_string(animation_type_t type);

/* 错误处理 */
typedef enum {
    RUBIDIUM_OK = 0,                     /* 成功 */
    RUBIDIUM_ERROR_NOT_FOUND = 1,        /* 未找到 */
    RUBIDIUM_ERROR_PERMISSION = 2,       /* 权限错误 */
    RUBIDIUM_ERROR_INVALID_ARG = 3,      /* 无效参数 */
    RUBIDIUM_ERROR_INVALID_STATE = 4,    /* 无效状态 */
    RUBIDIUM_ERROR_OUT_OF_MEMORY = 5,    /* 内存不足 */
    RUBIDIUM_ERROR_ALREADY_EXISTS = 6,   /* 已存在 */
    RUBIDIUM_ERROR_NOT_SUPPORTED = 7,    /* 不支持 */
    RUBIDIUM_ERROR_BUSY = 8,             /* 忙碌 */
    RUBIDIUM_ERROR_TIMEOUT = 9           /* 超时 */
} rubidium_error_t;

const char *rubidium_error_to_string(rubidium_error_t error);

#endif /* __RUBIDIUM_H__ */
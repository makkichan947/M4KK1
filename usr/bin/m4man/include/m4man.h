/*
 * M4KK1 M4man手册系统 - 主接口头文件
 * 定义TUI手册浏览器的核心数据结构和接口
 */

#ifndef __M4MAN_H__
#define __M4MAN_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>

/* 版本信息 */
#define M4MAN_VERSION_MAJOR 1
#define M4MAN_VERSION_MINOR 0
#define M4MAN_VERSION_PATCH 0
#define M4MAN_VERSION_STRING "1.0.0"

/* 常量定义 */
#define MAX_DOCUMENTS       1000
#define MAX_BOOKMARKS       100
#define MAX_HISTORY         200
#define MAX_SEARCH_RESULTS  500
#define MAX_LINE_LENGTH     4096
#define MAX_PAGES           10000

/* 文档类型枚举 */
typedef enum {
    DOC_TYPE_MAN = 0,       /* man页面 */
    DOC_TYPE_INFO = 1,      /* info文档 */
    DOC_TYPE_MARKDOWN = 2,  /* Markdown文档 */
    DOC_TYPE_HTML = 3,      /* HTML文档 */
    DOC_TYPE_PDF = 4,       /* PDF文档 */
    DOC_TYPE_TEXT = 5,      /* 纯文本文档 */
    DOC_TYPE_UNKNOWN = 6    /* 未知类型 */
} document_type_t;

/* 显示模式枚举 */
typedef enum {
    DISPLAY_MODE_NORMAL = 0,    /* 普通模式 */
    DISPLAY_MODE_FULLSCREEN = 1, /* 全屏模式 */
    DISPLAY_MODE_DOUBLE = 2,     /* 双栏模式 */
    DISPLAY_MODE_RAW = 3        /* 原始模式 */
} display_mode_t;

/* 搜索类型枚举 */
typedef enum {
    SEARCH_TYPE_TEXT = 0,       /* 文本搜索 */
    SEARCH_TYPE_TITLE = 1,      /* 标题搜索 */
    SEARCH_TYPE_SECTION = 2,    /* 章节搜索 */
    SEARCH_TYPE_REGEX = 3       /* 正则搜索 */
} search_type_t;

/* 书签类型枚举 */
typedef enum {
    BOOKMARK_TYPE_PAGE = 0,     /* 页面书签 */
    BOOKMARK_TYPE_SECTION = 1,  /* 章节书签 */
    BOOKMARK_TYPE_SEARCH = 2,   /* 搜索书签 */
    BOOKMARK_TYPE_CUSTOM = 3    /* 自定义书签 */
} bookmark_type_t;

/* 文档节点类型枚举 */
typedef enum {
    NODE_TYPE_ROOT = 0,         /* 根节点 */
    NODE_TYPE_CHAPTER = 1,      /* 章节 */
    NODE_TYPE_SECTION = 2,      /* 节 */
    NODE_TYPE_SUBSECTION = 3,   /* 子节 */
    NODE_TYPE_PARAGRAPH = 4,    /* 段落 */
    NODE_TYPE_CODE = 5,         /* 代码块 */
    NODE_TYPE_LIST = 6,         /* 列表 */
    NODE_TYPE_TABLE = 7,        /* 表格 */
    NODE_TYPE_IMAGE = 8,        /* 图像 */
    NODE_TYPE_LINK = 9          /* 链接 */
} node_type_t;

/* 文档节点结构 */
typedef struct document_node {
    node_type_t type;           /* 节点类型 */
    char *title;                /* 标题 */
    char *content;              /* 内容 */
    char *anchor;               /* 锚点标识符 */

    size_t level;               /* 层次级别 */
    size_t line_number;         /* 行号 */
    size_t page_number;         /* 页面号 */

    struct document_node **children; /* 子节点列表 */
    size_t child_count;         /* 子节点数量 */

    struct document_node *parent;   /* 父节点 */
    struct document_node *next;     /* 下一个兄弟节点 */
    struct document_node *prev;     /* 上一个兄弟节点 */

    void *render_data;          /* 渲染数据 */
} document_node_t;

/* 文档结构 */
typedef struct document {
    char *path;                 /* 文档路径 */
    char *name;                 /* 文档名称 */
    char *title;                /* 文档标题 */
    document_type_t type;       /* 文档类型 */

    document_node_t *root;      /* 根节点 */
    size_t node_count;          /* 节点总数 */
    size_t page_count;          /* 页面总数 */

    time_t created;             /* 创建时间 */
    time_t modified;            /* 修改时间 */
    size_t size;                /* 文档大小 */

    char *author;               /* 作者 */
    char *version;              /* 版本 */
    char *description;          /* 描述 */

    struct document *next;      /* 下一个文档 */
} document_t;

/* 书签结构 */
typedef struct bookmark {
    char *name;                 /* 书签名称 */
    char *description;          /* 描述 */
    bookmark_type_t type;       /* 书签类型 */

    char *document_path;        /* 文档路径 */
    char *section_anchor;       /* 章节锚点 */
    size_t page_number;         /* 页面号 */
    size_t line_number;         /* 行号 */

    time_t created;             /* 创建时间 */
    time_t accessed;            /* 访问时间 */
    size_t access_count;        /* 访问次数 */

    struct bookmark *next;      /* 下一个书签 */
} bookmark_t;

/* 历史记录结构 */
typedef struct history_entry {
    char *document_path;        /* 文档路径 */
    char *section_anchor;       /* 章节锚点 */
    size_t page_number;         /* 页面号 */
    size_t line_number;         /* 行号 */

    time_t timestamp;           /* 访问时间 */
    size_t duration;            /* 浏览时长 */

    struct history_entry *next; /* 下一个记录 */
} history_entry_t;

/* 搜索结果结构 */
typedef struct search_result {
    char *document_path;        /* 文档路径 */
    char *section_title;        /* 章节标题 */
    char *content_snippet;      /* 内容片段 */
    size_t line_number;         /* 行号 */
    size_t match_count;         /* 匹配数量 */

    double relevance_score;     /* 相关性评分 */

    struct search_result *next; /* 下一个结果 */
} search_result_t;

/* 配置结构 */
typedef struct config {
    /* 显示设置 */
    char *theme;                /* 主题名称 */
    size_t font_size;           /* 字体大小 */
    double line_height;         /* 行高 */
    bool show_line_numbers;     /* 显示行号 */
    bool show_bookmarks;        /* 显示书签 */
    bool show_history;          /* 显示历史 */

    /* 导航设置 */
    bool auto_bookmarks;        /* 自动书签 */
    size_t max_history;         /* 最大历史记录 */
    size_t max_bookmarks;       /* 最大书签数 */
    bool remember_position;     /* 记住位置 */
    bool smooth_scrolling;      /* 平滑滚动 */

    /* 搜索设置 */
    bool fuzzy_search;          /* 模糊搜索 */
    bool case_sensitive;        /* 大小写敏感 */
    size_t max_results;         /* 最大搜索结果 */
    bool highlight_matches;     /* 高亮匹配 */
    bool search_history;        /* 搜索历史 */

    /* 键盘快捷键 */
    char **keybindings;         /* 快捷键绑定 */
    size_t keybinding_count;    /* 快捷键数量 */

    /* 解析器设置 */
    char **parser_commands;     /* 解析器命令 */
    size_t parser_count;        /* 解析器数量 */
} config_t;

/* 应用状态结构 */
typedef struct app_state {
    document_t **documents;     /* 文档列表 */
    size_t document_count;      /* 文档数量 */

    document_t *current_document; /* 当前文档 */
    document_node_t *current_node; /* 当前节点 */
    size_t current_page;        /* 当前页面 */

    bookmark_t **bookmarks;     /* 书签列表 */
    size_t bookmark_count;      /* 书签数量 */

    history_entry_t **history;  /* 历史记录列表 */
    size_t history_count;       /* 历史记录数量 */
    size_t history_index;       /* 历史记录索引 */

    search_result_t **search_results; /* 搜索结果列表 */
    size_t search_result_count; /* 搜索结果数量 */

    config_t *config;           /* 配置 */
    char *config_file;          /* 配置文件路径 */

    display_mode_t display_mode; /* 显示模式 */
    bool running;               /* 运行状态 */

    void *ui_context;           /* UI上下文 */
    void *parser_context;       /* 解析器上下文 */
    void *renderer_context;     /* 渲染器上下文 */
} app_state_t;

/* 函数声明 */

/* 应用初始化和清理 */
app_state_t *m4man_create(void);
void m4man_destroy(app_state_t *app);
bool m4man_init(app_state_t *app);
void m4man_run(app_state_t *app);
void m4man_quit(app_state_t *app);

/* 文档管理 */
document_t *document_open(const char *path);
void document_close(document_t *doc);
document_t *document_parse_file(const char *filepath);
document_t *document_parse_buffer(const char *buffer, size_t size, document_type_t type);
document_node_t *document_find_node_by_anchor(document_t *doc, const char *anchor);
size_t document_get_page_count(document_t *doc);

/* 导航功能 */
bool document_goto_page(document_t *doc, size_t page);
bool document_goto_line(document_t *doc, size_t line);
bool document_goto_section(document_t *doc, const char *section);
bool document_next_page(document_t *doc);
bool document_prev_page(document_t *doc);

/* 书签管理 */
bookmark_t *bookmark_create(const char *name, document_t *doc, document_node_t *node);
void bookmark_destroy(bookmark_t *bookmark);
bool bookmark_add(app_state_t *app, bookmark_t *bookmark);
bool bookmark_remove(app_state_t *app, const char *name);
bookmark_t *bookmark_find(app_state_t *app, const char *name);
void bookmark_load(app_state_t *app, const char *filename);
void bookmark_save(app_state_t *app, const char *filename);

/* 历史记录管理 */
void history_add(app_state_t *app, document_t *doc, document_node_t *node);
bool history_back(app_state_t *app);
bool history_forward(app_state_t *app);
void history_load(app_state_t *app, const char *filename);
void history_save(app_state_t *app, const char *filename);

/* 搜索功能 */
search_result_t **document_search(document_t *doc, const char *query, search_type_t type, size_t *count);
search_result_t **search_all_documents(const char *query, search_type_t type, size_t *count);
bool search_goto_result(app_state_t *app, search_result_t *result);

/* 配置管理 */
config_t *config_create(void);
void config_destroy(config_t *config);
bool config_load(config_t *config, const char *filename);
bool config_save(config_t *config, const char *filename);
void config_set_default(config_t *config);

/* 渲染和显示 */
bool renderer_init(void);
void renderer_cleanup(void);
bool renderer_render_page(document_t *doc, size_t page, char **output, size_t *lines);
bool renderer_render_node(document_node_t *node, char **output);

/* 工具函数 */
document_type_t document_detect_type(const char *filepath);
const char *document_type_to_string(document_type_t type);
const char *node_type_to_string(node_type_t type);
const char *display_mode_to_string(display_mode_t mode);

/* 错误处理 */
typedef enum {
    M4MAN_OK = 0,                    /* 成功 */
    M4MAN_ERROR_NOT_FOUND = 1,       /* 未找到 */
    M4MAN_ERROR_PERMISSION = 2,      /* 权限错误 */
    M4MAN_ERROR_INVALID_FORMAT = 3,  /* 格式错误 */
    M4MAN_ERROR_PARSE_FAILED = 4,    /* 解析失败 */
    M4MAN_ERROR_RENDER_FAILED = 5,   /* 渲染失败 */
    M4MAN_ERROR_MEMORY = 6,          /* 内存错误 */
    M4MAN_ERROR_IO = 7,              /* I/O错误 */
    M4MAN_ERROR_CANCELLED = 8        /* 已取消 */
} m4man_error_t;

const char *m4man_error_to_string(m4man_error_t error);

#endif /* __M4MAN_H__ */
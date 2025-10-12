# M4KK1 M4man手册系统设计文档

## 概述

M4KK1 M4man是一个先进的终端用户界面（TUI）手册系统，为M4KK1操作系统提供直观、美观、功能强大的文档浏览和帮助系统。与传统的man页面不同，M4man提供了现代化的交互式界面，支持多格式文档显示、搜索、书签、历史记录等高级功能。

## 设计目标

- **界面友好**：美观的TUI界面，提供优秀的阅读体验
- **功能全面**：支持多种文档格式和显示模式
- **搜索强大**：全文搜索、模糊搜索、索引搜索
- **导航便捷**：书签、历史记录、快速跳转
- **扩展性强**：插件系统支持，易于功能扩展

## 核心特性

### 文档浏览

- **多格式支持**：man页面、info文档、markdown、HTML等
- **分页显示**：智能分页和换行处理
- **语法高亮**：代码和标记语言语法高亮
- **图像显示**：内联图像和图表显示
- **表格渲染**：表格的格式化和对齐显示

### 搜索功能

- **全文搜索**：在所有文档中搜索文本内容
- **标题搜索**：按章节标题和子标题搜索
- **模糊搜索**：模糊匹配和拼写纠错
- **搜索历史**：搜索历史记录和快速搜索
- **搜索结果高亮**：搜索结果中的关键词高亮

### 导航和书签

- **书签管理**：添加、删除、管理文档书签
- **历史记录**：浏览历史记录和快速返回
- **快速跳转**：跳转到特定章节或页面
- **面包屑导航**：显示当前位置和导航路径
- **目录树**：文档结构树和快速导航

### 显示选项

- **主题切换**：多种颜色主题选择
- **字体设置**：字体大小和类型的调整
- **布局定制**：界面布局的个性化配置
- **全屏模式**：全屏阅读模式
- **双栏显示**：双栏对比显示文档

### 交互操作

- **键盘快捷键**：丰富的键盘操作支持
- **鼠标支持**：鼠标点击和滚轮操作
- **触摸支持**：触摸屏手势操作
- **语音控制**：语音导航和搜索
- **手势识别**：手势导航和操作

## 架构设计

### 系统架构图

```
┌─────────────────────────────────────────────────────────────┐
│                    M4man手册系统                            │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │   主界面    │  │   文档浏览器 │  │   搜索面板   │           │
│  │  (Main UI)   │  │  (Browser)  │  │  (Search)   │           │
│  └─────────────┘  └─────────────┘  └─────────────┘           │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │  文档解析    │  │   渲染引擎   │  │   搜索索引   │           │
│  │  (Parser)   │  │  (Renderer) │  │  (Indexer)  │           │
│  └─────────────┘  └─────────────┘  └─────────────┘           │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │  书签管理    │  │   历史管理   │  │   配置管理   │           │
│  │  (Bookmark)  │  │  (History)  │  │  (Config)   │           │
│  └─────────────┘  └─────────────┘  └─────────────┘           │
└─────────────────────────────────────────────────────────────┘
```

### 模块关系图

```
┌─────────────────┐    ┌─────────────────┐
│     主程序       │───▶│    配置管理      │
│    (main.c)     │    │   (config.c)    │
└─────────────────┘    └─────────────────┘
         │                       │
         ▼                       ▼
┌─────────────────┐    ┌─────────────────┐
│   界面管理器     │    │   文档管理器     │
│  (ui_manager)   │◄──►│ (document_mgr)  │
└─────────────────┘    └─────────────────┘
         │                       │
         ▼                       ▼
┌─────────────────┐    ┌─────────────────┐
│   渲染引擎       │    │   搜索管理器     │
│  (renderer)     │◄──►│ (search_mgr)    │
└─────────────────┘    └─────────────────┘
         │                       │
         ▼                       ▼
┌─────────────────┐    ┌─────────────────┐
│   书签管理器     │    │   历史管理器     │
│  (bookmark_mgr) │◄──►│ (history_mgr)   │
└─────────────────┘    └─────────────────┘
```

## 数据结构设计

### 文档结构

```c
// 文档类型枚举
typedef enum {
    DOC_TYPE_MAN = 0,       /* man页面 */
    DOC_TYPE_INFO = 1,      /* info文档 */
    DOC_TYPE_MARKDOWN = 2,  /* Markdown文档 */
    DOC_TYPE_HTML = 3,      /* HTML文档 */
    DOC_TYPE_PDF = 4,       /* PDF文档 */
    DOC_TYPE_TEXT = 5,      /* 纯文本文档 */
    DOC_TYPE_UNKNOWN = 6    /* 未知类型 */
} document_type_t;

// 文档节点类型枚举
typedef enum {
    NODE_TYPE_DOCUMENT = 0, /* 文档根节点 */
    NODE_TYPE_SECTION = 1,  /* 章节 */
    NODE_TYPE_SUBSECTION = 2, /* 子章节 */
    NODE_TYPE_PARAGRAPH = 3, /* 段落 */
    NODE_TYPE_CODE = 4,     /* 代码块 */
    NODE_TYPE_LIST = 5,     /* 列表 */
    NODE_TYPE_TABLE = 6,    /* 表格 */
    NODE_TYPE_IMAGE = 7,    /* 图像 */
    NODE_TYPE_LINK = 8      /* 链接 */
} node_type_t;

// 文档节点结构
typedef struct document_node {
    node_type_t type;       /* 节点类型 */
    char *title;            /* 标题 */
    char *content;          /* 内容 */
    char *anchor;           /* 锚点标识符 */

    struct document_node **children; /* 子节点列表 */
    size_t child_count;     /* 子节点数量 */

    int level;              /* 层次级别 */
    bool expanded;          /* 是否展开 */

    struct document_node *parent; /* 父节点 */
    struct document_node *next;   /* 下一个兄弟节点 */
    struct document_node *prev;   /* 上一个兄弟节点 */
} document_node_t;

// 文档结构
typedef struct {
    char *path;             /* 文档路径 */
    char *name;             /* 文档名称 */
    char *title;            /* 文档标题 */
    document_type_t type;   /* 文档类型 */

    document_node_t *root;  /* 文档根节点 */
    size_t node_count;      /* 节点总数 */

    time_t created;         /* 创建时间 */
    time_t modified;        /* 修改时间 */
    size_t size;            /* 文档大小 */

    char *author;           /* 作者 */
    char *version;          /* 版本 */
    char *description;      /* 描述 */

    struct document *next;  /* 下一个文档 */
} document_t;
```

### 书签结构

```c
// 书签类型枚举
typedef enum {
    BOOKMARK_PAGE = 0,      /* 页面书签 */
    BOOKMARK_SECTION = 1,   /* 章节书签 */
    BOOKMARK_SEARCH = 2,    /* 搜索书签 */
    BOOKMARK_CUSTOM = 3     /* 自定义书签 */
} bookmark_type_t;

// 书签结构
typedef struct {
    char *name;             /* 书签名称 */
    char *description;      /* 书签描述 */
    bookmark_type_t type;   /* 书签类型 */

    char *document_path;    /* 文档路径 */
    char *section_anchor;   /* 章节锚点 */
    size_t page_number;     /* 页面号 */
    size_t line_number;     /* 行号 */
    size_t column_number;   /* 列号 */

    time_t created;         /* 创建时间 */
    time_t accessed;        /* 访问时间 */
    size_t access_count;    /* 访问次数 */

    char *tags[16];         /* 标签列表 */
    size_t tag_count;       /* 标签数量 */

    struct bookmark *next;  /* 下一个书签 */
} bookmark_t;
```

### 历史记录结构

```c
// 历史记录结构
typedef struct {
    char *document_path;    /* 文档路径 */
    char *section_anchor;   /* 章节锚点 */
    size_t page_number;     /* 页面号 */
    size_t line_number;     /* 行号 */

    time_t timestamp;       /* 访问时间 */
    size_t duration;        /* 浏览时长 */

    struct history_entry *next; /* 下一个记录 */
} history_entry_t;
```

### 搜索结果结构

```c
// 搜索结果结构
typedef struct {
    char *document_path;    /* 文档路径 */
    char *section_title;    /* 章节标题 */
    char *content_snippet;  /* 内容片段 */
    size_t line_number;     /* 行号 */
    size_t match_count;     /* 匹配数量 */

    char *matched_text;     /* 匹配的文本 */
    size_t match_start;     /* 匹配起始位置 */
    size_t match_end;       /* 匹配结束位置 */

    double relevance_score; /* 相关性评分 */

    struct search_result *next; /* 下一个结果 */
} search_result_t;
```

## 用户界面设计

### 主界面布局

```
┌─────────────────────────────────────────────────────────────┐
│ M4man: Manual Browser v1.0                                  │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────┬─────────────────┬─────────────────────┐ │
│  │    文档列表       │    文档内容       │      搜索面板        │ │
│  │   (Documents)   │   (Content)      │     (Search)        │ │
│  │                 │                 │                     │ │
│  │  ┌─────────────┐ │  ┌─────────────┐ │  ┌─────────────────┐ │ │
│  │  │ man pages   │ │  │ Chapter 1   │ │  │ Search: [_____] │ │ │
│  │  │ info docs   │ │  │ Section A   │ │  │ Results: 15     │ │ │
│  │  │ markdown    │ │  │ Section B   │ │  │                 │ │ │
│  │  │ html docs   │ │  │ Section C   │ │  │ [Search] [Clear]│ │ │
│  │  └─────────────┘ │  │ ...         │ │  └─────────────────┘ │ │
│  │                 │  │             │ │                     │ │
│  │  [Browse]       │  │ [Prev] [Next]│ │  [Bookmarks]        │ │
│  └─────────────────┴─────────────────┴─────────────────────┘ │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ Status: Ready | Page: 1/25 | Bookmarks: 3 | History: 12  │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### 菜单结构

```
File (F)
├── Open Document (Ctrl+O)
├── Open Directory (Ctrl+D)
├── Recent Documents (Ctrl+R)
├── Bookmarks (Ctrl+B)
├── History (Ctrl+H)
└── Exit (Ctrl+Q)

Edit (E)
├── Copy (Ctrl+C)
├── Search (Ctrl+F)
├── Find Next (F3)
├── Find Previous (Shift+F3)
├── Go to Line (Ctrl+G)
└── Preferences (Ctrl+P)

View (V)
├── Zoom In (Ctrl++)
├── Zoom Out (Ctrl+-)
├── Fit Width (Ctrl+W)
├── Full Screen (F11)
├── Show Bookmarks (F2)
├── Show History (F4)
└── Themes (F12)

Navigate (N)
├── Back (Alt+Left)
├── Forward (Alt+Right)
├── Up (Alt+Up)
├── Down (Alt+Down)
├── Home (Home)
├── End (End)
├── Page Up (Page Up)
└── Page Down (Page Down)

Tools (T)
├── Search All (Ctrl+Shift+F)
├── Index (Ctrl+I)
├── Table of Contents (Ctrl+T)
├── Export (Ctrl+E)
└── Print (Ctrl+P)

Help (H)
├── User Guide (F1)
├── Keyboard Shortcuts (Shift+F1)
├── About (F12)
└── License
```

### 键盘快捷键

```c
// 导航快捷键
Arrow Keys      /* 上下左右导航 */
Page Up/Down    /* 翻页 */
Home/End        /* 跳转到开头/结尾 */
Tab             /* 切换面板 */
Enter           /* 确认/进入 */

// 文档操作
Ctrl+O          /* 打开文档 */
Ctrl+F          /* 搜索 */
Ctrl+G          /* 跳转到行 */
Ctrl+B          /* 添加书签 */
Ctrl+H          /* 查看历史 */

// 显示控制
Ctrl++          /* 放大 */
Ctrl+-          /* 缩小 */
Ctrl+W          /* 适应宽度 */
F11             /* 全屏 */

// 应用控制
Ctrl+Q          /* 退出 */
F1              /* 帮助 */
F12             /* 主题切换 */
```

## 技术实现

### 文档解析器

```c
// 解析器接口结构
typedef struct {
    document_type_t type;   /* 支持的文档类型 */
    char *extension;        /* 文件扩展名 */

    /* 解析函数 */
    document_t *(*parse_file)(const char *filepath);
    document_t *(*parse_buffer)(const char *buffer, size_t size);

    /* 格式化函数 */
    char *(*format_node)(document_node_t *node);
    char *(*format_document)(document_t *doc);

    struct document_parser *next; /* 下一个解析器 */
} document_parser_t;

// 解析器管理器结构
typedef struct {
    document_parser_t **parsers; /* 解析器列表 */
    size_t parser_count;    /* 解析器数量 */

    document_t *(*detect_type)(const char *filepath);
    document_parser_t *(*get_parser)(document_type_t type);
} parser_manager_t;
```

### 渲染引擎

```c
// 渲染器结构
typedef struct {
    char *name;             /* 渲染器名称 */
    document_type_t type;   /* 支持的文档类型 */

    /* 渲染函数 */
    int (*render_node)(document_node_t *node, render_context_t *ctx);
    int (*render_document)(document_t *doc, render_context_t *ctx);

    /* 布局函数 */
    void (*layout_page)(page_t *page, render_context_t *ctx);
    void (*layout_line)(line_t *line, render_context_t *ctx);

    /* 样式函数 */
    void (*apply_style)(style_t *style, render_context_t *ctx);
    void (*apply_theme)(theme_t *theme, render_context_t *ctx);
} document_renderer_t;

// 渲染上下文结构
typedef struct {
    size_t screen_width;    /* 屏幕宽度 */
    size_t screen_height;   /* 屏幕高度 */
    size_t page_width;      /* 页面宽度 */
    size_t page_height;     /* 页面高度 */

    size_t current_line;    /* 当前行 */
    size_t current_column;  /* 当前列 */
    size_t scroll_offset;   /* 滚动偏移 */

    style_t *current_style; /* 当前样式 */
    theme_t *current_theme; /* 当前主题 */

    font_t *font;           /* 字体设置 */
    color_t *colors;        /* 颜色表 */
} render_context_t;
```

### 搜索系统

```c
// 搜索查询结构
typedef struct {
    char *query;            /* 搜索查询 */
    search_type_t type;     /* 搜索类型 */
    search_scope_t scope;   /* 搜索范围 */

    bool case_sensitive;    /* 大小写敏感 */
    bool whole_word;        /* 全词匹配 */
    bool regex;             /* 正则表达式 */

    char **include_patterns; /* 包含模式 */
    char **exclude_patterns; /* 排除模式 */

    size_t max_results;     /* 最大结果数 */
    double min_score;       /* 最小评分 */
} search_query_t;

// 搜索索引结构
typedef struct {
    char *document_path;    /* 文档路径 */
    char *section_title;    /* 章节标题 */
    char *content;          /* 内容文本 */

    size_t line_number;     /* 行号 */
    size_t word_count;      /* 单词数量 */

    /* 索引数据 */
    char **words;           /* 单词列表 */
    size_t *positions;      /* 位置信息 */
    size_t word_count;      /* 单词数量 */

    struct search_index *next; /* 下一个索引 */
} search_index_t;

// 搜索管理器结构
typedef struct {
    search_index_t **indices; /* 搜索索引列表 */
    size_t index_count;     /* 索引数量 */

    /* 搜索选项 */
    bool fuzzy_search;      /* 模糊搜索 */
    bool spell_check;       /* 拼写检查 */
    size_t max_suggestions; /* 最大建议数 */

    /* 性能优化 */
    cache_t *result_cache;  /* 结果缓存 */
    cache_t *word_cache;    /* 单词缓存 */
} search_manager_t;
```

## 配置系统

### 主配置文件格式

```json
{
  "display": {
    "theme": "default",
    "font_size": 12,
    "line_height": 1.2,
    "show_line_numbers": true,
    "show_bookmarks": true,
    "show_history": true
  },
  "navigation": {
    "auto_bookmarks": true,
    "max_history": 100,
    "max_bookmarks": 50,
    "remember_position": true,
    "smooth_scrolling": true
  },
  "search": {
    "fuzzy_search": true,
    "case_sensitive": false,
    "max_results": 100,
    "highlight_matches": true,
    "search_history": true
  },
  "keybindings": {
    "next_page": "Page_Down",
    "prev_page": "Page_Up",
    "search": "Ctrl+F",
    "bookmark": "Ctrl+B",
    "fullscreen": "F11"
  },
  "parsers": {
    "man": "/usr/bin/groff -mandoc -Thtml",
    "markdown": "/usr/bin/markdown",
    "html": "/usr/bin/w3m -dump"
  }
}
```

## 测试策略

### 单元测试

- 文档解析器测试
- 渲染引擎测试
- 搜索功能测试
- 书签管理测试

### 集成测试

- 完整文档浏览流程测试
- 多格式文档显示测试
- 搜索和导航功能测试
- 性能压力测试

### 用户体验测试

- 界面操作便捷性测试
- 键盘快捷键可用性测试
- 多语言文档支持测试
- 无障碍访问测试

## 部署和集成

### 安装配置

1. **编译安装**：从源码编译安装
2. **文档索引**：建立系统文档索引
3. **权限设置**：设置文档访问权限
4. **桌面集成**：集成桌面环境菜单

### 系统集成

- **默认手册浏览器**：设置为系统默认man浏览器
- **文档搜索**：集成系统文档搜索
- **帮助系统**：作为系统帮助系统前端
- **开发工具集成**：集成到开发环境中

## 未来规划

### 短期目标（1-3个月）

- 完成核心浏览功能
- 实现基本搜索功能
- 添加书签管理
- 完善用户界面

### 中期目标（3-6个月）

- 高级渲染引擎
- 多格式支持
- 协作功能
- 云端同步

### 长期目标（6个月以上）

- AI辅助搜索
- 多媒体支持
- 国际化支持
- 移动端版本

## 总结

M4KK1 M4man手册系统是一个功能强大、界面友好的文档浏览工具，不仅提供了传统手册页面的所有功能，还增加了现代化的交互式界面、强大的搜索功能、便捷的导航和书签管理。通过模块化的设计和可扩展的架构，M4man将成为M4KK1系统文档浏览的首选工具，为用户提供专业级的文档阅读体验。
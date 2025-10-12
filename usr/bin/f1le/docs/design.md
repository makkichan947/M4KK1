# M4KK1 F1le文件管理器设计文档

## 概述

M4KK1 F1le文件管理器是专为M4KK1操作系统设计的现代化终端文件管理器，参考了yazi的设计理念，提供了直观、高效、美观的文件管理体验。

## 设计目标

- **高性能**：异步I/O操作，快速的文件浏览和搜索
- **现代化**：采用Rust风格的设计模式，提供优秀的用户体验
- **可扩展**：插件系统支持，高度可定制化
- **美观**：精美的TUI界面，支持图标和主题
- **智能**：智能排序、预览、多媒体支持

## 架构设计

### 核心组件架构

```
┌─────────────────────────────────────────────────────────────┐
│                        F1le文件管理器                        │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │   主界面    │  │   预览面板   │  │   状态栏    │           │
│  │  (Main UI)   │  │  (Preview)   │  │  (Status)   │           │
│  └─────────────┘  └─────────────┘  └─────────────┘           │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │ 文件列表    │  │  目录树     │  │   任务管理   │           │
│  │  (FileList)  │  │  (Tree)     │  │  (Tasks)    │           │
│  └─────────────┘  └─────────────┘  └─────────────┘           │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │   输入处理   │  │   配置管理   │  │   插件系统   │           │
│  │   (Input)   │  │  (Config)   │  │  (Plugin)   │           │
│  └─────────────┘  └─────────────┘  └─────────────┘           │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │ 文件操作    │  │  预览器     │  │   搜索系统   │           │
│  │  (FileOps)  │  │  (Preview)  │  │  (Search)   │           │
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
│   应用管理器     │    │   插件管理器     │
│  (app_manager)  │◄──►│  (plugin_mgr)   │
└─────────────────┘    └─────────────────┘
         │                       │
         ▼                       ▼
┌─────────────────┐    ┌─────────────────┐
│   文件管理器     │    │   预览管理器     │
│  (file_manager) │◄──►│ (preview_mgr)   │
└─────────────────┘    └─────────────────┘
         │                       │
         ▼                       ▼
┌─────────────────┐    ┌─────────────────┐
│   输入管理器     │    │   显示管理器     │
│  (input_mgr)    │◄──►│ (display_mgr)   │
└─────────────────┘    └─────────────────┘
```

## 核心特性

### 文件浏览

- **双面板布局**：支持左右两个面板独立浏览
- **多标签页**：支持多个标签页管理不同目录
- **快速导航**：书签、历史记录、快速跳转
- **智能排序**：多种排序方式（名称、大小、日期、类型）
- **过滤显示**：按文件类型、名称模式过滤

### 文件预览

- **文本预览**：语法高亮显示文本文件内容
- **图像预览**：缩略图显示图片文件
- **多媒体预览**：视频、音频文件信息显示
- **文档预览**：PDF、Office文档预览
- **压缩包预览**：压缩文件内容浏览

### 文件操作

- **基本操作**：复制、移动、删除、重命名
- **批量操作**：多文件批量处理
- **高级操作**：权限修改、所有者变更
- **安全操作**：回收站支持、操作确认
- **快捷操作**：键盘快捷键、鼠标操作

### 搜索功能

- **实时搜索**：输入时实时过滤文件
- **模糊搜索**：模糊匹配文件名
- **内容搜索**：在文件中搜索文本内容
- **正则搜索**：支持正则表达式搜索
- **搜索历史**：搜索历史记录管理

### 插件系统

- **原生插件**：C/C++编写的原生插件
- **脚本插件**：Shell脚本插件支持
- **插件市场**：插件安装和管理
- **插件配置**：插件个性化配置

## 技术实现

### 数据结构设计

```c
// 文件信息结构
typedef struct {
    char *name;              /* 文件名 */
    char *path;              /* 完整路径 */
    size_t size;             /* 文件大小 */
    time_t mtime;            /* 修改时间 */
    time_t atime;            /* 访问时间 */
    mode_t mode;             /* 文件模式 */
    uid_t uid;               /* 用户ID */
    gid_t gid;               /* 组ID */
    char *mime_type;         /* MIME类型 */
    bool is_dir;             /* 是否目录 */
    bool is_hidden;          /* 是否隐藏 */
    bool is_symlink;         /* 是否符号链接 */
    char *symlink_target;    /* 符号链接目标 */
} file_info_t;

// 文件列表结构
typedef struct {
    file_info_t **files;     /* 文件列表 */
    size_t count;            /* 文件数量 */
    size_t capacity;         /* 容量 */
    sort_type_t sort;        /* 排序方式 */
    bool reverse;            /* 逆序排序 */
    char *filter;            /* 过滤模式 */
} file_list_t;

// 应用状态结构
typedef struct {
    file_list_t *current_list; /* 当前文件列表 */
    char *current_path;      /* 当前路径 */
    size_t current_index;    /* 当前选中索引 */
    preview_t *preview;      /* 预览信息 */
    config_t *config;        /* 配置 */
    plugin_manager_t *plugins; /* 插件管理器 */
    task_manager_t *tasks;   /* 任务管理器 */
} app_state_t;
```

### 异步处理架构

```c
// 任务结构
typedef struct {
    task_id_t id;            /* 任务ID */
    task_type_t type;        /* 任务类型 */
    char *description;       /* 任务描述 */
    void *data;              /* 任务数据 */
    task_callback_t callback; /* 回调函数 */
    bool completed;          /* 是否完成 */
    int progress;            /* 进度百分比 */
    char *error;             /* 错误信息 */
} task_t;

// 任务管理器结构
typedef struct {
    task_t **tasks;          /* 任务列表 */
    size_t count;            /* 任务数量 */
    pthread_mutex_t mutex;   /* 互斥锁 */
    pthread_cond_t cond;     /* 条件变量 */
    bool shutdown;           /* 关闭标志 */
} task_manager_t;
```

### 插件系统架构

```c
// 插件接口结构
typedef struct {
    char *name;              /* 插件名称 */
    char *version;           /* 版本 */
    char *description;       /* 描述 */
    plugin_init_t init;      /* 初始化函数 */
    plugin_cleanup_t cleanup; /* 清理函数 */
    plugin_execute_t execute; /* 执行函数 */
    void *data;              /* 插件数据 */
} plugin_t;

// 插件管理器结构
typedef struct {
    plugin_t **plugins;      /* 插件列表 */
    size_t count;            /* 插件数量 */
    char *plugin_dir;        /* 插件目录 */
    config_t *config;        /* 插件配置 */
} plugin_manager_t;
```

## 用户界面设计

### 主界面布局

```
┌─────────────────────────────────────────────────────────────┐
│ F1le: /home/user  [Bookmarks] [History] [Tasks: 0] [Git]     │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────┬─────────────────┬─────────────────────┐ │
│  │    左侧面板      │    右侧面板      │      预览面板        │ │
│  │   (目录树)      │   (文件列表)     │     (Preview)       │ │
│  │                 │                 │                     │ │
│  │  /home          │  Name      Size  │  ┌─────────────────┐ │ │
│  │  ├── user       │  ├── file1.txt  │  │ File: file1.txt │ │ │
│  │  ├── Documents  │  ├── file2.c    │  │ Size: 1.2KB     │ │ │
│  │  ├── Downloads  │  ├── image.jpg  │  │ Type: Text      │ │ │
│  │  └── Pictures   │  └── ...        │  │                 │ │ │
│  │                 │                 │  │ [Preview Content]│ │ │
│  │  [Selected: 3]  │  [Total: 15]    │  │                 │ │ │
│  └─────────────────┴─────────────────┴─────────────────────┘ │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ Normal Mode | Help: ? | Status: Ready | Progress: 100%  │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### 键盘快捷键设计

```c
// 导航快捷键
KEY_UP, KEY_DOWN          /* 上下移动 */
KEY_LEFT, KEY_RIGHT       /* 左右切换面板 */
KEY_HOME, KEY_END         /* 跳转到开头/结尾 */
KEY_PAGE_UP, KEY_PAGE_DOWN /* 翻页 */

// 文件操作快捷键
'y'                       /* 复制 */
'd'                       /* 删除 */
'p'                       /* 粘贴 */
'r'                       /* 重命名 */
'+'                       /* 创建目录 */
'-'                       /* 删除文件 */

// 视图快捷键
't'                       /* 树视图 */
'l'                       /* 列表视图 */
'i'                       /* 图标视图 */
'v'                       /* 分栏视图 */

// 搜索快捷键
'/'                       /* 搜索 */
'*'                       /* 模糊搜索 */
'f'                       /* 过滤 */

// 应用快捷键
':'                       /* 命令模式 */
'?'                       /* 帮助 */
'q'                       /* 退出 */
```

## 配置系统

### 主配置文件结构

```json
{
  "general": {
    "default_layout": "double_panel",
    "show_hidden": false,
    "show_icons": true,
    "confirm_delete": true,
    "use_trash": true
  },
  "display": {
    "theme": "default",
    "icon_set": "nerd_fonts",
    "show_preview": true,
    "preview_max_size": "10MB",
    "column_width": {
      "name": 30,
      "size": 10,
      "date": 15,
      "permissions": 12
    }
  },
  "keybindings": {
    "normal_mode": {
      "j": "move_down",
      "k": "move_up",
      "h": "parent_dir",
      "l": "enter_dir",
      "y": "copy",
      "d": "delete",
      "p": "paste"
    }
  },
  "plugins": {
    "enabled": [
      "git_status",
      "file_preview",
      "image_preview",
      "archive_preview"
    ]
  }
}
```

### 主题系统

```c
// 主题结构
typedef struct {
    char *name;              /* 主题名称 */
    color_scheme_t colors;   /* 颜色方案 */
    icon_set_t icons;        /* 图标集合 */
    font_t font;             /* 字体设置 */
} theme_t;

// 颜色方案
typedef struct {
    color_t background;      /* 背景色 */
    color_t foreground;      /* 前景色 */
    color_t selection;       /* 选择色 */
    color_t cursor;          /* 光标色 */
    color_t status_bar;      /* 状态栏颜色 */
    color_t preview_bg;      /* 预览背景色 */
} color_scheme_t;
```

## 预览系统

### 预览器架构

```c
// 预览器接口
typedef struct {
    char *name;              /* 预览器名称 */
    char **mime_types;       /* 支持的MIME类型 */
    char **extensions;       /* 支持的文件扩展名 */
    preview_generate_t generate; /* 生成预览函数 */
    preview_cleanup_t cleanup;   /* 清理函数 */
} previewer_t;

// 预览数据结构
typedef struct {
    previewer_t *previewer;  /* 预览器 */
    void *data;              /* 预览数据 */
    size_t size;             /* 数据大小 */
    char *error;             /* 错误信息 */
} preview_t;
```

### 支持的预览类型

- **文本文件**：语法高亮、行号显示
- **图像文件**：缩略图生成、EXIF信息
- **视频文件**：封面提取、媒体信息
- **音频文件**：封面、元数据、波形图
- **文档文件**：文本提取、页面预览
- **压缩文件**：内容列表、解压预览
- **二进制文件**：十六进制查看、结构分析

## 插件系统设计

### 插件类型

1. **预览插件**：提供文件预览功能
2. **操作插件**：提供文件操作功能
3. **界面插件**：扩展用户界面
4. **工具插件**：提供实用工具

### 插件开发接口

```c
// 插件接口定义
typedef struct {
    char *name;              /* 插件名称 */
    char *version;           /* 版本 */
    char *description;       /* 描述 */

    /* 生命周期函数 */
    int (*init)(app_t *app); /* 初始化 */
    void (*cleanup)(void);   /* 清理 */
    void (*reload)(void);    /* 重新加载 */

    /* 事件处理函数 */
    void (*on_file_selected)(file_info_t *file);
    void (*on_directory_changed)(const char *path);
    void (*on_key_pressed)(int key);

    /* 功能函数 */
    bool (*can_handle)(file_info_t *file);
    preview_t *(*generate_preview)(file_info_t *file);
    bool (*execute_action)(const char *action, file_info_t **files, size_t count);
} plugin_interface_t;
```

## 性能优化

### 异步处理

- **文件列表加载**：后台异步加载目录内容
- **文件搜索**：增量搜索，避免阻塞界面
- **预览生成**：异步生成文件预览
- **大文件处理**：分块处理大文件

### 缓存机制

- **文件信息缓存**：缓存文件元数据
- **预览缓存**：缓存生成的预览内容
- **目录缓存**：缓存目录结构信息
- **搜索缓存**：缓存搜索结果

### 内存管理

- **对象池**：复用临时对象
- **延迟加载**：按需加载数据
- **智能清理**：自动清理过期缓存
- **内存限制**：限制内存使用量

## 测试策略

### 单元测试

- 文件操作函数测试
- 预览生成函数测试
- 配置解析测试
- 插件接口测试

### 集成测试

- 文件浏览功能测试
- 搜索功能测试
- 插件系统测试
- 性能测试

### 用户体验测试

- 键盘操作测试
- 鼠标操作测试
- 多语言支持测试
- 无障碍访问测试

## 部署和分发

### 安装方式

1. **包管理器安装**：通过pkgmgr安装
2. **源码编译安装**：从源码构建
3. **便携版**：单文件可执行程序

### 系统集成

- **默认文件管理器**：设置为系统默认
- **上下文菜单**：集成到右键菜单
- **MIME类型关联**：关联文件类型
- **桌面快捷方式**：创建桌面图标

## 未来规划

### 短期目标（1-3个月）

- 完成核心功能开发
- 实现基本插件系统
- 添加多媒体预览支持
- 性能优化和测试

### 中期目标（3-6个月）

- 图形界面版本开发
- 云存储集成
- 协作功能支持
- 高级插件API

### 长期目标（6个月以上）

- 跨平台支持
- 移动端版本
- 在线插件市场
- 企业级特性

## 总结

M4KK1 F1le文件管理器是专为M4KK1操作系统设计的现代化文件管理工具，参考了yazi的设计理念，在保持终端界面简洁高效的同时，提供了丰富的功能和优秀的用户体验。通过模块化的架构设计和插件系统，F1le具有良好的可扩展性和维护性，将成为M4KK1系统不可或缺的核心工具。
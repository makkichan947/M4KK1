/**
 * IVFetch - M4KK1系统信息工具主头文件
 * 定义系统信息收集和显示的核心数据结构和接口
 */

#ifndef _IVFETCH_H
#define _IVFETCH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * IVFetch魔数
 */
#define IVFETCH_MAGIC 0x49564654    /* "IVFT" */

/**
 * IVFetch版本
 */
#define IVFETCH_VERSION_MAJOR 0
#define IVFETCH_VERSION_MINOR 1
#define IVFETCH_VERSION_PATCH 0

/**
 * 常量定义
 */
#define IVFETCH_MAX_MODULES     64      /* 最大模块数量 */
#define IVFETCH_MAX_ARTWORK     1024    /* 最大艺术字大小 */
#define IVFETCH_MAX_CONFIG      4096    /* 最大配置文件大小 */
#define IVFETCH_MAX_CACHE       8192    /* 最大缓存大小 */
#define IVFETCH_MAX_COLORS      16      /* 最大颜色数量 */

/**
 * 布局类型
 */
#define IVFETCH_LAYOUT_SINGLE   0       /* 单栏布局 */
#define IVFETCH_LAYOUT_DOUBLE   1       /* 双栏布局 */
#define IVFETCH_LAYOUT_TRIPLE   2       /* 三栏布局 */
#define IVFETCH_LAYOUT_COMPACT  3       /* 紧凑布局 */

/**
 * 艺术字类型
 */
#define IVFETCH_ARTWORK_ASCII   0       /* ASCII艺术字 */
#define IVFETCH_ARTWORK_UNICODE 1       /* Unicode艺术字 */
#define IVFETCH_ARTWORK_CUSTOM  2       /* 自定义艺术字 */
#define IVFETCH_ARTWORK_NONE    3       /* 无艺术字 */

/**
 * 颜色类型
 */
#define IVFETCH_COLOR_BLACK     0       /* 黑色 */
#define IVFETCH_COLOR_RED       1       /* 红色 */
#define IVFETCH_COLOR_GREEN     2       /* 绿色 */
#define IVFETCH_COLOR_YELLOW    3       /* 黄色 */
#define IVFETCH_COLOR_BLUE      4       /* 蓝色 */
#define IVFETCH_COLOR_MAGENTA   5       /* 洋红色 */
#define IVFETCH_COLOR_CYAN      6       /* 青色 */
#define IVFETCH_COLOR_WHITE     7       /* 白色 */
#define IVFETCH_COLOR_BRIGHT    8       /* 亮色前缀 */

/**
 * 模块类型
 */
#define IVFETCH_MODULE_TITLE    0       /* 标题模块 */
#define IVFETCH_MODULE_SYSTEM   1       /* 系统模块 */
#define IVFETCH_MODULE_HARDWARE 2       /* 硬件模块 */
#define IVFETCH_MODULE_NETWORK  3       /* 网络模块 */
#define IVFETCH_MODULE_USER     4       /* 用户模块 */
#define IVFETCH_MODULE_MEMORY   5       /* 内存模块 */
#define IVFETCH_MODULE_STORAGE  6       /* 存储模块 */
#define IVFETCH_MODULE_PROCESS  7       /* 进程模块 */
#define IVFETCH_MODULE_TIME     8       /* 时间模块 */
#define IVFETCH_MODULE_WEATHER  9       /* 天气模块 */
#define IVFETCH_MODULE_CUSTOM   10      /* 自定义模块 */

/**
 * 颜色结构
 */
typedef struct {
    uint8_t r;                  /* 红色分量 */
    uint8_t g;                  /* 绿色分量 */
    uint8_t b;                  /* 蓝色分量 */
    uint8_t a;                  /* 透明度 */
    bool bright;                /* 亮色标志 */
} ivfetch_color_t;

/**
 * 艺术字结构
 */
typedef struct {
    char *content;              /* 艺术字内容 */
    uint32_t width;             /* 宽度 */
    uint32_t height;            /* 高度 */
    uint32_t type;              /* 艺术字类型 */
    ivfetch_color_t color;      /* 颜色 */
    char *name;                 /* 艺术字名称 */
} ivfetch_artwork_t;

/**
 * 信息模块结构
 */
typedef struct {
    char *name;                 /* 模块名称 */
    uint32_t type;              /* 模块类型 */
    char *format;               /* 显示格式 */
    ivfetch_color_t color;      /* 显示颜色 */
    void *data;                 /* 模块数据 */
    bool enabled;               /* 启用状态 */
    uint32_t priority;          /* 显示优先级 */
    uint32_t (*collect_func)(void *data);  /* 收集函数 */
    uint32_t (*format_func)(void *data, char *buffer, size_t size);  /* 格式化函数 */
} ivfetch_module_t;

/**
 * 布局结构
 */
typedef struct {
    uint32_t type;              /* 布局类型 */
    uint32_t columns;           /* 列数 */
    uint32_t spacing;           /* 间距 */
    bool show_borders;          /* 显示边框 */
    bool center_align;          /* 居中对齐 */
    uint32_t max_width;         /* 最大宽度 */
    uint32_t max_height;        /* 最大高度 */
} ivfetch_layout_t;

/**
 * 主题结构
 */
typedef struct {
    char *name;                 /* 主题名称 */
    char *description;          /* 主题描述 */
    ivfetch_color_t colors[IVFETCH_MAX_COLORS];  /* 颜色表 */
    ivfetch_artwork_t *artwork; /* 默认艺术字 */
    ivfetch_layout_t *layout;   /* 默认布局 */
    char *author;               /* 作者 */
    char *version;              /* 版本 */
} ivfetch_theme_t;

/**
 * 配置结构
 */
typedef struct {
    ivfetch_layout_t *layout;   /* 布局配置 */
    ivfetch_artwork_t *artwork; /* 艺术字配置 */
    ivfetch_theme_t *theme;     /* 主题配置 */
    ivfetch_module_t *modules;  /* 模块列表 */
    uint32_t module_count;      /* 模块数量 */
    char *config_file;          /* 配置文件路径 */
    bool cache_enabled;         /* 启用缓存 */
    uint32_t cache_timeout;     /* 缓存超时时间 */
    bool verbose;               /* 详细输出 */
    bool debug;                 /* 调试模式 */
    bool color_enabled;         /* 启用颜色 */
    bool animation_enabled;     /* 启用动画 */
} ivfetch_config_t;

/**
 * 上下文结构
 */
typedef struct {
    uint32_t magic;             /* 魔数 */
    uint32_t version;           /* 版本 */
    ivfetch_config_t *config;   /* 配置 */
    ivfetch_module_t *modules;  /* 模块列表 */
    uint32_t module_count;      /* 模块数量 */
    void *cache;                /* 缓存数据 */
    char *output_buffer;        /* 输出缓冲区 */
    size_t output_size;         /* 输出大小 */
    uint32_t error_count;       /* 错误计数 */
    bool initialized;           /* 初始化标志 */
} ivfetch_context_t;

/**
 * 系统信息结构
 */
typedef struct {
    char *os_name;              /* 操作系统名称 */
    char *kernel_version;       /* 内核版本 */
    char *architecture;         /* 系统架构 */
    char *hostname;             /* 主机名 */
    char *uptime;               /* 运行时间 */
    char *load_average;         /* 负载平均值 */
    uint32_t process_count;     /* 进程数量 */
    uint32_t thread_count;      /* 线程数量 */
} ivfetch_system_info_t;

/**
 * 硬件信息结构
 */
typedef struct {
    char *cpu_model;            /* CPU型号 */
    uint32_t cpu_cores;         /* CPU核心数 */
    uint32_t cpu_threads;       /* CPU线程数 */
    uint64_t cpu_frequency;     /* CPU频率 */
    char *gpu_model;            /* GPU型号 */
    uint64_t memory_total;      /* 总内存 */
    uint64_t memory_used;       /* 已用内存 */
    uint64_t memory_free;       /* 空闲内存 */
    uint64_t swap_total;        /* 总交换空间 */
    uint64_t swap_used;         /* 已用交换空间 */
    char *disk_model;           /* 磁盘型号 */
    uint64_t disk_total;        /* 磁盘总容量 */
    uint64_t disk_used;         /* 磁盘已用容量 */
} ivfetch_hardware_info_t;

/**
 * 网络信息结构
 */
typedef struct {
    char *primary_ip;           /* 主IP地址 */
    char *mac_address;          /* MAC地址 */
    char *interface_name;       /* 接口名称 */
    char *gateway;              /* 默认网关 */
    char *dns_servers;          /* DNS服务器 */
    uint64_t rx_bytes;          /* 接收字节数 */
    uint64_t tx_bytes;          /* 发送字节数 */
    uint32_t active_connections; /* 活动连接数 */
} ivfetch_network_info_t;

/**
 * 用户信息结构
 */
typedef struct {
    char *username;             /* 用户名 */
    char *real_name;            /* 真实姓名 */
    char *home_dir;             /* 主目录 */
    char *shell;                /* Shell程序 */
    char *terminal;             /* 终端类型 */
    uint32_t uid;               /* 用户ID */
    uint32_t gid;               /* 组ID */
    char *groups;               /* 组列表 */
} ivfetch_user_info_t;

/**
 * 函数声明
 */

/* 上下文管理 */
int ivfetch_init(ivfetch_context_t *ctx);
int ivfetch_cleanup(ivfetch_context_t *ctx);
int ivfetch_load_config(ivfetch_context_t *ctx, const char *config_file);
int ivfetch_save_config(ivfetch_context_t *ctx, const char *config_file);

/* 信息收集 */
int ivfetch_collect_system_info(ivfetch_system_info_t *info);
int ivfetch_collect_hardware_info(ivfetch_hardware_info_t *info);
int ivfetch_collect_network_info(ivfetch_network_info_t *info);
int ivfetch_collect_user_info(ivfetch_user_info_t *info);

/* 模块管理 */
int ivfetch_module_register(ivfetch_context_t *ctx, const char *name, uint32_t type,
                           uint32_t (*collect_func)(void *), uint32_t (*format_func)(void *, char *, size_t));
int ivfetch_module_unregister(ivfetch_context_t *ctx, const char *name);
int ivfetch_module_enable(ivfetch_context_t *ctx, const char *name);
int ivfetch_module_disable(ivfetch_context_t *ctx, const char *name);

/* 显示功能 */
int ivfetch_display(ivfetch_context_t *ctx);
int ivfetch_display_single_column(ivfetch_context_t *ctx);
int ivfetch_display_double_column(ivfetch_context_t *ctx);
int ivfetch_display_triple_column(ivfetch_context_t *ctx);
int ivfetch_display_compact(ivfetch_context_t *ctx);

/* 艺术字处理 */
int ivfetch_artwork_load(ivfetch_artwork_t *artwork, const char *name);
int ivfetch_artwork_create_ascii(ivfetch_artwork_t *artwork, const char *content);
int ivfetch_artwork_create_unicode(ivfetch_artwork_t *artwork, const char *content);
int ivfetch_artwork_set_color(ivfetch_artwork_t *artwork, ivfetch_color_t color);

/* 布局管理 */
int ivfetch_layout_set_type(ivfetch_layout_t *layout, uint32_t type);
int ivfetch_layout_set_spacing(ivfetch_layout_t *layout, uint32_t spacing);
int ivfetch_layout_set_borders(ivfetch_layout_t *layout, bool enabled);
int ivfetch_layout_calculate_dimensions(ivfetch_layout_t *layout, uint32_t content_width, uint32_t content_height);

/* 主题管理 */
int ivfetch_theme_load(ivfetch_theme_t *theme, const char *name);
int ivfetch_theme_create_default(ivfetch_theme_t *theme);
int ivfetch_theme_set_color(ivfetch_theme_t *theme, uint32_t index, ivfetch_color_t color);
int ivfetch_theme_apply(ivfetch_context_t *ctx, ivfetch_theme_t *theme);

/* 缓存管理 */
int ivfetch_cache_init(ivfetch_context_t *ctx);
int ivfetch_cache_put(ivfetch_context_t *ctx, const char *key, void *data, size_t size);
int ivfetch_cache_get(ivfetch_context_t *ctx, const char *key, void **data, size_t *size);
int ivfetch_cache_invalidate(ivfetch_context_t *ctx, const char *key);
int ivfetch_cache_clear(ivfetch_context_t *ctx);

/* 工具函数 */
uint32_t ivfetch_get_version(void);
const char *ivfetch_get_version_string(void);
int ivfetch_strcmp(const char *s1, const char *s2);
size_t ivfetch_strlen(const char *str);
char *ivfetch_strcpy(char *dest, const char *src);
char *ivfetch_strcat(char *dest, const char *src);
char *ivfetch_strdup(const char *str);

/* 内存管理 */
void *ivfetch_malloc(size_t size);
void ivfetch_free(void *ptr);
void *ivfetch_realloc(void *ptr, size_t size);

/* 错误处理 */
void ivfetch_error(const char *message);
void ivfetch_warning(const char *message);
void ivfetch_debug(const char *message);

/* 版本信息宏 */
#define IVFETCH_VERSION \
    (IVFETCH_VERSION_MAJOR << 16 | IVFETCH_VERSION_MINOR << 8 | IVFETCH_VERSION_PATCH)

#define IVFETCH_VERSION_STRING \
    "IVFetch " \
    STRINGIFY(IVFETCH_VERSION_MAJOR) "." \
    STRINGIFY(IVFETCH_VERSION_MINOR) "." \
    STRINGIFY(IVFETCH_VERSION_PATCH)

/* 字符串化宏 */
#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

#endif /* _IVFETCH_H */
</content>
<parameter name="line_count">242</parameter>
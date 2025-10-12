/**
 * MTop - M4KK1系统监控工具主头文件
 * 定义系统监控的核心数据结构和接口
 */

#ifndef _MTOP_H
#define _MTOP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * MTop魔数
 */
#define MTOP_MAGIC 0x4D544F50    /* "MTOP" */

/**
 * MTop版本
 */
#define MTOP_VERSION_MAJOR 0
#define MTOP_VERSION_MINOR 1
#define MTOP_VERSION_PATCH 0

/**
 * 常量定义
 */
#define MTOP_MAX_PROCESSES      1024    /* 最大进程数量 */
#define MTOP_MAX_USERS          256     /* 最大用户数量 */
#define MTOP_MAX_INTERFACES     16      /* 最大网络接口 */
#define MTOP_MAX_FILESYSTEMS    32      /* 最大文件系统 */
#define MTOP_MAX_SENSORS        64      /* 最大传感器 */
#define MTOP_MAX_DISPLAY_LINES  1024    /* 最大显示行数 */

/**
 * 显示模式
 */
#define MTOP_MODE_PROCESS       0       /* 进程模式 */
#define MTOP_MODE_SYSTEM        1       /* 系统模式 */
#define MTOP_MODE_NETWORK       2       /* 网络模式 */
#define MTOP_MODE_STORAGE       3       /* 存储模式 */
#define MTOP_MODE_TREE          4       /* 树模式 */
#define MTOP_MODE_SUMMARY       5       /* 摘要模式 */

/**
 * 排序类型
 */
#define MTOP_SORT_PID           0       /* 按PID排序 */
#define MTOP_SORT_USER          1       /* 按用户排序 */
#define MTOP_SORT_PRIORITY      2       /* 按优先级排序 */
#define MTOP_SORT_NICE          3       /* 按nice值排序 */
#define MTOP_SORT_VIRT          4       /* 按虚拟内存排序 */
#define MTOP_SORT_RES           5       /* 按常驻内存排序 */
#define MTOP_SORT_SHR           6       /* 按共享内存排序 */
#define MTOP_SORT_STATE         7       /* 按状态排序 */
#define MTOP_SORT_CPU           8       /* 按CPU使用率排序 */
#define MTOP_SORT_MEM           9       /* 按内存使用率排序 */
#define MTOP_SORT_TIME          10      /* 按时间排序 */
#define MTOP_SORT_COMMAND       11      /* 按命令排序 */

/**
 * 进程状态
 */
#define MTOP_STATE_RUNNING      'R'     /* 运行中 */
#define MTOP_STATE_SLEEPING     'S'     /* 睡眠 */
#define MTOP_STATE_DISK_SLEEP   'D'     /* 磁盘睡眠 */
#define MTOP_STATE_ZOMBIE       'Z'     /* 僵尸进程 */
#define MTOP_STATE_STOPPED      'T'     /* 停止 */
#define MTOP_STATE_TRACING      't'     /* 跟踪 */
#define MTOP_STATE_DEAD         'X'     /* 死亡 */
#define MTOP_STATE_IDLE         'I'     /* 空闲 */

/**
 * 颜色定义
 */
#define MTOP_COLOR_BLACK        0       /* 黑色 */
#define MTOP_COLOR_RED          1       /* 红色 */
#define MTOP_COLOR_GREEN        2       /* 绿色 */
#define MTOP_COLOR_YELLOW       3       /* 黄色 */
#define MTOP_COLOR_BLUE         4       /* 蓝色 */
#define MTOP_COLOR_MAGENTA      5       /* 洋红色 */
#define MTOP_COLOR_CYAN         6       /* 青色 */
#define MTOP_COLOR_WHITE        7       /* 白色 */

/**
 * 进程信息结构
 */
typedef struct {
    uint32_t pid;               /* 进程ID */
    uint32_t ppid;              /* 父进程ID */
    uint32_t uid;               /* 用户ID */
    uint32_t gid;               /* 组ID */
    char user[32];              /* 用户名 */
    char state;                 /* 进程状态 */
    uint32_t priority;          /* 优先级 */
    int32_t nice;               /* nice值 */
    uint64_t virt;              /* 虚拟内存 */
    uint64_t res;               /* 常驻内存 */
    uint64_t shr;               /* 共享内存 */
    double cpu_percent;         /* CPU使用率 */
    double mem_percent;         /* 内存使用率 */
    uint64_t time;              /* CPU时间 */
    char command[256];          /* 命令行 */
    uint32_t thread_count;      /* 线程数量 */
    uint64_t start_time;        /* 启动时间 */
    char tty[32];               /* 终端 */
    uint32_t flags;             /* 标志位 */
} mtop_process_t;

/**
 * 系统信息结构
 */
typedef struct {
    char os_name[64];           /* 操作系统名称 */
    char kernel_version[64];    /* 内核版本 */
    char architecture[32];      /* 系统架构 */
    char hostname[64];          /* 主机名 */
    uint64_t uptime;            /* 运行时间 */
    double load1;               /* 1分钟负载 */
    double load5;               /* 5分钟负载 */
    double load15;              /* 15分钟负载 */
    uint32_t process_count;     /* 进程数量 */
    uint32_t running_count;     /* 运行中进程数 */
    uint32_t sleeping_count;    /* 睡眠进程数 */
    uint32_t thread_count;      /* 线程数量 */
    uint32_t context_switches;  /* 上下文切换次数 */
} mtop_system_t;

/**
 * 内存信息结构
 */
typedef struct {
    uint64_t total;             /* 总内存 */
    uint64_t used;              /* 已用内存 */
    uint64_t free;              /* 空闲内存 */
    uint64_t shared;            /* 共享内存 */
    uint64_t buffers;           /* 缓冲区 */
    uint64_t cached;            /* 缓存 */
    uint64_t available;         /* 可用内存 */
    uint64_t swap_total;        /* 总交换空间 */
    uint64_t swap_used;         /* 已用交换空间 */
    uint64_t swap_free;         /* 空闲交换空间 */
    double mem_percent;         /* 内存使用率 */
    double swap_percent;        /* 交换空间使用率 */
} mtop_memory_t;

/**
 * CPU信息结构
 */
typedef struct {
    uint32_t count;             /* CPU核心数 */
    uint32_t physical_count;    /* 物理CPU数 */
    uint64_t frequency;         /* CPU频率 */
    double user_percent;        /* 用户CPU使用率 */
    double system_percent;      /* 系统CPU使用率 */
    double nice_percent;        /* nice CPU使用率 */
    double idle_percent;        /* 空闲CPU使用率 */
    double iowait_percent;      /* I/O等待CPU使用率 */
    double irq_percent;         /* 中断CPU使用率 */
    double softirq_percent;     /* 软中断CPU使用率 */
    uint32_t interrupts;        /* 中断次数 */
    uint32_t context_switches;  /* 上下文切换次数 */
    double temperature;         /* CPU温度 */
} mtop_cpu_t;

/**
 * 磁盘信息结构
 */
typedef struct {
    char device[32];            /* 设备名 */
    char mount_point[256];      /* 挂载点 */
    char filesystem[32];        /* 文件系统类型 */
    uint64_t total;             /* 总容量 */
    uint64_t used;              /* 已用容量 */
    uint64_t free;              /* 空闲容量 */
    uint64_t available;         /* 可用容量 */
    double use_percent;         /* 使用率 */
    uint32_t inodes_total;      /* 总索引节点 */
    uint32_t inodes_used;       /* 已用索引节点 */
    uint32_t inodes_free;       /* 空闲索引节点 */
    uint64_t read_bytes;        /* 读取字节数 */
    uint64_t write_bytes;       /* 写入字节数 */
    uint32_t read_count;        /* 读取次数 */
    uint32_t write_count;       /* 写入次数 */
} mtop_disk_t;

/**
 * 网络信息结构
 */
typedef struct {
    char interface[32];         /* 接口名 */
    char ip_address[64];        /* IP地址 */
    char mac_address[32];       /* MAC地址 */
    uint64_t rx_bytes;          /* 接收字节数 */
    uint64_t tx_bytes;          /* 发送字节数 */
    uint64_t rx_packets;        /* 接收数据包 */
    uint64_t tx_packets;        /* 发送数据包 */
    uint64_t rx_errors;         /* 接收错误 */
    uint64_t tx_errors;         /* 发送错误 */
    uint64_t rx_dropped;        /* 接收丢包 */
    uint64_t tx_dropped;        /* 发送丢包 */
    double rx_speed;            /* 接收速度 */
    double tx_speed;            /* 发送速度 */
    uint32_t mtu;               /* MTU */
    bool is_up;                 /* 接口状态 */
} mtop_network_t;

/**
 * 传感器信息结构
 */
typedef struct {
    char name[64];              /* 传感器名称 */
    char type[32];              /* 传感器类型 */
    double value;               /* 传感器值 */
    char unit[16];              /* 单位 */
    double high_limit;          /* 上限 */
    double low_limit;           /* 下限 */
    bool enabled;               /* 启用状态 */
} mtop_sensor_t;

/**
 * 显示配置结构
 */
typedef struct {
    uint32_t mode;              /* 显示模式 */
    uint32_t sort_key;          /* 排序键 */
    bool sort_reverse;          /* 反向排序 */
    bool show_tree;             /* 显示进程树 */
    bool show_threads;          /* 显示线程 */
    bool show_colors;           /* 显示颜色 */
    bool show_borders;          /* 显示边框 */
    uint32_t delay;             /* 刷新间隔 */
    uint32_t iterations;        /* 迭代次数 */
    char *config_file;          /* 配置文件 */
    char *theme;                /* 主题 */
    uint32_t columns[16];       /* 显示列 */
    uint32_t column_count;      /* 列数量 */
} mtop_display_t;

/**
 * 上下文结构
 */
typedef struct {
    uint32_t magic;             /* 魔数 */
    uint32_t version;           /* 版本 */
    mtop_display_t *display;    /* 显示配置 */
    mtop_process_t *processes;  /* 进程列表 */
    uint32_t process_count;     /* 进程数量 */
    mtop_system_t *system;      /* 系统信息 */
    mtop_memory_t *memory;      /* 内存信息 */
    mtop_cpu_t *cpu;            /* CPU信息 */
    mtop_disk_t *disks;         /* 磁盘列表 */
    uint32_t disk_count;        /* 磁盘数量 */
    mtop_network_t *networks;   /* 网络列表 */
    uint32_t network_count;     /* 网络数量 */
    mtop_sensor_t *sensors;     /* 传感器列表 */
    uint32_t sensor_count;      /* 传感器数量 */
    bool running;               /* 运行状态 */
    uint32_t current_sort;      /* 当前排序 */
    uint32_t current_process;   /* 当前进程 */
    char *filter;               /* 过滤器 */
    uint32_t error_count;       /* 错误计数 */
} mtop_context_t;

/**
 * 函数声明
 */

/* 上下文管理 */
int mtop_init(mtop_context_t *ctx);
int mtop_cleanup(mtop_context_t *ctx);
int mtop_load_config(mtop_context_t *ctx, const char *config_file);
int mtop_save_config(mtop_context_t *ctx, const char *config_file);

/* 数据收集 */
int mtop_collect_processes(mtop_context_t *ctx);
int mtop_collect_system_info(mtop_context_t *ctx);
int mtop_collect_memory_info(mtop_context_t *ctx);
int mtop_collect_cpu_info(mtop_context_t *ctx);
int mtop_collect_disk_info(mtop_context_t *ctx);
int mtop_collect_network_info(mtop_context_t *ctx);
int mtop_collect_sensor_info(mtop_context_t *ctx);

/* 数据处理 */
int mtop_sort_processes(mtop_context_t *ctx, uint32_t sort_key, bool reverse);
int mtop_filter_processes(mtop_context_t *ctx, const char *filter);
int mtop_update_process_info(mtop_context_t *ctx, uint32_t pid, mtop_process_t *process);

/* 显示功能 */
int mtop_display(mtop_context_t *ctx);
int mtop_display_header(mtop_context_t *ctx);
int mtop_display_processes(mtop_context_t *ctx);
int mtop_display_system_info(mtop_context_t *ctx);
int mtop_display_meters(mtop_context_t *ctx);
int mtop_display_help(mtop_context_t *ctx);

/* 用户交互 */
int mtop_handle_input(mtop_context_t *ctx, int key);
int mtop_search_process(mtop_context_t *ctx, const char *pattern);
int mtop_filter_by_user(mtop_context_t *ctx, const char *username);
int mtop_filter_by_command(mtop_context_t *ctx, const char *command);

/* 进程操作 */
int mtop_kill_process(mtop_context_t *ctx, uint32_t pid, int signal);
int mtop_renice_process(mtop_context_t *ctx, uint32_t pid, int nice_value);
int mtop_suspend_process(mtop_context_t *ctx, uint32_t pid);
int mtop_resume_process(mtop_context_t *ctx, uint32_t pid);

/* 工具函数 */
uint32_t mtop_get_version(void);
const char *mtop_get_version_string(void);
int mtop_strcmp(const char *s1, const char *s2);
size_t mtop_strlen(const char *str);
char *mtop_strcpy(char *dest, const char *src);
char *mtop_strcat(char *dest, const char *src);
char *mtop_strdup(const char *str);

/* 内存管理 */
void *mtop_malloc(size_t size);
void mtop_free(void *ptr);
void *mtop_realloc(void *ptr, size_t size);

/* 错误处理 */
void mtop_error(const char *message);
void mtop_warning(const char *message);
void mtop_debug(const char *message);

/* 版本信息宏 */
#define MTOP_VERSION \
    (MTOP_VERSION_MAJOR << 16 | MTOP_VERSION_MINOR << 8 | MTOP_VERSION_PATCH)

#define MTOP_VERSION_STRING \
    "MTop " \
    STRINGIFY(MTOP_VERSION_MAJOR) "." \
    STRINGIFY(MTOP_VERSION_MINOR) "." \
    STRINGIFY(MTOP_VERSION_PATCH)

/* 字符串化宏 */
#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

#endif /* _MTOP_H */
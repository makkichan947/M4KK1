/**
 * M4KK1 System Monitor - Header Definitions
 * 系统监控工具头文件定义
 */

#ifndef _M4K_M4KTOP_H
#define _M4K_M4KTOP_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* 监控模式 */
#define M4KTOP_MODE_TEXT 1
#define M4KTOP_MODE_GRAPHICAL 2
#define M4KTOP_MODE_JSON 3

/* 显示选项 */
#define M4KTOP_SHOW_CPU 1
#define M4KTOP_SHOW_MEMORY 2
#define M4KTOP_SHOW_PROCESSES 4
#define M4KTOP_SHOW_NETWORK 8
#define M4KTOP_SHOW_DISK 16
#define M4KTOP_SHOW_SYSTEM 32
#define M4KTOP_SHOW_ALL 63

/* 颜色定义 */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_BOLD    "\033[1m"

/* 系统信息结构 */
typedef struct {
    char hostname[256];
    char kernel_version[64];
    char architecture[32];
    time_t boot_time;
    uint32_t cpu_count;
    uint64_t memory_total;
    uint64_t memory_free;
    uint64_t memory_used;
} system_info_t;

/* CPU信息结构 */
typedef struct {
    double usage_percent;
    double load_1min;
    double load_5min;
    double load_15min;
    uint32_t frequency_mhz;
    char model[128];
} cpu_info_t;

/* 内存信息结构 */
typedef struct {
    uint64_t total_bytes;
    uint64_t free_bytes;
    uint64_t used_bytes;
    uint64_t cached_bytes;
    uint64_t buffers_bytes;
    double usage_percent;
} memory_info_t;

/* 进程信息结构 */
typedef struct {
    uint32_t count;
    uint32_t running;
    uint32_t sleeping;
    uint32_t zombie;
    uint32_t stopped;
} process_info_t;

/* 网络信息结构 */
typedef struct {
    uint64_t rx_bytes;
    uint64_t tx_bytes;
    uint64_t rx_packets;
    uint64_t tx_packets;
    uint64_t rx_errors;
    uint64_t tx_errors;
} network_info_t;

/* 磁盘信息结构 */
typedef struct {
    char mount_point[256];
    uint64_t total_bytes;
    uint64_t free_bytes;
    uint64_t used_bytes;
    double usage_percent;
} disk_info_t;

/* 配置结构 */
typedef struct {
    int mode;
    int interval;
    uint32_t show_flags;
    bool color_output;
    bool compact_mode;
    char *output_file;
} m4ktop_config_t;

/* API函数声明 */
int m4ktop_init(m4ktop_config_t *config);
int m4ktop_run(m4ktop_config_t *config);
void m4ktop_cleanup(void);

/* 信息收集函数 */
int m4ktop_get_system_info(system_info_t *info);
int m4ktop_get_cpu_info(cpu_info_t *info);
int m4ktop_get_memory_info(memory_info_t *info);
int m4ktop_get_process_info(process_info_t *info);
int m4ktop_get_network_info(network_info_t *info);
int m4ktop_get_disk_info(disk_info_t *info, const char *path);

/* 显示函数 */
void m4ktop_display_header(void);
void m4ktop_display_system_info(system_info_t *info);
void m4ktop_display_cpu_info(cpu_info_t *info);
void m4ktop_display_memory_info(memory_info_t *info);
void m4ktop_display_process_info(process_info_t *info);
void m4ktop_display_network_info(network_info_t *info);
void m4ktop_display_disk_info(disk_info_t *info);

/* 工具函数 */
void m4ktop_format_bytes(uint64_t bytes, char *buffer, size_t size);
void m4ktop_format_percentage(double percent, char *buffer, size_t size);
void m4ktop_format_uptime(time_t uptime, char *buffer, size_t size);
const char *m4ktop_get_color(double percent);

/* 配置管理 */
m4ktop_config_t *m4ktop_config_create(void);
void m4ktop_config_destroy(m4ktop_config_t *config);
int m4ktop_config_parse(int argc, char *argv[], m4ktop_config_t *config);

/* 历史数据管理 */
#define MAX_HISTORY 100
typedef struct {
    double cpu_history[MAX_HISTORY];
    double memory_history[MAX_HISTORY];
    uint32_t history_count;
    uint32_t history_index;
} history_data_t;

void history_init(history_data_t *history);
void history_add_cpu(history_data_t *history, double usage);
void history_add_memory(history_data_t *history, double usage);
double history_get_average_cpu(history_data_t *history);
double history_get_average_memory(history_data_t *history);

#endif /* _M4K_M4KTOP_H */
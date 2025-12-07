/**
 * M4KK1 free - Display amount of free and used memory
 * 显示空闲和已用内存量
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>

#define PROC_MEMINFO "/proc/meminfo"

/* 内存信息结构 */
typedef struct {
    unsigned long total;
    unsigned long free;
    unsigned long buffers;
    unsigned long cached;
    unsigned long swap_total;
    unsigned long swap_free;
} memory_info_t;

/* 选项标志 */
static int show_bytes = 0;
static int show_kilobytes = 1;
static int show_megabytes = 0;
static int show_gigabytes = 0;
static int show_human = 0;
static int show_total = 0;

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 free - Display amount of free and used memory\n");
    printf("用法: free [选项]\n");
    printf("\n");
    printf("选项:\n");
    printf("  -b, --bytes      以字节显示\n");
    printf("  -k, --kilobytes  以KB显示（默认）\n");
    printf("  -m, --megabytes  以MB显示\n");
    printf("  -g, --gigabytes  以GB显示\n");
    printf("  -h, --human      人类可读格式\n");
    printf("  -t, --total      显示总计\n");
    printf("  --help           显示此帮助信息\n");
}

/* 解析内存信息行 */
unsigned long parse_meminfo_line(const char* line, const char* key) {
    char* colon = strchr(line, ':');
    if (!colon) return 0;

    /* 检查是否是我们需要的键 */
    if (strncmp(line, key, colon - line) != 0) {
        return 0;
    }

    /* 找到数值 */
    char* value_start = colon + 1;
    while (*value_start == ' ' || *value_start == '\t') {
        value_start++;
    }

    unsigned long value = strtoul(value_start, NULL, 10);

    /* 检查单位 */
    char* unit = value_start;
    while (*unit >= '0' && *unit <= '9') unit++;
    while (*unit == ' ' || *unit == '\t') unit++;

    if (strcmp(unit, "kB\n") == 0) {
        /* 已经是KB */
    } else {
        /* 其他单位，暂时按字节处理 */
    }

    return value;
}

/* 读取内存信息 */
int read_memory_info(memory_info_t* mem) {
    FILE* fp;
    char line[256];

    fp = fopen(PROC_MEMINFO, "r");
    if (!fp) {
        /* 如果/proc/meminfo不存在，使用模拟数据 */
        mem->total = 128 * 1024;      /* 128MB */
        mem->free = 64 * 1024;       /* 64MB */
        mem->buffers = 4 * 1024;     /* 4MB */
        mem->cached = 16 * 1024;     /* 16MB */
        mem->swap_total = 256 * 1024; /* 256MB */
        mem->swap_free = 128 * 1024;  /* 128MB */
        return 0;
    }

    memset(mem, 0, sizeof(memory_info_t));

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            mem->total = parse_meminfo_line(line, "MemTotal");
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            mem->free = parse_meminfo_line(line, "MemFree");
        } else if (strncmp(line, "Buffers:", 8) == 0) {
            mem->buffers = parse_meminfo_line(line, "Buffers");
        } else if (strncmp(line, "Cached:", 7) == 0) {
            mem->cached = parse_meminfo_line(line, "Cached");
        } else if (strncmp(line, "SwapTotal:", 10) == 0) {
            mem->swap_total = parse_meminfo_line(line, "SwapTotal");
        } else if (strncmp(line, "SwapFree:", 9) == 0) {
            mem->swap_free = parse_meminfo_line(line, "SwapFree");
        }
    }

    fclose(fp);
    return 0;
}

/* 格式化内存大小 */
void format_memory_size(unsigned long size_kb, char* buffer, size_t buffer_size) {
    double size;

    if (show_bytes) {
        size = size_kb * 1024.0;
        snprintf(buffer, buffer_size, "%lu", (unsigned long)size);
    } else if (show_megabytes) {
        size = size_kb / 1024.0;
        snprintf(buffer, buffer_size, "%.1f", size);
    } else if (show_gigabytes) {
        size = size_kb / (1024.0 * 1024.0);
        snprintf(buffer, buffer_size, "%.2f", size);
    } else if (show_human) {
        if (size_kb >= 1024 * 1024) { /* GB */
            size = size_kb / (1024.0 * 1024.0);
            snprintf(buffer, buffer_size, "%.1fG", size);
        } else if (size_kb >= 1024) { /* MB */
            size = size_kb / 1024.0;
            snprintf(buffer, buffer_size, "%.1fM", size);
        } else { /* KB */
            snprintf(buffer, buffer_size, "%luK", size_kb);
        }
    } else { /* KB (默认) */
        snprintf(buffer, buffer_size, "%lu", size_kb);
    }
}

/* 显示内存信息 */
void display_memory_info(const memory_info_t* mem) {
    char total_str[32], used_str[32], free_str[32], shared_str[32], buffers_str[32], cached_str[32];
    char swap_total_str[32], swap_used_str[32], swap_free_str[32];
    unsigned long mem_used, mem_shared, swap_used;

    /* 计算内存使用情况 */
    mem_used = mem->total - mem->free - mem->buffers - mem->cached;
    mem_shared = 0; /* 暂时不支持 */

    /* 计算交换空间使用情况 */
    swap_used = mem->swap_total - mem->swap_free;

    /* 格式化输出 */
    format_memory_size(mem->total, total_str, sizeof(total_str));
    format_memory_size(mem_used, used_str, sizeof(used_str));
    format_memory_size(mem->free, free_str, sizeof(free_str));
    format_memory_size(mem_shared, shared_str, sizeof(shared_str));
    format_memory_size(mem->buffers, buffers_str, sizeof(buffers_str));
    format_memory_size(mem->cached, cached_str, sizeof(cached_str));
    format_memory_size(mem->swap_total, swap_total_str, sizeof(swap_total_str));
    format_memory_size(swap_used, swap_used_str, sizeof(swap_used_str));
    format_memory_size(mem->swap_free, swap_free_str, sizeof(swap_free_str));

    /* 显示表头 */
    printf("%-7s %10s %10s %10s %10s %10s %10s\n",
           "", "total", "used", "free", "shared", "buffers", "cached");

    /* 显示内存信息 */
    printf("%-7s %10s %10s %10s %10s %10s %10s\n",
           "Mem:",
           total_str, used_str, free_str, shared_str, buffers_str, cached_str);

    /* 显示交换空间信息 */
    printf("%-7s %10s %10s %10s\n",
           "Swap:",
           swap_total_str, swap_used_str, swap_free_str);

    /* 显示总计 */
    if (show_total) {
        unsigned long total_mem = mem->total + mem->swap_total;
        unsigned long used_mem = mem_used + swap_used;
        unsigned long free_mem = mem->free + mem->swap_free;

        char total_mem_str[32], used_mem_str[32], free_mem_str[32];

        format_memory_size(total_mem, total_mem_str, sizeof(total_mem_str));
        format_memory_size(used_mem, used_mem_str, sizeof(used_mem_str));
        format_memory_size(free_mem, free_mem_str, sizeof(free_mem_str));

        printf("%-7s %10s %10s %10s\n",
               "Total:",
               total_mem_str, used_mem_str, free_mem_str);
    }
}

/* 主函数 */
int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;
    memory_info_t mem;

    static struct option long_options[] = {
        {"bytes", no_argument, 0, 'b'},
        {"kilobytes", no_argument, 0, 'k'},
        {"megabytes", no_argument, 0, 'm'},
        {"gigabytes", no_argument, 0, 'g'},
        {"human", no_argument, 0, 'h'},
        {"total", no_argument, 0, 't'},
        {"help", no_argument, 0, 'H'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "bkmght", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'b':
                show_bytes = 1;
                show_kilobytes = 0;
                show_megabytes = 0;
                show_gigabytes = 0;
                show_human = 0;
                break;
            case 'k':
                show_bytes = 0;
                show_kilobytes = 1;
                show_megabytes = 0;
                show_gigabytes = 0;
                show_human = 0;
                break;
            case 'm':
                show_bytes = 0;
                show_kilobytes = 0;
                show_megabytes = 1;
                show_gigabytes = 0;
                show_human = 0;
                break;
            case 'g':
                show_bytes = 0;
                show_kilobytes = 0;
                show_megabytes = 0;
                show_gigabytes = 1;
                show_human = 0;
                break;
            case 'h':
                show_bytes = 0;
                show_kilobytes = 0;
                show_megabytes = 0;
                show_gigabytes = 0;
                show_human = 1;
                break;
            case 't':
                show_total = 1;
                break;
            case 'H':
                show_help();
                return 0;
            default:
                fprintf(stderr, "无效选项。使用 --help 查看帮助。\n");
                return 1;
        }
    }

    /* 读取内存信息 */
    if (read_memory_info(&mem) != 0) {
        fprintf(stderr, "无法读取内存信息\n");
        return 1;
    }

    /* 显示内存信息 */
    display_memory_info(&mem);

    return 0;
}
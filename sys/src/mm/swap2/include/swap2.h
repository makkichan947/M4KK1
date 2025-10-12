/**
 * Swap2 - 改进版交换系统头文件
 * 定义交换系统的核心数据结构和接口
 */

#ifndef _SWAP2_H
#define _SWAP2_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * Swap2魔数
 */
#define SWAP2_MAGIC 0x53573221    /* "SW2!" */

/**
 * Swap2版本
 */
#define SWAP2_VERSION_MAJOR 0
#define SWAP2_VERSION_MINOR 1
#define SWAP2_VERSION_PATCH 0

/**
 * 页面大小选项
 */
#define SWAP2_PAGE_SIZE_4K   4096
#define SWAP2_PAGE_SIZE_8K   8192
#define SWAP2_PAGE_SIZE_16K  16384
#define SWAP2_PAGE_SIZE_32K  32768

/**
 * 默认页面大小
 */
#define SWAP2_DEFAULT_PAGE_SIZE SWAP2_PAGE_SIZE_4K

/**
 * 超级块大小
 */
#define SWAP2_SUPERBLOCK_SIZE 65536    /* 64KB */

/**
 * 最大交换设备数
 */
#define SWAP2_MAX_DEVICES 16

/**
 * 压缩算法
 */
#define SWAP2_COMPRESSION_NONE    0
#define SWAP2_COMPRESSION_LZ4     1
#define SWAP2_COMPRESSION_ZSTD    2
#define SWAP2_COMPRESSION_LZMA    3
#define SWAP2_COMPRESSION_ADAPTIVE 4

/**
 * 校验算法
 */
#define SWAP2_CHECKSUM_NONE       0
#define SWAP2_CHECKSUM_CRC32C     1
#define SWAP2_CHECKSUM_SHA256     2
#define SWAP2_CHECKSUM_BLAKE3     3

/**
 * 优先级级别
 */
#define SWAP2_PRIORITY_LOW        0
#define SWAP2_PRIORITY_NORMAL     1
#define SWAP2_PRIORITY_HIGH       2
#define SWAP2_PRIORITY_CRITICAL   3

/**
 * 标志位定义
 */
#define SWAP2_FLAG_COMPRESSED     (1 << 0)
#define SWAP2_FLAG_ENCRYPTED      (1 << 1)
#define SWAP2_FLAG_DIRTY          (1 << 2)
#define SWAP2_FLAG_LOCKED         (1 << 3)
#define SWAP2_FLAG_REFERENCED     (1 << 4)
#define SWAP2_FLAG_ACCESSED       (1 << 5)

/**
 * Swap2超级块结构
 */
typedef struct {
    uint32_t magic;              /* Swap2魔数 */
    uint32_t version;            /* 版本号 */
    uint64_t total_pages;        /* 总页面数 */
    uint64_t free_pages;         /* 空闲页面数 */
    uint64_t used_pages;         /* 已用页面数 */
    uint64_t journal_pages;      /* 日志页面数 */
    uint64_t metadata_pages;     /* 元数据页面数 */
    uint32_t page_size;          /* 页面大小 */
    uint32_t compression_alg;    /* 默认压缩算法 */
    uint32_t checksum_alg;       /* 校验算法 */
    uint8_t  uuid[16];           /* 交换空间UUID */
    uint64_t creation_time;      /* 创建时间 */
    uint32_t flags;              /* 标志位 */
    uint8_t  reserved[4040];     /* 保留空间 */
    uint32_t checksum;           /* 校验和 */
} __attribute__((packed)) swap2_superblock_t;

/**
 * 页面头结构
 */
typedef struct {
    uint64_t virtual_address;    /* 虚拟地址 */
    uint32_t process_id;         /* 进程ID */
    uint32_t original_size;      /* 原始大小 */
    uint32_t compressed_size;    /* 压缩后大小 */
    uint32_t compression_alg;    /* 压缩算法 */
    uint32_t checksum_alg;       /* 校验算法 */
    uint64_t swap_time;          /* 交换时间 */
    uint32_t access_count;       /* 访问计数 */
    uint32_t flags;              /* 标志位 */
    uint8_t  reserved[16];       /* 保留空间 */
    uint32_t header_checksum;    /* 头校验和 */
    uint32_t data_checksum;      /* 数据校验和 */
} __attribute__((packed)) swap2_page_header_t;

/**
 * 元数据结构
 */
typedef struct {
    uint64_t page_address;       /* 页面地址 */
    uint64_t swap_location;      /* 交换位置 */
    uint32_t swap_size;          /* 交换大小 */
    uint32_t compression_ratio;  /* 压缩比率 */
    uint64_t last_access;        /* 最后访问时间 */
    uint32_t access_frequency;   /* 访问频率 */
    uint32_t priority;           /* 优先级 */
    uint8_t  flags;              /* 标志位 */
    uint8_t  reserved[7];        /* 保留空间 */
} __attribute__((packed)) swap2_metadata_t;

/**
 * 交换设备信息
 */
typedef struct {
    char *device_name;           /* 设备名称 */
    void *device_handle;         /* 设备句柄 */
    uint64_t start_sector;       /* 起始扇区 */
    uint64_t total_sectors;      /* 总扇区数 */
    uint64_t free_sectors;       /* 空闲扇区数 */
    uint32_t priority;           /* 优先级 */
    bool active;                 /* 激活状态 */
    uint8_t reserved[3];         /* 保留空间 */
} swap2_device_t;

/**
 * 交换统计信息
 */
typedef struct {
    uint64_t total_pages_swapped_in;     /* 换入页面数 */
    uint64_t total_pages_swapped_out;    /* 换出页面数 */
    uint64_t total_bytes_swapped_in;     /* 换入字节数 */
    uint64_t total_bytes_swapped_out;    /* 换出字节数 */
    uint64_t compressed_pages;           /* 压缩页面数 */
    uint64_t total_compression_ratio;    /* 总压缩比率 */
    uint64_t checksum_errors;            /* 校验错误数 */
    uint64_t journal_entries;            /* 日志条目数 */
    uint32_t active_devices;             /* 活动设备数 */
    uint32_t average_swap_time;          /* 平均交换时间 */
} swap2_stats_t;

/**
 * 交换配置
 */
typedef struct {
    uint32_t page_size;          /* 页面大小 */
    uint32_t compression_alg;    /* 压缩算法 */
    uint32_t checksum_alg;       /* 校验算法 */
    uint32_t max_devices;        /* 最大设备数 */
    uint32_t journal_size_mb;    /* 日志大小(MB) */
    uint32_t metadata_size_mb;   /* 元数据大小(MB) */
    bool enable_compression;     /* 启用压缩 */
    bool enable_encryption;      /* 启用加密 */
    bool enable_priority;        /* 启用优先级 */
    bool enable_snapshot;        /* 启用快照 */
    uint8_t reserved[4];         /* 保留空间 */
} swap2_config_t;

/**
 * 函数声明
 */

/* 超级块操作 */
int swap2_read_superblock(swap2_superblock_t *super, void *device);
int swap2_write_superblock(swap2_superblock_t *super, void *device);
int swap2_verify_superblock(swap2_superblock_t *super);
int swap2_create_superblock(swap2_superblock_t *super, uint64_t total_pages,
                           uint32_t page_size, uint32_t compression_alg,
                           uint32_t checksum_alg);

/* 设备管理 */
int swap2_add_device(const char *device_name, uint64_t start_sector,
                    uint64_t sector_count, uint32_t priority);
int swap2_remove_device(const char *device_name);
int swap2_activate_device(const char *device_name);
int swap2_deactivate_device(const char *device_name);
swap2_device_t *swap2_get_device_info(const char *device_name);

/* 页面交换操作 */
int swap2_swap_out(uint64_t virtual_address, uint32_t process_id,
                  void *page_data, uint32_t page_size, uint32_t priority);
int swap2_swap_in(uint64_t virtual_address, uint32_t process_id,
                 void *page_buffer, uint32_t buffer_size);
int swap2_swap_free(uint64_t virtual_address, uint32_t process_id);

/* 压缩和校验 */
int swap2_compress_page(void *input, uint32_t input_size,
                       void *output, uint32_t *output_size, uint32_t algorithm);
int swap2_decompress_page(void *input, uint32_t input_size,
                         void *output, uint32_t *output_size, uint32_t algorithm);
uint32_t swap2_calculate_checksum(void *data, uint32_t size, uint32_t algorithm);

/* 快照操作 */
int swap2_create_snapshot(const char *name);
int swap2_restore_snapshot(const char *name);
int swap2_list_snapshots(char *names[], int *count);
int swap2_delete_snapshot(const char *name);

/* 休眠支持 */
int swap2_prepare_hibernate(void);
int swap2_enter_hibernate(void);
int swap2_resume_hibernate(void);

/* 统计和监控 */
int swap2_get_stats(swap2_stats_t *stats);
int swap2_reset_stats(void);
int swap2_dump_stats(void);

/* 配置管理 */
int swap2_set_config(swap2_config_t *config);
int swap2_get_config(swap2_config_t *config);
int swap2_save_config(const char *config_file);
int swap2_load_config(const char *config_file);

/* 调试和诊断 */
void swap2_dump_superblock(swap2_superblock_t *super);
void swap2_dump_page_header(swap2_page_header_t *header);
void swap2_dump_metadata(swap2_metadata_t *metadata);
void swap2_dump_device_info(swap2_device_t *device);

/* 工具函数 */
uint64_t swap2_align_pages(uint64_t size);
uint32_t swap2_get_page_count(uint64_t size);
uint64_t swap2_get_swap_size(uint32_t pages);
void swap2_uuid_generate(uint8_t *uuid);
uint64_t swap2_time_current(void);

#endif /* _SWAP2_H */
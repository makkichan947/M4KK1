/*
 * M4KK1 Ptdsk分区工具 - 主接口头文件
 * 定义分区工具的核心数据结构和接口
 */

#ifndef __PTDSK_H__
#define __PTDSK_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>

/* 版本信息 */
#define PTDSK_VERSION_MAJOR 2
#define PTDSK_VERSION_MINOR 0
#define PTDSK_VERSION_PATCH 0
#define PTDSK_VERSION_STRING "2.0.0"

/* 常量定义 */
#define MAX_DEVICES         64
#define MAX_PARTITIONS      128
#define MAX_FILESYSTEMS     32
#define MAX_BACKUPS         100
#define SECTOR_SIZE         512
#define SECTOR_SIZE_4K      4096

/* 设备类型枚举 */
typedef enum {
    DEVICE_HDD = 0,         /* 机械硬盘 */
    DEVICE_SSD = 1,         /* 固态硬盘 */
    DEVICE_NVME = 2,        /* NVMe设备 */
    DEVICE_USB = 3,         /* USB设备 */
    DEVICE_CDROM = 4,       /* 光驱 */
    DEVICE_RAID = 5,        /* RAID设备 */
    DEVICE_LVM = 6,         /* LVM设备 */
    DEVICE_LOOP = 7,        /* 循环设备 */
    DEVICE_UNKNOWN = 8      /* 未知设备 */
} device_type_t;

/* 接口类型枚举 */
typedef enum {
    INTERFACE_ATA = 0,      /* ATA接口 */
    INTERFACE_SATA = 1,     /* SATA接口 */
    INTERFACE_SCSI = 2,     /* SCSI接口 */
    INTERFACE_USB = 3,      /* USB接口 */
    INTERFACE_NVME = 4,     /* NVMe接口 */
    INTERFACE_FC = 5,       /* 光纤通道 */
    INTERFACE_SAS = 6,      /* SAS接口 */
    INTERFACE_UNKNOWN = 7   /* 未知接口 */
} interface_type_t;

/* 分区表类型枚举 */
typedef enum {
    PARTITION_TABLE_NONE = 0,   /* 无分区表 */
    PARTITION_TABLE_MBR = 1,    /* MBR分区表 */
    PARTITION_TABLE_GPT = 2,    /* GPT分区表 */
    PARTITION_TABLE_BSD = 3,    /* BSD分区表 */
    PARTITION_TABLE_SUN = 4,    /* Sun分区表 */
    PARTITION_TABLE_UNKNOWN = 5 /* 未知分区表 */
} partition_table_type_t;

/* 文件系统类型枚举 */
typedef enum {
    FS_NONE = 0,            /* 无文件系统 */
    FS_EXT2 = 1,            /* ext2 */
    FS_EXT3 = 2,            /* ext3 */
    FS_EXT4 = 3,            /* ext4 */
    FS_XFS = 4,             /* XFS */
    FS_BTRFS = 5,           /* Btrfs */
    FS_NTFS = 6,            /* NTFS */
    FS_FAT16 = 7,           /* FAT16 */
    FS_FAT32 = 8,           /* FAT32 */
    FS_EXFAT = 9,           /* exFAT */
    FS_HFS_PLUS = 10,       /* HFS+ */
    FS_APFS = 11,           /* APFS */
    FS_UDF = 12,            /* UDF */
    FS_ISO9660 = 13,        /* ISO9660 */
    FS_SWAP = 14,           /* 交换分区 */
    FS_LVM = 15,            /* LVM */
    FS_RAID = 16,           /* RAID */
    FS_UNKNOWN = 17         /* 未知文件系统 */
} filesystem_type_t;

/* 健康状态枚举 */
typedef enum {
    HEALTH_GOOD = 0,        /* 良好 */
    HEALTH_WARNING = 1,     /* 警告 */
    HEALTH_CRITICAL = 2,    /* 严重 */
    HEALTH_FAILED = 3,      /* 故障 */
    HEALTH_UNKNOWN = 4      /* 未知 */
} health_status_t;

/* 操作类型枚举 */
typedef enum {
    OP_NONE = 0,            /* 无操作 */
    OP_CREATE = 1,          /* 创建分区 */
    OP_DELETE = 2,          /* 删除分区 */
    OP_RESIZE = 3,          /* 调整大小 */
    OP_MOVE = 4,            /* 移动分区 */
    OP_FORMAT = 5,          /* 格式化 */
    OP_CHECK = 6,           /* 检查 */
    OP_REPAIR = 7,          /* 修复 */
    OP_BACKUP = 8,          /* 备份 */
    OP_RESTORE = 9,         /* 还原 */
    OP_SCAN = 10,           /* 扫描 */
    OP_ERASE = 11           /* 擦除 */
} operation_type_t;

/* 设备信息结构 */
typedef struct device {
    char *path;             /* 设备路径 (/dev/sda) */
    char *name;             /* 设备名称 */
    char *model;            /* 设备型号 */
    char *serial;           /* 序列号 */
    char *firmware;         /* 固件版本 */

    device_type_t type;     /* 设备类型 */
    interface_type_t interface; /* 接口类型 */

    uint64_t size;          /* 设备大小 (字节) */
    uint64_t sector_count;  /* 扇区数量 */
    uint32_t sector_size;   /* 扇区大小 */

    partition_table_type_t table_type; /* 分区表类型 */
    struct partition *partitions; /* 分区列表 */
    size_t partition_count; /* 分区数量 */

    health_status_t health; /* 健康状态 */
    double temperature;     /* 温度 */
    uint32_t rotation_speed; /* 转速 (HDD) */

    bool is_removable;      /* 是否可移动 */
    bool is_readonly;       /* 是否只读 */
    bool is_mounted;        /* 是否已挂载 */

    time_t last_access;     /* 最后访问时间 */
    void *userdata;         /* 用户数据 */
} device_t;

/* 分区信息结构 */
typedef struct partition {
    device_t *device;       /* 所属设备 */
    int number;             /* 分区号 */
    char *name;             /* 分区名称 */

    uint64_t start_sector;  /* 起始扇区 */
    uint64_t end_sector;    /* 结束扇区 */
    uint64_t sector_count;  /* 扇区数量 */
    uint64_t size;          /* 分区大小 */

    filesystem_type_t fs_type; /* 文件系统类型 */
    char *fs_label;         /* 文件系统标签 */
    char *fs_uuid;          /* 文件系统UUID */

    bool is_bootable;       /* 是否可引导 */
    bool is_active;         /* 是否活动 */
    bool is_hidden;         /* 是否隐藏 */
    bool is_mounted;        /* 是否已挂载 */
    char *mount_point;      /* 挂载点 */

    uint64_t used_space;    /* 已用空间 */
    uint64_t free_space;    /* 空闲空间 */

    struct partition *next; /* 下一个分区 */
    void *userdata;         /* 用户数据 */
} partition_t;

/* 文件系统支持结构 */
typedef struct filesystem_support {
    filesystem_type_t type; /* 文件系统类型 */
    char *name;             /* 名称 */
    char *description;      /* 描述 */

    bool can_create;        /* 是否支持创建 */
    bool can_resize;        /* 是否支持调整大小 */
    bool can_check;         /* 是否支持检查 */
    bool can_repair;        /* 是否支持修复 */

    char **create_options;  /* 创建选项 */
    char **mount_options;   /* 挂载选项 */

    struct filesystem_support *next; /* 下一个支持 */
} filesystem_support_t;

/* 操作进度结构 */
typedef struct operation_progress {
    operation_type_t type;  /* 操作类型 */
    char *description;      /* 操作描述 */

    uint64_t current;       /* 当前进度 */
    uint64_t total;         /* 总进度 */
    uint32_t percentage;    /* 百分比 */

    time_t start_time;      /* 开始时间 */
    time_t estimated_end;   /* 预计结束时间 */

    bool can_cancel;        /* 是否可取消 */
    bool cancelled;         /* 是否已取消 */

    char *status_message;   /* 状态消息 */
    char *error_message;    /* 错误消息 */
} operation_progress_t;

/* 应用状态结构 */
typedef struct app_state {
    device_t **devices;     /* 设备列表 */
    size_t device_count;    /* 设备数量 */

    device_t *current_device; /* 当前设备 */
    partition_t *current_partition; /* 当前分区 */

    filesystem_support_t *fs_support; /* 文件系统支持 */

    operation_progress_t *current_op; /* 当前操作 */

    char *config_file;      /* 配置文件路径 */
    char *backup_dir;       /* 备份目录 */

    bool readonly_mode;     /* 只读模式 */
    bool expert_mode;       /* 专家模式 */

    void *ui_context;       /* UI上下文 */
    void *plugin_context;   /* 插件上下文 */
} app_state_t;

/* 函数声明 */

/* 应用初始化和清理 */
app_state_t *ptdsk_create(void);
void ptdsk_destroy(app_state_t *app);
bool ptdsk_init(app_state_t *app);
void ptdsk_run(app_state_t *app);
void ptdsk_quit(app_state_t *app);

/* 设备管理 */
device_t **device_scan(void);
device_t *device_open(const char *path);
void device_close(device_t *device);
bool device_refresh_info(device_t *device);
bool device_read_sectors(device_t *device, uint64_t sector, uint32_t count, void *buffer);
bool device_write_sectors(device_t *device, uint64_t sector, uint32_t count, const void *buffer);

/* 分区表操作 */
partition_table_type_t partition_table_detect(const char *device);
bool partition_table_read(const char *device, partition_t **partitions, size_t *count);
bool partition_table_write(const char *device, partition_t *partitions, size_t count);
bool partition_table_backup(const char *device, const char *backup_path);
bool partition_table_restore(const char *device, const char *backup_path);

/* 分区操作 */
partition_t *partition_create(device_t *device, uint64_t start, uint64_t size, filesystem_type_t fs_type);
bool partition_delete(partition_t *partition);
bool partition_resize(partition_t *partition, uint64_t new_size);
bool partition_move(partition_t *partition, uint64_t new_start);
bool partition_format(partition_t *partition, filesystem_type_t fs_type, const char *label);
bool partition_check(partition_t *partition);
bool partition_repair(partition_t *partition);

/* 文件系统操作 */
filesystem_support_t *filesystem_get_support(filesystem_type_t type);
bool filesystem_create(const char *device, filesystem_type_t type, const char *label, char **options);
bool filesystem_resize(const char *device, uint64_t new_size);
bool filesystem_check(const char *device);
bool filesystem_repair(const char *device);

/* 监控和诊断 */
health_status_t device_assess_health(device_t *device);
bool device_monitor_start(device_t *device);
void device_monitor_stop(device_t *device);
bool device_smart_read(device_t *device, void *smart_data);
bool device_temperature_read(device_t *device, double *temperature);

/* 坏道检测 */
typedef enum {
    SCAN_TYPE_QUICK = 0,    /* 快速扫描 */
    SCAN_TYPE_FULL = 1,     /* 完整扫描 */
    SCAN_TYPE_READONLY = 2, /* 只读扫描 */
    SCAN_TYPE_DESTRUCTIVE = 3 /* 破坏性扫描 */
} scan_type_t;

bool badblock_scan_start(device_t *device, scan_type_t type, operation_progress_t *progress);
bool badblock_scan_cancel(operation_progress_t *progress);
uint64_t *badblock_scan_get_results(operation_progress_t *progress, size_t *count);
bool badblock_repair(device_t *device, uint64_t sector);

/* 备份和还原 */
bool backup_create(const char *device, const char *backup_path, const char *description);
bool backup_restore(const char *backup_path, const char *device, bool force);
bool backup_list(const char *backup_dir, char ***backups, size_t *count);
void backup_free_list(char **backups, size_t count);

/* 配置管理 */
bool config_load(app_state_t *app, const char *filename);
bool config_save(app_state_t *app, const char *filename);
void config_set_default(app_state_t *app);

/* 工具函数 */
const char *device_type_to_string(device_type_t type);
const char *filesystem_type_to_string(filesystem_type_t type);
const char *health_status_to_string(health_status_t status);
const char *operation_type_to_string(operation_type_t type);
uint64_t sector_to_bytes(uint64_t sectors, uint32_t sector_size);
uint64_t bytes_to_sector(uint64_t bytes, uint32_t sector_size);

/* 错误处理 */
typedef enum {
    PTDSK_OK = 0,                    /* 成功 */
    PTDSK_ERROR_PERMISSION = 1,      /* 权限错误 */
    PTDSK_ERROR_DEVICE = 2,          /* 设备错误 */
    PTDSK_ERROR_IO = 3,              /* I/O错误 */
    PTDSK_ERROR_MEMORY = 4,          /* 内存错误 */
    PTDSK_ERROR_INVALID_ARG = 5,     /* 无效参数 */
    PTDSK_ERROR_NOT_SUPPORTED = 6,   /* 不支持的操作 */
    PTDSK_ERROR_CANCELLED = 7,       /* 操作已取消 */
    PTDSK_ERROR_IN_PROGRESS = 8,     /* 操作进行中 */
    PTDSK_ERROR_NO_SPACE = 9,        /* 空间不足 */
    PTDSK_ERROR_CORRUPTION = 10      /* 数据损坏 */
} ptdsk_error_t;

const char *ptdsk_error_to_string(ptdsk_error_t error);

#endif /* __PTDSK_H__ */
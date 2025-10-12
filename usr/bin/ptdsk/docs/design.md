# M4KK1 Ptdsk分区工具设计文档

## 概述

M4KK1 Ptdsk是一个先进的图形化分区工具，超越了传统的cfdisk功能，不仅支持GPT和MBR分区表的创建、编辑和管理，还提供了硬盘监控、坏道检测、健康评估等高级功能。

## 设计目标

- **功能强大**：支持GPT/MBR分区表操作，比cfdisk更强大
- **界面友好**：直观的图形化界面，易于操作
- **监控全面**：实时硬盘状态监控和健康评估
- **诊断准确**：先进的坏道检测和修复功能
- **安全可靠**：操作确认、备份还原、错误恢复

## 核心特性

### 分区管理

- **分区表类型**：支持GPT和MBR分区表
- **分区操作**：创建、删除、调整大小、移动分区
- **文件系统支持**：ext4、xfs、btrfs、ntfs、fat32等
- **引导管理**：引导标志设置、引导加载器安装
- **分区标签**：自定义分区名称和UUID

### 硬盘监控

- **实时状态**：温度、转速、健康状态监控
- **性能指标**：读写速度、I/O延迟、吞吐量
- **SMART信息**：SMART属性监控和分析
- **错误日志**：I/O错误记录和分析
- **预警系统**：故障预测和预警通知

### 坏道检测

- **表面扫描**：全面的磁盘表面扫描
- **坏道定位**：精确的坏道位置定位
- **坏道标记**：自动标记和隔离坏道
- **修复尝试**：坏道修复和恢复尝试
- **报告生成**：详细的坏道检测报告

### 数据安全

- **备份还原**：分区表备份和还原功能
- **操作确认**：危险操作二次确认
- **撤销功能**：操作历史记录和撤销
- **安全擦除**：安全的数据擦除功能
- **加密支持**：分区加密和解密

## 架构设计

### 系统架构图

```
┌─────────────────────────────────────────────────────────────┐
│                    Ptdsk分区工具                           │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │   主界面    │  │   分区编辑器 │  │   硬盘监控  │           │
│  │  (Main UI)   │  │  (Partition) │  │  (Monitor)  │           │
│  └─────────────┘  └─────────────┘  └─────────────┘           │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │  分区引擎    │  │   文件系统   │  │   设备管理   │           │
│  │  (Engine)   │  │  (FileSys)  │  │  (Device)   │           │
│  └─────────────┘  └─────────────┘  └─────────────┘           │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │  坏道检测    │  │   数据安全   │  │   配置管理   │           │
│  │  (BadBlock)  │  │  (Security)  │  │  (Config)   │           │
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
│   界面管理器     │    │   分区管理器     │
│  (ui_manager)   │◄──►│ (partition_mgr) │
└─────────────────┘    └─────────────────┘
         │                       │
         ▼                       ▼
┌─────────────────┐    ┌─────────────────┐
│   设备管理器     │    │   文件系统工具   │
│  (device_mgr)   │◄──►│ (fs_tools)      │
└─────────────────┘    └─────────────────┘
         │                       │
         ▼                       ▼
┌─────────────────┐    ┌─────────────────┐
│   监控引擎       │    │   坏道检测器     │
│  (monitor_eng)  │◄──►│ (badblock_det)  │
└─────────────────┘    └─────────────────┘
```

## 数据结构设计

### 分区表结构

```c
// 分区类型枚举
typedef enum {
    PARTITION_PRIMARY = 0,      /* 主分区 */
    PARTITION_EXTENDED = 1,     /* 扩展分区 */
    PARTITION_LOGICAL = 2,      /* 逻辑分区 */
    PARTITION_GPT = 3           /* GPT分区 */
} partition_type_t;

// 分区状态枚举
typedef enum {
    PARTITION_ACTIVE = 0,       /* 活动分区 */
    PARTITION_INACTIVE = 1,     /* 非活动分区 */
    PARTITION_HIDDEN = 2,       /* 隐藏分区 */
    PARTITION_DORMANT = 3       /* 休眠分区 */
} partition_state_t;

// 分区信息结构
typedef struct {
    char *device;               /* 设备路径 */
    int number;                 /* 分区号 */
    partition_type_t type;      /* 分区类型 */
    partition_state_t state;    /* 分区状态 */

    uint64_t start_sector;      /* 起始扇区 */
    uint64_t end_sector;        /* 结束扇区 */
    uint64_t total_sectors;     /* 总扇区数 */

    char *filesystem;           /* 文件系统类型 */
    char *label;                /* 分区标签 */
    char *uuid;                 /* UUID */
    char *guid;                 /* GUID (GPT) */

    bool bootable;              /* 可引导标志 */
    bool encrypted;             /* 加密标志 */
    bool compressed;            /* 压缩标志 */

    uint64_t used_space;        /* 已用空间 */
    uint64_t free_space;        /* 空闲空间 */
    uint64_t total_space;       /* 总空间 */

    struct partition *next;     /* 下一个分区 */
} partition_t;

// 磁盘信息结构
typedef struct {
    char *device;               /* 设备路径 */
    char *model;                /* 型号 */
    char *serial;               /* 序列号 */
    char *firmware;             /* 固件版本 */

    uint64_t total_sectors;     /* 总扇区数 */
    uint32_t sector_size;       /* 扇区大小 */
    uint64_t total_size;        /* 总容量 */

    partition_t *partitions;    /* 分区列表 */
    size_t partition_count;     /* 分区数量 */

    disk_type_t type;           /* 磁盘类型 */
    interface_t interface;      /* 接口类型 */

    smart_info_t *smart;        /* SMART信息 */
    health_status_t health;     /* 健康状态 */
} disk_t;
```

### 监控数据结构

```c
// 监控指标结构
typedef struct {
    time_t timestamp;           /* 时间戳 */
    double temperature;         /* 温度 */
    uint32_t rotation_speed;    /* 转速 */
    uint64_t power_cycles;      /* 通电次数 */
    uint64_t power_on_hours;    /* 通电小时数 */

    io_stats_t io_stats;        /* I/O统计 */
    error_stats_t error_stats;  /* 错误统计 */

    struct monitor_data *next;  /* 下一个数据点 */
} monitor_data_t;

// SMART属性结构
typedef struct {
    uint8_t id;                 /* 属性ID */
    char *name;                 /* 属性名称 */
    uint16_t flags;             /* 标志 */
    uint8_t value;              /* 当前值 */
    uint8_t worst;              /* 最坏值 */
    uint8_t threshold;          /* 阈值 */
    uint64_t raw_value;         /* 原始值 */

    struct smart_attr *next;    /* 下一个属性 */
} smart_attr_t;

// 健康评估结构
typedef struct {
    health_status_t overall;    /* 总体健康状态 */
    uint8_t score;              /* 健康评分 (0-100) */

    char *issues[32];           /* 问题列表 */
    size_t issue_count;         /* 问题数量 */

    time_t last_check;          /* 最后检查时间 */
    time_t next_check;          /* 下次检查时间 */

    char *recommendations[16];   /* 建议列表 */
    size_t recommendation_count; /* 建议数量 */
} health_assessment_t;
```

## 用户界面设计

### 主界面布局

```
┌─────────────────────────────────────────────────────────────┐
│ Ptdsk: Advanced Disk Partitioning Tool v2.0                 │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────┬─────────────────┬─────────────────────┐ │
│  │    设备列表       │    分区列表       │      属性面板        │ │
│  │   (Devices)     │   (Partitions)   │     (Properties)    │ │
│  │                 │                 │                     │ │
│  │  ┌─────────────┐ │  ┌─────────────┐ │  ┌─────────────────┐ │ │
│  │  │ /dev/sda    │ │  │ /dev/sda1   │ │  │ Device: /dev/sda│ │ │
│  │  │ /dev/sdb    │ │  │ /dev/sda2   │ │  │ Model: ST1000DM │ │ │
│  │  │ /dev/nvme0n1│ │  │ /dev/sda3   │ │  │ Serial: ZA1ABC  │ │ │
│  │  └─────────────┘ │  │ /dev/sda4   │ │  │ Size: 1TB       │ │ │
│  │                 │  │ ...         │ │  │ Health: Good    │ │ │
│  │  [Refresh]      │  │ [New] [Del] │ │  │                 │ │ │
│  │                 │  └─────────────┘ │  └─────────────────┘ │ │
│  └─────────────────┴─────────────────┴─────────────────────┘ │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ Status: Ready | Operation: None | Progress: 0% | Monitor │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### 菜单结构

```
File (F)
├── New Partition Table (Ctrl+N)
├── Open Disk Image (Ctrl+O)
├── Save Partition Table (Ctrl+S)
├── Export Report (Ctrl+E)
├── Print (Ctrl+P)
└── Exit (Ctrl+Q)

Edit (E)
├── Undo (Ctrl+Z)
├── Redo (Ctrl+Y)
├── Copy Partition (Ctrl+C)
├── Paste Partition (Ctrl+V)
├── Resize/Move (Ctrl+R)
└── Properties (Alt+Enter)

View (V)
├── Refresh (F5)
├── Zoom In (Ctrl++)
├── Zoom Out (Ctrl+-)
├── Fit to Window (Ctrl+0)
└── Show Toolbar (Ctrl+T)

Tools (T)
├── Check Disk (F6)
├── Repair Disk (F7)
├── Surface Scan (F8)
├── SMART Monitor (F9)
├── Backup Table (F10)
└── Restore Table (F11)

Help (H)
├── Contents (F1)
├── About (F12)
└── License
```

### 键盘快捷键

```c
// 通用快捷键
Ctrl+Q          /* 退出 */
Ctrl+N          /* 新建分区表 */
Ctrl+O          /* 打开磁盘映像 */
Ctrl+S          /* 保存分区表 */
F5              /* 刷新 */
F1              /* 帮助 */

// 分区操作
Ctrl+C          /* 复制分区 */
Ctrl+V          /* 粘贴分区 */
Ctrl+R          /* 调整大小/移动 */
Delete          /* 删除分区 */
Insert          /* 新建分区 */

// 导航快捷键
Tab             /* 切换面板 */
Arrow Keys      /* 导航 */
Page Up/Down    /* 翻页 */
Home/End        /* 首尾 */

// 监控快捷键
F6              /* 磁盘检查 */
F7              /* 磁盘修复 */
F8              /* 表面扫描 */
F9              /* SMART监控 */
F10             /* 备份表 */
F11             /* 还原表 */
```

## 分区引擎设计

### GPT分区表处理

```c
// GPT头结构
typedef struct {
    uint8_t signature[8];       /* GPT签名 */
    uint32_t revision;          /* 版本 */
    uint32_t header_size;       /* 头大小 */
    uint32_t crc32;             /* CRC32校验 */
    uint32_t reserved;          /* 保留 */
    uint64_t current_lba;       /* 当前LBA */
    uint64_t backup_lba;        /* 备份LBA */
    uint64_t first_usable;      /* 第一个可用LBA */
    uint64_t last_usable;       /* 最后一个可用LBA */
    uint8_t disk_guid[16];      /* 磁盘GUID */
    uint64_t partition_start;   /* 分区表起始LBA */
    uint32_t partition_count;   /* 分区数量 */
    uint32_t partition_size;    /* 分区条目大小 */
    uint32_t partition_crc;     /* 分区表CRC */
} gpt_header_t;

// GPT分区条目
typedef struct {
    uint8_t type_guid[16];      /* 分区类型GUID */
    uint8_t partition_guid[16]; /* 分区GUID */
    uint64_t start_lba;         /* 起始LBA */
    uint64_t end_lba;           /* 结束LBA */
    uint64_t attributes;        /* 属性 */
    uint16_t name[36];          /* 分区名称 (UTF-16) */
} gpt_entry_t;
```

### MBR分区表处理

```c
// MBR结构
typedef struct {
    uint8_t bootstrap[440];     /* 引导代码 */
    uint32_t disk_signature;    /* 磁盘签名 */
    uint16_t reserved;          /* 保留 */
    mbr_partition_t partitions[4]; /* 分区表 */
    uint16_t boot_signature;    /* 引导签名 */
} mbr_t;

// MBR分区条目
typedef struct {
    uint8_t boot_flag;          /* 引导标志 */
    uint8_t start_chs[3];       /* 起始CHS地址 */
    uint8_t type;               /* 分区类型 */
    uint8_t end_chs[3];         /* 结束CHS地址 */
    uint32_t start_lba;         /* 起始LBA */
    uint32_t sector_count;      /* 扇区数量 */
} mbr_partition_t;
```

## 监控系统设计

### SMART监控

```c
// SMART命令结构
typedef struct {
    uint8_t command;            /* SMART命令 */
    uint8_t feature;            /* 特性寄存器 */
    uint8_t sector_count;       /* 扇区数 */
    uint64_t lba;               /* LBA地址 */
    void *data;                 /* 数据缓冲区 */
    size_t data_size;           /* 数据大小 */
} smart_command_t;

// SMART属性监控
typedef struct {
    smart_attr_t **attributes;  /* 属性列表 */
    size_t count;               /* 属性数量 */
    time_t last_update;         /* 最后更新时间 */
    bool enabled;               /* 是否启用 */

    /* 阈值设置 */
    uint8_t warning_threshold;  /* 警告阈值 */
    uint8_t critical_threshold; /* 严重阈值 */

    /* 历史数据 */
    smart_history_t *history;   /* 历史记录 */
} smart_monitor_t;
```

### 性能监控

```c
// I/O统计结构
typedef struct {
    uint64_t read_count;        /* 读操作次数 */
    uint64_t write_count;       /* 写操作次数 */
    uint64_t read_sectors;      /* 读扇区数 */
    uint64_t write_sectors;     /* 写扇区数 */
    uint64_t read_time;         /* 读时间 (ms) */
    uint64_t write_time;        /* 写时间 (ms) */

    double read_speed;          /* 读速度 (MB/s) */
    double write_speed;         /* 写速度 (MB/s) */
    double avg_latency;         /* 平均延迟 (ms) */
    double max_latency;         /* 最大延迟 (ms) */
} io_stats_t;

// 温度监控结构
typedef struct {
    double current;             /* 当前温度 */
    double minimum;             /* 最低温度 */
    double maximum;             /* 最高温度 */
    double average;             /* 平均温度 */

    uint32_t overheat_count;    /* 过热次数 */
    time_t last_overheat;       /* 最后过热时间 */

    double warning_temp;        /* 警告温度 */
    double critical_temp;       /* 严重温度 */
} temperature_monitor_t;
```

## 坏道检测系统

### 扫描算法

```c
// 扫描类型枚举
typedef enum {
    SCAN_QUICK = 0,             /* 快速扫描 */
    SCAN_NORMAL = 1,            /* 普通扫描 */
    SCAN_THOROUGH = 2,          /* 彻底扫描 */
    SCAN_BUTTERFLY = 3,         /* 蝶形扫描 */
    SCAN_RANDOM = 4             /* 随机扫描 */
} scan_type_t;

// 坏道信息结构
typedef struct {
    uint64_t sector;            /* 扇区号 */
    bad_block_type_t type;      /* 坏道类型 */
    uint32_t error_count;       /* 错误次数 */
    time_t first_detected;      /* 首次检测时间 */
    time_t last_detected;       /* 最后检测时间 */

    bool remapped;              /* 是否已重映射 */
    bool repaired;              /* 是否已修复 */

    struct bad_block *next;     /* 下一个坏道 */
} bad_block_t;

// 扫描进度结构
typedef struct {
    uint64_t current_sector;    /* 当前扇区 */
    uint64_t total_sectors;     /* 总扇区数 */
    uint32_t percent;           /* 百分比 */

    bad_block_t *bad_blocks;    /* 坏道列表 */
    size_t bad_block_count;     /* 坏道数量 */

    time_t start_time;          /* 开始时间 */
    time_t estimated_end;       /* 预计结束时间 */

    scan_status_t status;       /* 扫描状态 */
    char *current_operation;    /* 当前操作 */
} scan_progress_t;
```

## 安全系统设计

### 备份系统

```c
// 备份类型枚举
typedef enum {
    BACKUP_FULL = 0,            /* 完整备份 */
    BACKUP_INCREMENTAL = 1,     /* 增量备份 */
    BACKUP_DIFFERENTIAL = 2      /* 差分备份 */
} backup_type_t;

// 备份信息结构
typedef struct {
    char *backup_id;            /* 备份ID */
    backup_type_t type;         /* 备份类型 */
    char *device;               /* 设备路径 */
    char *backup_path;          /* 备份路径 */

    time_t created;             /* 创建时间 */
    uint64_t size;              /* 备份大小 */
    char *checksum;             /* 校验和 */

    char *description;          /* 描述 */
    char *version;              /* 版本 */

    struct backup_info *next;   /* 下一个备份 */
} backup_info_t;

// 还原操作结构
typedef struct {
    backup_info_t *backup;      /* 备份信息 */
    char *target_device;        /* 目标设备 */
    bool verify_only;           /* 仅验证 */
    bool force_restore;         /* 强制还原 */

    restore_progress_t progress; /* 还原进度 */
    char *log_file;             /* 日志文件 */
} restore_operation_t;
```

## 测试策略

### 单元测试

- 分区表读写测试
- 文件系统检测测试
- 坏道扫描算法测试
- SMART数据解析测试

### 集成测试

- 完整分区操作流程测试
- 多磁盘环境测试
- 错误处理和恢复测试
- 性能压力测试

### 用户体验测试

- 界面操作便捷性测试
- 错误提示友好性测试
- 操作确认机制测试
- 帮助系统完整性测试

## 部署和集成

### 安装配置

1. **编译安装**：从源码编译安装
2. **权限设置**：设置必要的设备访问权限
3. **桌面集成**：集成到桌面环境菜单
4. **图标和快捷方式**：创建桌面图标和快捷方式

### 系统集成

- **udev规则**：自动检测新设备
- **系统服务**：后台监控服务
- **日志集成**：集成系统日志
- **通知系统**：桌面通知支持

## 未来规划

### 短期目标（1-3个月）

- 完成核心分区功能
- 实现基本监控功能
- 添加坏道检测功能
- 完善用户界面

### 中期目标（3-6个月）

- 高级文件系统支持
- RAID管理功能
- LVM支持
- 远程管理功能

### 长期目标（6个月以上）

- 云存储支持
- 容器存储管理
- 分布式存储支持
- AI辅助诊断

## 总结

M4KK1 Ptdsk分区工具是一个功能强大、界面友好的磁盘管理工具，不仅提供了传统分区工具的所有功能，还增加了硬盘监控、坏道检测、健康评估等高级特性。通过模块化的设计和可扩展的架构，Ptdsk将成为系统管理员和普通用户进行磁盘管理的首选工具，为M4KK1操作系统的存储管理提供了坚实的基础。
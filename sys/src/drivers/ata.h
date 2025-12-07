/*
 * M4KK1 ATA/IDE驱动 - ATA设备管理接口
 * 支持ATA/ATAPI硬盘和光驱设备
 */

#ifndef __ATA_H__
#define __ATA_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ATA设备信息结构 */
typedef struct {
    uint16_t signature;           /* 设备签名 */
    uint16_t cylinders;           /* 柱面数 */
    uint16_t heads;               /* 磁头数 */
    uint16_t sectors;             /* 每磁道扇区数 */
    uint32_t size;                /* 总扇区数（LBA28） */
    uint64_t size_lba48;          /* 总扇区数（LBA48） */
    bool lba48_supported;         /* 是否支持LBA48 */
    char model[41];               /* 设备型号（字符串） */
} ata_device_info_t;

/* ATA驱动接口结构 */
typedef struct {
    char *name;                     /* 驱动名称 */
    char *description;              /* 驱动描述 */

    /* 初始化和清理 */
    int (*init)(void);              /* 初始化驱动 */
    void (*cleanup)(void);          /* 清理驱动 */

    /* 设备管理 */
    int (*identify)(int device);    /* 识别设备 */
    bool (*read)(int device, uint32_t lba, uint8_t count, void *buffer);
    bool (*write)(int device, uint32_t lba, uint8_t count, const void *buffer);

    /* 设备信息 */
    ata_device_info_t* (*get_info)(int device); /* 获取设备信息 */

    /* 私有数据 */
    void *priv_data;                /* 私有数据 */
} ata_driver_t;

/* 函数声明 */

/* ATA驱动管理 */
int ata_driver_register(ata_driver_t *driver);
int ata_driver_unregister(ata_driver_t *driver);
ata_driver_t *ata_driver_get(const char *name);

/* ATA设备枚举 */
void ata_init(void);

/* ATA设备操作 */
bool ata_read_sectors(uint8_t device, uint32_t lba, uint8_t count, void *buffer);
bool ata_write_sectors(uint8_t device, uint32_t lba, uint8_t count, const void *buffer);

/* ATA设备信息获取 */
ata_device_info_t* ata_get_device_info(uint8_t device);
bool ata_device_exists(uint8_t device);
uint8_t ata_get_device_count(void);
uint32_t ata_get_total_sectors(uint8_t device);

/* 状态查询 */
bool ata_is_initialized(void);

/* ATA设备类型检测 */
static inline bool ata_is_hard_disk(uint8_t device) {
    ata_device_info_t *info = ata_get_device_info(device);
    return (info != NULL);
}

static inline bool ata_is_cdrom(uint8_t device) {
    // 简单判断：如果设备存在但无法确定类型，暂定为硬盘
    return false;
}

#endif /* __ATA_H__ */
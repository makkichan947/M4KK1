/**
 * M4KK1 ATA/IDE驱动实现
 * 支持ATA/ATAPI硬盘和光驱设备
 */

#include "ata.h"
#include "../include/console.h"
#include "../include/kernel.h"
#include <stdint.h>
#include <stdbool.h>
#include "../include/string.h"

/* ATA寄存器端口定义 */
#define ATA_DATA_PORT           0x1F0
#define ATA_FEATURES_PORT       0x1F1
#define ATA_SECTOR_COUNT_PORT   0x1F2
#define ATA_LBA_LOW_PORT        0x1F3
#define ATA_LBA_MID_PORT        0x1F4
#define ATA_LBA_HIGH_PORT       0x1F5
#define ATA_DEVICE_PORT         0x1F6
#define ATA_STATUS_PORT         0x1F7
#define ATA_COMMAND_PORT        0x1F7

/* ATA备用状态端口 */
#define ATA_ALT_STATUS_PORT     0x3F6
#define ATA_CONTROL_PORT        0x3F6

/* ATA命令定义 */
#define ATA_CMD_READ_PIO        0x20
#define ATA_CMD_READ_PIO_EXT    0x24
#define ATA_CMD_READ_DMA        0xC8
#define ATA_CMD_READ_DMA_EXT    0x25
#define ATA_CMD_WRITE_PIO       0x30
#define ATA_CMD_WRITE_PIO_EXT   0x34
#define ATA_CMD_WRITE_DMA       0xCA
#define ATA_CMD_WRITE_DMA_EXT   0x35
#define ATA_CMD_CACHE_FLUSH     0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET          0xA0
#define ATA_CMD_IDENTIFY        0xEC
#define ATA_CMD_IDENTIFY_PACKET 0xA1

/* ATA状态位定义 */
#define ATA_STATUS_BSY          0x80    /* 忙碌 */
#define ATA_STATUS_DRDY         0x40    /* 就绪 */
#define ATA_STATUS_DRQ          0x08    /* 请求数据传输 */
#define ATA_STATUS_ERR          0x01    /* 错误 */

/* ATA设备选择位 */
#define ATA_DEVICE_MASTER       0x00
#define ATA_DEVICE_SLAVE        0x10

/* ATA驱动状态 */
typedef struct {
    bool initialized;
    bool devices[2];              /* 主从设备存在标志 */
    ata_device_info_t device_info[2];  /* 设备信息 */
} ata_state_t;

static ata_state_t ata_state;

/* I/O端口操作函数 */
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t value;
    __asm__ volatile ("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/* 等待ATA设备就绪 */
static bool ata_wait_ready(void) {
    uint32_t timeout = 1000000;  /* 防止无限等待 */
    
    while (timeout--) {
        uint8_t status = inb(ATA_STATUS_PORT);
        if (!(status & ATA_STATUS_BSY) && (status & ATA_STATUS_DRDY)) {
            return true;
        }
    }
    
    return false;
}

/* 等待数据请求 */
static bool ata_wait_drq(void) {
    uint32_t timeout = 1000000;  /* 防止无限等待 */
    
    while (timeout--) {
        uint8_t status = inb(ATA_STATUS_PORT);
        if (status & ATA_STATUS_DRQ) {
            return true;
        }
        if (status & ATA_STATUS_ERR) {
            return false;  /* 错误 */
        }
    }
    
    return false;
}

/* 选择ATA设备 */
static void ata_select_device(uint8_t device) {
    outb(ATA_DEVICE_PORT, 0xE0 | (device << 4));
    
    // 短延时
    inb(ATA_ALT_STATUS_PORT);
    inb(ATA_ALT_STATUS_PORT);
    inb(ATA_ALT_STATUS_PORT);
    inb(ATA_ALT_STATUS_PORT);
}

/* 发送ATA命令 */
static void ata_send_command(uint8_t command) {
    outb(ATA_COMMAND_PORT, command);
}

/* 识别ATA设备 */
static bool ata_identify_device(uint8_t device, ata_device_info_t *info) {
    // 选择设备
    ata_select_device(device);
    
    // 检查设备是否存在
    outb(ATA_SECTOR_COUNT_PORT, 0x00);
    outb(ATA_LBA_LOW_PORT, 0x00);
    outb(ATA_LBA_MID_PORT, 0x00);
    outb(ATA_LBA_HIGH_PORT, 0x00);
    
    // 发送识别命令
    ata_send_command(ATA_CMD_IDENTIFY);
    
    // 检查是否有设备响应
    uint8_t status = inb(ATA_STATUS_PORT);
    if (status == 0) {
        return false;  /* 没有设备 */
    }
    
    // 等待就绪
    if (!ata_wait_ready()) {
        return false;
    }
    
    // 等待DRQ或错误
    status = inb(ATA_STATUS_PORT);
    if (status & ATA_STATUS_ERR) {
        return false;  /* 错误 */
    }
    
    if (!(status & ATA_STATUS_DRQ)) {
        return false;  /* 没有数据请求 */
    }
    
    // 读取识别数据
    uint16_t buffer[256];
    for (int i = 0; i < 256; i++) {
        buffer[i] = inw(ATA_DATA_PORT);
    }
    
    // 解析识别数据
    info->signature = buffer[0];
    info->cylinders = buffer[1];
    info->heads = buffer[3];
    info->sectors = buffer[6];
    info->size = buffer[60] | ((uint32_t)buffer[61] << 16);
    
    // 检查是否支持LBA48
    if (buffer[83] & (1 << 10)) {
        info->lba48_supported = true;
        info->size_lba48 = buffer[100] | ((uint32_t)buffer[101] << 16) |
                          ((uint32_t)buffer[102] << 32) | ((uint32_t)buffer[103] << 48);
    } else {
        info->lba48_supported = false;
        info->size_lba48 = 0;
    }
    
    // 读取型号信息
    for (int i = 0; i < 20; i++) {
        uint16_t word = buffer[27 + i];
        info->model[i * 2] = (uint8_t)(word >> 8);
        info->model[i * 2 + 1] = (uint8_t)word;
    }
    info->model[40] = '\0';
    
    // 修正型号字符串（移除尾部空格）
    for (int i = 39; i >= 0; i--) {
        if (info->model[i] == ' ') {
            info->model[i] = '\0';
        } else {
            break;
        }
    }
    
    return true;
}

/* 读取扇区 */
bool ata_read_sectors(uint8_t device, uint32_t lba, uint8_t count, void *buffer) {
    if (device > 1 || !ata_state.devices[device]) {
        return false;
    }
    
    // 选择设备
    ata_select_device(device);
    
    if (!ata_wait_ready()) {
        return false;
    }
    
    // 设置扇区数和LBA
    outb(ATA_SECTOR_COUNT_PORT, count);
    outb(ATA_LBA_LOW_PORT, (uint8_t)lba);
    outb(ATA_LBA_MID_PORT, (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH_PORT, (uint8_t)(lba >> 16));
    
    // 设置设备位（LBA模式）
    uint8_t device_reg = 0xE0 | (device << 4) | ((lba >> 24) & 0x0F);
    outb(ATA_DEVICE_PORT, device_reg);
    
    // 发送读取命令
    ata_send_command(ATA_CMD_READ_PIO);
    
    uint8_t *buf = (uint8_t *)buffer;
    for (uint8_t sector = 0; sector < count; sector++) {
        if (!ata_wait_drq()) {
            return false;
        }
        
        // 读取一个扇区的数据（512字节）
        for (int i = 0; i < 256; i++) {
            uint16_t data = inw(ATA_DATA_PORT);
            *buf++ = (uint8_t)data;
            *buf++ = (uint8_t)(data >> 8);
        }
    }
    
    return true;
}

/* 写入扇区 */
bool ata_write_sectors(uint8_t device, uint32_t lba, uint8_t count, const void *buffer) {
    if (device > 1 || !ata_state.devices[device]) {
        return false;
    }
    
    // 选择设备
    ata_select_device(device);
    
    if (!ata_wait_ready()) {
        return false;
    }
    
    // 设置扇区数和LBA
    outb(ATA_SECTOR_COUNT_PORT, count);
    outb(ATA_LBA_LOW_PORT, (uint8_t)lba);
    outb(ATA_LBA_MID_PORT, (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH_PORT, (uint8_t)(lba >> 16));
    
    // 设置设备位（LBA模式）
    uint8_t device_reg = 0xE0 | (device << 4) | ((lba >> 24) & 0x0F);
    outb(ATA_DEVICE_PORT, device_reg);
    
    // 发送写入命令
    ata_send_command(ATA_CMD_WRITE_PIO);
    
    if (!ata_wait_drq()) {
        return false;
    }
    
    const uint8_t *buf = (const uint8_t *)buffer;
    for (uint8_t sector = 0; sector < count; sector++) {
        // 写入一个扇区的数据（512字节）
        for (int i = 0; i < 256; i++) {
            uint16_t data = *buf++;
            data |= (uint16_t)(*buf++) << 8;
            outw(ATA_DATA_PORT, data);
        }
        
        if (sector < count - 1) {
            if (!ata_wait_drq()) {
                return false;
            }
        }
    }
    
    // 刷新缓存
    ata_send_command(ATA_CMD_CACHE_FLUSH);
    ata_wait_ready();
    
    return true;
}

/* 初始化ATA驱动 */
void ata_init(void) {
    KLOG_INFO("Initializing ATA/IDE driver...");
    
    // 初始化ATA状态
    memset(&ata_state, 0, sizeof(ata_state));
    ata_state.initialized = true;
    
    // 检测主设备
    if (ata_identify_device(ATA_DEVICE_MASTER, &ata_state.device_info[0])) {
        ata_state.devices[0] = true;
        console_write("ATA Master device found: ");
        console_write(ata_state.device_info[0].model);
        console_write("\n");
    }
    
    // 检测从设备
    if (ata_identify_device(ATA_DEVICE_SLAVE, &ata_state.device_info[1])) {
        ata_state.devices[1] = true;
        console_write("ATA Slave device found: ");
        console_write(ata_state.device_info[1].model);
        console_write("\n");
    }
    
    KLOG_INFO("ATA/IDE driver initialized");
}

/* 获取ATA设备信息 */
ata_device_info_t* ata_get_device_info(uint8_t device) {
    if (device > 1 || !ata_state.devices[device]) {
        return NULL;
    }
    return &ata_state.device_info[device];
}

/* 检查ATA设备是否存在 */
bool ata_device_exists(uint8_t device) {
    if (device > 1) {
        return false;
    }
    return ata_state.devices[device];
}

/* 获取设备数量 */
uint8_t ata_get_device_count(void) {
    uint8_t count = 0;
    for (int i = 0; i < 2; i++) {
        if (ata_state.devices[i]) {
            count++;
        }
    }
    return count;
}

/* 获取总扇区数 */
uint32_t ata_get_total_sectors(uint8_t device) {
    if (device > 1 || !ata_state.devices[device]) {
        return 0;
    }
    
    if (ata_state.device_info[device].lba48_supported) {
        return (uint32_t)ata_state.device_info[device].size_lba48;
    } else {
        return ata_state.device_info[device].size;
    }
}

/* ATA驱动状态 */
bool ata_is_initialized(void) {
    return ata_state.initialized;
}
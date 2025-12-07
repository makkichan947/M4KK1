/**
 * M4KK1 PCI驱动实现
 * 支持PCI和PCIe设备枚举和管理
 */

#include "pci.h"
#include "../include/console.h"
#include "../include/kernel.h"
#include <stdint.h>
#include <stdbool.h>
#include "../include/string.h"

/* PCI配置空间寄存器 */
#define PCI_CONFIG_ADDRESS    0xCF8
#define PCI_CONFIG_DATA       0xCFC

/* PCI配置空间字段偏移 */
#define PCI_VENDOR_ID         0x00
#define PCI_DEVICE_ID         0x02
#define PCI_COMMAND           0x04
#define PCI_STATUS            0x06
#define PCI_REVISION_ID       0x08
#define PCI_PROG_IF           0x09
#define PCI_SUBCLASS          0x0A
#define PCI_CLASS             0x0B
#define PCI_HEADER_TYPE       0x0E
#define PCI_BAR0              0x10
#define PCI_BAR1              0x14
#define PCI_BAR2              0x18
#define PCI_BAR3              0x1C
#define PCI_BAR4              0x20
#define PCI_BAR5              0x24
#define PCI_INTERRUPT_LINE    0x3C
#define PCI_INTERRUPT_PIN     0x3D

/* PCI设备类型 */
#define PCI_CLASS_BRIDGE      0x06
#define PCI_SUBCLASS_HOST_BRIDGE  0x00
#define PCI_SUBCLASS_ISA_BRIDGE   0x01
#define PCI_SUBCLASS_PCI_BRIDGE   0x04

/* PCI总线、设备、功能掩码 */
#define PCI_BUS_MASK          0xFF000000
#define PCI_DEVICE_MASK       0x00FF0000
#define PCI_FUNCTION_MASK     0x0000F000
#define PCI_REG_MASK          0x00000FFC
#define PCI_ENABLE_BIT        0x80000000

/* PCI驱动状态 */
typedef struct {
    bool initialized;
    uint32_t device_count;
    pci_device_t devices[PCI_MAX_DEVICES];
} pci_state_t;

static pci_state_t pci_state;

/* I/O端口操作函数 */
static inline void outl(uint16_t port, uint32_t value) {
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t value;
    __asm__ volatile ("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/* 读取PCI配置空间 */
uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = (uint32_t)(1 << 31) | 
                      ((uint32_t)bus << 16) | 
                      ((uint32_t)device << 11) | 
                      ((uint32_t)function << 8) | 
                      (offset & 0xFC);
    
    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

/* 写入PCI配置空间 */
void pci_write_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
    uint32_t address = (uint32_t)(1 << 31) | 
                      ((uint32_t)bus << 16) | 
                      ((uint32_t)device << 11) | 
                      ((uint32_t)function << 8) | 
                      (offset & 0xFC);
    
    outl(PCI_CONFIG_ADDRESS, address);
    outl(PCI_CONFIG_DATA, value);
}

/* 读取PCI设备的特定字段 */
uint16_t pci_read_word(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t value = pci_read_config(bus, device, function, offset & ~3);
    if ((offset & 3) == 0) {
        return (uint16_t)(value & 0xFFFF);
    } else if ((offset & 3) == 2) {
        return (uint16_t)((value >> 16) & 0xFFFF);
    }
    return 0;
}

uint8_t pci_read_byte(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t value = pci_read_config(bus, device, function, offset & ~3);
    return (uint8_t)((value >> ((offset & 3) * 8)) & 0xFF);
}

/* 检查PCI设备是否存在 */
bool pci_device_exists(uint8_t bus, uint8_t device, uint8_t function) {
    uint16_t vendor_id = pci_read_word(bus, device, function, PCI_VENDOR_ID);
    return vendor_id != 0xFFFF;
}

/* 枚举PCI设备 */
void pci_scan_bus(uint8_t bus);

/* 枚举PCI设备的功能 */
void pci_scan_device(uint8_t bus, uint8_t device) {
    uint8_t function = 0;
    
    if (pci_device_exists(bus, device, 0)) {
        pci_scan_bus(bus); // 检查是否为PCI桥
        return;
    }
    
    for (function = 0; function < 8; function++) {
        if (pci_device_exists(bus, device, function)) {
            uint16_t vendor_id = pci_read_word(bus, device, function, PCI_VENDOR_ID);
            uint16_t device_id = pci_read_word(bus, device, function, PCI_DEVICE_ID);
            uint8_t class_code = pci_read_byte(bus, device, function, PCI_CLASS);
            uint8_t subclass = pci_read_byte(bus, device, function, PCI_SUBCLASS);
            uint8_t prog_if = pci_read_byte(bus, device, function, PCI_PROG_IF);
            uint8_t revision_id = pci_read_byte(bus, device, function, PCI_REVISION_ID);
            uint8_t header_type = pci_read_byte(bus, device, function, PCI_HEADER_TYPE);
            
            // 存储设备信息
            if (pci_state.device_count < PCI_MAX_DEVICES) {
                pci_device_t *dev = &pci_state.devices[pci_state.device_count];
                dev->bus = bus;
                dev->device = device;
                dev->function = function;
                dev->vendor_id = vendor_id;
                dev->device_id = device_id;
                dev->class_code = class_code;
                dev->subclass = subclass;
                dev->prog_if = prog_if;
                dev->revision_id = revision_id;
                dev->header_type = header_type & 0x7F; // 移除多功能位
                dev->multifunction = (header_type & 0x80) != 0;
                
                // 获取BAR信息
                for (int bar = 0; bar < 6; bar++) {
                    dev->bars[bar] = pci_read_config(bus, device, function, PCI_BAR0 + bar * 4);
                }
                
                // 获取中断信息
                dev->interrupt_line = pci_read_byte(bus, device, function, PCI_INTERRUPT_LINE);
                dev->interrupt_pin = pci_read_byte(bus, device, function, PCI_INTERRUPT_PIN);
                
                pci_state.device_count++;
            }
        }
    }
}

/* 枚举PCI总线 */
void pci_scan_bus(uint8_t bus) {
    for (uint8_t device = 0; device < 32; device++) {
        pci_scan_device(bus, device);
    }
}

/* 初始化PCI驱动 */
void pci_init(void) {
    KLOG_INFO("Initializing PCI driver...");
    
    // 初始化PCI状态
    memset(&pci_state, 0, sizeof(pci_state));
    pci_state.initialized = true;
    
    // 枚举PCI设备
    pci_scan_bus(0);
    
    // 检查是否是多PCI桥系统
    uint8_t header_type = pci_read_byte(0, 0, 0, PCI_HEADER_TYPE);
    if (header_type & 0x80) {
        // 多功能设备，扫描其他功能
        for (uint8_t function = 1; function < 8; function++) {
            if (pci_device_exists(0, 0, function)) {
                pci_scan_bus(function);
            }
        }
    }
    
    KLOG_INFO("PCI driver initialized");
    console_write("PCI devices found: ");
    console_write_dec(pci_state.device_count);
    console_write("\n");
}

/* 获取PCI设备数量 */
uint32_t pci_get_device_count(void) {
    return pci_state.device_count;
}

/* 获取PCI设备信息 */
pci_device_t* pci_get_device(uint32_t index) {
    if (index >= pci_state.device_count) {
        return NULL;
    }
    return &pci_state.devices[index];
}

/* 根据设备ID查找设备 */
pci_device_t* pci_find_device(uint16_t vendor_id, uint16_t device_id) {
    for (uint32_t i = 0; i < pci_state.device_count; i++) {
        if (pci_state.devices[i].vendor_id == vendor_id && 
            pci_state.devices[i].device_id == device_id) {
            return &pci_state.devices[i];
        }
    }
    return NULL;
}

/* 根据类代码查找设备 */
pci_device_t* pci_find_device_by_class(uint8_t class_code, uint8_t subclass) {
    for (uint32_t i = 0; i < pci_state.device_count; i++) {
        if (pci_state.devices[i].class_code == class_code && 
            pci_state.devices[i].subclass == subclass) {
            return &pci_state.devices[i];
        }
    }
    return NULL;
}

/* 读取设备BAR值 */
uint32_t pci_get_bar(pci_device_t *dev, int bar_index) {
    if (bar_index < 0 || bar_index > 5) {
        return 0;
    }
    return dev->bars[bar_index];
}

/* 获取设备是否为桥接器 */
bool pci_is_bridge(pci_device_t *dev) {
    return (dev->class_code == PCI_CLASS_BRIDGE);
}

/* 获取设备是否为主机桥 */
bool pci_is_host_bridge(pci_device_t *dev) {
    return (dev->class_code == PCI_CLASS_BRIDGE && 
            dev->subclass == PCI_SUBCLASS_HOST_BRIDGE);
}

/* 获取设备是否为PCI桥 */
bool pci_is_pci_bridge(pci_device_t *dev) {
    return (dev->class_code == PCI_CLASS_BRIDGE && 
            dev->subclass == PCI_SUBCLASS_PCI_BRIDGE);
}

/* 获取设备是否为ISA桥 */
bool pci_is_isa_bridge(pci_device_t *dev) {
    return (dev->class_code == PCI_CLASS_BRIDGE && 
            dev->subclass == PCI_SUBCLASS_ISA_BRIDGE);
}

/* 启用设备的内存和I/O访问 */
void pci_enable_device(pci_device_t *dev) {
    uint16_t command = pci_read_word(dev->bus, dev->device, dev->function, PCI_COMMAND);
    
    // 启用内存访问、I/O访问和总线主控
    command |= 0x0007; // 启用I/O, 内存, 总线主控
    
    pci_write_config(dev->bus, dev->device, dev->function, PCI_COMMAND, command);
}

/* 设置设备的中断线 */
void pci_set_interrupt_line(pci_device_t *dev, uint8_t interrupt_line) {
    uint32_t config = pci_read_config(dev->bus, dev->device, dev->function, PCI_INTERRUPT_LINE);
    config = (config & 0xFFFFFF00) | interrupt_line;
    pci_write_config(dev->bus, dev->device, dev->function, PCI_INTERRUPT_LINE, config);
}

/* 获取设备状态 */
bool pci_is_initialized(void) {
    return pci_state.initialized;
}
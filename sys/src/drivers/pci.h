/*
 * M4KK1 PCI驱动 - PCI设备管理接口
 * 支持PCI和PCIe设备枚举和管理
 */

#ifndef __PCI_H__
#define __PCI_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 最大PCI设备数量 */
#define PCI_MAX_DEVICES 64

/* PCI设备结构 */
typedef struct {
    uint8_t bus;              /* 总线号 */
    uint8_t device;           /* 设备号 */
    uint8_t function;         /* 功能号 */
    uint16_t vendor_id;       /* 厂商ID */
    uint16_t device_id;       /* 设备ID */
    uint8_t class_code;       /* 类代码 */
    uint8_t subclass;         /* 子类 */
    uint8_t prog_if;          /* 编程接口 */
    uint8_t revision_id;      /* 修订版本 */
    uint8_t header_type;      /* 头部类型 */
    bool multifunction;       /* 是否为多功能设备 */
    uint32_t bars[6];         /* 基地址寄存器 */
    uint8_t interrupt_line;   /* 中断线 */
    uint8_t interrupt_pin;    /* 中断引脚 */
} pci_device_t;

/* PCI设备类代码 */
typedef enum {
    PCI_CLASS_LEGACY = 0x00,
    PCI_CLASS_STORAGE = 0x01,
    PCI_CLASS_NETWORK = 0x02,
    PCI_CLASS_DISPLAY = 0x03,
    PCI_CLASS_MULTIMEDIA = 0x04,
    PCI_CLASS_MEMORY = 0x05,
    PCI_CLASS_BRIDGE = 0x06,
    PCI_CLASS_COMMUNICATION = 0x07,
    PCI_CLASS_PERIPHERAL = 0x08,
    PCI_CLASS_INPUT = 0x09,
    PCI_CLASS_DOCKING = 0x0A,
    PCI_CLASS_PROCESSOR = 0x0B,
    PCI_CLASS_SERIAL = 0x0C,
    PCI_CLASS_INTELLIGENT = 0x0D,
    PCI_CLASS_SATELLITE = 0x0E,
    PCI_CLASS_CRYPT = 0x10,
    PCI_CLASS_SIGNAL = 0x11,
    PCI_CLASS_PROCESSING = 0x12,
    PCI_CLASS_NON_ESSENTIAL = 0x13,
    PCI_CLASS_CO = 0x40,
    PCI_CLASS_OTHER = 0xFF
} pci_class_code_t;

/* PCI子类代码 */
typedef enum {
    /* 存储设备子类 */
    PCI_SUBCLASS_SCSI = 0x00,
    PCI_SUBCLASS_IDE = 0x01,
    PCI_SUBCLASS_FLOPPY = 0x02,
    PCI_SUBCLASS_IPI = 0x03,
    PCI_SUBCLASS_RAID = 0x04,
    PCI_SUBCLASS_ATA = 0x05,
    PCI_SUBCLASS_SATA = 0x06,
    PCI_SUBCLASS_SAS = 0x07,
    PCI_SUBCLASS_NVM = 0x08,
    
    /* 网络设备子类 */
    PCI_SUBCLASS_ETHERNET = 0x00,
    PCI_SUBCLASS_TOKEN_RING = 0x01,
    PCI_SUBCLASS_FDDI = 0x02,
    PCI_SUBCLASS_ATM = 0x03,
    PCI_SUBCLASS_ISDN = 0x04,
    PCI_SUBCLASS_WORLDFIP = 0x05,
    PCI_SUBCLASS_PICMG = 0x06,
    PCI_SUBCLASS_INFINIBAND = 0x07,
    PCI_SUBCLASS_FABRIC = 0x08,
    
    /* 显示设备子类 */
    PCI_SUBCLASS_VGA = 0x00,
    PCI_SUBCLASS_XGA = 0x01,
    PCI_SUBCLASS_3D = 0x02,
    
    /* 输入设备子类 */
    PCI_SUBCLASS_KEYBOARD = 0x00,
    PCI_SUBCLASS_DIGITIZER = 0x01,
    PCI_SUBCLASS_MOUSE = 0x02,
    
    /* 桥接设备子类 */
    PCI_SUBCLASS_HOST_BRIDGE = 0x00,
    PCI_SUBCLASS_ISA_BRIDGE = 0x01,
    PCI_SUBCLASS_EISA_BRIDGE = 0x02,
    PCI_SUBCLASS_MCA_BRIDGE = 0x03,
    PCI_SUBCLASS_PCI_BRIDGE = 0x04,
    PCI_SUBCLASS_PCMCIA_BRIDGE = 0x05,
    PCI_SUBCLASS_NUBUS_BRIDGE = 0x06,
    PCI_SUBCLASS_CARDBUS_BRIDGE = 0x07,
    PCI_SUBCLASS_RACEWAY_BRIDGE = 0x08,
    PCI_SUBCLASS_PCI_SEMITRANSPARENT_BRIDGE = 0x09,
    PCI_SUBCLASS_INFINIBAND_TO_PCI_HOST_BRIDGE = 0x0A,
    
    /* 串行设备子类 */
    PCI_SUBCLASS_FIREWIRE = 0x00,
    PCI_SUBCLASS_ACCESS = 0x01,
    PCI_SUBCLASS_SSA = 0x02,
    PCI_SUBCLASS_USB = 0x03,
    PCI_SUBCLASS_FIBER = 0x04,
    PCI_SUBCLASS_SMBUS = 0x05,
    PCI_SUBCLASS_INFINIBAND_SERIAL = 0x06,
    PCI_SUBCLASS_IPMI_INTERFACE = 0x07,
    PCI_SUBCLASS_SERCOS = 0x08,
    PCI_SUBCLASS_CANBUS = 0x09,
} pci_subclass_code_t;

/* PCI驱动接口结构 */
typedef struct {
    char *name;                     /* 驱动名称 */
    char *description;              /* 驱动描述 */

    /* 初始化和清理 */
    int (*init)(void);              /* 初始化驱动 */
    void (*cleanup)(void);          /* 清理驱动 */

    /* 设备管理 */
    int (*enumerate)(void);         /* 枚举设备 */
    int (*get_device_count)(void);  /* 获取设备数量 */
    pci_device_t* (*get_device)(int index); /* 获取设备 */

    /* 配置管理 */
    uint32_t (*read_config)(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
    void (*write_config)(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);

    /* 私有数据 */
    void *priv_data;                /* 私有数据 */
} pci_driver_t;

/* 函数声明 */

/* PCI驱动管理 */
int pci_driver_register(pci_driver_t *driver);
int pci_driver_unregister(pci_driver_t *driver);
pci_driver_t *pci_driver_get(const char *name);

/* PCI设备枚举 */
void pci_init(void);
void pci_scan_bus(uint8_t bus);
void pci_scan_device(uint8_t bus, uint8_t device);

/* PCI配置空间访问 */
uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
void pci_write_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);
uint16_t pci_read_word(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
uint8_t pci_read_byte(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);

/* PCI设备信息获取 */
uint32_t pci_get_device_count(void);
pci_device_t* pci_get_device(uint32_t index);
pci_device_t* pci_find_device(uint16_t vendor_id, uint16_t device_id);
pci_device_t* pci_find_device_by_class(uint8_t class_code, uint8_t subclass);

/* PCI设备操作 */
uint32_t pci_get_bar(pci_device_t *dev, int bar_index);
bool pci_is_bridge(pci_device_t *dev);
bool pci_is_host_bridge(pci_device_t *dev);
bool pci_is_pci_bridge(pci_device_t *dev);
bool pci_is_isa_bridge(pci_device_t *dev);
void pci_enable_device(pci_device_t *dev);
void pci_set_interrupt_line(pci_device_t *dev, uint8_t interrupt_line);

/* 状态查询 */
bool pci_is_initialized(void);

/* PCI设备类型检测 */
static inline bool pci_is_storage_device(pci_device_t *dev) {
    return (dev->class_code == PCI_CLASS_STORAGE);
}

static inline bool pci_is_network_device(pci_device_t *dev) {
    return (dev->class_code == PCI_CLASS_NETWORK);
}

static inline bool pci_is_display_device(pci_device_t *dev) {
    return (dev->class_code == PCI_CLASS_DISPLAY);
}

static inline bool pci_is_input_device(pci_device_t *dev) {
    return (dev->class_code == PCI_CLASS_INPUT);
}

static inline bool pci_is_usb_controller(pci_device_t *dev) {
    return (dev->class_code == PCI_CLASS_SERIAL && dev->subclass == PCI_SUBCLASS_USB);
}

#endif /* __PCI_H__ */
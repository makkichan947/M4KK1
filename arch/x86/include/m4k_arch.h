/**
 * M4KK1 x86 Architecture Definitions
 * x86架构特定定义和接口 (32位)
 *
 * 文件: m4k_arch.h
 * 作者: M4KK1 Development Team
 * 版本: v0.2.0-multarch
 * 日期: 2025-10-16
 *
 * 描述:
 *   定义x86架构特定的常量、宏和接口函数
 *   提供与硬件抽象层的接口
 *
 * 架构特性:
 *   - 32位x86架构
 *   - 保护模式
 *   - 2级页表结构
 *   - 多处理器支持
 *
 * 内存布局:
 *   - 内核基地址: 0xC0000000
 *   - 用户空间: 0x00000000 - 0xBFFFFFFF
 *   - 内核空间: 0xC0000000 - 0xFFFFFFFF
 *
 * 中断处理:
 *   - 0x00-0x1F: 异常处理
 *   - 0x20-0x2F: 硬件中断
 *   - 0x4D: M4KK1独特系统调用
 *   - 0x80: 传统系统调用（兼容性）
 *
 * 修改历史:
 *   v0.2.0: 实现x86架构支持
 *   v0.1.0: 初始架构定义
 */

#ifndef __M4K_X86_ARCH_H__
#define __M4K_X86_ARCH_H__

#include <stdint.h>

/* 架构标识 */
#define M4K_ARCH_X86            1
#define M4K_ARCH_NAME           "x86"
#define M4K_ARCH_BITS           32

/* 内存布局 */
#define M4K_KERNEL_BASE         0xC0000000UL
#define M4K_KERNEL_HEAP         0xC0400000UL
#define M4K_KERNEL_STACK        0xC07FE000UL
#define M4K_USER_BASE           0x00000000UL
#define M4K_USER_LIMIT          0xBFFFFFFFUL

/* 页表结构 */
#define M4K_PAGE_SIZE           4096
#define M4K_PAGE_MASK           (~(M4K_PAGE_SIZE - 1))
#define M4K_PAGE_SHIFT          12
#define M4K_PD_SHIFT            22
#define M4K_PT_SHIFT            12

/* 中断向量 */
#define M4K_INT_SYSCALL         0x4D    /* M4KK1独特系统调用 */
#define M4K_INT_TIMER           0x20    /* 定时器中断 */
#define M4K_INT_KEYBOARD        0x21    /* 键盘中断 */
#define M4K_INT_MOUSE           0x2C    /* 鼠标中断 */

/* 段选择子 */
#define M4K_KERNEL_CODE         0x08
#define M4K_KERNEL_DATA         0x10
#define M4K_USER_CODE           0x18
#define M4K_USER_DATA           0x20
#define M4K_TSS                 0x28

/* 控制寄存器 */
#define M4K_CR0_PE              (1 << 0)  /* 保护模式启用 */
#define M4K_CR0_MP              (1 << 1)  /* 监控协处理器 */
#define M4K_CR0_EM              (1 << 2)  /* 模拟协处理器 */
#define M4K_CR0_TS              (1 << 3)  /* 任务切换 */
#define M4K_CR0_ET              (1 << 4)  /* 扩展类型 */
#define M4K_CR0_NE              (1 << 5)  /* 数字错误 */
#define M4K_CR0_WP              (1 << 16) /* 写保护 */
#define M4K_CR0_AM              (1 << 18) /* 对齐掩码 */
#define M4K_CR0_NW              (1 << 29) /* 非写穿 */
#define M4K_CR0_CD              (1 << 30) /* 缓存禁用 */
#define M4K_CR0_PG              (1 << 31) /* 分页启用 */

/* 架构特定函数声明 */
void m4k_arch_init(void);
void m4k_arch_detect_features(void);
uint32_t m4k_arch_get_cpu_count(void);
void m4k_arch_send_ipi(uint32_t cpu, uint32_t vector);

/* 内存管理 */
void m4k_arch_paging_init(void);
void *m4k_arch_map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags);
void m4k_arch_unmap_page(uint32_t virtual_addr);

/* 中断处理 */
void m4k_arch_interrupt_init(void);
void m4k_arch_register_handler(uint32_t vector, void *handler);
void m4k_arch_enable_interrupts(void);
void m4k_arch_disable_interrupts(void);

/* I/O 操作 */
uint8_t m4k_arch_inb(uint16_t port);
void m4k_arch_outb(uint16_t port, uint8_t value);
uint16_t m4k_arch_inw(uint16_t port);
void m4k_arch_outw(uint16_t port, uint16_t value);
uint32_t m4k_arch_ind(uint16_t port);
void m4k_arch_outd(uint16_t port, uint32_t value);

/* CPU信息 */
typedef struct {
    uint32_t cpu_id;
    uint32_t features_edx;
    uint32_t features_ecx;
    uint32_t cpu_count;
    uint32_t core_count;
    uint32_t thread_count;
    char vendor_string[13];
    char brand_string[48];
} m4k_cpu_info_t;

extern m4k_cpu_info_t m4k_cpu_info;

/* 内存信息 */
typedef struct {
    uint32_t total_memory;
    uint32_t free_memory;
    uint32_t used_memory;
    uint32_t kernel_memory;
    uint32_t user_memory;
} m4k_memory_info_t;

extern m4k_memory_info_t m4k_memory_info;

/* 架构特定内联函数 */
static inline uint32_t m4k_read_cr0(void) {
    uint32_t value;
    __asm__ volatile ("movl %%cr0, %0" : "=r"(value));
    return value;
}

static inline void m4k_write_cr0(uint32_t value) {
    __asm__ volatile ("movl %0, %%cr0" : : "r"(value));
}

static inline uint32_t m4k_read_cr2(void) {
    uint32_t value;
    __asm__ volatile ("movl %%cr2, %0" : "=r"(value));
    return value;
}

static inline uint32_t m4k_read_cr3(void) {
    uint32_t value;
    __asm__ volatile ("movl %%cr3, %0" : "=r"(value));
    return value;
}

static inline void m4k_write_cr3(uint32_t value) {
    __asm__ volatile ("movl %0, %%cr3" : : "r"(value));
}

static inline uint32_t m4k_read_cr4(void) {
    uint32_t value;
    __asm__ volatile ("movl %%cr4, %0" : "=r"(value));
    return value;
}

static inline void m4k_write_cr4(uint32_t value) {
    __asm__ volatile ("movl %0, %%cr4" : : "r"(value));
}

static inline void m4k_enable_interrupts(void) {
    __asm__ volatile ("sti");
}

static inline void m4k_disable_interrupts(void) {
    __asm__ volatile ("cli");
}

static inline uint32_t m4k_read_flags(void) {
    uint32_t flags;
    __asm__ volatile ("pushfl; popl %0" : "=r"(flags));
    return flags;
}

static inline void m4k_write_flags(uint32_t flags) {
    __asm__ volatile ("pushl %0; popfl" : : "r"(flags));
}

static inline void m4k_invalidate_tlb(uint32_t addr) {
    __asm__ volatile ("invlpg (%0)" : : "r"(addr));
}

static inline void m4k_halt(void) {
    __asm__ volatile ("hlt");
}

static inline void m4k_pause(void) {
    __asm__ volatile ("pause");
}

#endif /* __M4K_X86_ARCH_H__ */
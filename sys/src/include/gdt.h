/**
 * M4KK1 GDT (Global Descriptor Table) Header
 * 全局描述符表定义和操作函数
 */

#ifndef __GDT_H__
#define __GDT_H__

#include <stdint.h>

/**
 * GDT条目结构
 */
typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

/**
 * GDT指针结构
 */
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

/**
 * GDT段选择子
 */
#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_CODE   0x18
#define GDT_USER_DATA   0x20
#define GDT_TSS         0x28

/**
 * GDT访问字节标志
 */
#define GDT_ACCESS_PRESENT      0x80
#define GDT_ACCESS_PRIVILEGE_0  0x00
#define GDT_ACCESS_PRIVILEGE_1  0x20
#define GDT_ACCESS_PRIVILEGE_2  0x40
#define GDT_ACCESS_PRIVILEGE_3  0x60
#define GDT_ACCESS_EXECUTABLE   0x08
#define GDT_ACCESS_DIRECTION    0x04
#define GDT_ACCESS_READWRITE    0x02
#define GDT_ACCESS_ACCESSED     0x01

/**
 * GDT粒度标志
 */
#define GDT_GRANULARITY_1B      0x00
#define GDT_GRANULARITY_4K      0x80
#define GDT_GRANULARITY_16BIT   0x00
#define GDT_GRANULARITY_32BIT   0x40

/**
 * TSS结构
 */
typedef struct {
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed)) tss_entry_t;

/**
 * 初始化GDT
 */
void gdt_init(void);

/**
 * 设置GDT条目
 */
void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

/**
 * 刷新GDT
 */
void gdt_flush(void);

/**
 * 加载TSS
 */
void tss_flush(void);

/**
 * 设置内核栈
 */
void set_kernel_stack(uint32_t stack);

#endif /* __GDT_H__ */
/**
 * M4KK1 GDT (Global Descriptor Table) Implementation
 * 全局描述符表C语言实现
 */

#include "../../include/gdt.h"
#include "../../include/stdint.h"
#include "../../include/string.h"

// 外部汇编函数声明
extern void gdt_load(void);
extern void gdt_set_tss_base(uint32_t base);
extern void gdt_set_tss_limit(uint32_t limit);

// GDT表和指针定义
static gdt_entry_t gdt_entries[6];  // 空描述符 + 内核代码段 + 内核数据段 + 用户代码段 + 用户数据段 + TSS段
static gdt_ptr_t gdt_ptr;

// TSS结构定义
static tss_entry_t tss_entry;

// 内核栈地址
static uint32_t kernel_stack = 0;

/**
 * 设置GDT条目
 */
void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entry_t *entry = &gdt_entries[num];

    // 设置基址
    entry->base_low = (base & 0xFFFF);
    entry->base_middle = (base >> 16) & 0xFF;
    entry->base_high = (base >> 24) & 0xFF;

    // 设置限制
    entry->limit_low = (limit & 0xFFFF);
    entry->granularity = (limit >> 16) & 0x0F;

    // 设置粒度和访问权限
    entry->granularity |= gran & 0xF0;
    entry->access = access;
}

/**
 * 初始化GDT
 */
void gdt_init(void) {
    // 设置GDT指针
    gdt_ptr.limit = sizeof(gdt_entries) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    // 清空GDT条目
    memset(gdt_entries, 0, sizeof(gdt_entries));

    // 设置内核代码段 (特权级0)
    gdt_set_gate(1, 0x00000000, 0xFFFFFFFF,
                 GDT_ACCESS_PRESENT | GDT_ACCESS_PRIVILEGE_0 | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_READWRITE,
                 GDT_GRANULARITY_4K | GDT_GRANULARITY_32BIT);

    // 设置内核数据段 (特权级0)
    gdt_set_gate(2, 0x00000000, 0xFFFFFFFF,
                 GDT_ACCESS_PRESENT | GDT_ACCESS_PRIVILEGE_0 | GDT_ACCESS_READWRITE,
                 GDT_GRANULARITY_4K | GDT_GRANULARITY_32BIT);

    // 设置用户代码段 (特权级3)
    gdt_set_gate(3, 0x00000000, 0xFFFFFFFF,
                 GDT_ACCESS_PRESENT | GDT_ACCESS_PRIVILEGE_3 | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_READWRITE,
                 GDT_GRANULARITY_4K | GDT_GRANULARITY_32BIT);

    // 设置用户数据段 (特权级3)
    gdt_set_gate(4, 0x00000000, 0xFFFFFFFF,
                 GDT_ACCESS_PRESENT | GDT_ACCESS_PRIVILEGE_3 | GDT_ACCESS_READWRITE,
                 GDT_GRANULARITY_4K | GDT_GRANULARITY_32BIT);

    // 初始化TSS
    memset(&tss_entry, 0, sizeof(tss_entry_t));

    // 设置TSS段
    gdt_set_gate(5, (uint32_t)&tss_entry, sizeof(tss_entry_t) - 1,
                 GDT_ACCESS_PRESENT | GDT_ACCESS_PRIVILEGE_0 | 0x09,
                 0x00);

    // 设置内核栈到TSS
    if (kernel_stack != 0) {
        tss_entry.ss0 = GDT_KERNEL_DATA;
        tss_entry.esp0 = kernel_stack;
    }

    // 刷新GDT
    gdt_flush();
}

/**
 * 刷新GDT
 */
void gdt_flush(void) {
    // 调用汇编函数加载GDT
    gdt_load();
}

/**
 * 加载TSS
 */
void tss_flush(void) {
    // 使用汇编函数设置TSS基址和限制
    gdt_set_tss_base((uint32_t)&tss_entry);
    gdt_set_tss_limit(sizeof(tss_entry_t) - 1);

    // 使用LTR指令加载TSS (通过内联汇编)
    __asm__ volatile (
        "ltr %%ax\n"
        :
        : "a" (0x28)    // TSS段选择子 (0x28 = 40)
    );
}

/**
 * 设置内核栈
 */
void set_kernel_stack(uint32_t stack) {
    kernel_stack = stack;

    // 更新TSS中的内核栈
    tss_entry.ss0 = GDT_KERNEL_DATA;
    tss_entry.esp0 = stack;
}
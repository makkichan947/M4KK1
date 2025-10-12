/**
 * M4KK1 IDT (Interrupt Descriptor Table) Header
 * 中断描述符表定义和操作函数
 */

#ifndef __IDT_H__
#define __IDT_H__

#include <stdint.h>

/**
 * IDT条目结构
 */
typedef struct {
    uint16_t base_low;
    uint16_t selector;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed)) idt_entry_t;

/**
 * IDT指针结构
 */
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

/**
 * 中断处理函数类型
 */
typedef void (*interrupt_handler_t)(void);

/**
 * IDT标志位
 */
#define IDT_PRESENT         0x80
#define IDT_DPL_0           0x00
#define IDT_DPL_1           0x20
#define IDT_DPL_2           0x40
#define IDT_DPL_3           0x60
#define IDT_STORAGE_SEGMENT 0x10
#define IDT_GATE_16BIT      0x00
#define IDT_GATE_32BIT      0x08
#define IDT_TRAP_GATE       0x07
#define IDT_INTERRUPT_GATE  0x06
#define IDT_TASK_GATE       0x05

/**
 * 中断号定义
 */
#define IDT_DIVIDE_BY_ZERO          0x00
#define IDT_DEBUG                   0x01
#define IDT_NMI                     0x02
#define IDT_BREAKPOINT              0x03
#define IDT_OVERFLOW                0x04
#define IDT_BOUND_RANGE             0x05
#define IDT_INVALID_OPCODE          0x06
#define IDT_DEVICE_NOT_AVAILABLE    0x07
#define IDT_DOUBLE_FAULT            0x08
#define IDT_COPROCESSOR_SEGMENT     0x09
#define IDT_INVALID_TSS             0x0A
#define IDT_SEGMENT_NOT_PRESENT     0x0B
#define IDT_STACK_SEGMENT_FAULT     0x0C
#define IDT_GENERAL_PROTECTION      0x0D
#define IDT_PAGE_FAULT              0x0E
#define IDT_RESERVED                0x0F
#define IDT_FPU_ERROR               0x10
#define IDT_ALIGNMENT_CHECK         0x11
#define IDT_MACHINE_CHECK           0x12
#define IDT_SIMD_ERROR              0x13
#define IDT_VIRT_EXCEPTION          0x14
#define IDT_CONTROL_PROTECTION      0x15

#define IDT_TIMER                   0x20
#define IDT_KEYBOARD                0x21
#define IDT_CASCADE                 0x22
#define IDT_COM2                    0x23
#define IDT_COM1                    0x24
#define IDT_LPT2                    0x25
#define IDT_FLOPPY                  0x26
#define IDT_LPT1                    0x27
#define IDT_RTC                     0x28
#define IDT_FREE1                   0x29
#define IDT_FREE2                   0x2A
#define IDT_FREE3                   0x2B
#define IDT_MOUSE                   0x2C
#define IDT_FPU                     0x2D
#define IDT_PRIMARY_ATA             0x2E
#define IDT_SECONDARY_ATA           0x2F

/**
 * 初始化IDT
 */
void idt_init(void);

/**
 * 设置IDT条目
 */
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

/**
 * 注册中断处理函数
 */
void idt_register_handler(uint8_t num, interrupt_handler_t handler);

/**
 * 启用中断
 */
void idt_enable_interrupts(void);

/**
 * 禁用中断
 */
void idt_disable_interrupts(void);

/**
 * 中断处理函数声明
 */
extern void idt_handler_00(void);
extern void idt_handler_01(void);
extern void idt_handler_02(void);
extern void idt_handler_03(void);
extern void idt_handler_04(void);
extern void idt_handler_05(void);
extern void idt_handler_06(void);
extern void idt_handler_07(void);
extern void idt_handler_08(void);
extern void idt_handler_09(void);
extern void idt_handler_0A(void);
extern void idt_handler_0B(void);
extern void idt_handler_0C(void);
extern void idt_handler_0D(void);
extern void idt_handler_0E(void);
extern void idt_handler_0F(void);
extern void idt_handler_10(void);
extern void idt_handler_11(void);
extern void idt_handler_12(void);
extern void idt_handler_13(void);
extern void idt_handler_14(void);
extern void idt_handler_15(void);
extern void idt_handler_20(void);
extern void idt_handler_21(void);
extern void idt_handler_22(void);
extern void idt_handler_23(void);
extern void idt_handler_24(void);
extern void idt_handler_25(void);
extern void idt_handler_26(void);
extern void idt_handler_27(void);
extern void idt_handler_28(void);
extern void idt_handler_29(void);
extern void idt_handler_2A(void);
extern void idt_handler_2B(void);
extern void idt_handler_2C(void);
extern void idt_handler_2D(void);
extern void idt_handler_2E(void);
extern void idt_handler_2F(void);

#endif /* __IDT_H__ */
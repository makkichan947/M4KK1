/**
 * M4KK1 IDT (Interrupt Descriptor Table) Implementation
 * 中断描述符表C语言实现
 */

#include "../../include/idt.h"
#include "../../include/stdint.h"
#include "../../include/string.h"
#include "../../include/console.h"
#include "../../include/kernel.h"

/* 汇编函数声明 */
extern void idt_init(void);
extern void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);
extern void pic_init(void);
extern void enable_interrupts(void);
extern void disable_interrupts(void);
extern uint32_t interrupts_enabled(void);
extern void pic_send_eoi(uint32_t irq_num);

/**
 * 中断处理函数数组
 * 用于存储用户注册的中断处理函数
 */
static interrupt_handler_t interrupt_handlers[256];

/**
 * 异常消息数组
 */
static const char *exception_messages[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Device not available",
    "Double fault",
    "Coprocessor segment overrun",
    "Invalid TSS",
    "Segment not present",
    "Stack segment fault",
    "General protection fault",
    "Page fault",
    "Reserved",
    "FPU floating point error",
    "Alignment check",
    "Machine check",
    "SIMD floating point error",
    "Virtualization error",
    "Control protection error",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

/**
 * 初始化IDT系统
 * 调用汇编函数进行底层初始化
 */
void idt_init_c(void)
{
    /* 清空中断处理函数数组 */
    memset(interrupt_handlers, 0, sizeof(interrupt_handlers));

    /* 调用汇编初始化函数 */
    idt_init();

    /* 初始化PIC */
    pic_init();

    /* 打印初始化信息 */
    KLOG_INFO("IDT initialized successfully");
}

/**
 * 设置IDT条目
 * @param num 中断向量号
 * @param base 中断处理函数地址
 * @param selector 段选择子
 * @param flags 标志位
 */
void idt_set_gate_c(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
{
    /* 调用汇编函数设置IDT条目 */
    idt_set_gate(num, base, selector, flags);
}

/**
 * 注册中断处理函数
 * @param num 中断向量号
 * @param handler 中断处理函数指针
 */
void idt_register_handler(uint8_t num, interrupt_handler_t handler)
{
    if (num < 256) {
        interrupt_handlers[num] = handler;
        KLOG_INFO("Interrupt handler registered for vector 0x");
        console_write_hex(num);
        console_write("\n");
    } else {
        KLOG_WARN("Invalid interrupt vector number: 0x");
        console_write_hex(num);
        console_write("\n");
    }
}

/**
 * 取消注册中断处理函数
 * @param num 中断向量号
 */
void idt_unregister_handler(uint8_t num)
{
    if (num < 256) {
        interrupt_handlers[num] = NULL;
        KLOG_INFO("Interrupt handler unregistered for vector 0x");
        console_write_hex(num);
        console_write("\n");
    }
}

/**
 * 获取中断处理函数
 * @param num 中断向量号
 * @return 中断处理函数指针，未注册则返回NULL
 */
interrupt_handler_t idt_get_handler(uint8_t num)
{
    if (num < 256) {
        return interrupt_handlers[num];
    }
    return NULL;
}

/**
 * 启用中断
 */
void idt_enable_interrupts(void)
{
    enable_interrupts();
    KLOG_INFO("Interrupts enabled");
}

/**
 * 禁用中断
 */
void idt_disable_interrupts(void)
{
    disable_interrupts();
    KLOG_INFO("Interrupts disabled");
}

/**
 * 检查中断是否启用
 * @return 1表示启用，0表示禁用
 */
uint32_t idt_interrupts_enabled(void)
{
    return interrupts_enabled();
}

/**
 * 处理异常
 * @param vector 异常向量号
 */
void idt_handle_exception(uint32_t vector)
{
    const char *message = NULL;

    /* 获取异常消息 */
    if (vector < 32 && exception_messages[vector]) {
        message = exception_messages[vector];
    } else {
        message = "Unknown exception";
    }

    /* 打印异常信息 */
    KLOG_ERROR("*** EXCEPTION OCCURRED ***");
    KLOG_ERROR("Vector: 0x");
    console_write_hex(vector);
    console_write("\n");
    KLOG_ERROR("Error: ");
    console_write(message);
    console_write("\n");

    /* 如果有注册的处理函数，调用它 */
    if (interrupt_handlers[vector]) {
        KLOG_INFO("Calling registered exception handler...");
        interrupt_handlers[vector]();
    } else {
        KLOG_ERROR("No handler registered for this exception.");
        KLOG_ERROR("System halted.");

        /* 停止系统 */
        idt_disable_interrupts();
        while (1) {
            /* 无限循环 */
        }
    }
}

/**
 * 处理IRQ中断
 * @param irq_num IRQ号
 */
void idt_handle_irq(uint32_t irq_num)
{
    uint32_t vector = irq_num + 0x20; /* IRQ基础向量号 */

    /* 发送EOI到PIC */
    pic_send_eoi(irq_num );

    /* 如果有注册的处理函数，调用它 */
    if (interrupt_handlers[vector]) {
        interrupt_handlers[vector]();
    } else {
        KLOG_WARN("Unhandled IRQ ");
        console_write_dec(irq_num );
        console_write(" (vector 0x");
        console_write_hex(vector);
        console_write(")\n");
    }
}

/**
 * 获取异常描述信息
 * @param vector 异常向量号
 * @return 异常描述字符串
 */
const char *idt_get_exception_message(uint32_t vector)
{
    if (vector < 32 && exception_messages[vector]) {
        return exception_messages[vector];
    }
    return "Unknown exception";
}

/**
 * 打印IDT状态信息
 */
void idt_print_status(void)
{
    uint32_t i, count = 0;

    KLOG_INFO("IDT Status:");
    KLOG_INFO("Registered handlers:");

    for (i = 0; i < 256; i++) {
        if (interrupt_handlers[i]) {
            if (i < 32) {
                KLOG_INFO("  Vector 0x");
                console_write_hex(i);
                console_write(" (Exception): ");
                console_write(idt_get_exception_message(i));
                console_write("\n");
            } else if (i >= 0x20 && i < 0x30) {
                KLOG_INFO("  Vector 0x");
                console_write_hex(i);
                console_write(" (IRQ ");
                console_write_dec(i - 0x20);
                console_write("): Registered\n");
            } else {
                KLOG_INFO("  Vector 0x");
                console_write_hex(i);
                console_write(": Registered\n");
            }
            count++;
        }
    }

    KLOG_INFO("Total registered handlers: ");
    console_write_dec(count);
    console_write("\n");
    KLOG_INFO("Interrupts are ");
    console_write(idt_interrupts_enabled() ? "enabled" : "disabled");
    console_write("\n");
}
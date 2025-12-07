/**
 * M4KK1 Y4KU Kernel - Main Kernel Implementation
 * 主内核文件 - 负责系统初始化和核心功能管理
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "kernel.h"
#include "multiboot.h"
#include "console.h"
#include "memory.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "process.h"
#include "m4k_syscall.h"
#include "ldso.h"
#include "../drivers/keyboard/keyboard.h"
#include "../drivers/mouse/mouse.h"

/* 前向声明 */
void m4k_syscall_init(void);
void keyboard_init(void);
void mouse_init(void);

/**
 * 内核信息实例
 */
static kernel_info_t kernel_info = {
    .magic = M4KK1_KERNEL_MAGIC,
    .version = (KERNEL_VERSION_MAJOR << 16) | (KERNEL_VERSION_MINOR << 8) | KERNEL_VERSION_PATCH,
    .build_date = KERNEL_BUILD_DATE,
    .build_time = KERNEL_BUILD_TIME,
    .uptime_seconds = 0,
    .process_count = 0,
    .memory_total = 0,
    .memory_free = 0,
    .memory_used = 0
};

/**
 * 内核主函数
 * 这是内核的入口点，由汇编代码调用
 */
void kmain(multiboot_info_t *mb_info, uint32_t magic) {
    int init_status = 0;

    // 初始化控制台
    console_init();
    console_write("=====================================\n");
    console_write("    M4KK1 Y4KU Kernel Starting...\n");
    console_write("=====================================\n");

    // 显示内核启动消息
    console_write("Bootloader Magic: 0x");
    console_write_hex(magic);
    console_write("\n");

    // 验证魔数
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        console_write("ERROR: Invalid bootloader magic! Halting.\n");
        panic("Invalid bootloader magic");
    }

    // 显示内核信息
    console_write("Kernel Version: ");
    console_write_dec((kernel_info.version >> 16) & 0xFF);
    console_write(".");
    console_write_dec((kernel_info.version >> 8) & 0xFF);
    console_write(".");
    console_write_dec(kernel_info.version & 0xFF);
    console_write("-");
    console_write(KERNEL_VERSION_TYPE);
    console_write("\n");
    console_write("Build Date: ");
    console_write(kernel_info.build_date);
    console_write(" ");
    console_write(kernel_info.build_time);
    console_write("\n");

    console_write("=====================================\n");
    console_write("Initializing System Components...\n");
    console_write("=====================================\n");

    // 1. 初始化内存管理系统
    console_write("1. Initializing Memory Management...\n");
    if (mb_info == NULL) {
        console_write("ERROR: Invalid multiboot info!\n");
        panic("Invalid multiboot info");
    }

    memory_init(mb_info);
    uint32_t total_mem = memory_get_total();
    uint32_t free_mem = memory_get_free();

    console_write("   ✓ Memory management initialized.\n");
    console_write("   Total memory: ");
    console_write_dec(total_mem / 1024);
    console_write(" KB\n");
    console_write("   Free memory: ");
    console_write_dec(free_mem / 1024);
    console_write(" KB\n");

    // 2. 初始化GDT系统
    console_write("2. Initializing GDT...\n");
    gdt_init();
    console_write("   ✓ GDT initialized.\n");

    // 3. 初始化IDT系统和中断处理
    console_write("3. Initializing IDT and Interrupts...\n");
    idt_init();
    console_write("   ✓ IDT initialized.\n");

    // 4. 初始化定时器系统
    console_write("4. Initializing Timer System...\n");
    timer_init(1000); // 1ms tick
    console_write("   ✓ Timer initialized (1000 Hz).\n");

    // 5. 初始化进程管理系统
    console_write("5. Initializing Process Management...\n");
    process_init();
    console_write("   ✓ Process management initialized.\n");

    // 6. 初始化M4KK1独特系统调用系统
    console_write("6. Initializing M4KK1 System Calls...\n");
    m4k_syscall_init();
    console_write("   ✓ M4KK1 system calls initialized.\n");

    // 7. 初始化设备驱动
    console_write("7. Initializing Device Drivers...\n");
    // Initialize keyboard driver
    keyboard_init();
    console_write("   ✓ Keyboard driver initialized.\n");
    
    // Initialize mouse driver
    mouse_init();
    console_write("   ✓ Mouse driver initialized.\n");

    // 8. 初始化动态链接库系统
    console_write("8. Initializing Dynamic Linker...\n");
    if (m4ll_init() != 0) {
        console_write("   WARNING: Dynamic linker initialization failed!\n");
        console_write("   Some features may not be available.\n");
    } else {
        console_write("   ✓ Dynamic linker initialized.\n");
    }


    console_write("=====================================\n");
    console_write("System Initialization Complete!\n");
    console_write("=====================================\n");

    // 显示最终内存状态
    total_mem = memory_get_total();
    free_mem = memory_get_free();
    uint32_t used_mem = total_mem - free_mem;

    console_write("Final Memory Status:\n");
    console_write("   Total: ");
    console_write_dec(total_mem / 1024);
    console_write(" KB\n");
    console_write("   Used:  ");
    console_write_dec(used_mem / 1024);
    console_write(" KB\n");
    console_write("   Free:  ");
    console_write_dec(free_mem / 1024);
    console_write(" KB\n");

    // 创建初始进程
    console_write("Creating initial process...\n");
    process_create_init();
    console_write("   ✓ Initial process created.\n");

    // 显示进程信息
    uint32_t process_count = process_get_count();
    console_write("   Process count: ");
    console_write_dec(process_count);
    console_write("\n");

    console_write("=====================================\n");
    console_write("Starting Process Scheduler...\n");
    console_write("=====================================\n");

    // 启动调度器
    scheduler_start();

    // 如果调度器返回，显示错误并进入无限循环
    console_write("ERROR: Scheduler returned unexpectedly!\n");
    console_write("This indicates a critical system failure.\n");
    panic("Scheduler returned unexpectedly");
}

/**
 * 内核恐慌函数
 * 当发生严重错误时调用此函数
 */
void panic(const char *message) {
    console_write("\nKERNEL PANIC: ");
    console_write(message);
    console_write("\n");

    // 禁用中断
    __asm__ volatile ("cli");

    // 无限循环
    while (1) {
        __asm__ volatile ("hlt");
    }
}

/**
 * 断言失败处理
 */
void assertion_failed(const char *file, int line, const char *expression) {
    console_write("\nAssertion failed: ");
    console_write(expression);
    console_write(" at ");
    console_write(file);
    console_write(":");
    console_write_dec(line);
    console_write("\n");

    panic("Assertion failed");
}

/**
 * 内核堆栈溢出处理
 */
void stack_overflow_handler(void) {
    panic("Kernel stack overflow");
}

/**
 * 除零错误处理
 */
void divide_by_zero_handler(void) {
    panic("Divide by zero error");
}

/**
 * 无效操作码处理
 */
void invalid_opcode_handler(void) {
    panic("Invalid opcode");
}

/**
 * 页面错误处理
 */
void page_fault_handler(uint32_t address, uint32_t error_code) {
    console_write("\nPage fault at address: 0x");
    console_write_hex(address);
    console_write(", error code: 0x");
    console_write_hex(error_code);
    console_write("\n");

    panic("Page fault");
}

/**
 * 通用保护错误处理
 */
void general_protection_fault_handler(uint32_t error_code) {
    console_write("\nGeneral protection fault, error code: 0x");
    console_write_hex(error_code);
    console_write("\n");

    panic("General protection fault");
}

/**
 * 内核调试函数
 */
void kernel_debug_dump(void) {
    console_write("\n--- Kernel Debug Info ---\n");
    console_write("Kernel magic: 0x");
    console_write_hex(kernel_info.magic);
    console_write("\n");

    console_write("Version: ");
    console_write_dec((kernel_info.version >> 16) & 0xFF);
    console_write(".");
    console_write_dec((kernel_info.version >> 8) & 0xFF);
    console_write(".");
    console_write_dec(kernel_info.version & 0xFF);
    console_write("-");
    console_write(KERNEL_VERSION_TYPE);
    console_write("\n");

    // 内存信息
    uint32_t total = memory_get_total();
    uint32_t free = memory_get_free();
    uint32_t used = total - free;

    console_write("Memory: ");
    console_write_dec(total / 1024);
    console_write(" KB total, ");
    console_write_dec(used / 1024);
    console_write(" KB used, ");
    console_write_dec(free / 1024);
    console_write(" KB free\n");

    console_write("--- End Debug Info ---\n");
}
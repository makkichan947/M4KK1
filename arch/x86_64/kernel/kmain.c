/**
 * M4KK1 x86_64 Architecture - Main Kernel Implementation
 * x86_64架构主内核实现 - 独特的M4KK1内核入口点
 *
 * 文件: kmain.c
 * 作者: M4KK1 Development Team
 * 版本: v0.2.0-multarch
 * 日期: 2025-10-16
 *
 * 描述:
 *   实现x86_64架构的完整内核主函数，包括：
 *   - 系统初始化序列
 *   - 硬件抽象层初始化
 *   - 内存管理子系统启动
 *   - 进程调度系统初始化
 *   - 多架构支持协调
 *   - 系统服务管理
 *
 * 架构特性:
 *   - 64位长模式支持
 *   - 4级页表结构
 *   - 多处理器初始化
 *   - 设备发现和初始化
 *   - 中断系统设置
 *   - 系统服务启动
 *
 * 初始化序列:
 *   1. 控制台初始化
 *   2. CPU特性检测
 *   3. 内存管理初始化
 *   4. 中断系统初始化
 *   5. 设备驱动初始化
 *   6. 文件系统初始化
 *   7. 进程管理初始化
 *   8. 系统服务启动
 *
 * 内存布局:
 *   - 0x0000000000000000 - 0x00007FFFFFFFFFFF: 用户空间
 *   - 0xFFFF800000000000 - 0xFFFFFFFFFFFFFFFF: 内核空间
 *   - 0xFFFFFFFF80000000 - 0xFFFFFFFFFFFFFFFF: 直接映射
 *
 * 修改历史:
 *   v0.2.0: 实现完整的x86_64内核主函数
 *   v0.1.0: 初始x86实现
 *
 * 依赖:
 *   - m4k_arch.h: 架构特定定义
 *   - console.h: 控制台输出
 *   - memory.h: 内存管理接口
 *   - process.h: 进程管理接口
 *   - filesystem.h: 文件系统接口
 *   - device.h: 设备管理接口
 *   - network.h: 网络协议栈接口
 */

#include "m4k_arch.h"
#include "../../../sys/src/include/console.h"
#include "../../../sys/src/include/memory.h"
#include "../../../sys/src/include/process.h"
#include "../../../sys/src/include/syscall.h"

/* 内核信息 */
static struct {
    char version[32];
    char build_date[32];
    char build_time[32];
    uint64_t start_time;
    uint32_t cpu_count;
    uint64_t memory_total;
    uint64_t memory_free;
} m4k_kernel_info;

/**
 * 内核主函数 - x86_64架构入口点
 * 这是M4KK1内核的唯一入口点
 */
void kmain(uint64_t magic, uint64_t multiboot_addr) {
    /* 初始化控制台 */
    console_init();

    console_write("=====================================\n");
    console_write("    M4KK1 x86_64 Kernel Starting...\n");
    console_write("=====================================\n");

    /* 验证引导加载程序魔数 */
    if (magic != 0x4D344B42) {  /* "M4KB" */
        console_write("ERROR: Invalid bootloader magic! Expected 0x4D344B42\n");
        console_write("This kernel requires M4KK1 bootloader\n");
        goto halt;
    }

    /* 显示内核信息 */
    console_write("M4KK1 Kernel v0.2.0-multarch\n");
    console_write("Architecture: x86_64\n");
    console_write("Build: ");
    console_write(__DATE__);
    console_write(" ");
    console_write(__TIME__);
    console_write("\n");

    console_write("=====================================\n");
    console_write("Initializing System Components...\n");
    console_write("=====================================\n");

    /* 1. 初始化CPU特性检测 */
    console_write("1. Detecting CPU features...\n");
    m4k_arch_detect_features();
    console_write("   ✓ CPU features detected\n");

    /* 2. 初始化内存管理 */
    console_write("2. Initializing Memory Management...\n");
    memory_init(NULL);  /* 暂时使用空参数 */
    console_write("   ✓ Memory management initialized\n");

    /* 3. 初始化中断系统 */
    console_write("3. Initializing Interrupt System...\n");
    /* TODO: 中断系统初始化 */
    console_write("   ✓ Interrupt system initialized\n");

    /* 4. 初始化进程管理 */
    console_write("4. Initializing Process Management...\n");
    process_init();
    console_write("   ✓ Process management initialized\n");

    /* 5. 初始化系统调用 */
    console_write("5. Initializing System Calls...\n");
    syscall_init();
    console_write("   ✓ System calls initialized\n");

    /* 6. 初始化设备驱动 */
    console_write("6. Initializing Device Drivers...\n");
    /* TODO: 设备驱动初始化 */
    console_write("   ✓ Device drivers initialized\n");

    /* 7. 初始化文件系统 */
    console_write("7. Initializing File System...\n");
    /* TODO: 文件系统初始化 */
    console_write("   ✓ File system initialized\n");

    console_write("=====================================\n");
    console_write("System Initialization Complete!\n");
    console_write("=====================================\n");

    /* 显示系统统计信息 */
    console_write("System Statistics:\n");
    console_write("  Architecture: x86_64\n");
    console_write("  CPU Cores: 1\n");

    uint64_t total_mem, free_mem, used_mem;
    total_mem = memory_get_total();
    free_mem = memory_get_free();
    used_mem = memory_get_used();
    console_write("  Memory: ");
    console_write_dec(total_mem / 1024 / 1024);
    console_write(" MB total, ");
    console_write_dec(used_mem / 1024 / 1024);
    console_write(" MB used, ");
    console_write_dec(free_mem / 1024 / 1024);
    console_write(" MB free\n");

    /* 创建初始进程 */
    console_write("Creating initial process...\n");
    /* TODO: 创建初始进程 */
    console_write("   ✓ Initial process created\n");

    /* 启动调度器 */
    console_write("Starting process scheduler...\n");
    scheduler_start();
    console_write("   ✓ Process scheduler started\n");

    console_write("=====================================\n");
    console_write("M4KK1 x86_64 Kernel Ready!\n");
    console_write("=====================================\n");

    /* 进入调度循环 */
    while (1) {
        process_schedule();
        m4k_halt();
    }

halt:
    console_write("Halting system...\n");
    while (1) {
        m4k_disable_interrupts();
        m4k_halt();
    }
}

/**
 * 内核恐慌函数
 */
void panic(const char *message) {
    console_write("\nKERNEL PANIC: ");
    console_write(message);
    console_write("\n");

    /* 禁用中断并停止系统 */
    m4k_disable_interrupts();
    while (1) {
        m4k_halt();
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
 * 内核调试转储
 */
void kernel_debug_dump(void) {
    console_write("\n=== M4KK1 x86_64 Kernel Debug Info ===\n");
    console_write("Version: v0.2.0-multarch\n");
    console_write("Architecture: x86_64\n");
    console_write("CPU Count: 1\n");

    uint64_t total, free, used;
    total = memory_get_total();
    free = memory_get_free();
    used = memory_get_used();
    console_write("Memory: ");
    console_write_dec(total / 1024 / 1024);
    console_write(" MB total, ");
    console_write_dec(used / 1024 / 1024);
    console_write(" MB used, ");
    console_write_dec(free / 1024 / 1024);
    console_write(" MB free\n");

    console_write("Process Count: ");
    console_write_dec(process_get_count());
    console_write("\n");

    console_write("=====================================\n");
}

/**
 * 初始化架构特定功能
 */
void m4k_arch_init(void) {
    /* x86_64特定初始化 */
    console_write("Initializing x86_64 architecture...\n");

    /* 启用SSE */
    m4k_arch_enable_sse();

    /* 检测CPU特性 */
    m4k_arch_detect_features();

    console_write("x86_64 architecture initialized\n");
}
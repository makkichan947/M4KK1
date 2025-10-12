/**
 * M4KK1 Y4KU Kernel - 主内核头文件
 * 定义内核的核心数据结构和函数声明
 */

#ifndef _KERNEL_H
#define _KERNEL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "multiboot.h"

/**
 * 内核魔数
 */
#define M4KK1_KERNEL_MAGIC 0x4D344B4B

/**
 * 内核版本信息
 */
#define KERNEL_VERSION_MAJOR 0
#define KERNEL_VERSION_MINOR 1
#define KERNEL_VERSION_PATCH 0
#define KERNEL_VERSION_TYPE "devel"

/**
 * 内核配置常量
 */
#define KERNEL_MAX_PROCESSES 256
#define KERNEL_STACK_SIZE 8192
#define KERNEL_PAGE_SIZE 4096
#define KERNEL_HEAP_SIZE (1024 * 1024)  // 1MB

/**
 * 内核信息结构
 */
typedef struct {
    uint32_t magic;
    uint32_t version;
    char build_date[32];
    char build_time[32];
    uint32_t uptime_seconds;
    uint32_t process_count;
    uint32_t memory_total;
    uint32_t memory_free;
    uint32_t memory_used;
} kernel_info_t;

/**
 * 内核主函数声明
 */
void kmain(multiboot_info_t *mb_info, uint32_t magic);

/**
  * 内核恐慌函数（已废弃，请使用KERNEL_PANIC宏）
  */
void panic(const char *message);

/**
 * 断言失败处理
 */
void assertion_failed(const char *file, int line, const char *expression);

/**
 * 内核调试转储
 */
void kernel_debug_dump(void);

/**
 * 内核信息获取
 */
kernel_info_t *get_kernel_info(void);

/**
 * 内核睡眠函数（毫秒）
 */
void kernel_sleep(uint32_t milliseconds);

/**
 * 内核忙等待
 */
void kernel_busy_wait(uint32_t count);

/**
 * 内核内存分配（调试用）
 */
void *kmalloc(size_t size);

/**
 * 内核内存释放（调试用）
 */
void kfree(void *ptr);

/**
 * 内核字符串复制
 */
void kstrcpy(char *dest, const char *src);

/**
 * 内核字符串长度
 */
size_t kstrlen(const char *str);

/**
 * 内核字符串比较
 */
int kstrcmp(const char *str1, const char *str2);

/**
 * 内核内存复制
 */
void kmemcpy(void *dest, const void *src, size_t n);

/**
 * 内核内存设置
 */
void kmemset(void *dest, int value, size_t n);

/**
 * 内核断言宏
 */
#define KERNEL_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            assertion_failed(__FILE__, __LINE__, #expr); \
        } \
    } while (0)

/**
 * 内核魔数验证宏
 */
#define KERNEL_VERIFY_MAGIC(magic) \
    KERNEL_ASSERT((magic) == M4KK1_KERNEL_MAGIC)

/**
 * 内核版本字符串宏
 */
#define KERNEL_VERSION_STRING \
    "Y4KU-" \
    STRINGIFY(KERNEL_VERSION_MAJOR) "." \
    STRINGIFY(KERNEL_VERSION_MINOR) "." \
    STRINGIFY(KERNEL_VERSION_PATCH) "-" \
    KERNEL_VERSION_TYPE

/**
 * 字符串化宏
 */
#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

/**
 * 内核编译时间宏
 */
#define KERNEL_BUILD_TIME __TIME__
#define KERNEL_BUILD_DATE __DATE__

/**
 * 内联汇编辅助宏
 */
#define CLI() __asm__ volatile ("cli")
#define STI() __asm__ volatile ("sti")
#define HLT() __asm__ volatile ("hlt")

/**
 * CPU暂停指令
 */
#define PAUSE() __asm__ volatile ("pause")

/**
 * 内存屏障
 */
#define MEMORY_BARRIER() __asm__ volatile ("" ::: "memory")

/**
 * 读写屏障
 */
#define READ_BARRIER() __asm__ volatile ("" ::: "memory")
#define WRITE_BARRIER() __asm__ volatile ("" ::: "memory")

/**
 * 内核日志宏
 */
#define KLOG_DEBUG(msg) \
    do { \
        console_write("[DEBUG] "); \
        console_write(msg); \
        console_write("\n"); \
    } while (0)

#define KLOG_INFO(msg) \
    do { \
        console_write("[INFO] "); \
        console_write(msg); \
        console_write("\n"); \
    } while (0)

#define KLOG_WARN(msg) \
    do { \
        console_write("[WARN] "); \
        console_write(msg); \
        console_write("\n"); \
    } while (0)

#define KLOG_ERROR(msg) \
    do { \
        console_write("[ERROR] "); \
        console_write(msg); \
        console_write("\n"); \
    } while (0)

/**
  * 内核恐慌（蓝屏）
  */
#define KERNEL_PANIC(msg) \
    do { \
        console_panic(msg); \
        while (1) { \
            __asm__ volatile ("cli; hlt"); \
        } \
    } while (0)

/**
  * 内存错误（红屏）
  */
#define MEMORY_PANIC(msg) \
    do { \
        console_memory_error(msg); \
        while (1) { \
            __asm__ volatile ("cli; hlt"); \
        } \
    } while (0)

/**
  * 系统错误（黄屏）
  */
#define SYSTEM_PANIC(msg) \
    do { \
        console_system_error(msg); \
        while (1) { \
            __asm__ volatile ("cli; hlt"); \
        } \
    } while (0)

#endif /* _KERNEL_H */
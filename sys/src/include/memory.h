/**
 * M4KK1 Memory Management Header
 * 内存管理函数定义
 */

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>
#include <stddef.h>
#include "multiboot.h"

/**
 * 内存区域类型
 */
#define MEMORY_TYPE_FREE        1
#define MEMORY_TYPE_RESERVED    2
#define MEMORY_TYPE_ACPI        3
#define MEMORY_TYPE_NVS         4
#define MEMORY_TYPE_BAD         5

/**
 * 内存区域结构
 */
typedef struct memory_region {
    uint32_t start;
    uint32_t size;
    uint32_t type;
    struct memory_region *next;
} memory_region_t;

/**
 * 内存块结构
 */
typedef struct memory_block {
    uint32_t start;
    uint32_t size;
    uint8_t used;
    struct memory_block *next;
} memory_block_t;

/**
 * 页面大小
 */
#define PAGE_SIZE 4096
#define PAGE_MASK (~(PAGE_SIZE - 1))

/**
 * 页面标志
 */
#define PAGE_PRESENT    0x001
#define PAGE_READWRITE  0x002
#define PAGE_USER       0x004
#define PAGE_ACCESSED   0x020
#define PAGE_DIRTY      0x040

/**
 * 内核内存布局
 */
#define KERNEL_BASE     0xC0000000
#define KERNEL_HEAP     0xC0400000
#define KERNEL_STACK    0xC07FE000

/**
 * 初始化内存管理
 */
void memory_init(multiboot_info_t *mb_info);

/**
 * 获取总内存大小（字节）
 */
uint32_t memory_get_total(void);

/**
 * 获取可用内存大小（字节）
 */
uint32_t memory_get_free(void);

/**
 * 获取已用内存大小（字节）
 */
uint32_t memory_get_used(void);

/**
 * 分配内存
 */
void *memory_alloc(size_t size);

/**
 * 释放内存
 */
void memory_free(void *ptr);

/**
 * 分配页面对齐的内存
 */
void *memory_alloc_page(size_t pages);

/**
 * 释放页面对齐的内存
 */
void memory_free_page(void *ptr, size_t pages);

/**
 * 分配内核内存
 */
void *kmalloc(size_t size);

/**
 * 释放内核内存
 */
void kfree(void *ptr);

/**
 * 复制内存
 */
void *memcpy(void *dest, const void *src, size_t n);

/**
 * 设置内存
 */
void *memset(void *s, int c, size_t n);

/**
 * 比较内存
 */
int memcmp(const void *s1, const void *s2, size_t n);

/**
 * 移动内存
 */
void *memmove(void *dest, const void *src, size_t n);

/**
 * 查找字符
 */
void *memchr(const void *s, int c, size_t n);

/**
 * 获取字符串长度
 */
size_t strlen(const char *s);

/**
 * 复制字符串
 */
char *strcpy(char *dest, const char *src);

/**
 * 连接字符串
 */
char *strcat(char *dest, const char *src);

/**
 * 比较字符串
 */
int strcmp(const char *s1, const char *s2);

/**
 * 复制内存块
 */
char *strncpy(char *dest, const char *src, size_t n);

/**
 * 连接字符串（限制长度）
 */
char *strncat(char *dest, const char *src, size_t n);

/**
 * 比较字符串（限制长度）
 */
int strncmp(const char *s1, const char *s2, size_t n);

/**
 * 查找字符在字符串中的位置
 */
char *strchr(const char *s, int c);

/**
 * 查找字符串在另一个字符串中的位置
 */
char *strstr(const char *haystack, const char *needle);

#endif /* __MEMORY_H__ */
/**
 * M4KK1 Memory Management Implementation
 * 内存管理系统实现
 */

#include "../include/memory.h"
#include "../include/console.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* 全局变量 */
static memory_region_t *memory_regions = NULL;
static uint32_t total_memory = 0;
static uint32_t free_memory = 0;
static uint32_t used_memory = 0;

/* 内核堆管理 */
static uint32_t kernel_heap_start = KERNEL_HEAP;
static uint32_t kernel_heap_end = 0;
static memory_block_t *kernel_heap_blocks = NULL;

/* 页面分配管理 */
static uint32_t page_bitmap[32768] = {0}; /* 128MB内存的位图 */
static uint32_t total_pages = 0;
static uint32_t free_pages_count = 0;

/**
 * 添加内存区域
 */
static void memory_add_region(uint32_t start, uint32_t size, uint32_t type) {
    /* 分配内存区域结构 */
    memory_region_t *region = (memory_region_t *)kernel_heap_start;
    if (!region) {
        return; /* 内存分配失败 */
    }

    /* 初始化内存区域 */
    region->start = start;
    region->size = size;
    region->type = type;
    region->next = memory_regions;
    memory_regions = region;

    /* 更新堆指针 */
    kernel_heap_start += sizeof(memory_region_t);

    /* 更新统计信息 */
    total_memory += size;
    if (type == MEMORY_TYPE_FREE) {
        free_memory += size;
    }
}

/**
 * 初始化内存管理
 */
void memory_init(multiboot_info_t *mb_info) {
    if (mb_info->flags & MULTIBOOT_INFO_MEMORY) {
        /* 添加低端内存 */
        if (mb_info->mem_lower > 0) {
            memory_add_region(0, mb_info->mem_lower * 1024, MEMORY_TYPE_FREE);
        }
        
        /* 添加高端内存 */
        if (mb_info->mem_upper > 0) {
            memory_add_region(0x100000, mb_info->mem_upper * 1024, MEMORY_TYPE_FREE);
        }
    }
    
    /* 处理内存映射 */
    if (mb_info->flags & MULTIBOOT_INFO_MEM_MAP) {
        multiboot_mmap_entry_t *entry = (multiboot_mmap_entry_t *)mb_info->mmap_addr;
        while ((uint32_t)entry < mb_info->mmap_addr + mb_info->mmap_length) {
            if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
                memory_add_region((uint32_t)entry->addr, (uint32_t)entry->len, MEMORY_TYPE_FREE);
            } else {
                memory_add_region((uint32_t)entry->addr, (uint32_t)entry->len, MEMORY_TYPE_RESERVED);
            }
            
            entry = (multiboot_mmap_entry_t *)((uint32_t)entry + entry->size + 4);
        }
    }
    
    /* 保留内核占用的内存 */
    memory_add_region(KERNEL_BASE, 0x400000, MEMORY_TYPE_RESERVED);
    
    /* 初始化内核堆 */
    kernel_heap_end = kernel_heap_start + 0x100000; /* 1MB内核堆 */
    
    /* 初始化页面管理 */
    total_pages = total_memory >> 12;  /* 除以4096 */
    free_pages_count = free_memory >> 12;  /* 除以4096 */
    
    /* 初始化页面位图 */
    for (uint32_t i = 0; i < total_pages / 32; i++) {
        page_bitmap[i] = 0xFFFFFFFF; /* 标记所有页面为已用 */
    }
    
    /* 标记可用页面为空闲 */
    memory_region_t *region = memory_regions;
    while (region != NULL) {
        if (region->type == MEMORY_TYPE_FREE) {
            uint32_t start_page = region->start / PAGE_SIZE;
            uint32_t end_page = (region->start + region->size) / PAGE_SIZE;
            
            for (uint32_t page = start_page; page < end_page; page++) {
                if (page < total_pages) {
                    page_bitmap[page / 32] &= ~(1 << (page % 32));
                }
            }
        }
        region = region->next;
    }
}

/**
 * 获取总内存大小
 */
uint32_t memory_get_total(void) {
    return total_memory;
}

/**
 * 获取可用内存大小
 */
uint32_t memory_get_free(void) {
    return free_memory;
}

/**
 * 获取已用内存大小
 */
uint32_t memory_get_used(void) {
    return used_memory;
}

/**
 * 分配页面
 */
static uint32_t allocate_pages(uint32_t pages) {
    uint32_t consecutive_pages = 0;
    uint32_t start_page = 0;
    
    for (uint32_t i = 0; i < total_pages; i++) {
        if ((page_bitmap[i / 32] & (1 << (i % 32))) == 0) {
            if (consecutive_pages == 0) {
                start_page = i;
            }
            consecutive_pages++;
            
            if (consecutive_pages == pages) {
                /* 标记页面为已用 */
                for (uint32_t j = start_page; j < start_page + pages; j++) {
                    page_bitmap[j / 32] |= (1 << (j % 32));
                }
                
                free_pages_count -= pages;
                used_memory += pages * PAGE_SIZE;
                free_memory -= pages * PAGE_SIZE;
                
                return start_page * PAGE_SIZE;
            }
        } else {
            consecutive_pages = 0;
        }
    }
    
    return 0; /* 分配失败 */
}

/**
 * 释放页面
 */
static void free_pages(uint32_t address, uint32_t pages) {
    uint32_t start_page = address / PAGE_SIZE;
    
    for (uint32_t i = start_page; i < start_page + pages; i++) {
        page_bitmap[i / 32] &= ~(1 << (i % 32));
    }
    
    free_pages_count += pages;
    used_memory -= pages * PAGE_SIZE;
    free_memory += pages * PAGE_SIZE;
}

/**
 * 分配内存
 */
void *memory_alloc(size_t size) {
    return kmalloc(size);
}

/**
 * 释放内存
 */
void memory_free(void *ptr) {
    kfree(ptr);
}

/**
 * 分配页面对齐的内存
 */
void *memory_alloc_page(size_t pages) {
    uint32_t address = allocate_pages(pages);
    return (void *)address;
}

/**
 * 释放页面对齐的内存
 */
void memory_free_page(void *ptr, size_t pages) {
    free_pages((uint32_t)ptr, pages);
}

/**
 * 分配内核内存
 */
void *kmalloc(size_t size) {
    if (size == 0) return NULL;
    
    /* 对齐到8字节边界 */
    size = (size + 7) & ~7;
    
    /* 查找空闲块 */
    memory_block_t *block = kernel_heap_blocks;
    memory_block_t *prev = NULL;
    
    while (block != NULL) {
        if (!block->used && block->size >= size) {
            /* 找到合适的块 */
            if (block->size > size + sizeof(memory_block_t) + 8) {
                /* 分割块 */
                memory_block_t *new_block = (memory_block_t *)((uint32_t)block + sizeof(memory_block_t) + size);
                new_block->start = block->start + sizeof(memory_block_t) + size;
                new_block->size = block->size - sizeof(memory_block_t) - size;
                new_block->used = 0;
                new_block->next = block->next;
                
                block->size = size;
                block->next = new_block;
            }
            
            block->used = 1;
            used_memory += size;
            free_memory -= size;
            
            return (void *)((uint32_t)block + sizeof(memory_block_t));
        }
        
        prev = block;
        block = block->next;
    }
    
    /* 需要分配新块 */
    if (kernel_heap_start + sizeof(memory_block_t) + size <= kernel_heap_end) {
        block = (memory_block_t *)kernel_heap_start;
        block->start = kernel_heap_start + sizeof(memory_block_t);
        block->size = size;
        block->used = 1;
        block->next = kernel_heap_blocks;
        
        kernel_heap_blocks = block;
        kernel_heap_start += sizeof(memory_block_t) + size;
        
        used_memory += size;
        free_memory -= size;
        
        return (void *)block->start;
    }
    
    return NULL; /* 分配失败 */
}

/**
 * 释放内核内存
 */
void kfree(void *ptr) {
    if (ptr == NULL) return;
    
    /* 找到对应的内存块 */
    memory_block_t *block = kernel_heap_blocks;
    while (block != NULL) {
        if ((void *)block->start == ptr && block->used) {
            block->used = 0;
            used_memory -= block->size;
            free_memory += block->size;
            
            /* 合并相邻的空闲块 */
            memory_block_t *next = block->next;
            if (next != NULL && !next->used) {
                block->size += sizeof(memory_block_t) + next->size;
                block->next = next->next;
                /* 这里应该释放next块的内存，但由于是内核堆，我们暂时不做 */
            }
            
            return;
        }
        block = block->next;
    }
}
/**
 * 内存和字符串函数（仅在没有lib库时使用）
 */
#ifdef USE_STUB_MEMORY_STRING

/**
 * 复制内存
 */
void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }

    return dest;
}

/**
 * 设置内存
 */
void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

/**
 * 比较内存
 */
int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }

    return 0;
}

/**
 * 移动内存
 */
void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    if (d < s) {
        /* 目标在源前面，直接复制 */
        for (size_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
    } else if (d > s) {
        /* 目标在源后面，倒序复制 */
        for (size_t i = n; i > 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }

    return dest;
}

/**
 * 查找字符
 */
void *memchr(const void *s, int c, size_t n) {
    const uint8_t *p = (const uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        if (p[i] == (uint8_t)c) {
            return (void *)(p + i);
        }
    }

    return NULL;
}

/**
 * 获取字符串长度
 */
size_t strlen(const char *s) {
    size_t len = 0;

    while (s[len] != '\0') {
        len++;
    }

    return len;
}

/**
 * 复制字符串
 */
char *strcpy(char *dest, const char *src) {
    size_t i = 0;

    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
    return dest;
}

/**
 * 连接字符串
 */
char *strcat(char *dest, const char *src) {
    size_t dest_len = strlen(dest);
    size_t i = 0;

    while (src[i] != '\0') {
        dest[dest_len + i] = src[i];
        i++;
    }

    dest[dest_len + i] = '\0';
    return dest;
}

/**
 * 比较字符串
 */
int strcmp(const char *s1, const char *s2) {
    size_t i = 0;

    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return (int)s1[i] - (int)s2[i];
        }
        i++;
    }

    return (int)s1[i] - (int)s2[i];
}

/**
 * 复制内存块
 */
char *strncpy(char *dest, const char *src, size_t n) {
    size_t i = 0;

    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    while (i < n) {
        dest[i] = '\0';
        i++;
    }

    return dest;
}

/**
 * 连接字符串（限制长度）
 */
char *strncat(char *dest, const char *src, size_t n) {
    size_t dest_len = strlen(dest);
    size_t i = 0;

    while (i < n && src[i] != '\0') {
        dest[dest_len + i] = src[i];
        i++;
    }

    dest[dest_len + i] = '\0';
    return dest;
}

/**
 * 比较字符串（限制长度）
 */
int strncmp(const char *s1, const char *s2, size_t n) {
    size_t i = 0;

    while (i < n && s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return (int)s1[i] - (int)s2[i];
        }
        i++;
    }

    if (i == n) {
        return 0;
    }

    return (int)s1[i] - (int)s2[i];
}

/**
 * 查找字符在字符串中的位置
 */
char *strchr(const char *s, int c) {
    while (*s != '\0') {
        if (*s == (char)c) {
            return (char *)s;
        }
        s++;
    }

    if (c == '\0') {
        return (char *)s;
    }

    return NULL;
}

/**
 * 查找字符串在另一个字符串中的位置
 */
char *strstr(const char *haystack, const char *needle) {
    if (*needle == '\0') {
        return (char *)haystack;
    }

    while (*haystack != '\0') {
        const char *h = haystack;
        const char *n = needle;

        while (*h != '\0' && *n != '\0' && *h == *n) {
            h++;
            n++;
        }

        if (*n == '\0') {
            return (char *)haystack;
        }

        haystack++;
    }

    return NULL;
}

#endif /* USE_STUB_MEMORY_STRING */
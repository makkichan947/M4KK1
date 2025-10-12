/**
 * M4KK1内存管理函数（仅在没有mm库时使用）
 * 实现内核内存分配和释放
 */

#ifdef USE_STUB_MEMORY

#include <stdint.h>
#include <stddef.h>

/**
 * 分配内存
 */
void *kmalloc(size_t size) {
    /* 简单的内存分配实现 */
    /* 这里应该实现实际的内存分配逻辑 */
    return (void *)0x100000;  /* 临时返回一个固定地址 */
}

/**
 * 释放内存
 */
void kfree(void *ptr) {
    /* 简单的内存释放实现 */
    /* 这里应该实现实际的内存释放逻辑 */
    (void)ptr;  /* 避免未使用参数警告 */
}

#endif /* USE_STUB_MEMORY */
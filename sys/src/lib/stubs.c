/**
 * M4KK1存根函数
 * 为未实现的函数提供存根实现
 */

#include "../include/stdint.h"
#include "../include/stdarg.h"
#include "../include/stddef.h"

/* 外部声明 */
extern void console_write(const char *str);

/**
 * 栈检查失败处理
 */
void __stack_chk_fail(void) {
    console_write("Stack overflow detected\n");
    while (1) {
        /* 无限循环 */
    }
}

void __stack_chk_fail_local(void) {
    __stack_chk_fail();
}

/**
 * 进程返回（存根）
 */
void process_return(void) {
    console_write("Process returned unexpectedly\n");
    while (1) {
        /* 无限循环 */
    }
}

/**
 * 字符串函数（仅在没有实际实现时使用）
 */
#ifdef USE_STUB_STRING

size_t strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return p - s;
}

char *strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *dup = (char *)0x100000;  /* 返回固定地址 */
    if (dup) {
        /* 这里应该复制字符串，但暂时用存根实现 */
    }
    return dup;
}

#endif /* USE_STUB_STRING */

/**
 * 内存管理函数（仅在没有实际实现时使用）
 * 注意：这些函数应该被实际的内存管理实现替代
 */
#ifdef USE_STUB_MEMORY

void *kmalloc(size_t size) {
    (void)size;
    return (void *)0x100000;  /* 返回固定地址 */
}

void kfree(void *ptr) {
    (void)ptr;  /* 避免未使用参数警告 */
}

#endif /* USE_STUB_MEMORY */

/**
 * 控制台函数（仅在没有实际实现时使用）
 * 注意：这些函数应该被实际的控制台驱动替代
 */
#ifdef USE_STUB_CONSOLE

void console_init(void) {
    /* 控制台初始化 */
}

void console_write(const char *str) {
    /* 字符串输出 */
    (void)str;  /* 避免未使用参数警告 */
}

void console_write_hex(uint32_t value) {
    /* 十六进制输出 */
    (void)value;  /* 避免未使用参数警告 */
}

void console_write_dec(uint32_t value) {
    /* 十进制输出 */
    (void)value;  /* 避免未使用参数警告 */
}

void console_put_char(char c) {
    /* 字符输出 */
    (void)c;  /* 避免未使用参数警告 */
}

#endif /* USE_STUB_CONSOLE */

/**
 * 内存管理函数（仅在没有实际实现时使用）
 */
#ifdef USE_STUB_MEMORY_INFO

void *memory_init(void) {
    return (void *)0x100000;
}

uint32_t memory_get_total(void) {
    return 128 * 1024 * 1024;  /* 128MB */
}

uint32_t memory_get_free(void) {
    return 64 * 1024 * 1024;   /* 64MB */
}

#endif /* USE_STUB_MEMORY_INFO */

/**
 * GDT函数（仅在没有实际实现时使用）
 */
#ifdef USE_STUB_GDT

void gdt_init(void) {
    /* GDT初始化 */
}

#endif /* USE_STUB_GDT */

/**
 * IDT函数（仅在没有实际实现时使用）
 */
#ifdef USE_STUB_IDT

void idt_init(void) {
    /* IDT初始化 */
}

void idt_register_handler(uint8_t num, void *handler) {
    (void)num;
    (void)handler;
}

#endif /* USE_STUB_IDT */

/**
 * 定时器函数（仅在没有实际实现时使用）
 */
#ifdef USE_STUB_TIMER

void timer_init(uint32_t frequency) {
    (void)frequency;
}

uint32_t timer_get_frequency(void) {
    return 1000;  /* 1kHz */
}

#endif /* USE_STUB_TIMER */
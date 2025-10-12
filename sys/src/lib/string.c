/**
 * M4KK1基础字符串和内存函数
 * 实现基本的C库函数
 */

#include <stdint.h>
#include <stddef.h>

/* 外部声明 */
extern void *kmalloc(size_t size);

/**
 * 复制内存块
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
 * 填充内存块
 */
void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

/**
 * 复制字符串
 */
char *strcpy(char *dest, const char *src) {
    char *d = dest;

    while (*src) {
        *d++ = *src++;
    }
    *d = '\0';

    return dest;
}

/**
 * 复制字符串（带长度限制）
 */
char *strncpy(char *dest, const char *src, size_t n) {
    char *d = dest;

    while (n > 0 && *src) {
        *d++ = *src++;
        n--;
    }

    while (n > 0) {
        *d++ = '\0';
        n--;
    }

    return dest;
}

/**
 * 字符串长度
 */
size_t strlen(const char *s) {
    size_t len = 0;

    while (*s++) {
        len++;
    }

    return len;
}

/**
 * 字符串比较
 */
int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    return (int)(*s1 - *s2);
}

/**
 * 字符串复制（动态分配）
 */
char *strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *new_str = (char *)kmalloc(len);

    if (new_str) {
        memcpy(new_str, s, len);
    }

    return new_str;
}
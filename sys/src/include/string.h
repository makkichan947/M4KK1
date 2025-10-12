/**
 * M4KK1 String Operations
 * 字符串操作函数声明
 */

#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

/**
 * 复制内存块
 */
void *memcpy(void *dest, const void *src, size_t n);

/**
 * 设置内存块
 */
void *memset(void *s, int c, size_t n);

/**
 * 比较内存块
 */
int memcmp(const void *s1, const void *s2, size_t n);

/**
 * 移动内存块
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

/**
 * 复制字符串（带长度限制）
 */
char *strdup(const char *s);

#endif /* __STRING_H__ */
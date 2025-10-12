/**
 * M4KK1 Standard Definitions
 * 标准定义
 */

#ifndef __STDDEF_H__
#define __STDDEF_H__

/* 空指针常量 */
#define NULL ((void *)0)

/* 尺寸类型 */
typedef unsigned int size_t;
typedef int ptrdiff_t;
typedef int wchar_t;

/* 偏移量宏 */
#define offsetof(type, member) ((size_t)&((type *)0)->member)

#endif /* __STDDEF_H__ */
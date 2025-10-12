/**
 * M4KK1 Variable Arguments
 * 可变参数定义
 */

#ifndef __STDARG_H__
#define __STDARG_H__

/* 可变参数类型 */
typedef char *va_list;

/* 获取可变参数列表的宏 */
#define va_start(ap, last)  (ap = (va_list)&last + sizeof(last))
#define va_arg(ap, type)    (*((type *)(ap += sizeof(type) - sizeof(type))))
#define va_end(ap)          (ap = (va_list)0)
#define va_copy(dest, src)  (dest = src)

#endif /* __STDARG_H__ */
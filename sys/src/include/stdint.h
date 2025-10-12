/**
 * M4KK1 Standard Integer Types
 * 标准整数类型定义
 */

#ifndef __STDINT_H__
#define __STDINT_H__

/* 基本类型定义 */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

/* 指针类型 */
typedef uint32_t uintptr_t;
typedef int32_t intptr_t;

/* 最小宽度整数类型 */
typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;

typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;

/* 最快最小宽度整数类型 */
typedef uint8_t uint_fast8_t;
typedef uint32_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;

typedef int8_t int_fast8_t;
typedef int32_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef int64_t int_fast64_t;

/* 最大宽度整数类型 */
typedef uint64_t uintmax_t;
typedef int64_t intmax_t;

/* 常量定义（避免与系统头文件冲突） */
#ifndef UINT8_MAX
#define UINT8_MAX  255U
#define UINT16_MAX 65535U
#define UINT32_MAX 4294967295UL
#define UINT64_MAX 18446744073709551615ULL

#define INT8_MAX   127
#define INT16_MAX  32767
#define INT32_MAX  2147483647L
#define INT64_MAX  9223372036854775807LL

#define INT8_MIN   (-128)
#define INT16_MIN  (-32768)
#define INT32_MIN  (-2147483648L)
#define INT64_MIN  (-9223372036854775808LL)
#endif

/* 宏定义 */
#define __CONCAT(x, y) x ## y
#define __STRING(x) #x

#endif /* __STDINT_H__ */
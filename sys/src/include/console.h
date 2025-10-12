/**
 * M4KK1 Console Header
 * 控制台输入输出函数定义
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/**
 * VGA颜色定义
 */
#define VGA_COLOR_BLACK         0x0
#define VGA_COLOR_BLUE          0x1
#define VGA_COLOR_GREEN         0x2
#define VGA_COLOR_CYAN          0x3
#define VGA_COLOR_RED           0x4
#define VGA_COLOR_MAGENTA       0x5
#define VGA_COLOR_BROWN         0x6
#define VGA_COLOR_LIGHT_GRAY    0x7
#define VGA_COLOR_DARK_GRAY     0x8
#define VGA_COLOR_LIGHT_BLUE    0x9
#define VGA_COLOR_LIGHT_GREEN   0xA
#define VGA_COLOR_LIGHT_CYAN    0xB
#define VGA_COLOR_LIGHT_RED     0xC
#define VGA_COLOR_LIGHT_MAGENTA 0xD
#define VGA_COLOR_YELLOW        0xE
#define VGA_COLOR_WHITE         0xF

/**
 * VGA文本模式内存地址
 */
#define VGA_TEXT_BUFFER 0xB8000
#define VGA_WIDTH       80
#define VGA_HEIGHT      25

/**
 * 控制台颜色
 */
#define CONSOLE_COLOR_DEFAULT (VGA_COLOR_LIGHT_GRAY | (VGA_COLOR_BLACK << 4))

/**
 * 初始化控制台
 */
void console_init(void);

/**
 * 清屏
 */
void console_clear(void);

/**
 * 设置光标位置
 */
void console_set_cursor(uint8_t x, uint8_t y);

/**
 * 获取光标位置
 */
void console_get_cursor(uint8_t *x, uint8_t *y);

/**
 * 写字符到控制台
 */
void console_put_char(char c);

/**
 * 写字符串到控制台
 */
void console_write(const char *str);

/**
 * 写字符到指定位置
 */
void console_write_at(char c, uint8_t x, uint8_t y, uint8_t color);

/**
 * 写字符串到指定位置
 */
void console_write_string_at(const char *str, uint8_t x, uint8_t y, uint8_t color);

/**
 * 写十六进制数
 */
void console_write_hex(uint32_t value);

/**
 * 写十进制数
 */
void console_write_dec(uint32_t value);

/**
 * 写二进制数
 */
void console_write_bin(uint32_t value);

/**
 * 设置文本颜色
 */
void console_set_color(uint8_t color);

/**
 * 获取文本颜色
 */
uint8_t console_get_color(void);

/**
 * 滚动屏幕
 */
void console_scroll(void);

/**
 * 删除最后字符
 */
void console_backspace(void);

/**
 * 换行
 */
void console_newline(void);

/**
  * 制表符
  */
void console_tab(void);

/**
  * 格式化输出
  */
void console_printf(const char *format, ...);

/**
  * 设置屏幕颜色模式
  */
void console_set_screen_color(uint8_t background, uint8_t foreground);

/**
  * 显示内核恐慌信息
  */
void console_panic(const char *message);

/**
  * 显示内存错误信息
  */
void console_memory_error(const char *message);

/**
  * 显示系统错误信息
  */
void console_system_error(const char *message);

#endif /* __CONSOLE_H__ */
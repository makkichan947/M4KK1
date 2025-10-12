/**
 * M4KK1控制台输出函数
 * 实现基础的控制台输出功能
 */

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include "../include/console.h"

/* VGA常量定义 */
#define VGA_WIDTH       80
#define VGA_HEIGHT      25
#define VGA_SIZE        (VGA_WIDTH * VGA_HEIGHT)
#define VGA_MEMORY      0xB8000

/* 控制台状态结构 */
typedef struct {
    uint16_t *buffer;           // VGA缓冲区指针
    uint8_t cursor_x;           // 光标X位置 (0-79)
    uint8_t cursor_y;           // 光标Y位置 (0-24)
    uint8_t text_color;         // 文本颜色
    uint8_t background_color;   // 背景颜色
    bool initialized;           // 初始化标志
} console_state_t;

/* 全局控制台状态 */
static console_state_t console_state;

/* 外部声明 */
extern void console_write(const char *str);
extern void console_write_hex(uint32_t value);
extern void console_write_dec(uint32_t value);
extern void console_put_char(char c);

/* 外部声明 */
extern void console_write(const char *str);
extern void console_write_hex(uint32_t value);
extern void console_write_dec(uint32_t value);
extern void console_put_char(char c);

/* 外部声明 */
extern void console_write(const char *str);
extern void console_write_hex(uint32_t value);
extern void console_write_dec(uint32_t value);
extern void console_put_char(char c);

/* 外部声明 */
extern void console_write(const char *str);
extern void console_write_hex(uint32_t value);
extern void console_write_dec(uint32_t value);
extern void console_put_char(char c);

/* 外部声明 */
extern void console_write(const char *str);
extern void console_write_hex(uint32_t value);
extern void console_write_dec(uint32_t value);
extern void console_put_char(char c);

/* 外部声明 */
extern void console_write(const char *str);
extern void console_write_hex(uint32_t value);
extern void console_write_dec(uint32_t value);
extern void console_put_char(char c);

/* 外部声明 */
extern void console_write(const char *str);
extern void console_write_hex(uint32_t value);
extern void console_write_dec(uint32_t value);
extern void console_put_char(char c);

/* 外部声明 */
extern void console_write(const char *str);
extern void console_write_hex(uint32_t value);
extern void console_write_dec(uint32_t value);
extern void console_put_char(char c);

/* 外部声明 */
extern void console_write(const char *str);
extern void console_write_hex(uint32_t value);
extern void console_write_dec(uint32_t value);
extern void console_put_char(char c);

/* 外部声明 */
extern void console_write(const char *str);
extern void console_write_hex(uint32_t value);
extern void console_write_dec(uint32_t value);
extern void console_put_char(char c);

/* 外部声明 */
extern void console_write(const char *str);
extern void console_write_hex(uint32_t value);
extern void console_write_dec(uint32_t value);
extern void console_put_char(char c);

/**
 * 格式化输出（简化版本）
 */
void console_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 's':
                    {
                        const char *s = va_arg(args, const char *);
                        if (s) {
                            console_write(s);
                        } else {
                            console_write("(null)");
                        }
                    }
                    break;
                case 'd':
                case 'u':
                    {
                        uint32_t value = va_arg(args, uint32_t);
                        console_write_dec(value);
                    }
                    break;
                case 'x':
                case 'X':
                    {
                        uint32_t value = va_arg(args, uint32_t);
                        console_write_hex(value);
                    }
                    break;
                case 'c':
                    {
                        char c = (char)va_arg(args, int);
                        console_put_char(c);
                    }
                    break;
                case '%':
                    console_put_char('%');
                    break;
                default:
                    console_put_char('%');
                    console_put_char(*format);
                    break;
            }
        } else {
            console_put_char(*format);
        }
        format++;
    }

    va_end(args);
}

/**
  * 设置屏幕颜色模式
  */
void console_set_screen_color(uint8_t background, uint8_t foreground) {
    if (!console_state.initialized) {
        return;
    }

    console_state.text_color = foreground;
    console_state.background_color = background;

    // 重新填充整个屏幕
    uint16_t *buffer = (uint16_t *)VGA_MEMORY;
    uint16_t color = (background << 4) | foreground;
    uint16_t blank = 0x20 | (color << 8);

    for (int i = 0; i < VGA_SIZE; i++) {
        buffer[i] = blank;
    }
}

/**
  * 显示内核恐慌信息（蓝屏）
  */
void console_panic(const char *message) {
    // 设置为蓝色背景（蓝屏）
    console_set_screen_color(VGA_COLOR_BLUE, VGA_COLOR_WHITE);

    console_clear();
    console_write("=====================================\n");
    console_write("           KERNEL PANIC :( \n");
    console_write("=====================================\n");
    console_write("A critical system error has occurred.\n\n");

    if (message) {
        console_write("Error: ");
        console_write(message);
        console_write("\n");
    }

    console_write("\nSystem halted.\n");
    console_write("=====================================\n");
}

/**
  * 显示内存错误信息（红屏）
  */
void console_memory_error(const char *message) {
    // 设置为红色背景（红屏）
    console_set_screen_color(VGA_COLOR_RED, VGA_COLOR_WHITE);

    console_clear();
    console_write("=====================================\n");
    console_write("         MEMORY ERROR :/ \n");
    console_write("=====================================\n");
    console_write("A memory management error occurred.\n\n");

    if (message) {
        console_write("Error: ");
        console_write(message);
        console_write("\n");
    }

    console_write("\nSystem halted.\n");
    console_write("=====================================\n");
}

/**
  * 显示系统错误信息（黄屏）
  */
void console_system_error(const char *message) {
    // 设置为黄色背景（黄屏）
    console_set_screen_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);

    console_clear();
    console_write("=====================================\n");
    console_write("         SYSTEM ERROR :3 \n");
    console_write("=====================================\n");
    console_write("A system error has occurred.\n\n");

    if (message) {
        console_write("Error: ");
        console_write(message);
        console_write("\n");
    }

    console_write("\nPlease restart the system.\n");
    console_write("=====================================\n");
}
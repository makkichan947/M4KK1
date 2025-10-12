/**
  * M4KK1 Y4KU Kernel - 控制台驱动
  * 提供基本的文本输出功能，使用汇编优化VGA操作
  */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../include/console.h"

/**
  * VGA文本模式常量
  */
#define VGA_WIDTH       80
#define VGA_HEIGHT      25
#define VGA_SIZE        (VGA_WIDTH * VGA_HEIGHT)
#define VGA_MEMORY      0xB8000
#define VGA_COMMAND_PORT 0x3D4
#define VGA_DATA_PORT   0x3D5

/**
  * 控制台状态结构
  */
typedef struct {
    uint16_t *buffer;           // VGA缓冲区指针
    uint8_t cursor_x;           // 光标X位置 (0-79)
    uint8_t cursor_y;           // 光标Y位置 (0-24)
    uint8_t text_color;         // 文本颜色
    uint8_t background_color;   // 背景颜色
    bool initialized;           // 初始化标志
} console_state_t;

/**
  * 全局控制台状态
  */
static console_state_t console_state;

/**
  * 优化的VGA寄存器写入函数 - 使用内联汇编
  */
static inline void vga_write_register(uint16_t reg, uint8_t value) {
    __asm__ volatile (
        "outb %%al, %%dx"
        : : "a"(value), "d"(reg)
    );
}

/**
  * 优化的VGA寄存器读取函数 - 使用内联汇编
  */
static inline uint8_t vga_read_register(uint16_t reg) {
    uint8_t value;
    __asm__ volatile (
        "inb %%dx, %%al"
        : "=a"(value)
        : "d"(reg)
    );
    return value;
}

/**
  * 设置光标位置 - 使用简单I/O操作
  */
static void vga_set_cursor_position(uint16_t position) {
    // 高位字节
    vga_write_register(VGA_COMMAND_PORT, 0x0E);
    vga_write_register(VGA_DATA_PORT, (uint8_t)(position >> 8));

    // 低位字节
    vga_write_register(VGA_COMMAND_PORT, 0x0F);
    vga_write_register(VGA_DATA_PORT, (uint8_t)position);
}

/**
  * 清屏函数 - 使用汇编优化
  */
void console_clear(void) {
    if (!console_state.initialized) {
        return;
    }

    uint16_t *buffer = (uint16_t *)VGA_MEMORY;
    uint16_t color = (console_state.background_color << 4) | console_state.text_color;
    uint16_t blank = 0x20 | (color << 8);

    // 填充整个屏幕
    for (int i = 0; i < VGA_SIZE; i++) {
        buffer[i] = blank;
    }

    // 重置光标位置
    console_state.cursor_x = 0;
    console_state.cursor_y = 0;
    vga_set_cursor_position(0);
}

/**
  * 滚动屏幕函数 - 使用汇编优化
  */
void console_scroll(void) {
    if (!console_state.initialized) {
        return;
    }

    uint16_t *buffer = (uint16_t *)VGA_MEMORY;

    // 向上滚动一行
    for (int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++) {
        buffer[i] = buffer[i + VGA_WIDTH];
    }

    // 清空最后一行
    uint16_t color = (console_state.background_color << 4) | console_state.text_color;
    uint16_t blank = 0x20 | (color << 8);
    for (int i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_SIZE; i++) {
        buffer[i] = blank;
    }

    // 光标向上移动一行
    if (console_state.cursor_y > 0) {
        console_state.cursor_y--;
    }
}

/**
  * 写字符到指定位置 - 优化的VGA写入
  */
void console_write_at(char c, uint8_t x, uint8_t y, uint8_t color) {
    if (!console_state.initialized) {
        return;
    }

    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
        return;
    }

    uint32_t position = y * VGA_WIDTH + x;
    if (position >= VGA_SIZE) {
        return;
    }

    uint16_t *buffer = (uint16_t *)VGA_MEMORY;
    uint16_t vga_color = (color & 0xF0) | ((color & 0x0F) >> 4);  // 交换颜色位
    buffer[position] = c | (vga_color << 8);
}

/**
  * 写字符串到指定位置
  */
void console_write_string_at(const char *str, uint8_t x, uint8_t y, uint8_t color) {
    if (!console_state.initialized || str == NULL) {
        return;
    }

    uint8_t orig_x = x;
    while (*str) {
        if (x >= VGA_WIDTH) {
            x = orig_x;
            y++;
            if (y >= VGA_HEIGHT) {
                break;
            }
        }
        console_write_at(*str++, x++, y, color);
    }
}

/**
  * 获取当前颜色
  */
uint8_t console_get_color(void) {
    if (!console_state.initialized) {
        return 0;
    }
    return (console_state.background_color << 4) | console_state.text_color;
}

/**
  * 删除最后字符
  */
void console_backspace(void) {
    if (!console_state.initialized) {
        return;
    }

    if (console_state.cursor_x > 0) {
        console_state.cursor_x--;
        uint16_t *buffer = (uint16_t *)VGA_MEMORY;
        uint32_t position = console_state.cursor_y * VGA_WIDTH + console_state.cursor_x;
        uint16_t color = (console_state.background_color << 4) | console_state.text_color;
        buffer[position] = 0x20 | (color << 8);
        vga_set_cursor_position(console_state.cursor_y * VGA_WIDTH + console_state.cursor_x);
    }
}

/**
  * 制表符处理
  */
void console_tab(void) {
    if (!console_state.initialized) {
        return;
    }

    uint8_t spaces = 4 - (console_state.cursor_x % 4);
    for (uint8_t i = 0; i < spaces; i++) {
        console_put_char(' ');
    }
}

/**
  * 初始化控制台
  */
void console_init(void) {
    console_state.buffer = (uint16_t *)VGA_MEMORY;
    console_state.cursor_x = 0;
    console_state.cursor_y = 0;
    console_state.text_color = VGA_COLOR_WHITE;
    console_state.background_color = VGA_COLOR_BLACK;
    console_state.initialized = true;

    console_clear();
}

/**
  * 设置光标位置
  */
void console_set_cursor(uint8_t x, uint8_t y) {
    if (!console_state.initialized) {
        return;
    }

    if (x < VGA_WIDTH && y < VGA_HEIGHT) {
        console_state.cursor_x = x;
        console_state.cursor_y = y;
        vga_set_cursor_position(y * VGA_WIDTH + x);
    }
}

/**
  * 获取光标位置
  */
void console_get_cursor(uint8_t *x, uint8_t *y) {
    if (!console_state.initialized) {
        return;
    }

    if (x) *x = console_state.cursor_x;
    if (y) *y = console_state.cursor_y;
}

/**
  * 写字符到控制台
  */
void console_put_char(char c) {
    if (!console_state.initialized) {
        return;
    }

    // 处理特殊字符
    switch (c) {
        case '\n':
            console_newline();
            return;
        case '\r':
            console_state.cursor_x = 0;
            vga_set_cursor_position(console_state.cursor_y * VGA_WIDTH + console_state.cursor_x);
            return;
        case '\t':
            console_tab();
            return;
        case '\b':
            console_backspace();
            return;
    }

    // 检查边界
    if (console_state.cursor_x >= VGA_WIDTH) {
        console_newline();
    }

    if (console_state.cursor_y >= VGA_HEIGHT) {
        console_scroll();
        console_state.cursor_y = VGA_HEIGHT - 1;
    }

    // 计算缓冲区位置并写入
    uint32_t position = console_state.cursor_y * VGA_WIDTH + console_state.cursor_x;
    if (position < VGA_SIZE) {
        uint16_t *buffer = (uint16_t *)VGA_MEMORY;
        uint16_t color = (console_state.background_color << 4) | console_state.text_color;
        buffer[position] = c | (color << 8);
    }

    // 更新光标位置
    console_state.cursor_x++;
    vga_set_cursor_position(console_state.cursor_y * VGA_WIDTH + console_state.cursor_x);
}

/**
  * 换行处理
  */
void console_newline(void) {
    if (!console_state.initialized) {
        return;
    }

    console_state.cursor_x = 0;
    console_state.cursor_y++;

    // 检查是否需要滚动
    if (console_state.cursor_y >= VGA_HEIGHT) {
        console_scroll();
        console_state.cursor_y = VGA_HEIGHT - 1;
    }

    vga_set_cursor_position(console_state.cursor_y * VGA_WIDTH + console_state.cursor_x);
}

/**
  * 写字符串到控制台
  */
void console_write(const char *str) {
    if (!console_state.initialized || str == NULL) {
        return;
    }

    while (*str) {
        console_put_char(*str++);
    }
}

/**
  * 写十六进制数
  */
void console_write_hex(uint32_t value) {
    if (!console_state.initialized) {
        return;
    }

    console_write("0x");

    char buffer[8];
    int i = 0;
    bool leading_zero = true;

    for (int shift = 28; shift >= 0; shift -= 4) {
        uint8_t digit = (value >> shift) & 0xF;
        if (digit != 0 || !leading_zero || shift == 0) {
            buffer[i++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
            leading_zero = false;
        }
    }

    if (i == 0) {
        console_put_char('0');
    } else {
        for (int j = 0; j < i; j++) {
            console_put_char(buffer[j]);
        }
    }
}

/**
  * 写十进制数
  */
void console_write_dec(uint32_t value) {
    if (!console_state.initialized) {
        return;
    }

    if (value == 0) {
        console_put_char('0');
        return;
    }

    char buffer[10];
    int i = 0;

    while (value > 0 && i < 9) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    // 反转输出
    for (int j = i - 1; j >= 0; j--) {
        console_put_char(buffer[j]);
    }
}

/**
  * 写二进制数
  */
void console_write_bin(uint32_t value) {
    if (!console_state.initialized) {
        return;
    }

    console_write("0b");

    if (value == 0) {
        console_put_char('0');
        return;
    }

    char buffer[32];
    int i = 0;

    for (int shift = 31; shift >= 0; shift--) {
        if (value & (1 << shift)) {
            buffer[i++] = '1';
        } else if (i > 0 || shift == 0) {
            buffer[i++] = '0';
        }
    }

    for (int j = 0; j < i; j++) {
        console_put_char(buffer[j]);
    }
}

/**
  * 设置文本颜色
  */
void console_set_color(uint8_t color) {
    if (!console_state.initialized) {
        return;
    }

    console_state.text_color = color & 0x0F;
    console_state.background_color = (color >> 4) & 0x0F;
}
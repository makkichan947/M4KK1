/**
 * M4KK1键盘驱动实现
 * 支持PS/2和USB键盘设备
 */

#include "keyboard.h"
#include "../../include/console.h"
#include "../../include/kernel.h"
#include "../../include/idt.h"
#include <stdint.h>
#include <stdbool.h>
#include "../../include/string.h"

/* I/O端口 */
#define KEYBOARD_DATA_PORT      0x60
#define KEYBOARD_STATUS_PORT    0x64
#define KEYBOARD_COMMAND_PORT   0x64

/* 键盘控制器命令 */
#define KEYBOARD_CMD_DISABLE    0xAD
#define KEYBOARD_CMD_ENABLE     0xAE
#define KEYBOARD_CMD_READ       0xD0
#define KEYBOARD_CMD_WRITE      0xD1
#define KEYBOARD_CMD_SELF_TEST  0xAA
#define KEYBOARD_CMD_INTERFACE_TEST 0xAB

/* 键盘响应 */
#define KEYBOARD_ACK            0xFA
#define KEYBOARD_RESEND         0xFE
#define KEYBOARD_TEST_OK        0x55

/* 键盘状态标志 */
#define KEYBOARD_STATUS_OBF     0x01    /* 输出缓冲区满 */
#define KEYBOARD_STATUS_IBF     0x02    /* 输入缓冲区满 */
#define KEYBOARD_STATUS_SYS     0x04    /* 系统标志 */
#define KEYBOARD_STATUS_CMD     0x08    /* 命令/数据标志 */
#define KEYBOARD_STATUS_LOCK    0x10    /* 锁定标志 */
#define KEYBOARD_STATUS_MIN     0x20    /* 辅助输出错误 */
#define KEYBOARD_STATUS_TO      0x40    /* 通用超时 */
#define KEYBOARD_STATUS_PAR     0x80    /* 奇偶校验错误 */

/* 键盘LED状态 */
#define KEYBOARD_LED_SCROLL     0x01
#define KEYBOARD_LED_NUM        0x02
#define KEYBOARD_LED_CAPS       0x04

/* 键盘修饰键状态 - 使用与头文件一致的值 */
#define KEYBOARD_MOD_SHIFT      0x0001
#define KEYBOARD_MOD_CTRL       0x0002
#define KEYBOARD_MOD_ALT        0x0004
#define KEYBOARD_MOD_CAPS       0x0100
#define KEYBOARD_MOD_NUM        0x0200
#define KEYBOARD_MOD_SCROLL     0x0400

/* 键盘扫描码集 */
#define SCANCODE_SET_1          1
#define SCANCODE_SET_2          2
#define SCANCODE_SET_3          3

/* 键盘缓冲区大小 */
#define KEYBOARD_BUFFER_SIZE    256

/* 键盘驱动状态 */
typedef struct {
    bool initialized;           /* 是否已初始化 */
    uint8_t scancode_set;       /* 扫描码集 */
    uint8_t led_status;         /* LED状态 */
    bool extended_mode;         /* 扩展模式标志 */
    uint8_t buffer[KEYBOARD_BUFFER_SIZE]; /* 键盘缓冲区 */
    uint32_t buffer_head;       /* 缓冲区头指针 */
    uint32_t buffer_tail;       /* 缓冲区尾指针 */
    bool shift_pressed;         /* Shift键状态 */
    bool ctrl_pressed;          /* Ctrl键状态 */
    bool alt_pressed;           /* Alt键状态 */
    bool caps_lock;             /* Caps Lock状态 */
    bool num_lock;              /* Num Lock状态 */
    bool scroll_lock;           /* Scroll Lock状态 */
} keyboard_state_t;

/* 全局键盘状态 */
static keyboard_state_t keyboard_state;

/* 键盘布局映射表（简化的US布局） */
static const char keymap_lower[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const char keymap_upper[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* I/O端口操作函数 */
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/* 等待键盘控制器准备好 */
static bool keyboard_wait_ready(void) {
    uint32_t timeout = 100000; /* 防止无限等待 */

    while (timeout--) {
        uint8_t status = inb(KEYBOARD_STATUS_PORT);
        if (!(status & KEYBOARD_STATUS_IBF)) {
            return true;
        }
    }

    return false;
}

/* 发送命令到键盘控制器 */
static bool keyboard_send_command(uint8_t command) {
    if (!keyboard_wait_ready()) {
        return false;
    }

    outb(KEYBOARD_COMMAND_PORT, command);
    return true;
}

/* 发送数据到键盘 */
static bool keyboard_send_data(uint8_t data) {
    if (!keyboard_wait_ready()) {
        return false;
    }

    outb(KEYBOARD_DATA_PORT, data);
    return true;
}

/* 从键盘读取数据 */
static uint8_t keyboard_read_data(void) {
    while (inb(KEYBOARD_STATUS_PORT) & KEYBOARD_STATUS_OBF) {
        /* 等待输出缓冲区满 */
    }

    return inb(KEYBOARD_DATA_PORT);
}

/* 等待键盘响应 */
static uint8_t keyboard_wait_response(void) {
    uint32_t timeout = 100000;

    while (timeout--) {
        if (inb(KEYBOARD_STATUS_PORT) & KEYBOARD_STATUS_OBF) {
            return inb(KEYBOARD_DATA_PORT);
        }
    }

    return 0; /* 超时 */
}

/* 设置键盘LED状态 */
static void keyboard_set_leds(uint8_t leds) {
    /* 发送设置LED命令 */
    if (keyboard_send_data(0xED)) {
        uint8_t response = keyboard_wait_response();
        if (response == KEYBOARD_ACK) {
            keyboard_send_data(leds);
        }
    }
}

/* 处理键盘中断 */
void keyboard_handler(void) {
    uint8_t scancode = keyboard_read_data();
    uint8_t key_code = 0;
    bool pressed = true;

    /* 处理扩展扫描码 */
    if (scancode == 0xE0) {
        keyboard_state.extended_mode = true;
        return; /* 等待下一个字节 */
    }

    /* 处理按键释放 */
    if (scancode & 0x80) {
        pressed = false;
        scancode &= 0x7F;
    }

    /* 获取键码 */
    if (keyboard_state.extended_mode) {
        /* 扩展键码处理（简化的处理） */
        key_code = scancode + 128; /* 扩展键码范围 */
        keyboard_state.extended_mode = false;
    } else {
        key_code = scancode;
    }

    /* 更新修饰键状态 */
    switch (key_code) {
        case 0x2A: /* Left Shift */
        case 0x36: /* Right Shift */
            keyboard_state.shift_pressed = pressed;
            break;
        case 0x1D: /* Left Ctrl */
            keyboard_state.ctrl_pressed = pressed;
            break;
        case 0x38: /* Right Ctrl */
            keyboard_state.ctrl_pressed = pressed;
            break;
        case 0x64: /* Left Alt */
            keyboard_state.alt_pressed = pressed;
            break;
        case 0x3A: /* Caps Lock */
            if (!pressed) {
                keyboard_state.caps_lock = !keyboard_state.caps_lock;
                keyboard_set_leds((keyboard_state.caps_lock ? KEYBOARD_LED_CAPS : 0) |
                                (keyboard_state.num_lock ? KEYBOARD_LED_NUM : 0) |
                                (keyboard_state.scroll_lock ? KEYBOARD_LED_SCROLL : 0));
            }
            break;
        case 0x45: /* Num Lock */
            if (!pressed) {
                keyboard_state.num_lock = !keyboard_state.num_lock;
                keyboard_set_leds((keyboard_state.caps_lock ? KEYBOARD_LED_CAPS : 0) |
                                (keyboard_state.num_lock ? KEYBOARD_LED_NUM : 0) |
                                (keyboard_state.scroll_lock ? KEYBOARD_LED_SCROLL : 0));
            }
            break;
        case 0x46: /* Scroll Lock */
            if (!pressed) {
                keyboard_state.scroll_lock = !keyboard_state.scroll_lock;
                keyboard_set_leds((keyboard_state.caps_lock ? KEYBOARD_LED_CAPS : 0) |
                                (keyboard_state.num_lock ? KEYBOARD_LED_NUM : 0) |
                                (keyboard_state.scroll_lock ? KEYBOARD_LED_SCROLL : 0));
            }
            break;
    }

    /* 如果是修饰键，不产生字符 */
    if (key_code >= 0x80 || key_code < 128) {
        return;
    }

    /* 获取字符 */
    char ch = 0;

    if (keyboard_state.shift_pressed ^ keyboard_state.caps_lock) {
        /* 大写模式 */
        ch = keymap_upper[key_code];
    } else {
        /* 小写模式 */
        ch = keymap_lower[key_code];
    }

    /* 如果有字符，添加到缓冲区 */
    if (ch != 0) {
        uint32_t next_tail = (keyboard_state.buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;

        if (next_tail != keyboard_state.buffer_head) {
            keyboard_state.buffer[keyboard_state.buffer_tail] = ch;
            keyboard_state.buffer_tail = next_tail;
        }
    }
}

/* 从键盘缓冲区读取字符 */
char keyboard_get_char(void) {
    if (keyboard_state.buffer_head == keyboard_state.buffer_tail) {
        return 0; /* 缓冲区为空 */
    }

    char ch = keyboard_state.buffer[keyboard_state.buffer_head];
    keyboard_state.buffer_head = (keyboard_state.buffer_head + 1) % KEYBOARD_BUFFER_SIZE;

    return ch;
}

/* 检查键盘缓冲区是否有数据 */
bool keyboard_has_char(void) {
    return keyboard_state.buffer_head != keyboard_state.buffer_tail;
}

/* 初始化键盘驱动 */
void keyboard_init(void) {
    KLOG_INFO("Initializing keyboard driver...");

    /* 初始化键盘状态 */
    memset(&keyboard_state, 0, sizeof(keyboard_state));
    keyboard_state.scancode_set = SCANCODE_SET_1;
    keyboard_state.num_lock = true; /* 默认启用Num Lock */

    /* 清空键盘缓冲区 */
    keyboard_state.buffer_head = 0;
    keyboard_state.buffer_tail = 0;

    /* 禁用键盘 */
    keyboard_send_command(KEYBOARD_CMD_DISABLE);

    /* 清空输出缓冲区 */
    while (inb(KEYBOARD_STATUS_PORT) & KEYBOARD_STATUS_OBF) {
        inb(KEYBOARD_DATA_PORT);
    }

    /* 设置扫描码集 */
    if (keyboard_send_data(0xF0)) { /* 设置扫描码集命令 */
        if (keyboard_wait_response() == KEYBOARD_ACK) {
            keyboard_send_data(SCANCODE_SET_1); /* 使用扫描码集1 */
        }
    }

    /* 设置LED状态 */
    keyboard_set_leds((keyboard_state.caps_lock ? KEYBOARD_LED_CAPS : 0) |
                     (keyboard_state.num_lock ? KEYBOARD_LED_NUM : 0) |
                     (keyboard_state.scroll_lock ? KEYBOARD_LED_SCROLL : 0));

    /* 启用键盘 */
    keyboard_send_command(KEYBOARD_CMD_ENABLE);

    /* 注册键盘中断处理函数 */
    idt_register_handler(0x21, (interrupt_handler_t)keyboard_handler);

    keyboard_state.initialized = true;

    KLOG_INFO("Keyboard driver initialized");
}

/* 获取键盘状态 */
bool keyboard_is_initialized(void) {
    return keyboard_state.initialized;
}

/* 获取修饰键状态 */
uint32_t keyboard_get_modifiers(void) {
    uint32_t modifiers = 0;

    if (keyboard_state.shift_pressed) modifiers |= KEYBOARD_MOD_SHIFT;
    if (keyboard_state.ctrl_pressed) modifiers |= KEYBOARD_MOD_CTRL;
    if (keyboard_state.alt_pressed) modifiers |= KEYBOARD_MOD_ALT;
    if (keyboard_state.caps_lock) modifiers |= KEYBOARD_MOD_CAPS;
    if (keyboard_state.num_lock) modifiers |= KEYBOARD_MOD_NUM;
    if (keyboard_state.scroll_lock) modifiers |= KEYBOARD_MOD_SCROLL;

    return modifiers;
}
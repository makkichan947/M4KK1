/*
 * M4KK1键盘驱动 - 键盘输入处理接口
 * 支持PS/2和USB键盘设备
 */

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 键盘事件类型 */
typedef enum {
    KEYBOARD_EVENT_KEY_PRESS = 0,   /* 按键按下 */
    KEYBOARD_EVENT_KEY_RELEASE = 1, /* 按键释放 */
    KEYBOARD_EVENT_KEY_REPEAT = 2   /* 按键重复 */
} keyboard_event_type_t;

/* 按键状态 */
typedef enum {
    KEYBOARD_KEY_RELEASED = 0,      /* 按键释放 */
    KEYBOARD_KEY_PRESSED = 1        /* 按键按下 */
} keyboard_key_state_t;

/* 修饰键标志 */
#define KEYBOARD_MOD_NONE   0x0000
#define KEYBOARD_MOD_LSHIFT 0x0001
#define KEYBOARD_MOD_RSHIFT 0x0002
#define KEYBOARD_MOD_LCTRL  0x0004
#define KEYBOARD_MOD_RCTRL  0x0008
#define KEYBOARD_MOD_LALT   0x0010
#define KEYBOARD_MOD_RALT   0x0020
#define KEYBOARD_MOD_LMETA  0x0040
#define KEYBOARD_MOD_RMETA  0x0080
#define KEYBOARD_MOD_CAPS   0x0100
#define KEYBOARD_MOD_NUM    0x0200

/* 键盘事件结构 */
typedef struct {
    keyboard_event_type_t type;     /* 事件类型 */
    uint32_t keycode;               /* 按键代码 */
    keyboard_key_state_t state;     /* 按键状态 */
    uint32_t modifiers;             /* 修饰键状态 */
    char ascii_char;                /* ASCII字符 */
    uint64_t timestamp;             /* 时间戳 */
} keyboard_event_t;

/* 键盘LED状态 */
typedef struct {
    bool num_lock;                  /* 数字锁定 */
    bool caps_lock;                 /* 大写锁定 */
    bool scroll_lock;               /* 滚动锁定 */
} keyboard_led_t;

/* 键盘布局类型 */
typedef enum {
    KEYBOARD_LAYOUT_QWERTY = 0,     /* QWERTY布局 */
    KEYBOARD_LAYOUT_AZERTY = 1,     /* AZERTY布局 */
    KEYBOARD_LAYOUT_QWERTZ = 2,     /* QWERTZ布局 */
    KEYBOARD_LAYOUT_DVORAK = 3,     /* Dvorak布局 */
    KEYBOARD_LAYOUT_COLEMAK = 4     /* Colemak布局 */
} keyboard_layout_t;

/* 键盘配置结构 */
typedef struct {
    keyboard_layout_t layout;       /* 键盘布局 */
    bool repeat_enabled;            /* 重复启用 */
    uint32_t repeat_delay;          /* 重复延迟 (ms) */
    uint32_t repeat_rate;           /* 重复速率 (ms) */
    keyboard_led_t led_state;       /* LED状态 */
} keyboard_config_t;

/* 键盘驱动接口结构 */
typedef struct {
    char *name;                     /* 驱动名称 */
    char *description;              /* 驱动描述 */

    /* 初始化和清理 */
    int (*init)(void);              /* 初始化驱动 */
    void (*cleanup)(void);          /* 清理驱动 */

    /* 事件处理 */
    int (*poll_event)(keyboard_event_t *event); /* 轮询事件 */
    int (*wait_event)(keyboard_event_t *event); /* 等待事件 */

    /* 配置管理 */
    int (*set_config)(keyboard_config_t *config); /* 设置配置 */
    int (*get_config)(keyboard_config_t *config); /* 获取配置 */

    /* LED控制 */
    int (*set_led)(keyboard_led_t *led); /* 设置LED状态 */
    int (*get_led)(keyboard_led_t *led); /* 获取LED状态 */

    /* 私有数据 */
    void *priv_data;                /* 私有数据 */
} keyboard_driver_t;

/* 函数声明 */

/* 键盘驱动管理 */
int keyboard_driver_register(keyboard_driver_t *driver);
int keyboard_driver_unregister(keyboard_driver_t *driver);
keyboard_driver_t *keyboard_driver_get(const char *name);

/* PS/2键盘驱动 */
int ps2_keyboard_init(void);
void ps2_keyboard_cleanup(void);
int ps2_keyboard_poll_event(keyboard_event_t *event);

/* USB键盘驱动 */
int usb_keyboard_init(void);
void usb_keyboard_cleanup(void);
int usb_keyboard_poll_event(keyboard_event_t *event);

/* 键盘事件处理 */
int keyboard_process_event(keyboard_event_t *event);
int keyboard_translate_keycode(uint32_t keycode, uint32_t modifiers, char *ascii);

/* 键盘配置 */
int keyboard_load_config(const char *filename, keyboard_config_t *config);
int keyboard_save_config(const char *filename, keyboard_config_t *config);

/* 键盘测试 */
int keyboard_self_test(void);
int keyboard_diagnostic(void);

#endif /* __KEYBOARD_H__ */
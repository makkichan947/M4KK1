/*
 * M4KK1鼠标驱动 - 鼠标输入处理接口
 * 支持PS/2和USB鼠标设备
 */

#ifndef __MOUSE_H__
#define __MOUSE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 鼠标事件类型 */
typedef enum {
    MOUSE_EVENT_BUTTON_PRESS = 0,   /* 按钮按下 */
    MOUSE_EVENT_BUTTON_RELEASE = 1, /* 按钮释放 */
    MOUSE_EVENT_MOTION = 2,         /* 移动 */
    MOUSE_EVENT_WHEEL = 3           /* 滚轮 */
} mouse_event_type_t;

/* 鼠标按钮 */
typedef enum {
    MOUSE_BUTTON_LEFT = 0,          /* 左键 */
    MOUSE_BUTTON_RIGHT = 1,         /* 右键 */
    MOUSE_BUTTON_MIDDLE = 2,        /* 中键 */
    MOUSE_BUTTON_SIDE = 3,          /* 侧键 */
    MOUSE_BUTTON_EXTRA = 4          /* 额外按钮 */
} mouse_button_t;

/* 鼠标按钮状态 */
typedef enum {
    MOUSE_BUTTON_RELEASED = 0,      /* 按钮释放 */
    MOUSE_BUTTON_PRESSED = 1        /* 按钮按下 */
} mouse_button_state_t;

/* 鼠标事件结构 */
typedef struct {
    mouse_event_type_t type;        /* 事件类型 */
    mouse_button_t button;          /* 鼠标按钮 */
    mouse_button_state_t button_state; /* 按钮状态 */

    /* 位置信息 */
    int32_t x;                      /* X坐标 */
    int32_t y;                      /* Y坐标 */
    int32_t wheel_x;                /* 滚轮X */
    int32_t wheel_y;                /* 滚轮Y */

    /* 相对移动 */
    int32_t dx;                     /* X轴相对移动 */
    int32_t dy;                     /* Y轴相对移动 */
    int32_t dwheel_x;               /* 滚轮X相对移动 */
    int32_t dwheel_y;               /* 滚轮Y相对移动 */

    /* 状态标志 */
    bool left_button;               /* 左键状态 */
    bool right_button;              /* 右键状态 */
    bool middle_button;             /* 中键状态 */

    uint64_t timestamp;             /* 时间戳 */
} mouse_event_t;

/* 鼠标配置结构 */
typedef struct {
    bool enabled;                   /* 启用状态 */
    int32_t acceleration;           /* 加速 */
    int32_t sensitivity;            /* 灵敏度 */
    int32_t threshold;              /* 阈值 */
    bool swap_buttons;              /* 交换左右键 */
    bool wheel_emulation;           /* 滚轮模拟 */
} mouse_config_t;

/* 鼠标驱动接口结构 */
typedef struct {
    char *name;                     /* 驱动名称 */
    char *description;              /* 驱动描述 */

    /* 初始化和清理 */
    int (*init)(void);              /* 初始化驱动 */
    void (*cleanup)(void);          /* 清理驱动 */

    /* 事件处理 */
    int (*poll_event)(mouse_event_t *event); /* 轮询事件 */
    int (*wait_event)(mouse_event_t *event); /* 等待事件 */

    /* 配置管理 */
    int (*set_config)(mouse_config_t *config); /* 设置配置 */
    int (*get_config)(mouse_config_t *config); /* 获取配置 */

    /* 私有数据 */
    void *priv_data;                /* 私有数据 */
} mouse_driver_t;

/* 函数声明 */

/* 鼠标驱动管理 */
int mouse_driver_register(mouse_driver_t *driver);
int mouse_driver_unregister(mouse_driver_t *driver);
mouse_driver_t *mouse_driver_get(const char *name);

/* PS/2鼠标驱动 */
int ps2_mouse_init(void);
void ps2_mouse_cleanup(void);
int ps2_mouse_poll_event(mouse_event_t *event);

/* USB鼠标驱动 */
int usb_mouse_init(void);
void usb_mouse_cleanup(void);
int usb_mouse_poll_event(mouse_event_t *event);

/* 鼠标事件处理 */
int mouse_process_event(mouse_event_t *event);
int mouse_calibrate(int32_t *x_scale, int32_t *y_scale);

/* 鼠标配置 */
int mouse_load_config(const char *filename, mouse_config_t *config);
int mouse_save_config(const char *filename, mouse_config_t *config);

/* 鼠标测试 */
int mouse_self_test(void);
int mouse_diagnostic(void);

#endif /* __MOUSE_H__ */
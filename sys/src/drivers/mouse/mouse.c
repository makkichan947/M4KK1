/**
 * M4KK1鼠标驱动实现
 * 支持PS/2和USB鼠标设备
 */

#include "mouse.h"
#include "../../include/console.h"
#include "../../include/kernel.h"
#include "../../include/idt.h"
#include <stdint.h>
#include <stdbool.h>
#include "../../include/string.h"

/* I/O端口 */
#define MOUSE_DATA_PORT         0x60
#define MOUSE_STATUS_PORT       0x64
#define MOUSE_COMMAND_PORT      0x64

/* 鼠标控制器命令 */
#define MOUSE_CMD_DISABLE       0xA7
#define MOUSE_CMD_ENABLE        0xA8
#define MOUSE_CMD_TEST_MOUSE    0xA9
#define MOUSE_CMD_SELF_TEST     0xAA
#define MOUSE_CMD_INTERFACE_TEST 0xAB

/* 鼠标响应 */
#define MOUSE_ACK               0xFA
#define MOUSE_RESEND            0xFE
#define MOUSE_TEST_OK           0x00

/* 鼠标状态标志 */
#define MOUSE_STATUS_OBF        0x01    /* 输出缓冲区满 */
#define MOUSE_STATUS_IBF        0x02    /* 输入缓冲区满 */

/* 鼠标数据包大小 */
#define MOUSE_PACKET_SIZE       3

/* 鼠标状态标志位 */
#define MOUSE_LEFT_BUTTON       0x01
#define MOUSE_RIGHT_BUTTON      0x02
#define MOUSE_MIDDLE_BUTTON     0x04
#define MOUSE_X_SIGN            0x10
#define MOUSE_Y_SIGN            0x20
#define MOUSE_X_OVERFLOW        0x40
#define MOUSE_Y_OVERFLOW        0x80

/* 鼠标驱动状态 */
typedef struct {
    bool initialized;           /* 是否已初始化 */
    bool has_wheel;             /* 是否有滚轮 */
    int8_t x_movement;          /* X轴移动 */
    int8_t y_movement;          /* Y轴移动 */
    int8_t z_movement;          /* Z轴移动（滚轮） */
    uint8_t buttons;            /* 按钮状态 */
    uint8_t packet[MOUSE_PACKET_SIZE]; /* 数据包缓冲区 */
    uint8_t packet_index;       /* 数据包索引 */
    int32_t x_position;         /* 当前X位置 */
    int32_t y_position;         /* 当前Y位置 */
    uint32_t sample_rate;       /* 采样率 */
} mouse_state_t;

/* 全局鼠标状态 */
static mouse_state_t mouse_state;

/* I/O端口操作函数 */
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/* 等待鼠标控制器准备好 */
static bool mouse_wait_ready(void) {
    uint32_t timeout = 100000; /* 防止无限等待 */

    while (timeout--) {
        uint8_t status = inb(MOUSE_STATUS_PORT);
        if (!(status & MOUSE_STATUS_IBF)) {
            return true;
        }
    }

    return false;
}

/* 发送命令到鼠标控制器 */
static bool mouse_send_command(uint8_t command) {
    if (!mouse_wait_ready()) {
        return false;
    }

    outb(MOUSE_COMMAND_PORT, command);
    return true;
}

/* 发送数据到鼠标 */
static bool mouse_send_data(uint8_t data) {
    if (!mouse_wait_ready()) {
        return false;
    }

    outb(MOUSE_DATA_PORT, data);
    return true;
}

/* 从鼠标读取数据 */
static uint8_t mouse_read_data(void) {
    while (!(inb(MOUSE_STATUS_PORT) & MOUSE_STATUS_OBF)) {
        /* 等待输出缓冲区满 */
    }

    return inb(MOUSE_DATA_PORT);
}

/* 等待鼠标响应 */
static uint8_t mouse_wait_response(void) {
    uint32_t timeout = 100000;

    while (timeout--) {
        if (inb(MOUSE_STATUS_PORT) & MOUSE_STATUS_OBF) {
            return inb(MOUSE_DATA_PORT);
        }
    }

    return 0; /* 超时 */
}

/* 处理鼠标数据包 */
void mouse_process_packet(uint8_t *packet);

/* 处理鼠标中断 */
void mouse_handler(void) {
    uint8_t data;

    /* 检查是否有数据 */
    if (!(inb(MOUSE_STATUS_PORT) & MOUSE_STATUS_OBF)) {
        return;
    }

    data = inb(MOUSE_DATA_PORT);

    /* 处理数据包 */
    mouse_state.packet[mouse_state.packet_index++] = data;

    if (mouse_state.packet_index >= MOUSE_PACKET_SIZE) {
        /* 完整的数据包接收完成 */
        mouse_process_packet(mouse_state.packet);
        mouse_state.packet_index = 0;
    }
}

/* 处理鼠标数据包 */
void mouse_process_packet(uint8_t *packet) {

    /* 解析数据包 */
    mouse_state.buttons = packet[0] & 0x07; /* 按钮状态 */
    mouse_state.x_movement = (int8_t)packet[1];
    mouse_state.y_movement = (int8_t)packet[2];

    /* 处理符号位 */
    if (packet[0] & MOUSE_X_SIGN) {
        mouse_state.x_movement -= 256;
    }
    if (packet[0] & MOUSE_Y_SIGN) {
        mouse_state.y_movement -= 256;
    }

    /* 处理滚轮（如果有） */
    if (mouse_state.has_wheel && mouse_state.packet_index > 3) {
        mouse_state.z_movement = (int8_t)packet[3];
        if (packet[0] & MOUSE_X_OVERFLOW) {
            mouse_state.z_movement -= 256;
        }
    }

    /* 更新位置 */
    mouse_state.x_position += mouse_state.x_movement;
    mouse_state.y_position -= mouse_state.y_movement; /* Y轴通常是反向的 */

    /* 边界检查 */
    if (mouse_state.x_position < 0) mouse_state.x_position = 0;
    if (mouse_state.y_position < 0) mouse_state.y_position = 0;

    /* 这里可以添加鼠标事件通知机制 */
    /* 例如：通知窗口管理器鼠标移动、按钮状态变化等 */
}

/* 初始化鼠标驱动 */
void mouse_init(void) {
    KLOG_INFO("Initializing mouse driver...");

    /* 初始化鼠标状态 */
    memset(&mouse_state, 0, sizeof(mouse_state));
    mouse_state.packet_index = 0;
    mouse_state.sample_rate = 100; /* 默认采样率 */

    /* 禁用鼠标 */
    mouse_send_command(MOUSE_CMD_DISABLE);

    /* 清空输出缓冲区 */
    while (inb(MOUSE_STATUS_PORT) & MOUSE_STATUS_OBF) {
        inb(MOUSE_DATA_PORT);
    }

    /* 执行鼠标自检 */
    if (mouse_send_command(MOUSE_CMD_SELF_TEST)) {
        uint8_t response = mouse_wait_response();
        if (response != MOUSE_TEST_OK) {
            KLOG_WARN("Mouse self-test failed");
            return;
        }
    }

    /* 启用鼠标 */
    if (mouse_send_command(MOUSE_CMD_ENABLE)) {
        uint8_t response = mouse_wait_response();
        if (response != MOUSE_ACK) {
            KLOG_WARN("Mouse enable failed");
            return;
        }
    }

    /* 设置鼠标参数 */
    /* 设置采样率 */
    if (mouse_send_data(0xF3)) {
        mouse_wait_response(); /* 应该收到ACK */
        mouse_send_data(100); /* 100Hz采样率 */
    }

    /* 启用滚轮（如果支持） */
    if (mouse_send_data(0xF4)) {
        uint8_t response = mouse_wait_response();
        if (response == MOUSE_ACK) {
            mouse_state.has_wheel = true;
        }
    }

    /* 设置分辨率 */
    if (mouse_send_data(0xE8)) {
        mouse_wait_response(); /* ACK */
        mouse_send_data(0x03); /* 8 counts/mm分辨率 */
    }

    /* 注册鼠标中断处理函数 */
    idt_register_handler(0x2C, (interrupt_handler_t)mouse_handler);

    mouse_state.initialized = true;

    KLOG_INFO("Mouse driver initialized");
}

/* 获取鼠标状态 */
bool mouse_is_initialized(void) {
    return mouse_state.initialized;
}

/* 获取鼠标位置 */
void mouse_get_position(int32_t *x, int32_t *y) {
    if (x) *x = mouse_state.x_position;
    if (y) *y = mouse_state.y_position;
}

/* 获取鼠标移动 */
void mouse_get_movement(int8_t *x, int8_t *y, int8_t *z) {
    if (x) *x = mouse_state.x_movement;
    if (y) *y = mouse_state.y_movement;
    if (z) *z = mouse_state.z_movement;
}

/* 获取鼠标按钮状态 */
uint8_t mouse_get_buttons(void) {
    return mouse_state.buttons;
}

/* 设置鼠标采样率 */
void mouse_set_sample_rate(uint32_t rate) {
    if (rate > 200) rate = 200; /* 最大采样率 */
    if (rate < 10) rate = 10;   /* 最小采样率 */

    mouse_state.sample_rate = rate;

    /* 发送采样率命令 */
    if (mouse_send_data(0xF3)) {
        mouse_wait_response(); /* ACK */
        mouse_send_data((uint8_t)rate);
    }
}

/* 获取鼠标采样率 */
uint32_t mouse_get_sample_rate(void) {
    return mouse_state.sample_rate;
}

/* 检查鼠标是否有滚轮 */
bool mouse_has_wheel(void) {
    return mouse_state.has_wheel;
}
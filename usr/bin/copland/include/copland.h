/*
 * M4KK1 Copland窗口服务器 - 主接口头文件
 * 定义窗口服务器的核心数据结构和接口
 * 完全原创架构，由makkichan947独立设计开发
 */

#ifndef __COPLAND_H__
#define __COPLAND_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "copland-protocol.h"

/* 版本信息 */
#define COPLAND_VERSION_MAJOR 1
#define COPLAND_VERSION_MINOR 0
#define COPLAND_VERSION_PATCH 0
#define COPLAND_VERSION_STRING "1.0.0"

/* 常量定义 */
#define MAX_CLIENTS   128
#define MAX_WINDOWS   1024
#define MAX_SURFACES  2048
#define MAX_BUFFERS   4096
#define MAX_OUTPUTS   16
#define MAX_SEATS     8

/* 协议版本 */
#define COPLAND_PROTOCOL_VERSION 1

/* 对象ID分配 */
#define COPLAND_FIRST_OBJECT_ID 0xFF000000
#define COPLAND_LAST_OBJECT_ID  0xFFFFFFFF

/* 使用协议定义的类型别名 */
typedef copland_window_type_t window_type_t;
typedef copland_window_state_t window_state_t;
typedef copland_surface_role_t surface_role_t;
typedef copland_buffer_type_t buffer_type_t;
typedef copland_buffer_format_t buffer_format_t;
typedef copland_input_device_type_t input_device_type_t;

/* 窗口结构 */
typedef struct window {
    uint32_t id;            /* 窗口ID */
    copland_window_type_t type;     /* 窗口类型 */
    copland_window_state_t state;   /* 窗口状态 */

    char *title;            /* 窗口标题 */
    char *class;            /* 窗口类名 */
    char *instance;         /* 窗口实例名 */

    /* 几何信息 */
    int32_t x, y;           /* 窗口位置 */
    uint32_t width, height; /* 窗口大小 */
    uint32_t min_width, min_height; /* 最小尺寸 */
    uint32_t max_width, max_height; /* 最大尺寸 */

    /* 表面信息 */
    struct surface *surface; /* 窗口表面 */
    struct buffer *buffer;  /* 窗口缓冲区 */

    /* 层级信息 */
    struct window *parent;  /* 父窗口 */
    struct window **children; /* 子窗口列表 */
    size_t child_count;     /* 子窗口数量 */

    /* 属性 */
    bool decorated;         /* 是否装饰 */
    bool resizable;         /* 是否可调整大小 */
    bool modal;             /* 是否模态 */
    bool override_redirect; /* 是否覆盖重定向 */

    struct window *next;    /* 下一个窗口 */
    void *userdata;         /* 用户数据 */
} window_t;

/* 表面结构 */
typedef struct surface {
    uint32_t id;            /* 表面ID */
    copland_surface_role_t role;    /* 表面角色 */

    /* 缓冲区信息 */
    struct buffer *buffer;  /* 当前缓冲区 */
    struct buffer **buffers; /* 缓冲区列表 */
    size_t buffer_count;    /* 缓冲区数量 */

    /* 几何信息 */
    int32_t x, y;           /* 相对于父表面的偏移 */
    uint32_t width, height; /* 表面大小 */
    uint32_t scale;         /* 缩放因子 */

    /* 状态信息 */
    bool opaque;            /* 是否不透明 */
    bool input_region;      /* 输入区域 */
    bool damage;            /* 损坏区域 */

    struct surface *next;   /* 下一个表面 */
    void *userdata;         /* 用户数据 */
} surface_t;

/* 缓冲区结构 */
typedef struct buffer {
    uint32_t id;            /* 缓冲区ID */
    copland_buffer_type_t type;     /* 缓冲区类型 */
    copland_buffer_format_t format; /* 缓冲区格式 */

    /* 内存信息 */
    void *data;             /* 缓冲区数据 */
    size_t size;            /* 缓冲区大小 */
    int fd;                 /* DMA缓冲区文件描述符 */

    /* 几何信息 */
    uint32_t width, height; /* 缓冲区尺寸 */
    uint32_t stride;        /* 行_stride */
    uint32_t offset;        /* 数据偏移 */

    /* 元数据 */
    uint32_t flags;         /* 标志位 */
    uint32_t y_inverted;    /* Y轴翻转 */

    /* 引用计数 */
    uint32_t ref_count;     /* 引用计数 */

    struct buffer *next;    /* 下一个缓冲区 */
} buffer_t;

/* 客户端结构 */
typedef struct client {
    uint32_t id;            /* 客户端ID */
    char *name;             /* 客户端名称 */
    pid_t pid;              /* 进程ID */
    uid_t uid;              /* 用户ID */

    uint32_t permissions; /* 权限 */
    char *security_context; /* 安全上下文 */

    /* 资源 */
    window_t **windows;     /* 窗口列表 */
    size_t window_count;    /* 窗口数量 */

    surface_t **surfaces;   /* 表面列表 */
    size_t surface_count;   /* 表面数量 */

    buffer_t **buffers;     /* 缓冲区列表 */
    size_t buffer_count;    /* 缓冲区数量 */

    /* 通信 */
    int fd;                 /* 客户端文件描述符 */

    /* 统计信息 */
    uint64_t message_count; /* 消息数量 */
    uint64_t error_count;   /* 错误数量 */
    time_t connect_time;    /* 连接时间 */

    struct client *next;    /* 下一个客户端 */
    void *userdata;         /* 用户数据 */
} client_t;

/* Copland客户端结构 */
typedef struct {
    bool connected;         /* 连接状态 */
    bool running;           /* 运行状态 */
    void *internal;         /* 内部数据 */
} copland_client_t;

/* 输出结构 */
typedef struct output {
    uint32_t id;            /* 输出ID */
    char *name;             /* 输出名称 */
    char *make;             /* 制造商 */
    char *model;            /* 型号 */

    /* 物理信息 */
    int32_t width_mm, height_mm; /* 物理尺寸 */
    int32_t subpixel;       /* 子像素布局 */
    int32_t transform;      /* 变换 */

    /* 模式信息 */
    struct output_mode **modes; /* 显示模式 */
    size_t mode_count;      /* 模式数量 */
    struct output_mode *current_mode; /* 当前模式 */

    /* 区域信息 */
    int32_t x, y;           /* 位置 */
    uint32_t width, height; /* 分辨率 */
    uint32_t refresh;       /* 刷新率 */
    uint32_t scale;         /* 缩放因子 */

    struct output *next;    /* 下一个输出 */
} output_t;

/* 输出模式结构 */
typedef struct output_mode {
    uint32_t width, height; /* 分辨率 */
    uint32_t refresh;       /* 刷新率 */
    uint32_t flags;         /* 标志位 */
    bool preferred;         /* 首选模式 */
} output_mode_t;

/* 座席结构 */
typedef struct seat {
    uint32_t id;            /* 座席ID */
    char *name;             /* 座席名称 */

    /* 设备 */
    struct input_device **devices; /* 输入设备列表 */
    size_t device_count;    /* 设备数量 */

    /* 能力 */
    uint32_t capabilities;  /* 座席能力 */
    uint32_t pointer;       /* 指针设备 */
    uint32_t keyboard;      /* 键盘设备 */
    uint32_t touch;         /* 触摸设备 */

    struct seat *next;      /* 下一个座席 */
} seat_t;

/* 输入设备结构 */
typedef struct input_device {
    uint32_t id;            /* 设备ID */
    copland_input_device_type_t type; /* 设备类型 */
    char *name;             /* 设备名称 */
    char *path;             /* 设备路径 */

    /* 设备信息 */
    uint32_t vendor;        /* 厂商ID */
    uint32_t product;       /* 产品ID */
    uint32_t version;       /* 版本 */

    /* 能力信息 */
    uint32_t capabilities;  /* 设备能力 */

    struct input_device *next; /* 下一个设备 */
} input_device_t;

/* 服务器结构 */
typedef struct copland_server {
    /* 网络通信 */
    int socket_fd;          /* 服务器Socket文件描述符 */

    /* 资源管理 */
    client_t **clients;     /* 客户端列表 */
    size_t client_count;    /* 客户端数量 */

    window_t **windows;     /* 窗口列表 */
    size_t window_count;    /* 窗口数量 */

    surface_t **surfaces;   /* 表面列表 */
    size_t surface_count;   /* 表面数量 */

    buffer_t **buffers;     /* 缓冲区列表 */
    size_t buffer_count;    /* 缓冲区数量 */

    output_t **outputs;     /* 输出列表 */
    size_t output_count;    /* 输出数量 */

    seat_t **seats;         /* 座席列表 */
    size_t seat_count;      /* 座席数量 */

    /* 配置 */
    char *config_file;      /* 配置文件路径 */
    char *socket_path;      /* Socket路径 */
    bool verbose;           /* 详细输出 */

    /* 运行状态 */
    bool running;           /* 运行状态 */
    pthread_mutex_t mutex;  /* 互斥锁 */
} copland_server_t;

/* 函数声明 */

/* 服务器管理 */
copland_server_t *copland_server_create(void);
void copland_server_destroy(copland_server_t *server);
bool copland_server_init(copland_server_t *server);
bool copland_server_run(copland_server_t *server);
void copland_server_quit(copland_server_t *server);

/* 客户端管理 */
client_t *client_create(int fd, uint32_t id);
void client_destroy(client_t *client);
bool client_add_window(client_t *client, window_t *window);
bool client_remove_window(client_t *client, window_t *window);
bool client_add_surface(client_t *client, surface_t *surface);
bool client_remove_surface(client_t *client, surface_t *surface);

/* 窗口管理 */
window_t *window_create(client_t *client, uint32_t id, copland_window_type_t type);
void window_destroy(window_t *window);
bool window_set_title(window_t *window, const char *title);
bool window_set_geometry(window_t *window, int32_t x, int32_t y, uint32_t width, uint32_t height);
bool window_set_state(window_t *window, copland_window_state_t state);

/* 表面管理 */
surface_t *surface_create(client_t *client, uint32_t id);
void surface_destroy(surface_t *surface);
void surface_attach_buffer(surface_t *surface, buffer_t *buffer);
bool surface_damage(surface_t *surface, int32_t x, int32_t y, uint32_t width, uint32_t height);
bool surface_commit(surface_t *surface);

/* 缓冲区管理 */
buffer_t *buffer_create(client_t *client, uint32_t id, copland_buffer_type_t type, copland_buffer_format_t format);
void buffer_destroy(buffer_t *buffer);
bool buffer_add_reference(buffer_t *buffer);
bool buffer_remove_reference(buffer_t *buffer);

/* 输出管理 */
output_t *output_create(uint32_t id, const char *name);
void output_destroy(output_t *output);
bool output_add_mode(output_t *output, uint32_t width, uint32_t height, uint32_t refresh, bool preferred);
bool output_set_mode(output_t *output, uint32_t width, uint32_t height, uint32_t refresh);

/* 座席管理 */
seat_t *seat_create(uint32_t id, const char *name);
void seat_destroy(seat_t *seat);
bool seat_add_device(seat_t *seat, input_device_t *device);
bool seat_remove_device(seat_t *seat, input_device_t *device);

/* 输入管理 */
input_device_t *input_device_create(uint32_t id, copland_input_device_type_t type, const char *name);
void input_device_destroy(input_device_t *device);
bool input_device_add_capability(input_device_t *device, uint32_t capability);

/* 工具函数 */
uint32_t copland_allocate_id(copland_server_t *server);
void copland_release_id(copland_server_t *server, uint32_t id);
const char *window_type_to_string(copland_window_type_t type);
const char *window_state_to_string(copland_window_state_t state);
const char *buffer_format_to_string(copland_buffer_format_t format);

/* 使用协议定义的错误类型 */
typedef copland_error_t copland_error_t;

/* 事件处理回调函数类型 */
typedef bool (*copland_event_handler_t)(copland_client_t *client,
                                       const copland_message_header_t *header,
                                       void *data, void *user_data);

/* 客户端API函数声明 */
copland_client_t *copland_client_create(void);
void copland_client_destroy(copland_client_t *client);
bool copland_client_connect(copland_client_t *client, const char *socket_path);
void copland_client_disconnect(copland_client_t *client);

uint32_t copland_client_create_window(copland_client_t *client, uint32_t parent_id,
                                     copland_window_type_t type, int32_t x, int32_t y,
                                     uint32_t width, uint32_t height, const char *title);
bool copland_client_destroy_window(copland_client_t *client, uint32_t window_id);
bool copland_client_move_window(copland_client_t *client, uint32_t window_id, int32_t x, int32_t y);
bool copland_client_resize_window(copland_client_t *client, uint32_t window_id, uint32_t width, uint32_t height);
bool copland_client_set_window_title(copland_client_t *client, uint32_t window_id, const char *title);

uint32_t copland_client_create_surface(copland_client_t *client, uint32_t window_id, copland_surface_role_t role);
uint32_t copland_client_create_shm_buffer(copland_client_t *client, uint32_t width, uint32_t height,
                                         copland_buffer_format_t format, int shm_fd, size_t size);
bool copland_client_attach_buffer(copland_client_t *client, uint32_t surface_id, uint32_t buffer_id, int32_t x, int32_t y);
bool copland_client_commit_surface(copland_client_t *client, uint32_t surface_id);

bool copland_client_send_button_event(copland_client_t *client, uint32_t button, bool pressed, uint32_t time);
bool copland_client_send_key_event(copland_client_t *client, uint32_t keycode, bool pressed, uint32_t modifiers, uint32_t time);
bool copland_client_send_motion_event(copland_client_t *client, double x, double y, uint32_t time);
bool copland_client_send_touch_event(copland_client_t *client, uint32_t touch_id, uint32_t type,
                                    double x, double y, uint32_t time);

bool copland_client_ping(copland_client_t *client, uint32_t ping_id);
bool copland_client_get_server_info(copland_client_t *client);
void copland_client_set_event_handler(copland_client_t *client, copland_event_handler_t handler, void *user_data);

#endif /* __COPLAND_H__ */
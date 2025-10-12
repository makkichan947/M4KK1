/*
 * Copland协议定义头文件
 * 定义轻量级、自研Copland协议的消息格式和数据结构
 * 基于Vulkan渲染管线，支持实时性能监控
 */

#ifndef __COPLAND_PROTOCOL_H__
#define __COPLAND_PROTOCOL_H__

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

/* 协议版本 */
#define COPLAND_PROTOCOL_VERSION 1
#define COPLAND_PROTOCOL_MAGIC 0x43504C44  /* "CPLD" */

/* 消息类型枚举 */
typedef enum {
    /* 客户端到服务器消息 */
    COPLAND_MSG_CREATE_WINDOW = 1,
    COPLAND_MSG_DESTROY_WINDOW = 2,
    COPLAND_MSG_SHOW_WINDOW = 3,
    COPLAND_MSG_HIDE_WINDOW = 4,
    COPLAND_MSG_MOVE_WINDOW = 5,
    COPLAND_MSG_RESIZE_WINDOW = 6,
    COPLAND_MSG_SET_TITLE = 7,
    COPLAND_MSG_CREATE_SURFACE = 8,
    COPLAND_MSG_DESTROY_SURFACE = 9,
    COPLAND_MSG_ATTACH_BUFFER = 10,
    COPLAND_MSG_COMMIT_SURFACE = 11,
    COPLAND_MSG_CREATE_BUFFER = 12,
    COPLAND_MSG_DESTROY_BUFFER = 13,
    COPLAND_MSG_CREATE_SHM_BUFFER = 14,
    COPLAND_MSG_PING = 15,
    COPLAND_MSG_PONG = 16,
    COPLAND_MSG_GET_SERVER_INFO = 17,
    COPLAND_MSG_SET_CURSOR = 18,
    COPLAND_MSG_MOVE_CURSOR = 19,
    COPLAND_MSG_BUTTON_DOWN = 20,
    COPLAND_MSG_BUTTON_UP = 21,
    COPLAND_MSG_KEY_DOWN = 22,
    COPLAND_MSG_KEY_UP = 23,
    COPLAND_MSG_AXIS = 24,
    COPLAND_MSG_TOUCH_DOWN = 25,
    COPLAND_MSG_TOUCH_UP = 26,
    COPLAND_MSG_TOUCH_MOTION = 27,
    COPLAND_MSG_FRAME = 28,

    /* 服务器到客户端消息 */
    COPLAND_MSG_WINDOW_GEOMETRY = 1001,
    COPLAND_MSG_WINDOW_STATE = 1002,
    COPLAND_MSG_WINDOW_ENTER = 1003,
    COPLAND_MSG_WINDOW_LEAVE = 1004,
    COPLAND_MSG_SURFACE_ENTER = 1005,
    COPLAND_MSG_SURFACE_LEAVE = 1006,
    COPLAND_MSG_POINTER_ENTER = 1007,
    COPLAND_MSG_POINTER_LEAVE = 1008,
    COPLAND_MSG_KEYBOARD_ENTER = 1009,
    COPLAND_MSG_KEYBOARD_LEAVE = 1010,
    COPLAND_MSG_TOUCH_ENTER = 1011,
    COPLAND_MSG_TOUCH_LEAVE = 1012,
    COPLAND_MSG_SERVER_INFO = 1013,
    COPLAND_MSG_ERROR = 1014,
    COPLAND_MSG_PONG_REPLY = 1015,
    COPLAND_MSG_BUFFER_RELEASE = 1016,
    COPLAND_MSG_OUTPUT_GEOMETRY = 1017,
    COPLAND_MSG_OUTPUT_MODE = 1018,
    COPLAND_MSG_OUTPUT_DONE = 1019,
    COPLAND_MSG_FRAME_DONE = 1020,
    COPLAND_MSG_TOUCH_FRAME = 1021,
} copland_message_type_t;

/* 窗口类型枚举 */
typedef enum {
    COPLAND_WINDOW_TOPLEVEL = 0,
    COPLAND_WINDOW_POPUP = 1,
    COPLAND_WINDOW_TRANSIENT = 2,
    COPLAND_WINDOW_DND = 3,
    COPLAND_WINDOW_SUBSURFACE = 4,
    COPLAND_WINDOW_CURSOR = 5,
} copland_window_type_t;

/* 窗口状态枚举 */
typedef enum {
    COPLAND_WINDOW_STATE_NORMAL = 0,
    COPLAND_WINDOW_STATE_MINIMIZED = 1,
    COPLAND_WINDOW_STATE_MAXIMIZED = 2,
    COPLAND_WINDOW_STATE_FULLSCREEN = 3,
    COPLAND_WINDOW_STATE_ACTIVE = 4,
    COPLAND_WINDOW_STATE_INACTIVE = 5,
} copland_window_state_t;

/* 表面角色枚举 */
typedef enum {
    COPLAND_SURFACE_ROLE_NONE = 0,
    COPLAND_SURFACE_ROLE_CURSOR = 1,
    COPLAND_SURFACE_ROLE_DND = 2,
    COPLAND_SURFACE_ROLE_TOOLTIP = 3,
    COPLAND_SURFACE_ROLE_POPUP = 4,
} copland_surface_role_t;

/* 缓冲区类型枚举 */
typedef enum {
    COPLAND_BUFFER_SHM = 0,
    COPLAND_BUFFER_DMA = 1,
    COPLAND_BUFFER_GBM = 2,
    COPLAND_BUFFER_EGL = 3,
} copland_buffer_type_t;

/* 缓冲区格式枚举 */
typedef enum {
    COPLAND_BUFFER_FORMAT_ARGB8888 = 0,
    COPLAND_BUFFER_FORMAT_XRGB8888 = 1,
    COPLAND_BUFFER_FORMAT_RGB565 = 2,
    COPLAND_BUFFER_FORMAT_NV12 = 3,
    COPLAND_BUFFER_FORMAT_YUV420 = 4,
} copland_buffer_format_t;

/* 输入设备类型枚举 */
typedef enum {
    COPLAND_INPUT_DEVICE_KEYBOARD = 0,
    COPLAND_INPUT_DEVICE_POINTER = 1,
    COPLAND_INPUT_DEVICE_TOUCH = 2,
} copland_input_device_type_t;

/* 错误码枚举 */
typedef enum {
    COPLAND_ERROR_NONE = 0,
    COPLAND_ERROR_PERMISSION_DENIED = 1,
    COPLAND_ERROR_NOT_FOUND = 2,
    COPLAND_ERROR_ALREADY_EXISTS = 3,
    COPLAND_ERROR_INVALID_ARGUMENT = 4,
    COPLAND_ERROR_INVALID_OBJECT = 5,
    COPLAND_ERROR_OUT_OF_MEMORY = 6,
    COPLAND_ERROR_IO = 7,
    COPLAND_ERROR_PROTOCOL = 8,
    COPLAND_ERROR_NOT_SUPPORTED = 9,
    COPLAND_ERROR_SURFACE_BUSY = 10,
    COPLAND_ERROR_SURFACE_EXISTS = 11,
    COPLAND_ERROR_BUFFER_EXISTS = 12,
    COPLAND_ERROR_WINDOW_EXISTS = 13,
} copland_error_t;

/* 消息头结构 */
typedef struct {
    uint32_t magic;      /* 协议魔数 */
    uint32_t version;    /* 协议版本 */
    uint32_t type;       /* 消息类型 */
    uint32_t size;       /* 消息总大小 */
    uint32_t object_id;  /* 对象ID */
    uint32_t serial;     /* 序列号 */
} copland_message_header_t;

/* 通用对象标识结构 */
typedef struct {
    uint32_t id;         /* 对象ID */
} copland_object_t;

/* 通用坐标结构 */
typedef struct {
    int32_t x, y;        /* 坐标 */
} copland_point_t;

/* 通用尺寸结构 */
typedef struct {
    uint32_t width, height; /* 尺寸 */
} copland_size_t;

/* 通用矩形结构 */
typedef struct {
    int32_t x, y;        /* 位置 */
    uint32_t width, height; /* 尺寸 */
} copland_rectangle_t;

/* 创建窗口请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t parent_id;  /* 父窗口ID，0表示顶级窗口 */
    uint32_t type;       /* 窗口类型 */
    int32_t x, y;        /* 初始位置 */
    uint32_t width, height; /* 初始大小 */
    char title[256];     /* 窗口标题 */
} copland_create_window_request_t;

/* 销毁窗口请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t window_id;  /* 窗口ID */
} copland_destroy_window_request_t;

/* 显示/隐藏窗口请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t window_id;  /* 窗口ID */
} copland_show_hide_window_request_t;

/* 移动窗口请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t window_id;  /* 窗口ID */
    int32_t x, y;        /* 新位置 */
} copland_move_window_request_t;

/* 调整窗口大小请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t window_id;  /* 窗口ID */
    uint32_t width, height; /* 新大小 */
} copland_resize_window_request_t;

/* 设置窗口标题请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t window_id;  /* 窗口ID */
    char title[256];     /* 新标题 */
} copland_set_title_request_t;

/* 创建表面请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t window_id;  /* 关联窗口ID */
    uint32_t role;       /* 表面角色 */
} copland_create_surface_request_t;

/* 销毁表面请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t surface_id; /* 表面ID */
} copland_destroy_surface_request_t;

/* 附加缓冲区请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t surface_id; /* 表面ID */
    uint32_t buffer_id;  /* 缓冲区ID */
    int32_t x, y;        /* 缓冲区在表面上的偏移 */
} copland_attach_buffer_request_t;

/* 提交表面请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t surface_id; /* 表面ID */
} copland_commit_surface_request_t;

/* 创建缓冲区请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t type;       /* 缓冲区类型 */
    uint32_t format;     /* 缓冲区格式 */
    uint32_t width, height; /* 缓冲区尺寸 */
    uint32_t stride;     /* 行跨度 */
} copland_create_buffer_request_t;

/* 销毁缓冲区请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t buffer_id;  /* 缓冲区ID */
} copland_destroy_buffer_request_t;

/* 创建共享内存缓冲区请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t format;     /* 缓冲区格式 */
    uint32_t width, height; /* 缓冲区尺寸 */
    uint32_t stride;     /* 行跨度 */
    int32_t fd;          /* 共享内存文件描述符 */
    uint32_t size;       /* 共享内存大小 */
} copland_create_shm_buffer_request_t;

/* Ping请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t ping_id;    /* Ping ID */
} copland_ping_request_t;

/* Pong响应 */
typedef struct {
    copland_message_header_t header;
    uint32_t ping_id;    /* Ping ID */
} copland_pong_response_t;

/* 获取服务器信息请求 */
typedef struct {
    copland_message_header_t header;
} copland_get_server_info_request_t;

/* 设置光标请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t surface_id; /* 光标表面ID */
    int32_t hotspot_x, hotspot_y; /* 热点坐标 */
} copland_set_cursor_request_t;

/* 移动光标请求 */
typedef struct {
    copland_message_header_t header;
    uint32_t time;       /* 时间戳 */
    double x, y;         /* 光标位置 */
} copland_move_cursor_request_t;

/* 按钮事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t time;       /* 时间戳 */
    uint32_t button;     /* 按钮编号 */
    uint32_t state;      /* 按钮状态：0=释放，1=按下 */
} copland_button_event_t;

/* 键盘事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t time;       /* 时间戳 */
    uint32_t keycode;    /* 键码 */
    uint32_t state;      /* 键状态：0=释放，1=按下 */
    uint32_t modifiers;  /* 修饰键状态 */
} copland_key_event_t;

/* 轴事件（滚轮等） */
typedef struct {
    copland_message_header_t header;
    uint32_t time;       /* 时间戳 */
    uint32_t axis;       /* 轴类型 */
    double value;        /* 轴值 */
} copland_axis_event_t;

/* 触摸事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t time;       /* 时间戳 */
    uint32_t touch_id;   /* 触摸点ID */
    uint32_t state;      /* 触摸状态 */
    double x, y;         /* 触摸位置 */
} copland_touch_event_t;

/* 帧事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t frame_id;   /* 帧ID */
} copland_frame_event_t;

/* 窗口几何事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t window_id;  /* 窗口ID */
    int32_t x, y;        /* 窗口位置 */
    uint32_t width, height; /* 窗口大小 */
} copland_window_geometry_event_t;

/* 窗口状态事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t window_id;  /* 窗口ID */
    uint32_t state;      /* 窗口状态 */
} copland_window_state_event_t;

/* 窗口进入/离开事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t window_id;  /* 窗口ID */
    uint32_t surface_id; /* 表面ID */
} copland_window_enter_leave_event_t;

/* 指针进入/离开事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t surface_id; /* 表面ID */
    double x, y;         /* 指针位置 */
} copland_pointer_enter_leave_event_t;

/* 键盘焦点事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t surface_id; /* 表面ID */
} copland_keyboard_focus_event_t;

/* 触摸焦点事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t surface_id; /* 表面ID */
} copland_touch_focus_event_t;

/* 服务器信息响应 */
typedef struct {
    copland_message_header_t header;
    uint32_t version;    /* 服务器版本 */
    char vendor[64];     /* 供应商名称 */
    uint32_t max_windows; /* 最大窗口数 */
    uint32_t max_surfaces; /* 最大表面数 */
    uint32_t max_buffers; /* 最大缓冲区数 */
} copland_server_info_response_t;

/* 错误响应 */
typedef struct {
    copland_message_header_t header;
    uint32_t error_code; /* 错误码 */
    char message[256];   /* 错误消息 */
} copland_error_response_t;

/* 缓冲区释放事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t buffer_id;  /* 缓冲区ID */
} copland_buffer_release_event_t;

/* 输出几何事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t output_id;  /* 输出ID */
    int32_t x, y;        /* 输出位置 */
    uint32_t width, height; /* 输出尺寸 */
    uint32_t scale;      /* 缩放因子 */
} copland_output_geometry_event_t;

/* 输出模式事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t output_id;  /* 输出ID */
    uint32_t width, height; /* 分辨率 */
    uint32_t refresh;    /* 刷新率 */
    uint32_t flags;      /* 标志位 */
} copland_output_mode_event_t;

/* 输出完成事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t output_id;  /* 输出ID */
} copland_output_done_event_t;

/* 帧完成事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t frame_id;   /* 帧ID */
} copland_frame_done_event_t;

/* 触摸帧事件 */
typedef struct {
    copland_message_header_t header;
    uint32_t time;       /* 时间戳 */
} copland_touch_frame_event_t;

/* Vulkan相关结构 */

/* Vulkan扩展信息 */
typedef struct {
    uint32_t extension_count;
    char **extension_names;
} copland_vulkan_extensions_t;

/* Vulkan表面信息 */
typedef struct {
    uint32_t surface_id;
    uint64_t surface_handle;
    uint32_t width, height;
} copland_vulkan_surface_t;

/* 性能监控信息 */
typedef struct {
    uint64_t frame_count;    /* 帧计数 */
    double fps;             /* 帧率 */
    uint64_t memory_usage;   /* 内存使用量 */
    double cpu_usage;       /* CPU使用率 */
    uint64_t buffer_count;  /* 缓冲区数量 */
    uint64_t surface_count; /* 表面数量 */
    uint64_t window_count;  /* 窗口数量 */
} copland_performance_stats_t;

/* 协议工具函数声明 */

/* 消息序列化函数 */
size_t copland_serialize_message(void *buffer, const copland_message_header_t *header);
size_t copland_deserialize_message(const void *buffer, copland_message_header_t *header);

/* 消息验证函数 */
bool copland_validate_message(const copland_message_header_t *header);

/* 错误码转字符串 */
const char *copland_error_to_string(copland_error_t error);

/* 消息类型转字符串 */
const char *copland_message_type_to_string(copland_message_type_t type);

/* 缓冲区格式转字符串 */
const char *copland_buffer_format_to_string(copland_buffer_format_t format);

/* 窗口类型转字符串 */
const char *copland_window_type_to_string(copland_window_type_t type);

/* 窗口状态转字符串 */
const char *copland_window_state_to_string(copland_window_state_t state);

/* 表面角色转字符串 */
const char *copland_surface_role_to_string(copland_surface_role_t role);

/* 缓冲区类型转字符串 */
const char *copland_buffer_type_to_string(copland_buffer_type_t type);

/* 输入设备类型转字符串 */
const char *copland_input_device_type_to_string(copland_input_device_type_t type);

#endif /* __COPLAND_PROTOCOL_H__ */
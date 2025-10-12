/*
 * Copland客户端库实现
 * 提供客户端API接口和协议编解码功能
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <pthread.h>
#include <poll.h>

#include "copland.h"
#include "copland-protocol.h"

/* 客户端内部数据结构 */
typedef struct {
    copland_client_t *client;
    int server_fd;
    pthread_t event_thread;
    bool connected;
    bool running;

    /* 消息序列号 */
    uint32_t next_serial;

    /* 回调函数 */
    copland_event_handler_t event_handler;
    void *user_data;

    /* 异步消息处理 */
    pthread_mutex_t message_mutex;
    struct pending_message *pending_messages;

    /* 零拷贝缓冲区 */
    pthread_mutex_t buffer_mutex;
    struct client_buffer *buffers;
} copland_client_internal_t;

/* 待处理消息结构 */
typedef struct pending_message {
    copland_message_header_t header;
    void *data;
    size_t data_size;
    struct pending_message *next;
} pending_message_t;

/* 客户端缓冲区结构 */
typedef struct client_buffer {
    uint32_t id;
    uint32_t width, height;
    uint32_t stride;
    uint32_t format;
    void *data;
    size_t size;
    int shm_fd;
    struct client_buffer *next;
} client_buffer_t;

/* 全局客户端内部实例 */
static copland_client_internal_t *internal_client = NULL;

/* 前向声明 */
static void *event_loop_thread(void *arg);
static bool send_message(copland_message_header_t *header, void *data);
static bool receive_message(copland_message_header_t *header, void **data);
static bool handle_server_message(copland_message_header_t *header, void *data);
static uint32_t get_next_serial(void);
static client_buffer_t *create_client_buffer(uint32_t width, uint32_t height, uint32_t format);
static void destroy_client_buffer(client_buffer_t *buffer);

/* 创建客户端实例 */
copland_client_t *copland_client_create(void)
{
    copland_client_t *client = calloc(1, sizeof(copland_client_t));
    if (!client) {
        return NULL;
    }

    /* 初始化客户端 */
    client->connected = false;
    client->running = false;

    return client;
}

/* 连接到服务器 */
bool copland_client_connect(copland_client_t *client, const char *socket_path)
{
    if (!client || !socket_path) return false;

    /* 创建内部实例 */
    internal_client = calloc(1, sizeof(copland_client_internal_t));
    if (!internal_client) {
        return false;
    }

    internal_client->client = client;
    internal_client->connected = false;
    internal_client->running = false;
    internal_client->next_serial = 1;

    /* 初始化互斥锁 */
    pthread_mutex_init(&internal_client->message_mutex, NULL);
    pthread_mutex_init(&internal_client->buffer_mutex, NULL);

    /* 创建Unix域Socket */
    internal_client->server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (internal_client->server_fd < 0) {
        perror("创建客户端Socket失败");
        copland_client_destroy(client);
        return false;
    }

    /* 连接到服务器 */
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (connect(internal_client->server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("连接到服务器失败");
        copland_client_destroy(client);
        return false;
    }

    /* 设置非阻塞模式 */
    int flags = fcntl(internal_client->server_fd, F_GETFL, 0);
    fcntl(internal_client->server_fd, F_SETFL, flags | O_NONBLOCK);

    internal_client->connected = true;

    /* 启动事件循环线程 */
    internal_client->running = true;
    if (pthread_create(&internal_client->event_thread, NULL, event_loop_thread, internal_client) != 0) {
        perror("创建事件线程失败");
        copland_client_destroy(client);
        return false;
    }

    return true;
}

/* 断开连接 */
void copland_client_disconnect(copland_client_t *client)
{
    if (!client || !internal_client) return;

    internal_client->running = false;
    internal_client->connected = false;

    /* 等待事件线程结束 */
    if (internal_client->event_thread) {
        pthread_join(internal_client->event_thread, NULL);
    }

    /* 关闭连接 */
    if (internal_client->server_fd >= 0) {
        close(internal_client->server_fd);
        internal_client->server_fd = -1;
    }
}

/* 销毁客户端实例 */
void copland_client_destroy(copland_client_t *client)
{
    if (!client) return;

    /* 断开连接 */
    copland_client_disconnect(client);

    /* 清理内部实例 */
    if (internal_client) {
        /* 清理待处理消息 */
        pthread_mutex_lock(&internal_client->message_mutex);
        pending_message_t *msg = internal_client->pending_messages;
        while (msg) {
            pending_message_t *next = msg->next;
            free(msg->data);
            free(msg);
            msg = next;
        }
        internal_client->pending_messages = NULL;
        pthread_mutex_unlock(&internal_client->message_mutex);

        /* 清理缓冲区 */
        pthread_mutex_lock(&internal_client->buffer_mutex);
        client_buffer_t *buffer = internal_client->buffers;
        while (buffer) {
            client_buffer_t *next = buffer->next;
            destroy_client_buffer(buffer);
            buffer = next;
        }
        internal_client->buffers = NULL;
        pthread_mutex_unlock(&internal_client->buffer_mutex);

        /* 销毁互斥锁 */
        pthread_mutex_destroy(&internal_client->message_mutex);
        pthread_mutex_destroy(&internal_client->buffer_mutex);

        free(internal_client);
        internal_client = NULL;
    }

    free(client);
}

/* 创建窗口 */
uint32_t copland_client_create_window(copland_client_t *client, uint32_t parent_id,
                                     copland_window_type_t type, int32_t x, int32_t y,
                                     uint32_t width, uint32_t height, const char *title)
{
    if (!client || !internal_client || !internal_client->connected) {
        return 0;
    }

    copland_create_window_request_t req;
    memset(&req, 0, sizeof(req));

    req.header.magic = COPLAND_PROTOCOL_MAGIC;
    req.header.version = COPLAND_PROTOCOL_VERSION;
    req.header.type = COPLAND_MSG_CREATE_WINDOW;
    req.header.size = sizeof(req);
    req.header.object_id = 0;
    req.header.serial = get_next_serial();

    req.parent_id = parent_id;
    req.type = type;
    req.x = x;
    req.y = y;
    req.width = width;
    req.height = height;
    strncpy(req.title, title ? title : "", sizeof(req.title) - 1);

    if (send_message(&req.header, &req.parent_id)) {
        /* 返回窗口ID，这里应该等待服务器响应获取实际ID */
        return req.header.serial;  /* 临时使用序列号作为ID */
    }

    return 0;
}

/* 销毁窗口 */
bool copland_client_destroy_window(copland_client_t *client, uint32_t window_id)
{
    if (!client || !internal_client || !internal_client->connected) {
        return false;
    }

    copland_destroy_window_request_t req;
    memset(&req, 0, sizeof(req));

    req.header.magic = COPLAND_PROTOCOL_MAGIC;
    req.header.version = COPLAND_PROTOCOL_VERSION;
    req.header.type = COPLAND_MSG_DESTROY_WINDOW;
    req.header.size = sizeof(req);
    req.header.object_id = window_id;
    req.header.serial = get_next_serial();

    req.window_id = window_id;

    return send_message(&req.header, &req.window_id);
}

/* 移动窗口 */
bool copland_client_move_window(copland_client_t *client, uint32_t window_id, int32_t x, int32_t y)
{
    if (!client || !internal_client || !internal_client->connected) {
        return false;
    }

    copland_move_window_request_t req;
    memset(&req, 0, sizeof(req));

    req.header.magic = COPLAND_PROTOCOL_MAGIC;
    req.header.version = COPLAND_PROTOCOL_VERSION;
    req.header.type = COPLAND_MSG_MOVE_WINDOW;
    req.header.size = sizeof(req);
    req.header.object_id = window_id;
    req.header.serial = get_next_serial();

    req.window_id = window_id;
    req.x = x;
    req.y = y;

    return send_message(&req.header, &req.window_id);
}

/* 调整窗口大小 */
bool copland_client_resize_window(copland_client_t *client, uint32_t window_id, uint32_t width, uint32_t height)
{
    if (!client || !internal_client || !internal_client->connected) {
        return false;
    }

    copland_resize_window_request_t req;
    memset(&req, 0, sizeof(req));

    req.header.magic = COPLAND_PROTOCOL_MAGIC;
    req.header.version = COPLAND_PROTOCOL_VERSION;
    req.header.type = COPLAND_MSG_RESIZE_WINDOW;
    req.header.size = sizeof(req);
    req.header.object_id = window_id;
    req.header.serial = get_next_serial();

    req.window_id = window_id;
    req.width = width;
    req.height = height;

    return send_message(&req.header, &req.window_id);
}

/* 设置窗口标题 */
bool copland_client_set_window_title(copland_client_t *client, uint32_t window_id, const char *title)
{
    if (!client || !internal_client || !internal_client->connected) {
        return false;
    }

    copland_set_title_request_t req;
    memset(&req, 0, sizeof(req));

    req.header.magic = COPLAND_PROTOCOL_MAGIC;
    req.header.version = COPLAND_PROTOCOL_VERSION;
    req.header.type = COPLAND_MSG_SET_TITLE;
    req.header.size = sizeof(req);
    req.header.object_id = window_id;
    req.header.serial = get_next_serial();

    req.window_id = window_id;
    strncpy(req.title, title ? title : "", sizeof(req.title) - 1);

    return send_message(&req.header, &req.window_id);
}

/* 创建表面 */
uint32_t copland_client_create_surface(copland_client_t *client, uint32_t window_id, copland_surface_role_t role)
{
    if (!client || !internal_client || !internal_client->connected) {
        return 0;
    }

    copland_create_surface_request_t req;
    memset(&req, 0, sizeof(req));

    req.header.magic = COPLAND_PROTOCOL_MAGIC;
    req.header.version = COPLAND_PROTOCOL_VERSION;
    req.header.type = COPLAND_MSG_CREATE_SURFACE;
    req.header.size = sizeof(req);
    req.header.object_id = 0;
    req.header.serial = get_next_serial();

    req.window_id = window_id;
    req.role = role;

    if (send_message(&req.header, &req.window_id)) {
        return req.header.serial;  /* 临时使用序列号作为ID */
    }

    return 0;
}

/* 创建共享内存缓冲区 */
uint32_t copland_client_create_shm_buffer(copland_client_t *client, uint32_t width, uint32_t height,
                                         copland_buffer_format_t format, int shm_fd, size_t size)
{
    if (!client || !internal_client || !internal_client->connected) {
        return 0;
    }

    copland_create_shm_buffer_request_t req;
    memset(&req, 0, sizeof(req));

    req.header.magic = COPLAND_PROTOCOL_MAGIC;
    req.header.version = COPLAND_PROTOCOL_VERSION;
    req.header.type = COPLAND_MSG_CREATE_SHM_BUFFER;
    req.header.size = sizeof(req);
    req.header.object_id = 0;
    req.header.serial = get_next_serial();

    req.format = format;
    req.width = width;
    req.height = height;
    req.stride = width * 4;  /* 假设4字节每像素 */
    req.fd = shm_fd;
    req.size = size;

    if (send_message(&req.header, &req.format)) {
        return req.header.serial;  /* 临时使用序列号作为ID */
    }

    return 0;
}

/* 附加缓冲区到表面 */
bool copland_client_attach_buffer(copland_client_t *client, uint32_t surface_id, uint32_t buffer_id, int32_t x, int32_t y)
{
    if (!client || !internal_client || !internal_client->connected) {
        return false;
    }

    copland_attach_buffer_request_t req;
    memset(&req, 0, sizeof(req));

    req.header.magic = COPLAND_PROTOCOL_MAGIC;
    req.header.version = COPLAND_PROTOCOL_VERSION;
    req.header.type = COPLAND_MSG_ATTACH_BUFFER;
    req.header.size = sizeof(req);
    req.header.object_id = surface_id;
    req.header.serial = get_next_serial();

    req.surface_id = surface_id;
    req.buffer_id = buffer_id;
    req.x = x;
    req.y = y;

    return send_message(&req.header, &req.surface_id);
}

/* 提交表面 */
bool copland_client_commit_surface(copland_client_t *client, uint32_t surface_id)
{
    if (!client || !internal_client || !internal_client->connected) {
        return false;
    }

    copland_commit_surface_request_t req;
    memset(&req, 0, sizeof(req));

    req.header.magic = COPLAND_PROTOCOL_MAGIC;
    req.header.version = COPLAND_PROTOCOL_VERSION;
    req.header.type = COPLAND_MSG_COMMIT_SURFACE;
    req.header.size = sizeof(req);
    req.header.object_id = surface_id;
    req.header.serial = get_next_serial();

    req.surface_id = surface_id;

    return send_message(&req.header, &req.surface_id);
}

/* 发送鼠标按钮事件 */
bool copland_client_send_button_event(copland_client_t *client, uint32_t button, bool pressed, uint32_t time)
{
    if (!client || !internal_client || !internal_client->connected) {
        return false;
    }

    copland_button_event_t event;
    memset(&event, 0, sizeof(event));

    event.header.magic = COPLAND_PROTOCOL_MAGIC;
    event.header.version = COPLAND_PROTOCOL_VERSION;
    event.header.type = pressed ? COPLAND_MSG_BUTTON_DOWN : COPLAND_MSG_BUTTON_UP;
    event.header.size = sizeof(event);
    event.header.object_id = 0;
    event.header.serial = get_next_serial();

    event.time = time;
    event.button = button;
    event.state = pressed ? 1 : 0;

    return send_message(&event.header, &event.time);
}

/* 发送键盘事件 */
bool copland_client_send_key_event(copland_client_t *client, uint32_t keycode, bool pressed, uint32_t modifiers, uint32_t time)
{
    if (!client || !internal_client || !internal_client->connected) {
        return false;
    }

    copland_key_event_t event;
    memset(&event, 0, sizeof(event));

    event.header.magic = COPLAND_PROTOCOL_MAGIC;
    event.header.version = COPLAND_PROTOCOL_VERSION;
    event.header.type = pressed ? COPLAND_MSG_KEY_DOWN : COPLAND_MSG_KEY_UP;
    event.header.size = sizeof(event);
    event.header.object_id = 0;
    event.header.serial = get_next_serial();

    event.time = time;
    event.keycode = keycode;
    event.state = pressed ? 1 : 0;
    event.modifiers = modifiers;

    return send_message(&event.header, &event.time);
}

/* 发送鼠标移动事件 */
bool copland_client_send_motion_event(copland_client_t *client, double x, double y, uint32_t time)
{
    if (!client || !internal_client || !internal_client->connected) {
        return false;
    }

    copland_move_cursor_request_t event;
    memset(&event, 0, sizeof(event));

    event.header.magic = COPLAND_PROTOCOL_MAGIC;
    event.header.version = COPLAND_PROTOCOL_VERSION;
    event.header.type = COPLAND_MSG_MOVE_CURSOR;
    event.header.size = sizeof(event);
    event.header.object_id = 0;
    event.header.serial = get_next_serial();

    event.time = time;
    event.x = x;
    event.y = y;

    return send_message(&event.header, &event.time);
}

/* 发送触摸事件 */
bool copland_client_send_touch_event(copland_client_t *client, uint32_t touch_id, uint32_t type,
                                   double x, double y, uint32_t time)
{
    if (!client || !internal_client || !internal_client->connected) {
        return false;
    }

    copland_touch_event_t event;
    memset(&event, 0, sizeof(event));

    event.header.magic = COPLAND_PROTOCOL_MAGIC;
    event.header.version = COPLAND_PROTOCOL_VERSION;
    event.header.type = type;
    event.header.size = sizeof(event);
    event.header.object_id = 0;
    event.header.serial = get_next_serial();

    event.time = time;
    event.touch_id = touch_id;
    event.state = (type == COPLAND_MSG_TOUCH_DOWN) ? 1 : (type == COPLAND_MSG_TOUCH_UP) ? 0 : 2;
    event.x = x;
    event.y = y;

    return send_message(&event.header, &event.time);
}

/* 发送Ping */
bool copland_client_ping(copland_client_t *client, uint32_t ping_id)
{
    if (!client || !internal_client || !internal_client->connected) {
        return false;
    }

    copland_ping_request_t req;
    memset(&req, 0, sizeof(req));

    req.header.magic = COPLAND_PROTOCOL_MAGIC;
    req.header.version = COPLAND_PROTOCOL_VERSION;
    req.header.type = COPLAND_MSG_PING;
    req.header.size = sizeof(req);
    req.header.object_id = 0;
    req.header.serial = get_next_serial();

    req.ping_id = ping_id;

    return send_message(&req.header, &req.ping_id);
}

/* 获取服务器信息 */
bool copland_client_get_server_info(copland_client_t *client)
{
    if (!client || !internal_client || !internal_client->connected) {
        return false;
    }

    copland_get_server_info_request_t req;
    memset(&req, 0, sizeof(req));

    req.header.magic = COPLAND_PROTOCOL_MAGIC;
    req.header.version = COPLAND_PROTOCOL_VERSION;
    req.header.type = COPLAND_MSG_GET_SERVER_INFO;
    req.header.size = sizeof(req);
    req.header.object_id = 0;
    req.header.serial = get_next_serial();

    return send_message(&req.header, NULL);
}

/* 设置事件回调 */
void copland_client_set_event_handler(copland_client_t *client, copland_event_handler_t handler, void *user_data)
{
    if (!client || !internal_client) return;

    internal_client->event_handler = handler;
    internal_client->user_data = user_data;
}

/* 事件循环线程 */
static void *event_loop_thread(void *arg)
{
    copland_client_internal_t *internal = (copland_client_internal_t *)arg;
    struct pollfd pfd;
    copland_message_header_t header;
    void *data = NULL;

    /* 设置poll结构 */
    pfd.fd = internal->server_fd;
    pfd.events = POLLIN;

    while (internal->running && internal->connected) {
        /* 等待服务器消息 */
        int ret = poll(&pfd, 1, 100);  /* 100ms超时 */

        if (ret < 0) {
            if (errno == EINTR) continue;
            perror("poll失败");
            break;
        }

        if (ret == 0) continue;  /* 超时 */

        /* 接收消息 */
        if (pfd.revents & POLLIN) {
            if (receive_message(&header, &data)) {
                /* 处理服务器消息 */
                handle_server_message(&header, data);
            }
        }

        free(data);
        data = NULL;
    }

    return NULL;
}

/* 发送消息 */
static bool send_message(copland_message_header_t *header, void *data)
{
    if (!internal_client || !internal_client->connected) {
        return false;
    }

    ssize_t n;
    size_t total_size = header->size;
    size_t sent = 0;

    /* 发送消息头 */
    n = write(internal_client->server_fd, header, sizeof(*header));
    if (n != sizeof(*header)) {
        return false;
    }
    sent += n;

    /* 发送消息数据 */
    if (data && sent < total_size) {
        n = write(internal_client->server_fd, data, total_size - sent);
        if (n != (ssize_t)(total_size - sent)) {
            return false;
        }
    }

    return true;
}

/* 接收消息 */
static bool receive_message(copland_message_header_t *header, void **data)
{
    if (!internal_client || !internal_client->connected) {
        return false;
    }

    ssize_t n;

    /* 读取消息头 */
    n = read(internal_client->server_fd, header, sizeof(*header));
    if (n != sizeof(*header)) {
        if (n == 0) {
            /* 连接关闭 */
            internal_client->connected = false;
        }
        return false;
    }

    /* 验证消息头 */
    if (!copland_validate_message(header)) {
        fprintf(stderr, "无效的消息头\n");
        return false;
    }

    /* 读取消息数据 */
    if (header->size > sizeof(*header)) {
        *data = malloc(header->size - sizeof(*header));
        if (!*data) {
            fprintf(stderr, "内存分配失败\n");
            return false;
        }

        n = read(internal_client->server_fd, *data, header->size - sizeof(*header));
        if (n != (ssize_t)(header->size - sizeof(*header))) {
            free(*data);
            *data = NULL;
            return false;
        }
    }

    return true;
}

/* 处理服务器消息 */
static bool handle_server_message(copland_message_header_t *header, void *data)
{
    if (!internal_client || !internal_client->event_handler) {
        return false;
    }

    /* 调用用户事件处理回调 */
    return internal_client->event_handler(internal_client->client, header, data, internal_client->user_data);
}

/* 获取下一个序列号 */
static uint32_t get_next_serial(void)
{
    if (!internal_client) return 0;

    return internal_client->next_serial++;
}

/* 创建客户端缓冲区 */
static client_buffer_t *create_client_buffer(uint32_t width, uint32_t height, uint32_t format)
{
    client_buffer_t *buffer = calloc(1, sizeof(client_buffer_t));
    if (!buffer) {
        return NULL;
    }

    buffer->width = width;
    buffer->height = height;
    buffer->format = format;
    buffer->stride = width * 4;  /* 假设4字节每像素 */

    /* 创建共享内存 */
    buffer->size = buffer->stride * height;
    buffer->shm_fd = shm_open("/copland_buffer", O_CREAT | O_RDWR, 0600);
    if (buffer->shm_fd < 0) {
        free(buffer);
        return NULL;
    }

    /* 调整共享内存大小 */
    if (ftruncate(buffer->shm_fd, buffer->size) < 0) {
        close(buffer->shm_fd);
        shm_unlink("/copland_buffer");
        free(buffer);
        return NULL;
    }

    /* 映射共享内存 */
    buffer->data = mmap(NULL, buffer->size, PROT_READ | PROT_WRITE, MAP_SHARED, buffer->shm_fd, 0);
    if (buffer->data == MAP_FAILED) {
        close(buffer->shm_fd);
        shm_unlink("/copland_buffer");
        free(buffer);
        return NULL;
    }

    return buffer;
}

/* 销毁客户端缓冲区 */
static void destroy_client_buffer(client_buffer_t *buffer)
{
    if (!buffer) return;

    if (buffer->data && buffer->data != MAP_FAILED) {
        munmap(buffer->data, buffer->size);
    }

    if (buffer->shm_fd >= 0) {
        close(buffer->shm_fd);
        shm_unlink("/copland_buffer");
    }

    free(buffer);
}

/* 协议工具函数实现 */

/* 消息序列化 */
size_t copland_serialize_message(void *buffer, const copland_message_header_t *header)
{
    if (!buffer || !header) return 0;

    memcpy(buffer, header, sizeof(*header));
    return sizeof(*header);
}

/* 消息反序列化 */
size_t copland_deserialize_message(const void *buffer, copland_message_header_t *header)
{
    if (!buffer || !header) return 0;

    memcpy(header, buffer, sizeof(*header));
    return sizeof(*header);
}

/* 消息验证 */
bool copland_validate_message(const copland_message_header_t *header)
{
    if (!header) return false;

    return (header->magic == COPLAND_PROTOCOL_MAGIC &&
            header->version == COPLAND_PROTOCOL_VERSION &&
            header->size >= sizeof(*header));
}

/* 错误码转字符串 */
const char *copland_error_to_string(copland_error_t error)
{
    switch (error) {
        case COPLAND_ERROR_NONE: return "无错误";
        case COPLAND_ERROR_PERMISSION_DENIED: return "权限被拒绝";
        case COPLAND_ERROR_NOT_FOUND: return "未找到";
        case COPLAND_ERROR_ALREADY_EXISTS: return "已存在";
        case COPLAND_ERROR_INVALID_ARGUMENT: return "无效参数";
        case COPLAND_ERROR_INVALID_OBJECT: return "无效对象";
        case COPLAND_ERROR_OUT_OF_MEMORY: return "内存不足";
        case COPLAND_ERROR_IO: return "I/O错误";
        case COPLAND_ERROR_PROTOCOL: return "协议错误";
        case COPLAND_ERROR_NOT_SUPPORTED: return "不支持";
        case COPLAND_ERROR_SURFACE_BUSY: return "表面忙";
        case COPLAND_ERROR_SURFACE_EXISTS: return "表面已存在";
        case COPLAND_ERROR_BUFFER_EXISTS: return "缓冲区已存在";
        case COPLAND_ERROR_WINDOW_EXISTS: return "窗口已存在";
        default: return "未知错误";
    }
}

/* 消息类型转字符串 */
const char *copland_message_type_to_string(copland_message_type_t type)
{
    switch (type) {
        case COPLAND_MSG_CREATE_WINDOW: return "创建窗口";
        case COPLAND_MSG_DESTROY_WINDOW: return "销毁窗口";
        case COPLAND_MSG_MOVE_WINDOW: return "移动窗口";
        case COPLAND_MSG_RESIZE_WINDOW: return "调整窗口大小";
        case COPLAND_MSG_CREATE_SURFACE: return "创建表面";
        case COPLAND_MSG_ATTACH_BUFFER: return "附加缓冲区";
        case COPLAND_MSG_PING: return "Ping";
        case COPLAND_MSG_PONG: return "Pong";
        default: return "未知消息类型";
    }
}

/* 缓冲区格式转字符串 */
const char *copland_buffer_format_to_string(copland_buffer_format_t format)
{
    switch (format) {
        case COPLAND_BUFFER_FORMAT_ARGB8888: return "ARGB8888";
        case COPLAND_BUFFER_FORMAT_XRGB8888: return "XRGB8888";
        case COPLAND_BUFFER_FORMAT_RGB565: return "RGB565";
        case COPLAND_BUFFER_FORMAT_NV12: return "NV12";
        case COPLAND_BUFFER_FORMAT_YUV420: return "YUV420";
        default: return "未知格式";
    }
}

/* 窗口类型转字符串 */
const char *copland_window_type_to_string(copland_window_type_t type)
{
    switch (type) {
        case COPLAND_WINDOW_TOPLEVEL: return "顶级窗口";
        case COPLAND_WINDOW_POPUP: return "弹出窗口";
        case COPLAND_WINDOW_TRANSIENT: return "临时窗口";
        case COPLAND_WINDOW_DND: return "拖拽窗口";
        case COPLAND_WINDOW_SUBSURFACE: return "子表面";
        case COPLAND_WINDOW_CURSOR: return "光标窗口";
        default: return "未知窗口类型";
    }
}

/* 表面角色转字符串 */
const char *copland_surface_role_to_string(copland_surface_role_t role)
{
    switch (role) {
        case COPLAND_SURFACE_ROLE_NONE: return "无角色";
        case COPLAND_SURFACE_ROLE_CURSOR: return "光标表面";
        case COPLAND_SURFACE_ROLE_DND: return "拖拽表面";
        case COPLAND_SURFACE_ROLE_TOOLTIP: return "工具提示表面";
        case COPLAND_SURFACE_ROLE_POPUP: return "弹出表面";
        default: return "未知表面角色";
    }
}
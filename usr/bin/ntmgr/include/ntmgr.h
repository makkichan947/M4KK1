/*
 * M4KK1 Ntmgr网络管理器 - 主接口头文件
 * 定义TUI网络管理器的核心数据结构和接口
 */

#ifndef __NTMGR_H__
#define __NTMGR_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>

/* 版本信息 */
#define NTMGR_VERSION_MAJOR 1
#define NTMGR_VERSION_MINOR 0
#define NTMGR_VERSION_PATCH 0
#define NTMGR_VERSION_STRING "1.0.0"

/* 常量定义 */
#define MAX_INTERFACES      32
#define MAX_CONNECTIONS     64
#define MAX_VPN_CONNECTIONS 16
#define MAX_PROXY_SERVERS   16
#define MAX_FIREWALL_RULES  256
#define MAX_DNS_SERVERS     8

/* 网络接口类型枚举 */
typedef enum {
    IF_TYPE_ETHERNET = 0,   /* 以太网 */
    IF_TYPE_WIFI = 1,       /* Wi-Fi */
    IF_TYPE_BLUETOOTH = 2,  /* 蓝牙 */
    IF_TYPE_TUNNEL = 3,     /* 隧道 */
    IF_TYPE_BRIDGE = 4,     /* 网桥 */
    IF_TYPE_VLAN = 5,       /* VLAN */
    IF_TYPE_LOOPBACK = 6,   /* 回环 */
    IF_TYPE_UNKNOWN = 7     /* 未知 */
} interface_type_t;

/* 连接状态枚举 */
typedef enum {
    CONN_STATE_DISCONNECTED = 0, /* 未连接 */
    CONN_STATE_CONNECTING = 1,   /* 连接中 */
    CONN_STATE_CONNECTED = 2,    /* 已连接 */
    CONN_STATE_DISCONNECTING = 3, /* 断开中 */
    CONN_STATE_FAILED = 4,       /* 连接失败 */
    CONN_STATE_UNKNOWN = 5       /* 未知状态 */
} connection_state_t;

/* VPN协议枚举 */
typedef enum {
    VPN_PROTO_OPENVPN = 0,      /* OpenVPN */
    VPN_PROTO_WIREGUARD = 1,    /* WireGuard */
    VPN_PROTO_IPSEC = 2,        /* IPsec */
    VPN_PROTO_PPTP = 3,         /* PPTP */
    VPN_PROTO_L2TP = 4,         /* L2TP */
    VPN_PROTO_SSTP = 5,         /* SSTP */
    VPN_PROTO_IKEV2 = 6,        /* IKEv2 */
    VPN_PROTO_UNKNOWN = 7       /* 未知协议 */
} vpn_protocol_t;

/* 代理类型枚举 */
typedef enum {
    PROXY_TYPE_HTTP = 0,        /* HTTP代理 */
    PROXY_TYPE_SOCKS4 = 1,      /* SOCKS4代理 */
    PROXY_TYPE_SOCKS5 = 2,      /* SOCKS5代理 */
    PROXY_TYPE_TRANSPARENT = 3, /* 透明代理 */
    PROXY_TYPE_SYSTEM = 4       /* 系统代理 */
} proxy_type_t;

/* 防火墙后端枚举 */
typedef enum {
    FW_BACKEND_IPTABLES = 0,    /* iptables */
    FW_BACKEND_NFTABLES = 1,    /* nftables */
    FW_BACKEND_UFW = 2,         /* UFW */
    FW_BACKEND_FIREWALD = 3     /* firewalld */
} firewall_backend_t;

/* 网络接口结构 */
typedef struct network_interface {
    char *name;             /* 接口名称 */
    char *display_name;     /* 显示名称 */
    interface_type_t type;  /* 接口类型 */

    char *mac_address;      /* MAC地址 */
    char **ip_addresses;    /* IP地址列表 */
    size_t ip_count;        /* IP地址数量 */

    connection_state_t state; /* 连接状态 */
    uint64_t rx_bytes;      /* 接收字节数 */
    uint64_t tx_bytes;      /* 发送字节数 */
    uint64_t rx_packets;    /* 接收数据包数 */
    uint64_t tx_packets;    /* 发送数据包数 */

    int mtu;                /* MTU值 */
    bool is_up;             /* 是否启用 */
    bool is_wireless;       /* 是否无线 */
    bool is_virtual;        /* 是否虚拟 */

    struct network_interface *next; /* 下一个接口 */
} network_interface_t;

/* VPN连接结构 */
typedef struct vpn_connection {
    char *name;             /* 连接名称 */
    char *display_name;     /* 显示名称 */
    vpn_protocol_t protocol; /* VPN协议 */

    char *server;           /* 服务器地址 */
    int port;               /* 端口号 */
    char *username;         /* 用户名 */
    char *password;         /* 密码 */

    char *config_file;      /* 配置文件路径 */
    char *certificate;      /* 证书路径 */
    char *private_key;      /* 私钥路径 */

    connection_state_t state; /* 连接状态 */
    time_t connected_time;  /* 连接时间 */
    char *local_ip;         /* 本地IP地址 */
    char *remote_ip;        /* 远程IP地址 */

    bool auto_connect;      /* 自动连接 */
    bool kill_switch;       /* 网络锁定 */

    struct vpn_connection *next; /* 下一个连接 */
} vpn_connection_t;

/* 代理服务器结构 */
typedef struct proxy_server {
    char *name;             /* 代理名称 */
    proxy_type_t type;      /* 代理类型 */
    char *host;             /* 代理主机 */
    int port;               /* 代理端口 */

    char *username;         /* 用户名 */
    char *password;         /* 密码 */

    bool enabled;           /* 是否启用 */
    bool authentication;    /* 是否需要认证 */
    int timeout;            /* 超时时间 */

    struct proxy_server *next; /* 下一个代理 */
} proxy_server_t;

/* 防火墙规则结构 */
typedef struct firewall_rule {
    char *name;             /* 规则名称 */
    char *description;      /* 规则描述 */

    char *source;           /* 源地址 */
    char *destination;      /* 目的地址 */
    char *protocol;         /* 协议 */
    char *port;             /* 端口范围 */

    char *action;           /* 动作 (ACCEPT/DROP/REJECT) */
    char *direction;        /* 方向 (IN/OUT/FORWARD) */

    bool enabled;           /* 是否启用 */
    int priority;           /* 优先级 */

    struct firewall_rule *next; /* 下一个规则 */
} firewall_rule_t;

/* 监控统计结构 */
typedef struct monitor_stats {
    time_t timestamp;       /* 时间戳 */

    /* 网络流量统计 */
    uint64_t total_rx_bytes; /* 总接收字节数 */
    uint64_t total_tx_bytes; /* 总发送字节数 */
    double rx_rate;         /* 接收速率 */
    double tx_rate;         /* 发送速率 */

    /* 连接统计 */
    size_t active_connections; /* 活动连接数 */
    size_t total_connections;  /* 总连接数 */

    /* VPN统计 */
    size_t active_vpn;      /* 活动VPN连接数 */
    uint64_t vpn_rx_bytes;  /* VPN接收字节数 */
    uint64_t vpn_tx_bytes;  /* VPN发送字节数 */

    /* 代理统计 */
    size_t active_proxies;  /* 活动代理数 */
    uint64_t proxy_requests; /* 代理请求数 */

    struct monitor_stats *next; /* 下一个统计 */
} monitor_stats_t;

/* 配置结构 */
typedef struct config {
    /* 网络设置 */
    char **dns_servers;     /* DNS服务器列表 */
    size_t dns_count;       /* DNS服务器数量 */
    char **search_domains;  /* 搜索域列表 */
    size_t search_count;    /* 搜索域数量 */

    /* VPN设置 */
    bool vpn_auto_connect;  /* VPN自动连接 */
    bool vpn_kill_switch;   /* VPN网络锁定 */
    char *vpn_config_dir;   /* VPN配置目录 */

    /* 代理设置 */
    bool proxy_enabled;     /* 代理启用 */
    proxy_type_t proxy_type; /* 默认代理类型 */
    char *proxy_config;     /* 代理配置 */

    /* 防火墙设置 */
    bool firewall_enabled;  /* 防火墙启用 */
    firewall_backend_t firewall_backend; /* 防火墙后端 */
    char *firewall_policy;  /* 默认策略 */

    /* 监控设置 */
    bool monitoring_enabled; /* 监控启用 */
    int monitoring_interval; /* 监控间隔 */
    size_t max_history;     /* 最大历史记录 */

    /* 界面设置 */
    char *theme;            /* 主题名称 */
    char *layout;           /* 布局类型 */
    bool show_advanced;     /* 显示高级选项 */
} config_t;

/* 应用状态结构 */
typedef struct app_state {
    network_interface_t **interfaces; /* 网络接口列表 */
    size_t interface_count; /* 接口数量 */

    vpn_connection_t **vpn_connections; /* VPN连接列表 */
    size_t vpn_count;       /* VPN连接数量 */

    proxy_server_t **proxy_servers; /* 代理服务器列表 */
    size_t proxy_count;     /* 代理服务器数量 */

    firewall_rule_t **firewall_rules; /* 防火墙规则列表 */
    size_t firewall_count;  /* 防火墙规则数量 */

    monitor_stats_t *monitor_history; /* 监控历史 */
    size_t monitor_count;   /* 监控历史数量 */

    config_t *config;       /* 配置 */
    char *config_file;      /* 配置文件路径 */

    connection_state_t network_state; /* 网络状态 */
    bool running;           /* 运行状态 */

    void *ui_context;       /* UI上下文 */
    void *plugin_context;   /* 插件上下文 */
} app_state_t;

/* 函数声明 */

/* 应用初始化和清理 */
app_state_t *ntmgr_create(void);
void ntmgr_destroy(app_state_t *app);
bool ntmgr_init(app_state_t *app);
void ntmgr_run(app_state_t *app);
void ntmgr_quit(app_state_t *app);

/* 网络接口管理 */
network_interface_t **interface_scan(void);
network_interface_t *interface_get_by_name(const char *name);
bool interface_up(network_interface_t *interface);
bool interface_down(network_interface_t *interface);
bool interface_set_ip(network_interface_t *interface, const char *ip, const char *netmask);
bool interface_set_dns(network_interface_t *interface, const char **dns_servers, size_t count);

/* VPN连接管理 */
vpn_connection_t *vpn_connection_create(const char *name, vpn_protocol_t protocol);
void vpn_connection_destroy(vpn_connection_t *connection);
bool vpn_connection_connect(vpn_connection_t *connection);
bool vpn_connection_disconnect(vpn_connection_t *connection);
connection_state_t vpn_connection_get_state(vpn_connection_t *connection);

/* 代理服务器管理 */
proxy_server_t *proxy_server_create(const char *name, proxy_type_t type, const char *host, int port);
void proxy_server_destroy(proxy_server_t *server);
bool proxy_server_enable(proxy_server_t *server);
bool proxy_server_disable(proxy_server_t *server);
bool proxy_server_test(proxy_server_t *server);

/* 防火墙管理 */
firewall_rule_t *firewall_rule_create(const char *name, const char *action, const char *direction);
void firewall_rule_destroy(firewall_rule_t *rule);
bool firewall_rule_add(firewall_rule_t *rule);
bool firewall_rule_remove(firewall_rule_t *rule);
bool firewall_enable(void);
bool firewall_disable(void);

/* 监控系统 */
bool monitor_start(app_state_t *app);
void monitor_stop(app_state_t *app);
monitor_stats_t *monitor_get_current_stats(app_state_t *app);
bool monitor_get_history(app_state_t *app, time_t start, time_t end, monitor_stats_t **stats, size_t *count);

/* 配置管理 */
config_t *config_create(void);
void config_destroy(config_t *config);
bool config_load(config_t *config, const char *filename);
bool config_save(config_t *config, const char *filename);
void config_set_default(config_t *config);

/* 工具函数 */
const char *interface_type_to_string(interface_type_t type);
const char *connection_state_to_string(connection_state_t state);
const char *vpn_protocol_to_string(vpn_protocol_t protocol);
const char *proxy_type_to_string(proxy_type_t type);
const char *firewall_backend_to_string(firewall_backend_t backend);

/* 错误处理 */
typedef enum {
    NTMGR_OK = 0,                    /* 成功 */
    NTMGR_ERROR_PERMISSION = 1,      /* 权限错误 */
    NTMGR_ERROR_NOT_FOUND = 2,       /* 未找到 */
    NTMGR_ERROR_ALREADY_EXISTS = 3,  /* 已存在 */
    NTMGR_ERROR_INVALID_ARG = 4,     /* 无效参数 */
    NTMGR_ERROR_CONNECTION_FAILED = 5, /* 连接失败 */
    NTMGR_ERROR_TIMEOUT = 6,         /* 超时 */
    NTMGR_ERROR_NOT_SUPPORTED = 7,   /* 不支持 */
    NTMGR_ERROR_BUSY = 8,            /* 忙碌 */
    NTMGR_ERROR_CANCELLED = 9        /* 已取消 */
} ntmgr_error_t;

const char *ntmgr_error_to_string(ntmgr_error_t error);

#endif /* __NTMGR_H__ */
# M4KK1 Ntmgr网络管理器设计文档

## 概述

M4KK1 Ntmgr是一个先进的终端用户界面（TUI）网络管理器，提供了直观、美观、功能强大的网络配置和管理体验。与传统的网络管理工具不同，Ntmgr不仅支持基础的网络连接管理，还集成了VPN、代理、防火墙、网络监控等高级功能。

## 设计目标

- **界面友好**：美观的TUI界面，易于操作和配置
- **功能全面**：支持有线、无线、VPN、代理等多种网络类型
- **监控实时**：实时网络状态监控和流量统计
- **配置灵活**：支持多种配置文件格式和动态配置
- **扩展性强**：插件系统支持，易于功能扩展

## 核心特性

### 网络连接管理

- **有线连接**：Ethernet、DSL、Cable等有线网络
- **无线连接**：Wi-Fi、Bluetooth、NFC等无线网络
- **虚拟接口**：隧道、桥接、VLAN等虚拟网络接口
- **自动连接**：智能网络选择和自动切换
- **故障诊断**：网络连接问题自动诊断和修复

### VPN支持

- **协议支持**：OpenVPN、WireGuard、IPsec、PPTP、L2TP
- **证书管理**：数字证书、密钥对、私钥管理
- **路由配置**：VPN路由表自动配置和优化
- **负载均衡**：多VPN连接负载均衡和故障转移
- **安全策略**：VPN连接安全策略和访问控制

### 代理服务器

- **代理类型**：HTTP、SOCKS4、SOCKS5、透明代理
- **代理链**：多级代理链路配置和优化
- **负载均衡**：代理服务器负载均衡和故障转移
- **认证支持**：代理服务器认证和授权
- **规则引擎**：智能代理规则和例外处理

### 防火墙管理

- **规则管理**：iptables、nftables规则配置
- **策略模板**：预定义安全策略模板
- **实时监控**：防火墙规则执行情况监控
- **日志分析**：防火墙日志收集和分析
- **入侵检测**：基础入侵检测和响应

### 网络监控

- **流量监控**：实时流量统计和历史记录
- **性能监控**：网络延迟、带宽、丢包率监控
- **服务监控**：网络服务可用性和性能监控
- **设备发现**：网络设备自动发现和拓扑识别
- **警报系统**：网络异常检测和警报通知

## 架构设计

### 系统架构图

```
┌─────────────────────────────────────────────────────────────┐
│                    Ntmgr网络管理器                          │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │   主界面    │  │   连接管理   │  │   监控面板   │           │
│  │  (Main UI)   │  │  (Connection)│  │  (Monitor)  │           │
│  └─────────────┘  └─────────────┘  └─────────────┘           │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │   VPN管理    │  │   代理管理   │  │   防火墙    │           │
│  │   (VPN)     │  │  (Proxy)    │  │  (Firewall) │           │
│  └─────────────┘  └─────────────┘  └─────────────┘           │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │ 网络引擎    │  │   配置管理   │  │   插件系统   │           │
│  │  (Network)  │  │  (Config)   │  │  (Plugin)   │           │
│  └─────────────┘  └─────────────┘  └─────────────┘           │
└─────────────────────────────────────────────────────────────┘
```

### 模块关系图

```
┌─────────────────┐    ┌─────────────────┐
│     主程序       │───▶│    配置管理      │
│    (main.c)     │    │   (config.c)    │
└─────────────────┘    └─────────────────┘
         │                       │
         ▼                       ▼
┌─────────────────┐    ┌─────────────────┐
│   界面管理器     │    │   连接管理器     │
│  (ui_manager)   │◄──►│ (connection_mgr)│
└─────────────────┘    └─────────────────┘
         │                       │
         ▼                       ▼
┌─────────────────┐    ┌─────────────────┐
│   网络引擎       │    │   VPN管理器      │
│  (network_eng)  │◄──►│ (vpn_manager)   │
└─────────────────┘    └─────────────────┘
         │                       │
         ▼                       ▼
┌─────────────────┐    ┌─────────────────┐
│   监控引擎       │    │   代理管理器     │
│  (monitor_eng)  │◄──►│ (proxy_manager) │
└─────────────────┘    └─────────────────┘
```

## 数据结构设计

### 网络接口结构

```c
// 接口类型枚举
typedef enum {
    IF_ETHERNET = 0,        /* 以太网 */
    IF_WIFI = 1,            /* Wi-Fi */
    IF_BLUETOOTH = 2,       /* 蓝牙 */
    IF_TUNNEL = 3,          /* 隧道 */
    IF_BRIDGE = 4,          /* 网桥 */
    IF_VLAN = 5,            /* VLAN */
    IF_LOOPBACK = 6,        /* 回环 */
    IF_UNKNOWN = 7          /* 未知 */
} interface_type_t;

// 接口状态枚举
typedef enum {
    IF_DOWN = 0,            /* 关闭 */
    IF_UP = 1,              /* 开启 */
    IF_UNKNOWN_STATE = 2    /* 未知状态 */
} interface_state_t;

// 网络接口结构
typedef struct {
    char *name;             /* 接口名称 (eth0, wlan0) */
    char *display_name;     /* 显示名称 */
    interface_type_t type;  /* 接口类型 */
    interface_state_t state; /* 接口状态 */

    char *mac_address;      /* MAC地址 */
    char **ip_addresses;    /* IP地址列表 */
    size_t ip_count;        /* IP地址数量 */

    uint64_t rx_bytes;      /* 接收字节数 */
    uint64_t tx_bytes;      /* 发送字节数 */
    uint64_t rx_packets;    /* 接收数据包数 */
    uint64_t tx_packets;    /* 发送数据包数 */
    uint64_t rx_errors;     /* 接收错误数 */
    uint64_t tx_errors;     /* 发送错误数 */

    int mtu;                /* MTU值 */
    int tx_queue_len;       /* 发送队列长度 */

    struct network_interface *next; /* 下一个接口 */
} network_interface_t;
```

### VPN连接结构

```c
// VPN协议枚举
typedef enum {
    VPN_OPENVPN = 0,        /* OpenVPN */
    VPN_WIREGUARD = 1,      /* WireGuard */
    VPN_IPSEC = 2,          /* IPsec */
    VPN_PPTP = 3,           /* PPTP */
    VPN_L2TP = 4,           /* L2TP */
    VPN_SSTP = 5,           /* SSTP */
    VPN_IKEV2 = 6,          /* IKEv2 */
    VPN_UNKNOWN = 7         /* 未知协议 */
} vpn_protocol_t;

// VPN连接结构
typedef struct {
    char *name;             /* 连接名称 */
    vpn_protocol_t protocol; /* VPN协议 */
    char *server;           /* 服务器地址 */
    int port;               /* 端口号 */

    char *username;         /* 用户名 */
    char *password;         /* 密码 */
    char *certificate;      /* 证书路径 */
    char *private_key;      /* 私钥路径 */
    char *ca_certificate;   /* CA证书路径 */

    char *local_ip;         /* 本地IP地址 */
    char *remote_ip;        /* 远程IP地址 */
    char *dns_servers;      /* DNS服务器 */

    bool compression;       /* 启用压缩 */
    bool auto_connect;      /* 自动连接 */
    bool kill_switch;       /* 网络锁定 */

    struct vpn_connection *next; /* 下一个连接 */
} vpn_connection_t;
```

### 代理配置结构

```c
// 代理类型枚举
typedef enum {
    PROXY_HTTP = 0,         /* HTTP代理 */
    PROXY_SOCKS4 = 1,       /* SOCKS4代理 */
    PROXY_SOCKS5 = 2,       /* SOCKS5代理 */
    PROXY_TRANSPARENT = 3,  /* 透明代理 */
    PROXY_SYSTEM = 4        /* 系统代理 */
} proxy_type_t;

// 代理服务器结构
typedef struct {
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

// 代理规则结构
typedef struct {
    char *pattern;          /* 匹配模式 */
    proxy_server_t *proxy;  /* 使用的代理 */
    bool enabled;           /* 是否启用 */
    int priority;           /* 优先级 */

    struct proxy_rule *next; /* 下一个规则 */
} proxy_rule_t;
```

## 用户界面设计

### 主界面布局

```
┌─────────────────────────────────────────────────────────────┐
│ Ntmgr: Network Manager v1.0                                 │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────┬─────────────────┬─────────────────────┐ │
│  │    接口列表       │    连接状态       │      监控面板        │ │
│  │   (Interfaces)  │   (Connections)  │     (Monitor)       │ │
│  │                 │                 │                     │ │
│  │  ┌─────────────┐ │  ┌─────────────┐ │  ┌─────────────────┐ │ │
│  │  │ eth0 ↑↓     │ │  │ VPN: OpenVPN│ │  │ Traffic: 1.2MB  │ │ │
│  │  │ wlan0 ↑↓    │ │  │ Proxy: SOCKS│ │  │ Upload: 500KB/s │ │ │
│  │  │ tun0 ↑↓     │ │  │ Firewall: On│ │  │ Download: 2MB/s │ │ │
│  │  └─────────────┘ │  └─────────────┘ │  └─────────────────┘ │ │
│  │                 │                 │                     │ │
│  │  [Connect]      │  [Disconnect]    │  [Settings]         │ │
│  └─────────────────┴─────────────────┴─────────────────────┘ │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ Status: Connected | VPN: Connected | Firewall: Active    │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### 菜单结构

```
Network (N)
├── Interfaces (I)
│   ├── Ethernet (E)
│   ├── Wi-Fi (W)
│   ├── Bluetooth (B)
│   └── Virtual (V)
├── VPN (V)
│   ├── OpenVPN (O)
│   ├── WireGuard (W)
│   ├── IPsec (I)
│   └── PPTP (P)
├── Proxy (P)
│   ├── HTTP Proxy (H)
│   ├── SOCKS Proxy (S)
│   └── Proxy Rules (R)
└── Firewall (F)
    ├── Rules (R)
    ├── Policies (P)
    └── Logs (L)

Tools (T)
├── Network Scan (S)
├── Speed Test (T)
├── Traffic Monitor (M)
├── Diagnostics (D)
└── Backup Config (B)

Settings (S)
├── Preferences (P)
├── Themes (T)
├── Keybindings (K)
└── Plugins (P)

Help (H)
├── User Guide (U)
├── About (A)
└── License (L)
```

### 键盘快捷键

```c
// 通用快捷键
Ctrl+Q          /* 退出 */
Ctrl+S          /* 保存配置 */
Ctrl+R          /* 刷新 */
F1              /* 帮助 */

// 导航快捷键
Tab             /* 切换面板 */
Arrow Keys      /* 导航 */
Enter           /* 确认/进入 */
Esc             /* 返回/取消 */

// 网络操作
Ctrl+C          /* 连接 */
Ctrl+D          /* 断开 */
Ctrl+E          /* 编辑 */
Ctrl+N          /* 新建 */

// VPN操作
Ctrl+V          /* VPN开关 */
Ctrl+P          /* 代理开关 */
Ctrl+F          /* 防火墙开关 */

// 监控快捷键
F5              /* 刷新监控 */
F6              /* 流量统计 */
F7              /* 连接测试 */
F8              /* 诊断工具 */
```

## 技术实现

### 网络接口管理

```c
// 接口管理器结构
typedef struct {
    network_interface_t **interfaces; /* 接口列表 */
    size_t count;            /* 接口数量 */
    size_t capacity;         /* 容量 */

    interface_monitor_t *monitor; /* 接口监控器 */
    config_t *config;        /* 接口配置 */

    pthread_mutex_t mutex;   /* 互斥锁 */
} interface_manager_t;

// 接口监控结构
typedef struct {
    bool enabled;            /* 是否启用监控 */
    int interval;            /* 监控间隔 (秒) */

    stats_callback_t on_stats_update; /* 统计更新回调 */
    status_callback_t on_status_change; /* 状态改变回调 */

    pthread_t thread;        /* 监控线程 */
    bool running;            /* 运行状态 */
} interface_monitor_t;
```

### VPN管理系统

```c
// VPN管理器结构
typedef struct {
    vpn_connection_t **connections; /* VPN连接列表 */
    size_t count;            /* 连接数量 */

    vpn_protocol_t default_protocol; /* 默认协议 */
    char *default_config_dir; /* 默认配置目录 */

    bool auto_connect;       /* 自动连接 */
    bool kill_switch;        /* 网络锁定 */

    pthread_mutex_t mutex;   /* 互斥锁 */
} vpn_manager_t;

// VPN连接状态
typedef struct {
    vpn_connection_t *connection; /* VPN连接 */
    connection_state_t state; /* 连接状态 */
    time_t connected_time;   /* 连接时间 */
    uint64_t rx_bytes;       /* 接收字节数 */
    uint64_t tx_bytes;       /* 发送字节数 */

    char *local_ip;          /* 本地IP */
    char *remote_ip;         /* 远程IP */
    char *dns_servers;       /* DNS服务器 */

    error_t last_error;      /* 最后错误 */
} vpn_status_t;
```

### 代理管理系统

```c
// 代理管理器结构
typedef struct {
    proxy_server_t **servers; /* 代理服务器列表 */
    size_t server_count;     /* 服务器数量 */

    proxy_rule_t **rules;    /* 代理规则列表 */
    size_t rule_count;       /* 规则数量 */

    bool system_proxy;       /* 系统代理模式 */
    proxy_server_t *active_server; /* 活动代理服务器 */

    pthread_mutex_t mutex;   /* 互斥锁 */
} proxy_manager_t;

// 代理规则引擎
typedef struct {
    proxy_rule_t **rules;    /* 规则列表 */
    size_t count;            /* 规则数量 */

    rule_match_t match;      /* 规则匹配函数 */
    rule_priority_t priority; /* 优先级比较函数 */

    cache_t *cache;          /* 规则缓存 */
} proxy_rule_engine_t;
```

### 防火墙管理

```c
// 防火墙管理器结构
typedef struct {
    firewall_backend_t backend; /* 后端类型 (iptables/nftables) */
    rule_table_t *tables;    /* 规则表 */
    chain_t *chains;         /* 链 */

    bool enabled;            /* 是否启用 */
    policy_t default_policy; /* 默认策略 */

    log_manager_t *logs;     /* 日志管理器 */
    pthread_mutex_t mutex;   /* 互斥锁 */
} firewall_manager_t;

// 防火墙规则结构
typedef struct {
    char *name;              /* 规则名称 */
    rule_action_t action;    /* 规则动作 */
    rule_direction_t direction; /* 规则方向 */

    char *source;            /* 源地址 */
    char *destination;       /* 目的地址 */
    char *protocol;          /* 协议 */
    char *port;              /* 端口 */

    bool enabled;            /* 是否启用 */
    int priority;            /* 优先级 */

    struct firewall_rule *next; /* 下一个规则 */
} firewall_rule_t;
```

## 配置系统

### 主配置文件格式

```json
{
  "network": {
    "auto_connect": true,
    "default_interface": "eth0",
    "dns_servers": ["8.8.8.8", "8.8.4.4"],
    "search_domains": ["local"]
  },
  "vpn": {
    "auto_connect": false,
    "kill_switch": true,
    "default_protocol": "wireguard",
    "config_dir": "/etc/ntmgr/vpn"
  },
  "proxy": {
    "enabled": false,
    "type": "socks5",
    "host": "127.0.0.1",
    "port": 1080,
    "authentication": false
  },
  "firewall": {
    "enabled": true,
    "backend": "nftables",
    "default_policy": "drop",
    "log_level": "warning"
  },
  "monitoring": {
    "enabled": true,
    "interval": 5,
    "alert_threshold": {
      "cpu_usage": 80,
      "memory_usage": 90,
      "disk_usage": 85
    }
  }
}
```

### 连接配置文件

```ini
[connection_eth0]
type=ethernet
auto_connect=true
mtu=1500

[connection_wlan0]
type=wifi
ssid=MyWiFi
security=wpa2-psk
password=secret123
auto_connect=true

[connection_vpn_office]
type=vpn
protocol=wireguard
server=vpn.company.com
port=51820
config=/etc/ntmgr/vpn/office.conf
auto_connect=false
```

## 监控系统

### 流量监控

```c
// 流量统计结构
typedef struct {
    time_t timestamp;        /* 时间戳 */
    uint64_t rx_bytes;       /* 接收字节数 */
    uint64_t tx_bytes;       /* 发送字节数 */
    uint64_t rx_packets;     /* 接收数据包数 */
    uint64_t tx_packets;     /* 发送数据包数 */

    double rx_rate;          /* 接收速率 */
    double tx_rate;          /* 发送速率 */
    double rx_error_rate;    /* 接收错误率 */
    double tx_error_rate;    /* 发送错误率 */

    struct traffic_stats *next; /* 下一个统计 */
} traffic_stats_t;

// 监控器结构
typedef struct {
    bool enabled;            /* 是否启用 */
    int interval;            /* 监控间隔 */
    size_t history_size;     /* 历史记录大小 */

    traffic_stats_t *history; /* 历史统计 */
    size_t history_count;     /* 历史记录数量 */

    monitor_callback_t on_update; /* 更新回调 */
    pthread_t thread;        /* 监控线程 */
} traffic_monitor_t;
```

## 插件系统

### 插件类型

1. **协议插件**：支持新的网络协议
2. **认证插件**：支持新的认证方法
3. **监控插件**：扩展监控功能
4. **界面插件**：自定义界面组件

### 插件接口

```c
// 插件接口定义
typedef struct {
    char *name;              /* 插件名称 */
    char *version;           /* 版本 */
    char *description;       /* 描述 */

    /* 生命周期函数 */
    int (*init)(app_t *app); /* 初始化 */
    void (*cleanup)(void);   /* 清理 */
    void (*reload)(void);    /* 重新加载 */

    /* 功能函数 */
    bool (*can_handle)(const char *protocol);
    int (*connect)(connection_t *conn);
    int (*disconnect)(connection_t *conn);
    int (*get_status)(connection_t *conn, status_t *status);

    /* 配置函数 */
    bool (*load_config)(const char *config_file);
    bool (*save_config)(const char *config_file);
} plugin_interface_t;
```

## 测试策略

### 单元测试

- 网络接口操作测试
- VPN连接功能测试
- 代理规则引擎测试
- 防火墙规则测试

### 集成测试

- 完整网络配置流程测试
- 多协议VPN连接测试
- 代理链路测试
- 监控系统测试

### 用户体验测试

- 界面操作便捷性测试
- 配置向导完整性测试
- 错误处理友好性测试
- 帮助系统可用性测试

## 部署和集成

### 安装配置

1. **编译安装**：从源码编译安装
2. **权限设置**：设置网络设备访问权限
3. **服务集成**：集成系统服务管理
4. **桌面集成**：集成桌面环境菜单

### 系统集成

- **NetworkManager替代**：可替代传统的NetworkManager
- **系统服务**：支持systemd服务管理
- **开机自启**：自动启动和网络连接恢复
- **日志集成**：集成系统日志系统

## 未来规划

### 短期目标（1-3个月）

- 完成核心网络管理功能
- 实现基本VPN支持
- 添加流量监控功能
- 完善用户界面

### 中期目标（3-6个月）

- 高级防火墙管理
- 代理链路支持
- 网络诊断工具
- 远程管理功能

### 长期目标（6个月以上）

- 云网络管理
- SDN支持
- 容器网络管理
- AI辅助诊断

## 总结

M4KK1 Ntmgr网络管理器是一个功能全面、界面友好的网络管理工具，不仅提供了基础的网络连接管理功能，还集成了VPN、代理、防火墙、网络监控等高级特性。通过模块化的设计和插件系统，Ntmgr具有良好的可扩展性和维护性，将成为M4KK1系统网络管理的核心工具，为用户提供专业级的网络管理体验。
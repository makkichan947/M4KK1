/**
 * M4KK1 Network Protocol Stack Core
 * 网络协议栈核心实现
 */

#include "../include/net.h"
#include "../../include/console.h"
#include "../../include/memory.h"
#include "../../include/string.h"
#include <stdint.h>
#include <stdbool.h>

/* 网络设备列表 */
static net_device_t *net_devices = NULL;
static int net_device_count = 0;

/* 协议处理函数表 */
static net_protocol_handler_t protocol_handlers[256];

/* 网络统计信息 */
static struct {
    uint64_t packets_received;
    uint64_t packets_sent;
    uint64_t bytes_received;
    uint64_t bytes_sent;
    uint64_t packets_dropped;
} net_stats;

/* MAC地址工具函数 */
void net_get_mac_string(uint8_t *mac, char *buffer) {
    sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

/* IP地址工具函数 */
uint32_t net_string_to_ip(const char *string) {
    uint32_t ip = 0;
    uint32_t octet = 0;
    int dots = 0;

    while (*string) {
        if (*string >= '0' && *string <= '9') {
            octet = octet * 10 + (*string - '0');
        } else if (*string == '.') {
            ip = (ip << 8) | octet;
            octet = 0;
            dots++;
        } else {
            return 0; /* 无效字符 */
        }
        string++;
    }

    /* 处理最后一个八位组 */
    if (dots == 3) {
        ip = (ip << 8) | octet;
        return ip;
    }

    return 0;
}

uint32_t net_ip_to_string(uint32_t ip, char *buffer) {
    return sprintf(buffer, "%d.%d.%d.%d",
                   (ip >> 24) & 0xFF,
                   (ip >> 16) & 0xFF,
                   (ip >> 8) & 0xFF,
                   ip & 0xFF);
}

/* 校验和计算 */
uint16_t net_checksum(uint16_t *data, uint16_t len) {
    uint32_t sum = 0;

    while (len > 1) {
        sum += *data++;
        len -= 2;
    }

    if (len > 0) {
        sum += *(uint8_t *)data;
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}

/* 初始化网络协议栈 */
int net_init(void) {
    console_write("Initializing network protocol stack...\n");

    /* 初始化设备列表 */
    net_devices = NULL;
    net_device_count = 0;

    /* 初始化协议处理函数表 */
    memset(protocol_handlers, 0, sizeof(protocol_handlers));

    /* 初始化统计信息 */
    memset(&net_stats, 0, sizeof(net_stats));

    console_write("Network protocol stack initialized\n");
    return 0;
}

/* 注册网络设备 */
int net_device_register(net_device_t *dev) {
    if (!dev || !dev->name[0]) {
        return -1;
    }

    /* 检查设备是否已存在 */
    net_device_t *device = net_devices;
    while (device) {
        if (strcmp(device->name, dev->name) == 0) {
            return -1; /* 设备已存在 */
        }
        device = (net_device_t *)device->priv; /* 下一个设备 */
    }

    /* 添加到设备列表 */
    dev->priv = net_devices;
    net_devices = dev;
    net_device_count++;

    console_write("Network device registered: ");
    console_write(dev->name);
    console_write("\n");

    /* 初始化设备 */
    if (dev->init) {
        dev->init(dev);
    }

    return 0;
}

/* 注销网络设备 */
int net_device_unregister(const char *name) {
    if (!name) {
        return -1;
    }

    net_device_t *device = net_devices;
    net_device_t *prev = NULL;

    while (device) {
        if (strcmp(device->name, name) == 0) {
            /* 从列表中移除 */
            if (prev) {
                prev->priv = device->priv;
            } else {
                net_devices = device->priv;
            }

            net_device_count--;
            return 0;
        }

        prev = device;
        device = device->priv;
    }

    return -1; /* 设备未找到 */
}

/* 查找网络设备 */
net_device_t *net_device_find(const char *name) {
    if (!name) {
        return NULL;
    }

    net_device_t *device = net_devices;
    while (device) {
        if (strcmp(device->name, name) == 0) {
            return device;
        }
        device = device->priv;
    }

    return NULL;
}

/* 注册协议处理函数 */
int net_protocol_register(uint8_t protocol, net_protocol_handler_t handler) {
    if (protocol >= 256 || !handler) {
        return -1;
    }

    protocol_handlers[protocol] = handler;

    console_write("Protocol handler registered for protocol 0x");
    console_write_hex(protocol);
    console_write("\n");

    return 0;
}

/* 注销协议处理函数 */
int net_protocol_unregister(uint8_t protocol) {
    if (protocol >= 256) {
        return -1;
    }

    protocol_handlers[protocol] = NULL;
    return 0;
}

/* 发送IP数据包 */
int net_send_packet(uint32_t dst_ip, uint8_t protocol, uint8_t *data, uint16_t len) {
    if (!data || len == 0) {
        return -1;
    }

    /* 构建IP头部 */
    ip_header_t ip_header;
    memset(&ip_header, 0, sizeof(ip_header));

    ip_header.ver_ihl = 0x45;  /* IPv4, 20字节头部 */
    ip_header.tos = 0;
    ip_header.total_len = sizeof(ip_header) + len;
    ip_header.id = 0;  /* 应该递增 */
    ip_header.frag_offset = 0;
    ip_header.ttl = 64;
    ip_header.protocol = protocol;
    ip_header.src_ip = 0x0A000001;  /* 10.0.0.1 */
    ip_header.dst_ip = dst_ip;

    /* 计算IP校验和 */
    ip_header.checksum = net_checksum((uint16_t *)&ip_header, sizeof(ip_header));

    /* 发送数据包 */
    net_device_t *dev = net_devices;
    if (dev && dev->transmit) {
        /* 构建完整数据包 */
        uint8_t packet[sizeof(ip_header) + len];
        memcpy(packet, &ip_header, sizeof(ip_header));
        memcpy(packet + sizeof(ip_header), data, len);

        dev->transmit(dev, packet, sizeof(packet));
        net_stats.packets_sent++;
        net_stats.bytes_sent += sizeof(packet);

        return sizeof(packet);
    }

    return -1;
}

/* 发送以太网数据包 */
int net_send_ethernet(uint8_t *dst_mac, uint16_t type, uint8_t *data, uint16_t len) {
    if (!dst_mac || !data || len == 0) {
        return -1;
    }

    /* 构建以太网头部 */
    eth_header_t eth_header;
    memcpy(eth_header.dst_mac, dst_mac, 6);
    /* 这里应该设置源MAC地址 */
    memset(eth_header.src_mac, 0, 6);
    eth_header.type = type;

    /* 发送数据包 */
    net_device_t *dev = net_devices;
    if (dev && dev->transmit) {
        /* 构建完整数据包 */
        uint8_t packet[sizeof(eth_header) + len];
        memcpy(packet, &eth_header, sizeof(eth_header));
        memcpy(packet + sizeof(eth_header), data, len);

        dev->transmit(dev, packet, sizeof(packet));
        net_stats.packets_sent++;
        net_stats.bytes_sent += sizeof(packet);

        return sizeof(packet);
    }

    return -1;
}

/* 网络轮询 */
void net_poll(void) {
    net_device_t *dev = net_devices;

    while (dev) {
        if (dev->poll) {
            dev->poll(dev);
        }
        dev = dev->priv;
    }
}

/* 接收数据包 */
void net_receive_packet(net_device_t *dev, uint8_t *packet, uint16_t len) {
    if (!dev || !packet || len == 0) {
        return;
    }

    net_stats.packets_received++;
    net_stats.bytes_received += len;

    /* 解析以太网头部 */
    if (len < sizeof(eth_header_t)) {
        return;
    }

    eth_header_t *eth_header = (eth_header_t *)packet;

    /* 检查是否为IP数据包 */
    if (eth_header->type == ETH_TYPE_IP) {
        if (len < sizeof(eth_header_t) + sizeof(ip_header_t)) {
            return;
        }

        ip_header_t *ip_header = (ip_header_t *)(packet + sizeof(eth_header_t));

        /* 验证IP版本 */
        if ((ip_header->ver_ihl >> 4) != 4) {
            return;
        }

        /* 验证IP校验和 */
        uint16_t saved_checksum = ip_header->checksum;
        ip_header->checksum = 0;
        uint16_t calculated_checksum = net_checksum((uint16_t *)ip_header, sizeof(ip_header_t));

        if (saved_checksum != calculated_checksum) {
            console_write("IP checksum mismatch\n");
            return;
        }

        ip_header->checksum = saved_checksum;

        /* 查找协议处理函数 */
        if (ip_header->protocol < 256 && protocol_handlers[ip_header->protocol]) {
            uint8_t *data = packet + sizeof(eth_header_t) + sizeof(ip_header_t);
            uint16_t data_len = len - sizeof(eth_header_t) - sizeof(ip_header_t);

            protocol_handlers[ip_header->protocol](data, data_len,
                                                  ip_header->src_ip, ip_header->dst_ip);
        }
    }
}

/* 打印数据包内容（调试用） */
void net_print_packet(uint8_t *packet, uint16_t len) {
    console_write("Packet dump (");
    console_write_dec(len);
    console_write(" bytes):\n");

    for (int i = 0; i < len; i += 16) {
        console_write("  ");
        console_write_hex(i);
        console_write(": ");

        /* 打印十六进制 */
        for (int j = 0; j < 16; j++) {
            if (i + j < len) {
                console_write_hex(packet[i + j]);
                console_write(" ");
            } else {
                console_write("   ");
            }
        }

        /* 打印ASCII */
        console_write(" ");
        for (int j = 0; j < 16 && i + j < len; j++) {
            uint8_t c = packet[i + j];
            if (c >= 32 && c <= 126) {
                console_put_char(c);
            } else {
                console_put_char('.');
            }
        }

        console_write("\n");
    }
}

/* 获取网络统计信息 */
void net_get_stats(uint64_t *packets_received, uint64_t *packets_sent,
                   uint64_t *bytes_received, uint64_t *bytes_sent) {
    if (packets_received) *packets_received = net_stats.packets_received;
    if (packets_sent) *packets_sent = net_stats.packets_sent;
    if (bytes_received) *bytes_received = net_stats.bytes_received;
    if (bytes_sent) *bytes_sent = net_stats.bytes_sent;
}

/* 打印网络统计信息 */
void net_print_stats(void) {
    console_write("=== Network Statistics ===\n");
    console_write("Packets received: ");
    console_write_dec(net_stats.packets_received);
    console_write("\n");
    console_write("Packets sent: ");
    console_write_dec(net_stats.packets_sent);
    console_write("\n");
    console_write("Bytes received: ");
    console_write_dec(net_stats.bytes_received);
    console_write("\n");
    console_write("Bytes sent: ");
    console_write_dec(net_stats.bytes_sent);
    console_write("\n");
    console_write("Packets dropped: ");
    console_write_dec(net_stats.packets_dropped);
    console_write("\n");
    console_write("===========================\n");
}
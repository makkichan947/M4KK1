/**
 * M4KK1 Network Protocol Stack
 * 网络协议栈接口定义
 */

#ifndef _M4K_NET_H
#define _M4K_NET_H

#include <stdint.h>
#include <stdbool.h>

/* 网络设备类型 */
#define NET_DEV_ETHERNET    1
#define NET_DEV_WIFI        2
#define NET_DEV_LOOPBACK    3

/* IP协议类型 */
#define IP_PROTOCOL_ICMP    1
#define IP_PROTOCOL_TCP     6
#define IP_PROTOCOL_UDP     17

/* 以太网协议类型 */
#define ETH_TYPE_IP         0x0800
#define ETH_TYPE_ARP        0x0806
#define ETH_TYPE_IPV6       0x86DD

/* 以太网头部 */
typedef struct {
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t type;
} __attribute__((packed)) eth_header_t;

/* IP头部 */
typedef struct {
    uint8_t  ver_ihl;
    uint8_t  tos;
    uint16_t total_len;
    uint16_t id;
    uint16_t frag_offset;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
} __attribute__((packed)) ip_header_t;

/* UDP头部 */
typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed)) udp_header_t;

/* ICMP头部 */
typedef struct {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint32_t data;
} __attribute__((packed)) icmp_header_t;

/* 网络设备接口 */
typedef struct net_device {
    char name[16];
    uint32_t type;
    uint8_t mac_addr[6];
    uint32_t ip_addr;
    uint32_t netmask;
    uint32_t gateway;
    bool up;

    /* 设备操作函数 */
    int (*init)(struct net_device *dev);
    int (*transmit)(struct net_device *dev, uint8_t *data, uint32_t len);
    int (*receive)(struct net_device *dev, uint8_t *buffer, uint32_t len);
    void (*poll)(struct net_device *dev);

    /* 私有数据 */
    void *priv;
} net_device_t;

/* 网络协议处理函数类型 */
typedef void (*net_protocol_handler_t)(uint8_t *packet, uint16_t len,
                                      uint32_t src_ip, uint32_t dst_ip);

/* 网络API函数 */
int net_init(void);
int net_device_register(net_device_t *dev);
int net_device_unregister(const char *name);
net_device_t *net_device_find(const char *name);

int net_protocol_register(uint8_t protocol, net_protocol_handler_t handler);
int net_protocol_unregister(uint8_t protocol);

int net_send_packet(uint32_t dst_ip, uint8_t protocol, uint8_t *data, uint16_t len);
int net_send_ethernet(uint8_t *dst_mac, uint16_t type, uint8_t *data, uint16_t len);

void net_poll(void);

/* TCP函数 */
void tcp_init(void);
int tcp_listen(uint32_t local_ip, uint16_t local_port);
int tcp_connect(uint32_t local_ip, uint16_t local_port,
                uint32_t remote_ip, uint16_t remote_port);
int tcp_send(tcp_pcb_t *pcb, uint8_t *data, uint16_t len);
int tcp_close(tcp_pcb_t *pcb);
void tcp_handle_packet(uint8_t *packet, uint16_t len, uint32_t src_ip, uint32_t dst_ip);

/* UDP函数 */
int udp_send(uint32_t src_ip, uint16_t src_port,
             uint32_t dst_ip, uint16_t dst_port,
             uint8_t *data, uint16_t len);
void udp_handle_packet(uint8_t *packet, uint16_t len, uint32_t src_ip, uint32_t dst_ip);

/* ICMP函数 */
void icmp_handle_packet(uint8_t *packet, uint16_t len, uint32_t src_ip, uint32_t dst_ip);
int icmp_send_echo_request(uint32_t dst_ip, uint16_t id, uint16_t seq);
int icmp_send_echo_reply(uint32_t dst_ip, uint16_t id, uint16_t seq);

/* ARP函数 */
void arp_handle_packet(uint8_t *packet, uint16_t len);
int arp_resolve(uint32_t ip_addr, uint8_t *mac_addr);
int arp_send_request(uint32_t target_ip);
int arp_send_reply(uint32_t dst_ip, uint8_t *dst_mac);

/* 网络工具函数 */
uint16_t net_checksum(uint16_t *data, uint16_t len);
uint32_t net_ip_to_string(uint32_t ip, char *buffer);
uint32_t net_string_to_ip(const char *string);
void net_print_packet(uint8_t *packet, uint16_t len);

#endif /* _M4K_NET_H */
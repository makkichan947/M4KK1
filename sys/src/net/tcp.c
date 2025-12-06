/**
 * M4KK1 TCP Implementation
 * Basic TCP protocol implementation for M4KK1 OS
 */

#include <stdint.h>
#include <string.h>
#include "net.h"

// TCP states
typedef enum {
    TCP_CLOSED,
    TCP_LISTEN,
    TCP_SYN_SENT,
    TCP_SYN_RECEIVED,
    TCP_ESTABLISHED,
    TCP_FIN_WAIT_1,
    TCP_FIN_WAIT_2,
    TCP_CLOSE_WAIT,
    TCP_CLOSING,
    TCP_LAST_ACK,
    TCP_TIME_WAIT
} tcp_state_t;

// TCP header structure
typedef struct {
    uint16_t source_port;
    uint16_t dest_port;
    uint32_t seq_num;
    uint32_t ack_num;
    uint8_t data_offset;
    uint8_t flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent_ptr;
} __attribute__((packed)) tcp_header_t;

// TCP connection structure
typedef struct {
    tcp_state_t state;
    uint32_t local_ip;
    uint32_t remote_ip;
    uint16_t local_port;
    uint16_t remote_port;
    uint32_t seq_num;
    uint32_t ack_num;
} tcp_connection_t;

// Global TCP state
static tcp_connection_t tcp_connections[16];
static int tcp_connection_count = 0;

// Initialize TCP stack
void tcp_init(void) {
    memset(tcp_connections, 0, sizeof(tcp_connections));
    tcp_connection_count = 0;
}

// Create TCP connection
int tcp_connect(uint32_t remote_ip, uint16_t remote_port) {
    if (tcp_connection_count >= 16) {
        return -1; // No available connections
    }

    tcp_connection_t* conn = &tcp_connections[tcp_connection_count++];
    conn->state = TCP_CLOSED;
    conn->remote_ip = remote_ip;
    conn->remote_port = remote_port;
    conn->local_port = 1024 + tcp_connection_count; // Simple port allocation
    conn->seq_num = 0;
    conn->ack_num = 0;

    // Send SYN packet (simplified)
    // In real implementation, this would construct and send a TCP SYN packet

    conn->state = TCP_SYN_SENT;

    return tcp_connection_count - 1;
}

// Send data over TCP
int tcp_send(int conn_id, const void* data, size_t len) {
    if (conn_id < 0 || conn_id >= tcp_connection_count) {
        return -1;
    }

    tcp_connection_t* conn = &tcp_connections[conn_id];
    if (conn->state != TCP_ESTABLISHED) {
        return -1;
    }

    // Send data (simplified)
    // In real implementation, this would segment data and send TCP packets

    return len;
}

// Receive data from TCP
int tcp_receive(int conn_id, void* buffer, size_t len) {
    if (conn_id < 0 || conn_id >= tcp_connection_count) {
        return -1;
    }

    tcp_connection_t* conn = &tcp_connections[conn_id];
    if (conn->state != TCP_ESTABLISHED) {
        return -1;
    }

    // Receive data (simplified)
    // In real implementation, this would reassemble TCP packets

    return 0;
}

// Close TCP connection
int tcp_close(int conn_id) {
    if (conn_id < 0 || conn_id >= tcp_connection_count) {
        return -1;
    }

    tcp_connection_t* conn = &tcp_connections[conn_id];

    // Send FIN packet (simplified)
    // In real implementation, this would send a TCP FIN packet

    conn->state = TCP_FIN_WAIT_1;

    return 0;
}
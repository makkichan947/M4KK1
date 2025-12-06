/**
 * M4KK1 ARP Implementation
 * Basic ARP protocol implementation for M4KK1 OS
 */

#include <stdint.h>
#include <string.h>
#include "net.h"

// ARP header structure
typedef struct {
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t hardware_len;
    uint8_t protocol_len;
    uint16_t operation;
    uint8_t sender_mac[6];
    uint32_t sender_ip;
    uint8_t target_mac[6];
    uint32_t target_ip;
} __attribute__((packed)) arp_header_t;

// ARP cache entry
typedef struct {
    uint32_t ip;
    uint8_t mac[6];
    int valid;
} arp_entry_t;

// ARP cache
static arp_entry_t arp_cache[16];
static int arp_cache_count = 0;

// Initialize ARP
void arp_init(void) {
    memset(arp_cache, 0, sizeof(arp_cache));
    arp_cache_count = 0;
}

// Add ARP entry
void arp_add_entry(uint32_t ip, const uint8_t* mac) {
    if (arp_cache_count >= 16) {
        return; // Cache full
    }

    arp_entry_t* entry = &arp_cache[arp_cache_count++];
    entry->ip = ip;
    memcpy(entry->mac, mac, 6);
    entry->valid = 1;
}

// Lookup MAC address for IP
int arp_lookup(uint32_t ip, uint8_t* mac) {
    for (int i = 0; i < arp_cache_count; i++) {
        if (arp_cache[i].valid && arp_cache[i].ip == ip) {
            memcpy(mac, arp_cache[i].mac, 6);
            return 0;
        }
    }

    return -1; // Not found
}

// Send ARP request
int arp_request(uint32_t target_ip) {
    // Send ARP request (simplified)
    // In real implementation, this would broadcast an ARP request

    return 0;
}

// Process ARP reply
void arp_process_reply(const uint8_t* packet, size_t len) {
    if (len < sizeof(arp_header_t)) {
        return;
    }

    const arp_header_t* arp = (const arp_header_t*)packet;

    // Add to cache
    arp_add_entry(arp->sender_ip, arp->sender_mac);
}
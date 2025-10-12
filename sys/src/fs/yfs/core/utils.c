/**
 * YFS (Yet Another File System) - 工具函数
 * 实现校验和、UUID生成等辅助功能
 */

#include "yfs.h"
#include "../include/yfs.h"
#include "../../y4ku/include/console.h"

/**
 * CRC32C校验和计算
 * 使用CRC32C多项式：0x1EDC6F41
 */
uint32_t yfs_checksum_crc32c(const void *data, uint32_t size) {
    if (!data || size == 0) {
        return 0;
    }

    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t crc = 0xFFFFFFFF;

    static const uint32_t crc32c_table[256] = {
        0x00000000, 0xF26B8303, 0xE13B70F7, 0x1350F3F4,
        0xC79A971F, 0x35F1141C, 0x26A1E7E8, 0xD4CA64EB,
        0x8AD958CF, 0x78B2DBCC, 0x6BE22838, 0x9989AB3B,
        0x4D43CFD0, 0xBF284CD3, 0xAC78BF27, 0x5E133C24,
        0x105EC76F, 0xE235446C, 0xF165B798, 0x030E349B,
        0xD7C45070, 0x25AFD373, 0x36FF2087, 0xC494A384,
        0x9A879FA0, 0x68EC1CA3, 0x7BBCEF57, 0x89D76C54,
        0x5D1D08BF, 0xAF768BBC, 0xBC267848, 0x4E4DFB4B,
        0x20BD8EDE, 0xD2D60DDD, 0xC186FE29, 0x33ED7D2A,
        0xE72719C1, 0x154C9AC2, 0x061C6936, 0xF477EA35,
        0xAA64D611, 0x580F5512, 0x4B5FA6E6, 0xB93425E5,
        0x6DFE410E, 0x9F95C20D, 0x8CC531F9, 0x7EAEB2FA,
        0x30E349B1, 0xC288CAB2, 0xD1D83946, 0x23B3BA45,
        0xF779DEAE, 0x05125DAD, 0x1642AE59, 0xE4292D5A,
        0xBA3A117E, 0x4851927D, 0x5B416189, 0xA92AE28A,
        0x7DE08661, 0x8F8B0562, 0x9CDBF696, 0x6EB07595,
        0x417B1DBC, 0xB3109EBF, 0xA0406D4B, 0x522BEE48,
        0x86E18AA3, 0x748A09A0, 0x67DAFA54, 0x95B17957,
        0xCBA24573, 0x39C9C670, 0x2A993584, 0xD8F2B687,
        0x0C38D26C, 0xFE53516F, 0xED03A29B, 0x1F682198,
        0x5125DAD3, 0xA34E59D0, 0xB01EAA24, 0x42752927,
        0x96BF4DCC, 0x64D4CECF, 0x77843D3B, 0x85EFBE38,
        0xDBFC821C, 0x2997011F, 0x3AC7F2EB, 0xC8AC71E8,
        0x1C661503, 0xEE0D9600, 0xFD5D65F4, 0x0F36E6F7,
        0x61C69362, 0x93AD1061, 0x80FDE395, 0x72966096,
        0xA65C047D, 0x5437877E, 0x4767748A, 0xB50CF789,
        0xEB1FCBAD, 0x197448AE, 0x0A24BB5A, 0xF84F3859,
        0x2C855CB2, 0xDEEFDFB1, 0xCDBF2C45, 0x3FD4AF46,
        0x7198540D, 0x83F3D70E, 0x90A324FA, 0x62C8A7F9,
        0xB602C312, 0x44694011, 0x5739B3E5, 0xA55230E6,
        0xFB410CC2, 0x092A8FC1, 0x1A7A7C35, 0xE811FF36,
        0x3CDB9BDD, 0xCEB018DE, 0xDDE0EB2A, 0x2F8B6829,
        0x82F63B78, 0x709DB87B, 0x63CD4B8F, 0x91A6C88C,
        0x456CAC67, 0xB7072764, 0xA457D490, 0x563C5793,
        0x082F6BB7, 0xFA44E8B4, 0xE9141B40, 0x1B7F9843,
        0xCFB5FCA8, 0x3DDE7FAB, 0x2E8E8C5F, 0xDCE50F5C,
        0x92A8F417, 0x60C37714, 0x739384E0, 0x81F807E3,
        0x55326308, 0xA759E00B, 0xB40913FF, 0x466290FC,
        0x1871ACD8, 0xEA1A2FD9, 0xF94ADC2D, 0x0B215F2E,
        0xDFEB3BC5, 0x2D80A8C6, 0x3ED05B32, 0xCCBBD831,
        0xA24BAD84, 0x50002E87, 0x4350DD73, 0xB13B5E70,
        0x65F13A9B, 0x979AB998, 0x84CA4A6C, 0x76A1C96F,
        0x28B2F54B, 0xDAD97648, 0xC98985BC, 0x3BE206BF,
        0xEF286254, 0x1D43E157, 0x0E1312A3, 0xFC7891A0,
        0xB2356AEB, 0x405EE9E8, 0x530E1A1C, 0xA165991F,
        0x75AFFDF4, 0x87C47EF7, 0x94948D03, 0x66FF0E00,
        0x38EC3224, 0xCA87B127, 0xD9D742D3, 0x2BBCC1D0,
        0xFF76A53B, 0x0D1D2638, 0x1E4DD5CC, 0xEC2656CF,
        0xC38D26C4, 0x31E6A5C7, 0x22B65633, 0xD0DDD530,
        0x0417B1DB, 0xF67C32D8, 0xE52CC12C, 0x1747422F,
        0x49547E0B, 0xBB3FFD08, 0xA86F0EFC, 0x5A048DFF,
        0x8ECEE914, 0x7CA56A17, 0x6FF599E3, 0x9D9E1AE0,
        0xD3D3E1AB, 0x21B862A8, 0x32E8915C, 0xC083125F,
        0x144976B4, 0xE622F5B7, 0xF5720643, 0x07198540,
        0x590AB964, 0xAB613A67, 0xB831C993, 0x4A5A4A90,
        0x9E902E7B, 0x6CFBAD78, 0x7FAB5E8C, 0x8DC0DD8F,
        0xE330A81A, 0x115B2B19, 0x020BD8ED, 0xF0605BEE,
        0x24AA3F05, 0xD6C19C06, 0xC5916FF2, 0x37FAECF1,
        0x69E9D0D5, 0x9B8253D6, 0x88D2A022, 0x7AB92321,
        0xAE7347CA, 0x5C18ECC9, 0x4F68173D, 0xBD03943E,
        0xF34E6F75, 0x0125EC76, 0x12751F82, 0xE01E9C81,
        0x34D4F86A, 0xC6BF7B69, 0xD5EF889D, 0x27840B9E,
        0x799737BA, 0x8BFCB4B9, 0x98AC474D, 0x6AC7C44E,
        0xBE0DA0A5, 0x4C4623A6, 0x5F16D052, 0xAD7D5351
    };

    for (uint32_t i = 0; i < size; i++) {
        crc = (crc >> 8) ^ crc32c_table[(crc ^ bytes[i]) & 0xFF];
    }

    return crc ^ 0xFFFFFFFF;
}

/**
 * Adler32校验和计算
 * 简单的校验和算法，性能较好
 */
uint32_t yfs_checksum_adler32(const void *data, uint32_t size) {
    if (!data || size == 0) {
        return 0;
    }

    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t a = 1, b = 0;

    for (uint32_t i = 0; i < size; i++) {
        a = (a + bytes[i]) % 65521;
        b = (b + a) % 65521;
    }

    return (b << 16) | a;
}

/**
 * 生成UUID（版本4）
 * 使用伪随机数生成UUID
 */
void yfs_uuid_generate(uint8_t *uuid) {
    if (!uuid) {
        return;
    }

    // 使用当前时间和内存地址作为随机种子
    uint64_t seed = yfs_time_current();
    seed ^= (uint64_t)uuid;

    for (int i = 0; i < 16; i++) {
        // 生成伪随机字节
        seed = seed * 1103515245 + 12345;
        uuid[i] = (uint8_t)(seed >> 16);

        // 设置UUID版本（版本4）和变体
        if (i == 6) {
            uuid[i] = (uuid[i] & 0x0F) | 0x40;  // 版本4
        } else if (i == 8) {
            uuid[i] = (uuid[i] & 0x3F) | 0x80;  // RFC 4122变体
        }
    }
}

/**
 * 获取当前时间戳
 */
uint64_t yfs_time_current(void) {
    // 这里应该使用内核的时间函数
    // 暂时返回一个固定值
    return 1234567890ULL;
}

/**
 * 内存复制函数
 */
void *yfs_memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }

    return dest;
}

/**
 * 内存设置函数
 */
void *yfs_memset(void *dest, int value, size_t n) {
    uint8_t *d = (uint8_t *)dest;

    for (size_t i = 0; i < n; i++) {
        d[i] = (uint8_t)value;
    }

    return dest;
}

/**
 * 内存比较函数
 */
int yfs_memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *a = (const uint8_t *)s1;
    const uint8_t *b = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return a[i] - b[i];
        }
    }

    return 0;
}

/**
 * 字符串长度函数
 */
size_t yfs_strlen(const char *str) {
    size_t len = 0;

    while (str && str[len]) {
        len++;
    }

    return len;
}

/**
 * 字符串复制函数
 */
char *yfs_strcpy(char *dest, const char *src) {
    char *d = dest;

    while (src && *src) {
        *d++ = *src++;
    }

    *d = '\0';

    return dest;
}

/**
 * 字符串比较函数
 */
int yfs_strcmp(const char *s1, const char *s2) {
    while (s1 && s2 && *s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }

    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;

    return *s1 - *s2;
}

/**
 * 字符串连接函数
 */
char *yfs_strcat(char *dest, const char *src) {
    char *d = dest;

    while (*d) {
        d++;
    }

    while (src && *src) {
        *d++ = *src++;
    }

    *d = '\0';

    return dest;
}

/**
 * 查找字符函数
 */
char *yfs_strchr(const char *str, int c) {
    while (str && *str) {
        if (*str == (char)c) {
            return (char *)str;
        }
        str++;
    }

    return NULL;
}

/**
 * 查找子串函数
 */
char *yfs_strstr(const char *haystack, const char *needle) {
    if (!haystack || !needle) {
        return NULL;
    }

    size_t needle_len = yfs_strlen(needle);

    while (*haystack) {
        if (yfs_strncmp(haystack, needle, needle_len) == 0) {
            return (char *)haystack;
        }
        haystack++;
    }

    return NULL;
}

/**
 * 字符串比较函数（指定长度）
 */
int yfs_strncmp(const char *s1, const char *s2, size_t n) {
    while (n > 0 && s1 && s2 && *s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
        n--;
    }

    if (n == 0) return 0;
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;

    return *s1 - *s2;
}

/**
 * 格式化字符串函数（简化版）
 */
int yfs_snprintf(char *buffer, size_t size, const char *format, ...) {
    if (!buffer || !format || size == 0) {
        return 0;
    }

    // 简化的格式化实现
    // 这里应该实现完整的格式化功能
    // 暂时只复制格式字符串

    size_t len = yfs_strlen(format);
    if (len >= size) {
        len = size - 1;
    }

    yfs_strcpy(buffer, format);
    return len;
}

/**
 * 字节序转换函数
 */
uint16_t yfs_htole16(uint16_t value) {
    // 假设主机是小端序
    return value;
}

uint32_t yfs_htole32(uint32_t value) {
    // 假设主机是小端序
    return value;
}

uint64_t yfs_htole64(uint64_t value) {
    // 假设主机是小端序
    return value;
}

uint16_t yfs_le16toh(uint16_t value) {
    return yfs_htole16(value);
}

uint32_t yfs_le32toh(uint32_t value) {
    return yfs_htole32(value);
}

uint64_t yfs_le64toh(uint64_t value) {
    return yfs_htole64(value);
}

/**
 * 位操作函数
 */
uint32_t yfs_ffs(uint32_t value) {
    if (value == 0) {
        return 0;
    }

    uint32_t bit = 1;
    for (int i = 0; i < 32; i++) {
        if (value & bit) {
            return i + 1;
        }
        bit <<= 1;
    }

    return 0;
}

uint32_t yfs_fls(uint32_t value) {
    if (value == 0) {
        return 0;
    }

    uint32_t bit = 0x80000000;
    for (int i = 31; i >= 0; i--) {
        if (value & bit) {
            return i + 1;
        }
        bit >>= 1;
    }

    return 0;
}

/**
 * 数学函数
 */
uint32_t yfs_min(uint32_t a, uint32_t b) {
    return (a < b) ? a : b;
}

uint32_t yfs_max(uint32_t a, uint32_t b) {
    return (a > b) ? a : b;
}

uint64_t yfs_min64(uint64_t a, uint64_t b) {
    return (a < b) ? a : b;
}

uint64_t yfs_max64(uint64_t a, uint64_t b) {
    return (a > b) ? a : b;
}

/**
 * 对齐函数
 */
uint32_t yfs_align_up(uint32_t value, uint32_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

uint32_t yfs_align_down(uint32_t value, uint32_t alignment) {
    return value & ~(alignment - 1);
}

uint64_t yfs_align_up64(uint64_t value, uint64_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

uint64_t yfs_align_down64(uint64_t value, uint64_t alignment) {
    return value & ~(alignment - 1);
}
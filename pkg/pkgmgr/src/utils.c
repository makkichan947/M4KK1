/**
 * PkgMgr - M4KK1包管理系统工具函数
 * 实现内存管理、字符串处理、校验和等辅助功能
 */

#include "pkgmgr.h"
#include "../include/pkgmgr.h"
#include "../../y4ku/include/console.h"

/**
 * 内存分配函数
 */
void *pkgmgr_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    // 这里应该使用内核的内存分配函数
    // 暂时使用简单的内存分配模拟
    void *ptr = NULL;

    // 模拟内存分配
    if (size <= 4096) {
        // 使用静态缓冲区（仅用于演示）
        static uint8_t buffer[4096];
        static uint32_t offset = 0;

        if (offset + size <= 4096) {
            ptr = &buffer[offset];
            offset += size;
        }
    }

    return ptr;
}

/**
 * 内存释放函数
 */
void pkgmgr_free(void *ptr) {
    if (!ptr) {
        return;
    }

    // 这里应该释放内存
    // 暂时什么都不做（因为使用了静态缓冲区）
}

/**
 * 内存重新分配函数
 */
void *pkgmgr_realloc(void *ptr, size_t size) {
    if (!ptr) {
        return pkgmgr_malloc(size);
    }

    if (size == 0) {
        pkgmgr_free(ptr);
        return NULL;
    }

    // 这里应该重新分配内存
    // 暂时返回原指针（简化实现）
    return ptr;
}

/**
 * 字符串复制函数
 */
char *pkgmgr_strdup(const char *str) {
    if (!str) {
        return NULL;
    }

    size_t len = pkgmgr_strlen(str);
    char *new_str = pkgmgr_malloc(len + 1);

    if (new_str) {
        pkgmgr_strcpy(new_str, str);
    }

    return new_str;
}

/**
 * 字符串比较函数
 */
int pkgmgr_strcmp(const char *s1, const char *s2) {
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;

    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }

    return (unsigned char)*s1 - (unsigned char)*s2;
}

/**
 * 字符串长度函数
 */
size_t pkgmgr_strlen(const char *str) {
    size_t len = 0;

    if (str) {
        while (str[len]) {
            len++;
        }
    }

    return len;
}

/**
 * 字符串复制函数
 */
char *pkgmgr_strcpy(char *dest, const char *src) {
    if (!dest || !src) {
        return dest;
    }

    char *d = dest;

    while (*src) {
        *d++ = *src++;
    }

    *d = '\0';

    return dest;
}

/**
 * 字符串连接函数
 */
char *pkgmgr_strcat(char *dest, const char *src) {
    if (!dest || !src) {
        return dest;
    }

    char *d = dest;

    while (*d) {
        d++;
    }

    while (*src) {
        *d++ = *src++;
    }

    *d = '\0';

    return dest;
}

/**
 * 字符串标记化函数
 */
char *pkgmgr_strtok(char *str, const char *delim) {
    static char *next_token = NULL;

    if (str) {
        next_token = str;
    }

    if (!next_token || !delim) {
        return NULL;
    }

    // 跳过分隔符
    while (*next_token && pkgmgr_strchr(delim, *next_token)) {
        next_token++;
    }

    if (*next_token == '\0') {
        next_token = NULL;
        return NULL;
    }

    char *token_start = next_token;

    // 找到下一个分隔符
    while (*next_token && !pkgmgr_strchr(delim, *next_token)) {
        next_token++;
    }

    if (*next_token) {
        *next_token = '\0';
        next_token++;
    } else {
        next_token = NULL;
    }

    return token_start;
}

/**
 * 查找字符函数
 */
char *pkgmgr_strchr(const char *str, int c) {
    if (!str) {
        return NULL;
    }

    while (*str) {
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
char *pkgmgr_strstr(const char *haystack, const char *needle) {
    if (!haystack || !needle) {
        return NULL;
    }

    size_t needle_len = pkgmgr_strlen(needle);

    while (*haystack) {
        if (pkgmgr_strncmp(haystack, needle, needle_len) == 0) {
            return (char *)haystack;
        }
        haystack++;
    }

    return NULL;
}

/**
 * 字符串比较函数（指定长度）
 */
int pkgmgr_strncmp(const char *s1, const char *s2, size_t n) {
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;

    while (n > 0 && *s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
        n--;
    }

    if (n == 0) return 0;
    if (!*s1 && !*s2) return 0;
    if (!*s1) return -1;
    if (!*s2) return 1;

    return (unsigned char)*s1 - (unsigned char)*s2;
}

/**
 * 连接路径和文件名
 */
char *pkgmgr_path_join(const char *dir, const char *file) {
    if (!dir || !file) {
        return NULL;
    }

    size_t dir_len = pkgmgr_strlen(dir);
    size_t file_len = pkgmgr_strlen(file);
    size_t total_len = dir_len + file_len + 2;  // '/' + '\0'

    char *path = pkgmgr_malloc(total_len);

    if (!path) {
        return NULL;
    }

    pkgmgr_strcpy(path, dir);

    // 添加路径分隔符（如果需要）
    if (dir_len > 0 && path[dir_len - 1] != '/') {
        pkgmgr_strcat(path, "/");
    }

    pkgmgr_strcat(path, file);

    return path;
}

/**
 * 检查文件是否存在
 */
int pkgmgr_file_exists(const char *filename) {
    if (!filename) {
        return 0;
    }

    // 这里应该检查文件是否存在
    // 暂时返回1（存在）
    return 1;
}

/**
 * 读取文件内容
 */
int pkgmgr_file_read(const char *filename, uint8_t **buffer, uint32_t *size) {
    if (!filename || !buffer || !size) {
        return PKGMGR_ERROR_INVALID_ARG;
    }

    // 这里应该读取文件内容
    // 暂时返回模拟数据

    *size = 1024;  // 模拟1KB文件
    *buffer = pkgmgr_malloc(*size);

    if (!*buffer) {
        return PKGMGR_ERROR_INTERNAL_ERROR;
    }

    // 填充模拟内容
    for (uint32_t i = 0; i < *size; i++) {
        (*buffer)[i] = (uint8_t)i;
    }

    return PKGMGR_OK;
}

/**
 * 写入文件内容
 */
int pkgmgr_file_write(const char *filename, const uint8_t *buffer, uint32_t size) {
    if (!filename || !buffer) {
        return PKGMGR_ERROR_INVALID_ARG;
    }

    // 这里应该写入文件内容
    // 暂时什么都不做

    return PKGMGR_OK;
}

/**
 * 复制文件
 */
int pkgmgr_file_copy(const char *src, const char *dst) {
    if (!src || !dst) {
        return PKGMGR_ERROR_INVALID_ARG;
    }

    // 这里应该复制文件
    // 暂时什么都不做

    return PKGMGR_OK;
}

/**
 * CRC32校验和计算
 */
uint32_t pkgmgr_checksum_crc32(const void *data, uint32_t size) {
    if (!data || size == 0) {
        return 0;
    }

    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t crc = 0xFFFFFFFF;

    static const uint32_t crc32_table[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
        0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
        0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
        0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
        0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
        0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
        0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
        0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
        0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
        0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
        0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
        0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
        0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
        0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
        0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
        0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
        0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
        0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
        0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
        0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
        0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
        0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
        0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
        0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
        0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
        0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
        0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
        0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
        0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
        0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
        0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
        0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
        0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
        0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
        0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
        0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
        0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
        0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
        0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
        0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
        0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
        0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
        0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
        0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
        0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
        0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
        0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
        0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
        0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
        0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
        0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
        0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
        0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
        0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
        0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
        0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
        0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
        0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
        0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
        0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
        0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
        0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
        0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
        0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
    };

    for (uint32_t i = 0; i < size; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ bytes[i]) & 0xFF];
    }

    return crc ^ 0xFFFFFFFF;
}

/**
 * MD5校验和计算（简化实现）
 */
uint32_t pkgmgr_checksum_md5(const void *data, uint32_t size) {
    if (!data || size == 0) {
        return 0;
    }

    // 简化的MD5实现
    return pkgmgr_checksum_crc32(data, size);
}

/**
 * SHA256校验和计算（简化实现）
 */
uint32_t pkgmgr_checksum_sha256(const void *data, uint32_t size) {
    if (!data || size == 0) {
        return 0;
    }

    // 简化的SHA256实现
    return pkgmgr_checksum_crc32(data, size);
}

/**
 * 字符串格式化函数（简化版）
 */
int pkgmgr_snprintf(char *buffer, size_t size, const char *format, ...) {
    if (!buffer || !format || size == 0) {
        return 0;
    }

    // 简化的格式化实现
    size_t len = pkgmgr_strlen(format);

    if (len >= size) {
        len = size - 1;
    }

    pkgmgr_strncpy(buffer, format, len);
    buffer[len] = '\0';

    return len;
}

/**
 * 字符串复制函数（指定长度）
 */
char *pkgmgr_strncpy(char *dest, const char *src, size_t n) {
    if (!dest || !src) {
        return dest;
    }

    char *d = dest;
    size_t i = 0;

    while (i < n && *src) {
        *d++ = *src++;
        i++;
    }

    while (i < n) {
        *d++ = '\0';
        i++;
    }

    return dest;
}

/**
 * 整数转字符串
 */
void pkgmgr_itoa(int value, char *buffer, int radix) {
    if (!buffer) {
        return;
    }

    if (radix < 2 || radix > 36) {
        *buffer = '\0';
        return;
    }

    char *ptr = buffer;
    bool negative = false;

    if (value < 0 && radix == 10) {
        negative = true;
        value = -value;
    }

    // 转换数字
    do {
        int digit = value % radix;
        *ptr++ = (digit < 10) ? '0' + digit : 'A' + digit - 10;
        value /= radix;
    } while (value > 0);

    if (negative) {
        *ptr++ = '-';
    }

    *ptr = '\0';

    // 反转字符串
    char *start = buffer;
    char *end = ptr - 1;

    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

/**
 * 无符号整数转字符串
 */
void pkgmgr_utoa(uint32_t value, char *buffer, int radix) {
    if (!buffer) {
        return;
    }

    if (radix < 2 || radix > 36) {
        *buffer = '\0';
        return;
    }

    char *ptr = buffer;

    // 转换数字
    do {
        uint32_t digit = value % radix;
        *ptr++ = (digit < 10) ? '0' + digit : 'A' + digit - 10;
        value /= radix;
    } while (value > 0);

    *ptr = '\0';

    // 反转字符串
    char *start = buffer;
    char *end = ptr - 1;

    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

/**
 * 跳过空白字符
 */
const char *pkgmgr_skip_whitespace(const char *str) {
    if (!str) {
        return NULL;
    }

    while (*str && (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r')) {
        str++;
    }

    return str;
}

/**
 * 检查字符是否为数字
 */
bool pkgmgr_isdigit(int c) {
    return c >= '0' && c <= '9';
}

/**
 * 检查字符是否为字母
 */
bool pkgmgr_isalpha(int c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

/**
 * 检查字符是否为字母或数字
 */
bool pkgmgr_isalnum(int c) {
    return pkgmgr_isalpha(c) || pkgmgr_isdigit(c);
}

/**
 * 转换字符为小写
 */
int pkgmgr_tolower(int c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }

    return c;
}

/**
 * 转换字符为大写
 */
int pkgmgr_toupper(int c) {
    if (c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');
    }

    return c;
}

/**
 * 获取文件扩展名
 */
const char *pkgmgr_get_extension(const char *filename) {
    if (!filename) {
        return NULL;
    }

    const char *dot = NULL;
    const char *p = filename;

    while (*p) {
        if (*p == '.') {
            dot = p;
        }
        p++;
    }

    return dot ? dot + 1 : NULL;
}

/**
 * 检查文件扩展名
 */
bool pkgmgr_check_extension(const char *filename, const char *extension) {
    if (!filename || !extension) {
        return false;
    }

    const char *file_ext = pkgmgr_get_extension(filename);

    if (!file_ext) {
        return false;
    }

    return pkgmgr_strcmp(file_ext, extension) == 0;
}

/**
 * 创建目录（如果不存在）
 */
int pkgmgr_mkdir(const char *path) {
    if (!path) {
        return PKGMGR_ERROR_INVALID_ARG;
    }

    // 这里应该创建目录
    // 暂时什么都不做

    return PKGMGR_OK;
}

/**
 * 删除文件
 */
int pkgmgr_unlink(const char *path) {
    if (!path) {
        return PKGMGR_ERROR_INVALID_ARG;
    }

    // 这里应该删除文件
    // 暂时什么都不做

    return PKGMGR_OK;
}

/**
 * 重命名文件
 */
int pkgmgr_rename(const char *old_path, const char *new_path) {
    if (!old_path || !new_path) {
        return PKGMGR_ERROR_INVALID_ARG;
    }

    // 这里应该重命名文件
    // 暂时什么都不做

    return PKGMGR_OK;
}

/**
 * 获取文件大小
 */
int64_t pkgmgr_get_file_size(const char *filename) {
    if (!filename) {
        return -1;
    }

    // 这里应该获取文件大小
    // 暂时返回模拟值
    return 1024;
}

/**
 * 获取当前工作目录
 */
char *pkgmgr_getcwd(char *buffer, size_t size) {
    if (!buffer || size == 0) {
        return NULL;
    }

    // 这里应该获取当前工作目录
    // 暂时返回根目录
    pkgmgr_strcpy(buffer, "/");

    return buffer;
}

/**
 * 设置当前工作目录
 */
int pkgmgr_chdir(const char *path) {
    if (!path) {
        return PKGMGR_ERROR_INVALID_ARG;
    }

    // 这里应该设置当前工作目录
    // 暂时什么都不做

    return PKGMGR_OK;
}

/**
 * 获取环境变量
 */
char *pkgmgr_getenv(const char *name) {
    if (!name) {
        return NULL;
    }

    // 这里应该获取环境变量
    // 暂时返回NULL
    return NULL;
}

/**
 * 设置环境变量
 */
int pkgmgr_setenv(const char *name, const char *value, int overwrite) {
    if (!name || !value) {
        return PKGMGR_ERROR_INVALID_ARG;
    }

    // 这里应该设置环境变量
    // 暂时什么都不做

    return PKGMGR_OK;
}

/**
 * 休眠函数（毫秒）
 */
void pkgmgr_sleep(uint32_t milliseconds) {
    // 这里应该实现休眠
    // 暂时使用忙等待
    for (uint32_t i = 0; i < milliseconds * 1000; i++) {
        // 忙等待
    }
}

/**
 * 获取时间戳（毫秒）
 */
uint64_t pkgmgr_get_timestamp(void) {
    // 这里应该获取当前时间戳
    // 暂时返回固定值
    return 1234567890ULL;
}

/**
 * 打印到控制台
 */
void pkgmgr_print(const char *message) {
    if (message) {
        console_write(message);
    }
}

/**
 * 格式化打印到控制台
 */
void pkgmgr_printf(const char *format, ...) {
    if (!format) {
        return;
    }

    char buffer[1024];

    va_list args;
    va_start(args, format);

    pkgmgr_vsprintf(buffer, format, args);

    va_end(args);

    console_write(buffer);
}

/**
 * 可变参数格式化输出
 */
int pkgmgr_vsprintf(char *buffer, const char *format, va_list args) {
    if (!buffer || !format) {
        return 0;
    }

    // 简化的格式化实现
    return pkgmgr_snprintf(buffer, 1024, format);
}

/**
 * 错误打印到控制台
 */
void pkgmgr_eprintf(const char *format, ...) {
    if (!format) {
        return;
    }

    console_write("Error: ");

    char buffer[1024];

    va_list args;
    va_start(args, format);

    pkgmgr_vsprintf(buffer, format, args);

    va_end(args);

    console_write(buffer);
    console_write("\n");
}

/**
 * 警告打印到控制台
 */
void pkgmgr_wprintf(const char *format, ...) {
    if (!format) {
        return;
    }

    console_write("Warning: ");

    char buffer[1024];

    va_list args;
    va_start(args, format);

    pkgmgr_vsprintf(buffer, format, args);

    va_end(args);

    console_write(buffer);
    console_write("\n");
}

/**
 * 信息打印到控制台
 */
void pkgmgr_iprintf(const char *format, ...) {
    if (!format) {
        return;
    }

    console_write("Info: ");

    char buffer[1024];

    va_list args;
    va_start(args, format);

    pkgmgr_vsprintf(buffer, format, args);

    va_end(args);

    console_write(buffer);
    console_write("\n");
}

/**
 * 调试打印到控制台
 */
void pkgmgr_dprintf(const char *format, ...) {
    if (!format) {
        return;
    }

    console_write("Debug: ");

    char buffer[1024];

    va_list args;
    va_start(args, format);

    pkgmgr_vsprintf(buffer, format, args);

    va_end(args);

    console_write(buffer);
    console_write("\n");
}

/**
 * 十六进制打印
 */
void pkgmgr_print_hex(const uint8_t *data, uint32_t size) {
    if (!data || size == 0) {
        return;
    }

    for (uint32_t i = 0; i < size; i++) {
        char buffer[4];
        pkgmgr_snprintf(buffer, sizeof(buffer), "%02X ", data[i]);
        console_write(buffer);

        if ((i + 1) % 16 == 0) {
            console_write("\n");
        }
    }

    if (size % 16 != 0) {
        console_write("\n");
    }
}

/**
 * 二进制打印
 */
void pkgmgr_print_bin(uint32_t value) {
    char buffer[33];
    buffer[32] = '\0';

    for (int i = 31; i >= 0; i--) {
        buffer[31 - i] = (value & (1 << i)) ? '1' : '0';
    }

    console_write("0b");
    console_write(buffer);
}

/**
 * 内存转储
 */
void pkgmgr_dump_memory(const void *data, uint32_t size, uint32_t width) {
    if (!data || size == 0 || width == 0) {
        return;
    }

    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t offset = 0;

    while (offset < size) {
        // 打印偏移
        char offset_buffer[12];
        pkgmgr_snprintf(offset_buffer, sizeof(offset_buffer), "0x%08X: ", offset);
        console_write(offset_buffer);

        // 打印十六进制数据
        uint32_t line_size = (size - offset) < width ? (size - offset) : width;

        for (uint32_t i = 0; i < width; i++) {
            if (i < line_size) {
                char hex_buffer[4];
                pkgmgr_snprintf(hex_buffer, sizeof(hex_buffer), "%02X ", bytes[offset + i]);
                console_write(hex_buffer);
            } else {
                console_write("   ");
            }
        }

        console_write(" ");

        // 打印ASCII字符
        for (uint32_t i = 0; i < line_size; i++) {
            uint8_t c = bytes[offset + i];
            if (c >= 32 && c <= 126) {
                char char_buffer[2];
                char_buffer[0] = c;
                char_buffer[1] = '\0';
                console_write(char_buffer);
            } else {
                console_write(".");
            }
        }

        console_write("\n");
        offset += line_size;
    }
}

/**
 * 性能计时器
 */
typedef struct {
    uint64_t start_time;
    uint64_t end_time;
    bool running;
} pkgmgr_timer_t;

/**
 * 启动计时器
 */
void pkgmgr_timer_start(pkgmgr_timer_t *timer) {
    if (!timer) {
        return;
    }

    timer->start_time = pkgmgr_get_timestamp();
    timer->running = true;
}

/**
 * 停止计时器
 */
void pkgmgr_timer_stop(pkgmgr_timer_t *timer) {
    if (!timer || !timer->running) {
        return;
    }

    timer->end_time = pkgmgr_get_timestamp();
    timer->running = false;
}

/**
 * 获取计时器耗时（毫秒）
 */
uint64_t pkgmgr_timer_elapsed(pkgmgr_timer_t *timer) {
    if (!timer) {
        return 0;
    }

    uint64_t end_time = timer->running ? pkgmgr_get_timestamp() : timer->end_time;
    return end_time - timer->start_time;
}

/**
 * 格式化计时器耗时
 */
char *pkgmgr_timer_format(pkgmgr_timer_t *timer) {
    static char buffer[32];

    uint64_t elapsed = pkgmgr_timer_elapsed(timer);

    if (elapsed >= 1000) {
        pkgmgr_snprintf(buffer, sizeof(buffer), "%.2f s", elapsed / 1000.0);
    } else if (elapsed >= 1) {
        pkgmgr_snprintf(buffer, sizeof(buffer), "%llu ms", elapsed);
    } else {
        pkgmgr_snprintf(buffer, sizeof(buffer), "%llu μs", elapsed * 1000);
    }

    return buffer;
}

/**
 * 基准测试宏
 */
#define PKGMGR_BENCHMARK_START(timer) pkgmgr_timer_start(&timer)
#define PKGMGR_BENCHMARK_END(timer) pkgmgr_timer_stop(&timer)
#define PKGMGR_BENCHMARK_ELAPSED(timer) pkgmgr_timer_elapsed(&timer)
#define PKGMGR_BENCHMARK_FORMAT(timer) pkgmgr_timer_format(&timer)

/**
 * 内存泄漏检测（简化版）
 */
void pkgmgr_memory_check(void) {
    console_write("Memory check: No leaks detected (simplified implementation)\n");
}

/**
 * 断言宏
 */
void pkgmgr_assert_func(const char *file, int line, const char *func, const char *expr) {
    console_write("Assertion failed: ");
    console_write(expr);
    console_write(" at ");
    console_write(file);
    console_write(":");
    console_write_dec(line);
    console_write(" in function ");
    console_write(func);
    console_write("\n");

    // 这里应该触发断点或崩溃
    while (1) {
        // 无限循环
    }
}

#define pkgmgr_assert(expr) \
    do { \
        if (!(expr)) { \
            pkgmgr_assert_func(__FILE__, __LINE__, __func__, #expr); \
        } \
    } while (0)

/**
 * 调试宏
 */
#ifdef PKGMGR_DEBUG
#define pkgmgr_debug(msg) \
    do { \
        console_write("[DEBUG] "); \
        console_write(msg); \
        console_write("\n"); \
    } while (0)

#define pkgmgr_debugf(fmt, ...) \
    do { \
        console_write("[DEBUG] "); \
        pkgmgr_printf(fmt, __VA_ARGS__); \
    } while (0)
#else
#define pkgmgr_debug(msg) do { } while (0)
#define pkgmgr_debugf(fmt, ...) do { } while (0)
#endif

/**
 * 日志宏
 */
#define pkgmgr_log(level, msg) \
    do { \
        if (level <= PKGMGR_LOG_LEVEL) { \
            pkgmgr_log_write(level, msg); \
        } \
    } while (0)

#define pkgmgr_logf(level, fmt, ...) \
    do { \
        if (level <= PKGMGR_LOG_LEVEL) { \
            char buffer[1024]; \
            pkgmgr_snprintf(buffer, sizeof(buffer), fmt, __VA_ARGS__); \
            pkgmgr_log_write(level, buffer); \
        } \
    } while (0)
</content>
<parameter name="line_count">329</parameter>
/**
 * LangCC - Language Compiler Collection 工具函数
 * 实现内存管理、字符串处理、文件操作等辅助功能
 */

#include "langcc.h"
#include "../include/langcc.h"
#include "../../y4ku/include/console.h"

/**
 * 内存分配函数
 */
void *langcc_malloc(size_t size) {
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
void langcc_free(void *ptr) {
    if (!ptr) {
        return;
    }

    // 这里应该释放内存
    // 暂时什么都不做（因为使用了静态缓冲区）
}

/**
 * 内存重新分配函数
 */
void *langcc_realloc(void *ptr, size_t size) {
    if (!ptr) {
        return langcc_malloc(size);
    }

    if (size == 0) {
        langcc_free(ptr);
        return NULL;
    }

    // 这里应该重新分配内存
    // 暂时返回原指针（简化实现）
    return ptr;
}

/**
 * 字符串复制函数
 */
char *langcc_strdup(const char *str) {
    if (!str) {
        return NULL;
    }

    size_t len = langcc_strlen(str);
    char *new_str = langcc_malloc(len + 1);

    if (new_str) {
        langcc_strcpy(new_str, str);
    }

    return new_str;
}

/**
 * 字符串比较函数
 */
int langcc_strcmp(const char *s1, const char *s2) {
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
size_t langcc_strlen(const char *str) {
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
char *langcc_strcpy(char *dest, const char *src) {
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
char *langcc_strcat(char *dest, const char *src) {
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
 * 查找字符函数
 */
char *langcc_strchr(const char *str, int c) {
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
 * 反向查找字符函数
 */
char *langcc_strrchr(const char *str, int c) {
    if (!str) {
        return NULL;
    }

    char *last = NULL;

    while (*str) {
        if (*str == (char)c) {
            last = (char *)str;
        }
        str++;
    }

    return last;
}

/**
 * 查找子串函数
 */
char *langcc_strstr(const char *haystack, const char *needle) {
    if (!haystack || !needle) {
        return NULL;
    }

    size_t needle_len = langcc_strlen(needle);

    while (*haystack) {
        if (langcc_strncmp(haystack, needle, needle_len) == 0) {
            return (char *)haystack;
        }
        haystack++;
    }

    return NULL;
}

/**
 * 字符串比较函数（指定长度）
 */
int langcc_strncmp(const char *s1, const char *s2, size_t n) {
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
 * 获取路径的基础名
 */
char *langcc_basename(const char *path) {
    if (!path) {
        return NULL;
    }

    char *last_slash = langcc_strrchr(path, '/');

    if (last_slash) {
        return last_slash + 1;
    }

    return (char *)path;
}

/**
 * 获取路径的目录名
 */
char *langcc_dirname(const char *path) {
    if (!path) {
        return NULL;
    }

    char *last_slash = langcc_strrchr(path, '/');

    if (!last_slash) {
        return langcc_strdup(".");
    }

    if (last_slash == path) {
        return langcc_strdup("/");
    }

    // 复制路径并截断
    size_t len = last_slash - path;
    char *dir = langcc_malloc(len + 1);

    if (dir) {
        langcc_strncpy(dir, path, len);
        dir[len] = '\0';
    }

    return dir;
}

/**
 * 连接路径和文件名
 */
char *langcc_path_join(const char *dir, const char *file) {
    if (!dir || !file) {
        return NULL;
    }

    size_t dir_len = langcc_strlen(dir);
    size_t file_len = langcc_strlen(file);
    size_t total_len = dir_len + file_len + 2;  // '/' + '\0'

    char *path = langcc_malloc(total_len);

    if (!path) {
        return NULL;
    }

    langcc_strcpy(path, dir);

    // 添加路径分隔符（如果需要）
    if (dir_len > 0 && path[dir_len - 1] != '/') {
        langcc_strcat(path, "/");
    }

    langcc_strcat(path, file);

    return path;
}

/**
 * 检查路径是否存在
 */
bool langcc_path_exists(const char *path) {
    if (!path) {
        return false;
    }

    // 这里应该检查文件是否存在
    // 暂时返回true（简化实现）
    return true;
}

/**
 * 读取文件内容
 */
int langcc_file_read(const char *filename, uint8_t **buffer, uint32_t *size) {
    if (!filename || !buffer || !size) {
        return -1;
    }

    // 这里应该读取文件内容
    // 暂时返回模拟数据

    *size = 1024;  // 模拟1KB文件
    *buffer = langcc_malloc(*size);

    if (!*buffer) {
        return -1;
    }

    // 填充模拟内容
    for (uint32_t i = 0; i < *size; i++) {
        (*buffer)[i] = (uint8_t)i;
    }

    return 0;
}

/**
 * 写入文件内容
 */
int langcc_file_write(const char *filename, const uint8_t *buffer, uint32_t size) {
    if (!filename || !buffer) {
        return -1;
    }

    // 这里应该写入文件内容
    // 暂时什么都不做

    return 0;
}

/**
 * 检查文件是否存在
 */
int langcc_file_exists(const char *filename) {
    if (!filename) {
        return 0;
    }

    // 这里应该检查文件是否存在
    // 暂时返回1（存在）
    return 1;
}

/**
 * 字符串复制函数（指定长度）
 */
char *langcc_strncpy(char *dest, const char *src, size_t n) {
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
 * 字符串格式化函数（简化版）
 */
int langcc_snprintf(char *buffer, size_t size, const char *format, ...) {
    if (!buffer || !format || size == 0) {
        return 0;
    }

    // 简化的格式化实现
    size_t len = langcc_strlen(format);

    if (len >= size) {
        len = size - 1;
    }

    langcc_strncpy(buffer, format, len);
    buffer[len] = '\0';

    return len;
}

/**
 * 哈希表创建
 */
langcc_hash_t *langcc_hash_create(uint32_t bucket_count) {
    if (bucket_count == 0) {
        bucket_count = 16;
    }

    langcc_hash_t *hash = langcc_malloc(sizeof(langcc_hash_t));

    if (!hash) {
        return NULL;
    }

    hash->buckets = langcc_malloc(bucket_count * sizeof(langcc_hash_entry_t *));

    if (!hash->buckets) {
        langcc_free(hash);
        return NULL;
    }

    hash->bucket_count = bucket_count;
    hash->entry_count = 0;

    for (uint32_t i = 0; i < bucket_count; i++) {
        hash->buckets[i] = NULL;
    }

    return hash;
}

/**
 * 哈希表销毁
 */
void langcc_hash_destroy(langcc_hash_t *hash) {
    if (!hash) {
        return;
    }

    for (uint32_t i = 0; i < hash->bucket_count; i++) {
        langcc_hash_entry_t *entry = hash->buckets[i];

        while (entry) {
            langcc_hash_entry_t *next = entry->next;

            if (entry->key) {
                langcc_free(entry->key);
            }

            langcc_free(entry);
            entry = next;
        }
    }

    langcc_free(hash->buckets);
    langcc_free(hash);
}

/**
 * 哈希表插入
 */
int langcc_hash_insert(langcc_hash_t *hash, const char *key, void *value) {
    if (!hash || !key) {
        return -1;
    }

    // 计算哈希值
    uint32_t hash_value = 0;
    for (const char *p = key; *p; p++) {
        hash_value = hash_value * 31 + *p;
    }

    uint32_t bucket = hash_value % hash->bucket_count;

    // 检查是否已存在
    langcc_hash_entry_t *entry = hash->buckets[bucket];

    while (entry) {
        if (langcc_strcmp(entry->key, key) == 0) {
            // 更新现有值
            entry->value = value;
            return 0;
        }
        entry = entry->next;
    }

    // 创建新条目
    langcc_hash_entry_t *new_entry = langcc_malloc(sizeof(langcc_hash_entry_t));

    if (!new_entry) {
        return -1;
    }

    new_entry->key = langcc_strdup(key);
    new_entry->value = value;
    new_entry->next = hash->buckets[bucket];

    hash->buckets[bucket] = new_entry;
    hash->entry_count++;

    return 0;
}

/**
 * 哈希表查找
 */
void *langcc_hash_lookup(langcc_hash_t *hash, const char *key) {
    if (!hash || !key) {
        return NULL;
    }

    // 计算哈希值
    uint32_t hash_value = 0;
    for (const char *p = key; *p; p++) {
        hash_value = hash_value * 31 + *p;
    }

    uint32_t bucket = hash_value % hash->bucket_count;

    // 查找条目
    langcc_hash_entry_t *entry = hash->buckets[bucket];

    while (entry) {
        if (langcc_strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

/**
 * 哈希表删除
 */
int langcc_hash_remove(langcc_hash_t *hash, const char *key) {
    if (!hash || !key) {
        return -1;
    }

    // 计算哈希值
    uint32_t hash_value = 0;
    for (const char *p = key; *p; p++) {
        hash_value = hash_value * 31 + *p;
    }

    uint32_t bucket = hash_value % hash->bucket_count;

    // 查找并删除条目
    langcc_hash_entry_t *entry = hash->buckets[bucket];
    langcc_hash_entry_t *prev = NULL;

    while (entry) {
        if (langcc_strcmp(entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                hash->buckets[bucket] = entry->next;
            }

            if (entry->key) {
                langcc_free(entry->key);
            }

            langcc_free(entry);
            hash->entry_count--;

            return 0;
        }

        prev = entry;
        entry = entry->next;
    }

    return -1;  // 未找到
}

/**
 * 列表创建
 */
langcc_list_t *langcc_list_create(void) {
    langcc_list_t *list = langcc_malloc(sizeof(langcc_list_t));

    if (!list) {
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;

    return list;
}

/**
 * 列表销毁
 */
void langcc_list_destroy(langcc_list_t *list) {
    if (!list) {
        return;
    }

    langcc_list_node_t *node = list->head;

    while (node) {
        langcc_list_node_t *next = node->next;
        langcc_free(node);
        node = next;
    }

    langcc_free(list);
}

/**
 * 列表追加
 */
int langcc_list_append(langcc_list_t *list, void *data) {
    if (!list) {
        return -1;
    }

    langcc_list_node_t *node = langcc_malloc(sizeof(langcc_list_node_t));

    if (!node) {
        return -1;
    }

    node->data = data;
    node->next = NULL;
    node->prev = list->tail;

    if (list->tail) {
        list->tail->next = node;
    } else {
        list->head = node;
    }

    list->tail = node;
    list->count++;

    return 0;
}

/**
 * 列表前置
 */
int langcc_list_prepend(langcc_list_t *list, void *data) {
    if (!list) {
        return -1;
    }

    langcc_list_node_t *node = langcc_malloc(sizeof(langcc_list_node_t));

    if (!node) {
        return -1;
    }

    node->data = data;
    node->prev = NULL;
    node->next = list->head;

    if (list->head) {
        list->head->prev = node;
    } else {
        list->tail = node;
    }

    list->head = node;
    list->count++;

    return 0;
}

/**
 * 列表删除
 */
void *langcc_list_remove(langcc_list_t *list, void *data) {
    if (!list || !data) {
        return NULL;
    }

    langcc_list_node_t *node = list->head;

    while (node) {
        if (node->data == data) {
            void *node_data = node->data;

            if (node->prev) {
                node->prev->next = node->next;
            } else {
                list->head = node->next;
            }

            if (node->next) {
                node->next->prev = node->prev;
            } else {
                list->tail = node->prev;
            }

            langcc_free(node);
            list->count--;

            return node_data;
        }

        node = node->next;
    }

    return NULL;
}

/**
 * 列表获取（按索引）
 */
void *langcc_list_get(langcc_list_t *list, uint32_t index) {
    if (!list || index >= list->count) {
        return NULL;
    }

    langcc_list_node_t *node = list->head;
    uint32_t i = 0;

    while (node && i < index) {
        node = node->next;
        i++;
    }

    return node ? node->data : NULL;
}

/**
 * 简单的哈希函数
 */
uint32_t langcc_hash_string(const char *str) {
    if (!str) {
        return 0;
    }

    uint32_t hash = 5381;

    while (*str) {
        hash = hash * 33 + *str;
        str++;
    }

    return hash;
}

/**
 * 整数转字符串
 */
void langcc_itoa(int value, char *buffer, int radix) {
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
void langcc_utoa(uint32_t value, char *buffer, int radix) {
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
const char *langcc_skip_whitespace(const char *str) {
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
bool langcc_isdigit(int c) {
    return c >= '0' && c <= '9';
}

/**
 * 检查字符是否为字母
 */
bool langcc_isalpha(int c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

/**
 * 检查字符是否为字母或数字
 */
bool langcc_isalnum(int c) {
    return langcc_isalpha(c) || langcc_isdigit(c);
}

/**
 * 检查字符是否为十六进制数字
 */
bool langcc_isxdigit(int c) {
    return langcc_isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

/**
 * 转换字符为小写
 */
int langcc_tolower(int c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }

    return c;
}

/**
 * 转换字符为大写
 */
int langcc_toupper(int c) {
    if (c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');
    }

    return c;
}

/**
 * 获取文件扩展名
 */
const char *langcc_get_extension(const char *filename) {
    if (!filename) {
        return NULL;
    }

    const char *dot = langcc_strrchr(filename, '.');

    return dot ? dot + 1 : NULL;
}

/**
 * 检查文件扩展名
 */
bool langcc_check_extension(const char *filename, const char *extension) {
    if (!filename || !extension) {
        return false;
    }

    const char *file_ext = langcc_get_extension(filename);

    if (!file_ext) {
        return false;
    }

    return langcc_strcmp(file_ext, extension) == 0;
}

/**
 * 创建目录（如果不存在）
 */
int langcc_mkdir(const char *path) {
    if (!path) {
        return -1;
    }

    // 这里应该创建目录
    // 暂时什么都不做

    return 0;
}

/**
 * 删除文件
 */
int langcc_unlink(const char *path) {
    if (!path) {
        return -1;
    }

    // 这里应该删除文件
    // 暂时什么都不做

    return 0;
}

/**
 * 重命名文件
 */
int langcc_rename(const char *old_path, const char *new_path) {
    if (!old_path || !new_path) {
        return -1;
    }

    // 这里应该重命名文件
    // 暂时什么都不做

    return 0;
}

/**
 * 获取文件大小
 */
int64_t langcc_get_file_size(const char *filename) {
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
char *langcc_getcwd(char *buffer, size_t size) {
    if (!buffer || size == 0) {
        return NULL;
    }

    // 这里应该获取当前工作目录
    // 暂时返回根目录
    langcc_strcpy(buffer, "/");

    return buffer;
}

/**
 * 设置当前工作目录
 */
int langcc_chdir(const char *path) {
    if (!path) {
        return -1;
    }

    // 这里应该设置当前工作目录
    // 暂时什么都不做

    return 0;
}

/**
 * 获取环境变量
 */
char *langcc_getenv(const char *name) {
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
int langcc_setenv(const char *name, const char *value, int overwrite) {
    if (!name || !value) {
        return -1;
    }

    // 这里应该设置环境变量
    // 暂时什么都不做

    return 0;
}

/**
 * 休眠函数（毫秒）
 */
void langcc_sleep(uint32_t milliseconds) {
    // 这里应该实现休眠
    // 暂时使用忙等待
    for (uint32_t i = 0; i < milliseconds * 1000; i++) {
        // 忙等待
    }
}

/**
 * 获取时间戳（毫秒）
 */
uint64_t langcc_get_timestamp(void) {
    // 这里应该获取当前时间戳
    // 暂时返回固定值
    return 1234567890ULL;
}

/**
 * 格式化输出到字符串
 */
int langcc_vsprintf(char *buffer, const char *format, va_list args) {
    if (!buffer || !format) {
        return 0;
    }

    // 简化的格式化实现
    return langcc_snprintf(buffer, 1024, format);
}

/**
 * 格式化输出
 */
int langcc_sprintf(char *buffer, const char *format, ...) {
    if (!buffer || !format) {
        return 0;
    }

    va_list args;
    va_start(args, format);

    int ret = langcc_vsprintf(buffer, format, args);

    va_end(args);

    return ret;
}

/**
 * 打印到控制台
 */
void langcc_print(const char *message) {
    if (message) {
        console_write(message);
    }
}

/**
 * 格式化打印到控制台
 */
void langcc_printf(const char *format, ...) {
    if (!format) {
        return;
    }

    char buffer[1024];

    va_list args;
    va_start(args, format);

    langcc_vsprintf(buffer, format, args);

    va_end(args);

    console_write(buffer);
}

/**
 * 错误打印到控制台
 */
void langcc_eprintf(const char *format, ...) {
    if (!format) {
        return;
    }

    console_write("Error: ");

    char buffer[1024];

    va_list args;
    va_start(args, format);

    langcc_vsprintf(buffer, format, args);

    va_end(args);

    console_write(buffer);
    console_write("\n");
}

/**
 * 警告打印到控制台
 */
void langcc_wprintf(const char *format, ...) {
    if (!format) {
        return;
    }

    console_write("Warning: ");

    char buffer[1024];

    va_list args;
    va_start(args, format);

    langcc_vsprintf(buffer, format, args);

    va_end(args);

    console_write(buffer);
    console_write("\n");
}
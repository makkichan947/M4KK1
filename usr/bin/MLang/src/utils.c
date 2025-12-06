/**
 * M4KK1 MLang Compiler - Utility Functions
 * MLang编译器工具函数实现
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "mlang.h"

/* 内存管理 */
static void *mlang_memory_pool = NULL;
static size_t mlang_memory_used = 0;

/* 错误处理回调 */
static void (*error_handler)(const char *message, int line, int column) = NULL;

/* 初始化内存池 */
void mlang_memory_init(void) {
    if (!mlang_memory_pool) {
        mlang_memory_pool = malloc(1024 * 1024); /* 1MB内存池 */
        if (!mlang_memory_pool) {
            fprintf(stderr, "Failed to initialize memory pool\n");
            exit(1);
        }
        mlang_memory_used = 0;
    }
}

/* 分配内存 */
void *mlang_malloc(size_t size) {
    if (!mlang_memory_pool) {
        mlang_memory_init();
    }

    if (mlang_memory_used + size > 1024 * 1024) {
        /* 内存池不足，使用系统malloc */
        return malloc(size);
    }

    void *ptr = (char *)mlang_memory_pool + mlang_memory_used;
    mlang_memory_used += size;
    return ptr;
}

/* 释放内存 */
void mlang_free(void *ptr) {
    /* 简化实现，不实际释放内存 */
    if (ptr) {
        /* 检查是否在内存池中 */
        if (ptr >= mlang_memory_pool &&
            ptr < (char *)mlang_memory_pool + 1024 * 1024) {
            /* 在内存池中，不释放 */
            return;
        } else {
            /* 使用系统malloc分配的，释放 */
            free(ptr);
        }
    }
}

/* 重新分配内存 */
void *mlang_realloc(void *ptr, size_t size) {
    if (!ptr) {
        return mlang_malloc(size);
    }

    /* 检查是否在内存池中 */
    if (ptr >= mlang_memory_pool &&
        ptr < (char *)mlang_memory_pool + 1024 * 1024) {
        /* 在内存池中，分配新内存并复制 */
        void *new_ptr = mlang_malloc(size);
        if (new_ptr) {
            memcpy(new_ptr, ptr, size);
        }
        return new_ptr;
    } else {
        /* 使用系统malloc分配的，使用系统realloc */
        return realloc(ptr, size);
    }
}

/* 复制字符串 */
char *mlang_strdup(const char *str) {
    if (!str) return NULL;

    size_t len = strlen(str) + 1;
    char *new_str = mlang_malloc(len);
    if (new_str) {
        memcpy(new_str, str, len);
    }
    return new_str;
}

/* 复制字符串（限制长度） */
char *mlang_strndup(const char *str, size_t n) {
    if (!str) return NULL;

    size_t len = strlen(str);
    if (len > n) len = n;

    char *new_str = mlang_malloc(len + 1);
    if (new_str) {
        memcpy(new_str, str, len);
        new_str[len] = '\0';
    }
    return new_str;
}

/* 字符串比较 */
int mlang_strcmp(const char *s1, const char *s2) {
    return strcmp(s1, s2);
}

/* 字符串比较（忽略大小写） */
int mlang_strcasecmp(const char *s1, const char *s2) {
    return strcasecmp(s1, s2);
}

/* 字符串连接 */
char *mlang_strcat(char *dest, const char *src) {
    return strcat(dest, src);
}

/* 字符串复制 */
char *mlang_strcpy(char *dest, const char *src) {
    return strcpy(dest, src);
}

/* 字符串长度 */
size_t mlang_strlen(const char *str) {
    return strlen(str);
}

/* 读取文件 */
char *mlang_read_file(const char *filename, size_t *size) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buffer = mlang_malloc(file_size + 1);
    if (!buffer) {
        fclose(fp);
        return NULL;
    }

    size_t read_size = fread(buffer, 1, file_size, fp);
    fclose(fp);

    if (read_size != (size_t)file_size) {
        mlang_free(buffer);
        return NULL;
    }

    buffer[file_size] = '\0';

    if (size) {
        *size = file_size;
    }

    return buffer;
}

/* 写入文件 */
int mlang_write_file(const char *filename, const char *data, size_t size) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        return -1;
    }

    size_t write_size = fwrite(data, 1, size, fp);
    fclose(fp);

    return write_size == size ? 0 : -1;
}

/* 检查文件是否存在 */
int mlang_file_exists(const char *filename) {
    return access(filename, F_OK) == 0;
}

/* 获取目录名 */
char *mlang_get_dirname(const char *path) {
    if (!path) return NULL;

    char *path_copy = mlang_strdup(path);
    if (!path_copy) return NULL;

    char *last_slash = strrchr(path_copy, '/');
    if (last_slash) {
        *last_slash = '\0';
        return path_copy;
    } else {
        mlang_free(path_copy);
        return mlang_strdup(".");
    }
}

/* 获取文件名 */
char *mlang_get_basename(const char *path) {
    if (!path) return NULL;

    char *last_slash = strrchr(path, '/');
    return mlang_strdup(last_slash ? last_slash + 1 : path);
}

/* 获取文件扩展名 */
char *mlang_get_extension(const char *path) {
    if (!path) return NULL;

    char *last_dot = strrchr(path, '.');
    return mlang_strdup(last_dot ? last_dot + 1 : "");
}

/* 连接路径 */
char *mlang_join_path(const char *dir, const char *file) {
    if (!dir || !file) return NULL;

    size_t dir_len = strlen(dir);
    size_t file_len = strlen(file);
    size_t total_len = dir_len + file_len + 2; /* '/' + '\0' */

    char *path = mlang_malloc(total_len);
    if (!path) return NULL;

    strcpy(path, dir);

    if (dir_len > 0 && path[dir_len - 1] != '/') {
        strcat(path, "/");
    }

    strcat(path, file);

    return path;
}

/* 设置错误处理函数 */
void mlang_set_error_handler(void (*handler)(const char *message, int line, int column)) {
    error_handler = handler;
}

/* 报告错误 */
void mlang_error(const char *file, int line, int column, const char *format, ...) {
    va_list args;
    va_start(args, format);

    fprintf(stderr, "Error: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, " at %s:%d:%d\n", file, line, column);

    va_end(args);

    if (error_handler) {
        char message[256];
        vsnprintf(message, sizeof(message), format, args);
        error_handler(message, line, column);
    }
}

/* 报告警告 */
void mlang_warning(const char *file, int line, int column, const char *format, ...) {
    va_list args;
    va_start(args, format);

    fprintf(stderr, "Warning: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, " at %s:%d:%d\n", file, line, column);

    va_end(args);
}

/* 哈希表实现 */
mlang_hash_table_t *mlang_hash_create(uint32_t bucket_count) {
    mlang_hash_table_t *table = mlang_malloc(sizeof(mlang_hash_table_t));
    if (!table) return NULL;

    table->buckets = mlang_malloc(bucket_count * sizeof(mlang_hash_entry_t *));
    if (!table->buckets) {
        mlang_free(table);
        return NULL;
    }

    memset(table->buckets, 0, bucket_count * sizeof(mlang_hash_entry_t *));
    table->bucket_count = bucket_count;
    table->entry_count = 0;

    return table;
}

/* 销毁哈希表 */
void mlang_hash_destroy(mlang_hash_table_t *table) {
    if (!table) return;

    for (uint32_t i = 0; i < table->bucket_count; i++) {
        mlang_hash_entry_t *entry = table->buckets[i];
        while (entry) {
            mlang_hash_entry_t *next = entry->next;
            mlang_free(entry->key);
            mlang_free(entry);
            entry = next;
        }
    }

    mlang_free(table->buckets);
    mlang_free(table);
}

/* 哈希函数 */
static uint32_t hash_string(const char *str) {
    uint32_t hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

/* 插入哈希表 */
int mlang_hash_insert(mlang_hash_table_t *table, const char *key, void *value) {
    if (!table || !key) return -1;

    uint32_t bucket = hash_string(key) % table->bucket_count;

    /* 检查是否已存在 */
    mlang_hash_entry_t *entry = table->buckets[bucket];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return 0;
        }
        entry = entry->next;
    }

    /* 创建新条目 */
    entry = mlang_malloc(sizeof(mlang_hash_entry_t));
    if (!entry) return -1;

    entry->key = mlang_strdup(key);
    entry->value = value;
    entry->next = table->buckets[bucket];
    table->buckets[bucket] = entry;
    table->entry_count++;

    return 0;
}

/* 查找哈希表 */
void *mlang_hash_lookup(mlang_hash_table_t *table, const char *key) {
    if (!table || !key) return NULL;

    uint32_t bucket = hash_string(key) % table->bucket_count;
    mlang_hash_entry_t *entry = table->buckets[bucket];

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

/* 删除哈希表条目 */
int mlang_hash_remove(mlang_hash_table_t *table, const char *key) {
    if (!table || !key) return -1;

    uint32_t bucket = hash_string(key) % table->bucket_count;
    mlang_hash_entry_t *entry = table->buckets[bucket];
    mlang_hash_entry_t *prev = NULL;

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                table->buckets[bucket] = entry->next;
            }

            mlang_free(entry->key);
            mlang_free(entry);
            table->entry_count--;
            return 0;
        }

        prev = entry;
        entry = entry->next;
    }

    return -1;
}

/* 动态数组实现 */
mlang_dynamic_array_t *mlang_array_create(size_t element_size) {
    mlang_dynamic_array_t *array = mlang_malloc(sizeof(mlang_dynamic_array_t));
    if (!array) return NULL;

    array->data = NULL;
    array->count = 0;
    array->capacity = 0;
    array->element_size = element_size;

    return array;
}

/* 销毁动态数组 */
void mlang_array_destroy(mlang_dynamic_array_t *array) {
    if (!array) return;

    if (array->data) {
        mlang_free(array->data);
    }
    mlang_free(array);
}

/* 添加元素到数组 */
int mlang_array_append(mlang_dynamic_array_t *array, void *element) {
    if (!array || !element) return -1;

    /* 扩展容量 */
    if (array->count >= array->capacity) {
        uint32_t new_capacity = array->capacity == 0 ? 8 : array->capacity * 2;
        void *new_data = mlang_realloc(array->data,
                                      new_capacity * array->element_size);
        if (!new_data) return -1;

        array->data = new_data;
        array->capacity = new_capacity;
    }

    /* 添加元素 */
    memcpy((char *)array->data + array->count * array->element_size,
           element, array->element_size);
    array->count++;

    return 0;
}

/* 获取数组元素 */
void *mlang_array_get(mlang_dynamic_array_t *array, uint32_t index) {
    if (!array || index >= array->count) {
        return NULL;
    }

    return (char *)array->data + index * array->element_size;
}

/* 清空数组 */
void mlang_array_clear(mlang_dynamic_array_t *array) {
    if (!array) return;

    array->count = 0;
}

/* 获取数组大小 */
uint32_t mlang_array_size(mlang_dynamic_array_t *array) {
    return array ? array->count : 0;
}

/* 获取版本信息 */
void mlang_get_version(int *major, int *minor, int *patch) {
    if (major) *major = MLANG_VERSION_MAJOR;
    if (minor) *minor = MLANG_VERSION_MINOR;
    if (patch) *patch = MLANG_VERSION_PATCH;
}

/* 获取版本字符串 */
const char *mlang_get_version_string(void) {
    static char version_str[16];
    snprintf(version_str, sizeof(version_str), "%d.%d.%d",
             MLANG_VERSION_MAJOR, MLANG_VERSION_MINOR, MLANG_VERSION_PATCH);
    return version_str;
}

/* 获取编译器信息 */
void mlang_get_info(mlang_info_t *info) {
    if (!info) return;

    strncpy(info->name, "MLang", sizeof(info->name));
    strncpy(info->version, mlang_get_version_string(), sizeof(info->version));
    strncpy(info->target, "M4KK1", sizeof(info->target));
    info->features = 0xFFFFFFFF; /* 所有特性 */
    info->compile_time = time(NULL);
}

/* 获取统计信息 */
void mlang_get_stats(mlang_stats_t *stats) {
    if (!stats) return;

    memcpy(stats, &compiler_stats, sizeof(mlang_stats_t));
}

/* 重置统计信息 */
void mlang_reset_stats(void) {
    memset(&compiler_stats, 0, sizeof(compiler_stats));
}

/* 打印统计信息 */
void mlang_print_stats(void) {
    printf("MLang Compiler Statistics:\n");
    printf("  Total compilations: %llu\n", compiler_stats.total_compilations);
    printf("  Successful compilations: %llu\n", compiler_stats.successful_compilations);
    printf("  Failed compilations: %llu\n", compiler_stats.failed_compilations);
    printf("  Total lines compiled: %llu\n", compiler_stats.total_lines_compiled);
    printf("  Total optimization time: %llu ms\n", compiler_stats.total_optimization_time);
}
/**
 * M4KK1 Dynamic Linker Implementation
 * 动态链接器实现
 *
 * 实现自定义的M4KK1动态链接库格式(.m4ll)支持：
 * - 动态库文件格式定义和解析
 * - 符号表管理（全局符号表和局部符号表）
 * - 动态库加载和卸载机制
 * - 符号解析和重定位处理
 * - 依赖关系管理
 * - 内存映射和权限管理
 * - 与进程管理系统的集成
 */

#include "ldso.h"
#include "process.h"
#include "memory.h"
#include "kernel.h"
#include "console.h"
#include <string.h>
#include <stdint.h>

/* 全局错误状态 */
int m4ll_errno = M4LL_ERROR_NONE;
char m4ll_error_msg[256] = {0};

/* 全局上下文 */
static m4ll_context_t ldso_context;

/* 字符串哈希函数 */
uint32_t m4ll_hash_string(const char *str) {
    uint32_t hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

/* 字符串比较函数 */
int m4ll_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

/* 内存复制函数 */
void *m4ll_memcpy(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;

    while (n--) {
        *d++ = *s++;
    }

    return dest;
}

/* 内存设置函数 */
void *m4ll_memset(void *s, int c, size_t n) {
    unsigned char *p = s;

    while (n--) {
        *p++ = (unsigned char)c;
    }

    return s;
}

/* 设置错误信息 */
static void set_error(int error, const char *msg) {
    m4ll_errno = error;
    strncpy(m4ll_error_msg, msg, sizeof(m4ll_error_msg) - 1);
    m4ll_error_msg[sizeof(m4ll_error_msg) - 1] = '\0';
    KLOG_ERROR("LDSO Error");
}

/* 验证文件头 */
static int validate_header(m4ll_header_t *header) {
    if (header->magic != M4LL_MAGIC) {
        set_error(M4LL_ERROR_INVALID_FORMAT, "Invalid magic number");
        return -1;
    }

    if (header->version != 1) {
        set_error(M4LL_ERROR_INVALID_FORMAT, "Unsupported version");
        return -1;
    }

    /* 验证校验和 */
    uint32_t checksum = header->checksum;
    header->checksum = 0;
    uint32_t computed = 0; /* 简化的校验和计算 */

    for (int i = 0; i < sizeof(m4ll_header_t) / 4; i++) {
        computed += ((uint32_t*)header)[i];
    }

    header->checksum = checksum;

    if (computed != checksum) {
        set_error(M4LL_ERROR_INVALID_FORMAT, "Header checksum mismatch");
        return -1;
    }

    return 0;
}

/* 读取文件内容到内存 */
static void *read_file_to_memory(const char *filename, size_t *size) {
    /* 这里需要实现文件读取逻辑 */
    /* 暂时返回NULL，表示功能未完全实现 */
    set_error(M4LL_ERROR_FILE_NOT_FOUND, "File I/O not implemented");
    return NULL;
}

/* 分配库结构 */
static m4ll_library_t *alloc_library(void) {
    m4ll_library_t *lib = (m4ll_library_t *)kmalloc(sizeof(m4ll_library_t));
    if (!lib) {
        set_error(M4LL_ERROR_MEMORY_FAILED, "Failed to allocate library structure");
        return NULL;
    }

    memset(lib, 0, sizeof(m4ll_library_t));
    lib->status = M4LL_STATUS_UNLOADED;
    lib->ref_count = 1;

    return lib;
}

/* 释放库结构 */
static void free_library(m4ll_library_t *lib) {
    if (!lib) return;

    if (lib->name) kfree(lib->name);
    if (lib->header) kfree(lib->header);
    if (lib->symtab) kfree(lib->symtab);
    if (lib->strtab) kfree(lib->strtab);
    if (lib->reltab) kfree(lib->reltab);
    if (lib->deptab) kfree(lib->deptab);

    kfree(lib);
}

/* 解析文件头 */
static int parse_header(m4ll_library_t *lib, void *file_data) {
    m4ll_header_t *header = (m4ll_header_t *)file_data;

    if (validate_header(header) < 0) {
        return -1;
    }

    /* 复制文件头 */
    lib->header = (m4ll_header_t *)kmalloc(sizeof(m4ll_header_t));
    if (!lib->header) {
        set_error(M4LL_ERROR_MEMORY_FAILED, "Failed to allocate header");
        return -1;
    }

    memcpy(lib->header, header, sizeof(m4ll_header_t));

    return 0;
}

/* 解析符号表 */
static int parse_symbol_table(m4ll_library_t *lib, void *file_data) {
    m4ll_header_t *header = lib->header;

    if (header->symtab_count == 0) {
        return 0; /* 没有符号表 */
    }

    /* 分配符号表 */
    lib->symtab = (m4ll_sym_t *)kmalloc(header->symtab_count * sizeof(m4ll_sym_t));
    if (!lib->symtab) {
        set_error(M4LL_ERROR_MEMORY_FAILED, "Failed to allocate symbol table");
        return -1;
    }

    /* 复制符号表 */
    void *symtab_src = (uint8_t *)file_data + header->symtab_offset;
    memcpy(lib->symtab, symtab_src, header->symtab_count * sizeof(m4ll_sym_t));

    return 0;
}

/* 解析字符串表 */
static int parse_string_table(m4ll_library_t *lib, void *file_data) {
    m4ll_header_t *header = lib->header;

    if (header->strtab_size == 0) {
        return 0; /* 没有字符串表 */
    }

    /* 分配字符串表 */
    lib->strtab = (char *)kmalloc(header->strtab_size);
    if (!lib->strtab) {
        set_error(M4LL_ERROR_MEMORY_FAILED, "Failed to allocate string table");
        return -1;
    }

    /* 复制字符串表 */
    void *strtab_src = (uint8_t *)file_data + header->strtab_offset;
    memcpy(lib->strtab, strtab_src, header->strtab_size);

    return 0;
}

/* 解析重定位表 */
static int parse_relocation_table(m4ll_library_t *lib, void *file_data) {
    m4ll_header_t *header = lib->header;

    if (header->rel_count == 0) {
        return 0; /* 没有重定位表 */
    }

    /* 分配重定位表 */
    lib->reltab = (m4ll_rel_t *)kmalloc(header->rel_count * sizeof(m4ll_rel_t));
    if (!lib->reltab) {
        set_error(M4LL_ERROR_MEMORY_FAILED, "Failed to allocate relocation table");
        return -1;
    }

    /* 复制重定位表 */
    void *reltab_src = (uint8_t *)file_data + header->rel_offset;
    memcpy(lib->reltab, reltab_src, header->rel_count * sizeof(m4ll_rel_t));

    return 0;
}

/* 解析依赖表 */
static int parse_dependency_table(m4ll_library_t *lib, void *file_data) {
    m4ll_header_t *header = lib->header;

    if (header->dep_count == 0) {
        return 0; /* 没有依赖 */
    }

    /* 分配依赖表 */
    lib->deptab = (m4ll_dep_t *)kmalloc(header->dep_count * sizeof(m4ll_dep_t));
    if (!lib->deptab) {
        set_error(M4LL_ERROR_MEMORY_FAILED, "Failed to allocate dependency table");
        return -1;
    }

    /* 复制依赖表 */
    void *deptab_src = (uint8_t *)file_data + header->dep_offset;
    memcpy(lib->deptab, deptab_src, header->dep_count * sizeof(m4ll_dep_t));

    return 0;
}

/* 加载库到内存 */
static int load_library_data(m4ll_library_t *lib, void *file_data) {
    m4ll_header_t *header = lib->header;

    /* 分配基地址 */
    lib->base_addr = (void *)ldso_context.base_address;
    ldso_context.base_address += 0x100000; /* 增加4MB */

    /* 解析各个表 */
    if (parse_symbol_table(lib, file_data) < 0) return -1;
    if (parse_string_table(lib, file_data) < 0) return -1;
    if (parse_relocation_table(lib, file_data) < 0) return -1;
    if (parse_dependency_table(lib, file_data) < 0) return -1;

    return 0;
}

/* 添加全局符号 */
static int add_global_symbol(const char *name, void *address, uint32_t type, uint32_t binding) {
    m4ll_symbol_t *symbol = (m4ll_symbol_t *)kmalloc(sizeof(m4ll_symbol_t));
    if (!symbol) {
        set_error(M4LL_ERROR_MEMORY_FAILED, "Failed to allocate symbol structure");
        return -1;
    }

    symbol->name = strdup(name); /* 需要实现strdup */
    if (!symbol->name) {
        kfree(symbol);
        set_error(M4LL_ERROR_MEMORY_FAILED, "Failed to duplicate symbol name");
        return -1;
    }

    symbol->address = address;
    symbol->size = 0; /* 暂时设为0 */
    symbol->type = type;
    symbol->binding = binding;
    symbol->library = NULL; /* 暂时设为NULL */
    symbol->next = ldso_context.global_symbols;

    ldso_context.global_symbols = symbol;

    return 0;
}

/* 查找全局符号 */
void *m4ll_find_symbol(const char *name) {
    m4ll_symbol_t *symbol = ldso_context.global_symbols;

    while (symbol) {
        if (m4ll_strcmp(symbol->name, name) == 0) {
            return symbol->address;
        }
        symbol = symbol->next;
    }

    return NULL;
}

/* 添加符号到全局符号表 */
int m4ll_add_symbol(const char *name, void *address, uint32_t type, uint32_t binding) {
    return add_global_symbol(name, address, type, binding);
}

/* 执行重定位 */
static int perform_relocations(m4ll_library_t *lib) {
    m4ll_header_t *header = lib->header;
    uint32_t i;

    for (i = 0; i < header->rel_count; i++) {
        m4ll_rel_t *rel = &lib->reltab[i];
        m4ll_sym_t *sym = &lib->symtab[rel->sym_index];
        char *sym_name = &lib->strtab[sym->name_offset];

        /* 查找符号地址 */
        void *sym_addr = m4ll_find_symbol(sym_name);
        if (!sym_addr && sym->binding != M4LL_SYMBOL_WEAK) {
            set_error(M4LL_ERROR_SYMBOL_NOT_FOUND, "Symbol not found");
            return -1;
        }

        /* 计算重定位地址 */
        uint32_t *rel_addr = (uint32_t *)((uint8_t *)lib->base_addr + rel->offset);

        /* 执行重定位 */
        switch (rel->info & 0xFF) { /* 重定位类型 */
            case M4LL_RELOCATION_32:
                *rel_addr = (uint32_t)sym_addr + rel->addend;
                break;

            case M4LL_RELOCATION_PC32:
                *rel_addr = (uint32_t)sym_addr + rel->addend - (uint32_t)rel_addr;
                break;

            default:
                set_error(M4LL_ERROR_RELOCATION_FAILED, "Unknown relocation type");
                return -1;
        }
    }

    return 0;
}

/* 加载依赖库 */
static int load_dependencies(m4ll_library_t *lib) {
    m4ll_header_t *header = lib->header;
    uint32_t i;

    for (i = 0; i < header->dep_count; i++) {
        m4ll_dep_t *dep = &lib->deptab[i];
        char *dep_name = &lib->strtab[dep->name_offset];

        /* 递归加载依赖库 */
        m4ll_library_t *dep_lib;
        if (m4ll_load_library(dep_name, &dep_lib) < 0) {
            set_error(M4LL_ERROR_DEPENDENCY_FAILED, "Failed to load dependency");
            return -1;
        }

        /* 添加到依赖链表 */
        dep_lib->next = lib->deps;
        lib->deps = dep_lib;
    }

    return 0;
}

/* 加载动态库 */
int m4ll_load_library(const char *filename, m4ll_library_t **lib) {
    void *file_data;
    size_t file_size;
    m4ll_library_t *library;

    KLOG_INFO("Loading dynamic library");

    /* 读取文件 */
    file_data = read_file_to_memory(filename, &file_size);
    if (!file_data) {
        return -1;
    }

    /* 分配库结构 */
    library = alloc_library();
    if (!library) {
        kfree(file_data);
        return -1;
    }

    /* 设置库名称 */
    library->name = strdup(filename); /* 需要实现strdup */
    if (!library->name) {
        free_library(library);
        kfree(file_data);
        set_error(M4LL_ERROR_MEMORY_FAILED, "Failed to duplicate library name");
        return -1;
    }

    /* 解析文件头 */
    if (parse_header(library, file_data) < 0) {
        free_library(library);
        kfree(file_data);
        return -1;
    }

    /* 加载库数据 */
    if (load_library_data(library, file_data) < 0) {
        free_library(library);
        kfree(file_data);
        return -1;
    }

    /* 加载依赖 */
    if (load_dependencies(library) < 0) {
        free_library(library);
        kfree(file_data);
        return -1;
    }

    /* 执行重定位 */
    if (perform_relocations(library) < 0) {
        free_library(library);
        kfree(file_data);
        return -1;
    }

    /* 更新状态 */
    library->status = M4LL_STATUS_LOADED;

    /* 添加到已加载库链表 */
    library->next = ldso_context.loaded_libs;
    ldso_context.loaded_libs = library;

    *lib = library;

    kfree(file_data);

    KLOG_INFO("Library loaded successfully");

    return 0;
}

/* 卸载动态库 */
int m4ll_unload_library(m4ll_library_t *lib) {
    m4ll_library_t *prev, *curr;

    if (!lib) return 0;

    /* 减少引用计数 */
    lib->ref_count--;
    if (lib->ref_count > 0) {
        return 0; /* 还有其他引用 */
    }

    /* 从已加载库链表中移除 */
    prev = NULL;
    curr = ldso_context.loaded_libs;

    while (curr) {
        if (curr == lib) {
            if (prev) {
                prev->next = curr->next;
            } else {
                ldso_context.loaded_libs = curr->next;
            }
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    /* 释放库资源 */
    free_library(lib);

    KLOG_INFO("Library unloaded");

    return 0;
}

/* 初始化动态链接器 */
int m4ll_init(void) {
    KLOG_INFO("Initializing dynamic linker...");

    /* 初始化全局上下文 */
    memset(&ldso_context, 0, sizeof(m4ll_context_t));
    ldso_context.base_address = 0xD0000000; /* 库加载基地址 */

    /* 清空错误状态 */
    m4ll_errno = M4LL_ERROR_NONE;
    m4ll_error_msg[0] = '\0';

    KLOG_INFO("Dynamic linker initialized");

    return 0;
}

/* 清理动态链接器 */
void m4ll_cleanup(void) {
    m4ll_library_t *lib, *next;

    KLOG_INFO("Cleaning up dynamic linker...");

    /* 卸载所有库 */
    lib = ldso_context.loaded_libs;
    while (lib) {
        next = lib->next;
        m4ll_unload_library(lib);
        lib = next;
    }

    /* 清空全局符号表 */
    m4ll_symbol_t *symbol, *next_symbol;

    symbol = ldso_context.global_symbols;
    while (symbol) {
        next_symbol = symbol->next;
        if (symbol->name) kfree(symbol->name);
        kfree(symbol);
        symbol = next_symbol;
    }

    KLOG_INFO("Dynamic linker cleanup completed");
}

/* 内存分配函数 */
void *m4ll_allocate_memory(size_t size, uint32_t flags) {
    return kmalloc(size);
}

/* 内存释放函数 */
void m4ll_free_memory(void *ptr) {
    kfree(ptr);
}

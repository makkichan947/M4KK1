/**
 * M4KK1 Dynamic Linker Header
 * 动态链接器头文件定义
 */

#ifndef __LDSO_H__
#define __LDSO_H__

#include <stdint.h>
#include <stddef.h>

/**
 * M4KK1动态链接库文件格式(.m4ll)定义
 */

/* 文件魔数 */
#define M4LL_MAGIC 0x4D344C4C  /* "M4LL" */

/* 段类型 */
#define M4LL_SEGMENT_CODE   1  /* 代码段 */
#define M4LL_SEGMENT_DATA   2  /* 数据段 */
#define M4LL_SEGMENT_BSS    3  /* BSS段 */
#define M4LL_SEGMENT_RODATA 4  /* 只读数据段 */

/* 符号类型 */
#define M4LL_SYMBOL_LOCAL   0  /* 局部符号 */
#define M4LL_SYMBOL_GLOBAL  1  /* 全局符号 */
#define M4LL_SYMBOL_WEAK    2  /* 弱符号 */

/* 符号绑定类型 */
#define M4LL_SYMBOL_FUNCTION 0  /* 函数符号 */
#define M4LL_SYMBOL_OBJECT   1  /* 对象符号 */

/* 重定位类型 */
#define M4LL_RELOCATION_32   1  /* 32位绝对地址重定位 */
#define M4LL_RELOCATION_PC32 2  /* 32位PC相对重定位 */
#define M4LL_RELOCATION_GOT32 3 /* 32位GOT重定位 */
#define M4LL_RELOCATION_PLT32 4 /* 32位PLT重定位 */

/* 库状态 */
#define M4LL_STATUS_UNLOADED  0  /* 未加载 */
#define M4LL_STATUS_LOADING   1  /* 加载中 */
#define M4LL_STATUS_LOADED    2  /* 已加载 */
#define M4LL_STATUS_RELOCATED 3  /* 已重定位 */
#define M4LL_STATUS_ERROR     4  /* 错误状态 */

/**
 * 文件头结构
 */
typedef struct {
    uint32_t magic;           /* 文件魔数 */
    uint32_t version;         /* 格式版本 */
    uint32_t flags;           /* 标志位 */
    uint32_t entry_point;     /* 入口点偏移 */
    uint32_t phdr_offset;     /* 程序头表偏移 */
    uint32_t phdr_count;      /* 程序头数量 */
    uint32_t shdr_offset;     /* 段表偏移 */
    uint32_t shdr_count;      /* 段数量 */
    uint32_t strtab_offset;   /* 字符串表偏移 */
    uint32_t strtab_size;     /* 字符串表大小 */
    uint32_t symtab_offset;   /* 符号表偏移 */
    uint32_t symtab_count;    /* 符号数量 */
    uint32_t rel_offset;      /* 重定位表偏移 */
    uint32_t rel_count;       /* 重定位数量 */
    uint32_t dep_offset;      /* 依赖表偏移 */
    uint32_t dep_count;       /* 依赖数量 */
    uint32_t checksum;        /* 校验和 */
} m4ll_header_t;

/**
 * 程序头结构
 */
typedef struct {
    uint32_t type;      /* 段类型 */
    uint32_t offset;    /* 在文件中的偏移 */
    uint32_t vaddr;     /* 虚拟地址 */
    uint32_t paddr;     /* 物理地址 */
    uint32_t file_size; /* 文件大小 */
    uint32_t mem_size;  /* 内存大小 */
    uint32_t flags;     /* 段标志 */
    uint32_t align;     /* 对齐要求 */
} m4ll_phdr_t;

/**
 * 段表结构
 */
typedef struct {
    uint32_t name_offset;   /* 段名在字符串表中的偏移 */
    uint32_t type;          /* 段类型 */
    uint32_t flags;         /* 段标志 */
    uint32_t addr;          /* 虚拟地址 */
    uint32_t offset;        /* 文件偏移 */
    uint32_t size;          /* 段大小 */
    uint32_t link;          /* 链接到其他段 */
    uint32_t info;          /* 附加信息 */
    uint32_t align;         /* 对齐要求 */
    uint32_t entry_size;    /* 表项大小 */
} m4ll_shdr_t;

/**
 * 符号表项结构
 */
typedef struct {
    uint32_t name_offset;   /* 符号名在字符串表中的偏移 */
    uint32_t value;         /* 符号值 */
    uint32_t size;          /* 符号大小 */
    uint8_t  type;          /* 符号类型 */
    uint8_t  binding;       /* 符号绑定 */
    uint8_t  visibility;    /* 可见性 */
    uint8_t  section;       /* 所在段 */
} m4ll_sym_t;

/**
 * 重定位表项结构
 */
typedef struct {
    uint32_t offset;    /* 重定位偏移 */
    uint32_t info;      /* 重定位信息 */
    uint32_t sym_index; /* 符号表索引 */
    int32_t  addend;    /* 加数 */
} m4ll_rel_t;

/**
 * 依赖关系结构
 */
typedef struct {
    uint32_t name_offset;   /* 依赖库名在字符串表中的偏移 */
    uint32_t version;       /* 版本要求 */
    uint32_t flags;         /* 依赖标志 */
} m4ll_dep_t;

/**
 * 加载的库信息结构
 */
typedef struct m4ll_library {
    char *name;                 /* 库名称 */
    void *base_addr;           /* 基地址 */
    uint32_t status;           /* 库状态 */
    m4ll_header_t *header;     /* 文件头 */
    m4ll_sym_t *symtab;        /* 符号表 */
    char *strtab;              /* 字符串表 */
    m4ll_rel_t *reltab;        /* 重定位表 */
    m4ll_dep_t *deptab;        /* 依赖表 */
    uint32_t ref_count;        /* 引用计数 */
    struct m4ll_library *next; /* 下一个库 */
    struct m4ll_library *deps; /* 依赖库链表 */
} m4ll_library_t;

/**
 * 全局符号表项
 */
typedef struct m4ll_symbol {
    char *name;                 /* 符号名 */
    void *address;             /* 符号地址 */
    uint32_t size;             /* 符号大小 */
    uint32_t type;             /* 符号类型 */
    uint32_t binding;          /* 符号绑定 */
    m4ll_library_t *library;   /* 所属库 */
    struct m4ll_symbol *next;  /* 下一个符号 */
} m4ll_symbol_t;

/**
 * 全局状态结构
 */
typedef struct {
    m4ll_library_t *loaded_libs;   /* 已加载库链表 */
    m4ll_symbol_t *global_symbols; /* 全局符号表 */
    uint32_t base_address;         /* 库加载基地址 */
    uint32_t flags;                /* 全局标志 */
} m4ll_context_t;

/**
 * 函数声明
 */

/* 库加载和卸载 */
int m4ll_load_library(const char *filename, m4ll_library_t **lib);
int m4ll_unload_library(m4ll_library_t *lib);

/* 符号解析 */
void *m4ll_find_symbol(const char *name);
int m4ll_add_symbol(const char *name, void *address, uint32_t type, uint32_t binding);

/* 重定位处理 */
int m4ll_perform_relocations(m4ll_library_t *lib);

/* 依赖关系管理 */
int m4ll_resolve_dependencies(m4ll_library_t *lib);

/* 内存管理 */
void *m4ll_allocate_memory(size_t size, uint32_t flags);
void m4ll_free_memory(void *ptr);

/* 初始化和清理 */
int m4ll_init(void);
void m4ll_cleanup(void);

/* 工具函数 */
uint32_t m4ll_hash_string(const char *str);
int m4ll_strcmp(const char *s1, const char *s2);
void *m4ll_memcpy(void *dest, const void *src, size_t n);
void *m4ll_memset(void *s, int c, size_t n);

/* 错误处理 */
extern int m4ll_errno;
extern char m4ll_error_msg[256];

#define M4LL_ERROR_NONE         0
#define M4LL_ERROR_FILE_NOT_FOUND 1
#define M4LL_ERROR_INVALID_FORMAT 2
#define M4LL_ERROR_LOAD_FAILED    3
#define M4LL_ERROR_SYMBOL_NOT_FOUND 4
#define M4LL_ERROR_RELOCATION_FAILED 5
#define M4LL_ERROR_DEPENDENCY_FAILED 6
#define M4LL_ERROR_MEMORY_FAILED  7

#endif /* __LDSO_H__ */
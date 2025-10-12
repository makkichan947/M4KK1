/**
 * LangCC - Language Compiler Collection 主头文件
 * 定义编译器的核心数据结构和接口
 */

#ifndef _LANGCC_H
#define _LANGCC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * LangCC魔数
 */
#define LANGCC_MAGIC 0x4C434300    /* "LCC\0" */

/**
 * LangCC版本
 */
#define LANGCC_VERSION_MAJOR 0
#define LANGCC_VERSION_MINOR 1
#define LANGCC_VERSION_PATCH 0

/**
 * 语言类型
 */
#define LANGCC_LANG_C         0
#define LANGCC_LANG_CPP       1
#define LANGCC_LANG_OBJC      2
#define LANGCC_LANG_ASM       3
#define LANGCC_LANG_SHELL     4
#define LANGCC_LANG_CUSTOM    5

/**
 * 输出类型
 */
#define LANGCC_OUTPUT_EXEC    0     /* 可执行文件 */
#define LANGCC_OUTPUT_OBJECT  1     /* 目标文件 */
#define LANGCC_OUTPUT_SHARED  2     /* 共享库 */
#define LANGCC_OUTPUT_STATIC  3     /* 静态库 */
#define LANGCC_OUTPUT_ASM     4     /* 汇编代码 */
#define LANGCC_OUTPUT_IR      5     /* 中间表示 */

/**
 * 优化级别
 */
#define LANGCC_OPT_NONE       0     /* 无优化 */
#define LANGCC_OPT_BASIC      1     /* 基本优化 */
#define LANGCC_OPT_STANDARD   2     /* 标准优化 */
#define LANGCC_OPT_AGGRESSIVE 3     /* 激进优化 */
#define LANGCC_OPT_SIZE       4     /* 代码大小优化 */
#define LANGCC_OPT_SPEED      5     /* 速度优化 */

/**
 * 目标架构
 */
#define LANGCC_ARCH_M4KK1     0     /* M4KK1原生 */
#define LANGCC_ARCH_I386      1     /* i386 */
#define LANGCC_ARCH_X86_64    2     /* x86_64 */
#define LANGCC_ARCH_ARM       3     /* ARM */
#define LANGCC_ARCH_RISCV     4     /* RISC-V */

/**
 * 编译选项结构
 */
typedef struct {
    uint32_t language;           /* 源语言 */
    uint32_t output_type;        /* 输出类型 */
    uint32_t optimization;       /* 优化级别 */
    uint32_t architecture;       /* 目标架构 */
    uint32_t word_size;          /* 字大小 */
    bool pic;                    /* 位置无关代码 */
    bool debug;                  /* 调试信息 */
    bool warnings;               /* 警告信息 */
    bool strip;                  /* 剥离符号 */
    bool verbose;                /* 详细输出 */
    char *output_file;           /* 输出文件 */
    char **include_paths;        /* 包含路径 */
    uint32_t include_count;      /* 包含路径数量 */
    char **library_paths;        /* 库路径 */
    uint32_t library_count;      /* 库路径数量 */
    char **libraries;            /* 链接库 */
    uint32_t library_count;      /* 库数量 */
    char **defines;              /* 预定义宏 */
    uint32_t define_count;       /* 宏数量 */
    char *target;                /* 目标三元组 */
    uint32_t max_errors;         /* 最大错误数 */
} langcc_options_t;

/**
 * 编译器驱动结构
 */
typedef struct {
    uint32_t magic;              /* 魔数 */
    uint32_t version;            /* 版本 */
    langcc_options_t options;    /* 编译选项 */
    void *frontend;              /* 前端实例 */
    void *optimizer;             /* 优化器实例 */
    void *codegen;               /* 代码生成器 */
    void *linker;                /* 链接器实例 */
    char *input_file;            /* 输入文件 */
    char *output_file;           /* 输出文件 */
    uint32_t error_count;        /* 错误计数 */
    uint32_t warning_count;      /* 警告计数 */
    bool compiled;               /* 编译完成标志 */
} langcc_driver_t;

/**
 * 源文件信息
 */
typedef struct {
    char *filename;              /* 文件名 */
    uint32_t language;           /* 语言类型 */
    uint8_t *content;            /* 文件内容 */
    uint32_t size;               /* 文件大小 */
    uint32_t line_count;         /* 行数 */
    uint32_t *line_offsets;      /* 行偏移表 */
} langcc_source_t;

/**
 * 编译单元信息
 */
typedef struct {
    langcc_source_t *sources;    /* 源文件列表 */
    uint32_t source_count;       /* 源文件数量 */
    char **dependencies;         /* 依赖文件 */
    uint32_t dependency_count;   /* 依赖数量 */
    void *ast;                   /* 抽象语法树 */
    void *symbols;               /* 符号表 */
    void *ir;                    /* 中间表示 */
} langcc_unit_t;

/**
 * 目标信息
 */
typedef struct {
    uint32_t architecture;       /* 架构类型 */
    uint32_t word_size;          /* 字大小 */
    uint32_t pointer_size;       /* 指针大小 */
    bool little_endian;          /* 小端序 */
    char *name;                  /* 架构名称 */
    char *description;           /* 描述 */
    uint32_t features;           /* 特性位图 */
} langcc_target_t;

/**
 * 函数声明
 */

/* 驱动程序管理 */
int langcc_init(langcc_driver_t *driver);
int langcc_compile(langcc_driver_t *driver, const char *input_file);
int langcc_link(langcc_driver_t *driver);
int langcc_cleanup(langcc_driver_t *driver);

/* 选项管理 */
int langcc_parse_options(langcc_options_t *options, int argc, char *argv[]);
int langcc_set_default_options(langcc_options_t *options);
int langcc_validate_options(langcc_options_t *options);

/* 源文件处理 */
int langcc_load_source(const char *filename, langcc_source_t *source);
int langcc_free_source(langcc_source_t *source);
int langcc_preprocess_source(langcc_source_t *source, langcc_options_t *options);

/* 前端接口 */
int langcc_frontend_init(uint32_t language, void **frontend);
int langcc_frontend_parse(void *frontend, langcc_source_t *source, void **ast);
int langcc_frontend_analyze(void *frontend, void *ast, void **symbols);
int langcc_frontend_cleanup(void *frontend);

/* 优化器接口 */
int langcc_optimizer_init(uint32_t level, void **optimizer);
int langcc_optimizer_optimize(void *optimizer, void *ir, void **optimized_ir);
int langcc_optimizer_cleanup(void *optimizer);

/* 代码生成器接口 */
int langcc_codegen_init(langcc_target_t *target, void **codegen);
int langcc_codegen_generate(void *codegen, void *ir, void **object);
int langcc_codegen_cleanup(void *codegen);

/* 链接器接口 */
int langcc_linker_init(langcc_options_t *options, void **linker);
int langcc_linker_add_object(void *linker, const char *object_file);
int langcc_linker_link(void *linker, const char *output_file);
int langcc_linker_cleanup(void *linker);

/* 目标管理 */
int langcc_get_target_info(const char *target_triple, langcc_target_t *target);
int langcc_register_target(const char *name, langcc_target_t *target);
langcc_target_t *langcc_get_registered_targets(uint32_t *count);

/* 错误处理 */
int langcc_report_error(langcc_driver_t *driver, const char *file,
                       uint32_t line, uint32_t column, const char *message);
int langcc_report_warning(langcc_driver_t *driver, const char *file,
                         uint32_t line, uint32_t column, const char *message);

/* 调试支持 */
int langcc_enable_debug(langcc_driver_t *driver, uint32_t debug_level);
int langcc_disable_debug(langcc_driver_t *driver);
int langcc_dump_ast(void *ast, const char *filename);
int langcc_dump_ir(void *ir, const char *filename);
int langcc_dump_symbols(void *symbols, const char *filename);

/* 统计信息 */
typedef struct {
    uint64_t compile_time_ms;    /* 编译时间(毫秒) */
    uint32_t source_lines;       /* 源代码行数 */
    uint32_t ast_nodes;          /* AST节点数 */
    uint32_t ir_instructions;    /* IR指令数 */
    uint32_t object_size;        /* 目标文件大小 */
    uint32_t optimizations;      /* 优化次数 */
    uint32_t warnings;           /* 警告数量 */
    uint32_t errors;             /* 错误数量 */
} langcc_stats_t;

int langcc_get_stats(langcc_driver_t *driver, langcc_stats_t *stats);
int langcc_reset_stats(langcc_driver_t *driver);

/* 工具函数 */
uint32_t langcc_get_version(void);
const char *langcc_get_version_string(void);
const char *langcc_get_language_name(uint32_t language);
const char *langcc_get_target_name(uint32_t architecture);
bool langcc_is_supported_language(uint32_t language);
bool langcc_is_supported_target(uint32_t architecture);

/* 内存管理 */
void *langcc_malloc(size_t size);
void langcc_free(void *ptr);
void *langcc_realloc(void *ptr, size_t size);
char *langcc_strdup(const char *str);

/* 字符串处理 */
int langcc_strcmp(const char *s1, const char *s2);
size_t langcc_strlen(const char *str);
char *langcc_strcpy(char *dest, const char *src);
char *langcc_strcat(char *dest, const char *src);

/* 路径处理 */
char *langcc_basename(const char *path);
char *langcc_dirname(const char *path);
char *langcc_path_join(const char *dir, const char *file);
bool langcc_path_exists(const char *path);

/* 文件操作 */
int langcc_file_read(const char *filename, uint8_t **buffer, uint32_t *size);
int langcc_file_write(const char *filename, const uint8_t *buffer, uint32_t size);
int langcc_file_exists(const char *filename);

/* 哈希表和列表 */
typedef struct langcc_hash_entry {
    char *key;
    void *value;
    struct langcc_hash_entry *next;
} langcc_hash_entry_t;

typedef struct {
    langcc_hash_entry_t **buckets;
    uint32_t bucket_count;
    uint32_t entry_count;
} langcc_hash_t;

typedef struct langcc_list_node {
    void *data;
    struct langcc_list_node *next;
    struct langcc_list_node *prev;
} langcc_list_node_t;

typedef struct {
    langcc_list_node_t *head;
    langcc_list_node_t *tail;
    uint32_t count;
} langcc_list_t;

/* 容器操作 */
langcc_hash_t *langcc_hash_create(uint32_t bucket_count);
void langcc_hash_destroy(langcc_hash_t *hash);
int langcc_hash_insert(langcc_hash_t *hash, const char *key, void *value);
void *langcc_hash_lookup(langcc_hash_t *hash, const char *key);
int langcc_hash_remove(langcc_hash_t *hash, const char *key);

langcc_list_t *langcc_list_create(void);
void langcc_list_destroy(langcc_list_t *list);
int langcc_list_append(langcc_list_t *list, void *data);
int langcc_list_prepend(langcc_list_t *list, void *data);
void *langcc_list_remove(langcc_list_t *list, void *data);
void *langcc_list_get(langcc_list_t *list, uint32_t index);

/* 版本信息宏 */
#define LANGCC_VERSION \
    (LANGCC_VERSION_MAJOR << 16 | LANGCC_VERSION_MINOR << 8 | LANGCC_VERSION_PATCH)

#define LANGCC_VERSION_STRING \
    "LangCC " \
    STRINGIFY(LANGCC_VERSION_MAJOR) "." \
    STRINGIFY(LANGCC_VERSION_MINOR) "." \
    STRINGIFY(LANGCC_VERSION_PATCH)

/* 字符串化宏 */
#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

#endif /* _LANGCC_H */
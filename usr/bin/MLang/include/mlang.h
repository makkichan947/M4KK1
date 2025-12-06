/**
 * M4KK1 MLang Compiler - Header Definitions
 * MLang独特编译器头文件定义
 */

#ifndef _M4K_MLANG_H
#define _M4K_MLANG_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* MLang编译器版本 */
#define MLANG_VERSION_MAJOR 1
#define MLANG_VERSION_MINOR 0
#define MLANG_VERSION_PATCH 0

/* MLang独特编译阶段 */
#define MLANG_STAGE_LEXICAL     1   /* 词法分析 */
#define MLANG_STAGE_SYNTAX      2   /* 语法分析 */
#define MLANG_STAGE_SEMANTIC    3   /* 语义分析 */
#define MLANG_STAGE_IR          4   /* 中间表示生成 */
#define MLANG_STAGE_OPTIMIZE    5   /* 优化 */
#define MLANG_STAGE_CODEGEN     6   /* 代码生成 */
#define MLANG_STAGE_ASSEMBLE    7   /* 汇编 */
#define MLANG_STAGE_LINK        8   /* 链接 */

/* Token类型定义 */
#define TOKEN_KEYWORD           1
#define TOKEN_IDENTIFIER        2
#define TOKEN_INTEGER_LITERAL   3
#define TOKEN_FLOAT_LITERAL     4
#define TOKEN_STRING_LITERAL    5
#define TOKEN_OPERATOR          6
#define TOKEN_PUNCTUATION       7

/* AST节点类型定义 */
#define AST_ROOT                100
#define AST_FUNCTION            101
#define AST_VARIABLE_DECLARATION 102
#define AST_BINARY_OPERATION    103
#define AST_FUNCTION_CALL       104
#define AST_VARIABLE_REFERENCE  105

/* MLang独特优化级别 */
#define MLANG_OPT_NONE          0   /* 无优化 */
#define MLANG_OPT_BASIC         1   /* 基本优化 */
#define MLANG_OPT_ADVANCED      2   /* 高级优化 */
#define MLANG_OPT_AGGRESSIVE    3   /* 激进优化 */
#define MLANG_OPT_EXPERIMENTAL  4   /* 实验性优化 */

/* MLang独特的目标架构 */
#define MLANG_ARCH_M4K_X86_64   "m4k-x86_64"    /* M4KK1 x86_64 */
#define MLANG_ARCH_M4K_ARM64    "m4k-arm64"     /* M4KK1 ARM64 */
#define MLANG_ARCH_M4K_RISCV    "m4k-riscv"     /* M4KK1 RISC-V */
#define MLANG_ARCH_STANDARD     "standard"      /* 标准架构 */

/* MLang独特的数据类型 */
typedef struct {
    char name[64];
    uint32_t size;
    uint32_t alignment;
    bool is_signed;
    bool is_floating;
} mlang_type_t;

/* MLang独特的AST节点 */
typedef struct mlang_ast_node {
    uint32_t type;              /* 节点类型 */
    char *value;                /* 节点值 */
    struct mlang_ast_node *parent;
    struct mlang_ast_node **children;
    uint32_t child_count;
    uint32_t line;
    uint32_t column;
    void *data;                 /* 额外数据 */
} mlang_ast_node_t;

/* MLang独特的中間表示 */
typedef struct {
    char *function_name;
    uint32_t basic_blocks;
    void *ir_code;
    uint32_t ir_size;
    mlang_type_t return_type;
} mlang_ir_function_t;

/* MLang独特编译器配置 */
typedef struct {
    char *input_file;
    char *output_file;
    char *target_arch;
    int opt_level;
    bool debug_info;
    bool verbose;
    uint32_t max_stage;
    char **include_paths;
    int include_count;
    char **library_paths;
    int library_count;
    char **libraries;
    int lib_count;
    bool enable_experimental;
    bool enable_profiling;
} mlang_config_t;

/* MLang独特编译结果 */
typedef struct {
    bool success;
    char *output_file;
    char *error_message;
    uint32_t warnings;
    uint32_t errors;
    uint32_t compile_time_ms;
    uint32_t code_size;
    uint32_t optimization_time_ms;
} mlang_result_t;

/* MLang独特编译器统计 */
typedef struct {
    uint64_t total_compilations;
    uint64_t successful_compilations;
    uint64_t failed_compilations;
    uint64_t total_lines_compiled;
    uint64_t total_optimization_time;
    uint32_t average_stage_time[MLANG_STAGE_LINK + 1];
} mlang_stats_t;

/* API函数声明 */
int mlang_init(void);
int mlang_compile(mlang_config_t *config, mlang_result_t *result);
int mlang_compile_file(const char *input_file, const char *output_file);
void mlang_cleanup(void);

/* 词法分析 */
int mlang_lexical_analyze(const char *source, mlang_ast_node_t **tokens);
void mlang_free_tokens(mlang_ast_node_t *tokens);

/* 语法分析 */
int mlang_syntax_analyze(mlang_ast_node_t *tokens, mlang_ast_node_t **ast);
void mlang_free_ast(mlang_ast_node_t *ast);

/* 语义分析 */
int mlang_semantic_analyze(mlang_ast_node_t *ast);
int mlang_type_check(mlang_ast_node_t *ast);

/* 中间表示生成 */
int mlang_generate_ir(mlang_ast_node_t *ast, mlang_ir_function_t **ir_functions);
void mlang_free_ir(mlang_ir_function_t *ir_functions);

/* 独特优化 */
int mlang_optimize_ir(mlang_ir_function_t *ir_functions, int opt_level);
int mlang_dead_code_elimination(mlang_ir_function_t *functions);
int mlang_constant_propagation(mlang_ir_function_t *functions);
int mlang_loop_optimization(mlang_ir_function_t *functions);
int mlang_register_allocation(mlang_ir_function_t *functions);

/* 独特代码生成 */
int mlang_generate_code(mlang_ir_function_t *ir_functions, const char *target_arch,
                       uint8_t **code, uint32_t *code_size);
int mlang_generate_assembly(mlang_ir_function_t *functions, const char *target_arch,
                           char **assembly);
int mlang_assemble_code(const char *assembly, const char *target_arch,
                       uint8_t **code, uint32_t *code_size);

/* 独特的目标架构支持 */
int mlang_register_target(const char *arch_name, void *target_info);
int mlang_set_target(const char *arch_name);
const char *mlang_get_target(void);

/* 独特调试信息 */
int mlang_generate_debug_info(mlang_ast_node_t *ast, const char *output_file);
int mlang_generate_dwarf_info(mlang_ir_function_t *functions, const char *output_file);

/* 独特性能分析 */
int mlang_profile_code(const char *source_file, const char *profile_output);
int mlang_analyze_hotspots(const char *profile_data, char **hotspot_report);

/* 独特错误处理 */
void mlang_set_error_handler(void (*handler)(const char *message, int line, int column));
void mlang_error(const char *file, int line, int column, const char *format, ...);
void mlang_warning(const char *file, int line, int column, const char *format, ...);

/* 独特内存管理 */
void *mlang_malloc(size_t size);
void mlang_free(void *ptr);
void *mlang_realloc(void *ptr, size_t size);
char *mlang_strdup(const char *str);

/* 独特字符串处理 */
int mlang_strcmp(const char *s1, const char *s2);
int mlang_strcasecmp(const char *s1, const char *s2);
char *mlang_strcat(char *dest, const char *src);
char *mlang_strcpy(char *dest, const char *src);
size_t mlang_strlen(const char *str);

/* 独特文件处理 */
char *mlang_read_file(const char *filename, size_t *size);
int mlang_write_file(const char *filename, const char *data, size_t size);
int mlang_file_exists(const char *filename);

/* 独特路径处理 */
char *mlang_get_dirname(const char *path);
char *mlang_get_basename(const char *path);
char *mlang_get_extension(const char *path);
char *mlang_join_path(const char *dir, const char *file);

/* 独特哈希表 */
typedef struct mlang_hash_entry {
    char *key;
    void *value;
    struct mlang_hash_entry *next;
} mlang_hash_entry_t;

typedef struct {
    mlang_hash_entry_t **buckets;
    uint32_t bucket_count;
    uint32_t entry_count;
} mlang_hash_table_t;

mlang_hash_table_t *mlang_hash_create(uint32_t bucket_count);
void mlang_hash_destroy(mlang_hash_table_t *table);
int mlang_hash_insert(mlang_hash_table_t *table, const char *key, void *value);
void *mlang_hash_lookup(mlang_hash_table_t *table, const char *key);
int mlang_hash_remove(mlang_hash_table_t *table, const char *key);

/* 独特动态数组 */
typedef struct {
    void **data;
    uint32_t count;
    uint32_t capacity;
    size_t element_size;
} mlang_dynamic_array_t;

mlang_dynamic_array_t *mlang_array_create(size_t element_size);
void mlang_array_destroy(mlang_dynamic_array_t *array);
int mlang_array_append(mlang_dynamic_array_t *array, void *element);
void *mlang_array_get(mlang_dynamic_array_t *array, uint32_t index);
void mlang_array_clear(mlang_dynamic_array_t *array);
uint32_t mlang_array_size(mlang_dynamic_array_t *array);

/* 独特配置管理 */
mlang_config_t *mlang_config_create(void);
void mlang_config_destroy(mlang_config_t *config);
int mlang_config_parse(int argc, char *argv[], mlang_config_t *config);
void mlang_config_print(mlang_config_t *config);

/* 独特版本信息 */
void mlang_get_version(int *major, int *minor, int *patch);
const char *mlang_get_version_string(void);

/* 独特编译器信息 */
typedef struct {
    char name[32];
    char version[32];
    char target[32];
    uint32_t features;
    uint64_t compile_time;
} mlang_info_t;

void mlang_get_info(mlang_info_t *info);

/* 独特统计信息 */
void mlang_get_stats(mlang_stats_t *stats);
void mlang_reset_stats(void);
void mlang_print_stats(void);

/* 独特实验性功能 */
int mlang_enable_experimental(const char *feature);
int mlang_disable_experimental(const char *feature);
bool mlang_is_experimental_enabled(const char *feature);

/* 独特并行编译 */
int mlang_compile_parallel(mlang_config_t *configs[], int config_count,
                          mlang_result_t results[]);

/* 独特增量编译 */
int mlang_incremental_compile(const char *source_file, const char *output_file);
int mlang_supports_incremental(const char *source_file);

/* 独特交叉编译 */
int mlang_cross_compile(const char *source_file, const char *output_file,
                       const char *target_arch, const char *cross_prefix);

/* 独特代码分析 */
int mlang_analyze_complexity(const char *source_file, uint32_t *complexity_score);
int mlang_detect_code_smells(const char *source_file, char **smell_report);
int mlang_suggest_optimizations(const char *source_file, char **optimization_report);

/* 独特文档生成 */
int mlang_generate_docs(const char *source_file, const char *output_dir);
int mlang_extract_comments(const char *source_file, char **documentation);

/* 独特单元测试生成 */
int mlang_generate_unit_tests(const char *source_file, const char *output_file);
int mlang_analyze_test_coverage(const char *source_file, double *coverage_percent);

#endif /* _M4K_MLANG_H */
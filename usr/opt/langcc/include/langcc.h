/**
 * M4KK1 LangCC Compiler Collection - Header Definitions
 * LangCC编译器集合头文件定义
 */

#ifndef _M4K_LANGCC_H
#define _M4K_LANGCC_H

#include <stdint.h>
#include <stdbool.h>

/* 编译器版本 */
#define LANGCC_VERSION_MAJOR 0
#define LANGCC_VERSION_MINOR 1
#define LANGCC_VERSION_PATCH 0

/* 支持的语言 */
#define LANG_C      1
#define LANG_CPP    2
#define LANG_ASSEMBLY 3
#define LANG_FORTH  4

/* 支持的目标架构 */
#define ARCH_X86_64 "x86_64"
#define ARCH_X86    "x86"
#define ARCH_ARM64  "arm64"
#define ARCH_RISCV  "riscv"

/* 优化级别 */
#define OPT_NONE    0
#define OPT_SIZE    1
#define OPT_SPEED   2
#define OPT_AGGRESSIVE 3

/* 编译阶段 */
#define STAGE_PREPROCESS  1
#define STAGE_PARSE       2
#define STAGE_ANALYZE     3
#define STAGE_OPTIMIZE    4
#define STAGE_GENERATE    5
#define STAGE_ASSEMBLE    6
#define STAGE_LINK        7

/* 编译器选项结构 */
typedef struct {
    char *input_file;
    char *output_file;
    char *target_arch;
    int language;
    int opt_level;
    bool debug_info;
    bool verbose;
    int max_stage;
    char **include_paths;
    int include_count;
    char **library_paths;
    int library_count;
    char **libraries;
    int lib_count;
} langcc_options_t;

/* 编译结果结构 */
typedef struct {
    bool success;
    char *output_file;
    char *error_message;
    int exit_code;
    uint32_t compile_time_ms;
} langcc_result_t;

/* API函数声明 */
int langcc_init(void);
int langcc_compile(langcc_options_t *opts, langcc_result_t *result);
int langcc_compile_file(const char *input_file, const char *output_file,
                       const char *target_arch, int opt_level, bool debug);
int langcc_preprocess(const char *input_file, const char *output_file);
int langcc_assemble(const char *input_file, const char *output_file);
int langcc_link(const char *output_file, char **object_files, int obj_count);

/* 目标架构管理 */
int langcc_set_target(const char *arch);
const char *langcc_get_target(void);
int langcc_get_arch_info(const char *arch, uint32_t *bits, bool *is_little_endian);

/* 错误处理 */
void langcc_set_error_handler(void (*handler)(const char *message));
void langcc_error(const char *format, ...);
void langcc_warning(const char *format, ...);

/* 内存管理 */
void *langcc_malloc(size_t size);
void langcc_free(void *ptr);
void *langcc_realloc(void *ptr, size_t size);

/* 字符串处理 */
char *langcc_strdup(const char *str);
char *langcc_strndup(const char *str, size_t n);
int langcc_strcmp(const char *s1, const char *s2);
int langcc_strcasecmp(const char *s1, const char *s2);

/* 文件处理 */
char *langcc_read_file(const char *filename, size_t *size);
int langcc_write_file(const char *filename, const char *data, size_t size);
int langcc_file_exists(const char *filename);

/* 路径处理 */
char *langcc_get_dirname(const char *path);
char *langcc_get_basename(const char *path);
char *langcc_get_extension(const char *path);
char *langcc_join_path(const char *dir, const char *file);

/* 版本信息 */
void langcc_get_version(int *major, int *minor, int *patch);
const char *langcc_get_version_string(void);

/* 编译器信息 */
typedef struct {
    char name[32];
    char version[32];
    char target[32];
    uint32_t features;
} langcc_info_t;

void langcc_get_info(langcc_info_t *info);

#endif /* _M4K_LANGCC_H */
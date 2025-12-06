/**
 * M4KK1 LangCC Compiler Collection - Main Compiler
 * LangCC编译器集合主程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/stat.h>

#include "langcc.h"

/* 编译器版本 */
#define LANGCC_VERSION "0.1.0"

/* 编译阶段 */
#define STAGE_PREPROCESS  1
#define STAGE_COMPILE     2
#define STAGE_ASSEMBLE    3
#define STAGE_LINK        4

/* 目标架构 */
#define ARCH_X86_64       "x86_64"
#define ARCH_X86          "x86"
#define ARCH_ARM64        "arm64"
#define ARCH_RISCV        "riscv"

/* 编译器选项 */
typedef struct {
    char *input_file;
    char *output_file;
    char *target_arch;
    int optimization_level;
    int debug_info;
    int verbose;
    int stage;              /* 编译到哪个阶段 */
    char **include_paths;
    int include_count;
    char **library_paths;
    int library_count;
    char **libraries;
    int lib_count;
} compiler_options_t;

/* 显示帮助信息 */
void show_help(const char *program_name) {
    printf("M4KK1 LangCC Compiler Collection v%s\n", LANGCC_VERSION);
    printf("用法: %s [选项] 输入文件\n", program_name);
    printf("\n");
    printf("选项:\n");
    printf("  -o, --output <file>     输出文件\n");
    printf("  -t, --target <arch>     目标架构 (x86_64, x86, arm64, riscv)\n");
    printf("  -O <level>              优化级别 (0-3)\n");
    printf("  -g                      生成调试信息\n");
    printf("  -I <path>               包含路径\n");
    printf("  -L <path>               库路径\n");
    printf("  -l <lib>                链接库\n");
    printf("  -S                      只编译到汇编\n");
    printf("  -c                      只编译到目标文件\n");
    printf("  -E                      只预处理\n");
    printf("  -v, --verbose           详细输出\n");
    printf("  -V, --version           显示版本\n");
    printf("  -h, --help              显示此帮助\n");
    printf("\n");
    printf("示例:\n");
    printf("  %s -o hello hello.c\n", program_name);
    printf("  %s -O2 -g hello.c -o hello\n", program_name);
    printf("  %s -S hello.c\n", program_name);
}

/* 显示版本信息 */
void show_version(void) {
    printf("M4KK1 LangCC Compiler Collection v%s\n", LANGCC_VERSION);
    printf("Target: M4KK1 Operating System\n");
    printf("Supported architectures: x86_64, x86, arm64, riscv\n");
}

/* 解析命令行参数 */
int parse_arguments(int argc, char *argv[], compiler_options_t *opts) {
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"output", required_argument, 0, 'o'},
        {"target", required_argument, 0, 't'},
        {"verbose", no_argument, 0, 'v'},
        {"version", no_argument, 0, 'V'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    /* 初始化选项 */
    memset(opts, 0, sizeof(compiler_options_t));
    opts->target_arch = ARCH_X86_64;
    opts->optimization_level = 0;
    opts->stage = STAGE_LINK;

    while ((opt = getopt_long(argc, argv, "o:t:O:gcEISvVh",
                             long_options, &option_index)) != -1) {
        switch (opt) {
            case 'o':
                opts->output_file = optarg;
                break;
            case 't':
                opts->target_arch = optarg;
                break;
            case 'O':
                opts->optimization_level = atoi(optarg);
                break;
            case 'g':
                opts->debug_info = 1;
                break;
            case 'c':
                opts->stage = STAGE_COMPILE;
                break;
            case 'S':
                opts->stage = STAGE_ASSEMBLE;
                break;
            case 'E':
                opts->stage = STAGE_PREPROCESS;
                break;
            case 'I':
                /* 简化处理，实际应该动态分配 */
                break;
            case 'L':
                /* 简化处理 */
                break;
            case 'l':
                /* 简化处理 */
                break;
            case 'v':
                opts->verbose = 1;
                break;
            case 'V':
                show_version();
                return 0;
            case 'h':
                show_help(argv[0]);
                return 0;
            default:
                return -1;
        }
    }

    /* 获取输入文件 */
    if (optind < argc) {
        opts->input_file = argv[optind];
    } else {
        printf("错误: 必须指定输入文件\n");
        return -1;
    }

    /* 生成输出文件名（如果未指定） */
    if (!opts->output_file) {
        const char *ext = strrchr(opts->input_file, '.');
        if (ext) {
            size_t len = ext - opts->input_file;
            opts->output_file = malloc(len + 8); /* .o 或 扩展名 */
            if (!opts->output_file) {
                printf("错误: 内存分配失败\n");
                return -1;
            }
            strncpy(opts->output_file, opts->input_file, len);

            switch (opts->stage) {
                case STAGE_PREPROCESS:
                    strcpy(opts->output_file + len, ".i");
                    break;
                case STAGE_ASSEMBLE:
                    strcpy(opts->output_file + len, ".s");
                    break;
                case STAGE_COMPILE:
                    strcpy(opts->output_file + len, ".o");
                    break;
                default:
                    strcpy(opts->output_file + len, ".out");
                    break;
            }
        }
    }

    return 0;
}

/* 检查文件是否存在 */
int file_exists(const char *filename) {
    return access(filename, F_OK) == 0;
}

/* 获取文件扩展名 */
const char *get_file_extension(const char *filename) {
    const char *ext = strrchr(filename, '.');
    return ext ? ext + 1 : "";
}

/* 编译C文件 */
int compile_c_file(compiler_options_t *opts) {
    char command[1024];

    if (opts->verbose) {
        printf("Compiling: %s -> %s\n", opts->input_file, opts->output_file);
    }

    /* 构建编译命令 */
    snprintf(command, sizeof(command),
             "gcc -Wall -Wextra -O%d %s -ffreestanding -nostdlib -m64 "
             "-I../../../sys/src/include -c %s -o %s",
             opts->optimization_level,
             opts->debug_info ? "-g" : "",
             opts->input_file, opts->output_file);

    if (opts->verbose) {
        printf("Command: %s\n", command);
    }

    return system(command);
}

/* 预处理 */
int preprocess_file(compiler_options_t *opts) {
    char command[1024];

    if (opts->verbose) {
        printf("Preprocessing: %s -> %s\n", opts->input_file, opts->output_file);
    }

    snprintf(command, sizeof(command), "gcc -E %s -o %s",
             opts->input_file, opts->output_file);

    return system(command);
}

/* 编译到汇编 */
int compile_to_assembly(compiler_options_t *opts) {
    char command[1024];

    if (opts->verbose) {
        printf("Compiling to assembly: %s -> %s\n", opts->input_file, opts->output_file);
    }

    snprintf(command, sizeof(command),
             "gcc -Wall -Wextra -O%d %s -ffreestanding -nostdlib -m64 "
             "-I../../../sys/src/include -S %s -o %s",
             opts->optimization_level,
             opts->debug_info ? "-g" : "",
             opts->input_file, opts->output_file);

    return system(command);
}

/* 链接目标文件 */
int link_object_files(compiler_options_t *opts) {
    char command[1024];

    if (opts->verbose) {
        printf("Linking: %s -> %s\n", opts->input_file, opts->output_file);
    }

    snprintf(command, sizeof(command),
             "ld -T../../../sys/src/init/linker.ld %s -o %s",
             opts->input_file, opts->output_file);

    return system(command);
}

/* 主编译流程 */
int compile_file(compiler_options_t *opts) {
    const char *ext = get_file_extension(opts->input_file);

    /* 检查输入文件是否存在 */
    if (!file_exists(opts->input_file)) {
        printf("错误: 输入文件不存在: %s\n", opts->input_file);
        return 1;
    }

    /* 根据文件类型和阶段进行编译 */
    if (strcmp(ext, "c") == 0 || strcmp(ext, "C") == 0) {
        switch (opts->stage) {
            case STAGE_PREPROCESS:
                return preprocess_file(opts);
            case STAGE_ASSEMBLE:
                return compile_to_assembly(opts);
            case STAGE_COMPILE:
                return compile_c_file(opts);
            case STAGE_LINK:
                if (compile_c_file(opts) == 0) {
                    return link_object_files(opts);
                }
                return 1;
            default:
                printf("错误: 未知编译阶段\n");
                return 1;
        }
    } else {
        printf("错误: 不支持的文件类型: %s\n", ext);
        return 1;
    }
}

/* 主函数 */
int main(int argc, char *argv[]) {
    compiler_options_t opts;
    int result;

    /* 解析命令行参数 */
    if (parse_arguments(argc, argv, &opts) != 0) {
        return 1;
    }

    /* 编译文件 */
    result = compile_file(&opts);

    /* 清理资源 */
    if (opts.output_file && opts.output_file != argv[argc-1]) {
        free(opts.output_file);
    }

    return result;
}
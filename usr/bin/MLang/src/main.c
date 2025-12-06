/**
 * M4KK1 MLang Compiler - Main Program
 * MLang独特编译器主程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <time.h>

#include "mlang.h"

/* 编译器版本 */
#define MLANG_VERSION "1.0.0"

/* 显示帮助信息 */
void show_help(const char *program_name) {
    printf("M4KK1 MLang Compiler v%s\n", MLANG_VERSION);
    printf("用法: %s [选项] 输入文件\n", program_name);
    printf("\n");
    printf("选项:\n");
    printf("  -o, --output <file>     输出文件\n");
    printf("  -t, --target <arch>     目标架构 (m4k-x86_64, m4k-arm64, m4k-riscv)\n");
    printf("  -O <level>              优化级别 (0-4)\n");
    printf("  -g                      生成调试信息\n");
    printf("  -S                      只编译到汇编\n");
    printf("  -c                      只编译到目标文件\n");
    printf("  -E                      只预处理\n");
    printf("  -v, --verbose           详细输出\n");
    printf("  -V, --version           显示版本\n");
    printf("  -h, --help              显示此帮助\n");
    printf("\n");
    printf("优化级别:\n");
    printf("  -O0                     无优化\n");
    printf("  -O1                     基本优化\n");
    printf("  -O2                     高级优化\n");
    printf("  -O3                     激进优化\n");
    printf("  -O4                     实验性优化\n");
    printf("\n");
    printf("示例:\n");
    printf("  %s -o hello.m4k hello.mlang\n", program_name);
    printf("  %s -O3 -g hello.mlang -o hello.m4k\n", program_name);
    printf("  %s -S hello.mlang\n", program_name);
}

/* 显示版本信息 */
void show_version(void) {
    printf("M4KK1 MLang Compiler v%s\n", MLANG_VERSION);
    printf("Unique compiler with advanced optimization techniques\n");
    printf("Copyright (C) 2025 M4KK1 Development Team\n");
    printf("License: GPL-3.0\n");
}

/* 解析命令行参数 */
int parse_arguments(int argc, char *argv[], mlang_config_t *config) {
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

    /* 初始化配置 */
    memset(config, 0, sizeof(mlang_config_t));
    config->target_arch = mlang_strdup(MLANG_ARCH_M4K_X86_64);
    config->opt_level = MLANG_OPT_ADVANCED;
    config->debug_info = false;
    config->verbose = false;
    config->max_stage = MLANG_STAGE_LINK;

    while ((opt = getopt_long(argc, argv, "o:t:O:gcEISvVh",
                             long_options, &option_index)) != -1) {
        switch (opt) {
            case 'o':
                config->output_file = mlang_strdup(optarg);
                break;
            case 't':
                mlang_free(config->target_arch);
                config->target_arch = mlang_strdup(optarg);
                break;
            case 'O':
                config->opt_level = atoi(optarg);
                if (config->opt_level < 0) config->opt_level = 0;
                if (config->opt_level > 4) config->opt_level = 4;
                break;
            case 'g':
                config->debug_info = true;
                break;
            case 'c':
                config->max_stage = MLANG_STAGE_CODEGEN;
                break;
            case 'S':
                config->max_stage = MLANG_STAGE_ASSEMBLE;
                break;
            case 'E':
                config->max_stage = MLANG_STAGE_IR;
                break;
            case 'v':
                config->verbose = true;
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
        config->input_file = mlang_strdup(argv[optind]);
    } else {
        printf("错误: 必须指定输入文件\n");
        return -1;
    }

    /* 生成输出文件名（如果未指定） */
    if (!config->output_file) {
        const char *input_ext = mlang_get_extension(config->input_file);
        size_t base_len = strlen(config->input_file) - strlen(input_ext) - 1;

        config->output_file = mlang_malloc(base_len + 8);
        if (!config->output_file) {
            printf("错误: 内存分配失败\n");
            return -1;
        }

        strncpy(config->output_file, config->input_file, base_len);
        config->output_file[base_len] = '\0';

        switch (config->max_stage) {
            case MLANG_STAGE_IR:
                strcat(config->output_file, ".ir");
                break;
            case MLANG_STAGE_ASSEMBLE:
                strcat(config->output_file, ".s");
                break;
            case MLANG_STAGE_CODEGEN:
                strcat(config->output_file, ".o");
                break;
            default:
                strcat(config->output_file, ".m4k");
                break;
        }
    }

    return 0;
}

/* 主函数 */
int main(int argc, char *argv[]) {
    mlang_config_t config;
    mlang_result_t result;
    int ret;

    /* 解析命令行参数 */
    if (parse_arguments(argc, argv, &config) != 0) {
        return 1;
    }

    /* 初始化编译器 */
    if (mlang_init() != 0) {
        printf("错误: 无法初始化MLang编译器\n");
        return 1;
    }

    /* 编译文件 */
    clock_t start_time = clock();
    ret = mlang_compile(&config, &result);
    clock_t end_time = clock();

    /* 计算编译时间 */
    double compile_time = (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000.0;

    /* 显示结果 */
    if (ret == 0 && result.success) {
        printf("编译成功: %s -> %s\n", config.input_file, config.output_file);
        printf("编译时间: %.2f ms\n", compile_time);
        printf("代码大小: %u bytes\n", result.code_size);

        if (config.verbose) {
            printf("警告: %u\n", result.warnings);
            printf("错误: %u\n", result.errors);
        }
    } else {
        printf("编译失败: %s\n", result.error_message ? result.error_message : "Unknown error");
        ret = 1;
    }

    /* 清理资源 */
    if (config.input_file) mlang_free(config.input_file);
    if (config.output_file) mlang_free(config.output_file);
    if (config.target_arch) mlang_free(config.target_arch);

    mlang_cleanup();

    return ret;
}
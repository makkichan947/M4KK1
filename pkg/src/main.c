/**
 * M4KK1 Package Manager - Main Program
 * 包管理器主程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "package.h"

/* 程序版本 */
#define PKGMGR_VERSION "1.0.0"

/* 命令行选项 */
static struct option long_options[] = {
    {"install", required_argument, 0, 'i'},
    {"remove", required_argument, 0, 'r'},
    {"update", required_argument, 0, 'u'},
    {"info", required_argument, 0, 'I'},
    {"list", no_argument, 0, 'l'},
    {"search", required_argument, 0, 's'},
    {"force", no_argument, 0, 'f'},
    {"yes", no_argument, 0, 'y'},
    {"version", no_argument, 0, 'v'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};

/* 显示帮助信息 */
void show_help(const char *program_name) {
    printf("M4KK1 Package Manager v%s\n", PKGMGR_VERSION);
    printf("用法: %s [选项] [包名/文件]\n", program_name);
    printf("\n");
    printf("选项:\n");
    printf("  -i, --install <file>    安装包文件\n");
    printf("  -r, --remove <package>  移除包\n");
    printf("  -u, --update <package>  更新包\n");
    printf("  -I, --info <package>    显示包信息\n");
    printf("  -l, --list             列出已安装的包\n");
    printf("  -s, --search <pattern>  搜索包\n");
    printf("  -f, --force            强制操作\n");
    printf("  -y, --yes              自动确认\n");
    printf("  -v, --version          显示版本\n");
    printf("  -h, --help             显示此帮助\n");
    printf("\n");
    printf("示例:\n");
    printf("  %s --install package.m4pkg\n", program_name);
    printf("  %s --remove vim\n", program_name);
    printf("  %s --list\n", program_name);
    printf("  %s --search editor\n", program_name);
}

/* 显示版本信息 */
void show_version(void) {
    printf("M4KK1 Package Manager v%s\n", PKGMGR_VERSION);
    printf("Copyright (C) 2025 M4KK1 Development Team\n");
    printf("License: GPL-3.0\n");
}

/* 主函数 */
int main(int argc, char *argv[]) {
    int opt;
    int option_index = 0;
    int force = 0;
    int yes = 0;
    const char *package_name = NULL;
    const char *package_file = NULL;
    int action = 0;

    /* 解析命令行参数 */
    while ((opt = getopt_long(argc, argv, "i:r:u:I:ls:fyvh",
                             long_options, &option_index)) != -1) {
        switch (opt) {
            case 'i':
                action = 'i';
                package_file = optarg;
                break;
            case 'r':
                action = 'r';
                package_name = optarg;
                break;
            case 'u':
                action = 'u';
                package_name = optarg;
                break;
            case 'I':
                action = 'I';
                package_name = optarg;
                break;
            case 'l':
                action = 'l';
                break;
            case 's':
                action = 's';
                package_name = optarg;
                break;
            case 'f':
                force = 1;
                break;
            case 'y':
                yes = 1;
                break;
            case 'v':
                show_version();
                return 0;
            case 'h':
                show_help(argv[0]);
                return 0;
            default:
                show_help(argv[0]);
                return 1;
        }
    }

    /* 检查参数 */
    if (action == 0) {
        printf("错误: 必须指定操作\n");
        show_help(argv[0]);
        return 1;
    }

    /* 初始化包管理系统 */
    if (package_init() != 0) {
        printf("错误: 无法初始化包管理系统\n");
        return 1;
    }

    /* 执行操作 */
    switch (action) {
        case 'i':
            if (!package_file) {
                printf("错误: 必须指定包文件\n");
                return 1;
            }
            return package_install(package_file, force);
            break;

        case 'r':
            if (!package_name) {
                printf("错误: 必须指定包名\n");
                return 1;
            }
            return package_remove(package_name, force);
            break;

        case 'u':
            if (!package_name) {
                printf("错误: 必须指定包名\n");
                return 1;
            }
            return package_update(package_name);
            break;

        case 'I':
            if (!package_name) {
                printf("错误: 必须指定包名\n");
                return 1;
            }
            return package_info(package_name);
            break;

        case 'l':
            return package_list();
            break;

        case 's':
            if (!package_name) {
                printf("错误: 必须指定搜索模式\n");
                return 1;
            }
            return package_search(package_name);
            break;

        default:
            printf("错误: 未知操作\n");
            return 1;
    }

    return 0;
}

/* 包管理器命令行工具 */
int main_cli(int argc, char *argv[]) {
    return main(argc, argv);
}
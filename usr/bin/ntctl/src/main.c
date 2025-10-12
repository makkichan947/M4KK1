/*
 * M4KK1 Ntctl网络管理器 - 主程序入口
 * CLI界面的网络管理器
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <locale.h>

#include "ntmgr.h"  /* 复用ntmgr的头文件 */

/* 全局应用状态 */
static app_state_t *global_app = NULL;

/* 信号处理函数 */
void signal_handler(int signo)
{
    if (global_app) {
        switch (signo) {
            case SIGTERM:
            case SIGINT:
                /* 优雅地退出应用 */
                ntmgr_quit(global_app);
                break;
            case SIGPIPE:
                /* 管道破裂，静默退出 */
                exit(0);
                break;
        }
    }
}

/* 设置信号处理 */
void setup_signal_handlers(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sa.sa_flags = SA_RESTART;

    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGPIPE, &sa, NULL);
}

/* 打印帮助信息 */
void print_help(const char *program_name)
{
    printf("M4KK1 Ntctl网络管理器 %s\n", NTMGR_VERSION_STRING);
    printf("用法: %s [选项] <子命令> [参数...]\n", program_name);
    printf("\n全局选项:\n");
    printf("  -h, --help              显示此帮助信息\n");
    printf("  -V, --version           显示版本信息\n");
    printf("  -v, --verbose           详细输出\n");
    printf("  -q, --quiet             安静模式\n");
    printf("  -c, --config <文件>     指定配置文件\n");
    printf("  -f, --format <格式>     输出格式 (text/json/xml)\n");
    printf("  -t, --timeout <秒数>     操作超时时间\n");
    printf("\n子命令:\n");
    printf("  interface <子命令>      网络接口管理\n");
    printf("  vpn <子命令>           VPN连接管理\n");
    printf("  proxy <子命令>         代理服务器管理\n");
    printf("  firewall <子命令>      防火墙管理\n");
    printf("  monitor <子命令>       网络监控\n");
    printf("  diagnose <参数>        网络诊断\n");
    printf("\n示例:\n");
    printf("  %s interface list                    列出网络接口\n", program_name);
    printf("  %s vpn connect office                连接VPN\n", program_name);
    printf("  %s monitor traffic -f json           监控流量（JSON格式）\n", program_name);
    printf("  %s diagnose 8.8.8.8                  诊断网络连接\n", program_name);
}

/* 打印版本信息 */
void print_version(void)
{
    printf("M4KK1 Ntctl网络管理器 %s\n", NTMGR_VERSION_STRING);
    printf("命令行网络管理工具\n");
    printf("专为M4KK1操作系统优化\n");
}

/* 解析全局选项 */
typedef struct {
    char *config_file;      /* 配置文件 */
    char *format;           /* 输出格式 */
    int timeout;            /* 超时时间 */
    bool verbose;           /* 详细模式 */
    bool quiet;             /* 安静模式 */
    bool help;              /* 帮助标志 */
    bool version;           /* 版本标志 */
} global_options_t;

global_options_t *parse_global_options(int argc, char *argv[])
{
    static global_options_t opts;
    int opt;

    memset(&opts, 0, sizeof(opts));
    opts.timeout = 30; /* 默认超时30秒 */

    /* 命令行选项定义 */
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'V'},
        {"verbose", no_argument, 0, 'v'},
        {"quiet", no_argument, 0, 'q'},
        {"config", required_argument, 0, 'c'},
        {"format", required_argument, 0, 'f'},
        {"timeout", required_argument, 0, 't'},
        {0, 0, 0, 0}
    };

    /* 解析选项 */
    while ((opt = getopt_long(argc, argv, "hVvqcf:t:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                opts.help = true;
                break;
            case 'V':
                opts.version = true;
                break;
            case 'v':
                opts.verbose = true;
                break;
            case 'q':
                opts.quiet = true;
                break;
            case 'c':
                opts.config_file = strdup(optarg);
                break;
            case 'f':
                opts.format = strdup(optarg);
                break;
            case 't':
                opts.timeout = atoi(optarg);
                break;
            case '?':
                /* 未知选项，getopt_long会打印错误信息 */
                exit(1);
                break;
        }
    }

    return &opts;
}

/* 清理全局选项 */
void cleanup_global_options(global_options_t *opts)
{
    if (opts->config_file) free(opts->config_file);
    if (opts->format) free(opts->format);
}

/* 处理子命令 */
int handle_subcommand(int argc, char *argv[], global_options_t *global_opts)
{
    if (argc <= 0) {
        fprintf(stderr, "错误: 未指定子命令\n");
        return 1;
    }

    char *subcommand = argv[0];

    /* 接口管理子命令 */
    if (strcmp(subcommand, "interface") == 0) {
        return handle_interface_command(argc - 1, argv + 1, global_opts);
    }
    /* VPN管理子命令 */
    else if (strcmp(subcommand, "vpn") == 0) {
        return handle_vpn_command(argc - 1, argv + 1, global_opts);
    }
    /* 代理管理子命令 */
    else if (strcmp(subcommand, "proxy") == 0) {
        return handle_proxy_command(argc - 1, argv + 1, global_opts);
    }
    /* 防火墙管理子命令 */
    else if (strcmp(subcommand, "firewall") == 0) {
        return handle_firewall_command(argc - 1, argv + 1, global_opts);
    }
    /* 监控子命令 */
    else if (strcmp(subcommand, "monitor") == 0) {
        return handle_monitor_command(argc - 1, argv + 1, global_opts);
    }
    /* 诊断子命令 */
    else if (strcmp(subcommand, "diagnose") == 0) {
        return handle_diagnose_command(argc - 1, argv + 1, global_opts);
    }
    else {
        fprintf(stderr, "错误: 未知子命令 '%s'\n", subcommand);
        return 1;
    }
}

/* 接口管理命令处理 */
int handle_interface_command(int argc, char *argv[], global_options_t *global_opts)
{
    if (argc <= 0) {
        fprintf(stderr, "用法: ntctl interface <子命令>\n");
        return 1;
    }

    char *subcmd = argv[0];

    if (strcmp(subcmd, "list") == 0) {
        /* 列出接口 */
        network_interface_t **interfaces = interface_scan();
        if (!interfaces) {
            printf("未找到网络接口\n");
            return 1;
        }

        /* 计算接口数量 */
        size_t count = 0;
        while (interfaces[count]) count++;

        /* 输出接口信息 */
        if (strcmp(global_opts->format, "json") == 0) {
            output_json_interfaces(interfaces, count);
        } else {
            output_text_interfaces(interfaces, count);
        }

        /* 清理资源 */
        for (size_t i = 0; i < count; i++) {
            interface_destroy(interfaces[i]);
        }
        free(interfaces);

        return 0;
    }
    else if (strcmp(subcmd, "show") == 0) {
        if (argc < 2) {
            fprintf(stderr, "用法: ntctl interface show <接口名>\n");
            return 1;
        }

        char *interface_name = argv[1];
        network_interface_t *interface = interface_get_by_name(interface_name);
        if (!interface) {
            fprintf(stderr, "接口 '%s' 不存在\n", interface_name);
            return 1;
        }

        /* 输出接口详细信息 */
        if (strcmp(global_opts->format, "json") == 0) {
            output_json_interface_detail(interface);
        } else {
            output_text_interface_detail(interface);
        }

        interface_destroy(interface);
        return 0;
    }
    else {
        fprintf(stderr, "未知的interface子命令: %s\n", subcmd);
        return 1;
    }
}

/* VPN管理命令处理 */
int handle_vpn_command(int argc, char *argv[], global_options_t *global_opts)
{
    if (argc <= 0) {
        fprintf(stderr, "用法: ntctl vpn <子命令>\n");
        return 1;
    }

    char *subcmd = argv[0];

    if (strcmp(subcmd, "list") == 0) {
        /* 列出VPN连接 */
        printf("VPN连接列表功能待实现\n");
        return 0;
    }
    else if (strcmp(subcmd, "connect") == 0) {
        if (argc < 2) {
            fprintf(stderr, "用法: ntctl vpn connect <连接名>\n");
            return 1;
        }

        char *connection_name = argv[1];
        printf("连接VPN '%s' 功能待实现\n", connection_name);
        return 0;
    }
    else {
        fprintf(stderr, "未知的vpn子命令: %s\n", subcmd);
        return 1;
    }
}

/* 代理管理命令处理 */
int handle_proxy_command(int argc, char *argv[], global_options_t *global_opts)
{
    if (argc <= 0) {
        fprintf(stderr, "用法: ntctl proxy <子命令>\n");
        return 1;
    }

    char *subcmd = argv[0];

    if (strcmp(subcmd, "list") == 0) {
        /* 列出代理服务器 */
        printf("代理服务器列表功能待实现\n");
        return 0;
    }
    else {
        fprintf(stderr, "未知的proxy子命令: %s\n", subcmd);
        return 1;
    }
}

/* 防火墙管理命令处理 */
int handle_firewall_command(int argc, char *argv[], global_options_t *global_opts)
{
    if (argc <= 0) {
        fprintf(stderr, "用法: ntctl firewall <子命令>\n");
        return 1;
    }

    char *subcmd = argv[0];

    if (strcmp(subcmd, "status") == 0) {
        /* 显示防火墙状态 */
        printf("防火墙状态功能待实现\n");
        return 0;
    }
    else {
        fprintf(stderr, "未知的firewall子命令: %s\n", subcmd);
        return 1;
    }
}

/* 监控命令处理 */
int handle_monitor_command(int argc, char *argv[], global_options_t *global_opts)
{
    if (argc <= 0) {
        fprintf(stderr, "用法: ntctl monitor <子命令>\n");
        return 1;
    }

    char *subcmd = argv[0];

    if (strcmp(subcmd, "traffic") == 0) {
        /* 监控流量 */
        printf("流量监控功能待实现\n");
        return 0;
    }
    else {
        fprintf(stderr, "未知的monitor子命令: %s\n", subcmd);
        return 1;
    }
}

/* 诊断命令处理 */
int handle_diagnose_command(int argc, char *argv[], global_options_t *global_opts)
{
    if (argc <= 0) {
        fprintf(stderr, "用法: ntctl diagnose <目标>\n");
        return 1;
    }

    char *target = argv[0];
    printf("诊断网络连接到 %s 功能待实现\n", target);
    return 0;
}

/* 输出格式化函数（简化版） */
void output_text_interfaces(network_interface_t **interfaces, size_t count)
{
    printf("%-10s %-8s %-15s %-10s %s\n",
           "INTERFACE", "TYPE", "IP_ADDRESS", "STATE", "MAC_ADDRESS");
    printf("%-10s %-8s %-15s %-10s %s\n",
           "----------", "--------", "---------------", "----------", "------------");

    for (size_t i = 0; i < count; i++) {
        network_interface_t *iface = interfaces[i];
        printf("%-10s %-8s %-15s %-10s %s\n",
               iface->name,
               interface_type_to_string(iface->type),
               iface->ip_count > 0 ? iface->ip_addresses[0] : "N/A",
               connection_state_to_string(iface->state),
               iface->mac_address);
    }
}

void output_json_interfaces(network_interface_t **interfaces, size_t count)
{
    printf("{\n");
    printf("  \"interfaces\": [\n");

    for (size_t i = 0; i < count; i++) {
        network_interface_t *iface = interfaces[i];

        printf("    {\n");
        printf("      \"name\": \"%s\",\n", iface->name);
        printf("      \"type\": \"%s\",\n", interface_type_to_string(iface->type));
        printf("      \"state\": \"%s\",\n", connection_state_to_string(iface->state));
        printf("      \"mac_address\": \"%s\"\n", iface->mac_address);
        printf("    }");
        if (i < count - 1) printf(",");
        printf("\n");
    }

    printf("  ]\n");
    printf("}\n");
}

void output_text_interface_detail(network_interface_t *interface)
{
    printf("接口详情: %s\n", interface->name);
    printf("类型: %s\n", interface_type_to_string(interface->type));
    printf("状态: %s\n", connection_state_to_string(interface->state));
    printf("MAC地址: %s\n", interface->mac_address);
    printf("MTU: %d\n", interface->mtu);
    printf("接收字节: %lu\n", interface->rx_bytes);
    printf("发送字节: %lu\n", interface->tx_bytes);
}

void output_json_interface_detail(network_interface_t *interface)
{
    printf("{\n");
    printf("  \"name\": \"%s\",\n", interface->name);
    printf("  \"type\": \"%s\",\n", interface_type_to_string(interface->type));
    printf("  \"state\": \"%s\",\n", connection_state_to_string(interface->state));
    printf("  \"mac_address\": \"%s\",\n", interface->mac_address);
    printf("  \"mtu\": %d,\n", interface->mtu);
    printf("  \"rx_bytes\": %lu,\n", interface->rx_bytes);
    printf("  \"tx_bytes\": %lu\n", interface->tx_bytes);
    printf("}\n");
}

/* 主函数 */
int main(int argc, char *argv[])
{
    int exit_code = 0;
    global_options_t *global_opts;

    /* 解析全局选项 */
    global_opts = parse_global_options(argc, argv);

    /* 处理帮助和版本 */
    if (global_opts->help) {
        print_help(argv[0]);
        cleanup_global_options(global_opts);
        return 0;
    }

    if (global_opts->version) {
        print_version();
        cleanup_global_options(global_opts);
        return 0;
    }

    /* 创建应用状态 */
    global_app = ntmgr_create();
    if (!global_app) {
        fprintf(stderr, "无法创建应用状态\n");
        cleanup_global_options(global_opts);
        return 1;
    }

    /* 设置信号处理 */
    setup_signal_handlers();

    /* 初始化应用 */
    if (!ntmgr_init(global_app)) {
        fprintf(stderr, "应用初始化失败\n");
        ntmgr_destroy(global_app);
        cleanup_global_options(global_opts);
        return 1;
    }

    /* 处理子命令 */
    exit_code = handle_subcommand(argc - optind, argv + optind, global_opts);

    /* 清理资源 */
    ntmgr_destroy(global_app);
    cleanup_global_options(global_opts);

    return exit_code;
}
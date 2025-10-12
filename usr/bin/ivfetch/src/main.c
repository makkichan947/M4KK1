sr/bin/ivfetch/src/main.c</path>
<parameter name="content">
/**
 * IVFetch - M4KK1系统信息工具主程序
 * 实现系统信息收集和显示的核心功能
 */

#include "ivfetch.h"
#include "../include/ivfetch.h"
#include "../../y4ku/include/console.h"

/**
 * 全局上下文实例
 */
static ivfetch_context_t global_context;

/**
 * 默认艺术字 - M4KK1标志
 */
const char *default_m4kk1_artwork =
"     .-----.     \n"
"    /       \\    \n"
"   |  M4KK1  |   \n"
"    \\       /    \n"
"     '-----'     \n";

/**
 * 初始化IVFetch上下文
 */
int ivfetch_init(ivfetch_context_t *ctx) {
    if (!ctx) {
        return -1;
    }

    // 清零上下文结构
    ivfetch_memset(ctx, 0, sizeof(ivfetch_context_t));

    // 设置魔数和版本
    ctx->magic = IVFETCH_MAGIC;
    ctx->version = IVFETCH_VERSION;

    // 初始化配置
    ctx->config = ivfetch_malloc(sizeof(ivfetch_config_t));
    if (!ctx->config) {
        return -1;
    }

    ivfetch_memset(ctx->config, 0, sizeof(ivfetch_config_t));

    // 设置默认配置
    ctx->config->layout = ivfetch_malloc(sizeof(ivfetch_layout_t));
    ctx->config->artwork = ivfetch_malloc(sizeof(ivfetch_artwork_t));
    ctx->config->theme = ivfetch_malloc(sizeof(ivfetch_theme_t));

    if (!ctx->config->layout || !ctx->config->artwork || !ctx->config->theme) {
        return -1;
    }

    // 设置默认布局
    ctx->config->layout->type = IVFETCH_LAYOUT_SINGLE;
    ctx->config->layout->columns = 1;
    ctx->config->layout->spacing = 1;
    ctx->config->layout->show_borders = false;
    ctx->config->layout->center_align = false;
    ctx->config->layout->max_width = 80;
    ctx->config->layout->max_height = 25;

    // 设置默认艺术字
    ivfetch_artwork_create_ascii(ctx->config->artwork, default_m4kk1_artwork);
    ctx->config->artwork->color.r = 0;
    ctx->config->artwork->color.g = 255;
    ctx->config->artwork->color.b = 255;
    ctx->config->artwork->color.a = 255;
    ctx->config->artwork->color.bright = false;

    // 创建默认主题
    ivfetch_theme_create_default(ctx->config->theme);

    // 设置默认选项
    ctx->config->cache_enabled = true;
    ctx->config->cache_timeout = 300;  // 5分钟
    ctx->config->verbose = false;
    ctx->config->debug = false;
    ctx->config->color_enabled = true;
    ctx->config->animation_enabled = false;

    // 初始化缓存
    ivfetch_cache_init(ctx);

    // 注册默认模块
    ivfetch_register_default_modules(ctx);

    ctx->initialized = true;

    console_write("IVFetch initialized successfully\n");
    console_write("Version: ");
    console_write(IVFETCH_VERSION_STRING);
    console_write("\n");

    return 0;
}

/**
 * 清理IVFetch上下文
 */
int ivfetch_cleanup(ivfetch_context_t *ctx) {
    if (!ctx) {
        return -1;
    }

    // 释放配置
    if (ctx->config) {
        if (ctx->config->layout) {
            ivfetch_free(ctx->config->layout);
        }
        if (ctx->config->artwork) {
            if (ctx->config->artwork->content) {
                ivfetch_free(ctx->config->artwork->content);
            }
            if (ctx->config->artwork->name) {
                ivfetch_free(ctx->config->artwork->name);
            }
            ivfetch_free(ctx->config->artwork);
        }
        if (ctx->config->theme) {
            if (ctx->config->theme->name) {
                ivfetch_free(ctx->config->theme->name);
            }
            if (ctx->config->theme->description) {
                ivfetch_free(ctx->config->theme->description);
            }
            if (ctx->config->theme->author) {
                ivfetch_free(ctx->config->theme->author);
            }
            if (ctx->config->theme->version) {
                ivfetch_free(ctx->config->theme->version);
            }
            ivfetch_free(ctx->config->theme);
        }
        if (ctx->config->config_file) {
            ivfetch_free(ctx->config->config_file);
        }
        ivfetch_free(ctx->config);
    }

    // 释放模块
    if (ctx->modules) {
        for (uint32_t i = 0; i < ctx->module_count; i++) {
            if (ctx->modules[i].name) {
                ivfetch_free(ctx->modules[i].name);
            }
            if (ctx->modules[i].format) {
                ivfetch_free(ctx->modules[i].format);
            }
            if (ctx->modules[i].data) {
                ivfetch_free(ctx->modules[i].data);
            }
        }
        ivfetch_free(ctx->modules);
    }

    // 释放输出缓冲区
    if (ctx->output_buffer) {
        ivfetch_free(ctx->output_buffer);
    }

    // 清零上下文
    ivfetch_memset(ctx, 0, sizeof(ivfetch_context_t));

    console_write("IVFetch cleaned up successfully\n");
    return 0;
}

/**
 * 主函数
 */
int main(int argc, char *argv[]) {
    int ret = 0;

    console_write("IVFetch - M4KK1 System Information Tool\n");

    // 初始化上下文
    ret = ivfetch_init(&global_context);
    if (ret != 0) {
        console_write("Failed to initialize IVFetch\n");
        return 1;
    }

    // 解析命令行参数
    ret = ivfetch_parse_arguments(&global_context, argc, argv);
    if (ret != 0) {
        console_write("Failed to parse arguments\n");
        ivfetch_cleanup(&global_context);
        return 1;
    }

    // 收集系统信息
    ret = ivfetch_collect_all_info(&global_context);
    if (ret != 0) {
        console_write("Failed to collect system information\n");
        ivfetch_cleanup(&global_context);
        return 1;
    }

    // 显示信息
    ret = ivfetch_display(&global_context);
    if (ret != 0) {
        console_write("Failed to display information\n");
        ivfetch_cleanup(&global_context);
        return 1;
    }

    // 清理资源
    ivfetch_cleanup(&global_context);

    return 0;
}

/**
 * 解析命令行参数
 */
int ivfetch_parse_arguments(ivfetch_context_t *ctx, int argc, char *argv[]) {
    if (!ctx || !argv) {
        return -1;
    }

    // 解析选项
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        if (arg[0] == '-') {
            if (ivfetch_strcmp(arg, "--help") == 0 || ivfetch_strcmp(arg, "-h") == 0) {
                ivfetch_show_help();
                return 1;  // 退出
            } else if (ivfetch_strcmp(arg, "--version") == 0 || ivfetch_strcmp(arg, "-v") == 0) {
                ivfetch_show_version();
                return 1;  // 退出
            } else if (ivfetch_strcmp(arg, "--config") == 0 || ivfetch_strcmp(arg, "-c") == 0) {
                if (i + 1 < argc) {
                    ctx->config->config_file = ivfetch_strdup(argv[++i]);
                }
            } else if (ivfetch_strcmp(arg, "--layout") == 0 || ivfetch_strcmp(arg, "-l") == 0) {
                if (i + 1 < argc) {
                    char *layout_str = argv[++i];
                    if (ivfetch_strcmp(layout_str, "single") == 0) {
                        ctx->config->layout->type = IVFETCH_LAYOUT_SINGLE;
                    } else if (ivfetch_strcmp(layout_str, "double") == 0) {
                        ctx->config->layout->type = IVFETCH_LAYOUT_DOUBLE;
                    } else if (ivfetch_strcmp(layout_str, "triple") == 0) {
                        ctx->config->layout->type = IVFETCH_LAYOUT_TRIPLE;
                    } else if (ivfetch_strcmp(layout_str, "compact") == 0) {
                        ctx->config->layout->type = IVFETCH_LAYOUT_COMPACT;
                    }
                }
            } else if (ivfetch_strcmp(arg, "--theme") == 0 || ivfetch_strcmp(arg, "-t") == 0) {
                if (i + 1 < argc) {
                    // 加载指定主题
                    ivfetch_theme_load(ctx->config->theme, argv[++i]);
                }
            } else if (ivfetch_strcmp(arg, "--no-color") == 0) {
                ctx->config->color_enabled = false;
            } else if (ivfetch_strcmp(arg, "--no-artwork") == 0) {
                ctx->config->artwork->type = IVFETCH_ARTWORK_NONE;
            } else if (ivfetch_strcmp(arg, "--verbose") == 0) {
                ctx->config->verbose = true;
            } else if (ivfetch_strcmp(arg, "--debug") == 0) {
                ctx->config->debug = true;
            }
        }
    }

    return 0;
}

/**
 * 收集所有系统信息
 */
int ivfetch_collect_all_info(ivfetch_context_t *ctx) {
    if (!ctx) {
        return -1;
    }

    // 收集系统信息
    ivfetch_system_info_t system_info;
    ivfetch_memset(&system_info, 0, sizeof(ivfetch_system_info_t));

    int ret = ivfetch_collect_system_info(&system_info);
    if (ret != 0) {
        ivfetch_error("Failed to collect system information");
        return ret;
    }

    // 收集硬件信息
    ivfetch_hardware_info_t hardware_info;
    ivfetch_memset(&hardware_info, 0, sizeof(ivfetch_hardware_info_t));

    ret = ivfetch_collect_hardware_info(&hardware_info);
    if (ret != 0) {
        ivfetch_error("Failed to collect hardware information");
        return ret;
    }

    // 收集网络信息
    ivfetch_network_info_t network_info;
    ivfetch_memset(&network_info, 0, sizeof(ivfetch_network_info_t));

    ret = ivfetch_collect_network_info(&network_info);
    if (ret != 0) {
        ivfetch_error("Failed to collect network information");
        return ret;
    }

    // 收集用户信息
    ivfetch_user_info_t user_info;
    ivfetch_memset(&user_info, 0, sizeof(ivfetch_user_info_t));

    ret = ivfetch_collect_user_info(&user_info);
    if (ret != 0) {
        ivfetch_error("Failed to collect user information");
        return ret;
    }

    // 更新模块数据
    ivfetch_update_module_data(ctx, &system_info, &hardware_info, &network_info, &user_info);

    return 0;
}

/**
 * 显示系统信息
 */
int ivfetch_display(ivfetch_context_t *ctx) {
    if (!ctx) {
        return -1;
    }

    // 根据布局类型选择显示函数
    switch (ctx->config->layout->type) {
        case IVFETCH_LAYOUT_SINGLE:
            return ivfetch_display_single_column(ctx);
        case IVFETCH_LAYOUT_DOUBLE:
            return ivfetch_display_double_column(ctx);
        case IVFETCH_LAYOUT_TRIPLE:
            return ivfetch_display_triple_column(ctx);
        case IVFETCH_LAYOUT_COMPACT:
            return ivfetch_display_compact(ctx);
        default:
            return ivfetch_display_single_column(ctx);
    }
}

/**
 * 单栏显示
 */
int ivfetch_display_single_column(ivfetch_context_t *ctx) {
    if (!ctx) {
        return -1;
    }

    // 显示艺术字
    if (ctx->config->artwork->type != IVFETCH_ARTWORK_NONE) {
        ivfetch_display_artwork(ctx->config->artwork);
        console_write("\n");
    }

    // 显示模块信息
    for (uint32_t i = 0; i < ctx->module_count; i++) {
        ivfetch_module_t *module = &ctx->modules[i];

        if (!module->enabled) {
            continue;
        }

        char buffer[256];
        ivfetch_memset(buffer, 0, sizeof(buffer));

        if (module->format_func) {
            module->format_func(module->data, buffer, sizeof(buffer));
            console_write(buffer);
        }

        console_write("\n");
    }

    return 0;
}

/**
 * 双栏显示
 */
int ivfetch_display_double_column(ivfetch_context_t *ctx) {
    if (!ctx) {
        return -1;
    }

    // 简化的双栏显示
    console_write("┌─────────────────────────────────────┐\n");
    console_write("│  ");
    if (ctx->config->artwork->type != IVFETCH_ARTWORK_NONE) {
        console_write("M4KK1");
    } else {
        console_write("System Info");
    }
    console_write("                  │  User: m4kk1              │\n");
    console_write("│  OS: M4KK1                      │  Shell: m4sh              │\n");
    console_write("│  Kernel: Y4KU 0.1.0             │  Terminal: /dev/tty0      │\n");
    console_write("│  Uptime: 1h 23m                 │  CPU: M4KK1 1.0 GHz       │\n");
    console_write("│  CPU: M4KK1 1.0 GHz             │  Memory: 1024MB / 2048MB  │\n");
    console_write("│  Memory: 1024MB / 2048MB        │  Disk: 50GB / 100GB       │\n");
    console_write("│  Disk: 50GB / 100GB             │  Network: 192.168.1.100  │\n");
    console_write("└─────────────────────────────────────┘\n");

    return 0;
}

/**
 * 三栏显示
 */
int ivfetch_display_triple_column(ivfetch_context_t *ctx) {
    if (!ctx) {
        return -1;
    }

    // 简化的三栏显示
    console_write("┌─────────────────────────────────────────────────────────────┐\n");
    console_write("│  M4KK1 OS          │  System Information  │  Hardware Info  │\n");
    console_write("│  Kernel: Y4KU      │  OS: M4KK1           │  CPU: M4KK1     │\n");
    console_write("│  Uptime: 1h 23m    │  Kernel: Y4KU 0.1.0  │  Memory: 1024MB │\n");
    console_write("│  Shell: m4sh       │  Uptime: 1h 23m      │  Disk: 50GB     │\n");
    console_write("│  User: m4kk1       │  User: m4kk1         │  Network: eth0  │\n");
    console_write("└─────────────────────────────────────────────────────────────┘\n");

    return 0;
}

/**
 * 紧凑显示
 */
int ivfetch_display_compact(ivfetch_context_t *ctx) {
    if (!ctx) {
        return -1;
    }

    console_write("M4KK1 OS | Y4KU 0.1.0 | m4kk1@m4kk1 | up 1h 23m | 1024MB/2048MB | 50GB/100GB\n");
    return 0;
}

/**
 * 显示艺术字
 */
int ivfetch_display_artwork(ivfetch_artwork_t *artwork) {
    if (!artwork || !artwork->content) {
        return -1;
    }

    // 简化的艺术字显示
    console_write(artwork->content);
    return 0;
}

/**
 * 收集系统信息
 */
int ivfetch_collect_system_info(ivfetch_system_info_t *info) {
    if (!info) {
        return -1;
    }

    // 模拟系统信息收集
    info->os_name = ivfetch_strdup("M4KK1");
    info->kernel_version = ivfetch_strdup("Y4KU 0.1.0");
    info->architecture = ivfetch_strdup("m4kk1");
    info->hostname = ivfetch_strdup("m4kk1");
    info->uptime = ivfetch_strdup("1h 23m");
    info->load_average = ivfetch_strdup("0.12 0.08 0.05");
    info->process_count = 42;
    info->thread_count = 84;

    return 0;
}

/**
 * 收集硬件信息
 */
int ivfetch_collect_hardware_info(ivfetch_hardware_info_t *info) {
    if (!info) {
        return -1;
    }

    // 模拟硬件信息收集
    info->cpu_model = ivfetch_strdup("M4KK1 1.0 GHz");
    info->cpu_cores = 1;
    info->cpu_threads = 1;
    info->cpu_frequency = 1000000000ULL;
    info->memory_total = 2147483648ULL;  // 2GB
    info->memory_used = 1073741824ULL;   // 1GB
    info->memory_free = 1073741824ULL;   // 1GB
    info->swap_total = 2147483648ULL;    // 2GB
    info->swap_used = 268435456ULL;      // 256MB
    info->disk_total = 107374182400ULL;  // 100GB
    info->disk_used = 53687091200ULL;    // 50GB

    return 0;
}

/**
 * 收集网络信息
 */
int ivfetch_collect_network_info(ivfetch_network_info_t *info) {
    if (!info) {
        return -1;
    }

    // 模拟网络信息收集
    info->primary_ip = ivfetch_strdup("192.168.1.100");
    info->mac_address = ivfetch_strdup("00:11:22:33:44:55");
    info->interface_name = ivfetch_strdup("eth0");
    info->gateway = ivfetch_strdup("192.168.1.1");
    info->dns_servers = ivfetch_strdup("8.8.8.8, 8.8.4.4");
    info->rx_bytes = 1234567890ULL;
    info->tx_bytes = 987654321ULL;
    info->active_connections = 5;

    return 0;
}

/**
 * 收集用户信息
 */
int ivfetch_collect_user_info(ivfetch_user_info_t *info) {
    if (!info) {
        return -1;
    }

    // 模拟用户信息收集
    info->username = ivfetch_strdup("m4kk1");
    info->real_name = ivfetch_strdup("M4KK1 User");
    info->home_dir = ivfetch_strdup("/home/m4kk1");
    info->shell = ivfetch_strdup("/usr/bin/m4sh");
    info->terminal = ivfetch_strdup("/dev/tty0");
    info->uid = 1000;
    info->gid = 1000;
    info->groups = ivfetch_strdup("m4kk1, wheel, users");

    return 0;
}

/**
 * 注册默认模块
 */
int ivfetch_register_default_modules(ivfetch_context_t *ctx) {
    if (!ctx) {
        return -1;
    }

    // 注册标题模块
    ivfetch_module_register(ctx, "title", IVFETCH_MODULE_TITLE,
                           NULL, ivfetch_format_title);

    // 注册系统模块
    ivfetch_module_register(ctx, "system", IVFETCH_MODULE_SYSTEM,
                           NULL, ivfetch_format_system);

    // 注册硬件模块
    ivfetch_module_register(ctx, "hardware", IVFETCH_MODULE_HARDWARE,
                           NULL, ivfetch_format_hardware);

    // 注册用户模块
    ivfetch_module_register(ctx, "user", IVFETCH_MODULE_USER,
                           NULL, ivfetch_format_user);

    return 0;
}

/**
 * 模块注册
 */
int ivfetch_module_register(ivfetch_context_t *ctx, const char *name, uint32_t type,
                           uint32_t (*collect_func)(void *), uint32_t (*format_func)(void *, char *, size_t)) {
    if (!ctx || !name) {
        return -1;
    }

    // 重新分配模块数组
    ivfetch_module_t *new_modules = ivfetch_realloc(ctx->modules,
                                                   (ctx->module_count + 1) * sizeof(ivfetch_module_t));
    if (!new_modules) {
        return -1;
    }

    ctx->modules = new_modules;

    // 初始化新模块
    ivfetch_module_t *module = &ctx->modules[ctx->module_count];
    ivfetch_memset(module, 0, sizeof(ivfetch_module_t));

    module->name = ivfetch_strdup(name);
    module->type = type;
    module->collect_func = collect_func;
    module->format_func = format_func;
    module->enabled = true;
    module->priority = ctx->module_count;

    ctx->module_count++;

    return 0;
}

/**
 * 更新模块数据
 */
int ivfetch_update_module_data(ivfetch_context_t *ctx, ivfetch_system_info_t *system_info,
                              ivfetch_hardware_info_t *hardware_info,
                              ivfetch_network_info_t *network_info,
                              ivfetch_user_info_t *user_info) {
    if (!ctx) {
        return -1;
    }

    // 为模块分配数据
    for (uint32_t i = 0; i < ctx->module_count; i++) {
        ivfetch_module_t *module = &ctx->modules[i];

        switch (module->type) {
            case IVFETCH_MODULE_SYSTEM:
                module->data = system_info;
                break;
            case IVFETCH_MODULE_HARDWARE:
                module->data = hardware_info;
                break;
            case IVFETCH_MODULE_NETWORK:
                module->data = network_info;
                break;
            case IVFETCH_MODULE_USER:
                module->data = user_info;
                break;
            default:
                module->data = NULL;
                break;
        }
    }

    return 0;
}

/**
 * 格式化标题信息
 */
uint32_t ivfetch_format_title(void *data, char *buffer, size_t size) {
    if (!buffer || size == 0) {
        return 0;
    }

    ivfetch_snprintf(buffer, size, "M4KK1 Operating System");
    return ivfetch_strlen(buffer);
}

/**
 * 格式化系统信息
 */
uint32_t ivfetch_format_system(void *data, char *buffer, size_t size) {
    if (!buffer || size == 0 || !data) {
        return 0;
    }

    ivfetch_system_info_t *info = (ivfetch_system_info_t *)data;

    ivfetch_snprintf(buffer, size, "OS: %s  Kernel: %s  Uptime: %s",
                    info->os_name, info->kernel_version, info->uptime);

    return ivfetch_strlen(buffer);
}

/**
 * 格式化硬件信息
 */
uint32_t ivfetch_format_hardware(void *data, char *buffer, size_t size) {
    if (!buffer || size == 0 || !data) {
        return 0;
    }

    ivfetch_hardware_info_t *info = (ivfetch_hardware_info_t *)data;

    ivfetch_snprintf(buffer, size, "CPU: %s  Memory: %lluMB / %lluMB",
                    info->cpu_model,
                    info->memory_used / 1024 / 1024,
                    info->memory_total / 1024 / 1024);

    return ivfetch_strlen(buffer);
}

/**
 * 格式化用户信息
 */
uint32_t ivfetch_format_user(void *data, char *buffer, size_t size) {
    if (!buffer || size == 0 || !data) {
        return 0;
    }

    ivfetch_user_info_t *info = (ivfetch_user_info_t *)data;

    ivfetch_snprintf(buffer, size, "User: %s  Shell: %s", info->username, info->shell);

    return ivfetch_strlen(buffer);
}

/**
 * 创建默认主题
 */
int ivfetch_theme_create_default(ivfetch_theme_t *theme) {
    if (!theme) {
        return -1;
    }

    ivfetch_memset(theme, 0, sizeof(ivfetch_theme_t));

    theme->name = ivfetch_strdup("m4kk1_default");
    theme->description = ivfetch_strdup("M4KK1 Default Theme");
    theme->author = ivfetch_strdup("M4KK1 Team");
    theme->version = ivfetch_strdup("1.0.0");

    // 设置默认颜色
    theme->colors[0] = (ivfetch_color_t){0, 0, 0, 255, false};        // Black
    theme->colors[1] = (ivfetch_color_t){255, 0, 0, 255, false};      // Red
    theme->colors[2] = (ivfetch_color_t){0, 255, 0, 255, false};      // Green
    theme->colors[3] = (ivfetch_color_t){255, 255, 0, 255, false};    // Yellow
    theme->colors[4] = (ivfetch_color_t){0, 0, 255, 255, false};      // Blue
    theme->colors[5] = (ivfetch_color_t){255, 0, 255, 255, false};    // Magenta
    theme->colors[6] = (ivfetch_color_t){0, 255, 255, 255, false};    // Cyan
    theme->colors[7] = (ivfetch_color_t){255, 255, 255, 255, false};  // White

    return 0;
}

/**
 * 创建ASCII艺术字
 */
int ivfetch_artwork_create_ascii(ivfetch_artwork_t *artwork, const char *content) {
    if (!artwork || !content) {
        return -1;
    }

    ivfetch_memset(artwork, 0, sizeof(ivfetch_artwork_t));

    artwork->content = ivfetch_strdup(content);
    artwork->type = IVFETCH_ARTWORK_ASCII;
    artwork->name = ivfetch_strdup("m4kk1_logo");

    // 计算尺寸
    char *p = artwork->content;
    artwork->width = 0;
    artwork->height = 0;

    while (*p) {
        if (*p == '\n') {
            artwork->height++;
            artwork->width = 0;
        } else {
            artwork->width++;
        }
        p++;
    }

    return 0;
}

/**
 * 初始化缓存
 */
int ivfetch_cache_init(ivfetch_context_t *ctx) {
    if (!ctx) {
        return -1;
    }

    // 简化实现
    ctx->cache = (void *)1;  // 占位符
    return 0;
}

/**
 * 显示帮助信息
 */
void ivfetch_show_help(void) {
    console_write("IVFetch - M4KK1 System Information Tool\n");
    console_write("\n");
    console_write("Usage: ivfetch [options]\n");
    console_write("\n");
    console_write("Options:\n");
    console_write("  -h, --help           Show this help message\n");
    console_write("  -v, --version        Show version information\n");
    console_write("  -c, --config FILE    Use specified config file\n");
    console_write("  -l, --layout LAYOUT  Set layout (single, double, triple, compact)\n");
    console_write("  -t, --theme THEME    Use specified theme\n");
    console_write("  --no-color           Disable colors\n");
    console_write("  --no-artwork         Don't display artwork\n");
    console_write("  --verbose            Enable verbose output\n");
    console_write("  --debug              Enable debug output\n");
    console_write("\n");
    console_write("Examples:\n");
    console_write("  ivfetch\n");
    console_write("  ivfetch --layout double\n");
    console_write("  ivfetch --theme minimal\n");
    console_write("  ivfetch --no-artwork --no-color\n");
    console_write("\n");
}

/**
 * 显示版本信息
 */
void ivfetch_show_version(void) {
    console_write("IVFetch ");
    console_write(IVFETCH_VERSION_STRING);
    console_write("\n");
    console_write("M4KK1 System Information Tool\n");
    console_write("Copyright (C) 2025 M4KK1 Team\n");
    console_write("License: GPL v3.0\n");
    console_write("\n");
}

/**
 * 获取版本号
 */
uint32_t ivfetch_get_version(void) {
    return IVFETCH_VERSION;
}

/**
 * 获取版本字符串
 */
const char *ivfetch_get_version_string(void) {
    return IVFETCH_VERSION_STRING;
}

/**
 * 字符串操作函数
 */
int ivfetch_strcmp(const char *s1, const char *s2) {
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;

    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }

    return (unsigned char)*s1 - (unsigned char)*s2;
}

size_t ivfetch_strlen(const char *str) {
    size_t len = 0;
    if (str) {
        while (str[len]) len++;
    }
    return len;
}

char *ivfetch_strcpy(char *dest, const char *src) {
    if (!dest || !src) return dest;
    char *d = dest;
    while (*src) *d++ = *src++;
    *d = '\0';
    return dest;
}

char *ivfetch_strcat(char *dest, const char *src) {
    if (!dest || !src) return dest;
    char *d = dest;
    while (*d) d++;
    while (*src) *d++ = *src++;
    *d = '\0';
    return dest;
}

char *ivfetch_strdup(const char *str) {
    if (!str) return NULL;
    size_t len = ivfetch_strlen(str);
    char *new_str = ivfetch_malloc(len + 1);
    if (new_str) {
        ivfetch_strcpy(new_str, str);
    }
    return new_str;
}

/**
 * 内存管理函数
 */
void *ivfetch_malloc(size_t size) {
    if (size == 0) return NULL;
    // 简化的内存分配
    static uint8_t buffer[8192];
    static uint32_t offset = 0;
    if (offset + size <= sizeof(buffer)) {
        void *ptr = &buffer[offset];
        offset += size;
        return ptr;
    }
    return NULL;
}

void ivfetch_free(void *ptr) {
    // 简化实现
}

void *ivfetch_realloc(void *ptr, size_t size) {
    if (!ptr) return ivfetch_malloc(size);
    return ptr;  // 简化实现
}

/**
 * 格式化函数
 */
int ivfetch_snprintf(char *buffer, size_t size, const char *format, ...) {
    if (!buffer || !format || size == 0) return 0;
    // 简化实现
    size_t len = ivfetch_strlen(format);
    if (len >= size) len = size - 1;
    ivfetch_strncpy(buffer, format, len);
    buffer[len] = '\0';
    return len;
}

char *ivfetch_strncpy(char *dest, const char *src, size_t n) {
    if (!dest || !src) return dest;
    char *d = dest;
    size_t i = 0;
    while (i < n && *src) {
        *d++ = *src++;
        i++;
    }
    while (i < n) {
        *d++ = '\0';
        i++;
    }
    return dest;
}

/**
 * 错误处理函数
 */
void ivfetch_error(const char *message) {
    if (message) {
        console_write("IVFetch Error: ");
        console_write(message);
        console_write("\n");
    }
}

void ivfetch_warning(const char *message) {
    if (message) {
        console_write("IVFetch Warning: ");
        console_write(message);
        console_write("\n");
    }
}

void ivfetch_debug(const char *message) {
    if (message) {
        console_write("IVFetch Debug: ");
        console_write(message);
        console_write("\n");
    }
}
</parameter>
<parameter name="line_count">329</parameter>
/**
 * M4SH - M4KK1 Shell 主头文件
 * 定义Shell的核心数据结构和接口
 */

#ifndef _M4SH_H
#define _M4SH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * M4SH魔数
 */
#define M4SH_MAGIC 0x4D345348    /* "M4SH" */

/**
 * M4SH版本
 */
#define M4SH_VERSION_MAJOR 0
#define M4SH_VERSION_MINOR 1
#define M4SH_VERSION_PATCH 0

/**
 * 常量定义
 */
#define M4SH_MAX_CMD_LEN    4096    /* 最大命令长度 */
#define M4SH_MAX_ARG_COUNT  256     /* 最大参数数量 */
#define M4SH_MAX_PATH_LEN   4096    /* 最大路径长度 */
#define M4SH_MAX_HISTORY    1000    /* 最大历史记录数 */
#define M4SH_MAX_JOBS       64      /* 最大作业数量 */
#define M4SH_MAX_ALIASES    256     /* 最大别名数量 */

/**
 * 命令类型
 */
#define M4SH_CMD_BUILTIN    0       /* 内置命令 */
#define M4SH_CMD_EXTERNAL   1       /* 外部命令 */
#define M4SH_CMD_FUNCTION   2       /* 函数定义 */

/**
 * 重定向类型
 */
#define M4SH_REDIRECT_NONE  0       /* 无重定向 */
#define M4SH_REDIRECT_IN    1       /* 输入重定向 */
#define M4SH_REDIRECT_OUT   2       /* 输出重定向 */
#define M4SH_REDIRECT_APPEND 3      /* 追加重定向 */
#define M4SH_REDIRECT_ERR   4       /* 错误重定向 */
#define M4SH_REDIRECT_HERE  5       /* Here文档 */

/**
 * 作业状态
 */
#define M4SH_JOB_RUNNING    0       /* 运行中 */
#define M4SH_JOB_STOPPED    1       /* 已停止 */
#define M4SH_JOB_DONE       2       /* 已完成 */
#define M4SH_JOB_KILLED     3       /* 已杀死 */

/**
 * Shell配置结构
 */
typedef struct {
    char *prompt;               /* 命令提示符 */
    char *home_dir;             /* 主目录 */
    char *current_dir;          /* 当前目录 */
    char *path;                 /* PATH环境变量 */
    char *shell;                /* SHELL环境变量 */
    char *user;                 /* USER环境变量 */
    char *hostname;             /* 主机名 */
    bool echo;                  /* 回显标志 */
    bool verbose;               /* 详细输出 */
    bool debug;                 /* 调试模式 */
    bool interactive;           /* 交互模式 */
    bool login_shell;           /* 登录Shell */
    uint32_t umask;             /* 文件创建掩码 */
    uint32_t options;           /* Shell选项 */
} m4sh_config_t;

/**
 * 命令结构
 */
typedef struct {
    char *name;                 /* 命令名 */
    uint32_t type;              /* 命令类型 */
    char **argv;                /* 参数列表 */
    uint32_t argc;              /* 参数数量 */
    char *input_file;           /* 输入文件 */
    char *output_file;          /* 输出文件 */
    char *error_file;           /* 错误文件 */
    bool append_output;         /* 追加输出 */
    bool background;            /* 后台运行 */
    int pipe_read;              /* 管道读端 */
    int pipe_write;             /* 管道写端 */
    struct m4sh_command *next;  /* 下一个命令（管道） */
} m4sh_command_t;

/**
 * 作业结构
 */
typedef struct {
    uint32_t job_id;            /* 作业ID */
    uint32_t status;            /* 作业状态 */
    uint32_t process_count;     /* 进程数量 */
    pid_t *pids;                /* 进程ID列表 */
    m4sh_command_t *command;    /* 命令结构 */
    char *command_line;         /* 命令行 */
    time_t start_time;          /* 启动时间 */
    struct m4sh_job *next;      /* 下一个作业 */
} m4sh_job_t;

/**
 * 别名结构
 */
typedef struct {
    char *name;                 /* 别名名称 */
    char *value;                /* 别名值 */
    bool global;                /* 全局别名 */
    struct m4sh_alias *next;    /* 下一个别名 */
} m4sh_alias_t;

/**
 * 函数结构
 */
typedef struct {
    char *name;                 /* 函数名 */
    m4sh_command_t *body;       /* 函数体 */
    char **local_vars;          /* 局部变量 */
    uint32_t var_count;         /* 变量数量 */
    struct m4sh_function *next; /* 下一个函数 */
} m4sh_function_t;

/**
 * 历史记录结构
 */
typedef struct {
    char *command;              /* 命令字符串 */
    time_t timestamp;           /* 时间戳 */
    uint32_t exit_status;       /* 退出状态 */
    struct m4sh_history *next;  /* 下一个记录 */
} m4sh_history_t;

/**
 * Shell上下文结构
 */
typedef struct {
    uint32_t magic;             /* 魔数 */
    uint32_t version;           /* 版本 */
    m4sh_config_t *config;      /* 配置 */
    m4sh_job_t *jobs;           /* 作业列表 */
    m4sh_alias_t *aliases;      /* 别名列表 */
    m4sh_function_t *functions; /* 函数列表 */
    m4sh_history_t *history;    /* 历史记录 */
    char **environment;         /* 环境变量 */
    uint32_t env_count;         /* 环境变量数量 */
    char **variables;           /* Shell变量 */
    uint32_t var_count;         /* 变量数量 */
    uint32_t current_job_id;    /* 当前作业ID */
    uint32_t last_exit_status;  /* 最后退出状态 */
    bool exit_requested;        /* 退出请求标志 */
    int input_fd;               /* 输入文件描述符 */
    int output_fd;              /* 输出文件描述符 */
    int error_fd;               /* 错误文件描述符 */
} m4sh_context_t;

/**
 * 内置命令函数指针类型
 */
typedef int (*m4sh_builtin_func_t)(m4sh_context_t *ctx, int argc, char *argv[]);

/**
 * 内置命令结构
 */
typedef struct {
    char *name;                 /* 命令名 */
    m4sh_builtin_func_t func;   /* 函数指针 */
    char *description;          /* 命令描述 */
    char *usage;                /* 使用说明 */
} m4sh_builtin_t;

/**
 * 函数声明
 */

/* Shell初始化和清理 */
int m4sh_init(m4sh_context_t *ctx);
int m4sh_cleanup(m4sh_context_t *ctx);
int m4sh_load_config(m4sh_context_t *ctx, const char *config_file);
int m4sh_save_config(m4sh_context_t *ctx, const char *config_file);

/* 主循环 */
int m4sh_main_loop(m4sh_context_t *ctx);
int m4sh_read_command(m4sh_context_t *ctx, char *buffer, size_t size);
int m4sh_parse_command(m4sh_context_t *ctx, const char *command_line, m4sh_command_t **command);
int m4sh_execute_command(m4sh_context_t *ctx, m4sh_command_t *command);

/* 内置命令 */
int m4sh_builtin_cd(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_echo(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_pwd(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_exit(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_ls(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_cat(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_mkdir(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_rmdir(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_rm(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_cp(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_mv(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_ps(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_kill(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_jobs(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_fg(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_bg(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_history(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_alias(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_umask(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_which(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_whereis(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_type(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_source(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_test(m4sh_context_t *ctx, int argc, char *argv[]);
int m4sh_builtin_bracket(m4sh_context_t *ctx, int argc, char *argv[]);

/* 作业管理 */
int m4sh_job_add(m4sh_context_t *ctx, m4sh_command_t *command, pid_t pid);
int m4sh_job_remove(m4sh_context_t *ctx, uint32_t job_id);
int m4sh_job_find(m4sh_context_t *ctx, uint32_t job_id, m4sh_job_t **job);
int m4sh_job_wait(m4sh_context_t *ctx, uint32_t job_id);
int m4sh_job_foreground(m4sh_context_t *ctx, uint32_t job_id);
int m4sh_job_background(m4sh_context_t *ctx, uint32_t job_id);
int m4sh_job_stop(m4sh_context_t *ctx, uint32_t job_id);
int m4sh_job_continue(m4sh_context_t *ctx, uint32_t job_id);

/* 别名管理 */
int m4sh_alias_add(m4sh_context_t *ctx, const char *name, const char *value, bool global);
int m4sh_alias_remove(m4sh_context_t *ctx, const char *name);
int m4sh_alias_find(m4sh_context_t *ctx, const char *name, m4sh_alias_t **alias);
int m4sh_alias_expand(m4sh_context_t *ctx, char *command_line);

/* 函数管理 */
int m4sh_function_add(m4sh_context_t *ctx, const char *name, m4sh_command_t *body);
int m4sh_function_remove(m4sh_context_t *ctx, const char *name);
int m4sh_function_find(m4sh_context_t *ctx, const char *name, m4sh_function_t **function);
int m4sh_function_execute(m4sh_context_t *ctx, m4sh_function_t *function, int argc, char *argv[]);

/* 历史管理 */
int m4sh_history_add(m4sh_context_t *ctx, const char *command, uint32_t exit_status);
int m4sh_history_find(m4sh_context_t *ctx, const char *pattern, m4sh_history_t **history, uint32_t *count);
int m4sh_history_load(m4sh_context_t *ctx, const char *history_file);
int m4sh_history_save(m4sh_context_t *ctx, const char *history_file);

/* 变量和环境 */
int m4sh_variable_set(m4sh_context_t *ctx, const char *name, const char *value);
int m4sh_variable_get(m4sh_context_t *ctx, const char *name, char **value);
int m4sh_variable_unset(m4sh_context_t *ctx, const char *name);
int m4sh_environment_set(m4sh_context_t *ctx, const char *name, const char *value);
int m4sh_environment_get(m4sh_context_t *ctx, const char *name, char **value);

/* 路径和补全 */
int m4sh_path_expand(m4sh_context_t *ctx, char *path);
int m4sh_path_find_command(m4sh_context_t *ctx, const char *command, char **full_path);
int m4sh_complete_command(m4sh_context_t *ctx, const char *partial, char ***completions, uint32_t *count);
int m4sh_complete_path(m4sh_context_t *ctx, const char *partial, char ***completions, uint32_t *count);

/* 管道和重定向 */
int m4sh_pipeline_create(m4sh_command_t *commands, int *pipe_fds);
int m4sh_redirect_setup(m4sh_command_t *command);
int m4sh_redirect_restore(int saved_fds[3]);

/* 信号处理 */
int m4sh_signal_init(m4sh_context_t *ctx);
int m4sh_signal_handler(int signal);

/* 工具函数 */
uint32_t m4sh_get_version(void);
const char *m4sh_get_version_string(void);
int m4sh_strcmp(const char *s1, const char *s2);
size_t m4sh_strlen(const char *str);
char *m4sh_strcpy(char *dest, const char *src);
char *m4sh_strcat(char *dest, const char *src);
char *m4sh_strdup(const char *str);

/* 内存管理 */
void *m4sh_malloc(size_t size);
void m4sh_free(void *ptr);
void *m4sh_realloc(void *ptr, size_t size);

/* 错误处理 */
void m4sh_error(m4sh_context_t *ctx, const char *message);
void m4sh_warning(m4sh_context_t *ctx, const char *message);
void m4sh_debug(m4sh_context_t *ctx, const char *message);

/* 版本信息宏 */
#define M4SH_VERSION \
    (M4SH_VERSION_MAJOR << 16 | M4SH_VERSION_MINOR << 8 | M4SH_VERSION_PATCH)

#define M4SH_VERSION_STRING \
    "M4SH " \
    STRINGIFY(M4SH_VERSION_MAJOR) "." \
    STRINGIFY(M4SH_VERSION_MINOR) "." \
    STRINGIFY(M4SH_VERSION_PATCH)

/* 字符串化宏 */
#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

#endif /* _M4SH_H */
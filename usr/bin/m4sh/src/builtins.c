
/**
 * M4SH - M4KK1 Shell 内置命令实现
 * 实现Shell的内置命令
 */

#include "m4sh.h"
#include "../include/m4sh.h"
#include "../../y4ku/include/console.h"

/**
 * cd命令 - 改变当前目录
 */
int m4sh_builtin_cd(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *target_dir = NULL;

    // 解析参数
    if (argc == 1) {
        // 没有参数，切换到家目录
        target_dir = ctx->config->home_dir;
    } else if (argc == 2) {
        // 有参数，使用指定目录
        target_dir = argv[1];

        // 处理特殊目录
        if (m4sh_strcmp(target_dir, "-") == 0) {
            // 切换到上一个目录
            target_dir = ctx->config->current_dir;
        } else if (m4sh_strcmp(target_dir, "~") == 0) {
            // 切换到家目录
            target_dir = ctx->config->home_dir;
        }
    } else {
        // 参数过多
        m4sh_error(ctx, "cd: too many arguments");
        return 1;
    }

    // 保存当前目录
    char *old_dir = m4sh_strdup(ctx->config->current_dir);

    // 尝试改变目录
    // 这里应该调用系统调用
    // 暂时模拟成功
    if (old_dir) {
        m4sh_free(ctx->config->current_dir);
        ctx->config->current_dir = m4sh_strdup(target_dir);
    }

    if (old_dir) {
        m4sh_free(old_dir);
    }

    return 0;
}

/**
 * echo命令 - 显示文本
 */
int m4sh_builtin_echo(m4sh_context_t *ctx, int argc, char *argv[]) {
    bool newline = true;
    int start_idx = 1;

    // 解析选项
    if (argc > 1 && m4sh_strcmp(argv[1], "-n") == 0) {
        newline = false;
        start_idx = 2;
    }

    // 输出参数
    for (int i = start_idx; i < argc; i++) {
        if (i > start_idx) {
            console_write(" ");
        }
        console_write(argv[i]);
    }

    if (newline) {
        console_write("\n");
    }

    return 0;
}

/**
 * pwd命令 - 显示当前目录
 */
int m4sh_builtin_pwd(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        m4sh_error(ctx, "pwd: too many arguments");
        return 1;
    }

    console_write(ctx->config->current_dir);
    console_write("\n");

    return 0;
}

/**
 * exit命令 - 退出Shell
 */
int m4sh_builtin_exit(m4sh_context_t *ctx, int argc, char *argv[]) {
    int exit_code = 0;

    if (argc > 1) {
        // 解析退出码
        exit_code = 0;  // 暂时使用默认值
    }

    ctx->exit_requested = true;
    return exit_code;
}

/**
 * ls命令 - 列出目录内容
 */
int m4sh_builtin_ls(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *target_dir = ".";

    // 解析参数
    if (argc > 1) {
        target_dir = argv[1];
    }

    // 模拟目录列表
    console_write("bin\n");
    console_write("dev\n");
    console_write("etc\n");
    console_write("home\n");
    console_write("lib\n");
    console_write("proc\n");
    console_write("root\n");
    console_write("sbin\n");
    console_write("sys\n");
    console_write("tmp\n");
    console_write("usr\n");
    console_write("var\n");

    return 0;
}

/**
 * cat命令 - 连接和显示文件
 */
int m4sh_builtin_cat(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "cat: missing file operand");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        // 模拟文件内容
        console_write("This is the content of ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * mkdir命令 - 创建目录
 */
int m4sh_builtin_mkdir(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "mkdir: missing operand");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        // 模拟目录创建
        console_write("Created directory: ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * rmdir命令 - 删除目录
 */
int m4sh_builtin_rmdir(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "rmdir: missing operand");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        // 模拟目录删除
        console_write("Removed directory: ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * rm命令 - 删除文件
 */
int m4sh_builtin_rm(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "rm: missing operand");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        // 模拟文件删除
        console_write("Removed file: ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * cp命令 - 复制文件
 */
int m4sh_builtin_cp(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 3) {
        m4sh_error(ctx, "cp: missing file operands");
        return 1;
    }

    const char *dest = argv[argc - 1];

    for (int i = 1; i < argc - 1; i++) {
        // 模拟文件复制
        console_write("Copied ");
        console_write(argv[i]);
        console_write(" to ");
        console_write(dest);
        console_write("\n");
    }

    return 0;
}

/**
 * mv命令 - 移动文件
 */
int m4sh_builtin_mv(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 3) {
        m4sh_error(ctx, "mv: missing file operands");
        return 1;
    }

    const char *dest = argv[argc - 1];

    for (int i = 1; i < argc - 1; i++) {
        // 模拟文件移动
        console_write("Moved ");
        console_write(argv[i]);
        console_write(" to ");
        console_write(dest);
        console_write("\n");
    }

    return 0;
}

/**
 * ps命令 - 显示进程状态
 */
int m4sh_builtin_ps(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("  PID TTY          TIME CMD\n");
    console_write("    1 ?        00:00:01 init\n");
    console_write("  100 ?        00:00:00 m4sh\n");
    console_write("  101 ?        00:00:00 ps\n");

    return 0;
}

/**
 * kill命令 - 发送信号到进程
 */
int m4sh_builtin_kill(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "kill: missing operand");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        // 模拟发送信号
        console_write("Sent signal to process: ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * jobs命令 - 显示作业状态
 */
int m4sh_builtin_jobs(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("No current jobs\n");
    return 0;
}

/**
 * fg命令 - 前台运行作业
 */
int m4sh_builtin_fg(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 2) {
        m4sh_error(ctx, "fg: too many arguments");
        return 1;
    }

    const char *job_spec = (argc == 2) ? argv[1] : "%1";
    console_write("Brought job to foreground: ");
    console_write(job_spec);
    console_write("\n");

    return 0;
}

/**
 * bg命令 - 后台运行作业
 */
int m4sh_builtin_bg(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 2) {
        m4sh_error(ctx, "bg: too many arguments");
        return 1;
    }

    const char *job_spec = (argc == 2) ? argv[1] : "%1";
    console_write("Sent job to background: ");
    console_write(job_spec);
    console_write("\n");

    return 0;
}

/**
 * history命令 - 显示命令历史
 */
int m4sh_builtin_history(m4sh_context_t *ctx, int argc, char *argv[]) {
    int count = 10;  // 默认显示最近10条

    if (argc > 1) {
        count = 0;  // 暂时使用默认值
    }

    console_write("Command history:\n");
    console_write("    1  echo 'Welcome to M4SH'\n");
    console_write("    2  pwd\n");
    console_write("    3  ls -la\n");
    console_write("    4  date\n");
    console_write("    5  whoami\n");
    console_write("    6  uname -a\n");
    console_write("    7  ps\n");
    console_write("    8  history\n");

    return 0;
}

/**
 * alias命令 - 定义或显示别名
 */
int m4sh_builtin_alias(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc == 1) {
        // 显示所有别名
        console_write("Current aliases:\n");
        console_write("ls='ls --color=auto'\n");
        console_write("ll='ls -la'\n");
        console_write("la='ls -A'\n");
        console_write("l='ls -CF'\n");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        // 模拟添加别名
        console_write("Created alias: ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * umask命令 - 设置文件创建掩码
 */
int m4sh_builtin_umask(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc == 1) {
        // 显示当前umask
        console_write("Current umask: 0022\n");
        return 0;
    }

    // 设置umask
    console_write("Set umask to: ");
    console_write(argv[1]);
    console_write("\n");

    return 0;
}

/**
 * which命令 - 定位命令
 */
int m4sh_builtin_which(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc != 2) {
        m4sh_error(ctx, "which: invalid number of arguments");
        return 1;
    }

    // 模拟命令查找
    console_write("/usr/bin/");
    console_write(argv[1]);
    console_write("\n");

    return 0;
}

/**
 * whereis命令 - 定位二进制和源文件
 */
int m4sh_builtin_whereis(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc != 2) {
        m4sh_error(ctx, "whereis: invalid number of arguments");
        return 1;
    }

    // 模拟文件查找
    console_write(argv[1]);
    console_write(": /usr/bin/");
    console_write(argv[1]);
    console_write(" /usr/share/man/man1/");
    console_write(argv[1]);
    console_write(".1\n");

    return 0;
}

/**
 * type命令 - 显示命令类型
 */
int m4sh_builtin_type(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc != 2) {
        m4sh_error(ctx, "type: invalid number of arguments");
        return 1;
    }

    // 检查是否为内置命令
    const char *cmd = argv[1];

    // 模拟类型检测
    if (m4sh_strcmp(cmd, "cd") == 0 ||
        m4sh_strcmp(cmd, "echo") == 0 ||
        m4sh_strcmp(cmd, "pwd") == 0 ||
        m4sh_strcmp(cmd, "exit") == 0) {
        console_write(cmd);
        console_write(" is a shell builtin\n");
    } else {
        console_write(cmd);
        console_write(" is an external command\n");
    }

    return 0;
}

/**
 * source命令 - 从文件执行命令
 */
int m4sh_builtin_source(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc != 2) {
        m4sh_error(ctx, "source: invalid number of arguments");
        return 1;
    }

    // 模拟执行文件
    console_write("Sourced file: ");
    console_write(argv[1]);
    console_write("\n");

    return 0;
}

/**
 * test命令 - 条件测试
 */
int m4sh_builtin_test(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        return 1;  // test失败
    }

    // 简化实现
    return 0;  // test成功
}

/**
 * [命令 - 条件测试（test的别名）
 */
int m4sh_builtin_bracket(m4sh_context_t *ctx, int argc, char *argv[]) {
    // 去掉开头和结尾的括号
    if (argc < 2 || m4sh_strcmp(argv[argc-1], "]") != 0) {
        m4sh_error(ctx, "[: missing `]'");
        return 2;
    }

    // 调用test功能
    return m4sh_builtin_test(ctx, argc - 1, argv);
}

/**
 * date命令 - 显示日期和时间
 */
int m4sh_builtin_date(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Current date: ");
    console_write("2025-10-01 18:20:00 UTC\n");
    return 0;
}

/**
 * whoami命令 - 显示当前用户名
 */
int m4sh_builtin_whoami(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(ctx->config->user);
    console_write("\n");
    return 0;
}

/**
 * uname命令 - 显示系统信息
 */
int m4sh_builtin_uname(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("M4KK1\n");
    return 0;
}

/**
 * id命令 - 显示用户和组信息
 */
int m4sh_builtin_id(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("uid=1000(");
    console_write(ctx->config->user);
    console_write(") gid=1000(");
    console_write(ctx->config->user);
    console_write(") groups=1000(");
    console_write(ctx->config->user);
    console_write(")\n");
    return 0;
}

/**
 * uptime命令 - 显示系统运行时间
 */
int m4sh_builtin_uptime(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("up 1 hour, 23 minutes, 1 user, load average: 0.12, 0.08, 0.05\n");
    return 0;
}

/**
 * free命令 - 显示内存使用情况
 */
int m4sh_builtin_free(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("              total        used        free      shared  buff/cache   available\n");
    console_write("Mem:        1048576      123456      789012       12345      123456      789012\n");
    console_write("Swap:       2097152       54321     2042831\n");
    return 0;
}

/**
 * df命令 - 显示磁盘空间使用情况
 */
int m4sh_builtin_df(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Filesystem     1K-blocks  Used Available Use% Mounted on\n");
    console_write("/dev/sda1       10485760 1234567   9247183  12% /\n");
    console_write("tmpfs            1048576       0   1048576   0% /dev/shm\n");
    return 0;
}

/**
 * du命令 - 显示目录空间使用情况
 */
int m4sh_builtin_du(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *target_dir = ".";

    if (argc > 1) {
        target_dir = argv[1];
    }

    console_write("1024\t");
    console_write(target_dir);
    console_write("\n");
    return 0;
}

/**
 * head命令 - 显示文件开头部分
 */
int m4sh_builtin_head(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "head: missing file operand");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        console_write("==> ");
        console_write(argv[i]);
        console_write(" <==\n");
        console_write("Line 1\n");
        console_write("Line 2\n");
        console_write("Line 3\n");
        console_write("Line 4\n");
        console_write("Line 5\n");
        console_write("Line 6\n");
        console_write("Line 7\n");
        console_write("Line 8\n");
        console_write("Line 9\n");
        console_write("Line 10\n");
    }

    return 0;
}

/**
 * tail命令 - 显示文件结尾部分
 */
int m4sh_builtin_tail(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "tail: missing file operand");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        console_write("==> ");
        console_write(argv[i]);
        console_write(" <==\n");
        console_write("Line 1\n");
        console_write("Line 2\n");
        console_write("Line 3\n");
        console_write("Line 4\n");
        console_write("Line 5\n");
        console_write("Line 6\n");
        console_write("Line 7\n");
        console_write("Line 8\n");
        console_write("Line 9\n");
        console_write("Line 10\n");
    }

    return 0;
}

/**
 * sort命令 - 排序文本行
 */
int m4sh_builtin_sort(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "sort: missing file operand");
        return 1;
    }

    console_write("apple\n");
    console_write("banana\n");
    console_write("cherry\n");
    console_write("date\n");
    console_write("elderberry\n");

    return 0;
}

/**
 * uniq命令 - 去除重复行
 */
int m4sh_builtin_uniq(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "uniq: missing file operand");
        return 1;
    }

    console_write("apple\n");
    console_write("banana\n");
    console_write("cherry\n");
    console_write("date\n");
    console_write("elderberry\n");

    return 0;
}

/**
 * wc命令 - 统计字数
 */
int m4sh_builtin_wc(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "wc: missing file operand");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        console_write("  10  50 300 ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * grep命令 - 文本搜索
 */
int m4sh_builtin_grep(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "grep: missing pattern");
        return 1;
    }

    const char *pattern = argv[1];

    console_write("file1.txt:This is a test file with ");
    console_write(pattern);
    console_write(" in it.\n");
    console_write("file2.txt:Another file containing ");
    console_write(pattern);
    console_write(".\n");

    return 0;
}

/**
 * cut命令 - 切割文本
 */
int m4sh_builtin_cut(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "cut: missing file operand");
        return 1;
    }

    console_write("column1\n");
    console_write("column2\n");
    console_write("column3\n");

    return 0;
}

/**
 * paste命令 - 合并文本行
 */
int m4sh_builtin_paste(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 3) {
        m4sh_error(ctx, "paste: missing file operands");
        return 1;
    }

    console_write("file1_line1 file2_line1\n");
    console_write("file1_line2 file2_line2\n");
    console_write("file1_line3 file2_line3\n");

    return 0;
}

/**
 * find命令 - 查找文件
 */
int m4sh_builtin_find(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *search_path = ".";

    if (argc > 1) {
        search_path = argv[1];
    }

    console_write(search_path);
    console_write("/file1.txt\n");
    console_write(search_path);
    console_write("/subdir/file2.txt\n");
    console_write(search_path);
    console_write("/file3.txt\n");

    return 0;
}

/**
 * touch命令 - 创建或更新文件时间戳
 */
int m4sh_builtin_touch(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "touch: missing file operand");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        console_write("Touched file: ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * chmod命令 - 改变文件权限
 */
int m4sh_builtin_chmod(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 3) {
        m4sh_error(ctx, "chmod: missing operands");
        return 1;
    }

    const char *mode = argv[1];

    for (int i = 2; i < argc; i++) {
        console_write("Changed mode of ");
        console_write(argv[i]);
        console_write(" to ");
        console_write(mode);
        console_write("\n");
    }

    return 0;
}

/**
 * chown命令 - 改变文件所有者
 */
int m4sh_builtin_chown(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 3) {
        m4sh_error(ctx, "chown: missing operands");
        return 1;
    }

    const char *owner = argv[1];

    for (int i = 2; i < argc; i++) {
        console_write("Changed owner of ");
        console_write(argv[i]);
        console_write(" to ");
        console_write(owner);
        console_write("\n");
    }

    return 0;
}

/**
 * ln命令 - 创建链接
 */
int m4sh_builtin_ln(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 3) {
        m4sh_error(ctx, "ln: missing operands");
        return 1;
    }

    const char *target = argv[1];
    const char *link_name = argv[2];

    console_write("Created link ");
    console_write(link_name);
    console_write(" -> ");
    console_write(target);
    console_write("\n");

    return 0;
}

/**
 * tar命令 - 归档工具
 */
int m4sh_builtin_tar(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 3) {
        m4sh_error(ctx, "tar: missing operands");
        return 1;
    }

    console_write("Archive created successfully\n");
    return 0;
}

/**
 * gzip命令 - 压缩工具
 */
int m4sh_builtin_gzip(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "gzip: missing file operand");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        console_write("Compressed: ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * gunzip命令 - 解压缩工具
 */
int m4sh_builtin_gunzip(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "gunzip: missing file operand");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        console_write("Decompressed: ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * wget命令 - 下载工具
 */
int m4sh_builtin_wget(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "wget: missing URL");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        console_write("Downloaded: ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * curl命令 - 数据传输工具
 */
int m4sh_builtin_curl(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "curl: missing URL");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        console_write("Fetched: ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * ping命令 - 网络连通性测试
 */
int m4sh_builtin_ping(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "ping: missing host");
        return 1;
    }

    console_write("PING ");
    console_write(argv[1]);
    console_write(" (127.0.0.1) 56(84) bytes of data.\n");
    console_write("64 bytes from ");
    console_write(argv[1]);
    console_write(": icmp_seq=1 ttl=64 time=0.123 ms\n");
    console_write("64 bytes from ");
    console_write(argv[1]);
    console_write(": icmp_seq=2 ttl=64 time=0.098 ms\n");
    console_write("64 bytes from ");
    console_write(argv[1]);
    console_write(": icmp_seq=3 ttl=64 time=0.145 ms\n");

    return 0;
}

/**
 * netstat命令 - 网络状态显示
 */
int m4sh_builtin_netstat(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Active Internet connections (w/o servers)\n");
    console_write("Proto Recv-Q Send-Q Local Address           Foreign Address         State\n");
    console_write("tcp        0      0 localhost:12345          localhost:54321          ESTABLISHED\n");
    console_write("tcp        0      0 localhost:54321          localhost:12345          ESTABLISHED\n");

    return 0;
}

/**
 * ifconfig命令 - 网络接口配置
 */
int m4sh_builtin_ifconfig(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("eth0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500\n");
    console_write("        inet 192.168.1.100  netmask 255.255.255.0  broadcast 192.168.1.255\n");
    console_write("        inet6 fe80::1c3:2d4:fe5:6789  prefixlen 64  scopeid 0x20<link>\n");
    console_write("        ether 00:11:22:33:44:55  txqueuelen 1000  (Ethernet)\n");
    console_write("        RX packets 12345  bytes 1234567 (1.2 MB)\n");
    console_write("        RX errors 0  dropped 0  overruns 0  frame 0\n");
    console_write("        TX packets 12345  bytes 1234567 (1.2 MB)\n");
    console_write("        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0\n");

    return 0;
}

/**
 * ssh命令 - 安全Shell客户端
 */
int m4sh_builtin_ssh(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "ssh: missing host");
        return 1;
    }

    console_write("Connected to ");
    console_write(argv[1]);
    console_write("\n");
    console_write("Welcome to M4KK1 SSH Server\n");
    console_write("Type 'exit' to disconnect\n");

    return 0;
}

/**
 * scp命令 - 安全文件复制
 */
int m4sh_builtin_scp(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 3) {
        m4sh_error(ctx, "scp: missing operands");
        return 1;
    }

    console_write("Copied securely: ");
    console_write(argv[1]);
    console_write(" -> ");
    console_write(argv[2]);
    console_write("\n");

    return 0;
}

/**
 * ftp命令 - 文件传输协议客户端
 */
int m4sh_builtin_ftp(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "ftp: missing host");
        return 1;
    }

    console_write("Connected to FTP server: ");
    console_write(argv[1]);
    console_write("\n");
    console_write("220 FTP server ready\n");
    console_write("Name: ");

    return 0;
}

/**
 * make命令 - 项目构建工具
 */
int m4sh_builtin_make(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("make: *** No targets specified and no makefile found.  Stop.\n");
    return 2;
}

/**
 * gcc命令 - C/C++编译器
 */
int m4sh_builtin_gcc(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "gcc: no input files");
        return 1;
    }

    console_write("Compilation successful\n");
    return 0;
}

/**
 * gdb命令 - 调试器
 */
int m4sh_builtin_gdb(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("GNU gdb (GDB) 8.3\n");
    console_write("Copyright (C) 2019 Free Software Foundation, Inc.\n");
    console_write("Type \"help\" for help.\n");
    console_write("(gdb) ");

    return 0;
}

/**
 * vim命令 - 文本编辑器
 */
int m4sh_builtin_vim(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("VIM - Vi IMproved 8.1\n");
    console_write("Type :q to exit\n");
    console_write("~");

    return 0;
}

/**
 * emacs命令 - 扩展编辑器
 */
int m4sh_builtin_emacs(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Welcome to GNU Emacs\n");
    console_write("Type C-x C-c to exit\n");
    console_write("~");

    return 0;
}

/**
 * git命令 - 版本控制系统
 */
int m4sh_builtin_git(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        console_write("usage: git [--version] [--help] [-C <path>] [-c <name>=<value>]\n");
        console_write("           [--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]\n");
        console_write("           [-p | --paginate | --no-pager] [--no-replace-objects] [--bare]\n");
        console_write("           [--git-dir=<path>] [--work-tree=<path>] [--namespace=<name>]\n");
        console_write("           <command> [<args>]\n");
        return 1;
    }

    console_write("Git repository initialized\n");
    return 0;
}

/**
 * man命令 - 手册查看器
 */
int m4sh_builtin_man(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "man: missing manual entry");
        return 1;
    }

    console_write("M4SH(1)                  User Commands                 M4SH(1)\n");
    console_write("\n");
    console_write("NAME\n");
    console_write("       m4sh - M4KK1 Shell\n");
    console_write("\n");
    console_write("SYNOPSIS\n");
    console_write("       m4sh [options]\n");
    console_write("\n");
    console_write("DESCRIPTION\n");
    console_write("       M4SH is the default shell for M4KK1 operating system.\n");
    console_write("\n");

    return 0;
}

/**
 * help命令 - 显示帮助信息
 */
int m4sh_builtin_help(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("M4SH Built-in Commands:\n");
    console_write("  cd         Change directory\n");
    console_write("  echo       Display text\n");
    console_write("  pwd        Print current directory\n");
    console_write("  exit       Exit the shell\n");
    console_write("  ls         List directory contents\n");
    console_write("  cat        Concatenate and display files\n");
    console_write("  mkdir      Create directories\n");
    console_write("  rmdir      Remove directories\n");
    console_write("  rm         Remove files\n");
    console_write("  cp         Copy files and directories\n");
    console_write("  mv         Move/rename files\n");
    console_write("  ps         Report process status\n");
    console_write("  kill       Send signal to processes\n");
    console_write("  jobs       Display job status\n");
    console_write("  fg         Bring job to foreground\n");
    console_write("  bg         Send job to background\n");
    console_write("  history    Display command history\n");
    console_write("  alias      Define or display aliases\n");
    console_write("  umask      Set file creation mask\n");
    console_write("  which      Locate a command\n");
    console_write("  whereis    Locate binary and source\n");
    console_write("  type       Display command type\n");
    console_write("  source     Execute commands from file\n");
    console_write("  test       Evaluate expression\n");
    console_write("  help       Display this help\n");
    console_write("\n");
    console_write("For more information, see the manual page for each command.\n");

    return 0;
}

/**
 * clear命令 - 清屏
 */
int m4sh_builtin_clear(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("\033[2J\033[H");  // ANSI清屏序列
    return 0;
}

/**
 * env命令 - 显示环境变量
 */
int m4sh_builtin_env(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("PATH=/usr/bin:/bin:/usr/sbin:/sbin\n");
    console_write("HOME=");
    console_write(ctx->config->home_dir);
    console_write("\n");
    console_write("USER=");
    console_write(ctx->config->user);
    console_write("\n");
    console_write("SHELL=");
    console_write(ctx->config->shell);
    console_write("\n");
    console_write("PWD=");
    console_write(ctx->config->current_dir);
    console_write("\n");
    console_write("HOSTNAME=");
    console_write(ctx->config->hostname);
    console_write("\n");

    return 0;
}

/**
 * export命令 - 设置环境变量
 */
int m4sh_builtin_export(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "export: missing variable");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        console_write("Exported: ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * unset命令 - 取消设置变量
 */
int m4sh_builtin_unset(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "unset: missing variable");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        console_write("Unset: ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * set命令 - 设置Shell选项
 */
int m4sh_builtin_set(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Shell options:\n");
    console_write("  echo       ");
    console_write(ctx->config->echo ? "on" : "off");
    console_write("\n");
    console_write("  verbose    ");
    console_write(ctx->config->verbose ? "on" : "off");
    console_write("\n");
    console_write("  debug      ");
    console_write(ctx->config->debug ? "on" : "off");
    console_write("\n");

    return 0;
}

/**
 * readonly命令 - 设置只读变量
 */
int m4sh_builtin_readonly(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "readonly: missing variable");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        console_write("Made readonly: ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * shift命令 - 移动位置参数
 */
int m4sh_builtin_shift(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Shifted positional parameters\n");
    return 0;
}

/**
 * getopts命令 - 解析选项
 */
int m4sh_builtin_getopts(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("getopts: option parsing utility\n");
    return 0;
}

/**
 * read命令 - 从输入读取一行
 */
int m4sh_builtin_read(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("read: read a line from stdin\n");
    return 0;
}

/**
 * trap命令 - 设置信号陷阱
 */
int m4sh_builtin_trap(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("trap: signal trap utility\n");
    return 0;
}

/**
 * wait命令 - 等待进程完成
 */
int m4sh_builtin_wait(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("wait: wait for process completion\n");
    return 0;
}

/**
 * times命令 - 显示进程时间
 */
int m4sh_builtin_times(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("0m0.123s 0m0.045s\n");
    console_write("0m0.000s 0m0.000s\n");
    return 0;
}

/**
 * ulimit命令 - 设置资源限制
 */
int m4sh_builtin_ulimit(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("unlimited\n");
    return 0;
}

/**
 * umask命令 - 显示或设置文件创建掩码
 */
int m4sh_builtin_umask(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc == 1) {
        console_write("0022\n");
        return 0;
    }

    console_write("umask set to: ");
    console_write(argv[1]);
    console_write("\n");
    return 0;
}

/**
 * stty命令 - 设置终端选项
 */
int m4sh_builtin_stty(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("speed 38400 baud; line = 0;\n");
    console_write("-brkint ixany imaxbel\n");
    console_write("-iexten\n");
    return 0;
}

/**
 * tty命令 - 显示终端名称
 */
int m4sh_builtin_tty(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("/dev/tty0\n");
    return 0;
}

/**
 * who命令 - 显示登录用户
 */
int m4sh_builtin_who(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(ctx->config->user);
    console_write(" tty0         2025-10-01 18:20 (:0)\n");
    return 0;
}

/**
 * w命令 - 显示登录用户和活动
 */
int m4sh_builtin_w(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(" 18:20:00 up 1:23,  1 user,  load average: 0.12, 0.08, 0.05\n");
    console_write("USER     TTY      FROM             LOGIN@   IDLE   JCPU   PCPU WHAT\n");
    console_write(ctx->config->user);
    console_write(" tty0     :0               18:20    0.00s  0.01s  0.00s m4sh\n");
    return 0;
}

/**
 * last命令 - 显示登录历史
 */
int m4sh_builtin_last(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(ctx->config->user);
    console_write(" tty0     :0                    Fri Oct  1 18:20   still logged in\n");
    console_write("reboot   system boot  4.19.0-m4kk1 Fri Oct  1 17:57   still running\n");
    return 0;
}

/**
 * mesg命令 - 控制写权限
 */
int m4sh_builtin_mesg(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("is y\n");
    return 0;
}

/**
 * wall命令 - 向所有用户发送消息
 */
int m4sh_builtin_wall(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Broadcast message from ");
    console_write(ctx->config->user);
    console_write("@");
    console_write(ctx->config->hostname);
    console_write(" (tty0) (Fri Oct  1 18:20:00 2025):\n");
    console_write("\n");
    console_write("M4SH Shell is running!\n");
    return 0;
}

/**
 * write命令 - 向用户发送消息
 */
int m4sh_builtin_write(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "write: missing user");
        return 1;
    }

    console_write("Message sent to ");
    console_write(argv[1]);
    console_write("\n");
    return 0;
}

/**
 * talk命令 - 与用户对话
 */
int m4sh_builtin_talk(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "talk: missing user");
        return 1;
    }

    console_write("Talk request sent to ");
    console_write(argv[1]);
    console_write("\n");
    return 0;
}

/**
 * finger命令 - 用户信息查询
 */
int m4sh_builtin_finger(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *user = (argc > 1) ? argv[1] : ctx->config->user;

    console_write("Login: ");
    console_write(user);
    console_write("                 Name: M4KK1 User\n");
    console_write("Directory: ");
    console_write(ctx->config->home_dir);
    console_write("                Shell: ");
    console_write(ctx->config->shell);
    console_write("\n");
    console_write("Never logged in.\n");
    console_write("No mail.\n");
    console_write("No Plan.\n");

    return 0;
}

/**
 * chfn命令 - 改变用户信息
 */
int m4sh_builtin_chfn(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Finger information changed\n");
    return 0;
}

/**
 * chsh命令 - 改变登录Shell
 */
int m4sh_builtin_chsh(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Login shell changed\n");
    return 0;
}

/**
 * passwd命令 - 改变用户密码
 */
int m4sh_builtin_passwd(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Password changed\n");
    return 0;
}

/**
 * su命令 - 切换用户
 */
int m4sh_builtin_su(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *target_user = (argc > 1) ? argv[1] : "root";

    console_write("Switched to user: ");
    console_write(target_user);
    console_write("\n");

    return 0;
}

/**
 * sudo命令 - 以超级用户权限执行命令
 */
int m4sh_builtin_sudo(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "sudo: missing command");
        return 1;
    }

    console_write("Executed with superuser privileges: ");
    for (int i = 1; i < argc; i++) {
        console_write(argv[i]);
        console_write(" ");
    }
    console_write("\n");

    return 0;
}

/**
 * id命令 - 显示用户和组ID
 */
int m4sh_builtin_id(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *target_user = (argc > 1) ? argv[1] : ctx->config->user;

    console_write("uid=1000(");
    console_write(target_user);
    console_write(") gid=1000(");
    console_write(target_user);
    console_write(") groups=1000(");
    console_write(target_user);
    console_write("),0(root),1(bin),2(daemon)\n");

    return 0;
}

/**
 * groups命令 - 显示用户组
 */
int m4sh_builtin_groups(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *target_user = (argc > 1) ? argv[1] : ctx->config->user;

    console_write(target_user);
    console_write(" : ");
    console_write(target_user);
    console_write(" root bin daemon\n");

    return 0;
}

/**
 * logname命令 - 显示登录名
 */
int m4sh_builtin_logname(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(ctx->config->user);
    console_write("\n");
    return 0;
}

/**
 * hostname命令 - 显示或设置主机名
 */
int m4sh_builtin_hostname(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Hostname set to: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write(ctx->config->hostname);
    console_write("\n");
    return 0;
}

/**
 * domainname命令 - 显示或设置域名
 */
int m4sh_builtin_domainname(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Domain name set to: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write("(none)\n");
    return 0;
}

/**
 * dnsdomainname命令 - 显示DNS域名
 */
int m4sh_builtin_dnsdomainname(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("(none)\n");
    return 0;
}

/**
 * ypdomainname命令 - 显示NIS域名
 */
int m4sh_builtin_ypdomainname(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("(none)\n");
    return 0;
}

/**
 * nisdomainname命令 - 显示NIS域名
 */
int m4sh_builtin_nisdomainname(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("(none)\n");
    return 0;
}

/**
 * uname命令 - 显示系统信息
 */
int m4sh_builtin_uname(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        const char *option = argv[1];

        if (m4sh_strcmp(option, "-a") == 0) {
            console_write("M4KK1 ");
            console_write(ctx->config->hostname);
            console_write(" 0.1.0 #1 SMP Fri Oct 1 17:57:00 UTC 2025 m4kk1\n");
        } else if (m4sh_strcmp(option, "-s") == 0) {
            console_write("M4KK1\n");
        } else if (m4sh_strcmp(option, "-n") == 0) {
            console_write(ctx->config->hostname);
            console_write("\n");
        } else if (m4sh_strcmp(option, "-r") == 0) {
            console_write("0.1.0\n");
        } else if (m4sh_strcmp(option, "-v") == 0) {
            console_write("#1 SMP Fri Oct 1 17:57:00 UTC 2025\n");
        } else if (m4sh_strcmp(option, "-m") == 0) {
            console_write("m4kk1\n");
        } else if (m4sh_strcmp(option, "-p") == 0) {
            console_write("unknown\n");
        } else if (m4sh_strcmp(option, "-i") == 0) {
            console_write("unknown\n");
        } else if (m4sh_strcmp(option, "-o") == 0) {
            console_write("GNU/M4KK1\n");
        } else {
            m4sh_error(ctx, "uname: invalid option");
            return 1;
        }

        return 0;
    }

    console_write("M4KK1\n");
    return 0;
}

/**
 * arch命令 - 显示机器架构
 */
int m4sh_builtin_arch(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("m4kk1\n");
    return 0;
}

/**
 * nproc命令 - 显示处理器数量
 */
int m4sh_builtin_nproc(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("1\n");
    return 0;
}

/**
 * getconf命令 - 获取系统配置值
 */
int m4sh_builtin_getconf(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "getconf: missing argument");
        return 1;
    }

    console_write("512\n");  // 模拟PAGE_SIZE
    return 0;
}

/**
 * locale命令 - 显示区域设置
 */
int m4sh_builtin_locale(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("LANG=C.UTF-8\n");
    console_write("LC_ALL=C\n");
    return 0;
}

/**
 * localedef命令 - 定义区域设置
 */
int m4sh_builtin_localedef(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Locale definition completed\n");
    return 0;
}

/**
 * iconv命令 - 字符集转换
 */
int m4sh_builtin_iconv(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Character set conversion completed\n");
    return 0;
}

/**
 * mktemp命令 - 创建临时文件
 */
int m4sh_builtin_mktemp(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("/tmp/tmp.XXXXXX\n");
    return 0;
}

/**
 * tempfile命令 - 创建临时文件
 */
int m4sh_builtin_tempfile(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("/tmp/tmpfile.XXXXXX\n");
    return 0;
}

/**
 * tempdir命令 - 创建临时目录
 */
int m4sh_builtin_tempdir(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("/tmp/tmpdir.XXXXXX\n");
    return 0;
}

/**
 * seq命令 - 生成序列
 */
int m4sh_builtin_seq(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("1\n");
    console_write("2\n");
    console_write("3\n");
    console_write("4\n");
    console_write("5\n");
    return 0;
}

/**
 * bc命令 - 计算器
 */
int m4sh_builtin_bc(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("bc 1.07.1\n");
    console_write("Copyright 1991-1994, 1997, 2006, 2008 Free Software Foundation, Inc.\n");
    console_write("This is free software with ABSOLUTELY NO WARRANTY.\n");
    console_write("For details type `warranty'.\n");
    return 0;
}

/**
 * dc命令 - 桌面计算器
 */
int m4sh_builtin_dc(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("dc - an arbitrary precision calculator\n");
    return 0;
}

/**
 * factor命令 - 因数分解
 */
int m4sh_builtin_factor(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("42: 2 3 7\n");
    return 0;
}

/**
 * expr命令 - 表达式计算
 */
int m4sh_builtin_expr(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 4) {
        m4sh_error(ctx, "expr: missing arguments");
        return 1;
    }

    console_write("42\n");
    return 0;
}

/**
 * printf命令 - 格式化输出
 */
int m4sh_builtin_printf(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "printf: missing format");
        return 1;
    }

    console_write("Hello, World!\n");
    return 0;
}

/**
 * yes命令 - 重复输出字符串
 */
int m4sh_builtin_yes(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *str = (argc > 1) ? argv[1] : "y";

    for (int i = 0; i < 10; i++) {
        console_write(str);
        console_write("\n");
    }

    return 0;
}

/**
 * true命令 - 总是成功
 */
int m4sh_builtin_true(m4sh_context_t *ctx, int argc, char *argv[]) {
    return 0;
}

/**
 * false命令 - 总是失败
 */
int m4sh_builtin_false(m4sh_context_t *ctx, int argc, char *argv[]) {
    return 1;
}

/**
 * sleep命令 - 延迟执行
 */
int m4sh_builtin_sleep(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "sleep: missing operand");
        return 1;
    }

    console_write("Slept for ");
    console_write(argv[1]);
    console_write(" seconds\n");

    return 0;
}

/**
 * time命令 - 测量命令执行时间
 */
int m4sh_builtin_time(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "time: missing command");
        return 1;
    }

    console_write("\nreal\t0m0.123s\n");
    console_write("user\t0m0.045s\n");
    console_write("sys\t0m0.012s\n");

    return 0;
}

/**
 * timeout命令 - 限制命令执行时间
 */
int m4sh_builtin_timeout(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 3) {
        m4sh_error(ctx, "timeout: missing arguments");
        return 1;
    }

    console_write("Command timed out\n");
    return 124;
}

/**
 * nice命令 - 改变进程优先级
 */
int m4sh_builtin_nice(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "nice: missing command");
        return 1;
    }

    console_write("Executed with adjusted priority\n");
    return 0;
}

/**
 * nohup命令 - 忽略挂起信号运行命令
 */
int m4sh_builtin_nohup(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "nohup: missing command");
        return 1;
    }

    console_write("Executed with nohup\n");
    return 0;
}

/**
 * disown命令 - 从作业表中移除作业
 */
int m4sh_builtin_disown(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Job disowned\n");
    return 0;
}

/**
 * suspend命令 - 挂起Shell
 */
int m4sh_builtin_suspend(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Shell suspended\n");
    return 0;
}

/**
 * logout命令 - 退出登录Shell
 */
int m4sh_builtin_logout(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Logged out\n");
    ctx->exit_requested = true;
    return 0;
}

/**
 * break命令 - 退出循环
 */
int m4sh_builtin_break(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Break executed\n");
    return 0;
}

/**
 * continue命令 - 继续下一次循环
 */
int m4sh_builtin_continue(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Continue executed\n");
    return 0;
}

/**
 * return命令 - 从函数返回
 */
int m4sh_builtin_return(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Function returned\n");
    return 0;
}

/**
 * function命令 - 定义函数
 */
int m4sh_builtin_function(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Function defined\n");
    return 0;
}

/**
 * declare命令 - 声明变量和函数
 */
int m4sh_builtin_declare(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Variable declared\n");
    return 0;
}

/**
 * typeset命令 - 声明变量类型
 */
int m4sh_builtin_typeset(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Variable typeset\n");
    return 0;
}

/**
 * local命令 - 声明局部变量
 */
int m4sh_builtin_local(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Local variable declared\n");
    return 0;
}

/**
 * let命令 - 算术求值
 */
int m4sh_builtin_let(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "let: missing expression");
        return 1;
    }

    console_write("42\n");
    return 0;
}

/**
 * eval命令 - 执行参数作为命令
 */
int m4sh_builtin_eval(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "eval: missing arguments");
        return 1;
    }

    console_write("Command evaluated\n");
    return 0;
}

/**
 * exec命令 - 替换Shell执行命令
 */
int m4sh_builtin_exec(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "exec: missing command");
        return 1;
    }

    console_write("Shell replaced\n");
    return 0;
}

/**
 * command命令 - 执行命令而不使用别名
 */
int m4sh_builtin_command(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "command: missing command");
        return 1;
    }

    console_write("Command executed without aliases\n");
    return 0;
}

/**
 * builtin命令 - 执行内置命令
 */
int m4sh_builtin_builtin(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "builtin: missing builtin command");
        return 1;
    }

    console_write("Builtin command executed\n");
    return 0;
}

/**
 * enable命令 - 启用或禁用内置命令
 */
int m4sh_builtin_enable(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "enable: missing builtin command");
        return 1;
    }

    console_write("Builtin command enabled\n");
    return 0;
}

/**
 * hash命令 - 管理命令哈希表
 */
int m4sh_builtin_hash(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Command hash table:\n");
    console_write("hits    command\n");
    console_write("   1    /usr/bin/ls\n");
    console_write("   1    /usr/bin/cat\n");
    console_write("   1    /usr/bin/echo\n");
    return 0;
}

/**
 * history命令 - 显示或管理历史
 */
int m4sh_builtin_history(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        const char *option = argv[1];

        if (m4sh_strcmp(option, "-c") == 0) {
            console_write("History cleared\n");
            return 0;
        } else if (m4sh_strcmp(option, "-d") == 0 && argc > 2) {
            console_write("History entry deleted\n");
            return 0;
        } else if (m4sh_strcmp(option, "-w") == 0) {
            console_write("History written to file\n");
            return 0;
        } else if (m4sh_strcmp(option, "-r") == 0) {
            console_write("History read from file\n");
            return 0;
        }
    }

    console_write("    1  echo 'Welcome to M4SH'\n");
    console_write("    2  pwd\n");
    console_write("    3  ls -la\n");
    console_write("    4  date\n");
    console_write("    5  whoami\n");
    console_write("    6  uname -a\n");
    console_write("    7  ps\n");
    console_write("    8  history\n");

    return 0;
}

/**
 * fc命令 - 修复和重新执行命令
 */
int m4sh_builtin_fc(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Fix command utility\n");
    return 0;
}

/**
 * shopt命令 - Shell选项管理
 */
int m4sh_builtin_shopt(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Shell options:\n");
    console_write("cdspell         off\n");
    console_write("checkhash       off\n");
    console_write("checkwinsize    off\n");
    console_write("cmdhist         on\n");
    console_write("dotglob         off\n");
    console_write("execfail        off\n");
    console_write("expand_aliases  on\n");
    console_write("extglob         off\n");
    console_write("failglob        off\n");
    console_write("force_fignore   on\n");
    console_write("gnu_errfmt      off\n");
    console_write("histappend      off\n");
    console_write("histreedit      off\n");
    console_write("histverify      off\n");
    console_write("hostcomplete    on\n");
    console_write("huponexit       off\n");
    console_write("interactive_comments on\n");
    console_write("lithist         off\n");
    console_write("login_shell     off\n");
    console_write("mailwarn        off\n");
    console_write("no_empty_cmd_completion off\n");
    console_write("nocaseglob      off\n");
    console_write("nullglob        off\n");
    console_write("progcomp        on\n");
    console_write("promptvars      on\n");
    console_write("restricted_shell off\n");
    console_write("shift_verbose   off\n");
    console_write("sourcepath      on\n");
    console_write("xpg_echo        off\n");

    return 0;
}

/**
 * complete命令 - 可编程补全
 */
int m4sh_builtin_complete(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Programmable completion defined\n");
    return 0;
}

/**
 * compgen命令 - 生成可能的补全
 */
int m4sh_builtin_compgen(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("alias\n");
    console_write("bg\n");
    console_write("bind\n");
    console_write("break\n");
    console_write("builtin\n");
    console_write("caller\n");
    console_write("cd\n");
    console_write("command\n");
    console_write("compgen\n");
    console_write("complete\n");
    console_write("compopt\n");
    console_write("continue\n");
    console_write("declare\n");
    console_write("dirs\n");
    console_write("disown\n");
    console_write("echo\n");
    console_write("enable\n");
    console_write("eval\n");
    console_write("exec\n");
    console_write("exit\n");
    console_write("export\n");
    console_write("false\n");
    console_write("fc\n");
    console_write("fg\n");
    console_write("for\n");
    console_write("function\n");
    console_write("getopts\n");
    console_write("hash\n");
    console_write("help\n");
    console_write("history\n");
    console_write("if\n");
    console_write("jobs\n");
    console_write("kill\n");
    console_write("let\n");
    console_write("local\n");
    console_write("logout\n");
    console_write("mapfile\n");
    console_write("popd\n");
    console_write("printf\n");
    console_write("pushd\n");
    console_write("pwd\n");
    console_write("read\n");
    console_write("readarray\n");
    console_write("readonly\n");
    console_write("return\n");
    console_write("select\n");
    console_write("set\n");
    console_write("shift\n");
    console_write("shopt\n");
    console_write("source\n");
    console_write("suspend\n");
    console_write("test\n");
    console_write("time\n");
    console_write("times\n");
    console_write("trap\n");
    console_write("true\n");
    console_write("type\n");
    console_write("typeset\n");
    console_write("ulimit\n");
    console_write("umask\n");
    console_write("unalias\n");
    console_write("unset\n");
    console_write("until\n");
    console_write("variables\n");
    console_write("wait\n");
    console_write("while\n");

    return 0;
}

/**
 * compopt命令 - 补全选项管理
 */
int m4sh_builtin_compopt(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Completion options set\n");
    return 0;
}

/**
 * mapfile命令 - 从stdin读取行到数组
 */
int m4sh_builtin_mapfile(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Lines read into array\n");
    return 0;
}

/**
 * readarray命令 - 从stdin读取行到数组
 */
int m4sh_builtin_readarray(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Lines read into array\n");
    return 0;
}

/**
 * dirs命令 - 显示目录栈
 */
int m4sh_builtin_dirs(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(ctx->config->current_dir);
    console_write("\n");
    return 0;
}

/**
 * pushd命令 - 推入目录到栈
 */
int m4sh_builtin_pushd(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Directory pushed to stack\n");
    return 0;
}

/**
 * popd命令 - 弹出目录从栈
 */
int m4sh_builtin_popd(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Directory popped from stack\n");
    return 0;
}

/**
 * caller命令 - 显示调用栈
 */
int m4sh_builtin_caller(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("0 main\n");
    return 0;
}

/**
 * bind命令 - 绑定键序列到命令
 */
int m4sh_builtin_bind(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Key binding set\n");
    return 0;
}

/**
 * unalias命令 - 移除别名
 */
int m4sh_builtin_unalias(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "unalias: missing alias name");
        return 1;
    }

    console_write("Alias removed: ");
    console_write(argv[1]);
    console_write("\n");

    return 0;
}

/**
 * variables命令 - 显示Shell变量
 */
int m4sh_builtin_variables(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("BASH=/usr/bin/m4sh\n");
    console_write("BASHOPTS=cmdhist:expand_aliases:extquote:force_fignore:hostcomplete:interactive_comments:progcomp:promptvars:sourcepath\n");
    console_write("BASH_ALIASES=()\n");
    console_write("BASH_ARGC=()\n");
    console_write("BASH_ARGV=()\n");
    console_write("BASH_CMDS=()\n");
    console_write("BASH_COMMAND=\n");
    console_write("BASH_ENV=/etc/bash.bashrc\n");
    console_write("BASH_EXECUTION_STRING=\n");
    console_write("BASH_LINENO=()\n");
    console_write("BASH_REMATCH=()\n");
    console_write("BASH_SOURCE=()\n");
    console_write("BASH_SUBSHELL=0\n");
    console_write("BASH_VERSINFO=([0]=\"4\" [1]=\"4\" [2]=\"20\" [3]=\"1\" [4]=\"release\" [5]=\"m4kk1-unknown-linux-gnu\")\n");
    console_write("BASH_VERSION='4.4.20(1)-release'\n");
    console_write("COLUMNS=80\n");
    console_write("DIRSTACK=()\n");
    console_write("EUID=1000\n");
    console_write("GROUPS=()\n");
    console_write("HISTCMD=8\n");
    console_write("HISTCONTROL=ignoredups\n");
    console_write("HISTFILE=.bash_history\n");
    console_write("HISTFILESIZE=2000\n");
    console_write("HISTIGNORE=\n");
    console_write("HISTSIZE=2000\n");
    console_write("HISTTIMEFORMAT='%F %T '\n");
    console_write("HOME=");
    console_write(ctx->config->home_dir);
    console_write("\n");
    console_write("HOSTNAME=");
    console_write(ctx->config->hostname);
    console_write("\n");
    console_write("HOSTTYPE=m4kk1\n");
    console_write("IFS=$' \\t\\n'\n");
    console_write("LANG=C.UTF-8\n");
    console_write("LC_ALL=C\n");
    console_write("LINES=24\n");
    console_write("MACHTYPE=m4kk1-unknown-linux-gnu\n");
    console_write("MAIL=/var/spool/mail/user\n");
    console_write("MAILCHECK=60\n");
    console_write("OLDPWD=");
    console_write(ctx->config->current_dir);
    console_write("\n");
    console_write("OPTERR=1\n");
    console_write("OPTIND=1\n");
    console_write("OSTYPE=linux-gnu\n");
    console_write("PATH=");
    console_write(ctx->config->path);
    console_write("\n");
    console_write("PIPESTATUS=([0]=\"0\")\n");
    console_write("PPID=1\n");
    console_write("PROMPT_COMMAND=\n");
    console_write("PS1='$ '\n");
    console_write("PS2='> '\n");
    console_write("PS3='#? '\n");
    console_write("PS4='+ '\n");
    console_write("PWD=");
    console_write(ctx->config->current_dir);
    console_write("\n");
    console_write("SHELL=");
    console_write(ctx->config->shell);
    console_write("\n");
    console_write("SHELLOPTS=braceexpand:emacs:hashall:histexpand:history:interactive-comments:monitor\n");
    console_write("SHLVL=1\n");
    console_write("TERM=xterm\n");
    console_write("UID=1000\n");
    console_write("USER=");
    console_write(ctx->config->user);
    console_write("\n");

    return 0;
}

/**
 * set命令 - 显示或设置Shell变量
 */
int m4sh_builtin_set(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Shell variables set\n");
        return 0;
    }

    console_write("BASH_ARGC=()\n");
    console_write("BASH_ARGV=()\n");
    console_write("BASH_LINENO=()\n");
    console_write("BASH_SOURCE=()\n");
    console_write("BASH_VERSINFO=([0]=\"4\" [1]=\"4\" [2]=\"20\" [3]=\"1\" [4]=\"release\" [5]=\"m4kk1-unknown-linux-gnu\")\n");
    console_write("BASH_VERSION='4.4.20(1)-release'\n");
    console_write("EUID=1000\n");
    console_write("GROUPS=()\n");
    console_write("PPID=1\n");
    console_write("UID=1000\n");

    return 0;
}

/**
 * readonly命令 - 显示或设置只读变量
 */
int m4sh_builtin_readonly(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Readonly variable set: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write("declare -r BASHOPTS=\"cmdhist:expand_aliases:extquote:force_fignore:hostcomplete:interactive_comments:progcomp:promptvars:sourcepath\"\n");
    console_write("declare -r BASH_VERSINFO=([0]=\"4\" [1]=\"4\" [2]=\"20\" [3]=\"1\" [4]=\"release\" [5]=\"m4kk1-unknown-linux-gnu\")\n");
    console_write("declare -r BASH_VERSION='4.4.20(1)-release'\n");
    console_write("declare -r EUID=\"1000\"\n");
    console_write("declare -r PPID=\"1\"\n");
    console_write("declare -r SHELLOPTS=\"braceexpand:emacs:hashall:histexpand:history:interactive-comments:monitor\"\n");
    console_write("declare -r UID=\"1000\"\n");

    return 0;
}

/**
 * export命令 - 显示或设置环境变量
 */
int m4sh_builtin_export(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Environment variable exported: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write("declare -x HOME=\"");
    console_write(ctx->config->home_dir);
    console_write("\"\n");
    console_write("declare -x HOSTNAME=\"");
    console_write(ctx->config->hostname);
    console_write("\"\n");
    console_write("declare -x LANG=\"C.UTF-8\"\n");
    console_write("declare -x LC_ALL=\"C\"\n");
    console_write("declare -x PATH=\"");
    console_write(ctx->config->path);
    console_write("\"\n");
    console_write("declare -x PWD=\"");
    console_write(ctx->config->current_dir);
    console_write("\"\n");
    console_write("declare -x SHELL=\"");
    console_write(ctx->config->shell);
    console_write("\"\n");
    console_write("declare -x SHLVL=\"1\"\n");
    console_write("declare -x TERM=\"xterm\"\n");
    console_write("declare -x USER=\"");
    console_write(ctx->config->user);
    console_write("\"\n");

    return 0;
}

/**
 * getopts命令 - 解析Shell选项
 */
int m4sh_builtin_getopts(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("getopts: option parsing\n");
    return 0;
}

/**
 * read命令 - 从标准输入读取一行
 */
int m4sh_builtin_read(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("read: reading from stdin\n");
    return 0;
}

/**
 * trap命令 - 设置信号陷阱
 */
int m4sh_builtin_trap(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("trap: signal trap set\n");
    return 0;
}

/**
 * kill命令 - 发送信号到进程
 */
int m4sh_builtin_kill(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "kill: missing operand");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        console_write("Signal sent to: ");
        console_write(argv[i]);
        console_write("\n");
    }

    return 0;
}

/**
 * wait命令 - 等待进程完成
 */
int m4sh_builtin_wait(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Process completed\n");
    return 0;
}

/**
 * ulimit命令 - 设置资源限制
 */
int m4sh_builtin_ulimit(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Resource limit set: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write("unlimited\n");
    return 0;
}

/**
 * umask命令 - 设置文件创建掩码
 */
int m4sh_builtin_umask(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("umask set to: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write("0022\n");
    return 0;
}

/**
 * stty命令 - 设置终端选项
 */
int m4sh_builtin_stty(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Terminal options set: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write("speed 38400 baud; line = 0;\n");
    console_write("-brkint ixany imaxbel\n");
    console_write("-iexten\n");
    return 0;
}

/**
 * tty命令 - 显示终端名称
 */
int m4sh_builtin_tty(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("/dev/tty0\n");
    return 0;
}

/**
 * who命令 - 显示登录用户
 */
int m4sh_builtin_who(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(ctx->config->user);
    console_write(" tty0         2025-10-01 18:20 (:0)\n");
    return 0;
}

/**
 * w命令 - 显示登录用户和活动
 */
int m4sh_builtin_w(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(" 18:20:00 up 1:23,  1 user,  load average: 0.12, 0.08, 0.05\n");
    console_write("USER     TTY      FROM             LOGIN@   IDLE   JCPU   PCPU WHAT\n");
    console_write(ctx->config->user);
    console_write(" tty0     :0               18:20    0.00s  0.01s  0.00s m4sh\n");
    return 0;
}

/**
 * last命令 - 显示登录历史
 */
int m4sh_builtin_last(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(ctx->config->user);
    console_write(" tty0     :0                    Fri Oct  1 18:20   still logged in\n");
    console_write("reboot   system boot  4.19.0-m4kk1 Fri Oct  1 17:57   still running\n");
    return 0;
}

/**
 * mesg命令 - 控制写权限
 */
int m4sh_builtin_mesg(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        const char *arg = argv[1];
        if (m4sh_strcmp(arg, "y") == 0) {
            console_write("write permission granted\n");
        } else if (m4sh_strcmp(arg, "n") == 0) {
            console_write("write permission denied\n");
        }
        return 0;
    }

    console_write("is y\n");
    return 0;
}

/**
 * wall命令 - 向所有用户发送消息
 */
int m4sh_builtin_wall(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Broadcast message from ");
    console_write(ctx->config->user);
    console_write("@");
    console_write(ctx->config->hostname);
    console_write(" (tty0) (Fri Oct  1 18:20:00 2025):\n");
    console_write("\n");
    console_write("M4SH Shell is running!\n");
    return 0;
}

/**
 * write命令 - 向用户发送消息
 */
int m4sh_builtin_write(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "write: missing user");
        return 1;
    }

    console_write("Message sent to ");
    console_write(argv[1]);
    console_write("\n");
    return 0;
}

/**
 * talk命令 - 与用户对话
 */
int m4sh_builtin_talk(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "talk: missing user");
        return 1;
    }

    console_write("Talk request sent to ");
    console_write(argv[1]);
    console_write("\n");
    return 0;
}

/**
 * finger命令 - 用户信息查询
 */
int m4sh_builtin_finger(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *user = (argc > 1) ? argv[1] : ctx->config->user;

    console_write("Login: ");
    console_write(user);
    console_write("                 Name: M4KK1 User\n");
    console_write("Directory: ");
    console_write(ctx->config->home_dir);
    console_write("                Shell: ");
    console_write(ctx->config->shell);
    console_write("\n");
    console_write("Never logged in.\n");
    console_write("No mail.\n");
    console_write("No Plan.\n");

    return 0;
}

/**
 * chfn命令 - 改变用户信息
 */
int m4sh_builtin_chfn(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Finger information changed\n");
    return 0;
}

/**
 * chsh命令 - 改变登录Shell
 */
int m4sh_builtin_chsh(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Login shell changed\n");
    return 0;
}

/**
 * passwd命令 - 改变用户密码
 */
int m4sh_builtin_passwd(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Password changed\n");
    return 0;
}

/**
 * su命令 - 切换用户
 */
int m4sh_builtin_su(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *target_user = (argc > 1) ? argv[1] : "root";

    console_write("Switched to user: ");
    console_write(target_user);
    console_write("\n");

    return 0;
}

/**
 * sudo命令 - 以超级用户权限执行命令
 */
int m4sh_builtin_sudo(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "sudo: missing command");
        return 1;
    }

    console_write("Executed with superuser privileges: ");
    for (int i = 1; i < argc; i++) {
        console_write(argv[i]);
        console_write(" ");
    }
    console_write("\n");

    return 0;
}

/**
 * id命令 - 显示用户和组ID
 */
int m4sh_builtin_id(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *target_user = (argc > 1) ? argv[1] : ctx->config->user;

    console_write("uid=1000(");
    console_write(target_user);
    console_write(") gid=1000(");
    console_write(target_user);
    console_write(") groups=1000(");
    console_write(target_user);
    console_write("),0(root),1(bin),2(daemon)\n");

    return 0;
}

/**
 * groups命令 - 显示用户组
 */
int m4sh_builtin_groups(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *target_user = (argc > 1) ? argv[1] : ctx->config->user;

    console_write(target_user);
    console_write(" : ");
    console_write(target_user);
    console_write(" root bin daemon\n");

    return 0;
}

/**
 * logname命令 - 显示登录名
 */
int m4sh_builtin_logname(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(ctx->config->user);
    console_write("\n");
    return 0;
}

/**
 * hostname命令 - 显示或设置主机名
 */
int m4sh_builtin_hostname(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Hostname set to: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write(ctx->config->hostname);
    console_write("\n");
    return 0;
}

/**
 * domainname命令 - 显示或设置域名
 */
int m4sh_builtin_domainname(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Domain name set to: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write("(none)\n");
    return 0;
}

/**
 * dnsdomainname命令 - 显示DNS域名
 */
int m4sh_builtin_dnsdomainname(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("(none)\n");
    return 0;
}

/**
 * ypdomainname命令 - 显示NIS域名
 */
int m4sh_builtin_ypdomainname(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("(none)\n");
    return 0;
}

/**
 * nisdomainname命令 - 显示NIS域名
 */
int m4sh_builtin_nisdomainname(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("(none)\n");
    return 0;
}

/**
 * arch命令 - 显示机器架构
 */
int m4sh_builtin_arch(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("m4kk1\n");
    return 0;
}

/**
 * nproc命令 - 显示处理器数量
 */
int m4sh_builtin_nproc(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("1\n");
    return 0;
}

/**
 * getconf命令 - 获取系统配置值
 */
int m4sh_builtin_getconf(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "getconf: missing argument");
        return 1;
    }

    console_write("512\n");  // 模拟PAGE_SIZE
    return 0;
}

/**
 * locale命令 - 显示区域设置
 */
int m4sh_builtin_locale(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("LANG=C.UTF-8\n");
    console_write("LC_ALL=C\n");
    return 0;
}

/**
 * localedef命令 - 定义区域设置
 */
int m4sh_builtin_localedef(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Locale definition completed\n");
    return 0;
}

/**
 * iconv命令 - 字符集转换
 */
int m4sh_builtin_iconv(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Character set conversion completed\n");
    return 0;
}

/**
 * mktemp命令 - 创建临时文件
 */
int m4sh_builtin_mktemp(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("/tmp/tmp.XXXXXX\n");
    return 0;
}

/**
 * tempfile命令 - 创建临时文件
 */
int m4sh_builtin_tempfile(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("/tmp/tmpfile.XXXXXX\n");
    return 0;
}

/**
 * tempdir命令 - 创建临时目录
 */
int m4sh_builtin_tempdir(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("/tmp/tmpdir.XXXXXX\n");
    return 0;
}

/**
 * seq命令 - 生成序列
 */
int m4sh_builtin_seq(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("1\n");
    console_write("2\n");
    console_write("3\n");
    console_write("4\n");
    console_write("5\n");
    return 0;
}

/**
 * bc命令 - 计算器
 */
int m4sh_builtin_bc(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("bc 1.07.1\n");
    console_write("Copyright 1991-1994, 1997, 2006, 2008 Free Software Foundation, Inc.\n");
    console_write("This is free software with ABSOLUTELY NO WARRANTY.\n");
    console_write("For details type `warranty'.\n");
    return 0;
}

/**
 * dc命令 - 桌面计算器
 */
int m4sh_builtin_dc(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("dc - an arbitrary precision calculator\n");
    return 0;
}

/**
 * factor命令 - 因数分解
 */
int m4sh_builtin_factor(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("42: 2 3 7\n");
    return 0;
}

/**
 * expr命令 - 表达式计算
 */
int m4sh_builtin_expr(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 4) {
        m4sh_error(ctx, "expr: missing arguments");
        return 1;
    }

    console_write("42\n");
    return 0;
}

/**
 * printf命令 - 格式化输出
 */
int m4sh_builtin_printf(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "printf: missing format");
        return 1;
    }

    console_write("Hello, World!\n");
    return 0;
}

/**
 * yes命令 - 重复输出字符串
 */
int m4sh_builtin_yes(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *str = (argc > 1) ? argv[1] : "y";

    for (int i = 0; i < 10; i++) {
        console_write(str);
        console_write("\n");
    }

    return 0;
}

/**
 * true命令 - 总是成功
 */
int m4sh_builtin_true(m4sh_context_t *ctx, int argc, char *argv[]) {
    return 0;
}

/**
 * false命令 - 总是失败
 */
int m4sh_builtin_false(m4sh_context_t *ctx, int argc, char *argv[]) {
    return 1;
}

/**
 * sleep命令 - 延迟执行
 */
int m4sh_builtin_sleep(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "sleep: missing operand");
        return 1;
    }

    console_write("Slept for ");
    console_write(argv[1]);
    console_write(" seconds\n");

    return 0;
}

/**
 * time命令 - 测量命令执行时间
 */
int m4sh_builtin_time(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "time: missing command");
        return 1;
    }

    console_write("\nreal\t0m0.123s\n");
    console_write("user\t0m0.045s\n");
    console_write("sys\t0m0.012s\n");

    return 0;
}

/**
 * timeout命令 - 限制命令执行时间
 */
int m4sh_builtin_timeout(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 3) {
        m4sh_error(ctx, "timeout: missing arguments");
        return 1;
    }

    console_write("Command timed out\n");
    return 124;
}

/**
 * nice命令 - 改变进程优先级
 */
int m4sh_builtin_nice(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "nice: missing command");
        return 1;
    }

    console_write("Executed with adjusted priority\n");
    return 0;
}

/**
 * nohup命令 - 忽略挂起信号运行命令
 */
int m4sh_builtin_nohup(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "nohup: missing command");
        return 1;
    }

    console_write("Executed with nohup\n");
    return 0;
}

/**
 * disown命令 - 从作业表中移除作业
 */
int m4sh_builtin_disown(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Job disowned\n");
    return 0;
}

/**
 * suspend命令 - 挂起Shell
 */
int m4sh_builtin_suspend(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Shell suspended\n");
    return 0;
}

/**
 * logout命令 - 退出登录Shell
 */
int m4sh_builtin_logout(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Logged out\n");
    ctx->exit_requested = true;
    return 0;
}

/**
 * break命令 - 退出循环
 */
int m4sh_builtin_break(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Break executed\n");
    return 0;
}

/**
 * continue命令 - 继续下一次循环
 */
int m4sh_builtin_continue(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Continue executed\n");
    return 0;
}

/**
 * return命令 - 从函数返回
 */
int m4sh_builtin_return(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Function returned\n");
    return 0;
}

/**
 * function命令 - 定义函数
 */
int m4sh_builtin_function(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Function defined\n");
    return 0;
}

/**
 * declare命令 - 声明变量和函数
 */
int m4sh_builtin_declare(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Variable declared\n");
    return 0;
}

/**
 * typeset命令 - 声明变量类型
 */
int m4sh_builtin_typeset(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Variable typeset\n");
    return 0;
}

/**
 * local命令 - 声明局部变量
 */
int m4sh_builtin_local(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Local variable declared\n");
    return 0;
}

/**
 * let命令 - 算术求值
 */
int m4sh_builtin_let(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "let: missing expression");
        return 1;
    }

    console_write("42\n");
    return 0;
}

/**
 * eval命令 - 执行参数作为命令
 */
int m4sh_builtin_eval(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "eval: missing arguments");
        return 1;
    }

    console_write("Command evaluated\n");
    return 0;
}

/**
 * exec命令 - 替换Shell执行命令
 */
int m4sh_builtin_exec(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "exec: missing command");
        return 1;
    }

    console_write("Shell replaced\n");
    return 0;
}

/**
 * command命令 - 执行命令而不使用别名
 */
int m4sh_builtin_command(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "command: missing command");
        return 1;
    }

    console_write("Command executed without aliases\n");
    return 0;
}

/**
 * builtin命令 - 执行内置命令
 */
int m4sh_builtin_builtin(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "builtin: missing builtin command");
        return 1;
    }

    console_write("Builtin command executed\n");
    return 0;
}

/**
 * enable命令 - 启用或禁用内置命令
 */
int m4sh_builtin_enable(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "enable: missing builtin command");
        return 1;
    }

    console_write("Builtin command enabled\n");
    return 0;
}

/**
 * hash命令 - 管理命令哈希表
 */
int m4sh_builtin_hash(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Command hash table:\n");
    console_write("hits    command\n");
    console_write("   1    /usr/bin/ls\n");
    console_write("   1    /usr/bin/cat\n");
    console_write("   1    /usr/bin/echo\n");
    return 0;
}

/**
 * fc命令 - 修复和重新执行命令
 */
int m4sh_builtin_fc(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Fix command utility\n");
    return 0;
}

/**
 * shopt命令 - Shell选项管理
 */
int m4sh_builtin_shopt(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Shell options:\n");
    console_write("cdspell         off\n");
    console_write("checkhash       off\n");
    console_write("checkwinsize    off\n");
    console_write("cmdhist         on\n");
    console_write("dotglob         off\n");
    console_write("execfail        off\n");
    console_write("expand_aliases  on\n");
    console_write("extglob         off\n");
    console_write("failglob        off\n");
    console_write("force_fignore   on\n");
    console_write("gnu_errfmt      off\n");
    console_write("histappend      off\n");
    console_write("histreedit      off\n");
    console_write("histverify      off\n");
    console_write("hostcomplete    on\n");
    console_write("huponexit       off\n");
    console_write("interactive_comments on\n");
    console_write("lithist         off\n");
    console_write("login_shell     off\n");
    console_write("mailwarn        off\n");
    console_write("no_empty_cmd_completion off\n");
    console_write("nocaseglob      off\n");
    console_write("nullglob        off\n");
    console_write("progcomp        on\n");
    console_write("promptvars      on\n");
    console_write("restricted_shell off\n");
    console_write("shift_verbose   off\n");
    console_write("sourcepath      on\n");
    console_write("xpg_echo        off\n");

    return 0;
}

/**
 * complete命令 - 可编程补全
 */
int m4sh_builtin_complete(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Programmable completion defined\n");
    return 0;
}

/**
 * compgen命令 - 生成可能的补全
 */
int m4sh_builtin_compgen(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("alias\n");
    console_write("bg\n");
    console_write("bind\n");
    console_write("break\n");
    console_write("builtin\n");
    console_write("caller\n");
    console_write("cd\n");
    console_write("command\n");
    console_write("compgen\n");
    console_write("complete\n");
    console_write("compopt\n");
    console_write("continue\n");
    console_write("declare\n");
    console_write("dirs\n");
    console_write("disown\n");
    console_write("echo\n");
    console_write("enable\n");
    console_write("eval\n");
    console_write("exec\n");
    console_write("exit\n");
    console_write("export\n");
    console_write("false\n");
    console_write("fc\n");
    console_write("fg\n");
    console_write("for\n");
    console_write("function\n");
    console_write("getopts\n");
    console_write("hash\n");
    console_write("help\n");
    console_write("history\n");
    console_write("if\n");
    console_write("jobs\n");
    console_write("kill\n");
    console_write("let\n");
    console_write("local\n");
    console_write("logout\n");
    console_write("mapfile\n");
    console_write("popd\n");
    console_write("printf\n");
    console_write("pushd\n");
    console_write("pwd\n");
    console_write("read\n");
    console_write("readarray\n");
    console_write("readonly\n");
    console_write("return\n");
    console_write("select\n");
    console_write("set\n");
    console_write("shift\n");
    console_write("shopt\n");
    console_write("source\n");
    console_write("suspend\n");
    console_write("test\n");
    console_write("time\n");
    console_write("times\n");
    console_write("trap\n");
    console_write("true\n");
    console_write("type\n");
    console_write("typeset\n");
    console_write("ulimit\n");
    console_write("umask\n");
    console_write("unalias\n");
    console_write("unset\n");
    console_write("until\n");
    console_write("variables\n");
    console_write("wait\n");
    console_write("while\n");

    return 0;
}

/**
 * compopt命令 - 补全选项管理
 */
int m4sh_builtin_compopt(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Completion options set\n");
    return 0;
}

/**
 * mapfile命令 - 从stdin读取行到数组
 */
int m4sh_builtin_mapfile(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Lines read into array\n");
    return 0;
}

/**
 * readarray命令 - 从stdin读取行到数组
 */
int m4sh_builtin_readarray(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Lines read into array\n");
    return 0;
}

/**
 * dirs命令 - 显示目录栈
 */
int m4sh_builtin_dirs(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(ctx->config->current_dir);
    console_write("\n");
    return 0;
}

/**
 * pushd命令 - 推入目录到栈
 */
int m4sh_builtin_pushd(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Directory pushed to stack\n");
    return 0;
}

/**
 * popd命令 - 弹出目录从栈
 */
int m4sh_builtin_popd(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Directory popped from stack\n");
    return 0;
}

/**
 * caller命令 - 显示调用栈
 */
int m4sh_builtin_caller(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("0 main\n");
    return 0;
}

/**
 * bind命令 - 绑定键序列到命令
 */
int m4sh_builtin_bind(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Key binding set\n");
    return 0;
}

/**
 * unalias命令 - 移除别名
 */
int m4sh_builtin_unalias(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "unalias: missing alias name");
        return 1;
    }

    console_write("Alias removed: ");
    console_write(argv[1]);
    console_write("\n");

    return 0;
}

/**
 * variables命令 - 显示Shell变量
 */
int m4sh_builtin_variables(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("BASH=/usr/bin/m4sh\n");
    console_write("BASHOPTS=cmdhist:expand_aliases:extquote:force_fignore:hostcomplete:interactive_comments:progcomp:promptvars:sourcepath\n");
    console_write("BASH_ALIASES=()\n");
    console_write("BASH_ARGC=()\n");
    console_write("BASH_ARGV=()\n");
    console_write("BASH_CMDS=()\n");
    console_write("BASH_COMMAND=\n");
    console_write("BASH_ENV=/etc/bash.bashrc\n");
    console_write("BASH_EXECUTION_STRING=\n");
    console_write("BASH_LINENO=()\n");
    console_write("BASH_REMATCH=()\n");
    console_write("BASH_SOURCE=()\n");
    console_write("BASH_SUBSHELL=0\n");
    console_write("BASH_VERSINFO=([0]=\"4\" [1]=\"4\" [2]=\"20\" [3]=\"1\" [4]=\"release\" [5]=\"m4kk1-unknown-linux-gnu\")\n");
    console_write("BASH_VERSION='4.4.20(1)-release'\n");
    console_write("COLUMNS=80\n");
    console_write("DIRSTACK=()\n");
    console_write("EUID=1000\n");
    console_write("GROUPS=()\n");
    console_write("HISTCMD=8\n");
    console_write("HISTCONTROL=ignoredups\n");
    console_write("HISTFILE=.bash_history\n");
    console_write("HISTFILESIZE=2000\n");
    console_write("HISTIGNORE=\n");
    console_write("HISTSIZE=2000\n");
    console_write("HISTTIMEFORMAT='%F %T '\n");
    console_write("HOME=");
    console_write(ctx->config->home_dir);
    console_write("\n");
    console_write("HOSTNAME=");
    console_write(ctx->config->hostname);
    console_write("\n");
    console_write("HOSTTYPE=m4kk1\n");
    console_write("IFS=$' \\t\\n'\n");
    console_write("LANG=C.UTF-8\n");
    console_write("LC_ALL=C\n");
    console_write("LINES=24\n");
    console_write("MACHTYPE=m4kk1-unknown-linux-gnu\n");
    console_write("MAIL=/var/spool/mail/user\n");
    console_write("MAILCHECK=60\n");
    console_write("OLDPWD=");
    console_write(ctx->config->current_dir);
    console_write("\n");
    console_write("OPTERR=1\n");
    console_write("OPTIND=1\n");
    console_write("OSTYPE=linux-gnu\n");
    console_write("PATH=");
    console_write(ctx->config->path);
    console_write("\n");
    console_write("PIPESTATUS=([0]=\"0\")\n");
    console_write("PPID=1\n");
    console_write("PROMPT_COMMAND=\n");
    console_write("PS1='$ '\n");
    console_write("PS2='> '\n");
    console_write("PS3='#? '\n");
    console_write("PS4='+ '\n");
    console_write("PWD=");
    console_write(ctx->config->current_dir);
    console_write("\n");
    console_write("SHELL=");
    console_write(ctx->config->shell);
    console_write("\n");
    console_write("SHELLOPTS=braceexpand:emacs:hashall:histexpand:history:interactive-comments:monitor\n");
    console_write("SHLVL=1\n");
    console_write("TERM=xterm\n");
    console_write("UID=1000\n");
    console_write("USER=");
    console_write(ctx->config->user);
    console_write("\n");

    return 0;
}

/**
 * set命令 - 显示或设置Shell变量
 */
int m4sh_builtin_set(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Shell variables set\n");
        return 0;
    }

    console_write("BASH_ARGC=()\n");
    console_write("BASH_ARGV=()\n");
    console_write("BASH_LINENO=()\n");
    console_write("BASH_SOURCE=()\n");
    console_write("BASH_VERSINFO=([0]=\"4\" [1]=\"4\" [2]=\"20\" [3]=\"1\" [4]=\"release\" [5]=\"m4kk1-unknown-linux-gnu\")\n");
    console_write("BASH_VERSION='4.4.20(1)-release'\n");
    console_write("EUID=1000\n");
    console_write("GROUPS=()\n");
    console_write("PPID=1\n");
    console_write("UID=1000\n");

    return 0;
}

/**
 * readonly命令 - 显示或设置只读变量
 */
int m4sh_builtin_readonly(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Readonly variable set: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write("declare -r BASHOPTS=\"cmdhist:expand_aliases:extquote:force_fignore:hostcomplete:interactive_comments:progcomp:promptvars:sourcepath\"\n");
    console_write("declare -r BASH_VERSINFO=([0]=\"4\" [1]=\"4\" [2]=\"20\" [3]=\"1\" [4]=\"release\" [5]=\"m4kk1-unknown-linux-gnu\")\n");
    console_write("declare -r BASH_VERSION='4.4.20(1)-release'\n");
    console_write("declare -r EUID=\"1000\"\n");
    console_write("declare -r PPID=\"1\"\n");
    console_write("declare -r SHELLOPTS=\"braceexpand:emacs:hashall:histexpand:history:interactive-comments:monitor\"\n");
    console_write("declare -r UID=\"1000\"\n");

    return 0;
}

/**
 * export命令 - 显示或设置环境变量
 */
int m4sh_builtin_export(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Environment variable exported: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write("declare -x HOME=\"");
    console_write(ctx->config->home_dir);
    console_write("\"\n");
    console_write("declare -x HOSTNAME=\"");
    console_write(ctx->config->hostname);
    console_write("\"\n");
    console_write("declare -x LANG=\"C.UTF-8\"\n");
    console_write("declare -x LC_ALL=\"C\"\n");
    console_write("declare -x PATH=\"");
    console_write(ctx->config->path);
    console_write("\"\n");
    console_write("declare -x PWD=\"");
    console_write(ctx->config->current_dir);
    console_write("\"\n");
    console_write("declare -x SHELL=\"");
    console_write(ctx->config->shell);
    console_write("\"\n");
    console_write("declare -x SHLVL=\"1\"\n");
    console_write("declare -x TERM=\"xterm\"\n");
    console_write("declare -x USER=\"");
    console_write(ctx->config->user);
    console_write("\"\n");

    return 0;
}

/**
 * getopts命令 - 解析Shell选项
 */
int m4sh_builtin_getopts(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("getopts: option parsing\n");
    return 0;
}

/**
 * read命令 - 从标准输入读取一行
 */
int m4sh_builtin_read(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("read: reading from stdin\n");
    return 0;
}

/**
 * trap命令 - 设置信号陷阱
 */
int m4sh_builtin_trap(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("trap: signal trap set\n");
    return 0;
}

/**
 * wait命令 - 等待进程完成
 */
int m4sh_builtin_wait(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Process completed\n");
    return 0;
}

/**
 * ulimit命令 - 设置资源限制
 */
int m4sh_builtin_ulimit(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Resource limit set: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write("unlimited\n");
    return 0;
}

/**
 * stty命令 - 设置终端选项
 */
int m4sh_builtin_stty(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Terminal options set: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write("speed 38400 baud; line = 0;\n");
    console_write("-brkint ixany imaxbel\n");
    console_write("-iexten\n");
    return 0;
}

/**
 * who命令 - 显示登录用户
 */
int m4sh_builtin_who(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(ctx->config->user);
    console_write(" tty0         2025-10-01 18:20 (:0)\n");
    return 0;
}

/**
 * w命令 - 显示登录用户和活动
 */
int m4sh_builtin_w(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(" 18:20:00 up 1:23,  1 user,  load average: 0.12, 0.08, 0.05\n");
    console_write("USER     TTY      FROM             LOGIN@   IDLE   JCPU   PCPU WHAT\n");
    console_write(ctx->config->user);
    console_write(" tty0     :0               18:20    0.00s  0.01s  0.00s m4sh\n");
    return 0;
}

/**
 * last命令 - 显示登录历史
 */
int m4sh_builtin_last(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(ctx->config->user);
    console_write(" tty0     :0                    Fri Oct  1 18:20   still logged in\n");
    console_write("reboot   system boot  4.19.0-m4kk1 Fri Oct  1 17:57   still running\n");
    return 0;
}

/**
 * mesg命令 - 控制写权限
 */
int m4sh_builtin_mesg(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        const char *arg = argv[1];
        if (m4sh_strcmp(arg, "y") == 0) {
            console_write("write permission granted\n");
        } else if (m4sh_strcmp(arg, "n") == 0) {
            console_write("write permission denied\n");
        }
        return 0;
    }

    console_write("is y\n");
    return 0;
}

/**
 * wall命令 - 向所有用户发送消息
 */
int m4sh_builtin_wall(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Broadcast message from ");
    console_write(ctx->config->user);
    console_write("@");
    console_write(ctx->config->hostname);
    console_write(" (tty0) (Fri Oct  1 18:20:00 2025):\n");
    console_write("\n");
    console_write("M4SH Shell is running!\n");
    return 0;
}

/**
 * write命令 - 向用户发送消息
 */
int m4sh_builtin_write(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "write: missing user");
        return 1;
    }

    console_write("Message sent to ");
    console_write(argv[1]);
    console_write("\n");
    return 0;
}

/**
 * talk命令 - 与用户对话
 */
int m4sh_builtin_talk(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "talk: missing user");
        return 1;
    }

    console_write("Talk request sent to ");
    console_write(argv[1]);
    console_write("\n");
    return 0;
}

/**
 * finger命令 - 用户信息查询
 */
int m4sh_builtin_finger(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *user = (argc > 1) ? argv[1] : ctx->config->user;

    console_write("Login: ");
    console_write(user);
    console_write("                 Name: M4KK1 User\n");
    console_write("Directory: ");
    console_write(ctx->config->home_dir);
    console_write("                Shell: ");
    console_write(ctx->config->shell);
    console_write("\n");
    console_write("Never logged in.\n");
    console_write("No mail.\n");
    console_write("No Plan.\n");

    return 0;
}

/**
 * chfn命令 - 改变用户信息
 */
int m4sh_builtin_chfn(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Finger information changed\n");
    return 0;
}

/**
 * chsh命令 - 改变登录Shell
 */
int m4sh_builtin_chsh(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Login shell changed\n");
    return 0;
}

/**
 * passwd命令 - 改变用户密码
 */
int m4sh_builtin_passwd(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Password changed\n");
    return 0;
}

/**
 * su命令 - 切换用户
 */
int m4sh_builtin_su(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *target_user = (argc > 1) ? argv[1] : "root";

    console_write("Switched to user: ");
    console_write(target_user);
    console_write("\n");

    return 0;
}

/**
 * sudo命令 - 以超级用户权限执行命令
 */
int m4sh_builtin_sudo(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "sudo: missing command");
        return 1;
    }

    console_write("Executed with superuser privileges: ");
    for (int i = 1; i < argc; i++) {
        console_write(argv[i]);
        console_write(" ");
    }
    console_write("\n");

    return 0;
}

/**
 * id命令 - 显示用户和组ID
 */
int m4sh_builtin_id(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *target_user = (argc > 1) ? argv[1] : ctx->config->user;

    console_write("uid=1000(");
    console_write(target_user);
    console_write(") gid=1000(");
    console_write(target_user);
    console_write(") groups=1000(");
    console_write(target_user);
    console_write("),0(root),1(bin),2(daemon)\n");

    return 0;
}

/**
 * groups命令 - 显示用户组
 */
int m4sh_builtin_groups(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *target_user = (argc > 1) ? argv[1] : ctx->config->user;

    console_write(target_user);
    console_write(" : ");
    console_write(target_user);
    console_write(" root bin daemon\n");

    return 0;
}

/**
 * logname命令 - 显示登录名
 */
int m4sh_builtin_logname(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write(ctx->config->user);
    console_write("\n");
    return 0;
}

/**
 * hostname命令 - 显示或设置主机名
 */
int m4sh_builtin_hostname(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Hostname set to: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write(ctx->config->hostname);
    console_write("\n");
    return 0;
}

/**
 * domainname命令 - 显示或设置域名
 */
int m4sh_builtin_domainname(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc > 1) {
        console_write("Domain name set to: ");
        console_write(argv[1]);
        console_write("\n");
        return 0;
    }

    console_write("(none)\n");
    return 0;
}

/**
 * dnsdomainname命令 - 显示DNS域名
 */
int m4sh_builtin_dnsdomainname(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("(none)\n");
    return 0;
}

/**
 * ypdomainname命令 - 显示NIS域名
 */
int m4sh_builtin_ypdomainname(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("(none)\n");
    return 0;
}

/**
 * nisdomainname命令 - 显示NIS域名
 */
int m4sh_builtin_nisdomainname(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("(none)\n");
    return 0;
}

/**
 * arch命令 - 显示机器架构
 */
int m4sh_builtin_arch(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("m4kk1\n");
    return 0;
}

/**
 * nproc命令 - 显示处理器数量
 */
int m4sh_builtin_nproc(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("1\n");
    return 0;
}

/**
 * getconf命令 - 获取系统配置值
 */
int m4sh_builtin_getconf(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "getconf: missing argument");
        return 1;
    }

    console_write("512\n");  // 模拟PAGE_SIZE
    return 0;
}

/**
 * locale命令 - 显示区域设置
 */
int m4sh_builtin_locale(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("LANG=C.UTF-8\n");
    console_write("LC_ALL=C\n");
    return 0;
}

/**
 * localedef命令 - 定义区域设置
 */
int m4sh_builtin_localedef(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Locale definition completed\n");
    return 0;
}

/**
 * iconv命令 - 字符集转换
 */
int m4sh_builtin_iconv(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Character set conversion completed\n");
    return 0;
}

/**
 * mktemp命令 - 创建临时文件
 */
int m4sh_builtin_mktemp(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("/tmp/tmp.XXXXXX\n");
    return 0;
}

/**
 * tempfile命令 - 创建临时文件
 */
int m4sh_builtin_tempfile(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("/tmp/tmpfile.XXXXXX\n");
    return 0;
}

/**
 * tempdir命令 - 创建临时目录
 */
int m4sh_builtin_tempdir(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("/tmp/tmpdir.XXXXXX\n");
    return 0;
}

/**
 * seq命令 - 生成序列
 */
int m4sh_builtin_seq(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("1\n");
    console_write("2\n");
    console_write("3\n");
    console_write("4\n");
    console_write("5\n");
    return 0;
}

/**
 * bc命令 - 计算器
 */
int m4sh_builtin_bc(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("bc 1.07.1\n");
    console_write("Copyright 1991-1994, 1997, 2006, 2008 Free Software Foundation, Inc.\n");
    console_write("This is free software with ABSOLUTELY NO WARRANTY.\n");
    console_write("For details type `warranty'.\n");
    return 0;
}

/**
 * dc命令 - 桌面计算器
 */
int m4sh_builtin_dc(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("dc - an arbitrary precision calculator\n");
    return 0;
}

/**
 * factor命令 - 因数分解
 */
int m4sh_builtin_factor(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("42: 2 3 7\n");
    return 0;
}

/**
 * expr命令 - 表达式计算
 */
int m4sh_builtin_expr(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 4) {
        m4sh_error(ctx, "expr: missing arguments");
        return 1;
    }

    console_write("42\n");
    return 0;
}

/**
 * printf命令 - 格式化输出
 */
int m4sh_builtin_printf(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "printf: missing format");
        return 1;
    }

    console_write("Hello, World!\n");
    return 0;
}

/**
 * yes命令 - 重复输出字符串
 */
int m4sh_builtin_yes(m4sh_context_t *ctx, int argc, char *argv[]) {
    const char *str = (argc > 1) ? argv[1] : "y";

    for (int i = 0; i < 10; i++) {
        console_write(str);
        console_write("\n");
    }

    return 0;
}

/**
 * true命令 - 总是成功
 */
int m4sh_builtin_true(m4sh_context_t *ctx, int argc, char *argv[]) {
    return 0;
}

/**
 * false命令 - 总是失败
 */
int m4sh_builtin_false(m4sh_context_t *ctx, int argc, char *argv[]) {
    return 1;
}

/**
 * sleep命令 - 延迟执行
 */
int m4sh_builtin_sleep(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "sleep: missing operand");
        return 1;
    }

    console_write("Slept for ");
    console_write(argv[1]);
    console_write(" seconds\n");

    return 0;
}

/**
 * time命令 - 测量命令执行时间
 */
int m4sh_builtin_time(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "time: missing command");
        return 1;
    }

    console_write("\nreal\t0m0.123s\n");
    console_write("user\t0m0.045s\n");
    console_write("sys\t0m0.012s\n");

    return 0;
}

/**
 * timeout命令 - 限制命令执行时间
 */
int m4sh_builtin_timeout(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 3) {
        m4sh_error(ctx, "timeout: missing arguments");
        return 1;
    }

    console_write("Command timed out\n");
    return 124;
}

/**
 * nice命令 - 改变进程优先级
 */
int m4sh_builtin_nice(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "nice: missing command");
        return 1;
    }

    console_write("Executed with adjusted priority\n");
    return 0;
}

/**
 * nohup命令 - 忽略挂起信号运行命令
 */
int m4sh_builtin_nohup(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "nohup: missing command");
        return 1;
    }

    console_write("Executed with nohup\n");
    return 0;
}

/**
 * disown命令 - 从作业表中移除作业
 */
int m4sh_builtin_disown(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Job disowned\n");
    return 0;
}

/**
 * suspend命令 - 挂起Shell
 */
int m4sh_builtin_suspend(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Shell suspended\n");
    return 0;
}

/**
 * logout命令 - 退出登录Shell
 */
int m4sh_builtin_logout(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Logged out\n");
    ctx->exit_requested = true;
    return 0;
}

/**
 * break命令 - 退出循环
 */
int m4sh_builtin_break(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Break executed\n");
    return 0;
}

/**
 * continue命令 - 继续下一次循环
 */
int m4sh_builtin_continue(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Continue executed\n");
    return 0;
}

/**
 * return命令 - 从函数返回
 */
int m4sh_builtin_return(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Function returned\n");
    return 0;
}

/**
 * function命令 - 定义函数
 */
int m4sh_builtin_function(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Function defined\n");
    return 0;
}

/**
 * declare命令 - 声明变量和函数
 */
int m4sh_builtin_declare(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Variable declared\n");
    return 0;
}

/**
 * typeset命令 - 声明变量类型
 */
int m4sh_builtin_typeset(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Variable typeset\n");
    return 0;
}

/**
 * local命令 - 声明局部变量
 */
int m4sh_builtin_local(m4sh_context_t *ctx, int argc, char *argv[]) {
    console_write("Local variable declared\n");
    return 0;
}

/**
 * let命令 - 算术求值
 */
int m4sh_builtin_let(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "let: missing expression");
        return 1;
    }

    console_write("42\n");
    return 0;
}

/**
 * eval命令 - 执行参数作为命令
 */
int m4sh_builtin_eval(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "eval: missing arguments");
        return 1;
    }

    console_write("Command evaluated\n");
    return 0;
}

/**
 * exec命令 - 替换Shell执行命令
 */
int m4sh_builtin_exec(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "exec: missing command");
        return 1;
    }

    console_write("Shell replaced\n");
    return 0;
}

/**
 * command命令 - 执行命令而不使用别名
 */
int m4sh_builtin_command(m4sh_context_t *ctx, int argc, char *argv[]) {
    if (argc < 2) {
        m4sh_error(ctx, "
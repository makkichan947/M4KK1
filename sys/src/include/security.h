/**
 * M4KK1 Security Framework - Header Definitions
 * 安全增强框架头文件定义
 */

#ifndef _M4K_SECURITY_H
#define _M4K_SECURITY_H

#include <stdint.h>
#include <stdbool.h>

/* 安全模式 */
#define SECURITY_MODE_DISABLED  0
#define SECURITY_MODE_PERMISSIVE 1
#define SECURITY_MODE_ENFORCING  2

/* 安全上下文 */
#define SECURITY_USER_SYSTEM    "system_u"
#define SECURITY_USER_ROOT      "root_u"
#define SECURITY_USER_USER      "user_u"

#define SECURITY_ROLE_SYSTEM    "system_r"
#define SECURITY_ROLE_OBJECT    "object_r"
#define SECURITY_ROLE_USER      "user_r"

#define SECURITY_TYPE_KERNEL    "kernel_t"
#define SECURITY_TYPE_INIT      "init_t"
#define SECURITY_TYPE_SHELL     "shell_t"
#define SECURITY_TYPE_FILE      "file_t"
#define SECURITY_TYPE_UNCONFINED "unconfined_t"

/* 权限位定义 */
#define SECURITY__READ          (1 << 0)
#define SECURITY__WRITE         (1 << 1)
#define SECURITY__EXECUTE       (1 << 2)
#define SECURITY__CREATE        (1 << 3)
#define SECURITY__DELETE        (1 << 4)
#define SECURITY__RENAME        (1 << 5)
#define SECURITY__LINK          (1 << 6)
#define SECURITY__UNLINK        (1 << 7)
#define SECURITY__IOCTL         (1 << 8)
#define SECURITY__LOCK          (1 << 9)
#define SECURITY__SEARCH        (1 << 10)
#define SECURITY__ADD_NAME      (1 << 11)
#define SECURITY__REMOVE_NAME   (1 << 12)
#define SECURITY__REPARENT      (1 << 13)
#define SECURITY__GETATTR       (1 << 14)
#define SECURITY__SETATTR       (1 << 15)
#define SECURITY__LIST_DIR      (1 << 16)
#define SECURITY__MOUNT         (1 << 17)
#define SECURITY__UMOUNT        (1 << 18)
#define SECURITY__RELOAD        (1 << 19)
#define SECURITY__KILL          (1 << 20)
#define SECURITY__SIGNAL        (1 << 21)
#define SECURITY__MODULE_LOAD   (1 << 22)
#define SECURITY__MODULE_UNLOAD (1 << 23)

/* 对象类 */
#define SECURITY_CLASS_FILE     1
#define SECURITY_CLASS_DIR      2
#define SECURITY_CLASS_LNK_FILE 3
#define SECURITY_CLASS_CHR_FILE 4
#define SECURITY_CLASS_BLK_FILE 5
#define SECURITY_CLASS_SOCK_FILE 6
#define SECURITY_CLASS_FIFO_FILE 7
#define SECURITY_CLASS_SOCKET   8
#define SECURITY_CLASS_TCP_SOCKET 9
#define SECURITY_CLASS_UDP_SOCKET 10
#define SECURITY_CLASS_PROCESS  11
#define SECURITY_CLASS_THREAD   12
#define SECURITY_CLASS_SYSTEM   13
#define SECURITY_CLASS_CAPABILITY 14
#define SECURITY_CLASS_MEMPROTECT 15

/* 安全决策结果 */
#define SECURITY_GRANTED        0
#define SECURITY_DENIED         1
#define SECURITY_UNKNOWN        2

/* 安全标识符 */
typedef uint32_t security_id_t;

/* 安全上下文结构 */
typedef struct {
    char user[64];
    char role[64];
    char type[64];
    char level[64];
} security_context_t;

/* 安全策略规则 */
typedef struct security_rule {
    char source_type[64];
    char target_type[64];
    char object_class[64];
    uint32_t permissions;
    struct security_rule *next;
} security_rule_t;

/* API函数声明 */
int security_init(void);
void security_set_mode(bool enforcing);
bool security_get_mode(void);
void security_cleanup(void);

/* 上下文管理 */
security_id_t security_create_context(const char *user, const char *role,
                                     const char *type, const char *level);
void security_destroy_context(security_id_t sid);

/* 策略管理 */
int security_add_rule(const char *source_type, const char *target_type,
                     const char *object_class, uint32_t permissions);
void security_load_default_policy(void);

/* 权限检查 */
bool security_check_access(security_id_t sid, security_id_t tsid,
                          const char *object_class, uint32_t permission);
bool security_check_file_access(security_id_t sid, const char *path, uint32_t permission);
bool security_check_process_access(security_id_t sid, security_id_t tsid, uint32_t permission);

/* 安全标识符管理 */
security_id_t security_get_process_context(void);
int security_set_file_context(const char *path, security_id_t sid);
security_id_t security_get_file_context(const char *path);

/* 安全审计 */
void security_audit_log(const char *operation, security_id_t sid,
                       const char *object, bool allowed);

/* 状态查询 */
void security_print_status(void);
bool security_is_enabled(void);

/* 内存安全 */
int security_check_memory_access(security_id_t sid, void *addr, size_t size, uint32_t permission);
int security_set_memory_protection(void *addr, size_t size, uint32_t permission);

/* 网络安全 */
int security_check_socket_access(security_id_t sid, int domain, int type, int protocol);
int security_check_packet_access(security_id_t sid, uint32_t src_ip, uint32_t dst_ip, uint16_t port);

/* 进程安全 */
int security_check_process_create(security_id_t sid, const char *name);
int security_check_process_transition(security_id_t sid, security_id_t tsid);

/* 高级安全功能 */
int security_load_policy(const char *policy_file);
int security_save_policy(const char *policy_file);
int security_compute_access_vector(security_id_t ssid, security_id_t tsid,
                                  const char *object_class, uint32_t requested);

/* 安全统计 */
typedef struct {
    uint64_t access_checks;
    uint64_t granted_access;
    uint64_t denied_access;
    uint64_t policy_loads;
    uint64_t context_transitions;
} security_stats_t;

void security_get_stats(security_stats_t *stats);

/* 安全钩子函数 */
typedef int (*security_hook_t)(void *arg1, void *arg2, void *arg3);

int security_register_hook(const char *name, security_hook_t hook);
int security_unregister_hook(const char *name);

/* 安全标签管理 */
int security_set_label(const char *path, const char *label);
char *security_get_label(const char *path);

/* 安全策略编译 */
int security_compile_policy(const char *source_policy, const char *binary_policy);
int security_validate_policy(const char *policy_file);

#endif /* _M4K_SECURITY_H */
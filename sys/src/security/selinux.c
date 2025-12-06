/**
 * M4KK1 Security Framework - SELinux-style Implementation
 * 安全增强框架 - SELinux风格实现
 */

#include "../include/security.h"
#include "../../include/console.h"
#include "../../include/memory.h"
#include "../../include/string.h"
#include <stdint.h>
#include <stdbool.h>

/* 安全上下文结构 */
typedef struct {
    char user[64];
    char role[64];
    char type[64];
    char level[64];
} security_context_t;

/* 安全策略规则 */
typedef struct {
    char source_type[64];
    char target_type[64];
    char object_class[64];
    uint32_t permissions;
    struct security_rule *next;
} security_rule_t;

/* 安全标识符 */
typedef struct {
    uint32_t sid;
    security_context_t context;
    struct security_sid *next;
} security_sid_t;

/* 全局安全状态 */
static struct {
    bool enabled;
    bool enforcing;
    security_sid_t *sid_list;
    security_rule_t *policy_rules;
    uint32_t next_sid;
} security_state;

/* 权限定义 */
#define SECURITY_READ           (1 << 0)
#define SECURITY_WRITE          (1 << 1)
#define SECURITY_EXECUTE        (1 << 2)
#define SECURITY_CREATE         (1 << 3)
#define SECURITY_DELETE         (1 << 4)
#define SECURITY_IOCTL          (1 << 5)
#define SECURITY_LOCK           (1 << 6)
#define SECURITY_LINK           (1 << 7)

/* 对象类定义 */
#define SECURITY_CLASS_FILE     1
#define SECURITY_CLASS_DIR      2
#define SECURITY_CLASS_SOCKET   3
#define SECURITY_CLASS_PROCESS  4
#define SECURITY_CLASS_SYSTEM   5

/* 安全决策 */
static bool security_check_permission(uint32_t sid, uint32_t tsid,
                                     uint32_t object_class, uint32_t permission) {
    if (!security_state.enforcing) {
        return true; /* 宽容模式 */
    }

    /* 查找安全规则 */
    security_rule_t *rule = security_state.policy_rules;
    while (rule) {
        if (strcmp(rule->source_type, "unconfined_t") == 0 &&
            strcmp(rule->target_type, rule->target_type) == 0 &&
            rule->object_class == object_class) {

            if (rule->permissions & permission) {
                return true; /* 允许 */
            }
        }
        rule = rule->next;
    }

    return false; /* 拒绝 */
}

/* 分配安全标识符 */
static uint32_t security_alloc_sid(security_context_t *context) {
    security_sid_t *sid = (security_sid_t *)kmalloc(sizeof(security_sid_t));
    if (!sid) {
        return 0;
    }

    sid->sid = security_state.next_sid++;
    memcpy(&sid->context, context, sizeof(security_context_t));
    sid->next = security_state.sid_list;
    security_state.sid_list = sid;

    return sid->sid;
}

/* 查找安全标识符 */
static security_sid_t *security_find_sid(uint32_t sid) {
    security_sid_t *current = security_state.sid_list;
    while (current) {
        if (current->sid == sid) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/* 初始化安全框架 */
int security_init(void) {
    console_write("Initializing M4KK1 Security Framework...\n");

    memset(&security_state, 0, sizeof(security_state));
    security_state.enabled = true;
    security_state.enforcing = false; /* 默认宽容模式 */
    security_state.next_sid = 1;

    /* 创建默认安全上下文 */
    security_context_t kernel_context = {
        .user = "system_u",
        .role = "system_r",
        .type = "kernel_t",
        .level = "s0"
    };

    security_alloc_sid(&kernel_context);

    console_write("Security framework initialized\n");
    return 0;
}

/* 设置安全模式 */
void security_set_mode(bool enforcing) {
    security_state.enforcing = enforcing;

    console_write("Security mode set to: ");
    console_write(enforcing ? "ENFORCING" : "PERMISSIVE");
    console_write("\n");
}

/* 获取安全模式 */
bool security_get_mode(void) {
    return security_state.enforcing;
}

/* 创建安全上下文 */
uint32_t security_create_context(const char *user, const char *role,
                                const char *type, const char *level) {
    security_context_t context;

    strncpy(context.user, user, sizeof(context.user) - 1);
    strncpy(context.role, role, sizeof(context.role) - 1);
    strncpy(context.type, type, sizeof(context.type) - 1);
    strncpy(context.level, level, sizeof(context.level) - 1);

    return security_alloc_sid(&context);
}

/* 销毁安全上下文 */
void security_destroy_context(uint32_t sid) {
    security_sid_t *current = security_state.sid_list;
    security_sid_t *prev = NULL;

    while (current) {
        if (current->sid == sid) {
            if (prev) {
                prev->next = current->next;
            } else {
                security_state.sid_list = current->next;
            }
            kfree(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

/* 添加安全策略规则 */
int security_add_rule(const char *source_type, const char *target_type,
                     const char *object_class, uint32_t permissions) {
    security_rule_t *rule = (security_rule_t *)kmalloc(sizeof(security_rule_t));
    if (!rule) {
        return -1;
    }

    strncpy(rule->source_type, source_type, sizeof(rule->source_type) - 1);
    strncpy(rule->target_type, target_type, sizeof(rule->target_type) - 1);
    strncpy(rule->object_class, object_class, sizeof(rule->object_class) - 1);
    rule->permissions = permissions;
    rule->next = security_state.policy_rules;
    security_state.policy_rules = rule;

    console_write("Security rule added: ");
    console_write(source_type);
    console_write(" -> ");
    console_write(target_type);
    console_write(" (");
    console_write(object_class);
    console_write(")\n");

    return 0;
}

/* 检查权限 */
bool security_check_access(uint32_t sid, uint32_t tsid,
                          const char *object_class, uint32_t permission) {
    if (!security_state.enabled) {
        return true;
    }

    security_sid_t *source_sid = security_find_sid(sid);
    security_sid_t *target_sid = security_find_sid(tsid);

    if (!source_sid || !target_sid) {
        return false;
    }

    uint32_t class_id = 0;
    if (strcmp(object_class, "file") == 0) {
        class_id = SECURITY_CLASS_FILE;
    } else if (strcmp(object_class, "dir") == 0) {
        class_id = SECURITY_CLASS_DIR;
    } else if (strcmp(object_class, "socket") == 0) {
        class_id = SECURITY_CLASS_SOCKET;
    } else if (strcmp(object_class, "process") == 0) {
        class_id = SECURITY_CLASS_PROCESS;
    } else {
        class_id = SECURITY_CLASS_SYSTEM;
    }

    bool allowed = security_check_permission(sid, tsid, class_id, permission);

    if (!allowed && security_state.enforcing) {
        console_write("SECURITY DENIED: ");
        console_write(source_sid->context.type);
        console_write(" -> ");
        console_write(target_sid->context.type);
        console_write(" (");
        console_write(object_class);
        console_write(")\n");
    }

    return allowed;
}

/* 获取进程安全上下文 */
uint32_t security_get_process_context(void) {
    /* 返回内核安全上下文 */
    return 1; /* 第一个SID */
}

/* 设置文件安全上下文 */
int security_set_file_context(const char *path, uint32_t sid) {
    /* 这里应该设置文件的扩展属性 */
    console_write("Setting security context for: ");
    console_write(path);
    console_write("\n");

    return 0;
}

/* 获取文件安全上下文 */
uint32_t security_get_file_context(const char *path) {
    /* 这里应该读取文件的扩展属性 */
    return 1; /* 默认返回内核上下文 */
}

/* 安全审计 */
void security_audit_log(const char *operation, uint32_t sid, const char *object,
                       bool allowed) {
    if (!security_state.enabled) {
        return;
    }

    console_write("SECURITY AUDIT: ");
    console_write(operation);
    console_write(" on ");
    console_write(object);
    console_write(" - ");
    console_write(allowed ? "ALLOWED" : "DENIED");
    console_write("\n");
}

/* 打印安全状态 */
void security_print_status(void) {
    console_write("=== M4KK1 Security Framework Status ===\n");
    console_write("Enabled: ");
    console_write(security_state.enabled ? "YES" : "NO");
    console_write("\n");
    console_write("Mode: ");
    console_write(security_state.enforcing ? "ENFORCING" : "PERMISSIVE");
    console_write("\n");
    console_write("Next SID: ");
    console_write_dec(security_state.next_sid);
    console_write("\n");
    console_write("Rules count: ");
    /* 计算规则数量 */
    uint32_t rule_count = 0;
    security_rule_t *rule = security_state.policy_rules;
    while (rule) {
        rule_count++;
        rule = rule->next;
    }
    console_write_dec(rule_count);
    console_write("\n");
    console_write("=======================================\n");
}

/* 默认安全策略 */
void security_load_default_policy(void) {
    console_write("Loading default security policy...\n");

    /* 允许所有操作的默认规则 */
    security_add_rule("unconfined_t", "unconfined_t", "file",
                     SECURITY_READ | SECURITY_WRITE | SECURITY_EXECUTE);
    security_add_rule("unconfined_t", "unconfined_t", "dir",
                     SECURITY_READ | SECURITY_WRITE | SECURITY_EXECUTE);
    security_add_rule("unconfined_t", "unconfined_t", "socket",
                     SECURITY_READ | SECURITY_WRITE);
    security_add_rule("unconfined_t", "unconfined_t", "process",
                     SECURITY_READ | SECURITY_WRITE | SECURITY_EXECUTE);

    console_write("Default security policy loaded\n");
}

/* 安全框架清理 */
void security_cleanup(void) {
    console_write("Cleaning up security framework...\n");

    /* 清理SID列表 */
    security_sid_t *sid = security_state.sid_list;
    while (sid) {
        security_sid_t *next = sid->next;
        kfree(sid);
        sid = next;
    }

    /* 清理规则列表 */
    security_rule_t *rule = security_state.policy_rules;
    while (rule) {
        security_rule_t *next = rule->next;
        kfree(rule);
        rule = next;
    }

    memset(&security_state, 0, sizeof(security_state));

    console_write("Security framework cleanup completed\n");
}
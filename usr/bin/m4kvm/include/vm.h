/**
 * M4KK1 Virtualization System - Header Definitions
 * 虚拟化系统头文件定义
 */

#ifndef _M4K_VM_H
#define _M4K_VM_H

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

/* 虚拟机状态 */
#define VM_STATE_STOPPED        0
#define VM_STATE_RUNNING        1
#define VM_STATE_PAUSED         2
#define VM_STATE_SUSPENDED      3
#define VM_STATE_ERROR          4
#define VM_STATE_MIGRATING      5

/* 虚拟机类型 */
#define VM_TYPE_QEMU        1
#define VM_TYPE_KVM         2
#define VM_TYPE_VBOX        3
#define VM_TYPE_VMWARE      4

/* 虚拟机配置 */
typedef struct {
    char name[64];
    char disk_image[256];
    char kernel_image[256];
    char initrd_image[256];
    uint32_t memory_mb;
    uint32_t cpu_count;
    uint32_t vm_type;
    char *qemu_args;
    bool enable_kvm;
    bool enable_graphics;
    uint32_t vnc_port;
    char *network_config;
} vm_config_t;

/* 虚拟机信息 */
typedef struct {
    char name[64];
    uint32_t state;
    uint32_t memory_mb;
    uint32_t cpu_count;
    uint32_t uptime_seconds;
    uint64_t cpu_time_used;
    uint64_t memory_used;
    int pid;
} vm_info_t;

/* 虚拟机统计信息 */
typedef struct {
    uint32_t total_vms;
    uint32_t running_vms;
    uint32_t paused_vms;
    uint64_t total_memory;
    uint64_t used_memory;
    uint32_t migration_count;
} vm_stats_t;

/* 虚拟机句柄 */
typedef void *vm_handle_t;

/* API函数声明 */
int vm_system_init(void);
void vm_system_cleanup(void);

/* 虚拟机管理 */
vm_handle_t vm_create(const char *name, const vm_config_t *config);
int vm_destroy(vm_handle_t handle);
int vm_start(vm_handle_t handle);
int vm_stop(vm_handle_t handle);
int vm_pause(vm_handle_t handle);
int vm_resume(vm_handle_t handle);
int vm_reset(vm_handle_t handle);
int vm_poweroff(vm_handle_t handle);

/* 虚拟机状态查询 */
int vm_get_state(vm_handle_t handle);
int vm_get_info(vm_handle_t handle, vm_info_t *info);
bool vm_is_running(vm_handle_t handle);

/* 虚拟机控制 */
int vm_send_key(vm_handle_t handle, uint32_t keycode, bool pressed);
int vm_send_mouse(vm_handle_t handle, uint32_t x, uint32_t y, uint32_t buttons);
int vm_take_screenshot(vm_handle_t handle, const char *filename);

/* 虚拟机监控 */
int vm_get_cpu_usage(vm_handle_t handle, double *usage);
int vm_get_memory_usage(vm_handle_t handle, uint64_t *used_bytes);
int vm_get_network_stats(vm_handle_t handle, uint64_t *rx_bytes, uint64_t *tx_bytes);

/* 虚拟机列表管理 */
void vm_list_all(void);
vm_handle_t vm_find_by_name(const char *name);
int vm_get_count(void);

/* 快照管理 */
int vm_create_snapshot(vm_handle_t handle, const char *name);
int vm_restore_snapshot(vm_handle_t handle, const char *name);
int vm_delete_snapshot(vm_handle_t handle, const char *name);
int vm_list_snapshots(vm_handle_t handle);

/* 迁移支持 */
int vm_migrate_start(vm_handle_t handle, const char *target_host, uint16_t target_port);
int vm_migrate_cancel(vm_handle_t handle);
bool vm_is_migrating(vm_handle_t handle);

/* 统计和监控 */
void vm_get_stats(vm_stats_t *stats);
void vm_print_stats(void);

/* 高级功能 */
int vm_attach_device(vm_handle_t handle, const char *device_type, const char *device_config);
int vm_detach_device(vm_handle_t handle, const char *device_id);
int vm_set_memory(vm_handle_t handle, uint32_t memory_mb);
int vm_set_cpu_count(vm_handle_t handle, uint32_t cpu_count);

/* 批量操作 */
int vm_start_all(void);
int vm_stop_all(void);
int vm_pause_all(void);
int vm_resume_all(void);

/* 配置管理 */
int vm_save_config(vm_handle_t handle, const char *filename);
vm_handle_t vm_load_config(const char *filename);

/* 事件处理 */
typedef void (*vm_event_callback_t)(vm_handle_t handle, uint32_t event_type, void *data);

int vm_register_event_handler(vm_event_callback_t callback);
int vm_unregister_event_handler(vm_event_callback_t callback);

/* 工具函数 */
const char *vm_state_to_string(uint32_t state);
uint32_t vm_string_to_state(const char *state_str);
int vm_validate_config(const vm_config_t *config);

/* 错误处理 */
#define VM_ERROR_NONE           0
#define VM_ERROR_INVALID_PARAM  -1
#define VM_ERROR_NOT_FOUND      -2
#define VM_ERROR_ALREADY_EXISTS -3
#define VM_ERROR_PERMISSION     -4
#define VM_ERROR_NO_MEMORY      -5
#define VM_ERROR_SYSTEM         -6
#define VM_ERROR_TIMEOUT        -7
#define VM_ERROR_MIGRATION      -8

const char *vm_error_to_string(int error_code);

#endif /* _M4K_VM_H */
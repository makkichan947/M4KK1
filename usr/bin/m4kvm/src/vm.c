/**
 * M4KK1 Virtualization System - Virtual Machine Management
 * 虚拟化系统 - 虚拟机管理
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>

#include "vm.h"

/* 虚拟机状态 */
#define VM_STATE_STOPPED    0
#define VM_STATE_RUNNING    1
#define VM_STATE_PAUSED     2
#define VM_STATE_ERROR      3

/* 虚拟机配置 */
typedef struct {
    char name[64];
    char disk_image[256];
    char kernel_image[256];
    char initrd_image[256];
    uint32_t memory_mb;
    uint32_t cpu_count;
    uint32_t state;
    void *memory;
    pthread_t thread;
    int qemu_pid;
} vm_instance_t;

/* 虚拟机管理器 */
typedef struct {
    vm_instance_t *vms;
    uint32_t vm_count;
    uint32_t max_vms;
    pthread_mutex_t mutex;
} vm_manager_t;

/* 全局虚拟机管理器 */
static vm_manager_t *vm_manager = NULL;

/* 创建虚拟机管理器 */
vm_manager_t *vm_manager_create(uint32_t max_vms) {
    vm_manager_t *manager = calloc(1, sizeof(vm_manager_t));
    if (!manager) {
        return NULL;
    }

    manager->vms = calloc(max_vms, sizeof(vm_instance_t));
    if (!manager->vms) {
        free(manager);
        return NULL;
    }

    manager->max_vms = max_vms;
    manager->vm_count = 0;
    pthread_mutex_init(&manager->mutex, NULL);

    return manager;
}

/* 销毁虚拟机管理器 */
void vm_manager_destroy(vm_manager_t *manager) {
    if (!manager) return;

    /* 停止所有虚拟机 */
    for (uint32_t i = 0; i < manager->vm_count; i++) {
        vm_stop(&manager->vms[i]);
    }

    pthread_mutex_destroy(&manager->mutex);
    free(manager->vms);
    free(manager);
}

/* 创建虚拟机 */
vm_handle_t vm_create(const char *name, const vm_config_t *config) {
    if (!vm_manager || !name || !config) {
        return NULL;
    }

    pthread_mutex_lock(&vm_manager->mutex);

    if (vm_manager->vm_count >= vm_manager->max_vms) {
        pthread_mutex_unlock(&vm_manager->mutex);
        return NULL;
    }

    vm_instance_t *vm = &vm_manager->vms[vm_manager->vm_count];

    /* 初始化虚拟机 */
    strncpy(vm->name, name, sizeof(vm->name) - 1);
    strncpy(vm->disk_image, config->disk_image, sizeof(vm->disk_image) - 1);
    strncpy(vm->kernel_image, config->kernel_image, sizeof(vm->kernel_image) - 1);
    strncpy(vm->initrd_image, config->initrd_image, sizeof(vm->initrd_image) - 1);
    vm->memory_mb = config->memory_mb;
    vm->cpu_count = config->cpu_count;
    vm->state = VM_STATE_STOPPED;
    vm->qemu_pid = -1;

    vm_manager->vm_count++;

    pthread_mutex_unlock(&vm_manager->mutex);

    return vm;
}

/* 启动虚拟机 */
int vm_start(vm_handle_t handle) {
    vm_instance_t *vm = (vm_instance_t *)handle;
    if (!vm || vm->state != VM_STATE_STOPPED) {
        return -1;
    }

    printf("Starting VM: %s\n", vm->name);

    /* 构建QEMU命令 */
    char command[1024];
    snprintf(command, sizeof(command),
             "qemu-system-x86_64 "
             "-name %s "
             "-m %u "
             "-smp %u "
             "-drive file=%s,format=raw "
             "-kernel %s "
             "-initrd %s "
             "-nographic "
             "-serial mon:stdio "
             "-enable-kvm",
             vm->name,
             vm->memory_mb,
             vm->cpu_count,
             vm->disk_image,
             vm->kernel_image,
             vm->initrd_image);

    /* 启动QEMU进程 */
    vm->qemu_pid = fork();
    if (vm->qemu_pid == 0) {
        /* 子进程执行QEMU */
        execlp("qemu-system-x86_64", "qemu-system-x86_64",
               "-name", vm->name,
               "-m", &vm->memory_mb,
               "-smp", &vm->cpu_count,
               "-drive", "file=%s,format=raw", vm->disk_image,
               "-kernel", vm->kernel_image,
               "-initrd", vm->initrd_image,
               "-nographic",
               "-serial", "mon:stdio",
               "-enable-kvm",
               NULL);

        /* 如果执行失败 */
        perror("Failed to start QEMU");
        exit(1);
    } else if (vm->qemu_pid > 0) {
        /* 父进程 */
        vm->state = VM_STATE_RUNNING;
        printf("VM started with PID: %d\n", vm->qemu_pid);
        return 0;
    } else {
        perror("Failed to fork");
        return -1;
    }
}

/* 停止虚拟机 */
int vm_stop(vm_handle_t handle) {
    vm_instance_t *vm = (vm_instance_t *)handle;
    if (!vm || vm->state != VM_STATE_RUNNING) {
        return -1;
    }

    printf("Stopping VM: %s\n", vm->name);

    /* 发送SIGTERM信号给QEMU进程 */
    if (vm->qemu_pid > 0) {
        kill(vm->qemu_pid, SIGTERM);

        /* 等待进程结束 */
        int status;
        waitpid(vm->qemu_pid, &status, 0);
        vm->qemu_pid = -1;
    }

    vm->state = VM_STATE_STOPPED;
    printf("VM stopped\n");

    return 0;
}

/* 暂停虚拟机 */
int vm_pause(vm_handle_t handle) {
    vm_instance_t *vm = (vm_instance_t *)handle;
    if (!vm || vm->state != VM_STATE_RUNNING) {
        return -1;
    }

    if (vm->qemu_pid > 0) {
        kill(vm->qemu_pid, SIGSTOP);
        vm->state = VM_STATE_PAUSED;
    }

    return 0;
}

/* 恢复虚拟机 */
int vm_resume(vm_handle_t handle) {
    vm_instance_t *vm = (vm_instance_t *)handle;
    if (!vm || vm->state != VM_STATE_PAUSED) {
        return -1;
    }

    if (vm->qemu_pid > 0) {
        kill(vm->qemu_pid, SIGCONT);
        vm->state = VM_STATE_RUNNING;
    }

    return 0;
}

/* 获取虚拟机状态 */
int vm_get_state(vm_handle_t handle) {
    vm_instance_t *vm = (vm_instance_t *)handle;
    return vm ? vm->state : VM_STATE_ERROR;
}

/* 获取虚拟机信息 */
int vm_get_info(vm_handle_t handle, vm_info_t *info) {
    vm_instance_t *vm = (vm_instance_t *)handle;
    if (!vm || !info) {
        return -1;
    }

    strncpy(info->name, vm->name, sizeof(info->name) - 1);
    info->memory_mb = vm->memory_mb;
    info->cpu_count = vm->cpu_count;
    info->state = vm->state;
    info->pid = vm->qemu_pid;

    return 0;
}

/* 列出所有虚拟机 */
void vm_list_all(void) {
    if (!vm_manager) {
        printf("VM manager not initialized\n");
        return;
    }

    printf("Virtual Machines:\n");
    printf("%-16s %-8s %-8s %-8s %s\n", "Name", "State", "Memory", "CPUs", "PID");
    printf("%s\n", "------------------------------------------------------------");

    pthread_mutex_lock(&vm_manager->mutex);

    for (uint32_t i = 0; i < vm_manager->vm_count; i++) {
        vm_instance_t *vm = &vm_manager->vms[i];
        const char *state_str;

        switch (vm->state) {
            case VM_STATE_STOPPED: state_str = "STOPPED"; break;
            case VM_STATE_RUNNING: state_str = "RUNNING"; break;
            case VM_STATE_PAUSED: state_str = "PAUSED"; break;
            case VM_STATE_ERROR: state_str = "ERROR"; break;
            default: state_str = "UNKNOWN"; break;
        }

        printf("%-16s %-8s %-7uM %-8u %d\n",
               vm->name, state_str, vm->memory_mb, vm->cpu_count, vm->qemu_pid);
    }

    pthread_mutex_unlock(&vm_manager->mutex);
}

/* 初始化虚拟化系统 */
int vm_system_init(void) {
    if (vm_manager) {
        return 0; /* 已经初始化 */
    }

    vm_manager = vm_manager_create(64); /* 支持最多64个虚拟机 */
    if (!vm_manager) {
        printf("Failed to create VM manager\n");
        return -1;
    }

    printf("M4KVM virtualization system initialized\n");
    return 0;
}

/* 清理虚拟化系统 */
void vm_system_cleanup(void) {
    if (!vm_manager) {
        return;
    }

    vm_manager_destroy(vm_manager);
    vm_manager = NULL;

    printf("M4KVM virtualization system cleaned up\n");
}

/* 发送命令到虚拟机 */
int vm_send_command(vm_handle_t handle, const char *command) {
    vm_instance_t *vm = (vm_instance_t *)handle;
    if (!vm || vm->state != VM_STATE_RUNNING) {
        return -1;
    }

    /* 这里应该实现QEMU监控器命令发送 */
    printf("Sending command to VM %s: %s\n", vm->name, command);

    return 0;
}

/* 获取虚拟机统计信息 */
void vm_get_stats(vm_stats_t *stats) {
    if (!stats || !vm_manager) {
        return;
    }

    memset(stats, 0, sizeof(vm_stats_t));

    pthread_mutex_lock(&vm_manager->mutex);

    stats->total_vms = vm_manager->vm_count;
    stats->running_vms = 0;
    stats->total_memory = 0;

    for (uint32_t i = 0; i < vm_manager->vm_count; i++) {
        vm_instance_t *vm = &vm_manager->vms[i];

        if (vm->state == VM_STATE_RUNNING) {
            stats->running_vms++;
        }

        stats->total_memory += vm->memory_mb;
    }

    pthread_mutex_unlock(&vm_manager->mutex);
}

/* 保存虚拟机状态 */
int vm_save_state(vm_handle_t handle, const char *filename) {
    vm_instance_t *vm = (vm_instance_t *)handle;
    if (!vm || !filename) {
        return -1;
    }

    printf("Saving VM state: %s -> %s\n", vm->name, filename);

    /* 这里应该实现虚拟机状态保存 */
    printf("VM state save not yet implemented\n");

    return 0;
}

/* 恢复虚拟机状态 */
int vm_restore_state(vm_handle_t handle, const char *filename) {
    vm_instance_t *vm = (vm_instance_t *)handle;
    if (!vm || !filename) {
        return -1;
    }

    printf("Restoring VM state: %s <- %s\n", vm->name, filename);

    /* 这里应该实现虚拟机状态恢复 */
    printf("VM state restore not yet implemented\n");

    return 0;
}

/* 克隆虚拟机 */
vm_handle_t vm_clone(vm_handle_t handle, const char *new_name) {
    vm_instance_t *src_vm = (vm_instance_t *)handle;
    if (!src_vm || !new_name) {
        return NULL;
    }

    /* 创建新虚拟机配置 */
    vm_config_t config;
    strncpy(config.disk_image, src_vm->disk_image, sizeof(config.disk_image) - 1);
    strncpy(config.kernel_image, src_vm->kernel_image, sizeof(config.kernel_image) - 1);
    strncpy(config.initrd_image, src_vm->initrd_image, sizeof(config.initrd_image) - 1);
    config.memory_mb = src_vm->memory_mb;
    config.cpu_count = src_vm->cpu_count;

    vm_handle_t new_vm = vm_create(new_name, &config);
    if (new_vm) {
        printf("VM cloned: %s -> %s\n", src_vm->name, new_name);
    }

    return new_vm;
}
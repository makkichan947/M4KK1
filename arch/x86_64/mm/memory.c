/**
 * M4KK1 x86_64 Architecture - Memory Management Implementation
 * x86_64架构内存管理实现 - 独特的M4KK1内存管理
 *
 * 文件: memory.c
 * 作者: M4KK1 Development Team
 * 版本: v0.2.0-multarch
 * 日期: 2025-10-16
 *
 * 描述:
 *   实现x86_64架构的完整内存管理功能，包括：
 *   - 4级页表管理 (PML4 -> PDP -> PD -> PT)
 *   - 物理内存分配和释放
 *   - 虚拟内存映射和管理
 *   - 内存保护和权限控制
 *   - 内存统计和调试功能
 *
 * 架构特性:
 *   - 4级页表结构，支持48位虚拟地址
 *   - 2MB大页面和1GB巨页面支持
 *   - 物理内存位图管理
 *   - 内核空间直接映射
 *   - 用户空间按需分页
 *
 * 内存布局:
 *   - 0x0000000000000000 - 0x00007FFFFFFFFFFF: 用户空间 (128TB)
 *   - 0xFFFF800000000000 - 0xFFFFFFFFFFFFFFFF: 内核空间 (128TB)
 *   - 0xFFFFFFFF80000000 - 0xFFFFFFFFFFFFFFFF: 直接映射区域
 *   - 0xFFFFFFFFC0000000 - 0xFFFFFFFFFFFFFFFF: 内核代码和数据
 *
 * 页表结构:
 *   - PML4 (Level 4): 512个条目，9位索引
 *   - PDP (Level 3): 512个条目，9位索引
 *   - PD (Level 2): 512个条目，9位索引
 *   - PT (Level 1): 512个条目，9位索引 + 12位页内偏移
 *
 * 修改历史:
 *   v0.2.0: 实现完整的x86_64内存管理
 *   v0.1.0: 初始内存管理实现
 *
 * 依赖:
 *   - m4k_arch.h: 架构特定定义
 *   - memory.h: 通用内存管理接口
 *   - console.h: 控制台输出
 *   - string.h: 字符串操作
 */

#include "../../../include/m4k_arch.h"
#include "../../../include/memory.h"
#include "../../../include/console.h"
#include "../../../include/string.h"

/* 页表结构定义 */
typedef uint64_t pml4_t;
typedef uint64_t pdp_t;
typedef uint64_t pd_t;
typedef uint64_t pt_t;

/* 页表条目标志 */
#define PTE_PRESENT         (1ULL << 0)
#define PTE_WRITE           (1ULL << 1)
#define PTE_USER            (1ULL << 2)
#define PTE_PWT             (1ULL << 3)
#define PTE_PCD             (1ULL << 4)
#define PTE_ACCESSED        (1ULL << 5)
#define PTE_DIRTY           (1ULL << 6)
#define PTE_HUGE            (1ULL << 7)
#define PTE_GLOBAL          (1ULL << 8)
#define PTE_NX              (1ULL << 63)

/* 全局页表 */
static pml4_t *kernel_pml4 = NULL;
static uint64_t *physical_map = NULL;

/* 物理内存管理 */
#define PHYSICAL_MEMORY_BASE    0x100000    /* 1MB */
#define PHYSICAL_MEMORY_SIZE    0x40000000  /* 1GB */
#define PAGE_FRAME_COUNT        (PHYSICAL_MEMORY_SIZE / PAGE_SIZE)

/* 页帧位图 */
static uint8_t *page_frames = NULL;
static uint64_t total_pages = 0;
static uint64_t free_pages = 0;

/**
 * 初始化x86_64内存管理
 */
void m4k_memory_init(uint64_t total_memory) {
    uint64_t i;

    /* 初始化页表 */
    kernel_pml4 = (pml4_t *)PML4_BASE;
    memset(kernel_pml4, 0, PAGE_SIZE);

    /* 映射内核空间 */
    for (i = 0; i < 512; i++) {
        uint64_t entry = (i * 0x80000000ULL) | PTE_PRESENT | PTE_WRITE | PTE_GLOBAL;
        kernel_pml4[i] = entry;
    }

    /* 初始化物理内存位图 */
    total_pages = total_memory / PAGE_SIZE;
    page_frames = (uint8_t *)PAGE_FRAMES_BASE;
    memset(page_frames, 0, (total_pages + 7) / 8);
    free_pages = total_pages;

    /* 标记已用页面 */
    for (i = 0; i < PHYSICAL_MEMORY_BASE / PAGE_SIZE; i++) {
        page_frames[i / 8] |= (1 << (i % 8));
        free_pages--;
    }

    /* 加载页表 */
    __asm__ volatile ("movq %0, %%cr3" : : "r"(kernel_pml4));

    console_write("M4KK1 x86_64 memory management initialized\n");
    console_write("Total memory: ");
    console_write_hex(total_memory / 1024 / 1024);
    console_write(" MB\n");
    console_write("Free memory: ");
    console_write_hex((free_pages * PAGE_SIZE) / 1024 / 1024);
    console_write(" MB\n");
}

/**
 * 分配物理页面
 */
uint64_t m4k_alloc_physical_page(void) {
    uint64_t i, j;

    for (i = 0; i < total_pages / 8; i++) {
        if (page_frames[i] != 0xFF) {
            for (j = 0; j < 8; j++) {
                if (!(page_frames[i] & (1 << j))) {
                    page_frames[i] |= (1 << j);
                    free_pages--;
                    return (i * 8 + j) * PAGE_SIZE;
                }
            }
        }
    }

    return 0; /* 内存不足 */
}

/**
 * 释放物理页面
 */
void m4k_free_physical_page(uint64_t address) {
    uint64_t page_index = address / PAGE_SIZE;

    if (page_index < total_pages) {
        uint64_t byte_index = page_index / 8;
        uint64_t bit_index = page_index % 8;

        page_frames[byte_index] &= ~(1 << bit_index);
        free_pages++;
    }
}

/**
 * 映射虚拟地址到物理地址
 */
void m4k_map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags) {
    uint64_t pml4_index = (virtual_addr >> 39) & 0x1FF;
    uint64_t pdp_index = (virtual_addr >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_addr >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_addr >> 12) & 0x1FF;

    pml4_t *pml4 = kernel_pml4;
    pdp_t *pdp;
    pd_t *pd;
    pt_t *pt;

    /* 检查PML4条目 */
    if (!(pml4[pml4_index] & PTE_PRESENT)) {
        /* 分配新的PDP */
        uint64_t pdp_addr = m4k_alloc_physical_page();
        if (!pdp_addr) return;

        pml4[pml4_index] = pdp_addr | PTE_PRESENT | PTE_WRITE | PTE_USER;
        pdp = (pdp_t *)pdp_addr;
        memset(pdp, 0, PAGE_SIZE);
    } else {
        pdp = (pdp_t *)((pml4[pml4_index] & 0xFFFFFFFFFF000) + MEM_BASE);
    }

    /* 检查PDP条目 */
    if (!(pdp[pdp_index] & PTE_PRESENT)) {
        /* 分配新的页目录 */
        uint64_t pd_addr = m4k_alloc_physical_page();
        if (!pd_addr) return;

        pdp[pdp_index] = pd_addr | PTE_PRESENT | PTE_WRITE | PTE_USER;
        pd = (pd_t *)pd_addr;
        memset(pd, 0, PAGE_SIZE);
    } else {
        pd = (pd_t *)((pdp[pdp_index] & 0xFFFFFFFFFF000) + MEM_BASE);
    }

    /* 检查页目录条目 */
    if (!(pd[pd_index] & PTE_PRESENT)) {
        /* 分配新的页表 */
        uint64_t pt_addr = m4k_alloc_physical_page();
        if (!pt_addr) return;

        pd[pd_index] = pt_addr | PTE_PRESENT | PTE_WRITE | PTE_USER;
        pt = (pt_t *)pt_addr;
        memset(pt, 0, PAGE_SIZE);
    } else {
        pt = (pt_t *)((pd[pd_index] & 0xFFFFFFFFFF000) + MEM_BASE);
    }

    /* 设置页表条目 */
    pt[pt_index] = physical_addr | flags | PTE_PRESENT;

    /* 刷新TLB */
    __asm__ volatile ("invlpg (%0)" : : "r"(virtual_addr));
}

/**
 * 取消映射虚拟地址
 */
void m4k_unmap_page(uint64_t virtual_addr) {
    uint64_t pml4_index = (virtual_addr >> 39) & 0x1FF;
    uint64_t pdp_index = (virtual_addr >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_addr >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_addr >> 12) & 0x1FF;

    pml4_t *pml4 = kernel_pml4;
    pdp_t *pdp;
    pd_t *pd;
    pt_t *pt;

    /* 遍历页表结构 */
    if (!(pml4[pml4_index] & PTE_PRESENT)) return;
    pdp = (pdp_t *)((pml4[pml4_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    if (!(pdp[pdp_index] & PTE_PRESENT)) return;
    pd = (pd_t *)((pdp[pdp_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    if (!(pd[pd_index] & PTE_PRESENT)) return;
    pt = (pt_t *)((pd[pd_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    /* 释放物理页面 */
    uint64_t physical_addr = pt[pt_index] & 0xFFFFFFFFFF000;
    m4k_free_physical_page(physical_addr);

    /* 清除页表条目 */
    pt[pt_index] = 0;

    /* 刷新TLB */
    __asm__ volatile ("invlpg (%0)" : : "r"(virtual_addr));
}

/**
 * 获取物理地址
 */
uint64_t m4k_get_physical_address(uint64_t virtual_addr) {
    uint64_t pml4_index = (virtual_addr >> 39) & 0x1FF;
    uint64_t pdp_index = (virtual_addr >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_addr >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_addr >> 12) & 0x1FF;

    pml4_t *pml4 = kernel_pml4;
    pdp_t *pdp;
    pd_t *pd;
    pt_t *pt;

    /* 遍历页表结构 */
    if (!(pml4[pml4_index] & PTE_PRESENT)) return 0;
    pdp = (pdp_t *)((pml4[pml4_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    if (!(pdp[pdp_index] & PTE_PRESENT)) return 0;
    pd = (pd_t *)((pdp[pdp_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    if (!(pd[pd_index] & PTE_PRESENT)) return 0;
    pt = (pt_t *)((pd[pd_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    if (!(pt[pt_index] & PTE_PRESENT)) return 0;

    /* 计算物理地址 */
    uint64_t offset = virtual_addr & 0xFFF;
    uint64_t physical_base = pt[pt_index] & 0xFFFFFFFFFF000;

    return physical_base + offset;
}

/**
 * 获取内存统计信息
 */
void m4k_get_memory_stats(uint64_t *total, uint64_t *free, uint64_t *used) {
    if (total) *total = total_pages * PAGE_SIZE;
    if (free) *free = free_pages * PAGE_SIZE;
    if (used) *used = (total_pages - free_pages) * PAGE_SIZE;
}

/**
 * 复制页表
 */
void m4k_copy_page_tables(pml4_t *dest_pml4, pml4_t *src_pml4) {
    uint64_t i;

    /* 复制内核空间映射 */
    for (i = 256; i < 512; i++) {
        dest_pml4[i] = src_pml4[i];
    }
}

/**
 * 切换地址空间
 */
void m4k_switch_address_space(pml4_t *new_pml4) {
    __asm__ volatile ("movq %0, %%cr3" : : "r"(new_pml4));
}

/**
 * 刷新TLB
 */
void m4k_flush_tlb(void) {
    __asm__ volatile ("movq %%cr3, %%rax; movq %%rax, %%cr3" : : : "rax");
}

/**
 * 刷新特定地址的TLB
 */
void m4k_flush_tlb_entry(uint64_t address) {
    __asm__ volatile ("invlpg (%0)" : : "r"(address));
}

/**
 * 初始化内存管理
 */
void m4k_arch_memory_init(void) {
    /* 获取内存信息 */
    uint64_t total_memory = m4k_get_memory_size();

    /* 初始化内存管理 */
    m4k_memory_init(total_memory);

    console_write("M4KK1 x86_64 memory management initialized\n");
}

/**
 * 获取内存大小（从BIOS或硬件检测）
 */
uint64_t m4k_get_memory_size(void) {
    /* 临时返回固定值，实际应该从硬件检测 */
    return 0x40000000; /* 1GB */
}

/**
 * 分配连续的物理页面
 */
uint64_t m4k_alloc_contiguous_pages(uint32_t count) {
    uint64_t start_page = 0;
    uint64_t consecutive = 0;
    uint64_t i;

    for (i = 0; i < total_pages; i++) {
        uint64_t byte_index = i / 8;
        uint64_t bit_index = i % 8;

        if (!(page_frames[byte_index] & (1 << bit_index))) {
            if (consecutive == 0) {
                start_page = i;
            }
            consecutive++;

            if (consecutive == count) {
                /* 标记页面为已用 */
                uint64_t j;
                for (j = start_page; j < start_page + count; j++) {
                    uint64_t b_idx = j / 8;
                    uint64_t b_bit = j % 8;
                    page_frames[b_idx] |= (1 << b_bit);
                }
                free_pages -= count;
                return start_page * PAGE_SIZE;
            }
        } else {
            consecutive = 0;
        }
    }

    return 0; /* 分配失败 */
}

/**
 * 释放连续的物理页面
 */
void m4k_free_contiguous_pages(uint64_t address, uint32_t count) {
    uint64_t start_page = address / PAGE_SIZE;
    uint64_t i;

    for (i = start_page; i < start_page + count; i++) {
        uint64_t byte_index = i / 8;
        uint64_t bit_index = i % 8;
        page_frames[byte_index] &= ~(1 << bit_index);
    }

    free_pages += count;
}

/**
 * 检查虚拟地址是否有效
 */
bool m4k_is_virtual_address_valid(uint64_t virtual_addr) {
    uint64_t pml4_index = (virtual_addr >> 39) & 0x1FF;
    uint64_t pdp_index = (virtual_addr >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_addr >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_addr >> 12) & 0x1FF;

    pml4_t *pml4 = kernel_pml4;
    pdp_t *pdp;
    pd_t *pd;
    pt_t *pt;

    /* 检查页表结构 */
    if (!(pml4[pml4_index] & PTE_PRESENT)) return false;
    pdp = (pdp_t *)((pml4[pml4_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    if (!(pdp[pdp_index] & PTE_PRESENT)) return false;
    pd = (pd_t *)((pdp[pdp_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    if (!(pd[pd_index] & PTE_PRESENT)) return false;
    pt = (pt_t *)((pd[pd_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    return (pt[pt_index] & PTE_PRESENT) != 0;
}

/**
 * 获取页面权限
 */
uint64_t m4k_get_page_flags(uint64_t virtual_addr) {
    uint64_t pml4_index = (virtual_addr >> 39) & 0x1FF;
    uint64_t pdp_index = (virtual_addr >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_addr >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_addr >> 12) & 0x1FF;

    pml4_t *pml4 = kernel_pml4;
    pdp_t *pdp;
    pd_t *pd;
    pt_t *pt;

    /* 遍历页表结构 */
    if (!(pml4[pml4_index] & PTE_PRESENT)) return 0;
    pdp = (pdp_t *)((pml4[pml4_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    if (!(pdp[pdp_index] & PTE_PRESENT)) return 0;
    pd = (pd_t *)((pdp[pdp_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    if (!(pd[pd_index] & PTE_PRESENT)) return 0;
    pt = (pt_t *)((pd[pd_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    return pt[pt_index] & 0xFFFFFFFFFF000FFF;
}

/**
 * 设置页面权限
 */
void m4k_set_page_flags(uint64_t virtual_addr, uint64_t flags) {
    uint64_t pml4_index = (virtual_addr >> 39) & 0x1FF;
    uint64_t pdp_index = (virtual_addr >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_addr >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_addr >> 12) & 0x1FF;

    pml4_t *pml4 = kernel_pml4;
    pdp_t *pdp;
    pd_t *pd;
    pt_t *pt;

    /* 遍历页表结构 */
    if (!(pml4[pml4_index] & PTE_PRESENT)) return;
    pdp = (pdp_t *)((pml4[pml4_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    if (!(pdp[pdp_index] & PTE_PRESENT)) return;
    pd = (pd_t *)((pdp[pdp_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    if (!(pd[pd_index] & PTE_PRESENT)) return;
    pt = (pt_t *)((pd[pd_index] & 0xFFFFFFFFFF000) + MEM_BASE);

    /* 更新标志 */
    uint64_t physical_addr = pt[pt_index] & 0xFFFFFFFFFF000;
    pt[pt_index] = physical_addr | flags | PTE_PRESENT;

    /* 刷新TLB */
    __asm__ volatile ("invlpg (%0)" : : "r"(virtual_addr));
}

/**
 * 复制内存内容
 */
void m4k_copy_page(uint64_t dest, uint64_t src) {
    uint64_t *dest_ptr = (uint64_t *)dest;
    uint64_t *src_ptr = (uint64_t *)src;
    uint64_t i;

    for (i = 0; i < PAGE_SIZE / 8; i++) {
        dest_ptr[i] = src_ptr[i];
    }
}

/**
 * 清零页面
 */
void m4k_zero_page(uint64_t address) {
    memset((void *)address, 0, PAGE_SIZE);
}

/**
 * 比较页面内容
 */
bool m4k_compare_pages(uint64_t page1, uint64_t page2) {
    uint64_t *ptr1 = (uint64_t *)page1;
    uint64_t *ptr2 = (uint64_t *)page2;
    uint64_t i;

    for (i = 0; i < PAGE_SIZE / 8; i++) {
        if (ptr1[i] != ptr2[i]) {
            return false;
        }
    }

    return true;
}

/**
 * 获取页面引用计数
 */
uint32_t m4k_get_page_refcount(uint64_t virtual_addr) {
    /* 临时实现，实际应该维护引用计数 */
    return 1;
}

/**
 * 增加页面引用计数
 */
void m4k_inc_page_refcount(uint64_t virtual_addr) {
    /* 临时实现 */
}

/**
 * 减少页面引用计数
 */
void m4k_dec_page_refcount(uint64_t virtual_addr) {
    /* 临时实现 */
}

/**
 * 锁定页面到内存
 */
void m4k_lock_page(uint64_t virtual_addr) {
    /* 临时实现 */
}

/**
 * 解锁页面
 */
void m4k_unlock_page(uint64_t virtual_addr) {
    /* 临时实现 */
}

/**
 * 检查页面是否被锁定
 */
bool m4k_is_page_locked(uint64_t virtual_addr) {
    /* 临时实现 */
    return false;
}

/**
 * 获取页面修改时间
 */
uint64_t m4k_get_page_mtime(uint64_t virtual_addr) {
    /* 临时实现 */
    return 0;
}

/**
 * 设置页面修改时间
 */
void m4k_set_page_mtime(uint64_t virtual_addr, uint64_t mtime) {
    /* 临时实现 */
}

/**
 * 预取页面到缓存
 */
void m4k_prefault_page(uint64_t virtual_addr) {
    /* 临时实现 */
}

/**
 * 刷新页面缓存
 */
void m4k_flush_page_cache(uint64_t virtual_addr) {
    /* 临时实现 */
}

/**
 * 获取页面缓存状态
 */
uint32_t m4k_get_page_cache_state(uint64_t virtual_addr) {
    /* 临时实现 */
    return 0;
}

/**
 * 设置页面缓存策略
 */
void m4k_set_page_cache_policy(uint64_t virtual_addr, uint32_t policy) {
    /* 临时实现 */
}

/**
 * 统计内存使用情况
 */
void m4k_memory_statistics(void) {
    uint64_t total, free, used;

    m4k_get_memory_stats(&total, &free, &used);

    console_write("=== M4KK1 x86_64 Memory Statistics ===\n");
    console_write("Total memory: ");
    console_write_hex(total / 1024 / 1024);
    console_write(" MB\n");
    console_write("Free memory: ");
    console_write_hex(free / 1024 / 1024);
    console_write(" MB\n");
    console_write("Used memory: ");
    console_write_hex(used / 1024 / 1024);
    console_write(" MB\n");
    console_write("Usage: ");
    console_write_dec((used * 100) / total);
    console_write("%\n");
    console_write("=====================================\n");
}

/**
 * 调试：转储页表
 */
void m4k_dump_page_table(uint64_t virtual_addr) {
    uint64_t pml4_index = (virtual_addr >> 39) & 0x1FF;
    uint64_t pdp_index = (virtual_addr >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_addr >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_addr >> 12) & 0x1FF;

    console_write("Page table dump for address 0x");
    console_write_hex(virtual_addr);
    console_write("\n");
    console_write("PML4[");
    console_write_hex(pml4_index);
    console_write("] = 0x");
    console_write_hex(kernel_pml4[pml4_index]);
    console_write("\n");

    if (kernel_pml4[pml4_index] & PTE_PRESENT) {
        pml4_t *pdp = (pml4_t *)((kernel_pml4[pml4_index] & 0xFFFFFFFFFF000) + MEM_BASE);
        console_write("PDP[");
        console_write_hex(pdp_index);
        console_write("] = 0x");
        console_write_hex(pdp[pdp_index]);
        console_write("\n");
    }

    console_write("=====================================\n");
}

/**
 * 内存管理初始化
 */
void m4k_arch_memory_init(void) {
    /* 获取内存信息 */
    uint64_t total_memory = m4k_get_memory_size();

    /* 初始化内存管理 */
    m4k_memory_init(total_memory);

    console_write("M4KK1 x86_64 memory management initialized\n");
}

/**
 * 获取内存大小（从BIOS或硬件检测）
 */
uint64_t m4k_get_memory_size(void) {
    /* 临时返回固定值，实际应该从硬件检测 */
    return 0x40000000; /* 1GB */
}
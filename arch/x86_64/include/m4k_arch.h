/**
 * M4KK1 x86_64 Architecture Definitions
 * x86_64架构特定定义
 */

#ifndef _M4K_ARCH_X86_64_H
#define _M4K_ARCH_X86_64_H

#include <stdint.h>

/* 架构标识 */
#define M4K_ARCH_X86_64         1
#define M4K_ARCH_NAME           "x86_64"
#define M4K_ARCH_BITS           64

/* 内存布局 */
#define KERNEL_BASE             0xFFFFFFFF80000000ULL  /* -2GB */
#define KERNEL_HEAP             0xFFFFFFFF90000000ULL  /* -1.75GB */
#define USER_BASE               0x0000000000000000ULL  /* 0 */
#define USER_STACK_TOP          0x00007FFFFFFFFFFFULL  /* 128TB - 1 */

/* 栈大小 */
#define KERNEL_STACK_SIZE       0x1000     /* 4KB */
#define USER_STACK_SIZE         0x10000    /* 64KB */

/* 页面大小 */
#define PAGE_SIZE               0x1000     /* 4KB */
#define PAGE_SHIFT              12
#define PAGE_MASK               (~(PAGE_SIZE - 1))

/* 段选择子 */
#define KERNEL_CODE_SEGMENT     0x08
#define KERNEL_DATA_SEGMENT     0x10
#define USER_CODE_SEGMENT       0x18
#define USER_DATA_SEGMENT       0x20

/* 中断相关 */
#define IDT_ENTRIES             256
#define IDT_BASE                0x0000000000000000ULL
#define IDT_LIMIT               (IDT_ENTRIES * 16 - 1)

/* GDT相关 */
#define GDT_ENTRIES             5
#define GDT_BASE                0x0000000000001000ULL
#define GDT_LIMIT               (GDT_ENTRIES * 8 - 1)

/* TSS相关 */
#define TSS_BASE                0x0000000000002000ULL
#define TSS_LIMIT               0x67
#define TSS_SEGMENT             0x28

/* 系统调用 */
#define SYSCALL_INTERRUPT       0x80
#define M4K_SYSCALL_INTERRUPT   0x4D

/* 寄存器结构 */
typedef struct {
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;
    uint64_t rip, rflags, cr3;
    uint64_t cs, ss, ds, es, fs, gs;
} m4k_registers_t;

/* 中断栈帧 */
typedef struct {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} m4k_interrupt_frame_t;

/* 页表项 */
typedef uint64_t pte_t;
typedef uint64_t pde_t;
typedef uint64_t pdpte_t;
typedef uint64_t pml4e_t;

/* 页表标志 */
#define PTE_PRESENT             (1ULL << 0)
#define PTE_WRITABLE            (1ULL << 1)
#define PTE_USER                (1ULL << 2)
#define PTE_WRITE_THROUGH       (1ULL << 3)
#define PTE_CACHE_DISABLE       (1ULL << 4)
#define PTE_ACCESSED            (1ULL << 5)
#define PTE_DIRTY               (1ULL << 6)
#define PTE_LARGE_PAGE          (1ULL << 7)
#define PTE_GLOBAL              (1ULL << 8)
#define PTE_NO_EXECUTE          (1ULL << 63)

/* 内存类型范围寄存器 */
#define MSR_MTRR_BASE           0x200
#define MSR_MTRR_MASK           0x201
#define MSR_MTRR_DEF_TYPE       0x2FF

/* 内存类型 */
#define MTRR_TYPE_UC            0x00    /* Uncacheable */
#define MTRR_TYPE_WC            0x01    /* Write Combining */
#define MTRR_TYPE_WT            0x04    /* Write Through */
#define MTRR_TYPE_WP            0x05    /* Write Protected */
#define MTRR_TYPE_WB            0x06    /* Write Back */

/* CPU特性标志 */
#define CPUID_FEAT_ECX_SSE3     (1 << 0)
#define CPUID_FEAT_ECX_PCLMUL   (1 << 1)
#define CPUID_FEAT_ECX_DTES64   (1 << 2)
#define CPUID_FEAT_ECX_MONITOR  (1 << 3)
#define CPUID_FEAT_ECX_DS_CPL   (1 << 4)
#define CPUID_FEAT_ECX_VMX      (1 << 5)
#define CPUID_FEAT_ECX_SMX      (1 << 6)
#define CPUID_FEAT_ECX_EST      (1 << 7)
#define CPUID_FEAT_ECX_TM2      (1 << 8)
#define CPUID_FEAT_ECX_SSSE3    (1 << 9)
#define CPUID_FEAT_ECX_CID      (1 << 10)
#define CPUID_FEAT_ECX_FMA      (1 << 12)
#define CPUID_FEAT_ECX_CX16     (1 << 13)
#define CPUID_FEAT_ECX_ETPRD    (1 << 14)
#define CPUID_FEAT_ECX_PDCM     (1 << 15)
#define CPUID_FEAT_ECX_DCA      (1 << 18)
#define CPUID_FEAT_ECX_SSE4_1   (1 << 19)
#define CPUID_FEAT_ECX_SSE4_2   (1 << 20)
#define CPUID_FEAT_ECX_x2APIC   (1 << 21)
#define CPUID_FEAT_ECX_MOVBE    (1 << 22)
#define CPUID_FEAT_ECX_POPCNT   (1 << 23)
#define CPUID_FEAT_ECX_AES      (1 << 25)
#define CPUID_FEAT_ECX_XSAVE    (1 << 26)
#define CPUID_FEAT_ECX_OSXSAVE  (1 << 27)
#define CPUID_FEAT_ECX_AVX      (1 << 28)
#define CPUID_FEAT_ECX_F16C     (1 << 29)
#define CPUID_FEAT_ECX_RDRAND   (1 << 30)

/* 架构特定函数声明 */
void m4k_arch_init(void);
void m4k_arch_detect_features(void);
void m4k_arch_setup_paging(void);
void m4k_arch_enable_sse(void);
void m4k_arch_enable_avx(void);

/* 内联汇编辅助函数 */
static inline void m4k_cpuid(uint32_t leaf, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
    __asm__ volatile (
        "cpuid"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(leaf)
    );
}

static inline uint64_t m4k_read_msr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile (
        "rdmsr"
        : "=a"(low), "=d"(high)
        : "c"(msr)
    );
    return ((uint64_t)high << 32) | low;
}

static inline void m4k_write_msr(uint32_t msr, uint64_t value) {
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    __asm__ volatile (
        "wrmsr"
        : : "a"(low), "d"(high), "c"(msr)
    );
}

static inline void m4k_enable_interrupts(void) {
    __asm__ volatile ("sti");
}

static inline void m4k_disable_interrupts(void) {
    __asm__ volatile ("cli");
}

static inline void m4k_halt(void) {
    __asm__ volatile ("hlt");
}

static inline void m4k_pause(void) {
    __asm__ volatile ("pause");
}

static inline uint64_t m4k_read_cr0(void) {
    uint64_t value;
    __asm__ volatile ("movq %%cr0, %0" : "=r"(value));
    return value;
}

static inline void m4k_write_cr0(uint64_t value) {
    __asm__ volatile ("movq %0, %%cr0" : : "r"(value));
}

static inline uint64_t m4k_read_cr3(void) {
    uint64_t value;
    __asm__ volatile ("movq %%cr3, %0" : "=r"(value));
    return value;
}

static inline void m4k_write_cr3(uint64_t value) {
    __asm__ volatile ("movq %0, %%cr3" : : "r"(value));
}

static inline uint64_t m4k_read_cr4(void) {
    uint64_t value;
    __asm__ volatile ("movq %%cr4, %0" : "=r"(value));
    return value;
}

static inline void m4k_write_cr4(uint64_t value) {
    __asm__ volatile ("movq %0, %%cr4" : : "r"(value));
}

/* 原子操作 */
static inline uint32_t m4k_atomic_exchange(uint32_t *ptr, uint32_t value) {
    __asm__ volatile (
        "xchgl %0, %1"
        : "+r"(value), "+m"(*ptr)
        : : "memory"
    );
    return value;
}

static inline uint32_t m4k_atomic_compare_exchange(uint32_t *ptr, uint32_t old_val, uint32_t new_val) {
    uint32_t result;
    __asm__ volatile (
        "lock cmpxchgl %2, %1"
        : "=a"(result), "+m"(*ptr)
        : "r"(new_val), "0"(old_val)
        : "memory"
    );
    return result;
}

static inline uint32_t m4k_atomic_add(uint32_t *ptr, uint32_t value) {
    __asm__ volatile (
        "lock xaddl %0, %1"
        : "+r"(value), "+m"(*ptr)
        : : "memory"
    );
    return value;
}

static inline uint32_t m4k_atomic_increment(uint32_t *ptr) {
    return m4k_atomic_add(ptr, 1);
}

static inline uint32_t m4k_atomic_decrement(uint32_t *ptr) {
    return m4k_atomic_add(ptr, -1);
}

/* 内存屏障 */
static inline void m4k_memory_barrier(void) {
    __asm__ volatile ("mfence" : : : "memory");
}

static inline void m4k_read_barrier(void) {
    __asm__ volatile ("lfence" : : : "memory");
}

static inline void m4k_write_barrier(void) {
    __asm__ volatile ("sfence" : : : "memory");
}

#endif /* _M4K_ARCH_X86_64_H */
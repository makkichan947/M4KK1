; M4KK1 Y4KU Kernel Entry Point
; 内核入口汇编代码，负责内核初始化

BITS 32
SECTION .text

; 外部符号声明
extern kmain
extern __bss_start
extern __bss_end
extern __stack_top

; 内核魔数
KERNEL_MAGIC equ 0x4D344B4B

; 多引导协议信息结构
struc multiboot_info
    .flags          resd 1
    .mem_lower      resd 1
    .mem_upper      resd 1
    .boot_device    resd 1
    .cmdline        resd 1
    .mods_count     resd 1
    .mods_addr      resd 1
    .syms           resd 4
    .mmap_length    resd 1
    .mmap_addr      resd 1
    .drives_length  resd 1
    .drives_addr    resd 1
    .config_table   resd 1
    .boot_loader_name resd 1
    .apm_table      resd 1
    .vbe_control_info resd 1
    .vbe_mode_info  resd 1
    .vbe_mode       resw 1
    .vbe_interface_seg resw 1
    .vbe_interface_off resw 1
    .vbe_interface_len resw 1
endstruc

; 内核入口点
GLOBAL _start
_start:
    ; 保存多引导信息指针
    mov edi, ebx

    ; 保存魔数验证
    cmp eax, 0x2BADB002
    jne .invalid_magic

    ; 设置栈指针
    mov esp, __stack_top

    ; 清空BSS段
    call clear_bss

    ; 保存多引导信息
    push edi

    ; 调用内核主函数
    call kmain

    ; 如果kmain返回，进入无限循环
.halt:
    cli
    hlt
    jmp .halt

.invalid_magic:
    ; 魔数无效，无限循环
    jmp .halt

; 清空BSS段
clear_bss:
    push eax
    push ecx
    push edi
    mov edi, __bss_start
    mov ecx, __bss_end
    sub ecx, edi
    xor eax, eax
    rep stosb
    pop edi
    pop ecx
    pop eax
    ret

; 内核魔数
SECTION .rodata
    dd KERNEL_MAGIC

; 栈空间
SECTION .bootstrap_stack, nobits
    align 4096
    resb 16384          ; 16KB栈空间
__stack_top:
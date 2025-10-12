/**
 * M4KK1 Dynamic Linker Test Program
 * 动态链接器测试程序
 */

#include <syscall.h>
#include <console.h>
#include <string.h>

/* 测试动态库加载 */
void test_dl_load_library(void) {
    console_write("Testing dynamic library loading...\n");

    /* 尝试加载一个测试库（这里会失败，因为库不存在） */
    uint32_t handle = SYSCALL1(SYSCALL_DL_LOAD_LIBRARY, "test_lib.m4ll");

    if (handle == SYSCALL_ERROR) {
        console_write("Library loading failed (expected - no library file)\n");

        /* 获取错误信息 */
        char error_buf[256];
        uint32_t error_len = SYSCALL2(SYSCALL_DL_GET_ERROR, error_buf, sizeof(error_buf));

        if (error_len > 0) {
            console_write("Error: ");
            console_write(error_buf);
            console_write("\n");
        }
    } else {
        console_write("Library loaded successfully, handle: 0x");
        console_write_hex(handle);
        console_write("\n");
    }
}

/* 测试符号查找 */
void test_dl_find_symbol(void) {
    console_write("Testing symbol lookup...\n");

    /* 尝试查找一个内核符号 */
    uint32_t address = SYSCALL1(SYSCALL_DL_FIND_SYMBOL, "console_write");

    if (address == SYSCALL_ERROR) {
        console_write("Symbol lookup failed\n");

        /* 获取错误信息 */
        char error_buf[256];
        uint32_t error_len = SYSCALL2(SYSCALL_DL_GET_ERROR, error_buf, sizeof(error_buf));

        if (error_len > 0) {
            console_write("Error: ");
            console_write(error_buf);
            console_write("\n");
        }
    } else {
        console_write("Symbol found at address: 0x");
        console_write_hex(address);
        console_write("\n");
    }
}

/* 测试错误信息获取 */
void test_dl_get_error(void) {
    console_write("Testing error message retrieval...\n");

    /* 故意制造一个错误 */
    uint32_t result = SYSCALL1(SYSCALL_DL_LOAD_LIBRARY, (uint32_t)NULL);

    if (result == SYSCALL_ERROR) {
        char error_buf[256];
        uint32_t error_len = SYSCALL2(SYSCALL_DL_GET_ERROR, error_buf, sizeof(error_buf));

        if (error_len > 0) {
            console_write("Retrieved error message: ");
            console_write(error_buf);
            console_write("\n");
        } else {
            console_write("No error message available\n");
        }
    }
}

/* 主测试函数 */
void test_dynamic_linker(void) {
    console_write("\n=== M4KK1 Dynamic Linker Test ===\n");

    test_dl_load_library();
    console_write("\n");

    test_dl_find_symbol();
    console_write("\n");

    test_dl_get_error();
    console_write("\n");

    console_write("=== Dynamic Linker Test Complete ===\n\n");
}

/* 程序入口点 */
int main(void) {
    test_dynamic_linker();
    return 0;
}
/**
 * M4KK1 Test Framework - Test Runner
 * 测试框架 - 测试运行器
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../../sys/src/include/console.h"
#include "../../sys/src/include/memory.h"
#include "../../sys/src/include/process.h"
#include "../../sys/src/include/kernel.h"

/* 测试结果结构 */
typedef struct {
    const char *test_name;
    bool passed;
    const char *message;
    uint32_t execution_time;
} test_result_t;

/* 测试函数类型 */
typedef bool (*test_function_t)(void);

/* 测试用例结构 */
typedef struct test_case {
    const char *name;
    test_function_t function;
    struct test_case *next;
} test_case_t;

/* 全局测试列表 */
static test_case_t *test_list = NULL;
static uint32_t total_tests = 0;
static uint32_t passed_tests = 0;
static uint32_t failed_tests = 0;

/* 测试结果缓冲区 */
#define MAX_TEST_RESULTS 256
static test_result_t test_results[MAX_TEST_RESULTS];
static uint32_t result_count = 0;

/* 添加测试用例 */
void test_add_case(const char *name, test_function_t function) {
    test_case_t *test = (test_case_t *)kmalloc(sizeof(test_case_t));
    if (!test) {
        console_write("Failed to allocate memory for test case\n");
        return;
    }

    test->name = name;
    test->function = function;
    test->next = test_list;
    test_list = test;
    total_tests++;

    console_write("Test case added: ");
    console_write(name);
    console_write("\n");
}

/* 运行单个测试 */
static bool run_single_test(test_case_t *test, test_result_t *result) {
    uint32_t start_time, end_time;

    console_write("Running test: ");
    console_write(test->name);
    console_write("... ");

    /* 记录开始时间 */
    start_time = 0; /* 这里应该使用实际的计时器 */

    /* 运行测试 */
    bool passed = false;
    const char *message = NULL;

    if (test->function) {
        passed = test->function();
        if (passed) {
            message = "PASSED";
        } else {
            message = "FAILED";
        }
    } else {
        passed = false;
        message = "NO FUNCTION";
    }

    /* 记录结束时间 */
    end_time = 0; /* 这里应该使用实际的计时器 */
    uint32_t execution_time = end_time - start_time;

    /* 保存结果 */
    if (result) {
        result->test_name = test->name;
        result->passed = passed;
        result->message = message;
        result->execution_time = execution_time;
    }

    /* 更新统计 */
    if (passed) {
        passed_tests++;
        console_write("✓ PASSED");
    } else {
        failed_tests++;
        console_write("✗ FAILED");
    }

    console_write(" (");
    console_write_dec(execution_time);
    console_write("ms)\n");

    return passed;
}

/* 运行所有测试 */
void test_run_all(void) {
    test_case_t *test = test_list;
    uint32_t test_number = 1;

    console_write("\n");
    console_write("=====================================\n");
    console_write("    M4KK1 Test Framework\n");
    console_write("=====================================\n");
    console_write("\n");

    /* 重置统计信息 */
    passed_tests = 0;
    failed_tests = 0;
    result_count = 0;

    /* 运行所有测试 */
    while (test && result_count < MAX_TEST_RESULTS) {
        console_write("[");
        console_write_dec(test_number++);
        console_write("] ");

        run_single_test(test, &test_results[result_count]);
        result_count++;

        test = test->next;
    }

    /* 显示总结 */
    console_write("\n");
    console_write("=====================================\n");
    console_write("Test Summary:\n");
    console_write("  Total: ");
    console_write_dec(total_tests);
    console_write("\n");
    console_write("  Passed: ");
    console_write_dec(passed_tests);
    console_write("\n");
    console_write("  Failed: ");
    console_write_dec(failed_tests);
    console_write("\n");

    if (failed_tests == 0) {
        console_write("  Result: ✓ ALL TESTS PASSED\n");
    } else {
        console_write("  Result: ✗ SOME TESTS FAILED\n");
    }

    console_write("=====================================\n");
}

/* 获取测试统计信息 */
void test_get_stats(uint32_t *total, uint32_t *passed, uint32_t *failed) {
    if (total) *total = total_tests;
    if (passed) *passed = passed_tests;
    if (failed) *failed = failed_tests;
}

/* 打印详细测试结果 */
void test_print_results(void) {
    console_write("\nDetailed Test Results:\n");
    console_write("=====================================\n");

    for (uint32_t i = 0; i < result_count; i++) {
        console_write("[");
        console_write_dec(i + 1);
        console_write("] ");
        console_write(test_results[i].test_name);
        console_write(" - ");
        console_write(test_results[i].message);
        console_write(" (");
        console_write_dec(test_results[i].execution_time);
        console_write("ms)\n");
    }

    console_write("=====================================\n");
}

/* 内存分配测试 */
static bool test_memory_allocation(void) {
    void *ptr1 = kmalloc(1024);
    void *ptr2 = kmalloc(512);
    void *ptr3 = kmalloc(256);

    if (!ptr1 || !ptr2 || !ptr3) {
        return false;
    }

    /* 验证内存内容 */
    memset(ptr1, 0xAA, 1024);
    memset(ptr2, 0xBB, 512);
    memset(ptr3, 0xCC, 256);

    /* 释放内存 */
    kfree(ptr1);
    kfree(ptr2);
    kfree(ptr3);

    return true;
}

/* 字符串操作测试 */
static bool test_string_operations(void) {
    char buffer[256];
    const char *test_str = "Hello, M4KK1!";

    /* 测试字符串复制 */
    strcpy(buffer, test_str);
    if (strcmp(buffer, test_str) != 0) {
        return false;
    }

    /* 测试字符串长度 */
    if (strlen(buffer) != strlen(test_str)) {
        return false;
    }

    /* 测试字符串连接 */
    strcat(buffer, " Test");
    if (strcmp(buffer, "Hello, M4KK1! Test") != 0) {
        return false;
    }

    return true;
}

/* 进程创建测试 */
static bool test_process_creation(void) {
    process_t *process = process_create("test_process", PROCESS_PRIORITY_NORMAL);
    if (!process) {
        return false;
    }

    if (process->pid == 0 || strcmp(process->name, "test_process") != 0) {
        return false;
    }

    /* 清理测试进程 */
    process_destroy(process);

    return true;
}

/* 数学运算测试 */
static bool test_math_operations(void) {
    /* 基本算术测试 */
    uint32_t a = 100, b = 200, c = 0;

    c = a + b;
    if (c != 300) return false;

    c = b - a;
    if (c != 100) return false;

    c = a * b;
    if (c != 20000) return false;

    c = b / a;
    if (c != 2) return false;

    return true;
}

/* 初始化测试框架 */
void test_framework_init(void) {
    console_write("Initializing M4KK1 Test Framework...\n");

    /* 添加测试用例 */
    test_add_case("Memory Allocation Test", test_memory_allocation);
    test_add_case("String Operations Test", test_string_operations);
    test_add_case("Process Creation Test", test_process_creation);
    test_add_case("Math Operations Test", test_math_operations);

    console_write("Test framework initialized\n");
}

/* 运行测试框架 */
void test_framework_run(void) {
    test_framework_init();
    test_run_all();
    test_print_results();
}
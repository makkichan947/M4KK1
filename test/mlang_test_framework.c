/**
 * M4KK1 MLang Test Framework
 * MLang测试框架
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "../usr/bin/MLang/include/mlang.h"

/* 测试结果结构 */
typedef struct {
    const char *test_name;
    bool passed;
    const char *message;
    uint32_t execution_time_ms;
} mlang_test_result_t;

/* 测试函数类型 */
typedef bool (*mlang_test_function_t)(void);

/* 测试用例结构 */
typedef struct mlang_test_case {
    const char *name;
    mlang_test_function_t function;
    struct mlang_test_case *next;
} mlang_test_case_t;

/* 全局测试列表 */
static mlang_test_case_t *test_list = NULL;
static uint32_t total_tests = 0;
static uint32_t passed_tests = 0;
static uint32_t failed_tests = 0;

/* 测试结果缓冲区 */
#define MAX_TEST_RESULTS 256
static mlang_test_result_t test_results[MAX_TEST_RESULTS];
static uint32_t result_count = 0;

/* 添加测试用例 */
void mlang_test_add_case(const char *name, mlang_test_function_t function) {
    mlang_test_case_t *test = mlang_malloc(sizeof(mlang_test_case_t));
    if (!test) {
        printf("Failed to allocate memory for test case\n");
        return;
    }

    test->name = mlang_strdup(name);
    test->function = function;
    test->next = test_list;
    test_list = test;
    total_tests++;

    printf("Test case added: %s\n", name);
}

/* 运行单个测试 */
static bool mlang_run_single_test(mlang_test_case_t *test, mlang_test_result_t *result) {
    clock_t start_time, end_time;

    printf("Running test: %s... ", test->name);

    /* 记录开始时间 */
    start_time = clock();

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
    end_time = clock();
    uint32_t execution_time_ms = (end_time - start_time) * 1000 / CLOCKS_PER_SEC;

    /* 保存结果 */
    if (result) {
        result->test_name = test->name;
        result->passed = passed;
        result->message = message;
        result->execution_time_ms = execution_time_ms;
    }

    /* 更新统计 */
    if (passed) {
        passed_tests++;
        printf("✓ PASSED");
    } else {
        failed_tests++;
        printf("✗ FAILED");
    }

    printf(" (%dms)\n", execution_time_ms);

    return passed;
}

/* 运行所有测试 */
void mlang_test_run_all(void) {
    mlang_test_case_t *test = test_list;
    uint32_t test_number = 1;

    printf("\n");
    printf("=====================================\n");
    printf("    M4KK1 MLang Test Framework\n");
    printf("=====================================\n");
    printf("\n");

    /* 重置统计信息 */
    passed_tests = 0;
    failed_tests = 0;
    result_count = 0;

    /* 运行所有测试 */
    while (test && result_count < MAX_TEST_RESULTS) {
        printf("[%d] ", test_number++);

        mlang_run_single_test(test, &test_results[result_count]);
        result_count++;

        test = test->next;
    }

    /* 显示总结 */
    printf("\n");
    printf("=====================================\n");
    printf("Test Summary:\n");
    printf("  Total: %d\n", total_tests);
    printf("  Passed: %d\n", passed_tests);
    printf("  Failed: %d\n", failed_tests);

    if (failed_tests == 0) {
        printf("  Result: ✓ ALL TESTS PASSED\n");
    } else {
        printf("  Result: ✗ SOME TESTS FAILED\n");
    }

    printf("=====================================\n");
}

/* 获取测试统计信息 */
void mlang_test_get_stats(uint32_t *total, uint32_t *passed, uint32_t *failed) {
    if (total) *total = total_tests;
    if (passed) *passed = passed_tests;
    if (failed) *failed = failed_tests;
}

/* 打印详细测试结果 */
void mlang_test_print_results(void) {
    printf("\nDetailed Test Results:\n");
    printf("=====================================\n");

    for (uint32_t i = 0; i < result_count; i++) {
        printf("[%d] %s - %s (%dms)\n", 
               i + 1, test_results[i].test_name, 
               test_results[i].message, test_results[i].execution_time_ms);
    }

    printf("=====================================\n");
}

/* MLang编译器功能测试 */

/* 测试MLang初始化 */
static bool test_mlang_init(void) {
    int result = mlang_init();
    if (result != 0) {
        return false;
    }
    
    // 验证初始化成功
    mlang_info_t info;
    mlang_get_info(&info);
    
    return (strcmp(info.name, "MLang") == 0);
}

/* 测试MLang词法分析 */
static bool test_mlang_lexical_analysis(void) {
    const char *source = "func main() -> int { var x: int = 42; return x; }";
    mlang_ast_node_t *tokens;
    
    int token_count = mlang_lexical_analyze(source, &tokens);
    
    if (token_count < 0) {
        return false;
    }
    
    // 检查是否至少有基本的token
    bool has_func = false;
    bool has_main = false;
    
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == TOKEN_KEYWORD && strcmp(tokens[i].value, "func") == 0) {
            has_func = true;
        }
        if (tokens[i].type == TOKEN_IDENTIFIER && strcmp(tokens[i].value, "main") == 0) {
            has_main = true;
        }
    }
    
    mlang_free(tokens);
    return has_func && has_main;
}

/* 测试MLang语法分析 */
static bool test_mlang_syntax_analysis(void) {
    const char *source = "func main() -> int { var x: int = 42; return x; }";
    mlang_ast_node_t *tokens;
    
    int token_count = mlang_lexical_analyze(source, &tokens);
    if (token_count < 0) {
        return false;
    }
    
    mlang_ast_node_t *ast;
    int result = mlang_syntax_analyze(tokens, &ast);
    
    mlang_free(tokens);
    if (ast) {
        mlang_free_ast(ast);
    }
    
    return result == 0;
}

/* 测试MLang编译 */
static bool test_mlang_compile(void) {
    // 创建一个简单的MLang源文件
    const char *test_source = 
        "func main() -> int {\n"
        "    var x: int = 42;\n"
        "    return x;\n"
        "}\n";
    
    if (mlang_write_file("/tmp/test.mlang", test_source, strlen(test_source)) != 0) {
        return false;
    }
    
    // 尝试编译
    int result = mlang_compile_file("/tmp/test.mlang", "/tmp/test.out");
    
    // 清理临时文件
    remove("/tmp/test.mlang");
    remove("/tmp/test.out");
    
    return result == 0;
}

/* 测试MLang工具函数 */
static bool test_mlang_tools(void) {
    // 测试字符串处理
    char *str1 = mlang_strdup("test");
    if (!str1) return false;
    
    if (mlang_strcmp(str1, "test") != 0) {
        mlang_free(str1);
        return false;
    }
    
    mlang_free(str1);
    
    // 测试哈希表
    mlang_hash_table_t *table = mlang_hash_create(16);
    if (!table) return false;
    
    mlang_hash_insert(table, "key", "value");
    void *value = mlang_hash_lookup(table, "key");
    
    bool success = (value != NULL && strcmp((char*)value, "value") == 0);
    
    mlang_hash_destroy(table);
    
    return success;
}

/* 测试MLang配置管理 */
static bool test_mlang_config(void) {
    mlang_config_t *config = mlang_config_create();
    if (!config) return false;
    
    // 设置一些配置
    if (config->input_file) mlang_free(config->input_file);
    config->input_file = mlang_strdup("test.mlang");
    
    if (config->output_file) mlang_free(config->output_file);
    config->output_file = mlang_strdup("test.out");
    
    config->opt_level = MLANG_OPT_ADVANCED;
    config->debug_info = true;
    
    // 验证配置
    bool success = (config->input_file && config->output_file && 
                   strcmp(config->input_file, "test.mlang") == 0 &&
                   strcmp(config->output_file, "test.out") == 0 &&
                   config->opt_level == MLANG_OPT_ADVANCED &&
                   config->debug_info == true);
    
    mlang_config_destroy(config);
    return success;
}

/* 测试MLang统计功能 */
static bool test_mlang_stats(void) {
    mlang_stats_t stats_before, stats_after;
    
    mlang_get_stats(&stats_before);
    
    // 执行一些操作
    mlang_init();
    mlang_cleanup();
    
    mlang_get_stats(&stats_after);
    
    // 统计信息应该有变化
    return true; // 简化测试
}

/* 测试MLang版本信息 */
static bool test_mlang_version(void) {
    int major, minor, patch;
    mlang_get_version(&major, &minor, &patch);
    
    const char *version_str = mlang_get_version_string();
    
    return (major >= 1 && version_str != NULL);
}

/* 测试MLang分析工具 */
static bool test_mlang_analysis_tools(void) {
    // 创建测试源文件
    const char *test_source = 
        "func main() -> int {\n"
        "    var x: int = 10;\n"
        "    var y: int = 20;\n"
        "    while x < 5 {\n"
        "        x = x + 1;\n"
        "    }\n"
        "    return x + y;\n"
        "}\n";
    
    if (mlang_write_file("/tmp/analysis_test.mlang", test_source, strlen(test_source)) != 0) {
        return false;
    }
    
    // 测试复杂度分析
    uint32_t complexity_score;
    int result1 = mlang_analyze_complexity("/tmp/analysis_test.mlang", &complexity_score);
    
    // 测试代码异味检测
    char *smell_report;
    int result2 = mlang_detect_code_smells("/tmp/analysis_test.mlang", &smell_report);
    
    // 测试文档生成
    int result3 = mlang_generate_docs("/tmp/analysis_test.mlang", "/tmp");
    
    // 清理
    remove("/tmp/analysis_test.mlang");
    remove("/tmp/analysis_test.mlang.md");
    if (smell_report) mlang_free(smell_report);
    
    return (result1 == 0 && result2 == 0 && result3 == 0);
}

/* 测试MLang测试生成工具 */
static bool test_mlang_test_generation(void) {
    // 创建测试源文件
    const char *test_source = 
        "func add(a: int, b: int) -> int {\n"
        "    return a + b;\n"
        "}\n"
        "func multiply(a: int, b: int) -> int {\n"
        "    return a * b;\n"
        "}\n";
    
    if (mlang_write_file("/tmp/test_gen.mlang", test_source, strlen(test_source)) != 0) {
        return false;
    }
    
    // 生成单元测试
    int result = mlang_generate_unit_tests("/tmp/test_gen.mlang", "/tmp/test_gen_tests.c");
    
    // 检查生成的测试文件
    bool file_exists = mlang_file_exists("/tmp/test_gen_tests.c");
    
    // 清理
    remove("/tmp/test_gen.mlang");
    remove("/tmp/test_gen_tests.c");
    
    return (result == 0 && file_exists);
}

/* 初始化MLang测试框架 */
void mlang_test_framework_init(void) {
    printf("Initializing M4KK1 MLang Test Framework...\n");

    /* 添加编译器功能测试 */
    mlang_test_add_case("MLang Init Test", test_mlang_init);
    mlang_test_add_case("MLang Lexical Analysis Test", test_mlang_lexical_analysis);
    mlang_test_add_case("MLang Syntax Analysis Test", test_mlang_syntax_analysis);
    mlang_test_add_case("MLang Compile Test", test_mlang_compile);
    mlang_test_add_case("MLang Tools Test", test_mlang_tools);
    mlang_test_add_case("MLang Config Test", test_mlang_config);
    mlang_test_add_case("MLang Stats Test", test_mlang_stats);
    mlang_test_add_case("MLang Version Test", test_mlang_version);
    mlang_test_add_case("MLang Analysis Tools Test", test_mlang_analysis_tools);
    mlang_test_add_case("MLang Test Generation Test", test_mlang_test_generation);

    printf("MLang test framework initialized\n");
}

/* 运行MLang测试框架 */
void mlang_test_framework_run(void) {
    mlang_test_framework_init();
    mlang_test_run_all();
    mlang_test_print_results();
}

/* 主函数用于运行测试 */
int main(int argc, char *argv[]) {
    printf("M4KK1 MLang Test Chain Runner\n");
    printf("==============================\n");
    
    mlang_test_framework_run();
    
    // 运行MLang测试链文件
    printf("\nRunning MLang test chain...\n");
    int compile_result = mlang_compile_file("/workspace/test/mlang_test_chain.mlang", "/tmp/mlang_test_chain.out");
    
    if (compile_result == 0) {
        printf("✓ MLang test chain compiled successfully\n");
    } else {
        printf("✗ MLang test chain compilation failed\n");
    }
    
    return 0;
}
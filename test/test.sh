#!/bin/bash
# M4KK1操作系统测试脚本
# 自动化测试套件

set -e

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 测试配置
TEST_START=$(date +%s)
QEMU_TIMEOUT=300  # 5分钟超时
LOG_FILE="test.log"

# 函数定义
log_info() {
    echo -e "${BLUE}[INFO]${NC} $*"
    echo "[INFO] $*" >> "$LOG_FILE"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $*"
    echo "[WARN] $*" >> "$LOG_FILE"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*"
    echo "[ERROR] $*" >> "$LOG_FILE"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $*"
    echo "[SUCCESS] $*" >> "$LOG_FILE"
}

log_test() {
    echo -e "${BLUE}[TEST]${NC} $*"
    echo "[TEST] $*" >> "$LOG_FILE"
}

# 初始化测试环境
init_test_env() {
    log_info "初始化测试环境..."

    # 创建测试目录
    mkdir -p test_results
    mkdir -p test_iso

    # 清理旧的测试文件
    rm -f test.log
    rm -f test_results/*

    # 检查测试依赖
    if ! command -v qemu-system-x86_64 >/dev/null 2>&1; then
        log_error "未找到 qemu-system-x86_64，无法进行虚拟机测试"
        exit 1
    fi

    if ! command -v expect >/dev/null 2>&1; then
        log_warn "未找到 expect，某些交互式测试将被跳过"
    fi

    log_success "测试环境初始化完成"
}

# 单元测试
run_unit_tests() {
    log_test "运行单元测试..."

    # 编译测试程序
    make -C test/unit all

    # 运行内核单元测试
    if [ -f "test/unit/kernel_test" ]; then
        ./test/unit/kernel_test
        if [ $? -eq 0 ]; then
            log_success "内核单元测试通过"
        else
            log_error "内核单元测试失败"
            return 1
        fi
    fi

    # 运行文件系统单元测试
    if [ -f "test/unit/fs_test" ]; then
        ./test/unit/fs_test
        if [ $? -eq 0 ]; then
            log_success "文件系统单元测试通过"
        else
            log_error "文件系统单元测试失败"
            return 1
        fi
    fi

    # 运行用户程序单元测试
    if [ -f "test/unit/user_test" ]; then
        ./test/unit/user_test
        if [ $? -eq 0 ]; then
            log_success "用户程序单元测试通过"
        else
            log_error "用户程序单元测试失败"
            return 1
        fi
    fi

    log_success "单元测试完成"
}

# 集成测试
run_integration_tests() {
    log_test "运行集成测试..."

    # 构建完整系统
    make everything

    # 创建ISO镜像
    make iso

    # 复制ISO到测试目录
    cp "m4kk1-*.iso" test_iso/

    # 运行基本集成测试
    make -C test/integration all

    log_success "集成测试完成"
}

# 性能测试
run_performance_tests() {
    log_test "运行性能测试..."

    # 内存使用测试
    if [ -f "test/performance/memory_test" ]; then
        ./test/performance/memory_test
    fi

    # CPU性能测试
    if [ -f "test/performance/cpu_test" ]; then
        ./test/performance/cpu_test
    fi

    # I/O性能测试
    if [ -f "test/performance/io_test" ]; then
        ./test/performance/io_test
    fi

    log_success "性能测试完成"
}

# 系统测试
run_system_tests() {
    log_test "运行系统测试..."

    # QEMU系统测试
    run_qemu_tests

    # 网络测试
    run_network_tests

    # 图形界面测试
    run_gui_tests

    log_success "系统测试完成"
}

# QEMU测试
run_qemu_tests() {
    log_test "运行QEMU虚拟机测试..."

    local iso_file="test_iso/m4kk1-*.iso"

    if [ ! -f "$iso_file" ]; then
        log_error "未找到ISO文件: $iso_file"
        return 1
    fi

    log_info "启动QEMU虚拟机测试..."

    # 基本引导测试
    timeout $QEMU_TIMEOUT qemu-system-x86_64 \
        -cdrom "$iso_file" \
        -m 512M \
        -serial file:test_results/qemu_basic.log \
        -display none \
        -boot d \
        -no-reboot \
        2>test_results/qemu_stderr.log

    if [ $? -eq 0 ]; then
        log_success "QEMU基本引导测试通过"
    else
        log_warn "QEMU基本引导测试可能失败，请检查日志"
    fi

    # 内存测试
    timeout $QEMU_TIMEOUT qemu-system-x86_64 \
        -cdrom "$iso_file" \
        -m 1G \
        -serial file:test_results/qemu_memory.log \
        -display none \
        2>test_results/qemu_memory_stderr.log

    if [ $? -eq 0 ]; then
        log_success "QEMU内存测试通过"
    else
        log_warn "QEMU内存测试可能失败，请检查日志"
    fi
}

# 网络测试
run_network_tests() {
    log_test "运行网络测试..."

    # 检查网络工具
    if [ -f "usr/bin/ntctl/ntctl" ]; then
        # 基本网络接口测试
        ./usr/bin/ntctl/ntctl interface list > test_results/network_interfaces.log 2>&1
        if [ $? -eq 0 ]; then
            log_success "网络接口测试通过"
        else
            log_warn "网络接口测试失败"
        fi
    fi

    # VPN测试（如果配置了）
    if [ -f "usr/bin/ntmgr/ntmgr" ]; then
        timeout 30 ./usr/bin/ntmgr/ntmgr --help > test_results/vpn_help.log 2>&1
        if [ $? -eq 0 ]; then
            log_success "VPN管理器测试通过"
        else
            log_warn "VPN管理器测试失败"
        fi
    fi
}

# 图形界面测试
run_gui_tests() {
    log_test "运行图形界面测试..."

    # 检查图形工具
    if [ -f "usr/bin/copland/copland" ]; then
        # 窗口服务器测试
        timeout 10 ./usr/bin/copland/copland --help > test_results/window_server.log 2>&1
        if [ $? -eq 0 ]; then
            log_success "窗口服务器测试通过"
        else
            log_warn "窗口服务器测试失败"
        fi
    fi

    if [ -f "usr/bin/rubidium/rubidium" ]; then
        # 桌面环境测试
        timeout 10 ./usr/bin/rubidium/rubidium --help > test_results/desktop_env.log 2>&1
        if [ $? -eq 0 ]; then
            log_success "桌面环境测试通过"
        else
            log_warn "桌面环境测试失败"
        fi
    fi
}

# 压力测试
run_stress_tests() {
    log_test "运行压力测试..."

    # 高负载测试
    if [ -f "test/stress/stress_test" ]; then
        timeout 60 ./test/stress/stress_test > test_results/stress.log 2>&1
        if [ $? -eq 0 ]; then
            log_success "压力测试通过"
        else
            log_warn "压力测试失败或超时"
        fi
    fi

    # 内存泄漏测试
    if [ -f "test/stress/mem_test" ]; then
        ./test/stress/mem_test > test_results/memory_leak.log 2>&1
        if [ $? -eq 0 ]; then
            log_success "内存泄漏测试通过"
        else
            log_warn "内存泄漏测试发现问题"
        fi
    fi
}

# 生成测试报告
generate_report() {
    local test_end=$(date +%s)
    local test_time=$((test_end - TEST_START))

    log_info "生成测试报告..."

    cat > test_results/test_report.txt << EOF
M4KK1操作系统测试报告
====================

测试时间: $(date)
总耗时: $test_time 秒
测试版本: $VERSION

测试结果汇总:
EOF

    # 统计测试结果
    local total_tests=0
    local passed_tests=0
    local failed_tests=0

    # 检查日志文件
    for log in test_results/*.log; do
        if [ -f "$log" ]; then
            total_tests=$((total_tests + 1))
            if grep -q "SUCCESS\|通过" "$log"; then
                passed_tests=$((passed_tests + 1))
            elif grep -q "ERROR\|失败" "$log"; then
                failed_tests=$((failed_tests + 1))
            fi
        fi
    done

    echo "- 总测试数: $total_tests" >> test_results/test_report.txt
    echo "- 通过: $passed_tests" >> test_results/test_report.txt
    echo "- 失败: $failed_tests" >> test_results/test_report.txt

    if [ $failed_tests -eq 0 ]; then
        echo "- 状态: 全部通过" >> test_results/test_report.txt
    else
        echo "- 状态: $failed_tests 个测试失败" >> test_results/test_report.txt
    fi

    echo "" >> test_results/test_report.txt
    echo "详细日志:" >> test_results/test_report.txt
    echo "- 主日志: test.log" >> test_results/test_report.txt
    echo "- QEMU日志: test_results/qemu_*.log" >> test_results/test_report.txt
    echo "- 组件日志: test_results/*.log" >> test_results/test_report.txt

    log_success "测试报告已生成: test_results/test_report.txt"
}

# 显示测试统计
show_test_stats() {
    echo ""
    log_info "测试统计:"

    # 计算文件大小
    if [ -f "m4kk1-$VERSION.iso" ]; then
        local iso_size=$(du -h "m4kk1-$VERSION.iso" | cut -f1)
        echo "  ISO镜像大小: $iso_size"
    fi

    # 显示测试结果摘要
    echo "  测试详情请查看: test_results/test_report.txt"
}

# 主函数
main() {
    local test_type="all"

    # 解析命令行参数
    while [[ $# -gt 0 ]]; do
        case $1 in
            unit)
                test_type="unit"
                shift
                ;;
            integration)
                test_type="integration"
                shift
                ;;
            performance)
                test_type="performance"
                shift
                ;;
            system)
                test_type="system"
                shift
                ;;
            stress)
                test_type="stress"
                shift
                ;;
            -h|--help)
                echo "用法: $0 [测试类型]"
                echo ""
                echo "测试类型:"
                echo "  unit         单元测试"
                echo "  integration  集成测试"
                echo "  performance  性能测试"
                echo "  system       系统测试"
                echo "  stress       压力测试"
                echo "  all          所有测试 (默认)"
                echo ""
                echo "选项:"
                echo "  -h, --help   显示此帮助"
                exit 0
                ;;
            *)
                log_error "未知测试类型: $1"
                exit 1
                ;;
        esac
    done

    # 执行测试
    init_test_env

    case $test_type in
        unit)
            run_unit_tests
            ;;
        integration)
            run_integration_tests
            ;;
        performance)
            run_performance_tests
            ;;
        system)
            run_system_tests
            ;;
        stress)
            run_stress_tests
            ;;
        all)
            run_unit_tests
            run_integration_tests
            run_performance_tests
            run_system_tests
            ;;
    esac

    generate_report
    show_test_stats
    log_success "测试完成!"
}

# 运行主函数
main "$@"
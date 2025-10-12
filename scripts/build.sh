#!/bin/bash
# M4KK1操作系统构建脚本
# 自动化构建和测试脚本

set -e

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 构建信息
BUILD_START=$(date +%s)
VERSION="0.1.0"
JOBS=$(nproc 2>/dev/null || echo 4)

echo -e "${BLUE}M4KK1操作系统构建脚本${NC}"
echo -e "${BLUE}版本: $VERSION${NC}"
echo -e "${BLUE}构建时间: $(date)${NC}"
echo ""

# 函数定义
log_info() {
    echo -e "${BLUE}[INFO]${NC} $*"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $*"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $*"
}

# 检查依赖
check_dependencies() {
    log_info "检查构建依赖..."

    local missing_deps=()

    # 检查编译器 (使用gcc作为替代)
    if ! command -v gcc >/dev/null 2>&1; then
        missing_deps+=("gcc")
    fi

    # 检查汇编器
    if ! command -v nasm >/dev/null 2>&1; then
        missing_deps+=("nasm")
    fi

    # 检查QEMU
    if ! command -v qemu-system-x86_64 >/dev/null 2>&1; then
        log_warn "未找到 qemu-system-x86_64，将无法进行虚拟机测试"
    fi

    # 检查其他工具
    for tool in xorriso grub-mkrescue; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            log_warn "未找到 $tool，将无法创建ISO镜像"
        fi
    done

    if [ ${#missing_deps[@]} -ne 0 ]; then
        log_error "缺少必要的依赖: ${missing_deps[*]}"
        log_info "请运行 'make dev-setup' 来安装依赖"
        exit 1
    fi

    log_success "依赖检查完成"
}

# 清理构建
clean_build() {
    log_info "清理旧的构建文件..."
    make clean
    log_success "清理完成"
}

# 构建内核
build_kernel() {
    log_info "构建M4KK1内核..."
    make kernel
    if [ $? -eq 0 ]; then
        log_success "内核构建完成"
    else
        log_error "内核构建失败"
        exit 1
    fi
}

# 构建引导程序
build_bootloader() {
    log_info "构建Bootcamp引导程序..."
    make bootloader
    if [ $? -eq 0 ]; then
        log_success "引导程序构建完成"
    else
        log_error "引导程序构建失败"
        exit 1
    fi
}

# 构建系统工具
build_system_tools() {
    log_info "构建系统工具..."
    make system-tools
    if [ $? -eq 0 ]; then
        log_success "系统工具构建完成"
    else
        log_error "系统工具构建失败"
        exit 1
    fi
}

# 构建用户工具
build_user_tools() {
    log_info "构建用户工具..."
    make user-tools
    if [ $? -eq 0 ]; then
        log_success "用户工具构建完成"
    else
        log_error "用户工具构建失败"
        exit 1
    fi
}

# 构建完整系统
build_everything() {
    log_info "构建完整M4KK1系统..."
    make everything
    if [ $? -eq 0 ]; then
        log_success "完整系统构建完成"
    else
        log_error "系统构建失败"
        exit 1
    fi
}

# 运行测试
run_tests() {
    log_info "运行测试套件..."
    make test-all
    if [ $? -eq 0 ]; then
        log_success "所有测试通过"
    else
        log_warn "部分测试失败，请检查输出"
    fi
}

# 创建ISO镜像
create_iso() {
    log_info "创建M4KK1 ISO镜像..."
    make iso
    if [ $? -eq 0 ]; then
        log_success "ISO镜像创建完成"
    else
        log_error "ISO镜像创建失败"
        exit 1
    fi
}

# 显示构建统计
show_stats() {
    BUILD_END=$(date +%s)
    BUILD_TIME=$((BUILD_END - BUILD_START))

    echo ""
    log_info "构建统计:"
    echo "  总耗时: $BUILD_TIME 秒"
    echo "  CPU核心数: $JOBS"
    echo "  平均速度: $(echo "scale=2; $BUILD_TIME / $JOBS" | bc) 秒/核心"

    if [ -f "m4kk1-$VERSION.iso" ]; then
        ISO_SIZE=$(du -h "m4kk1-$VERSION.iso" | cut -f1)
        echo "  ISO大小: $ISO_SIZE"
    fi
}

# 主函数
main() {
    local target="all"

    # 解析命令行参数
    while [[ $# -gt 0 ]]; do
        case $1 in
            kernel)
                target="kernel"
                shift
                ;;
            bootloader)
                target="bootloader"
                shift
                ;;
            tools)
                target="tools"
                shift
                ;;
            system-tools)
                target="system-tools"
                shift
                ;;
            user-tools)
                target="user-tools"
                shift
                ;;
            everything)
                target="everything"
                shift
                ;;
            test)
                target="test"
                shift
                ;;
            iso)
                target="iso"
                shift
                ;;
            clean)
                clean_build
                exit 0
                ;;
            -j|--jobs)
                JOBS="$2"
                shift 2
                ;;
            -h|--help)
                echo "用法: $0 [目标] [选项]"
                echo ""
                echo "目标:"
                echo "  kernel       构建内核"
                echo "  bootloader   构建引导程序"
                echo "  tools        构建所有工具"
                echo "  system-tools 构建系统工具"
                echo "  user-tools   构建用户工具"
                echo "  everything   构建完整系统"
                echo "  test         运行测试"
                echo "  iso          创建ISO镜像"
                echo "  clean        清理构建文件"
                echo ""
                echo "选项:"
                echo "  -j, --jobs   指定并行作业数"
                echo "  -h, --help   显示此帮助"
                exit 0
                ;;
            *)
                log_error "未知选项: $1"
                exit 1
                ;;
        esac
    done

    # 执行构建
    check_dependencies

    case $target in
        kernel)
            build_kernel
            ;;
        bootloader)
            build_bootloader
            ;;
        tools)
            build_user_tools
            ;;
        system-tools)
            build_system_tools
            ;;
        user-tools)
            build_user_tools
            ;;
        everything)
            build_everything
            ;;
        test)
            run_tests
            ;;
        iso)
            build_everything
            create_iso
            ;;
        all)
            build_everything
            run_tests
            ;;
    esac

    show_stats
    log_success "构建完成!"
}

# 运行主函数
main "$@"
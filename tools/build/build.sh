#!/bin/bash
# M4KK1多架构操作系统构建脚本
# 自动化构建和测试脚本 - 支持多架构

set -e

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

# 构建信息
BUILD_START=$(date +%s)
VERSION="0.2.0-multarch"
JOBS=$(nproc 2>/dev/null || echo 4)

# 支持的架构
SUPPORTED_ARCHES=("x86_64" "x86" "arm" "arm64" "powerpc" "riscv")

echo -e "${BLUE}M4KK1多架构操作系统构建脚本${NC}"
echo -e "${BLUE}版本: $VERSION${NC}"
echo -e "${BLUE}构建时间: $(date)${NC}"
echo -e "${PURPLE}支持架构: ${SUPPORTED_ARCHES[*]}${NC}"
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

    # 检查QEMU（多架构）
    for arch in x86_64 i386 aarch64 arm ppc64 riscv64; do
        local qemu_cmd="qemu-system-${arch}"
        if ! command -v "$qemu_cmd" >/dev/null 2>&1; then
            log_warn "未找到 $qemu_cmd，将无法测试 $arch 架构"
        fi
    done

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

# 构建特定架构的内核
build_kernel_arch() {
    local arch=$1
    log_info "构建M4KK1 $arch 内核..."
    make $arch
    if [ $? -eq 0 ]; then
        log_success "$arch 内核构建完成"
    else
        log_error "$arch 内核构建失败"
        exit 1
    fi
}

# 构建所有架构的内核
build_kernel() {
    log_info "构建M4KK1多架构内核..."
    for arch in "${SUPPORTED_ARCHES[@]}"; do
        build_kernel_arch $arch
    done
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

# 创建多架构ISO镜像
create_multarch_iso() {
    log_info "创建M4KK1多架构ISO镜像..."

    # 创建输出目录结构
    mkdir -p ../../iso/boot/grub
    mkdir -p ../../iso/boot/x86_64
    mkdir -p ../../iso/boot/x86
    mkdir -p ../../iso/boot/arm64
    mkdir -p ../../iso/boot/arm
    mkdir -p ../../iso/boot/riscv
    mkdir -p ../../iso/boot/powerpc

    # 复制所有架构的内核
    for arch in "${SUPPORTED_ARCHES[@]}"; do
        local kernel_file="../sys/src/m4kk1_${arch}.krn"
        local dest_file="../../iso/boot/${arch}/m4kk1.krn"

        if [ -f "$kernel_file" ]; then
            cp "$kernel_file" "$dest_file"
            log_success "复制 $arch 内核到ISO"
        else
            log_warn "$arch 内核未找到，跳过"
        fi
    done

    # 复制用户程序
    cp -r ../usr/bin/* ../../iso/usr/bin/ 2>/dev/null || true

    # 创建多架构GRUB配置
    cat > ../../iso/boot/grub/grub.cfg << 'EOF'
set timeout=10
set default=0

# M4KK1多架构引导菜单
submenu "M4KK1 Operating System (Multi-Architecture)" {
    set timeout=5

    menuentry "M4KK1 x86_64 (Default)" {
        multiboot /boot/x86_64/m4kk1.krn
        boot
    }

    menuentry "M4KK1 x86 (32-bit)" {
        multiboot /boot/x86/m4kk1.krn
        boot
    }

    menuentry "M4KK1 ARM64" {
        multiboot /boot/arm64/m4kk1.krn
        boot
    }

    menuentry "M4KK1 ARM (32-bit)" {
        multiboot /boot/arm/m4kk1.krn
        boot
    }

    menuentry "M4KK1 RISC-V" {
        multiboot /boot/riscv/m4kk1.krn
        boot
    }

    menuentry "M4KK1 PowerPC" {
        multiboot /boot/powerpc/m4kk1.krn
        boot
    }
}

# 兼容性选项
menuentry "M4KK1 Legacy (x86)" {
    multiboot /boot/m4kk1.krn
    boot
}

menuentry "M4KK1 with Serial Console" {
    multiboot /boot/x86_64/m4kk1.krn console=ttyS0
    boot
}
EOF

    # 使用grub-mkrescue创建ISO
    if command -v grub-mkrescue >/dev/null 2>&1; then
        grub-mkrescue -o m4kk1-multarch.iso ../../iso/
        log_success "多架构ISO镜像创建完成: m4kk1-multarch.iso"
    else
        log_warn "grub-mkrescue未找到，跳过ISO创建"
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

    if [ -f "m4kk1-multarch.iso" ]; then
        ISO_SIZE=$(du -h "m4kk1-multarch.iso" | cut -f1)
        echo "  多架构ISO大小: $ISO_SIZE"
    fi

    if [ -f "m4kk1-$VERSION.iso" ]; then
        ISO_SIZE=$(du -h "m4kk1-$VERSION.iso" | cut -f1)
        echo "  单架构ISO大小: $ISO_SIZE"
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
            multarch)
                target="multarch"
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
            multarch-iso)
                target="multarch-iso"
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
            -a|--arch)
                ARCH="$2"
                shift 2
                ;;
            -h|--help)
                echo "用法: $0 [目标] [选项]"
                echo ""
                echo "目标:"
                echo "  kernel       构建默认架构内核"
                echo "  multarch     构建所有架构内核"
                echo "  bootloader   构建引导程序"
                echo "  tools        构建所有工具"
                echo "  system-tools 构建系统工具"
                echo "  user-tools   构建用户工具"
                echo "  everything   构建完整系统"
                echo "  test         运行测试"
                echo "  iso          创建单架构ISO镜像"
                echo "  multarch-iso 创建多架构ISO镜像"
                echo "  clean        清理构建文件"
                echo ""
                echo "选项:"
                echo "  -j, --jobs   指定并行作业数"
                echo "  -a, --arch   指定目标架构"
                echo "  -h, --help   显示此帮助"
                echo ""
                echo "支持的架构: ${SUPPORTED_ARCHES[*]}"
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
        multarch)
            build_kernel
            create_multarch_iso
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
        multarch-iso)
            build_kernel
            create_multarch_iso
            ;;
        all)
            build_everything
            run_tests
            create_multarch_iso
            ;;
    esac

    show_stats
    log_success "构建完成!"
}

# 运行主函数
main "$@"
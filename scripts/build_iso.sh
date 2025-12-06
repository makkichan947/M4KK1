#!/bin/bash

# M4KK1 ISO镜像构建脚本
# 创建可引导的ISO镜像

set -e

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 脚本目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# 构建配置
ISO_DIR="$PROJECT_ROOT/iso"
BUILD_DIR="$PROJECT_ROOT/build"
SYSROOT_DIR="$BUILD_DIR/sysroot"
KERNEL_DIR="$BUILD_DIR/kernel"

# 工具链
MKISOFS="genisoimage"
GRUB_MKRESCUE="grub-mkrescue"

# 镜像文件名
ISO_NAME="m4kk1-$(date +%Y%m%d).iso"

echo -e "${BLUE}=======================================${NC}"
echo -e "${BLUE}    M4KK1 ISO镜像构建系统${NC}"
echo -e "${BLUE}=======================================${NC}"
echo

# 检查依赖
check_dependencies() {
    echo -e "${YELLOW}检查构建依赖...${NC}"

    if ! command -v nasm &> /dev/null; then
        echo -e "${RED}错误: 未找到nasm汇编器${NC}"
        exit 1
    fi

    if ! command -v gcc &> /dev/null; then
        echo -e "${RED}错误: 未找到gcc编译器${NC}"
        exit 1
    fi

    if ! command -v $MKISOFS &> /dev/null; then
        echo -e "${RED}错误: 未找到genisoimage工具${NC}"
        exit 1
    fi

    if ! command -v $GRUB_MKRESCUE &> /dev/null; then
        echo -e "${RED}错误: 未找到grub-mkrescue工具${NC}"
        exit 1
    fi

    echo -e "${GREEN}✓ 所有依赖检查通过${NC}"
}

# 清理旧的构建文件
clean_build() {
    echo -e "${YELLOW}清理旧的构建文件...${NC}"

    rm -rf "$BUILD_DIR"
    rm -rf "$ISO_DIR/boot/kernel"
    rm -f "$ISO_DIR/$ISO_NAME"

    echo -e "${GREEN}✓ 清理完成${NC}"
}

# 创建目录结构
create_directories() {
    echo -e "${YELLOW}创建目录结构...${NC}"

    mkdir -p "$SYSROOT_DIR"
    mkdir -p "$KERNEL_DIR"
    mkdir -p "$ISO_DIR/boot/grub"
    mkdir -p "$ISO_DIR/boot/kernel"

    echo -e "${GREEN}✓ 目录结构创建完成${NC}"
}

# 构建引导程序
build_bootloader() {
    echo -e "${YELLOW}构建引导程序...${NC}"

    # 进入引导程序目录
    cd "$PROJECT_ROOT/sys/boot/bootcamp"

    # 构建引导程序
    make clean
    make all

    # 复制引导程序到ISO目录
    cp m4kk1.img "$ISO_DIR/boot/"

    echo -e "${GREEN}✓ 引导程序构建完成${NC}"
}

# 构建内核
build_kernel() {
    echo -e "${YELLOW}构建内核...${NC}"

    # 进入内核目录
    cd "$PROJECT_ROOT/sys/src"

    # 清理旧的构建文件
    make clean

    # 构建内核
    make all

    # 复制内核到ISO目录
    find . -name "*.kernel" -exec cp {} "$ISO_DIR/boot/kernel/" \;

    echo -e "${GREEN}✓ 内核构建完成${NC}"
}

# 构建用户程序
build_userland() {
    echo -e "${YELLOW}构建用户程序...${NC}"

    # 构建Copland窗口服务器
    cd "$PROJECT_ROOT/usr/bin/copland"
    make clean
    make all
    cp copland "$SYSROOT_DIR/usr/bin/"

    # 构建其他用户程序
    for dir in "$PROJECT_ROOT/usr/bin"/*; do
        if [ -d "$dir" ] && [ "$dir" != "$PROJECT_ROOT/usr/bin/copland" ]; then
            echo -e "${BLUE}构建 $(basename "$dir")...${NC}"
            cd "$dir"
            if [ -f "Makefile" ]; then
                make clean
                make all
                # 复制可执行文件到sysroot
                find . -type f -executable -exec cp {} "$SYSROOT_DIR/usr/bin/" \; 2>/dev/null || true
            fi
            cd "$PROJECT_ROOT"
        fi
    done

    echo -e "${GREEN}✓ 用户程序构建完成${NC}"
}

# 创建initrd映像
create_initrd() {
    echo -e "${YELLOW}创建initrd映像...${NC}"

    # 创建initrd目录结构
    INITRD_DIR="$BUILD_DIR/initrd"
    mkdir -p "$INITRD_DIR"

    # 复制必要的文件到initrd
    cp -r "$SYSROOT_DIR"/* "$INITRD_DIR/"

    # 创建initrd映像（简化的实现）
    cd "$INITRD_DIR"
    find . | cpio -o -H newc > "$ISO_DIR/boot/initrd.img"

    echo -e "${GREEN}✓ initrd映像创建完成${NC}"
}

# 创建GRUB配置文件
create_grub_config() {
    echo -e "${YELLOW}创建GRUB配置文件...${NC}"

    cat > "$ISO_DIR/boot/grub/grub.cfg" << EOF
# M4KK1 GRUB配置文件
set timeout=5
set default=0

menuentry "M4KK1 Operating System" {
    multiboot /boot/kernel/m4kk1.kernel
    boot
}

menuentry "M4KK1 with initrd" {
    multiboot /boot/kernel/m4kk1.kernel
    module /boot/initrd.img
    boot
}

menuentry "M4KK1 (Text Mode)" {
    multiboot /boot/kernel/m4kk1.kernel nomodeset
    boot
}
EOF

    echo -e "${GREEN}✓ GRUB配置文件创建完成${NC}"
}

# 创建ISO镜像
create_iso() {
    echo -e "${YELLOW}创建ISO镜像...${NC}"

    cd "$ISO_DIR"

    # 使用grub-mkrescue创建ISO
    $GRUB_MKRESCUE -o "$ISO_NAME" . 2>/dev/null || {
        # 如果grub-mkrescue失败，使用genisoimage
        echo -e "${YELLOW}使用genisoimage创建ISO...${NC}"
        $MKISOFS -R -b boot/grub/i386-pc/eltorito.img \
                 -no-emul-boot -boot-load-size 4 \
                 -boot-info-table -o "$ISO_NAME" . 2>/dev/null || {
            echo -e "${RED}错误: 无法创建ISO镜像${NC}"
            exit 1
        }
    }

    echo -e "${GREEN}✓ ISO镜像创建完成: $ISO_NAME${NC}"
}

# 主构建流程
main() {
    echo -e "${BLUE}开始构建M4KK1 ISO镜像...${NC}"

    # 检查依赖
    check_dependencies

    # 清理旧的构建文件
    clean_build

    # 创建目录结构
    create_directories

    # 构建各个组件
    build_bootloader
    build_kernel
    build_userland
    create_initrd
    create_grub_config

    # 创建ISO镜像
    create_iso

    echo
    echo -e "${GREEN}=======================================${NC}"
    echo -e "${GREEN}    M4KK1 ISO镜像构建完成！${NC}"
    echo -e "${GREEN}=======================================${NC}"
    echo
    echo -e "${BLUE}镜像文件: ${ISO_DIR}/${ISO_NAME}${NC}"
    echo -e "${BLUE}大小: $(du -h "${ISO_DIR}/${ISO_NAME}" | cut -f1)${NC}"
    echo
    echo -e "${YELLOW}测试命令:${NC}"
    echo "  qemu-system-i386 -cdrom ${ISO_DIR}/${ISO_NAME}"
    echo
}

# 显示帮助信息
show_help() {
    echo "M4KK1 ISO镜像构建脚本"
    echo
    echo "用法: $0 [选项]"
    echo
    echo "选项:"
    echo "  clean     - 清理构建文件"
    echo "  kernel    - 只构建内核"
    echo "  userland  - 只构建用户程序"
    echo "  help      - 显示此帮助信息"
    echo
    echo "示例:"
    echo "  $0              # 构建完整ISO镜像"
    echo "  $0 clean        # 清理构建文件"
    echo "  $0 kernel       # 只构建内核"
}

# 处理命令行参数
case "${1:-}" in
    "clean")
        clean_build
        ;;
    "kernel")
        build_kernel
        ;;
    "userland")
        build_userland
        ;;
    "help"|"-h"|"--help")
        show_help
        ;;
    "")
        main
        ;;
    *)
        echo -e "${RED}未知选项: $1${NC}"
        echo
        show_help
        exit 1
        ;;
esac
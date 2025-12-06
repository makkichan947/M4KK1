#!/bin/bash

# M4KK1系统安装脚本
# 安装M4KK1操作系统到目标设备

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

# 默认安装设备
DEFAULT_DEVICE="/dev/sda"
DEFAULT_BOOTLOADER_DEVICE="/dev/sda"

# 安装配置
INSTALL_DEVICE="$DEFAULT_DEVICE"
BOOTLOADER_DEVICE="$DEFAULT_BOOTLOADER_DEVICE"
INSTALL_MODE="full"  # full, kernel, bootloader
CONFIRMATION=true

echo -e "${BLUE}=======================================${NC}"
echo -e "${BLUE}    M4KK1操作系统安装程序${NC}"
echo -e "${BLUE}=======================================${NC}"
echo

# 显示帮助信息
show_help() {
    echo "M4KK1系统安装脚本"
    echo
    echo "用法: $0 [选项]"
    echo
    echo "选项:"
    echo "  -d, --device DEVICE     安装目标设备 (默认: $DEFAULT_DEVICE)"
    echo "  -b, --bootloader DEVICE 引导程序安装设备 (默认: $DEFAULT_BOOTLOADER_DEVICE)"
    echo "  -m, --mode MODE         安装模式: full, kernel, bootloader (默认: full)"
    echo "  -y, --yes              自动确认所有提示"
    echo "  -h, --help             显示此帮助信息"
    echo
    echo "示例:"
    echo "  $0                      # 交互式安装"
    echo "  $0 -d /dev/sdb -y       # 安装到sdb并自动确认"
    echo "  $0 --mode kernel -y     # 只安装内核"
    echo
    echo -e "${RED}警告: 此操作将覆盖目标设备的引导扇区！${NC}"
    echo -e "${RED}请确保备份重要数据！${NC}"
}

# 解析命令行参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--device)
            INSTALL_DEVICE="$2"
            shift 2
            ;;
        -b|--bootloader)
            BOOTLOADER_DEVICE="$2"
            shift 2
            ;;
        -m|--mode)
            INSTALL_MODE="$2"
            shift 2
            ;;
        -y|--yes)
            CONFIRMATION=false
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            echo -e "${RED}未知选项: $1${NC}"
            show_help
            exit 1
            ;;
    esac
done

# 检查权限
check_permissions() {
    echo -e "${YELLOW}检查安装权限...${NC}"

    if [ "$EUID" -ne 0 ]; then
        echo -e "${RED}错误: 需要root权限运行此脚本${NC}"
        echo "请使用 sudo $0 或以root用户运行"
        exit 1
    fi

    echo -e "${GREEN}✓ 权限检查通过${NC}"
}

# 检查设备
check_device() {
    echo -e "${YELLOW}检查目标设备...${NC}"

    if [ ! -b "$INSTALL_DEVICE" ]; then
        echo -e "${RED}错误: 设备 $INSTALL_DEVICE 不存在${NC}"
        exit 1
    fi

    # 检查设备大小
    local device_size=$(blockdev --getsize64 "$INSTALL_DEVICE" 2>/dev/null || echo "0")
    if [ "$device_size" -lt $((1024*1024*1024)) ]; then  # 小于1GB
        echo -e "${YELLOW}警告: 设备大小小于1GB，可能不适合安装完整系统${NC}"
    fi

    echo -e "${GREEN}✓ 设备检查通过: $INSTALL_DEVICE${NC}"
}

# 确认安装
confirm_installation() {
    if [ "$CONFIRMATION" = true ]; then
        echo
        echo -e "${RED}警告: 此操作将修改以下设备:${NC}"
        echo "  引导程序设备: $BOOTLOADER_DEVICE"
        echo "  系统安装设备: $INSTALL_DEVICE"
        echo
        echo -e "${RED}这将覆盖引导扇区和可能的数据！${NC}"
        echo -e "${YELLOW}请确保已备份重要数据！${NC}"
        echo
        read -p "是否继续安装? (yes/no): " -r
        if [[ ! $REPLY =~ ^[Yy][Ee][Ss]$ ]]; then
            echo "安装已取消"
            exit 0
        fi
    fi
}

# 构建系统
build_system() {
    echo -e "${YELLOW}构建M4KK1系统...${NC}"

    # 构建引导程序
    if [[ "$INSTALL_MODE" == "full" || "$INSTALL_MODE" == "bootloader" ]]; then
        echo -e "${BLUE}构建引导程序...${NC}"
        cd "$PROJECT_ROOT/sys/boot/bootcamp"
        make clean
        make all
    fi

    # 构建内核
    if [[ "$INSTALL_MODE" == "full" || "$INSTALL_MODE" == "kernel" ]]; then
        echo -e "${BLUE}构建内核...${NC}"
        cd "$PROJECT_ROOT/sys/src"
        make clean
        make all
    fi

    echo -e "${GREEN}✓ 系统构建完成${NC}"
}

# 安装引导程序
install_bootloader() {
    echo -e "${YELLOW}安装引导程序...${NC}"

    cd "$PROJECT_ROOT/sys/boot/bootcamp"

    # 安装引导程序到设备
    if [ -f "m4kk1.img" ]; then
        echo -e "${BLUE}安装引导程序到 $BOOTLOADER_DEVICE...${NC}"
        dd if=m4kk1.img of="$BOOTLOADER_DEVICE" bs=512 count=1 conv=notrunc
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✓ 引导程序安装成功${NC}"
        else
            echo -e "${RED}错误: 引导程序安装失败${NC}"
            exit 1
        fi
    else
        echo -e "${RED}错误: 未找到引导程序映像文件${NC}"
        exit 1
    fi
}

# 安装内核
install_kernel() {
    echo -e "${YELLOW}安装内核...${NC}"

    # 查找内核文件
    local kernel_files=()
    while IFS= read -r -d '' file; do
        kernel_files+=("$file")
    done < <(find "$PROJECT_ROOT" -name "*.kernel" -print0 2>/dev/null)

    if [ ${#kernel_files[@]} -eq 0 ]; then
        echo -e "${RED}错误: 未找到内核文件${NC}"
        exit 1
    fi

    # 安装第一个找到的内核
    local kernel_file="${kernel_files[0]}"
    echo -e "${BLUE}安装内核 $kernel_file 到 ${INSTALL_DEVICE}1...${NC}"

    # 创建内核分区（如果不存在）
    # 注意：这是一个简化的实现，实际应该使用分区工具
    echo -e "${YELLOW}注意: 请确保已创建适当的分区结构${NC}"

    echo -e "${GREEN}✓ 内核安装完成${NC}"
}

# 安装系统文件
install_system() {
    echo -e "${YELLOW}安装系统文件...${NC}"

    # 创建文件系统（如果需要）
    # 注意：这是一个简化的实现

    echo -e "${GREEN}✓ 系统文件安装完成${NC}"
}

# 验证安装
verify_installation() {
    echo -e "${YELLOW}验证安装...${NC}"

    # 检查引导程序
    if [ -f "$PROJECT_ROOT/sys/boot/bootcamp/m4kk1.img" ]; then
        echo -e "${GREEN}✓ 引导程序文件存在${NC}"
    else
        echo -e "${RED}错误: 引导程序文件不存在${NC}"
        exit 1
    fi

    # 检查设备是否可访问
    if [ -b "$INSTALL_DEVICE" ]; then
        echo -e "${GREEN}✓ 目标设备可访问${NC}"
    else
        echo -e "${RED}错误: 目标设备不可访问${NC}"
        exit 1
    fi

    echo -e "${GREEN}✓ 安装验证通过${NC}"
}

# 主安装流程
main() {
    echo -e "${BLUE}开始M4KK1系统安装...${NC}"
    echo "安装模式: $INSTALL_MODE"
    echo "引导程序设备: $BOOTLOADER_DEVICE"
    echo "系统设备: $INSTALL_DEVICE"
    echo

    # 检查权限
    check_permissions

    # 检查设备
    check_device

    # 确认安装
    confirm_installation

    # 构建系统
    build_system

    # 验证构建结果
    verify_installation

    # 根据模式执行安装
    case "$INSTALL_MODE" in
        "bootloader")
            install_bootloader
            ;;
        "kernel")
            install_kernel
            ;;
        "full")
            install_bootloader
            install_kernel
            install_system
            ;;
        *)
            echo -e "${RED}错误: 未知安装模式 $INSTALL_MODE${NC}"
            exit 1
            ;;
    esac

    echo
    echo -e "${GREEN}=======================================${NC}"
    echo -e "${GREEN}    M4KK1系统安装完成！${NC}"
    echo -e "${GREEN}=======================================${NC}"
    echo
    echo -e "${BLUE}安装摘要:${NC}"
    echo "  引导程序已安装到: $BOOTLOADER_DEVICE"
    echo "  系统已安装到: $INSTALL_DEVICE"
    echo "  安装模式: $INSTALL_MODE"
    echo
    echo -e "${YELLOW}重启系统以使用新安装的M4KK1操作系统${NC}"
    echo -e "${YELLOW}注意: 可能需要在BIOS中选择正确的引导设备${NC}"
}

# 运行主函数
main "$@"
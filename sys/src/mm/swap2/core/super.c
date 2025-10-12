/**
 * Swap2 - 改进版交换系统超级块操作
 * 实现超级块的读写和管理功能
 */

#include "swap2.h"
#include "../include/swap2.h"
#include "../../y4ku/include/console.h"

/**
 * 读取超级块
 */
int swap2_read_superblock(swap2_superblock_t *super, void *device) {
    if (!super || !device) {
        return -1;
    }

    // 读取超级块到内存
    int ret = device->read(device, 0, (uint8_t *)super, SWAP2_SUPERBLOCK_SIZE);
    if (ret != SWAP2_SUPERBLOCK_SIZE) {
        console_write("Failed to read Swap2 superblock\n");
        return -1;
    }

    // 验证超级块魔数
    if (super->magic != SWAP2_MAGIC) {
        console_write("Invalid Swap2 magic: 0x");
        console_write_hex(super->magic);
        console_write("\n");
        return -1;
    }

    // 验证超级块校验和
    uint32_t saved_checksum = super->checksum;
    super->checksum = 0;
    uint32_t calculated_checksum = swap2_checksum_crc32c(super, SWAP2_SUPERBLOCK_SIZE);

    if (saved_checksum != calculated_checksum) {
        console_write("Swap2 superblock checksum mismatch\n");
        return -1;
    }

    super->checksum = saved_checksum;

    console_write("Swap2 superblock read successfully\n");
    console_write("Page size: ");
    console_write_dec(super->page_size);
    console_write(" bytes\n");
    console_write("Total pages: ");
    console_write_dec(super->total_pages);
    console_write("\n");
    console_write("Free pages: ");
    console_write_dec(super->free_pages);
    console_write("\n");

    return 0;
}

/**
 * 写入超级块
 */
int swap2_write_superblock(swap2_superblock_t *super, void *device) {
    if (!super || !device) {
        return -1;
    }

    // 计算校验和
    super->checksum = 0;
    super->checksum = swap2_checksum_crc32c(super, SWAP2_SUPERBLOCK_SIZE);

    // 写入超级块
    int ret = device->write(device, 0, (uint8_t *)super, SWAP2_SUPERBLOCK_SIZE);
    if (ret != SWAP2_SUPERBLOCK_SIZE) {
        console_write("Failed to write Swap2 superblock\n");
        return -1;
    }

    console_write("Swap2 superblock written successfully\n");
    return 0;
}

/**
 * 验证超级块
 */
int swap2_verify_superblock(swap2_superblock_t *super) {
    if (!super) {
        return -1;
    }

    // 验证魔数
    if (super->magic != SWAP2_MAGIC) {
        return -1;
    }

    // 验证版本
    if (super->version != (SWAP2_VERSION_MAJOR << 16 | SWAP2_VERSION_MINOR << 8 | SWAP2_VERSION_PATCH)) {
        console_write("Swap2 version mismatch\n");
        return -1;
    }

    // 验证页面大小
    if (super->page_size != SWAP2_DEFAULT_PAGE_SIZE) {
        console_write("Unsupported page size: ");
        console_write_dec(super->page_size);
        console_write("\n");
        return -1;
    }

    // 验证校验和
    uint32_t saved_checksum = super->checksum;
    super->checksum = 0;
    uint32_t calculated_checksum = swap2_checksum_crc32c(super, SWAP2_SUPERBLOCK_SIZE);

    if (saved_checksum != calculated_checksum) {
        console_write("Swap2 superblock checksum verification failed\n");
        return -1;
    }

    super->checksum = saved_checksum;

    return 0;
}

/**
 * 创建超级块
 */
int swap2_create_superblock(swap2_superblock_t *super, uint64_t total_pages,
                           uint32_t page_size, uint32_t compression_alg,
                           uint32_t checksum_alg) {
    if (!super) {
        return -1;
    }

    // 清零超级块
    swap2_memset(super, 0, SWAP2_SUPERBLOCK_SIZE);

    // 设置基本信息
    super->magic = SWAP2_MAGIC;
    super->version = (SWAP2_VERSION_MAJOR << 16) | (SWAP2_VERSION_MINOR << 8) | SWAP2_VERSION_PATCH;
    super->total_pages = total_pages;
    super->free_pages = total_pages - 1;  // 减去超级块
    super->used_pages = 1;  // 超级块占用的页面
    super->journal_pages = 32768;  // 32K个日志页面
    super->metadata_pages = 65536;  // 64K个元数据页面
    super->page_size = page_size;
    super->compression_alg = compression_alg;
    super->checksum_alg = checksum_alg;

    // 生成UUID
    swap2_uuid_generate(super->uuid);

    // 设置时间
    super->creation_time = swap2_time_current();

    // 设置标志位
    super->flags = SWAP2_FLAG_COMPRESSED | SWAP2_FLAG_DIRTY;

    // 计算校验和
    super->checksum = swap2_checksum_crc32c(super, SWAP2_SUPERBLOCK_SIZE);

    return 0;
}

/**
 * 更新超级块统计信息
 */
void swap2_update_superblock_stats(swap2_superblock_t *super) {
    if (!super) {
        return;
    }

    // 这里应该重新计算空闲页面和已用页面数量
    // 暂时使用简化实现

    // 重新计算校验和
    super->checksum = 0;
    super->checksum = swap2_checksum_crc32c(super, SWAP2_SUPERBLOCK_SIZE);
}

/**
 * 转储超级块信息（调试用）
 */
void swap2_dump_superblock(swap2_superblock_t *super) {
    if (!super) {
        return;
    }

    console_write("=== Swap2 Superblock Dump ===\n");
    console_write("Magic: 0x");
    console_write_hex(super->magic);
    console_write("\n");

    console_write("Version: ");
    console_write_dec(super->version >> 16);
    console_write(".");
    console_write_dec((super->version >> 8) & 0xFF);
    console_write(".");
    console_write_dec(super->version & 0xFF);
    console_write("\n");

    console_write("Page size: ");
    console_write_dec(super->page_size);
    console_write(" bytes\n");

    console_write("Total pages: ");
    console_write_dec(super->total_pages);
    console_write("\n");

    console_write("Free pages: ");
    console_write_dec(super->free_pages);
    console_write("\n");

    console_write("Used pages: ");
    console_write_dec(super->used_pages);
    console_write("\n");

    console_write("Journal pages: ");
    console_write_dec(super->journal_pages);
    console_write("\n");

    console_write("Metadata pages: ");
    console_write_dec(super->metadata_pages);
    console_write("\n");

    console_write("Compression algorithm: ");
    console_write_dec(super->compression_alg);
    console_write("\n");

    console_write("Checksum algorithm: ");
    console_write_dec(super->checksum_alg);
    console_write("\n");

    console_write("UUID: ");
    for (int i = 0; i < 16; i++) {
        console_write_hex(super->uuid[i]);
        if (i < 15) console_write("-");
    }
    console_write("\n");

    console_write("Creation time: ");
    console_write_dec(super->creation_time);
    console_write("\n");

    console_write("Flags: 0x");
    console_write_hex(super->flags);
    console_write("\n");

    console_write("Checksum: 0x");
    console_write_hex(super->checksum);
    console_write("\n");

    console_write("=== End Superblock Dump ===\n");
}

/**
 * 获取交换空间大小（字节）
 */
uint64_t swap2_get_swap_size_bytes(swap2_superblock_t *super) {
    if (!super) {
        return 0;
    }

    return super->total_pages * super->page_size;
}

/**
 * 获取交换空间大小（MB）
 */
uint64_t swap2_get_swap_size_mb(swap2_superblock_t *super) {
    return swap2_get_swap_size_bytes(super) / (1024 * 1024);
}

/**
 * 获取交换空间利用率
 */
uint32_t swap2_get_swap_utilization(swap2_superblock_t *super) {
    if (!super || super->total_pages == 0) {
        return 0;
    }

    return (uint32_t)((super->used_pages * 100) / super->total_pages);
}

/**
 * 检查交换空间是否可用
 */
bool swap2_is_swap_available(swap2_superblock_t *super) {
    if (!super) {
        return false;
    }

    return (super->magic == SWAP2_MAGIC) &&
           (super->free_pages > 0) &&
           ((super->flags & SWAP2_FLAG_DIRTY) == 0);
}

/**
 * 标记交换空间为脏
 */
void swap2_mark_dirty(swap2_superblock_t *super) {
    if (super) {
        super->flags |= SWAP2_FLAG_DIRTY;
    }
}

/**
 * 清除脏标记
 */
void swap2_clear_dirty(swap2_superblock_t *super) {
    if (super) {
        super->flags &= ~SWAP2_FLAG_DIRTY;
    }
}

/**
 * 获取交换空间状态字符串
 */
const char *swap2_get_status_string(swap2_superblock_t *super) {
    if (!super) {
        return "INVALID";
    }

    if (super->magic != SWAP2_MAGIC) {
        return "INVALID_MAGIC";
    }

    if (super->flags & SWAP2_FLAG_DIRTY) {
        return "DIRTY";
    }

    if (super->free_pages == 0) {
        return "FULL";
    }

    return "CLEAN";
}
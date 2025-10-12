/**
 * YFS (Yet Another File System) - 超级块操作
 * 实现超级块的读写和管理功能
 */

#include "../include/yfs.h"
#include "../../../include/console.h"

/**
 * 读取超级块
 */
int yfs_read_superblock(yfs_mount_t *mount) {
    if (!mount || !mount->device) {
        return -1;
    }

    // 读取超级块到内存
    int ret = mount->device->read(mount->device, 0, (uint8_t *)mount->super, YFS_SUPERBLOCK_SIZE);
    if (ret != YFS_SUPERBLOCK_SIZE) {
        console_write("Failed to read superblock\n");
        return -1;
    }

    // 验证超级块魔数
    if (mount->super->magic != YFS_MAGIC) {
        console_write("Invalid YFS magic: 0x");
        console_write_hex(mount->super->magic);
        console_write("\n");
        return -1;
    }

    // 验证超级块校验和
    uint32_t saved_checksum = mount->super->checksum;
    mount->super->checksum = 0;
    uint32_t calculated_checksum = yfs_checksum_crc32c(mount->super, YFS_SUPERBLOCK_SIZE);

    if (saved_checksum != calculated_checksum) {
        console_write("Superblock checksum mismatch\n");
        return -1;
    }

    mount->super->checksum = saved_checksum;

    // 设置挂载信息
    mount->block_size = mount->super->block_size;
    mount->blocks_per_group = YFS_BLOCK_GROUP_SIZE / mount->block_size;
    mount->inodes_per_group = mount->blocks_per_group / 4;  // 索引节点占块的1/4
    mount->group_count = (mount->super->total_blocks + mount->blocks_per_group - 1) / mount->blocks_per_group;
    mount->first_data_block = (YFS_SUPERBLOCK_SIZE + mount->block_size - 1) / mount->block_size;

    console_write("YFS superblock read successfully\n");
    console_write("Block size: ");
    console_write_dec(mount->block_size);
    console_write(" bytes\n");
    console_write("Total blocks: ");
    console_write_dec(mount->super->total_blocks);
    console_write("\n");
    console_write("Total inodes: ");
    console_write_dec(mount->super->total_inodes);
    console_write("\n");

    return 0;
}

/**
 * 写入超级块
 */
int yfs_write_superblock(yfs_mount_t *mount) {
    if (!mount || !mount->device || mount->read_only) {
        return -1;
    }

    // 计算校验和
    mount->super->checksum = 0;
    mount->super->checksum = yfs_checksum_crc32c(mount->super, YFS_SUPERBLOCK_SIZE);

    // 写入超级块
    int ret = mount->device->write(mount->device, 0, (uint8_t *)mount->super, YFS_SUPERBLOCK_SIZE);
    if (ret != YFS_SUPERBLOCK_SIZE) {
        console_write("Failed to write superblock\n");
        return -1;
    }

    console_write("YFS superblock written successfully\n");
    return 0;
}

/**
 * 验证超级块
 */
int yfs_verify_superblock(yfs_superblock_t *super) {
    if (!super) {
        return -1;
    }

    // 验证魔数
    if (super->magic != YFS_MAGIC) {
        return -1;
    }

    // 验证版本
    if (super->version != (YFS_VERSION_MAJOR << 16 | YFS_VERSION_MINOR << 8 | YFS_VERSION_PATCH)) {
        console_write("YFS version mismatch\n");
        return -1;
    }

    // 验证块大小
    if (super->block_size != YFS_DEFAULT_BLOCK_SIZE) {
        console_write("Unsupported block size: ");
        console_write_dec(super->block_size);
        console_write("\n");
        return -1;
    }

    // 验证校验和
    uint32_t saved_checksum = super->checksum;
    super->checksum = 0;
    uint32_t calculated_checksum = yfs_checksum_crc32c(super, YFS_SUPERBLOCK_SIZE);

    if (saved_checksum != calculated_checksum) {
        console_write("Superblock checksum verification failed\n");
        return -1;
    }

    super->checksum = saved_checksum;

    return 0;
}

/**
 * 创建超级块
 */
int yfs_create_superblock(yfs_superblock_t *super, uint64_t total_blocks,
                         uint32_t block_size, uint32_t compression_alg,
                         uint32_t checksum_alg) {
    if (!super) {
        return -1;
    }

    // 清零超级块
    memset(super, 0, YFS_SUPERBLOCK_SIZE);

    // 设置基本信息
    super->magic = YFS_MAGIC;
    super->version = (YFS_VERSION_MAJOR << 16) | (YFS_VERSION_MINOR << 8) | YFS_VERSION_PATCH;
    super->block_size = block_size;
    super->total_blocks = total_blocks;
    super->free_blocks = total_blocks - 1;  // 减去超级块
    super->total_inodes = total_blocks / 4;  // 索引节点占总块数的1/4
    super->free_inodes = super->total_inodes;
    super->journal_blocks = 32768;  // 32K个日志块
    super->compression_alg = compression_alg;
    super->checksum_alg = checksum_alg;

    // 生成UUID
    yfs_uuid_generate(super->uuid);

    // 设置时间
    super->creation_time = yfs_time_current();
    super->mount_time = super->creation_time;
    super->mount_count = 1;

    // 设置状态标志
    super->state_flags = YFS_STATE_CLEAN;

    // 计算校验和
    super->checksum = yfs_checksum_crc32c(super, YFS_SUPERBLOCK_SIZE);

    return 0;
}

/**
 * 更新超级块统计信息
 */
void yfs_update_superblock_stats(yfs_mount_t *mount) {
    if (!mount || !mount->super) {
        return;
    }

    // 这里应该重新计算空闲块和索引节点数量
    // 暂时使用简化实现

    mount->super->mount_time = yfs_time_current();
    mount->super->mount_count++;

    // 重新计算校验和
    mount->super->checksum = 0;
    mount->super->checksum = yfs_checksum_crc32c(mount->super, YFS_SUPERBLOCK_SIZE);
}

/**
 * 转储超级块信息（调试用）
 */
void yfs_dump_superblock(yfs_superblock_t *super) {
    if (!super) {
        return;
    }

    console_write("=== YFS Superblock Dump ===\n");
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

    console_write("Block size: ");
    console_write_dec(super->block_size);
    console_write(" bytes\n");

    console_write("Total blocks: ");
    console_write_dec(super->total_blocks);
    console_write("\n");

    console_write("Free blocks: ");
    console_write_dec(super->free_blocks);
    console_write("\n");

    console_write("Total inodes: ");
    console_write_dec(super->total_inodes);
    console_write("\n");

    console_write("Free inodes: ");
    console_write_dec(super->free_inodes);
    console_write("\n");

    console_write("Journal blocks: ");
    console_write_dec(super->journal_blocks);
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

    console_write("Mount time: ");
    console_write_dec(super->mount_time);
    console_write("\n");

    console_write("Mount count: ");
    console_write_dec(super->mount_count);
    console_write("\n");

    console_write("State flags: 0x");
    console_write_hex(super->state_flags);
    console_write("\n");

    console_write("Checksum: 0x");
    console_write_hex(super->checksum);
    console_write("\n");

    console_write("=== End Superblock Dump ===\n");
}
/**
  * YFS (Yet Another File System) 主头文件
  * 定义文件系统的核心数据结构和常量
  */

#ifndef _YFS_H
#define _YFS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

/* 外部声明 */
extern void *kmalloc(size_t size);
extern void kfree(void *ptr);
extern void *memset(void *s, int c, size_t n);
extern void *memcpy(void *dest, const void *src, size_t n);
extern size_t strlen(const char *s);
extern int strcmp(const char *s1, const char *s2);
extern char *strcpy(char *dest, const char *src);
extern char *strncpy(char *dest, const char *src, size_t n);

/**
 * YFS魔数
 */
#define YFS_MAGIC 0x59465321    /* "YFS!" */

/**
 * YFS版本
 */
#define YFS_VERSION_MAJOR 0
#define YFS_VERSION_MINOR 1
#define YFS_VERSION_PATCH 0

/**
 * 块大小选项
 */
#define YFS_BLOCK_SIZE_512   512
#define YFS_BLOCK_SIZE_1K    1024
#define YFS_BLOCK_SIZE_2K    2048
#define YFS_BLOCK_SIZE_4K    4096
#define YFS_BLOCK_SIZE_8K    8192
#define YFS_BLOCK_SIZE_16K   16384
#define YFS_BLOCK_SIZE_32K   32768
#define YFS_BLOCK_SIZE_64K   65536

/**
 * 默认块大小
 */
#define YFS_DEFAULT_BLOCK_SIZE YFS_BLOCK_SIZE_4K

/**
 * 超级块大小
 */
#define YFS_SUPERBLOCK_SIZE 65536    /* 64KB */

/**
 * 块组大小
 */
#define YFS_BLOCK_GROUP_SIZE (128 * 1024 * 1024)  /* 128MB */

/**
 * 索引节点大小
 */
#define YFS_INODE_SIZE 512

/**
 * 扩展大小
 */
#define YFS_EXTENT_SIZE 32

/**
 * 最大文件名长度
 */
#define YFS_MAX_NAME_LEN 255

/**
 * 最大路径长度
 */
#define YFS_MAX_PATH_LEN 4096

/**
 * 压缩算法
 */
#define YFS_COMPRESSION_NONE    0
#define YFS_COMPRESSION_LZ4     1
#define YFS_COMPRESSION_ZSTD    2
#define YFS_COMPRESSION_LZMA    3

/**
 * 校验算法
 */
#define YFS_CHECKSUM_NONE       0
#define YFS_CHECKSUM_CRC32C     1
#define YFS_CHECKSUM_SHA256     2
#define YFS_CHECKSUM_BLAKE3     3

/**
 * 文件系统状态标志
 */
#define YFS_STATE_CLEAN         0x0001
#define YFS_STATE_ERROR         0x0002
#define YFS_STATE_RECOVERY      0x0004

/**
 * 文件类型
 */
#define YFS_FT_UNKNOWN    0
#define YFS_FT_REG_FILE   1
#define YFS_FT_DIR        2
#define YFS_FT_CHRDEV     3
#define YFS_FT_BLKDEV     4
#define YFS_FT_FIFO       5
#define YFS_FT_SOCK       6
#define YFS_FT_SYMLINK    7

/**
 * 文件模式标志
 */
#define YFS_S_IFMT      0170000     /* 文件类型掩码 */
#define YFS_S_IFSOCK    0140000     /* 套接字 */
#define YFS_S_IFLNK     0120000     /* 符号链接 */
#define YFS_S_IFREG     0100000     /* 常规文件 */
#define YFS_S_IFBLK     0060000     /* 块设备 */
#define YFS_S_IFDIR     0040000     /* 目录 */
#define YFS_S_IFCHR     0020000     /* 字符设备 */
#define YFS_S_IFIFO     0010000     /* FIFO */

#define YFS_S_IRWXU     00700       /* 用户读写执行权限 */
#define YFS_S_IRUSR     00400       /* 用户读权限 */
#define YFS_S_IWUSR     00200       /* 用户写权限 */
#define YFS_S_IXUSR     00100       /* 用户执行权限 */

#define YFS_S_IRWXG     00070       /* 组读写执行权限 */
#define YFS_S_IRGRP     00040       /* 组读权限 */
#define YFS_S_IWGRP     00020       /* 组写权限 */
#define YFS_S_IXGRP     00010       /* 组执行权限 */

#define YFS_S_IRWXO     00007       /* 其他读写执行权限 */
#define YFS_S_IROTH     00004       /* 其他读权限 */
#define YFS_S_IWOTH     00002       /* 其他写权限 */
#define YFS_S_IXOTH     00001       /* 其他执行权限 */

/**
 * 超级块结构
 */
typedef struct {
    uint32_t magic;              /* YFS魔数 */
    uint32_t version;            /* 文件系统版本 */
    uint32_t block_size;         /* 块大小 */
    uint64_t total_blocks;       /* 总块数 */
    uint64_t free_blocks;        /* 空闲块数 */
    uint64_t total_inodes;       /* 总索引节点数 */
    uint64_t free_inodes;        /* 空闲索引节点数 */
    uint64_t journal_blocks;     /* 日志块数 */
    uint32_t compression_alg;    /* 默认压缩算法 */
    uint32_t checksum_alg;       /* 校验算法 */
    uint8_t  uuid[16];           /* 文件系统UUID */
    uint64_t creation_time;      /* 创建时间 */
    uint64_t mount_time;         /* 挂载时间 */
    uint32_t mount_count;        /* 挂载计数 */
    uint32_t state_flags;        /* 状态标志 */
    uint8_t  reserved[4080];     /* 保留空间 */
    uint32_t checksum;           /* 校验和 */
} __attribute__((packed)) yfs_superblock_t;

/**
 * 块组描述符
 */
typedef struct {
    uint32_t block_bitmap;       /* 块位图块号 */
    uint32_t inode_bitmap;       /* 索引节点位图块号 */
    uint32_t inode_table;        /* 索引节点表块号 */
    uint32_t free_blocks_count;  /* 空闲块计数 */
    uint32_t free_inodes_count;  /* 空闲索引节点计数 */
    uint32_t used_dirs_count;    /* 目录计数 */
    uint16_t flags;              /* 标志 */
    uint8_t  reserved[14];       /* 保留空间 */
    uint32_t checksum;           /* 校验和 */
} __attribute__((packed)) yfs_bg_descriptor_t;

/**
 * 索引节点结构
 */
typedef struct {
    uint32_t magic;              /* 索引节点魔数 */
    uint32_t mode;               /* 文件模式 */
    uint32_t uid;                /* 用户ID */
    uint32_t gid;                /* 组ID */
    uint64_t size;               /* 文件大小 */
    uint64_t atime;              /* 访问时间 */
    uint64_t mtime;              /* 修改时间 */
    uint64_t ctime;              /* 创建时间 */
    uint32_t block_count;        /* 块计数 */
    uint32_t link_count;         /* 链接计数 */
    uint32_t flags;              /* 标志 */
    uint32_t compression;        /* 压缩标志 */
    uint32_t checksum_alg;       /* 校验算法 */
    uint32_t extent_count;       /* 扩展计数 */
    uint8_t  reserved[232];      /* 保留空间 */
    uint32_t checksum;           /* 校验和 */
} __attribute__((packed)) yfs_inode_t;

/**
 * 扩展结构
 */
typedef struct {
    uint64_t logical_block;      /* 逻辑块号 */
    uint64_t physical_block;     /* 物理块号 */
    uint32_t length;             /* 长度（块） */
    uint32_t flags;              /* 标志 */
} __attribute__((packed)) yfs_extent_t;

/**
 * 目录项结构
 */
typedef struct {
    uint32_t inode;              /* 索引节点号 */
    uint16_t rec_len;            /* 记录长度 */
    uint8_t  name_len;           /* 名称长度 */
    uint8_t  file_type;          /* 文件类型 */
    char     name[YFS_MAX_NAME_LEN]; /* 文件名 */
} __attribute__((packed)) yfs_dirent_t;

/**
 * 日志项结构
 */
typedef struct {
    uint64_t trans_id;           /* 事务ID */
    uint32_t type;               /* 操作类型 */
    uint32_t size;               /* 数据大小 */
    uint64_t block_nr;           /* 块号 */
    uint8_t  data[];             /* 数据 */
} __attribute__((packed)) yfs_journal_entry_t;

/**
 * 文件系统挂载信息
 */
typedef struct {
    yfs_superblock_t *super;     /* 超级块指针 */
    uint8_t *block_bitmap;       /* 块位图 */
    uint8_t *inode_bitmap;       /* 索引节点位图 */
    yfs_inode_t *inode_table;    /* 索引节点表 */
    uint32_t block_size;         /* 块大小 */
    uint32_t blocks_per_group;   /* 每组块数 */
    uint32_t inodes_per_group;   /* 每组索引节点数 */
    uint32_t group_count;        /* 组数 */
    uint32_t first_data_block;   /* 第一个数据块 */
    char *device_name;           /* 设备名称 */
    void *device;                /* 设备指针 */
    bool read_only;              /* 只读标志 */
    uint32_t compression_alg;    /* 压缩算法 */
    uint32_t checksum_alg;       /* 校验算法 */
} yfs_mount_t;

/**
 * 文件句柄
 */
typedef struct {
    yfs_mount_t *mount;          /* 挂载点 */
    yfs_inode_t *inode;          /* 索引节点 */
    uint32_t flags;              /* 打开标志 */
    uint64_t position;           /* 文件位置 */
} yfs_file_t;

/**
 * 函数声明
 */

/* 超级块操作 */
int yfs_read_superblock(yfs_mount_t *mount);
int yfs_write_superblock(yfs_mount_t *mount);
int yfs_verify_superblock(yfs_superblock_t *super);

/* 块组操作 */
int yfs_read_block_group(yfs_mount_t *mount, uint32_t group, yfs_bg_descriptor_t *bg);
int yfs_write_block_group(yfs_mount_t *mount, uint32_t group, yfs_bg_descriptor_t *bg);

/* 索引节点操作 */
int yfs_read_inode(yfs_mount_t *mount, uint32_t inode_nr, yfs_inode_t *inode);
int yfs_write_inode(yfs_mount_t *mount, uint32_t inode_nr, yfs_inode_t *inode);
uint32_t yfs_alloc_inode(yfs_mount_t *mount);
void yfs_free_inode(yfs_mount_t *mount, uint32_t inode_nr);

/* 块操作 */
int yfs_read_block(yfs_mount_t *mount, uint64_t block_nr, void *buffer);
int yfs_write_block(yfs_mount_t *mount, uint64_t block_nr, const void *buffer);
uint64_t yfs_alloc_block(yfs_mount_t *mount);
void yfs_free_block(yfs_mount_t *mount, uint64_t block_nr);

/* 目录操作 */
int yfs_create_dirent(yfs_mount_t *mount, uint32_t dir_inode, const char *name,
                      uint32_t inode_nr, uint8_t file_type);
int yfs_delete_dirent(yfs_mount_t *mount, uint32_t dir_inode, const char *name);
yfs_dirent_t *yfs_find_dirent(yfs_mount_t *mount, uint32_t dir_inode, const char *name);
int yfs_list_dir(yfs_mount_t *mount, uint32_t dir_inode, yfs_dirent_t *entries, uint32_t *count);

/* 文件操作 */
int yfs_create_file(yfs_mount_t *mount, uint32_t dir_inode, const char *name,
                    uint32_t mode, uint32_t *inode_nr);
int yfs_delete_file(yfs_mount_t *mount, uint32_t dir_inode, const char *name);
int yfs_read_file(yfs_file_t *file, void *buffer, uint32_t size, uint32_t *bytes_read);
int yfs_write_file(yfs_file_t *file, const void *buffer, uint32_t size, uint32_t *bytes_written);

/* 文件系统操作 */
int yfs_mount(const char *device, yfs_mount_t *mount, bool read_only);
int yfs_umount(yfs_mount_t *mount);
int yfs_format(const char *device, uint32_t block_size, uint32_t compression_alg,
               uint32_t checksum_alg);

/* 工具函数 */
uint32_t yfs_checksum_crc32c(const void *data, uint32_t size);
uint32_t yfs_checksum_adler32(const void *data, uint32_t size);
void yfs_uuid_generate(uint8_t *uuid);
uint64_t yfs_time_current(void);

/* 调试和诊断 */
void yfs_dump_superblock(yfs_superblock_t *super);
void yfs_dump_inode(yfs_inode_t *inode);
void yfs_dump_mount(yfs_mount_t *mount);

#endif /* _YFS_H */
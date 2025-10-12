/*
 * M4KK1 VI编辑器 - 缓冲区管理接口
 * 定义缓冲区数据结构和相关操作函数
 */

#ifndef __VI_BUFFER_H__
#define __VI_BUFFER_H__

#include "editor.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 行信息结构 */
typedef struct line_info {
    size_t offset;      /* 在缓冲区中的偏移 */
    size_t length;      /* 行长度（不包括换行符） */
    size_t screen_len;  /* 屏幕显示长度（处理制表符后） */
    bool wrapped;       /* 是否被换行 */
} line_info_t;

/* 撤销操作类型 */
typedef enum {
    UNDO_INSERT,        /* 插入操作 */
    UNDO_DELETE,        /* 删除操作 */
    UNDO_REPLACE        /* 替换操作 */
} undo_type_t;

/* 撤销操作结构 */
typedef struct undo_op {
    undo_type_t type;       /* 操作类型 */
    size_t pos;             /* 操作位置 */
    size_t old_len;         /* 旧文本长度 */
    size_t new_len;         /* 新文本长度 */
    char *old_text;         /* 旧文本 */
    char *new_text;         /* 新文本 */
    struct undo_op *next;   /* 下一个操作 */
    struct undo_op *prev;   /* 上一个操作 */
} undo_op_t;

/* 缓冲区结构（扩展） */
struct buffer {
    char *data;              /* 文本数据 */
    size_t size;             /* 当前大小 */
    size_t capacity;         /* 容量 */

    char *filename;          /* 文件名 */
    char *display_name;      /* 显示名称 */
    bool modified;           /* 修改标志 */
    bool readonly;           /* 只读标志 */
    buffer_type_t type;      /* 缓冲区类型 */

    /* 光标和位置信息 */
    cursor_t cursor;         /* 主光标位置 */
    cursor_t saved_cursor;   /* 保存的光标位置 */
    size_t last_edit_pos;    /* 最后编辑位置 */

    /* 行信息 */
    line_info_t *lines;      /* 行信息数组 */
    size_t line_count;       /* 行数 */
    size_t line_capacity;    /* 行信息数组容量 */

    /* 撤销系统 */
    undo_op_t *undo_stack;   /* 撤销栈 */
    undo_op_t *redo_stack;   /* 重做栈 */
    size_t undo_levels;      /* 撤销级别 */
    size_t max_undo_levels;  /* 最大撤销级别 */

    /* 标记 */
    cursor_t marks[26];      /* 标记位置 (a-z) */
    cursor_t prev_context;   /* 上一个上下文位置 */

    /* 缓冲区链接 */
    struct buffer *next;     /* 下一个缓冲区 */
    struct buffer *prev;     /* 上一个缓冲区 */

    /* 元数据 */
    time_t mtime;            /* 文件修改时间 */
    uid_t owner;             /* 文件所有者 */
    gid_t group;             /* 文件组 */
    mode_t mode;             /* 文件权限 */

    /* 统计信息 */
    size_t insert_count;     /* 插入字符数 */
    size_t delete_count;     /* 删除字符数 */
    time_t last_access;      /* 最后访问时间 */
};

/* 函数声明 */

/* 缓冲区创建和销毁 */
buffer_t *buffer_create(void);
buffer_t *buffer_create_from_file(const char *filename);
buffer_t *buffer_create_scratch(const char *name);
void buffer_destroy(buffer_t *buffer);

/* 文件操作 */
bool buffer_load(buffer_t *buffer, const char *filename);
bool buffer_save(buffer_t *buffer, const char *filename);
bool buffer_reload(buffer_t *buffer);
bool buffer_write_to_file(buffer_t *buffer, const char *filename);

/* 文本操作 */
bool buffer_insert(buffer_t *buffer, size_t pos, const char *text, size_t len);
bool buffer_delete(buffer_t *buffer, size_t pos, size_t len);
bool buffer_replace(buffer_t *buffer, size_t pos, size_t len, const char *text, size_t new_len);

/* 光标操作 */
void buffer_set_cursor(buffer_t *buffer, size_t line, size_t col);
void buffer_get_cursor(const buffer_t *buffer, size_t *line, size_t *col);
bool buffer_move_cursor(buffer_t *buffer, long line_offset, long col_offset);
bool buffer_goto_line(buffer_t *buffer, size_t line);
bool buffer_goto_column(buffer_t *buffer, size_t col);

/* 行操作 */
size_t buffer_get_line_count(const buffer_t *buffer);
size_t buffer_get_line_length(const buffer_t *buffer, size_t line);
char *buffer_get_line_text(const buffer_t *buffer, size_t line);
bool buffer_insert_line(buffer_t *buffer, size_t line, const char *text);
bool buffer_delete_line(buffer_t *buffer, size_t line);
bool buffer_split_line(buffer_t *buffer, size_t line, size_t col);
bool buffer_join_lines(buffer_t *buffer, size_t line);

/* 搜索和替换 */
size_t buffer_search(const buffer_t *buffer, const char *pattern, size_t start_pos, bool forward);
size_t buffer_search_next(const buffer_t *buffer, const char *pattern);
size_t *buffer_find_all(const buffer_t *buffer, const char *pattern, size_t *count);
bool buffer_replace_text(buffer_t *buffer, const char *pattern, const char *replacement, size_t *replace_count);
bool buffer_replace_range(buffer_t *buffer, size_t start_pos, size_t end_pos, const char *replacement);

/* 撤销和重做 */
bool buffer_undo(buffer_t *buffer);
bool buffer_redo(buffer_t *buffer);
void buffer_clear_undo(buffer_t *buffer);

/* 标记操作 */
void buffer_set_mark(buffer_t *buffer, char mark, size_t line, size_t col);
bool buffer_goto_mark(buffer_t *buffer, char mark);

/* 文本统计 */
size_t buffer_get_char_count(const buffer_t *buffer);
size_t buffer_get_word_count(const buffer_t *buffer);
size_t buffer_get_byte_count(const buffer_t *buffer);

/* 缓冲区信息 */
const char *buffer_get_filename(const buffer_t *buffer);
const char *buffer_get_display_name(const buffer_t *buffer);
bool buffer_is_modified(const buffer_t *buffer);
bool buffer_is_readonly(const buffer_t *buffer);
buffer_type_t buffer_get_type(const buffer_t *buffer);

/* 内存管理 */
bool buffer_resize(buffer_t *buffer, size_t new_capacity);
bool buffer_compact(buffer_t *buffer);
void buffer_clear(buffer_t *buffer);

/* 调试和诊断 */
void buffer_dump_info(const buffer_t *buffer);
void buffer_check_integrity(const buffer_t *buffer);

#endif /* __VI_BUFFER_H__ */
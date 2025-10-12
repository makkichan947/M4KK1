/*
 * M4KK1 VI编辑器 - 输入处理接口
 * 定义键盘输入、命令处理和宏系统
 */

#ifndef __VI_INPUT_H__
#define __VI_INPUT_H__

#include "editor.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 键盘按键定义 */
#define KEY_ESCAPE      27
#define KEY_ENTER       10
#define KEY_TAB         9
#define KEY_BACKSPACE   8
#define KEY_DELETE      127
#define KEY_INSERT      -1
#define KEY_HOME        -2
#define KEY_END         -3
#define KEY_PAGE_UP     -4
#define KEY_PAGE_DOWN   -5
#define KEY_ARROW_UP    -6
#define KEY_ARROW_DOWN  -7
#define KEY_ARROW_LEFT  -8
#define KEY_ARROW_RIGHT -9
#define KEY_F1          -10
#define KEY_F2          -11
#define KEY_F3          -12
#define KEY_F4          -13
#define KEY_F5          -14
#define KEY_F6          -15
#define KEY_F7          -16
#define KEY_F8          -17
#define KEY_F9          -18
#define KEY_F10         -19
#define KEY_F11         -20
#define KEY_F12         -21

/* 修饰键标志 */
#define MOD_NONE        0
#define MOD_CTRL        (1 << 0)
#define MOD_ALT         (1 << 1)
#define MOD_SHIFT       (1 << 2)
#define MOD_META        (1 << 3)

/* 按键事件结构 */
typedef struct {
    int key;                /* 按键代码 */
    int modifiers;          /* 修饰键 */
    char *text;             /* 文本表示 */
} key_event_t;

/* 键盘映射结构 */
typedef struct keymap_entry {
    int key;                        /* 按键 */
    int modifiers;                  /* 修饰键 */
    char *command;                  /* 命令 */
    editor_mode_t mode;             /* 适用模式 */
    struct keymap_entry *next;      /* 下一个映射 */
} keymap_entry_t;

/* 键盘映射表 */
typedef struct {
    keymap_entry_t *entries;        /* 映射条目 */
    size_t count;                   /* 条目数量 */
} keymap_t;

/* 命令历史结构 */
typedef struct history_entry {
    char *command;                  /* 命令文本 */
    time_t timestamp;               /* 时间戳 */
    struct history_entry *next;     /* 下一个条目 */
    struct history_entry *prev;     /* 上一个条目 */
} history_entry_t;

/* 命令历史 */
typedef struct {
    history_entry_t *entries;       /* 历史条目 */
    history_entry_t *current;       /* 当前条目 */
    size_t max_entries;             /* 最大条目数 */
    size_t count;                   /* 当前数量 */
} command_history_t;

/* 宏定义结构 */
typedef struct {
    char name;                      /* 宏寄存器名 */
    char *commands;                 /* 命令序列 */
    size_t length;                  /* 长度 */
    bool recursive;                 /* 是否递归 */
} macro_t;

/* 输入缓冲区结构 */
typedef struct {
    char *buffer;                   /* 输入缓冲区 */
    size_t size;                    /* 当前大小 */
    size_t capacity;                /* 容量 */
    size_t cursor;                  /* 光标位置 */
    editor_mode_t mode;             /* 输入模式 */
} input_buffer_t;

/* 函数声明 */

/* 输入系统初始化 */
bool input_init(void);
void input_cleanup(void);

/* 键盘映射管理 */
keymap_t *keymap_create(void);
void keymap_destroy(keymap_t *keymap);
bool keymap_load(keymap_t *keymap, const char *filename);
bool keymap_save(const keymap_t *keymap, const char *filename);
bool keymap_add(keymap_t *keymap, int key, int modifiers, const char *command, editor_mode_t mode);
bool keymap_remove(keymap_t *keymap, int key, int modifiers, editor_mode_t mode);
const char *keymap_lookup(const keymap_t *keymap, int key, int modifiers, editor_mode_t mode);

/* 命令历史管理 */
command_history_t *history_create(size_t max_entries);
void history_destroy(command_history_t *history);
bool history_add(command_history_t *history, const char *command);
const char *history_get(command_history_t *history, int offset);
bool history_search(command_history_t *history, const char *prefix);

/* 宏系统 */
macro_t *macro_create(char name);
void macro_destroy(macro_t *macro);
bool macro_record(macro_t *macro, const char *commands);
bool macro_play(const macro_t *macro, editor_t *editor);
bool macro_save(macro_t *macro, const char *filename);
macro_t *macro_load(const char *filename);

/* 输入缓冲区管理 */
input_buffer_t *input_buffer_create(void);
void input_buffer_destroy(input_buffer_t *buffer);
bool input_buffer_insert(input_buffer_t *buffer, char ch);
bool input_buffer_delete(input_buffer_t *buffer);
void input_buffer_clear(input_buffer_t *buffer);
const char *input_buffer_get_text(const input_buffer_t *buffer);

/* 键盘输入处理 */
key_event_t *input_read_key(void);
bool input_poll_key(key_event_t *event, int timeout_ms);
void input_flush(void);

/* 命令解析和执行 */
bool command_parse(const char *cmd, char **args, size_t *argc);
bool command_execute(editor_t *editor, const char *cmd);
bool command_complete(const char *prefix, char ***completions, size_t *count);

/* 模式切换处理 */
bool input_handle_normal_mode(editor_t *editor, const key_event_t *event);
bool input_handle_insert_mode(editor_t *editor, const key_event_t *event);
bool input_handle_visual_mode(editor_t *editor, const key_event_t *event);
bool input_handle_command_mode(editor_t *editor, const key_event_t *event);

/* 特殊键处理 */
void input_handle_escape(editor_t *editor);
void input_handle_enter(editor_t *editor);
void input_handle_tab(editor_t *editor);
void input_handle_backspace(editor_t *editor);
void input_handle_arrows(editor_t *editor, int key);
void input_handle_function_keys(editor_t *editor, int key);

/* 输入法支持 */
bool input_method_init(const char *method);
void input_method_cleanup(void);
bool input_method_filter(char *text);

/* 鼠标支持 */
bool mouse_init(void);
void mouse_cleanup(void);
bool mouse_get_event(int *x, int *y, int *button, int *modifiers);

#endif /* __VI_INPUT_H__ */
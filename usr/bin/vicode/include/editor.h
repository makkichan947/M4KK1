#ifndef EDITOR_H
#define EDITOR_H

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <wchar.h>
#include <locale.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <sys/ioctl.h>

#define VERSION "0.1"
#define TAB_STOP 4
#define QUIT_TIMES 1

// Key definitions
#define CTRL_KEY(k) ((k) & 0x1f)
#define ALT_KEY(k) ((k) + 1000)
#define ARROW_LEFT 1000
#define ARROW_RIGHT 1001
#define ARROW_UP 1002
#define ARROW_DOWN 1003
#define DEL_KEY 1004
#define HOME_KEY 1005
#define END_KEY 1006
#define PAGE_UP 1007
#define PAGE_DOWN 1008
#define BACKSPACE 127

// Vim-like key bindings
#define KEY_ESC 27
#define KEY_TAB 9
#define KEY_SPACE 32

// Color definitions
#define COLOR_DEFAULT 0
#define COLOR_KEYWORD 1
#define COLOR_TYPE 2
#define COLOR_COMMENT 3
#define COLOR_STRING 4
#define COLOR_NUMBER 5
#define COLOR_PREPROCESSOR 6
#define COLOR_SEARCH_MATCH 7

// Theme definitions
typedef struct {
    char *name;
    int colors[8]; // COLOR_DEFAULT to COLOR_SEARCH_MATCH
    int bg_color;
    int fg_color;
} Theme;

extern Theme themes[];
extern int current_theme;

// Syntax highlighting types
#define HL_NORMAL 0
#define HL_COMMENT 1
#define HL_MLCOMMENT 2
#define HL_KEYWORD1 3
#define HL_KEYWORD2 4
#define HL_STRING 5
#define HL_NUMBER 6
#define HL_MATCH 7
#define HL_PREPROCESSOR 8

// Editor modes
typedef enum {
    MODE_NORMAL = 0,
    MODE_INSERT = 1,
    MODE_VISUAL = 2,
    MODE_COMMAND = 3,
    MODE_SEARCH = 4
} EditorMode;

// Editor row structure
typedef struct erow {
    int idx;
    int size;
    int rsize;
    char *chars;
    char *render;
    unsigned char *hl;
    int hl_open_comment;
} erow;

// Buffer structure for multiple file support
typedef struct Buffer {
    int cx, cy;           // Cursor position
    int rx;               // Real cursor position (taking tabs into account)
    int rowoff;           // Row offset
    int coloff;           // Column offset
    int numrows;          // Number of rows
    erow *row;            // Rows
    int dirty;            // Dirty flag (modified)
    char *filename;       // Filename
    struct stat file_stat;// File statistics
    char *syntax_name;    // Syntax name
    struct Buffer *next;  // Next buffer in list
    struct Buffer *prev;  // Previous buffer in list
} Buffer;

// Editor configuration structure
struct editorConfig {
    Buffer *current_buffer; // Current active buffer
    Buffer *buffer_list;    // List of all buffers
    int buffer_count;       // Number of buffers
    int screenrows;         // Screen rows
    int screencols;         // Screen columns
    int show_sidebar;       // Show file explorer sidebar
    EditorMode mode;        // Current editor mode
    int visual_start_x;     // Visual mode start position
    int visual_start_y;     // Visual mode start position
    char command_buf[256];  // Command buffer
    int command_len;        // Command buffer length
    char statusmsg[80];     // Status message
    time_t statusmsg_time;  // Status message time
};

// Syntax highlighting
typedef struct editorSyntax {
    char *filetype;
    char **filematch;
    char **keywords;
    char *singleline_comment_start;
    char *multiline_comment_start;
    char *multiline_comment_end;
    int flags;
} editorSyntax;

#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)

// Global editor instance
extern struct editorConfig E;

// Clipboard
extern char *clipboard;

// Function declarations
void editorInit();
void enableRawMode();
void disableRawMode();
void editorOpen(char *filename);
void editorSave();
void editorFind();
void editorFindAndReplace();
void editorFindRegex();
void editorIncrementalSearch();
void editorRefreshScreen();
void editorProcessKeypress();
void editorSetStatusMessage(const char *fmt, ...);
char *editorPrompt(char *prompt, void (*callback)(char *, int));
int editorReadKey();
void editorMoveCursor(int key);
void editorInsertChar(int c);
void editorInsertNewline();
void editorDelChar();
void editorCopy();
void editorPaste();
void editorInsertRow(int at, char *s, size_t len);
void editorDelRow(int at);
void editorUpdateRow(erow *row);
void editorRowInsertChar(erow *row, int at, int c);
void editorRowAppendString(erow *row, char *s, size_t len);
void editorRowDelChar(erow *row, int at);
int editorRowCxToRx(erow *row, int cx);
int editorRowRxToCx(erow *row, int rx);
void editorUpdateSyntax(erow *row);
int editorSyntaxToColor(int hl);
void editorSelectSyntaxHighlight();
char *editorRowsToString(int *buflen);
void die(const char *s);
int getWindowSize(int *rows, int *cols);
void setColor(int fg, int bg);
int getColor();

// Mode management functions
void setEditorMode(EditorMode mode);
void handleNormalMode(int key);
void handleInsertMode(int key);
void handleVisualMode(int key);
void handleCommandMode(int key);
void handleSearchMode(int key);

// Vim-like movement functions
void moveCursorWord(int direction);
void moveCursorWordEnd(int direction);
void moveCursorLine(int direction);
void moveCursorToLineStart();
void moveCursorToLineEnd();
void moveCursorToBufferStart();
void moveCursorToBufferEnd();
void moveCursorPage(int direction);

// Vim-like editing functions
void deleteWord(int direction);
void deleteLine();
void deleteToLineEnd();
void deleteToLineStart();
void changeWord(int direction);
void changeLine();
void changeToLineEnd();
void yankLine();
void yankWord(int direction);
void pasteAfter();
void pasteBefore();

// Visual mode functions
void startVisualMode();
void updateVisualSelection();
void yankVisualSelection();
void deleteVisualSelection();
void changeVisualSelection();

// Command mode functions
void executeCommand(const char *cmd);
void showCommandPalette();

// Search functions
void searchWordUnderCursor();
void searchNext();
void searchPrevious();

// Settings TUI functions
void initSettings();
void loadSettings();
void saveSettings();
void showSettingsMenu();
void applySettings();
void showSettingsInEditor();
char *getSetting(const char *name);
void setSetting(const char *name, const char *value);

// File explorer functions
void toggleFileExplorer();
void openFileFromExplorer(char *filename);
void listFilesInDirectory();
void runExternalCommand();

// Undo/Redo system
typedef struct UndoStep {
    char *operation;
    int row;
    int col;
    char *old_text;
    char *new_text;
    struct UndoStep *next;
    struct UndoStep *prev;
} UndoStep;

// Macro recording system
typedef struct MacroStep {
    int key;
    struct MacroStep *next;
} MacroStep;

typedef struct Macro {
    char name;
    MacroStep *steps;
    int step_count;
    struct Macro *next;
} Macro;

void undoInit();
void undoAddStep(const char *operation, int row, int col, const char *old_text, const char *new_text);
void undoPerform();
void redoPerform();

// Macro functions
void macroInit();
void macroStartRecording(char reg);
void macroStopRecording();
void macroPlayback(char reg);
void macroAddStep(int key);
int macroIsRecording();

// Plugin system
typedef struct Plugin {
    char *name;
    char *path;
    void *handle;  // For dynamic loading
    int enabled;
    struct Plugin *next;
} Plugin;

// Plugin hooks
typedef enum {
    HOOK_INIT,
    HOOK_KEYPRESS,
    HOOK_MODE_CHANGE,
    HOOK_FILE_OPEN,
    HOOK_FILE_SAVE,
    HOOK_BUFFER_CHANGE,
    HOOK_QUIT
} PluginHookType;

typedef void (*PluginHook)(void *data);

// Plugin functions
void pluginInit();
int pluginLoad(const char *path);
void pluginUnload(const char *name);
void pluginCallHook(PluginHookType hook, void *data);
Plugin *pluginGet(const char *name);
void pluginList();

// Theme functions
void nextTheme();
void prevTheme();
void setTheme(const char *theme_name);

// Buffer management functions
Buffer *bufferCreate();
void bufferDestroy(Buffer *buf);
Buffer *bufferOpenFile(const char *filename);
void bufferSave(Buffer *buf);
void bufferSwitch(Buffer *buf);
void bufferNext();
void bufferPrev();
void bufferClose(Buffer *buf);
void bufferList();

#endif
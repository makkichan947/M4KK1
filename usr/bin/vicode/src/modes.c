#include "../include/editor.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// Undo/Redo system
UndoStep *undo_stack = NULL;
UndoStep *redo_stack = NULL;
UndoStep *current_undo = NULL;

// Macro recording system
Macro *macros = NULL;
Macro *current_macro = NULL;
int macro_recording = 0;

void undoInit() {
    // Initialize undo system
    undo_stack = NULL;
    redo_stack = NULL;
    current_undo = NULL;
}

void undoAddStep(const char *operation, int row, int col, const char *old_text, const char *new_text) {
    UndoStep *step = malloc(sizeof(UndoStep));
    step->operation = malloc(strlen(operation) + 1);
    strcpy(step->operation, operation);
    step->row = row;
    step->col = col;
    step->old_text = old_text ? malloc(strlen(old_text) + 1) : NULL;
    if (step->old_text) strcpy(step->old_text, old_text);
    step->new_text = new_text ? malloc(strlen(new_text) + 1) : NULL;
    if (step->new_text) strcpy(step->new_text, new_text);
    step->next = NULL;
    step->prev = current_undo;

    if (current_undo) {
        current_undo->next = step;
    }
    current_undo = step;

    // Clear redo stack when new action is performed
    while (redo_stack) {
        UndoStep *temp = redo_stack;
        redo_stack = redo_stack->next;
        free(temp->operation);
        if (temp->old_text) free(temp->old_text);
        if (temp->new_text) free(temp->new_text);
        free(temp);
    }
}

void undoPerform() {
    if (!current_undo) return;

    // Move current step to redo stack
    UndoStep *step = current_undo;
    if (step->prev) {
        step->prev->next = NULL;
    }

    step->next = redo_stack;
    redo_stack = step;

    // Move to previous step
    current_undo = step->prev;

    // Apply undo operation
    E.cy = step->row;
    E.cx = step->col;

    if (strcmp(step->operation, "insert") == 0) {
        // Undo insert - delete characters
        if (step->old_text && E.cy < E.numrows) {
            erow *row = &E.row[E.cy];
            int len = strlen(step->old_text);
            for (int i = 0; i < len; i++) {
                editorRowDelChar(row, step->col);
            }
            editorUpdateRow(row);
        }
    } else if (strcmp(step->operation, "delete") == 0) {
        // Undo delete - insert characters
        if (step->new_text && E.cy < E.numrows) {
            erow *row = &E.row[E.cy];
            int len = strlen(step->new_text);
            for (int i = 0; i < len; i++) {
                editorRowInsertChar(row, step->col + i, step->new_text[i]);
            }
            editorUpdateRow(row);
        }
    }

    E.dirty = 1;
    editorSetStatusMessage("Undid: %s", step->operation);
}

void redoPerform() {
    if (!redo_stack) return;

    // Move step from redo to undo stack
    UndoStep *step = redo_stack;
    redo_stack = step->next;

    step->prev = current_undo;
    step->next = NULL;

    if (current_undo) {
        current_undo->next = step;
    }
    current_undo = step;

    // Apply redo operation
    E.cy = step->row;
    E.cx = step->col;

    if (strcmp(step->operation, "insert") == 0) {
        // Redo insert - insert characters
        if (step->new_text && E.cy < E.numrows) {
            erow *row = &E.row[E.cy];
            int len = strlen(step->new_text);
            for (int i = 0; i < len; i++) {
                editorRowInsertChar(row, step->col + i, step->new_text[i]);
            }
            editorUpdateRow(row);
        }
    } else if (strcmp(step->operation, "delete") == 0) {
        // Redo delete - delete characters
        if (step->old_text && E.cy < E.numrows) {
            erow *row = &E.row[E.cy];
            int len = strlen(step->old_text);
            for (int i = 0; i < len; i++) {
                editorRowDelChar(row, step->col);
            }
            editorUpdateRow(row);
        }
    }

    E.dirty = 1;
    editorSetStatusMessage("Redid: %s", step->operation);
}

// Set editor mode
void setEditorMode(EditorMode mode) {
    E.mode = mode;
    E.command_len = 0;
    E.command_buf[0] = '\0';

    switch (mode) {
        case MODE_NORMAL:
            editorSetStatusMessage("-- NORMAL --");
            break;
        case MODE_INSERT:
            editorSetStatusMessage("-- INSERT --");
            break;
        case MODE_VISUAL:
            editorSetStatusMessage("-- VISUAL --");
            E.visual_start_x = E.cx;
            E.visual_start_y = E.cy;
            break;
        case MODE_COMMAND:
            editorSetStatusMessage(":");
            break;
        case MODE_SEARCH:
            editorSetStatusMessage("/");
            break;
    }
}

// Handle normal mode keypresses
void handleNormalMode(int key) {
    switch (key) {
        case 'i':
            setEditorMode(MODE_INSERT);
            break;
        case 'I':
            moveCursorToLineStart();
            setEditorMode(MODE_INSERT);
            break;
        case 'a':
            editorMoveCursor(ARROW_RIGHT);
            setEditorMode(MODE_INSERT);
            break;
        case 'A':
            moveCursorToLineEnd();
            setEditorMode(MODE_INSERT);
            break;
        case 'o':
            moveCursorToLineEnd();
            editorInsertNewline();
            setEditorMode(MODE_INSERT);
            break;
        case 'O':
            moveCursorToLineStart();
            editorInsertNewline();
            E.cy--; // Move to the new line
            setEditorMode(MODE_INSERT);
            break;
        case 'v':
            startVisualMode();
            break;
        case ':':
            setEditorMode(MODE_COMMAND);
            break;
        case '/':
            setEditorMode(MODE_SEARCH);
            break;
        case 'h':
        case ARROW_LEFT:
            editorMoveCursor(ARROW_LEFT);
            break;
        case 'j':
        case ARROW_DOWN:
            editorMoveCursor(ARROW_DOWN);
            break;
        case 'k':
        case ARROW_UP:
            editorMoveCursor(ARROW_UP);
            break;
        case 'l':
        case ARROW_RIGHT:
            editorMoveCursor(ARROW_RIGHT);
            break;
        case 'w':
            moveCursorWord(1);
            break;
        case 'b':
            moveCursorWord(-1);
            break;
        case 'e':
            moveCursorWordEnd(1);
            break;
        case '0':
        case HOME_KEY:
            moveCursorToLineStart();
            break;
        case '$':
        case END_KEY:
            moveCursorToLineEnd();
            break;
        case 'G':
            moveCursorToBufferEnd();
            break;
        case 'g':
            if (E.command_len == 1 && E.command_buf[0] == 'g') {
                moveCursorToBufferStart();
                E.command_len = 0;
            } else {
                E.command_buf[E.command_len++] = 'g';
            }
            break;
        case 'x':
            editorDelChar();
            break;
        case 'X':
            editorMoveCursor(ARROW_LEFT);
            editorDelChar();
            break;
        case 'd':
            if (E.command_len == 0) {
                E.command_buf[E.command_len++] = 'd';
            } else if (strcmp(E.command_buf, "d") == 0) {
                deleteLine();
                E.command_len = 0;
            }
            break;
        case 'D':
            deleteToLineEnd();
            break;
        case 'c':
            if (E.command_len == 0) {
                E.command_buf[E.command_len++] = 'c';
            } else if (strcmp(E.command_buf, "c") == 0) {
                changeLine();
                E.command_len = 0;
            }
            break;
        case 'C':
            changeToLineEnd();
            break;
        case 'y':
            if (E.command_len == 0) {
                E.command_buf[E.command_len++] = 'y';
            } else if (strcmp(E.command_buf, "y") == 0) {
                yankLine();
                E.command_len = 0;
            }
            break;
        case 'Y':
            yankLine();
            break;
        case 'p':
            pasteAfter();
            break;
        case 'P':
            pasteBefore();
            break;
        case 'u':
            undoPerform();
            break;
        case CTRL_KEY('r'):
            redoPerform();
            break;
        case 'n':
            searchNext();
            break;
        case 'N':
            searchPrevious();
            break;
        case '*':
            searchWordUnderCursor();
            break;
        case CTRL_KEY('d'):
            moveCursorPage(1);
            break;
        case CTRL_KEY('u'):
            moveCursorPage(-1);
            break;
        case CTRL_KEY('f'):
            moveCursorPage(1);
            break;
        case CTRL_KEY('b'):
            moveCursorPage(-1);
            break;
        case CTRL_KEY('t'):
            showSettingsMenu();
            break;
        case CTRL_KEY('s'):
            editorSave();
            break;
        case CTRL_KEY('w'):
            // Quick save and continue
            editorSave();
            editorSetStatusMessage("File saved");
            break;
        case CTRL_KEY('z'):
            undoPerform();
            break;
        case CTRL_KEY('y'):
            redoPerform();
            break;
        case CTRL_KEY('f'):
            editorFind();
            break;
        case CTRL_KEY('r'):
            editorFindAndReplace();
            break;
        case CTRL_KEY('g'):
            // Go to line
            {
                char *line_str = editorPrompt("Go to line: %s", NULL);
                if (line_str) {
                    int line = atoi(line_str);
                    if (line > 0 && line <= E.numrows) {
                        E.cy = line - 1;
                        E.cx = 0;
                        editorSetStatusMessage("Jumped to line %d", line);
                    } else {
                        editorSetStatusMessage("Invalid line number");
                    }
                    free(line_str);
                }
            }
            break;
        case 'F':
            if (E.command_len == 1 && E.command_buf[0] == 'F') {
                nextTheme();
                E.command_len = 0;
            } else {
                E.command_buf[E.command_len++] = 'F';
            }
            break;
        case 'T':
            if (E.command_len == 1 && E.command_buf[0] == 'T') {
                prevTheme();
                E.command_len = 0;
            } else {
                E.command_buf[E.command_len++] = 'T';
            }
            break;
        case 'q':
            if (macro_recording) {
                macroStopRecording();
            } else {
                // Start recording macro - wait for register
                E.command_buf[E.command_len++] = 'q';
            }
            break;
        case '@':
            // Playback macro - wait for register
            E.command_buf[E.command_len++] = '@';
            break;
        default:
            // Handle macro recording register input
            if (E.command_len == 1 && E.command_buf[0] == 'q') {
                macroStartRecording(key);
                E.command_len = 0;
            } else if (E.command_len == 1 && E.command_buf[0] == '@') {
                macroPlayback(key);
                E.command_len = 0;
            } else {
                E.command_len = 0;
            }
            break;
    }
}

// Handle insert mode keypresses
void handleInsertMode(int key) {
    switch (key) {
        case KEY_ESC:
            setEditorMode(MODE_NORMAL);
            if (E.cx > 0) {
                E.cx--;
            }
            break;
        case BACKSPACE:
        case CTRL_KEY('h'):
        case DEL_KEY:
            if (key == DEL_KEY) editorMoveCursor(ARROW_RIGHT);
            editorDelChar();
            break;
        case ARROW_LEFT:
        case ARROW_RIGHT:
        case ARROW_UP:
        case ARROW_DOWN:
        case HOME_KEY:
        case END_KEY:
        case PAGE_UP:
        case PAGE_DOWN:
            editorMoveCursor(key);
            break;
        case '\r':
            editorInsertNewline();
            break;
        case '\t':
            for (int i = 0; i < TAB_STOP; i++) {
                editorInsertChar(' ');
            }
            break;
        default:
            if (!iscntrl(key) && key < 128) {
                editorInsertChar(key);
            }
            break;
    }
}

// Handle visual mode keypresses
void handleVisualMode(int key) {
    switch (key) {
        case KEY_ESC:
            setEditorMode(MODE_NORMAL);
            break;
        case 'h':
        case ARROW_LEFT:
            editorMoveCursor(ARROW_LEFT);
            updateVisualSelection();
            break;
        case 'j':
        case ARROW_DOWN:
            editorMoveCursor(ARROW_DOWN);
            updateVisualSelection();
            break;
        case 'k':
        case ARROW_UP:
            editorMoveCursor(ARROW_UP);
            updateVisualSelection();
            break;
        case 'l':
        case ARROW_RIGHT:
            editorMoveCursor(ARROW_RIGHT);
            updateVisualSelection();
            break;
        case 'w':
            moveCursorWord(1);
            updateVisualSelection();
            break;
        case 'b':
            moveCursorWord(-1);
            updateVisualSelection();
            break;
        case '0':
        case HOME_KEY:
            moveCursorToLineStart();
            updateVisualSelection();
            break;
        case '$':
        case END_KEY:
            moveCursorToLineEnd();
            updateVisualSelection();
            break;
        case 'G':
            moveCursorToBufferEnd();
            updateVisualSelection();
            break;
        case 'g':
            if (E.command_len == 1 && E.command_buf[0] == 'g') {
                moveCursorToBufferStart();
                updateVisualSelection();
                E.command_len = 0;
            } else {
                E.command_buf[E.command_len++] = 'g';
            }
            break;
        case 'y':
            yankVisualSelection();
            setEditorMode(MODE_NORMAL);
            break;
        case 'd':
        case 'x':
            deleteVisualSelection();
            setEditorMode(MODE_NORMAL);
            break;
        case 'c':
            changeVisualSelection();
            setEditorMode(MODE_INSERT);
            break;
        default:
            E.command_len = 0;
            break;
    }
}

// Handle command mode keypresses
void handleCommandMode(int key) {
    switch (key) {
        case KEY_ESC:
        case '\r':
            executeCommand(E.command_buf);
            setEditorMode(MODE_NORMAL);
            break;
        case BACKSPACE:
        case CTRL_KEY('h'):
            if (E.command_len > 0) {
                E.command_buf[--E.command_len] = '\0';
            }
            break;
        default:
            if (!iscntrl(key) && key < 128 && E.command_len < sizeof(E.command_buf) - 1) {
                E.command_buf[E.command_len++] = key;
                E.command_buf[E.command_len] = '\0';
            }
            break;
    }
}

// Handle search mode keypresses
void handleSearchMode(int key) {
    switch (key) {
        case KEY_ESC:
            setEditorMode(MODE_NORMAL);
            break;
        case '\r':
            searchNext();
            setEditorMode(MODE_NORMAL);
            break;
        case BACKSPACE:
        case CTRL_KEY('h'):
            if (E.command_len > 0) {
                E.command_buf[--E.command_len] = '\0';
            }
            break;
        default:
            if (!iscntrl(key) && key < 128 && E.command_len < sizeof(E.command_buf) - 1) {
                E.command_buf[E.command_len++] = key;
                E.command_buf[E.command_len] = '\0';
            }
            break;
    }
}

// Vim-like movement functions
void moveCursorWord(int direction) {
    if (E.cy >= E.numrows) return;

    erow *row = &E.row[E.cy];
    int start_cx = E.cx;

    if (direction > 0) {
        // Move forward to next word
        while (E.cx < row->size && isspace(row->chars[E.cx])) E.cx++;
        while (E.cx < row->size && !isspace(row->chars[E.cx])) E.cx++;
    } else {
        // Move backward to previous word
        if (E.cx == 0) {
            if (E.cy > 0) {
                E.cy--;
                row = &E.row[E.cy];
                E.cx = row->size;
            }
            return;
        }

        E.cx--;
        while (E.cx > 0 && isspace(row->chars[E.cx])) E.cx--;
        while (E.cx > 0 && !isspace(row->chars[E.cx - 1])) E.cx--;
    }
}

void moveCursorWordEnd(int direction) {
    if (E.cy >= E.numrows) return;

    erow *row = &E.row[E.cy];

    if (direction > 0) {
        // Move to end of current/next word
        while (E.cx < row->size && !isspace(row->chars[E.cx])) E.cx++;
        while (E.cx < row->size && isspace(row->chars[E.cx])) E.cx++;
    } else {
        // Move to end of previous word
        if (E.cx == 0) {
            if (E.cy > 0) {
                E.cy--;
                row = &E.row[E.cy];
                E.cx = row->size;
            }
            return;
        }

        while (E.cx > 0 && isspace(row->chars[E.cx - 1])) E.cx--;
        while (E.cx > 0 && !isspace(row->chars[E.cx - 1])) E.cx--;
    }
}

void moveCursorLine(int direction) {
    if (direction > 0 && E.cy < E.numrows - 1) {
        E.cy++;
    } else if (direction < 0 && E.cy > 0) {
        E.cy--;
    }
    E.cx = 0;
}

void moveCursorToLineStart() {
    E.cx = 0;
}

void moveCursorToLineEnd() {
    if (E.cy < E.numrows) {
        E.cx = E.row[E.cy].size;
    }
}

void moveCursorToBufferStart() {
    E.cy = 0;
    E.cx = 0;
}

void moveCursorToBufferEnd() {
    E.cy = E.numrows - 1;
    if (E.cy >= 0) {
        E.cx = E.row[E.cy].size;
    }
}

void moveCursorPage(int direction) {
    if (direction > 0) {
        E.cy = E.rowoff + E.screenrows - 1;
        if (E.cy > E.numrows - 1) E.cy = E.numrows - 1;
    } else {
        E.cy = E.rowoff;
    }

    int times = E.screenrows;
    while (times--) {
        editorMoveCursor(direction > 0 ? ARROW_DOWN : ARROW_UP);
    }
}

// Vim-like editing functions
void deleteWord(int direction) {
    int start_x = E.cx;
    int start_y = E.cy;

    moveCursorWord(direction);
    int end_x = E.cx;
    int end_y = E.cy;

    // Reset position
    E.cx = start_x;
    E.cy = start_y;

    // Delete from start to end
    if (start_y == end_y) {
        // Same line
        int del_count = abs(end_x - start_x);
        for (int i = 0; i < del_count; i++) {
            editorDelChar();
        }
    }
}

void deleteLine() {
    if (E.cy >= E.numrows) return;

    editorDelRow(E.cy);
    if (E.cy > 0) E.cy--;
    E.cx = 0;
}

void deleteToLineEnd() {
    if (E.cy >= E.numrows) return;

    erow *row = &E.row[E.cy];
    int del_count = row->size - E.cx;
    for (int i = 0; i < del_count; i++) {
        editorDelChar();
    }
}

void deleteToLineStart() {
    if (E.cy >= E.numrows) return;

    int del_count = E.cx;
    for (int i = 0; i < del_count; i++) {
        editorMoveCursor(ARROW_LEFT);
        editorDelChar();
    }
}

void changeWord(int direction) {
    deleteWord(direction);
    setEditorMode(MODE_INSERT);
}

void changeLine() {
    deleteLine();
    editorInsertNewline();
    setEditorMode(MODE_INSERT);
}

void changeToLineEnd() {
    deleteToLineEnd();
    setEditorMode(MODE_INSERT);
}

void yankLine() {
    if (E.cy >= E.numrows) return;

    erow *row = &E.row[E.cy];
    if (clipboard) free(clipboard);
    clipboard = malloc(row->size + 1);
    memcpy(clipboard, row->chars, row->size);
    clipboard[row->size] = '\0';

    editorSetStatusMessage("Yanked line");
}

void yankWord(int direction) {
    // Implementation for yanking words
    editorSetStatusMessage("Yank word not implemented yet");
}

void pasteAfter() {
    if (!clipboard) return;

    int len = strlen(clipboard);
    for (int i = 0; i < len; i++) {
        if (clipboard[i] == '\n') {
            editorInsertNewline();
        } else {
            editorInsertChar(clipboard[i]);
        }
    }
}

void pasteBefore() {
    if (!clipboard) return;

    editorMoveCursor(ARROW_LEFT);
    pasteAfter();
}

// Visual mode functions
void startVisualMode() {
    setEditorMode(MODE_VISUAL);
    E.visual_start_x = E.cx;
    E.visual_start_y = E.cy;
}

void updateVisualSelection() {
    // Update visual selection highlighting
    editorRefreshScreen();
}

void yankVisualSelection() {
    // Implementation for yanking visual selection
    editorSetStatusMessage("Yank visual selection not implemented yet");
}

void deleteVisualSelection() {
    // Implementation for deleting visual selection
    editorSetStatusMessage("Delete visual selection not implemented yet");
}

void changeVisualSelection() {
    deleteVisualSelection();
    setEditorMode(MODE_INSERT);
}

// Command mode functions
void executeCommand(const char *cmd) {
    if (strcmp(cmd, "w") == 0) {
        editorSave();
    } else if (strcmp(cmd, "wq") == 0 || strcmp(cmd, "x") == 0) {
        editorSave();
        clear();
        endwin();
        exit(0);
    } else if (strcmp(cmd, "q") == 0) {
        if (E.dirty) {
            editorSetStatusMessage("No write since last change (add ! to override)");
        } else {
            clear();
            endwin();
            exit(0);
        }
    } else if (strcmp(cmd, "q!") == 0) {
        clear();
        endwin();
        exit(0);
    } else if (strcmp(cmd, "set nu") == 0) {
        editorSetStatusMessage("Line numbers enabled");
    } else if (strcmp(cmd, "set nonu") == 0) {
        editorSetStatusMessage("Line numbers disabled");
    } else if (strncmp(cmd, "set tabstop=", 12) == 0) {
        int tabstop = atoi(cmd + 12);
        editorSetStatusMessage("Tab stop set to %d", tabstop);
    } else {
        editorSetStatusMessage("Unknown command: %s", cmd);
    }
}

void showCommandPalette() {
    // Implementation for command palette
    editorSetStatusMessage("Command palette not implemented yet");
}

// Search functions
void searchWordUnderCursor() {
    if (E.cy >= E.numrows) return;

    erow *row = &E.row[E.cy];
    int start = E.cx;
    int end = E.cx;

    // Find word boundaries
    while (start > 0 && !isspace(row->chars[start - 1])) start--;
    while (end < row->size && !isspace(row->chars[end])) end++;

    if (start < end) {
        int word_len = end - start;
        strncpy(E.command_buf, &row->chars[start], word_len);
        E.command_buf[word_len] = '\0';
        E.command_len = word_len;

        searchNext();
    }
}

void searchNext() {
    if (E.command_len == 0) return;

    // Simple search implementation
    for (int i = E.cy; i < E.numrows; i++) {
        erow *row = &E.row[i];
        char *match = strstr(row->render + (i == E.cy ? E.cx + 1 : 0), E.command_buf);
        if (match) {
            E.cy = i;
            E.cx = editorRowRxToCx(row, match - row->render);
            editorSetStatusMessage("Found: %s", E.command_buf);
            return;
        }
    }

    editorSetStatusMessage("Pattern not found: %s", E.command_buf);
}

void searchPrevious() {
    if (E.command_len == 0) return;

    // Simple reverse search implementation
    for (int i = E.cy; i >= 0; i--) {
        erow *row = &E.row[i];
        char *match = strstr(row->render, E.command_buf);
        if (match && (i < E.cy || match - row->render < E.cx)) {
            E.cy = i;
            E.cx = editorRowRxToCx(row, match - row->render);
            editorSetStatusMessage("Found: %s", E.command_buf);
            return;
        }
    }

    editorSetStatusMessage("Pattern not found: %s", E.command_buf);
}

// Macro recording functions
void macroInit() {
    macros = NULL;
    current_macro = NULL;
    macro_recording = 0;
}

void macroStartRecording(char reg) {
    if (macro_recording) {
        macroStopRecording();
    }

    // Find existing macro or create new one
    Macro *macro = macros;
    while (macro) {
        if (macro->name == reg) {
            // Clear existing macro
            MacroStep *step = macro->steps;
            while (step) {
                MacroStep *next = step->next;
                free(step);
                step = next;
            }
            macro->steps = NULL;
            macro->step_count = 0;
            break;
        }
        macro = macro->next;
    }

    if (!macro) {
        // Create new macro
        macro = malloc(sizeof(Macro));
        macro->name = reg;
        macro->steps = NULL;
        macro->step_count = 0;
        macro->next = macros;
        macros = macro;
    }

    current_macro = macro;
    macro_recording = 1;
    editorSetStatusMessage("Recording macro @%c", reg);
}

void macroStopRecording() {
    if (macro_recording) {
        macro_recording = 0;
        editorSetStatusMessage("Macro recorded @%c (%d steps)", current_macro->name, current_macro->step_count);
        current_macro = NULL;
    }
}

void macroPlayback(char reg) {
    Macro *macro = macros;
    while (macro) {
        if (macro->name == reg) {
            if (macro->step_count == 0) {
                editorSetStatusMessage("Macro @%c is empty", reg);
                return;
            }

            editorSetStatusMessage("Playing macro @%c", reg);

            MacroStep *step = macro->steps;
            while (step) {
                // Simulate key press
                if (E.mode == MODE_NORMAL) {
                    handleNormalMode(step->key);
                } else if (E.mode == MODE_INSERT) {
                    handleInsertMode(step->key);
                } else if (E.mode == MODE_VISUAL) {
                    handleVisualMode(step->key);
                }
                step = step->next;
            }

            editorSetStatusMessage("Macro @%c completed", reg);
            return;
        }
        macro = macro->next;
    }

    editorSetStatusMessage("Macro @%c not found", reg);
}

void macroAddStep(int key) {
    if (!macro_recording || !current_macro) return;

    // Don't record certain keys
    if (key == 'q' && macro_recording) return; // Don't record the stop recording key

    MacroStep *step = malloc(sizeof(MacroStep));
    step->key = key;
    step->next = NULL;

    if (!current_macro->steps) {
        current_macro->steps = step;
    } else {
        MacroStep *last = current_macro->steps;
        while (last->next) last = last->next;
        last->next = step;
    }

    current_macro->step_count++;
}

int macroIsRecording() {
    return macro_recording;
}

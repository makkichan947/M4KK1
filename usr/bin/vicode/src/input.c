#include "../include/editor.h"

// Read a keypress
int editorReadKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }
    
    if (c == '\x1b') {
        char seq[3];
        
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
        
        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
                }
            } else {
                switch (seq[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }
        } else if (seq[0] == 'O') {
            switch (seq[1]) {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }
        
        return '\x1b';
    } else {
        return c;
    }
}

// Move cursor
void editorMoveCursor(int key) {
    erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
    
    switch (key) {
        case ARROW_LEFT:
            if (E.cx != 0) {
                E.cx--;
            } else if (E.cy > 0) {
                E.cy--;
                E.cx = E.row[E.cy].size;
            }
            break;
        case ARROW_RIGHT:
            if (row && E.cx < row->size) {
                E.cx++;
            } else if (row && E.cx == row->size) {
                E.cy++;
                E.cx = 0;
            }
            break;
        case ARROW_UP:
            if (E.cy != 0) {
                E.cy--;
            }
            break;
        case ARROW_DOWN:
            if (E.cy < E.numrows) {
                E.cy++;
            }
            break;
    }
    
    // Fix cursor position if it's past the end of the line
    row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
    int rowlen = row ? row->size : 0;
    if (E.cx > rowlen) {
        E.cx = rowlen;
    }
}

// Process keypress
void editorProcessKeypress() {
    static int quit_times = QUIT_TIMES;

    int c = editorReadKey();

    // Record macro step if recording
    if (macroIsRecording()) {
        macroAddStep(c);
    }

    // Call plugin keypress hook
    pluginCallHook(HOOK_KEYPRESS, &c);

    // Handle mode-specific keypresses
    switch (E.mode) {
        case MODE_NORMAL:
            handleNormalMode(c);
            break;
        case MODE_INSERT:
            handleInsertMode(c);
            break;
        case MODE_VISUAL:
            handleVisualMode(c);
            break;
        case MODE_COMMAND:
            handleCommandMode(c);
            break;
        case MODE_SEARCH:
            handleSearchMode(c);
            break;
    }

    // Handle global shortcuts that work in any mode
    switch (c) {
        case CTRL_KEY('q'):
            if (E.dirty && quit_times > 0) {
                editorSetStatusMessage("WARNING!!! File has unsaved changes. "
                    "Press Ctrl-Q %d more times to quit.", quit_times);
                quit_times--;
                return;
            }
            clear();
            endwin();
            exit(0);
            break;

        case CTRL_KEY('s'):
            editorSave();
            break;

        case CTRL_KEY('e'):
            toggleFileExplorer();
            break;

        case CTRL_KEY('x'):
            // Run external command
            runExternalCommand();
            break;

        case CTRL_KEY('o'):
            // Open file from explorer
            {
                char *filename = editorPrompt("Open file: %s (ESC to cancel)", NULL);
                if (filename != NULL) {
                    editorOpen(filename);
                    free(filename);
                }
            }
            break;

        case KEY_ESC:
            if (E.mode != MODE_NORMAL) {
                setEditorMode(MODE_NORMAL);
            }
            break;

        default:
            // Mode-specific handling is done above
            break;
    }
    
    quit_times = QUIT_TIMES;
}
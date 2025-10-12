#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include "../include/editor.h"
#include "../include/syntax.h"

// Global editor state
struct editorConfig E;
char *clipboard = NULL;

// Enable raw mode for terminal
void enableRawMode() {
    // Initialize ncurses
    initscr();
    if (has_colors() == FALSE) {
        endwin();
        fprintf(stderr, "Your terminal does not support color\n");
        exit(1);
    }
    start_color();
    init_pair(COLOR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_KEYWORD, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_TYPE, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_COMMENT, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_STRING, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_NUMBER, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_PREPROCESSOR, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_SEARCH_MATCH, COLOR_BLACK, COLOR_WHITE);
    
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);
}

// Disable raw mode for terminal
void disableRawMode() {
    endwin();
}

// Die function for error handling
void die(const char *s) {
    clear();
    endwin();
    perror(s);
    exit(1);
}

// Set status message
void editorSetStatusMessage(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
    va_end(ap);
    E.statusmsg_time = time(NULL);
}

// Initialize the editor
void editorInit() {
    E.cx = 0;
    E.cy = 0;
    E.rx = 0;
    E.rowoff = 0;
    E.coloff = 0;
    E.numrows = 0;
    E.row = NULL;
    E.dirty = 0;
    E.filename = NULL;
    E.statusmsg[0] = '\0';
    E.statusmsg_time = 0;
    E.syntax_name = NULL;
    E.show_sidebar = 1; // Show sidebar by default
    E.mode = MODE_NORMAL; // Start in normal mode
    E.visual_start_x = 0;
    E.visual_start_y = 0;
    E.command_buf[0] = '\0';
    E.command_len = 0;

    // Initialize undo system
    undoInit();

    // Initialize and load settings
    initSettings();
    loadSettings();
    applySettings();

    // Get window size
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) {
        die("Unable to get window size");
    }

    // Adjust for status bar
    E.screenrows -= 2;
}

// Open a file
void editorOpen(char *filename) {
    free(E.filename);
    E.filename = strdup(filename);
    
    editorSelectSyntaxHighlight();
    
    FILE *fp = fopen(filename, "r");
    if (!fp) die("fopen");
    
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
            linelen--;
        editorInsertRow(E.numrows, line, linelen);
    }
    
    free(line);
    fclose(fp);
    E.dirty = 0;
}

// Save the current file
void editorSave() {
    if (E.filename == NULL) {
        E.filename = editorPrompt("Save as: %s (ESC to cancel)", NULL);
        if (E.filename == NULL) {
            editorSetStatusMessage("Save aborted");
            return;
        }
        
        // Set syntax highlighting based on file extension
        editorSelectSyntaxHighlight();
    }
    
    int len;
    char *buf = editorRowsToString(&len);
    
    int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1) {
        if (ftruncate(fd, len) != -1) {
            if (write(fd, buf, len) == len) {
                close(fd);
                free(buf);
                E.dirty = 0;
                editorSetStatusMessage("%d bytes written to disk", len);
                return;
            }
        }
        close(fd);
    }
    
    free(buf);
    editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
}

// Convert rows to string
char *editorRowsToString(int *buflen) {
    int totlen = 0;
    int j;
    for (j = 0; j < E.numrows; j++)
        totlen += E.row[j].size + 1;
    *buflen = totlen;
    
    char *buf = malloc(totlen);
    char *p = buf;
    for (j = 0; j < E.numrows; j++) {
        memcpy(p, E.row[j].chars, E.row[j].size);
        p += E.row[j].size;
        *p = '\n';
        p++;
    }
    
    return buf;
}

// Prompt user for input
char *editorPrompt(char *prompt, void (*callback)(char *, int)) {
    size_t bufsize = 128;
    char *buf = malloc(bufsize);
    
    size_t buflen = 0;
    buf[0] = '\0';
    
    while (1) {
        editorSetStatusMessage(prompt, buf);
        editorRefreshScreen();
        
        int c = editorReadKey();
        
        if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
            if (buflen != 0) buf[--buflen] = '\0';
        } else if (c == '\x1b') {
            editorSetStatusMessage("");
            if (callback) callback(buf, c);
            free(buf);
            return NULL;
        } else if (c == '\r') {
            if (buflen != 0) {
                editorSetStatusMessage("");
                if (callback) callback(buf, c);
                return buf;
            }
        } else if (!iscntrl(c) && c < 128) {
            if (buflen == bufsize - 1) {
                bufsize *= 2;
                buf = realloc(buf, bufsize);
            }
            buf[buflen++] = c;
            buf[buflen] = '\0';
        }
        
        if (callback) callback(buf, c);
    }
}

// Find callback
void editorFindCallback(char *query, int key) {
    static int last_match = -1;
    static int direction = 1;
    
    static int saved_hl_line;
    static char *saved_hl = NULL;
    
    if (saved_hl) {
        memcpy(E.row[saved_hl_line].hl, saved_hl, E.row[saved_hl_line].rsize);
        free(saved_hl);
        saved_hl = NULL;
    }
    
    if (key == '\r' || key == '\x1b') {
        last_match = -1;
        direction = 1;
        return;
    } else if (key == ARROW_RIGHT || key == ARROW_DOWN) {
        direction = 1;
    } else if (key == ARROW_LEFT || key == ARROW_UP) {
        direction = -1;
    } else {
        last_match = -1;
        direction = 1;
    }
    
    if (last_match == -1) direction = 1;
    int current = last_match;
    
    int i;
    for (i = 0; i < E.numrows; i++) {
        current += direction;
        if (current == -1) current = E.numrows - 1;
        else if (current == E.numrows) current = 0;
        
        erow *row = &E.row[current];
        char *match = strstr(row->render, query);
        if (match) {
            last_match = current;
            E.cy = current;
            E.cx = editorRowRxToCx(row, match - row->render);
            E.rowoff = E.numrows;
            
            saved_hl_line = current;
            saved_hl = malloc(row->rsize);
            memcpy(saved_hl, row->hl, row->rsize);
            memset(&row->hl[match - row->render], HL_MATCH, strlen(query));
            break;
        }
    }
}

// Find and replace function
void editorFindAndReplace() {
    // First, find the text
    int saved_cx = E.cx;
    int saved_cy = E.cy;
    int saved_coloff = E.coloff;
    int saved_rowoff = E.rowoff;
    
    char *query = editorPrompt("Search for: %s (Use ESC/Arrows/Enter)", NULL);
    
    if (query == NULL) {
        E.cx = saved_cx;
        E.cy = saved_cy;
        E.coloff = saved_coloff;
        E.rowoff = saved_rowoff;
        return;
    }
    
    if (strlen(query) == 0) {
        free(query);
        E.cx = saved_cx;
        E.cy = saved_cy;
        E.coloff = saved_coloff;
        E.rowoff = saved_rowoff;
        return;
    }
    
    // Search for the query
    int found = 0;
    int current_row = 0;
    int current_col = 0;
    
    // Search through all rows
    for (int i = 0; i < E.numrows; i++) {
        erow *row = &E.row[i];
        char *match = strstr(row->render, query);
        if (match) {
            current_row = i;
            current_col = editorRowRxToCx(row, match - row->render);
            found = 1;
            break;
        }
    }
    
    if (!found) {
        editorSetStatusMessage("Pattern not found: %s", query);
        free(query);
        E.cx = saved_cx;
        E.cy = saved_cy;
        E.coloff = saved_coloff;
        E.rowoff = saved_rowoff;
        return;
    }
    
    // Position cursor at found location
    E.cy = current_row;
    E.cx = current_col;
    
    // Ask for replacement text
    char *replacement = editorPrompt("Replace with: %s (ESC to cancel)", NULL);
    
    if (replacement == NULL) {
        free(query);
        E.cx = saved_cx;
        E.cy = saved_cy;
        E.coloff = saved_coloff;
        E.rowoff = saved_rowoff;
        return;
    }
    
    // Perform replacement in all occurrences
    int replacements = 0;
    for (int i = 0; i < E.numrows; i++) {
        erow *row = &E.row[i];
        char *match = strstr(row->chars, query);
        while (match) {
            // Calculate position
            int pos = match - row->chars;
            
            // Delete the old text
            size_t query_len = strlen(query);
            for (size_t j = 0; j < query_len; j++) {
                editorRowDelChar(row, pos);
            }
            
            // Insert the new text
            size_t replacement_len = strlen(replacement);
            for (size_t j = 0; j < replacement_len; j++) {
                editorRowInsertChar(row, pos + j, replacement[j]);
            }
            
            // Look for next occurrence
            match = strstr(row->chars + pos + replacement_len, query);
            replacements++;
        }
        editorUpdateRow(row);
    }
    
    // Update cursor position to beginning of document
    E.cx = 0;
    E.cy = 0;
    E.coloff = 0;
    E.rowoff = 0;
    
    editorSetStatusMessage("Replaced %d occurrence(s)", replacements);
    E.dirty = 1;
    
    free(query);
    free(replacement);
}

// Find function
void editorFind() {
    int saved_cx = E.cx;
    int saved_cy = E.cy;
    int saved_coloff = E.coloff;
    int saved_rowoff = E.rowoff;
    
    char *query = editorPrompt("Search: %s (Use ESC/Arrows/Enter)", editorFindCallback);
    
    if (query) {
        free(query);
    } else {
        E.cx = saved_cx;
        E.cy = saved_cy;
        E.coloff = saved_coloff;
        E.rowoff = saved_rowoff;
    }
}

// Find with regex function
void editorFindRegex() {
    int saved_cx = E.cx;
    int saved_cy = E.cy;
    int saved_coloff = E.coloff;
    int saved_rowoff = E.rowoff;
    
    char *query = editorPrompt("Regex search: %s (Use ESC/Arrows/Enter)", NULL);
    
    if (query == NULL) {
        E.cx = saved_cx;
        E.cy = saved_cy;
        E.coloff = saved_coloff;
        E.rowoff = saved_rowoff;
        return;
    }
    
    if (strlen(query) == 0) {
        free(query);
        E.cx = saved_cx;
        E.cy = saved_cy;
        E.coloff = saved_coloff;
        E.rowoff = saved_rowoff;
        return;
    }
    
    // For now, we'll use a simple string search as regex implementation
    // In a full implementation, this would use regex functions
    
    // Search for the query
    int found = 0;
    int current_row = 0;
    int current_col = 0;
    
    // Search through all rows
    for (int i = 0; i < E.numrows; i++) {
        erow *row = &E.row[i];
        char *match = strstr(row->render, query);
        if (match) {
            current_row = i;
            current_col = editorRowRxToCx(row, match - row->render);
            found = 1;
            break;
        }
    }
    
    if (!found) {
        editorSetStatusMessage("Pattern not found: %s", query);
        free(query);
        E.cx = saved_cx;
        E.cy = saved_cy;
        E.coloff = saved_coloff;
        E.rowoff = saved_rowoff;
        return;
    }
    
    // Position cursor at found location
    E.cy = current_row;
    E.cx = current_col;
    
    editorSetStatusMessage("Found pattern: %s", query);
    free(query);
}

// Copy current line to clipboard
void editorCopy() {
    if (E.cy >= E.numrows) return;
    
    erow *row = &E.row[E.cy];
    if (row->size > 0) {
        if (clipboard != NULL) {
            free(clipboard);
        }
        clipboard = malloc(row->size + 1);
        if (clipboard != NULL) {
            memcpy(clipboard, row->chars, row->size);
            clipboard[row->size] = '\0';
            editorSetStatusMessage("Copied line to clipboard");
        }
    }
}

// Paste from clipboard
void editorPaste() {
    if (clipboard == NULL) return;
    
    int len = strlen(clipboard);
    if (len > 0) {
        // Insert each character from clipboard
        for (int i = 0; i < len; i++) {
            if (clipboard[i] == '\n') {
                editorInsertNewline();
            } else {
                editorInsertChar(clipboard[i]);
            }
        }
        editorSetStatusMessage("Pasted from clipboard");
    }
}

// Incremental search function
void editorIncrementalSearch() {
    int saved_cx = E.cx;
    int saved_cy = E.cy;
    int saved_coloff = E.coloff;
    int saved_rowoff = E.rowoff;
    
    char *query = calloc(1, 2); // Start with empty query
    if (query == NULL) return;
    
    int query_len = 0;
    int query_size = 2;
    
    while (1) {
        editorSetStatusMessage("Incremental search: %s", query);
        editorRefreshScreen();
        
        int c = editorReadKey();
        
        if (c == '\x1b') { // ESC key
            // Cancel search and restore original position
            free(query);
            E.cx = saved_cx;
            E.cy = saved_cy;
            E.coloff = saved_coloff;
            E.rowoff = saved_rowoff;
            editorSetStatusMessage("");
            return;
        } else if (c == '\r') { // Enter key
            // Confirm search
            if (query_len > 0) {
                editorSetStatusMessage("Search completed: %s", query);
            } else {
                editorSetStatusMessage("");
            }
            free(query);
            return;
        } else if (c == BACKSPACE || c == DEL_KEY || c == CTRL_KEY('h')) {
            // Backspace - remove last character
            if (query_len > 0) {
                query_len--;
                query[query_len] = '\0';
                
                // Restore cursor position and search from beginning
                E.cx = saved_cx;
                E.cy = saved_cy;
                E.coloff = saved_coloff;
                E.rowoff = saved_rowoff;
                
                // If we still have characters, search for updated query
                if (query_len > 0) {
                    // Search through all rows
                    int found = 0;
                    for (int i = 0; i < E.numrows; i++) {
                        erow *row = &E.row[i];
                        char *match = strstr(row->render, query);
                        if (match) {
                            E.cy = i;
                            E.cx = editorRowRxToCx(row, match - row->render);
                            found = 1;
                            break;
                        }
                    }
                    
                    if (!found) {
                        // If not found, keep cursor at original position
                        E.cx = saved_cx;
                        E.cy = saved_cy;
                        E.coloff = saved_coloff;
                        E.rowoff = saved_rowoff;
                    }
                }
            }
        } else if (!iscntrl(c) && c < 128) {
            // Add character to query
            if (query_len + 2 > query_size) {
                query_size *= 2;
                char *new_query = realloc(query, query_size);
                if (new_query == NULL) {
                    free(query);
                    return;
                }
                query = new_query;
            }
            
            query[query_len] = c;
            query[query_len + 1] = '\0';
            query_len++;
            
            // Search for updated query
            int found = 0;
            // Start from current position
            for (int i = E.cy; i < E.numrows; i++) {
                erow *row = &E.row[i];
                int start_col = (i == E.cy) ? E.cx : 0;
                if (start_col < row->rsize) {
                    char *match = strstr(row->render + start_col, query);
                    if (match) {
                        E.cy = i;
                        E.cx = editorRowRxToCx(row, match - row->render);
                        found = 1;
                        break;
                    }
                }
            }
            
            // If not found from current position, search from beginning
            if (!found) {
                for (int i = 0; i <= E.cy; i++) {
                    erow *row = &E.row[i];
                    int end_col = (i == E.cy) ? E.cx : row->rsize;
                    char *match = strstr(row->render, query);
                    if (match && (i < E.cy || (i == E.cy && match - row->render < end_col))) {
                        E.cy = i;
                        E.cx = editorRowRxToCx(row, match - row->render);
                        found = 1;
                        break;
                    }
                }
            }
            
            // If still not found, keep cursor at current position
        }
    }
}
#include "../include/editor.h"
#include "../include/syntax.h"

// For DT_DIR
#ifndef DT_DIR
#define DT_DIR 4
#endif

// Theme definitions
Theme themes[] = {
    {
        "default",
        {COLOR_WHITE, COLOR_RED, COLOR_CYAN, COLOR_YELLOW, COLOR_GREEN, COLOR_MAGENTA, COLOR_BLUE, COLOR_BLACK},
        COLOR_BLACK,
        COLOR_WHITE
    },
    {
        "dark",
        {COLOR_WHITE, COLOR_RED, COLOR_CYAN, COLOR_YELLOW, COLOR_GREEN, COLOR_MAGENTA, COLOR_BLUE, COLOR_WHITE},
        COLOR_BLACK,
        COLOR_WHITE
    },
    {
        "light",
        {COLOR_BLACK, COLOR_RED, COLOR_CYAN, COLOR_YELLOW, COLOR_GREEN, COLOR_MAGENTA, COLOR_BLUE, COLOR_RED},
        COLOR_WHITE,
        COLOR_BLACK
    },
    {
        "monokai",
        {COLOR_WHITE, COLOR_RED, COLOR_CYAN, COLOR_YELLOW, COLOR_GREEN, COLOR_MAGENTA, COLOR_BLUE, COLOR_RED},
        COLOR_BLACK,
        COLOR_WHITE
    },
    {
        "solarized_dark",
        {COLOR_WHITE, COLOR_RED, COLOR_CYAN, COLOR_YELLOW, COLOR_GREEN, COLOR_MAGENTA, COLOR_BLUE, COLOR_CYAN},
        COLOR_BLACK,
        COLOR_WHITE
    },
    {
        "solarized_light",
        {COLOR_BLACK, COLOR_RED, COLOR_CYAN, COLOR_YELLOW, COLOR_GREEN, COLOR_MAGENTA, COLOR_BLUE, COLOR_RED},
        COLOR_WHITE,
        COLOR_BLACK
    }
};

int current_theme = 0;
int num_themes = sizeof(themes) / sizeof(Theme);

// Get window size
int getWindowSize(int *rows, int *cols) {
    struct winsize ws;
    
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

// Set color using theme
void setColor(int fg, int bg) {
    Theme *theme = &themes[current_theme];

    if (fg == -1) {
        attroff(COLOR_PAIR(0));
    } else if (fg >= 0 && fg < 8) {
        attron(COLOR_PAIR(theme->colors[fg]));
    } else {
        attron(COLOR_PAIR(fg));
    }

    if (bg != -1) {
        if (bg >= 0 && bg < 8) {
            bkgd(COLOR_PAIR(theme->colors[bg]));
        } else {
            bkgd(COLOR_PAIR(bg));
        }
    }
}

// Get current color
int getColor() {
    return COLOR_PAIR(-1);
}

// Draw the sidebar with file explorer
void editorDrawSidebar() {
    int sidebar_width = 20;
    if (!E.show_sidebar) return;
    
    // Save current color
    int original_color = getColor();
    
    // Set sidebar color
    setColor(COLOR_BLUE, COLOR_BLACK);
    
    // Draw sidebar background
    for (int y = 0; y < E.screenrows + 2; y++) {
        mvprintw(y, 0, "%*s", sidebar_width, "");
    }
    
    // Draw file explorer title
    setColor(COLOR_WHITE, COLOR_BLUE);
    mvprintw(0, 2, "EXPLORER");
    
    // Draw current directory contents
    DIR *dir;
    struct dirent *ent;
    int line = 2;
    
    if ((dir = opendir(".")) != NULL) {
        while ((ent = readdir(dir)) != NULL && line < E.screenrows + 1) {
            if (ent->d_name[0] != '.') {  // Skip hidden files
                if (ent->d_type == DT_DIR) {
                    // Directory
                    setColor(COLOR_CYAN, COLOR_BLACK);
                    mvprintw(line, 1, "📁 %s", ent->d_name);
                } else {
                    // File
                    setColor(COLOR_WHITE, COLOR_BLACK);
                    mvprintw(line, 1, "📄 %s", ent->d_name);
                }
                line++;
            }
        }
        closedir(dir);
    }
    
    // Restore original color
    setColor(original_color, -1);
}

// Draw rows (file content)
void editorDrawRows() {
    int y;
    for (y = 0; y < E.screenrows; y++) {
        int filerow = y + E.rowoff;
        
        if (filerow >= E.numrows) {
            if (E.numrows == 0 && filerow == E.screenrows / 3) {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome),
                    "Vicode editor -- version %s", VERSION);
                if (welcomelen > E.screencols) welcomelen = E.screencols;
                
                int padding = (E.screencols - welcomelen) / 2;
                if (padding) {
                    mvprintw(y, 0, "~");
                    padding--;
                }
                while (padding--) mvprintw(y, getcurx(stdscr), " ");
                
                mvprintw(y, getcurx(stdscr), "%s", welcome);
            } else {
                mvprintw(y, 0, "~");
            }
        } else {
            int len = E.row[filerow].rsize - E.coloff;
            if (len < 0) len = 0;
            if (len > E.screencols) len = E.screencols;
            
            char *c = &E.row[filerow].render[E.coloff];
            unsigned char *hl = &E.row[filerow].hl[E.coloff];
            int current_color = -1;
            
            // Calculate position considering sidebar
            int x_offset = E.show_sidebar ? 21 : 0;
            
            // Print line number
            setColor(COLOR_YELLOW, COLOR_BLACK);
            mvprintw(y, x_offset, "%d ", filerow + 1);
            int line_num_width = snprintf(NULL, 0, "%d ", filerow + 1);
            
            for (int j = 0; j < len; j++) {
                if (iscntrl(c[j])) {
                    char sym = (c[j] <= 26) ? '@' + c[j] : '?';
                    setColor(COLOR_WHITE, COLOR_RED);
                    mvprintw(y, j + line_num_width + x_offset, "%c", sym);
                    setColor(current_color, -1);
                } else if (hl[j] == HL_NORMAL) {
                    if (current_color != -1) {
                        setColor(COLOR_DEFAULT, COLOR_BLACK);
                        current_color = -1;
                    }
                    mvprintw(y, j + line_num_width + x_offset, "%c", c[j]);
                } else {
                    int color = editorSyntaxToColor(hl[j]);
                    if (color != current_color) {
                        setColor(color, COLOR_BLACK);
                        current_color = color;
                    }
                    mvprintw(y, j + line_num_width + x_offset, "%c", c[j]);
                }
            }
            setColor(COLOR_DEFAULT, COLOR_BLACK);
        }
        
        clrtoeol();
    }
}

// Draw the status bar
void editorDrawStatusBar() {
    setColor(COLOR_BLACK, COLOR_WHITE);
    char status[80], rstatus[80];
    const char *mode_str = "";
    switch (E.mode) {
        case MODE_NORMAL: mode_str = "NORMAL"; break;
        case MODE_INSERT: mode_str = "INSERT"; break;
        case MODE_VISUAL: mode_str = "VISUAL"; break;
        case MODE_COMMAND: mode_str = "COMMAND"; break;
        case MODE_SEARCH: mode_str = "SEARCH"; break;
    }

    int len = snprintf(status, sizeof(status), "%.20s - %d lines %s [%s]",
        E.filename ? E.filename : "[No Name]", E.numrows,
        E.dirty ? "(modified)" : "", mode_str);
    int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d",
        E.cy + 1, E.numrows);
    
    int x_offset = E.show_sidebar ? 21 : 0;
    int width = E.screencols - x_offset;
    
    if (len > width) len = width;
    mvprintw(E.screenrows, x_offset, "%s", status);
    
    while (len < width) {
        if (width - len == rlen) {
            mvprintw(E.screenrows, len + x_offset, "%s", rstatus);
            break;
        } else {
            mvprintw(E.screenrows, len + x_offset, " ");
            len++;
        }
    }
    setColor(COLOR_DEFAULT, COLOR_BLACK);
}

// Draw the message bar
void editorDrawMessageBar() {
    int x_offset = E.show_sidebar ? 21 : 0;
    clrtoeol();
    mvprintw(E.screenrows + 1, x_offset, "%s", "");
    
    if (E.statusmsg[0] != '\0' && time(NULL) - E.statusmsg_time < 5) {
        mvprintw(E.screenrows + 1, x_offset, "%s", E.statusmsg);
    }
}

// Refresh the screen with animations
void editorRefreshScreen() {
    clear();

    // Draw sidebar if enabled (with animations)
    if (E.show_sidebar) {
        editorDrawSidebarAnimated();
    }

    // Draw editor content
    editorDrawRows();
    editorDrawStatusBarAnimated();
    editorDrawMessageBar();

    // Position cursor
    int sidebar_offset = E.show_sidebar ? 21 : 0;
    E.rx = editorRowCxToRx((E.cy >= E.numrows) ? NULL : &E.row[E.cy], E.cx);
    move(E.cy - E.rowoff, E.rx - E.coloff + sidebar_offset);

    refresh();
}

// Theme management functions with animation
void nextTheme() {
    current_theme = (current_theme + 1) % num_themes;
    animateThemeTransition();
    editorSetStatusMessage("Theme: %s", themes[current_theme].name);
}

void prevTheme() {
    current_theme = (current_theme - 1 + num_themes) % num_themes;
    animateThemeTransition();
    editorSetStatusMessage("Theme: %s", themes[current_theme].name);
}

void setTheme(const char *theme_name) {
    for (int i = 0; i < num_themes; i++) {
        if (strcmp(themes[i].name, theme_name) == 0) {
            current_theme = i;
            animateThemeTransition();
            editorSetStatusMessage("Theme: %s", themes[current_theme].name);
            return;
        }
    }
    editorSetStatusMessage("Theme '%s' not found", theme_name);
}

// Animated theme transition
void animateThemeTransition() {
    // Simple fade effect by refreshing multiple times
    for (int i = 0; i < 3; i++) {
        editorRefreshScreen();
        usleep(50000); // 50ms delay for visual effect
    }
}

// Enhanced status bar with animations
void editorDrawStatusBarAnimated() {
    // Save current position
    int orig_y, orig_x;
    getyx(stdscr, orig_y, orig_x);

    setColor(COLOR_BLACK, COLOR_WHITE);

    // Animated status bar
    static int animation_frame = 0;
    animation_frame = (animation_frame + 1) % 4;

    char status[80], rstatus[80];
    const char *mode_str = "";
    switch (E.mode) {
        case MODE_NORMAL: mode_str = "NORMAL"; break;
        case MODE_INSERT: mode_str = "INSERT"; break;
        case MODE_VISUAL: mode_str = "VISUAL"; break;
        case MODE_COMMAND: mode_str = "COMMAND"; break;
        case MODE_SEARCH: mode_str = "SEARCH"; break;
    }

    // Add animation indicator for recording mode
    char mode_indicator[20] = "";
    if (macroIsRecording()) {
        const char *indicators[] = {"●", "○", "●", "○"};
        sprintf(mode_indicator, " %s REC", indicators[animation_frame]);
    }

    int len = snprintf(status, sizeof(status), "%.20s - %d lines %s [%s%s]",
        E.filename ? E.filename : "[No Name]", E.numrows,
        E.dirty ? "(modified)" : "", mode_str, mode_indicator);
    int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d",
        E.cy + 1, E.numrows);

    int x_offset = E.show_sidebar ? 21 : 0;
    int width = E.screencols - x_offset;

    if (len > width) len = width;
    mvprintw(E.screenrows, x_offset, "%s", status);

    while (len < width) {
        if (width - len == rlen) {
            mvprintw(E.screenrows, len + x_offset, "%s", rstatus);
            break;
        } else {
            mvprintw(E.screenrows, len + x_offset, " ");
            len++;
        }
    }
    setColor(COLOR_DEFAULT, COLOR_BLACK);

    // Restore cursor position
    move(orig_y, orig_x);
}

// Enhanced sidebar with animations
void editorDrawSidebarAnimated() {
    if (!E.show_sidebar) return;

    int sidebar_width = 20;

    // Animated sidebar background
    static int sidebar_animation = 0;
    sidebar_animation = (sidebar_animation + 1) % 10;

    // Save current color
    int original_color = getColor();

    // Set sidebar color with subtle animation
    int bg_color = (sidebar_animation < 5) ? COLOR_BLUE : COLOR_CYAN;
    setColor(COLOR_WHITE, bg_color);

    // Draw sidebar background
    for (int y = 0; y < E.screenrows + 2; y++) {
        mvprintw(y, 0, "%*s", sidebar_width, "");
    }

    // Draw file explorer title with animation
    const char *titles[] = {"EXPLORER", "EXPLORER.", "EXPLORER..", "EXPLORER..."};
    setColor(COLOR_WHITE, bg_color);
    mvprintw(0, 2, "%s", titles[sidebar_animation % 4]);

    // Draw current directory contents with icons
    DIR *dir;
    struct dirent *ent;
    int line = 2;

    if ((dir = opendir(".")) != NULL) {
        while ((ent = readdir(dir)) != NULL && line < E.screenrows + 1) {
            if (ent->d_name[0] != '.') {  // Skip hidden files
                if (ent->d_type == DT_DIR) {
                    // Animated directory icon
                    const char *dir_icons[] = {"📁", "📂", "🗂️", "📁"};
                    setColor(COLOR_CYAN, COLOR_BLACK);
                    mvprintw(line, 1, "%s %s", dir_icons[sidebar_animation % 4], ent->d_name);
                } else {
                    // File with extension-based icons
                    const char *file_icon = "📄";
                    char *ext = strrchr(ent->d_name, '.');
                    if (ext) {
                        if (strcmp(ext, ".c") == 0 || strcmp(ext, ".h") == 0) file_icon = "🅲";
                        else if (strcmp(ext, ".py") == 0) file_icon = "🐍";
                        else if (strcmp(ext, ".js") == 0) file_icon = "🟨";
                        else if (strcmp(ext, ".html") == 0) file_icon = "🌐";
                        else if (strcmp(ext, ".css") == 0) file_icon = "🎨";
                    }

                    setColor(COLOR_WHITE, COLOR_BLACK);
                    mvprintw(line, 1, "%s %s", file_icon, ent->d_name);
                }
                line++;
            }
        }
        closedir(dir);
    }

    // Restore original color
    setColor(original_color, -1);
}
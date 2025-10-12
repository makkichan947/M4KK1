#include "../include/editor.h"
#include <string.h>
#include <stdlib.h>

// Settings structure
typedef struct {
    char *name;
    char *value;
    char *description;
} Setting;

#define MAX_SETTINGS 50
Setting settings[MAX_SETTINGS];
int settings_count = 0;

// Available settings
void initSettings() {
    settings_count = 0;

    // Editor settings
    settings[settings_count++] = (Setting){"tab_size", "4", "Tab stop size"};
    settings[settings_count++] = (Setting){"show_line_numbers", "true", "Show line numbers"};
    settings[settings_count++] = (Setting){"auto_indent", "true", "Auto indent new lines"};
    settings[settings_count++] = (Setting){"syntax_highlight", "true", "Enable syntax highlighting"};
    settings[settings_count++] = (Setting){"show_sidebar", "true", "Show file explorer sidebar"};
    settings[settings_count++] = (Setting){"word_wrap", "false", "Enable word wrapping"};
    settings[settings_count++] = (Setting){"mouse_enabled", "true", "Enable mouse support"};
    settings[settings_count++] = (Setting){"theme", "default", "Color theme (default/dark/light)"};

    // Vim-like settings
    settings[settings_count++] = (Setting){"vim_mode", "true", "Enable vim-like keybindings"};
    settings[settings_count++] = (Setting){"relative_numbers", "false", "Show relative line numbers"};
    settings[settings_count++] = (Setting){"cursor_line", "true", "Highlight current line"};
    settings[settings_count++] = (Setting){"show_mode", "true", "Show current mode in status bar"};
    settings[settings_count++] = (Setting){"auto_save", "false", "Auto save on focus loss"};
    settings[settings_count++] = (Setting){"search_highlight", "true", "Highlight search results"};
    settings[settings_count++] = (Setting){"incremental_search", "true", "Show search results as you type"};
}

// Get setting value
char *getSetting(const char *name) {
    for (int i = 0; i < settings_count; i++) {
        if (strcmp(settings[i].name, name) == 0) {
            return settings[i].value;
        }
    }
    return NULL;
}

// Set setting value
void setSetting(const char *name, const char *value) {
    for (int i = 0; i < settings_count; i++) {
        if (strcmp(settings[i].name, name) == 0) {
            if (settings[i].value) {
                free(settings[i].value);
            }
            settings[i].value = malloc(strlen(value) + 1);
            strcpy(settings[i].value, value);
            break;
        }
    }
}

// Edit a specific setting
void editSetting(int index) {
    if (index < 0 || index >= settings_count) return;

    Setting *setting = &settings[index];
    char prompt[256];
    snprintf(prompt, sizeof(prompt), "Edit %s (%s): ", setting->name, setting->description);

    char *new_value = editorPrompt(prompt, NULL);
    if (new_value && strlen(new_value) > 0) {
        setSetting(setting->name, new_value);
        free(new_value);
        editorSetStatusMessage("Setting updated: %s = %s", setting->name, setting->value);
    }
}

// Show settings menu
void showSettingsMenu() {
    int current_selection = 0;
    int ch;
    int settings_per_page = E.screenrows - 4;
    int total_pages = (settings_count + settings_per_page - 1) / settings_per_page;
    int current_page = 0;

    while (1) {
        clear();

        // Draw header
        mvprintw(0, 0, "=== Vicode Settings ===");
        mvprintw(1, 0, "Use ↑/↓ to navigate, Enter to edit, 's' to save, 'q' to quit");

        // Calculate visible range
        int start_idx = current_page * settings_per_page;
        int end_idx = start_idx + settings_per_page;
        if (end_idx > settings_count) end_idx = settings_count;

        // Draw settings
        for (int i = start_idx; i < end_idx; i++) {
            int y = (i - start_idx) + 2;
            char *value = settings[i].value;

            if (i == current_selection) {
                attron(A_REVERSE);
            }

            mvprintw(y, 1, "%-20s: %-10s - %s",
                    settings[i].name,
                    value,
                    settings[i].description);

            if (i == current_selection) {
                attroff(A_REVERSE);
            }
        }

        // Draw footer
        mvprintw(E.screenrows - 1, 0, "Page %d/%d | Setting: %s",
                current_page + 1, total_pages, settings[current_selection].name);

        refresh();

        ch = getch();

        switch (ch) {
            case KEY_UP:
            case 'k':
                if (current_selection > start_idx) {
                    current_selection--;
                } else if (current_page > 0) {
                    current_page--;
                    current_selection = start_idx - 1;
                }
                break;

            case KEY_DOWN:
            case 'j':
                if (current_selection < end_idx - 1) {
                    current_selection++;
                } else if (current_page < total_pages - 1) {
                    current_page++;
                    current_selection = end_idx;
                }
                break;

            case KEY_PPAGE:
            case CTRL_KEY('b'):
                if (current_page > 0) {
                    current_page--;
                    current_selection = current_page * settings_per_page;
                }
                break;

            case KEY_NPAGE:
            case CTRL_KEY('f'):
                if (current_page < total_pages - 1) {
                    current_page++;
                    current_selection = current_page * settings_per_page;
                }
                break;

            case '\r':
            case KEY_ENTER:
                // Edit current setting
                editSetting(current_selection);
                break;

            case 's':
            case 'S':
                // Save settings
                saveSettings();
                editorSetStatusMessage("Settings saved");
                return;

            case 'q':
            case 'Q':
            case 27: // ESC
                return;

            case 'r':
            case 'R':
                // Reset to defaults
                initSettings();
                editorSetStatusMessage("Settings reset to defaults");
                break;
        }

        // Keep current_selection in valid range
        if (current_selection >= settings_count) {
            current_selection = settings_count - 1;
        }
        if (current_selection < 0) {
            current_selection = 0;
        }

        // Adjust current_page based on selection
        current_page = current_selection / settings_per_page;
    }
}


// Save settings to file
void saveSettings() {
    char config_dir[256];
    char config_file[512];

    // Create config directory in home directory
    snprintf(config_dir, sizeof(config_dir), "%s/.vicode", getenv("HOME"));
    mkdir(config_dir, 0755);

    snprintf(config_file, sizeof(config_file), "%s/settings.conf", config_dir);

    FILE *fp = fopen(config_file, "w");
    if (!fp) {
        editorSetStatusMessage("Error saving settings: %s", strerror(errno));
        return;
    }

    for (int i = 0; i < settings_count; i++) {
        fprintf(fp, "%s=%s\n", settings[i].name, settings[i].value);
    }

    fclose(fp);
    editorSetStatusMessage("Settings saved to %s", config_file);
}

// Load settings from file
void loadSettings() {
    char config_file[512];
    snprintf(config_file, sizeof(config_file), "%s/.vicode/settings.conf", getenv("HOME"));

    FILE *fp = fopen(config_file, "r");
    if (!fp) {
        // No config file, use defaults
        initSettings();
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;

        // Parse key=value
        char *equals = strchr(line, '=');
        if (equals) {
            *equals = '\0';
            char *key = line;
            char *value = equals + 1;

            setSetting(key, value);
        }
    }

    fclose(fp);
}

// Apply settings to editor
void applySettings() {
    // Apply tab size
    if (strcmp(getSetting("tab_size"), "2") == 0) {
        // Update TAB_STOP
        editorSetStatusMessage("Applied tab size: 2");
    } else if (strcmp(getSetting("tab_size"), "8") == 0) {
        editorSetStatusMessage("Applied tab size: 8");
    }

    // Apply sidebar setting
    if (strcmp(getSetting("show_sidebar"), "false") == 0) {
        E.show_sidebar = 0;
    } else {
        E.show_sidebar = 1;
    }

    // Apply syntax highlighting
    if (strcmp(getSetting("syntax_highlight"), "false") == 0) {
        editorSetStatusMessage("Syntax highlighting disabled");
    }

    editorSetStatusMessage("Settings applied");
}

// Show settings in editor
void showSettingsInEditor() {
    // Clear current file
    while (E.numrows > 0) {
        editorDelRow(0);
    }
    E.cx = 0;
    E.cy = 0;
    E.dirty = 0;

    // Add settings as lines
    editorInsertRow(0, "# Vicode Settings", 16);
    editorInsertRow(1, "", 0);

    for (int i = 0; i < settings_count; i++) {
        char line[256];
        snprintf(line, sizeof(line), "%s = %s  # %s",
                settings[i].name,
                settings[i].value,
                settings[i].description);
        editorInsertRow(i + 2, line, strlen(line));
    }

    editorSetStatusMessage("Settings loaded - edit and save as usual file");
}
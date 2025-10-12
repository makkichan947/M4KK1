#include "../include/editor.h"

// For DT_DIR
#ifndef DT_DIR
#define DT_DIR 4
#endif

// Toggle file explorer sidebar
void toggleFileExplorer() {
    E.show_sidebar = !E.show_sidebar;
    
    // Recalculate screen dimensions
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) {
        die("Unable to get window size");
    }
    
    // Adjust for status bar
    E.screenrows -= 2;
}

// Open file from explorer
void openFileFromExplorer(char *filename) {
    if (filename == NULL) return;
    
    // Check if it's a directory
    struct stat path_stat;
    if (stat(filename, &path_stat) == 0 && S_ISDIR(path_stat.st_mode)) {
        // Change directory
        if (chdir(filename) != 0) {
            editorSetStatusMessage("Cannot enter directory: %s", strerror(errno));
        } else {
            editorSetStatusMessage("Entered directory: %s", filename);
        }
        return;
    }
    
    // It's a file, open it
    editorOpen(filename);
}

// List files in current directory
void listFilesInDirectory() {
    DIR *dir;
    struct dirent *ent;
    
    // Clear the message area
    editorSetStatusMessage("");
    
    if ((dir = opendir(".")) != NULL) {
        int count = 0;
        while ((ent = readdir(dir)) != NULL && count < 10) {
            if (ent->d_name[0] != '.') {  // Skip hidden files
                if (ent->d_type == DT_DIR) {
                    editorSetStatusMessage("%s/", ent->d_name);
                } else {
                    editorSetStatusMessage("%s", ent->d_name);
                }
                count++;
            }
        }
        closedir(dir);
    } else {
        editorSetStatusMessage("Could not open directory");
    }
}

// Run external command
void runExternalCommand() {
    char *cmd = editorPrompt("Run command: %s (ESC to cancel)", NULL);
    if (cmd == NULL) return;
    
    // Save current state
    endwin();
    
    // Run the command
    int result = system(cmd);
    
    // Restore ncurses
    refresh();
    
    if (result == 0) {
        editorSetStatusMessage("Command executed successfully");
    } else {
        editorSetStatusMessage("Command failed with code %d", result);
    }
    
    free(cmd);
}
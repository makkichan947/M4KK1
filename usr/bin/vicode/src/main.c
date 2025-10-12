#include "../include/editor.h"

int main(int argc, char *argv[]) {
    // Enable raw mode
    enableRawMode();
    
    // Initialize the editor
    editorInit();

    // Initialize macro system
    macroInit();

    // Initialize plugin system
    pluginInit();
    
    // If a filename was provided, open it
    if (argc >= 2) {
        editorOpen(argv[1]);
    }
    
    // Set the initial status message
    editorSetStatusMessage(
        "Vicode Editor -- Vim-like keys: i=insert, v=visual, :=command, /=search, u=undo, Ctrl+R=redo");
    
    // Main loop
    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    
    return 0;
}
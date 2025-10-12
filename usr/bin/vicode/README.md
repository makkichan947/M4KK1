# Vicode - Terminal Text Editor

Vicode is a lightweight terminal-based text editor inspired by VSCode and Neovim, written in C. It provides essential features for code editing directly in the terminal with vim-like keybindings and modern features.

## Features

- **Neovim-like editing experience** with multiple modes (Normal, Insert, Visual, Command, Search)
- **Advanced syntax highlighting** for C, Python, JavaScript, TypeScript, Java, Go, Rust, HTML, CSS
- **Multiple themes** (Default, Dark, Light, Monokai, Solarized Dark/Light) with `FF`/`TT` switching
- **Macro recording and playback** (`q{reg}` to record, `@{reg}` to playback)
- **Plugin system** with hooks for extensibility
- **Buffer management** for multiple file editing
- **Integrated file explorer** with sidebar navigation
- **Settings TUI** (`Ctrl+T`) for configuration
- **Undo/Redo system** (`u` for undo, `Ctrl+R` for redo)
- **Advanced search** with incremental search and regex support
- **External command execution** (`Ctrl+X`)
- **Line numbering** and status bar
- **Minimalistic and fast** terminal-based interface

## Keybindings

### Normal Mode
- `i` - Enter insert mode
- `I` - Insert at line start
- `a` - Append after cursor
- `A` - Append at line end
- `o` - Open new line below
- `O` - Open new line above
- `v` - Enter visual mode
- `h/j/k/l` - Move cursor (left/down/up/right)
- `w/b` - Move by word
- `0/$` - Move to line start/end
- `G/gg` - Move to buffer end/start
- `x` - Delete character
- `X` - Delete before cursor
- `d` - Delete (combine with movement)
- `D` - Delete to line end
- `c` - Change (combine with movement)
- `C` - Change to line end
- `y` - Yank (copy)
- `Y` - Yank line
- `p/P` - Paste after/before
- `u` - Undo
- `Ctrl+R` - Redo
- `/` - Search
- `:` - Command mode
- `*` - Search word under cursor
- `n/N` - Next/previous search result
- `Ctrl+D/Ctrl+U` - Page up/down
- `Ctrl+S` - Save file
- `Ctrl+W` - Quick save
- `Ctrl+Z` - Undo
- `Ctrl+Y` - Redo
- `Ctrl+F` - Find
- `Ctrl+R` - Find and replace
- `Ctrl+G` - Go to line
- `FF` - Next theme
- `TT` - Previous theme
- `q{reg}` - Start recording macro to register
- `@{reg}` - Playback macro from register

### Insert Mode
- `Esc` - Return to normal mode
- Standard text editing keys

### Visual Mode
- `Esc` - Return to normal mode
- `y` - Yank selection
- `d/x` - Delete selection
- `c` - Change selection
- Movement keys to expand selection

### Command Mode
- `:w` - Save file
- `:wq` or `:x` - Save and quit
- `:q` - Quit
- `:q!` - Force quit
- `:set option` - Set editor options

## Settings

Press `Ctrl+T` in normal mode to open the interactive settings menu. Settings are automatically saved to `~/.vicode/settings.conf`.

Available settings:
- `tab_size` - Tab stop size (2, 4, 8)
- `show_line_numbers` - Show/hide line numbers
- `auto_indent` - Auto indent new lines
- `syntax_highlight` - Enable/disable syntax highlighting
- `show_sidebar` - Show/hide file explorer
- `vim_mode` - Enable vim-like keybindings
- `theme` - Color theme (default/dark/light)

## Building

```bash
make
```

## Running

```bash
./vicode [filename]
```

## Dependencies

- GCC compiler
- Ncurses library for terminal handling
- Make build system

## Installation

```bash
make install    # Install to /usr/local/bin/
make uninstall  # Remove from system
```

## Configuration

Settings are stored in `~/.vicode/settings.conf` and can be edited:
- Through the settings TUI (`Ctrl+T`)
- By editing the config file directly
- Settings are applied immediately when changed
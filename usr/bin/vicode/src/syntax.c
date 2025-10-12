#include "../include/editor.h"
#include "../include/syntax.h"

// C syntax highlighting
char *C_HL_extensions[] = {".c", ".h", ".cpp", ".hpp", ".cc", ".cxx", NULL};
char *C_HL_keywords[] = {
    "switch", "if", "while", "for", "break", "continue", "return", "else",
    "struct", "union", "typedef", "static", "enum", "class", "case",
    "volatile", "register", "sizeof", "typedef", "union", "goto", "const",
    "auto", "extern", "inline", "restrict", "signed", "unsigned", "void",
    "char", "short", "int", "long", "float", "double", "bool", "complex",
    "#define", "#include", "#if", "#ifdef", "#ifndef", "#endif", "#elif", "#else",
    NULL
};

// Python syntax highlighting
char *Python_HL_extensions[] = {".py", NULL};
char *Python_HL_keywords[] = {
    "and", "as", "assert", "break", "class", "continue", "def", "del", "elif",
    "else", "except", "exec", "finally", "for", "from", "global", "if", "import",
    "in", "is", "lambda", "not", "or", "pass", "print", "raise", "return", "try",
    "while", "with", "yield", "None", "True", "False",
    "self", "__init__", "__str__", "__repr__", "__len__", "__getitem__", "__setitem__",
    NULL
};

// JavaScript syntax highlighting
char *JS_HL_extensions[] = {".js", ".jsx", ".ts", ".tsx", NULL};
char *JS_HL_keywords[] = {
    "break", "case", "catch", "class", "const", "continue", "debugger", "default",
    "delete", "do", "else", "export", "extends", "finally", "for", "function",
    "if", "import", "in", "instanceof", "let", "new", "return", "super", "switch",
    "this", "throw", "try", "typeof", "var", "void", "while", "with", "yield",
    "true", "false", "null", "undefined", "NaN", "Infinity",
    "console", "window", "document", "Array", "Object", "String", "Number",
    "interface", "type", "enum", "implements", "private", "public", "protected",
    "static", "readonly", "abstract", "async", "await", "Promise", "Map", "Set",
    NULL
};

// Java syntax highlighting
char *Java_HL_extensions[] = {".java", NULL};
char *Java_HL_keywords[] = {
    "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char",
    "class", "const", "continue", "default", "do", "double", "else", "enum",
    "extends", "final", "finally", "float", "for", "goto", "if", "implements",
    "import", "instanceof", "int", "interface", "long", "native", "new", "package",
    "private", "protected", "public", "return", "short", "static", "strictfp",
    "super", "switch", "synchronized", "this", "throw", "throws", "transient",
    "try", "void", "volatile", "while", "true", "false", "null",
    "String", "Integer", "Double", "Boolean", "ArrayList", "HashMap", "List", "Map",
    NULL
};

// Go syntax highlighting
char *Go_HL_extensions[] = {".go", NULL};
char *Go_HL_keywords[] = {
    "break", "case", "chan", "const", "continue", "default", "defer", "else",
    "fallthrough", "for", "func", "go", "goto", "if", "import", "interface",
    "map", "package", "range", "return", "select", "struct", "switch", "type",
    "var", "true", "false", "nil", "iota", "make", "new", "len", "cap", "copy",
    "append", "panic", "recover", "print", "println", "fmt", "os", "io", "bufio",
    NULL
};

// Rust syntax highlighting
char *Rust_HL_extensions[] = {".rs", NULL};
char *Rust_HL_keywords[] = {
    "as", "break", "const", "continue", "crate", "else", "enum", "extern",
    "false", "fn", "for", "if", "impl", "in", "let", "loop", "match", "mod",
    "move", "mut", "pub", "ref", "return", "self", "Self", "static", "struct",
    "super", "trait", "true", "type", "unsafe", "use", "where", "while",
    "async", "await", "dyn", "abstract", "become", "box", "do", "final",
    "macro", "override", "priv", "typeof", "unsized", "virtual", "yield",
    "try", "union", "println!", "print!", "vec!", "format!", "panic!",
    NULL
};

// HTML syntax highlighting
char *HTML_HL_extensions[] = {".html", ".htm", NULL};
char *HTML_HL_keywords[] = {
    "html", "head", "body", "div", "span", "p", "a", "img", "table", "tr", "td",
    "th", "ul", "ol", "li", "form", "input", "button", "select", "option",
    "textarea", "label", "script", "style", "link", "meta", "title", "h1", "h2",
    "h3", "h4", "h5", "h6", "br", "hr", "strong", "em", "b", "i", "u", "s",
    "blockquote", "code", "pre", "section", "article", "header", "footer",
    "nav", "aside", "main", "figure", "figcaption", "audio", "video", "canvas",
    NULL
};

// CSS syntax highlighting
char *CSS_HL_extensions[] = {".css", NULL};
char *CSS_HL_keywords[] = {
    "color", "background", "margin", "padding", "border", "width", "height",
    "font-size", "font-family", "text-align", "display", "position", "float",
    "clear", "overflow", "z-index", "opacity", "visibility", "cursor", "transition",
    "animation", "transform", "box-shadow", "text-shadow", "linear-gradient",
    "radial-gradient", "flex", "grid", "media", "hover", "active", "focus",
    "nth-child", "before", "after", "important", "none", "block", "inline",
    "inline-block", "flexbox", "absolute", "relative", "fixed", "static",
    NULL
};

// Syntax highlighting database
struct editorSyntax HLDB[] = {
    {
        "c",
        C_HL_extensions,
        C_HL_keywords,
        "//",
        "/*",
        "*/",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    },
    {
        "python",
        Python_HL_extensions,
        Python_HL_keywords,
        "#",
        NULL,
        NULL,
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    },
    {
        "javascript",
        JS_HL_extensions,
        JS_HL_keywords,
        "//",
        "/*",
        "*/",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    },
    {
        "java",
        Java_HL_extensions,
        Java_HL_keywords,
        "//",
        "/*",
        "*/",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    },
    {
        "go",
        Go_HL_extensions,
        Go_HL_keywords,
        "//",
        "/*",
        "*/",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    },
    {
        "rust",
        Rust_HL_extensions,
        Rust_HL_keywords,
        "//",
        "/*",
        "*/",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    },
    {
        "html",
        HTML_HL_extensions,
        HTML_HL_keywords,
        NULL,
        "<!--",
        "-->",
        HL_HIGHLIGHT_STRINGS
    },
    {
        "css",
        CSS_HL_extensions,
        CSS_HL_keywords,
        NULL,
        "/*",
        "*/",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    },
    { NULL, NULL, NULL, NULL, NULL, NULL, 0 }
};

// Check if character is separator
int is_separator(int c) {
    return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c) != NULL;
}

// Convert syntax highlight type to color
int editorSyntaxToColor(int hl) {
    switch (hl) {
        case HL_COMMENT:
        case HL_MLCOMMENT: return COLOR_COMMENT;
        case HL_KEYWORD1:
        case HL_KEYWORD2: return COLOR_KEYWORD;
        case HL_STRING: return COLOR_STRING;
        case HL_NUMBER: return COLOR_NUMBER;
        case HL_MATCH: return COLOR_SEARCH_MATCH;
        case HL_PREPROCESSOR: return COLOR_PREPROCESSOR;
        default: return COLOR_DEFAULT;
    }
}

// Select syntax highlighting based on file extension
void editorSelectSyntaxHighlight() {
    E.syntax_name = NULL;
    if (E.filename == NULL) return;
    
    char *ext = strrchr(E.filename, '.');
    if (ext == NULL) return;
    
    for (int j = 0; HLDB[j].filetype != NULL; j++) {
        struct editorSyntax *s = &HLDB[j];
        int i = 0;
        while (s->filematch[i] != NULL) {
            int is_ext = (s->filematch[i][0] == '.');
            if ((is_ext && strcmp(ext, s->filematch[i]) == 0) ||
                (!is_ext && strstr(E.filename, s->filematch[i]))) {
                E.syntax_name = s->filetype;
                return;
            }
            i++;
        }
    }
}

// Update syntax highlighting for a row
void editorUpdateSyntax(erow *row) {
    row->hl = realloc(row->hl, row->rsize);
    memset(row->hl, HL_NORMAL, row->rsize);
    
    if (E.syntax_name == NULL) return;
    
    // Find the syntax definition
    struct editorSyntax *syntax = NULL;
    for (int j = 0; HLDB[j].filetype != NULL; j++) {
        if (strcmp(HLDB[j].filetype, E.syntax_name) == 0) {
            syntax = &HLDB[j];
            break;
        }
    }
    
    if (syntax == NULL) return;
    
    char **keywords = syntax->keywords;
    
    char *scs = syntax->singleline_comment_start;
    char *mcs = syntax->multiline_comment_start;
    char *mce = syntax->multiline_comment_end;
    
    int scs_len = scs ? strlen(scs) : 0;
    int mcs_len = mcs ? strlen(mcs) : 0;
    int mce_len = mce ? strlen(mce) : 0;
    
    int prev_sep = 1;
    int in_string = 0;
    int in_comment = (row->idx > 0 && E.row[row->idx - 1].hl_open_comment);
    
    int i = 0;
    while (i < row->rsize) {
        char c = row->render[i];
        unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : HL_NORMAL;
        
        if (scs_len && !in_string && !in_comment) {
            if (!strncmp(&row->render[i], scs, scs_len)) {
                memset(&row->hl[i], HL_COMMENT, row->rsize - i);
                break;
            }
        }
        
        if (mcs_len && mce_len && !in_string) {
            if (in_comment) {
                row->hl[i] = HL_MLCOMMENT;
                if (!strncmp(&row->render[i], mce, mce_len)) {
                    memset(&row->hl[i], HL_MLCOMMENT, mce_len);
                    i += mce_len;
                    in_comment = 0;
                    prev_sep = 1;
                    continue;
                } else {
                    i++;
                    continue;
                }
            } else if (!strncmp(&row->render[i], mcs, mcs_len)) {
                memset(&row->hl[i], HL_MLCOMMENT, mcs_len);
                i += mcs_len;
                in_comment = 1;
                continue;
            }
        }
        
        if (syntax->flags & HL_HIGHLIGHT_STRINGS) {
            if (in_string) {
                row->hl[i] = HL_STRING;
                if (c == '\\' && i + 1 < row->rsize) {
                    row->hl[i + 1] = HL_STRING;
                    i += 2;
                    continue;
                }
                if (c == in_string) in_string = 0;
                i++;
                prev_sep = 1;
                continue;
            } else {
                if (c == '"' || c == '\'') {
                    in_string = c;
                    row->hl[i] = HL_STRING;
                    i++;
                    continue;
                }
            }
        }
        
        if (syntax->flags & HL_HIGHLIGHT_NUMBERS) {
            if ((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) ||
                (c == '.' && prev_hl == HL_NUMBER)) {
                row->hl[i] = HL_NUMBER;
                i++;
                prev_sep = 0;
                continue;
            }
        }
        
        if (prev_sep) {
            int j;
            for (j = 0; keywords[j] != NULL; j++) {
                int klen = strlen(keywords[j]);
                int kw2 = keywords[j][0] == '#';
                if (!kw2 && !strncmp(&row->render[i], keywords[j], klen) &&
                    is_separator(row->render[i + klen])) {
                    memset(&row->hl[i], kw2 ? HL_KEYWORD2 : HL_KEYWORD1, klen);
                    i += klen;
                    break;
                }
            }
            if (keywords[j] != NULL) {
                prev_sep = 0;
                continue;
            }
        }
        
        prev_sep = is_separator(c);
        i++;
    }
    
    int changed = (row->hl_open_comment != in_comment);
    row->hl_open_comment = in_comment;
    if (changed && row->idx + 1 < E.numrows)
        editorUpdateSyntax(&E.row[row->idx + 1]);
}
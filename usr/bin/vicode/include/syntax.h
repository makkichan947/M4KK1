#ifndef SYNTAX_H
#define SYNTAX_H

// Syntax highlighting definitions
extern char *C_HL_extensions[];
extern char *C_HL_keywords[];

extern char *Python_HL_extensions[];
extern char *Python_HL_keywords[];

extern char *JS_HL_extensions[];
extern char *JS_HL_keywords[];

extern struct editorSyntax HLDB[];

// Function declarations
int is_separator(int c);

#endif
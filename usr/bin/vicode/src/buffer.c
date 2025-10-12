#include "../include/editor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Create a new buffer
Buffer *bufferCreate() {
    Buffer *buf = malloc(sizeof(Buffer));
    if (!buf) return NULL;

    buf->cx = 0;
    buf->cy = 0;
    buf->rx = 0;
    buf->rowoff = 0;
    buf->coloff = 0;
    buf->numrows = 0;
    buf->row = NULL;
    buf->dirty = 0;
    buf->filename = NULL;
    buf->syntax_name = NULL;
    buf->next = NULL;
    buf->prev = NULL;

    return buf;
}

// Destroy a buffer
void bufferDestroy(Buffer *buf) {
    if (!buf) return;

    // Free filename
    if (buf->filename) free(buf->filename);

    // Free rows
    for (int i = 0; i < buf->numrows; i++) {
        free(buf->row[i].chars);
        free(buf->row[i].render);
        free(buf->row[i].hl);
    }
    free(buf->row);

    free(buf);
}

// Open a file into a new buffer
Buffer *bufferOpenFile(const char *filename) {
    Buffer *buf = bufferCreate();
    if (!buf) return NULL;

    buf->filename = malloc(strlen(filename) + 1);
    if (!buf->filename) {
        bufferDestroy(buf);
        return NULL;
    }
    strcpy(buf->filename, filename);

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        // File doesn't exist, create empty buffer
        editorSetStatusMessage("New file: %s", filename);
        return buf;
    }

    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        size_t linelen = strlen(line);
        while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
            linelen--;
        editorInsertRow(buf->numrows, line, linelen);
    }
    fclose(fp);

    buf->dirty = 0;
    editorSelectSyntaxHighlight();
    editorSetStatusMessage("Opened file: %s (%d lines)", filename, buf->numrows);

    return buf;
}

// Save buffer to file
void bufferSave(Buffer *buf) {
    if (!buf || !buf->filename) return;

    FILE *fp = fopen(buf->filename, "w");
    if (!fp) {
        editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
        return;
    }

    int len;
    char *buf_content = editorRowsToString(&len);
    fwrite(buf_content, sizeof(char), len, fp);
    fclose(fp);
    free(buf_content);

    buf->dirty = 0;
    editorSetStatusMessage("%d bytes written to disk", len);
}

// Switch to a different buffer
void bufferSwitch(Buffer *buf) {
    if (!buf || buf == E.current_buffer) return;

    E.current_buffer = buf;
    editorSelectSyntaxHighlight();
    editorSetStatusMessage("Switched to buffer: %s", buf->filename ? buf->filename : "[No Name]");
}

// Switch to next buffer
void bufferNext() {
    if (!E.current_buffer || !E.current_buffer->next) return;
    bufferSwitch(E.current_buffer->next);
}

// Switch to previous buffer
void bufferPrev() {
    if (!E.current_buffer || !E.current_buffer->prev) return;
    bufferSwitch(E.current_buffer->prev);
}

// Close a buffer
void bufferClose(Buffer *buf) {
    if (!buf) return;

    // If this is the only buffer, don't close it
    if (E.buffer_count <= 1) {
        editorSetStatusMessage("Can't close last buffer");
        return;
    }

    // If buffer is dirty, ask for confirmation
    if (buf->dirty) {
        editorSetStatusMessage("Buffer has unsaved changes. Use :q! to force close");
        return;
    }

    // Remove from list
    if (buf->prev) buf->prev->next = buf->next;
    if (buf->next) buf->next->prev = buf->prev;

    // If this was the current buffer, switch to another
    if (E.current_buffer == buf) {
        if (buf->next) {
            bufferSwitch(buf->next);
        } else if (buf->prev) {
            bufferSwitch(buf->prev);
        }
    }

    E.buffer_count--;
    bufferDestroy(buf);
    editorSetStatusMessage("Buffer closed");
}

// List all buffers
void bufferList() {
    Buffer *buf = E.buffer_list;
    char message[256] = "Buffers: ";
    int len = strlen(message);

    while (buf) {
        char buf_info[64];
        int info_len = snprintf(buf_info, sizeof(buf_info), "%s%s%s ",
                               buf == E.current_buffer ? "[" : "",
                               buf->filename ? buf->filename : "[No Name]",
                               buf == E.current_buffer ? "]" : "");

        if (len + info_len < sizeof(message)) {
            strcat(message, buf_info);
            len += info_len;
        }
        buf = buf->next;
    }

    editorSetStatusMessage("%s", message);
}

// Helper function to insert row into buffer (adapted from editorInsertRow)
void editorInsertRow(int at, char *s, size_t len) {
    if (at < 0 || at > E.current_buffer->numrows) return;

    E.current_buffer->row = realloc(E.current_buffer->row, sizeof(erow) * (E.current_buffer->numrows + 1));
    memmove(&E.current_buffer->row[at + 1], &E.current_buffer->row[at], sizeof(erow) * (E.current_buffer->numrows - at));

    for (int j = at + 1; j <= E.current_buffer->numrows; j++) E.current_buffer->row[j].idx++;

    E.current_buffer->row[at].idx = at;

    E.current_buffer->row[at].size = len;
    E.current_buffer->row[at].chars = malloc(len + 1);
    memcpy(E.current_buffer->row[at].chars, s, len);
    E.current_buffer->row[at].chars[len] = '\0';

    E.current_buffer->row[at].rsize = 0;
    E.current_buffer->row[at].render = NULL;
    E.current_buffer->row[at].hl = NULL;
    E.current_buffer->row[at].hl_open_comment = 0;

    editorUpdateRow(&E.current_buffer->row[at]);

    E.current_buffer->numrows++;
    E.current_buffer->dirty = 1;
}
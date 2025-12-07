/**
 * M4KK1 echo - Display a line of text
 * 显示一行文本
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define BUFFER_SIZE 4096

/* 选项标志 */
static int no_newline = 0;
static int interpret_escapes = 0;

/* 显示帮助信息 */
void show_help(void) {
    printf("M4KK1 echo - Display a line of text\n");
    printf("用法: echo [选项] [字符串...]\n");
    printf("\n");
    printf("选项:\n");
    printf("  -n              不输出末尾的换行符\n");
    printf("  -e              解释反斜杠转义序列\n");
    printf("  -E              不解释反斜杠转义序列（默认）\n");
    printf("  --help          显示此帮助信息\n");
    printf("\n");
    printf("如果使用了-e选项，支持以下转义序列：\n");
    printf("  \\\\      反斜杠\n");
    printf("  \\a      警报（BEL）\n");
    printf("  \\b      退格\n");
    printf("  \\c      抑制末尾换行符\n");
    printf("  \\e      转义字符\n");
    printf("  \\f      换页\n");
    printf("  \\n      换行\n");
    printf("  \\r      回车\n");
    printf("  \\t      水平制表符\n");
    printf("  \\v      垂直制表符\n");
    printf("  \\0NNN   八进制数NNN的字节（1到3位数字）\n");
    printf("  \\xHH    十六进制数HH的字节（1到2位数字）\n");
}

/* 处理转义序列 */
void process_escape(const char* str, int* i, char* output, int* out_pos) {
    (*i)++; /* 跳过反斜杠 */

    if (*i >= strlen(str)) {
        output[(*out_pos)++] = '\\';
        return;
    }

    char c = str[*i];
    switch (c) {
        case 'a':
            output[(*out_pos)++] = '\a';
            break;
        case 'b':
            output[(*out_pos)++] = '\b';
            break;
        case 'c':
            no_newline = 1;
            break;
        case 'e':
            output[(*out_pos)++] = '\033';
            break;
        case 'f':
            output[(*out_pos)++] = '\f';
            break;
        case 'n':
            output[(*out_pos)++] = '\n';
            break;
        case 'r':
            output[(*out_pos)++] = '\r';
            break;
        case 't':
            output[(*out_pos)++] = '\t';
            break;
        case 'v':
            output[(*out_pos)++] = '\v';
            break;
        case '\\':
            output[(*out_pos)++] = '\\';
            break;
        case '0': { /* 八进制 */
            int octal = 0;
            int digits = 0;
            while (*i + 1 < strlen(str) && digits < 3) {
                char next = str[*i + 1];
                if (next >= '0' && next <= '7') {
                    octal = octal * 8 + (next - '0');
                    (*i)++;
                    digits++;
                } else {
                    break;
                }
            }
            output[(*out_pos)++] = (char)octal;
            break;
        }
        case 'x': { /* 十六进制 */
            int hex = 0;
            int digits = 0;
            while (*i + 1 < strlen(str) && digits < 2) {
                char next = str[*i + 1];
                if ((next >= '0' && next <= '9') ||
                    (next >= 'a' && next <= 'f') ||
                    (next >= 'A' && next <= 'F')) {
                    int val = (next >= '0' && next <= '9') ? next - '0' :
                             (next >= 'a' && next <= 'f') ? next - 'a' + 10 :
                             next - 'A' + 10;
                    hex = hex * 16 + val;
                    (*i)++;
                    digits++;
                } else {
                    break;
                }
            }
            output[(*out_pos)++] = (char)hex;
            break;
        }
        default:
            /* 未知转义序列，原样输出 */
            output[(*out_pos)++] = '\\';
            output[(*out_pos)++] = c;
            break;
    }
}

/* 处理字符串 */
void process_string(const char* str) {
    static char output[BUFFER_SIZE];
    int out_pos = 0;

    for (int i = 0; i < strlen(str) && out_pos < BUFFER_SIZE - 1; i++) {
        if (interpret_escapes && str[i] == '\\') {
            process_escape(str, &i, output, &out_pos);
        } else {
            output[out_pos++] = str[i];
        }
    }

    output[out_pos] = '\0';

    /* 输出处理后的字符串 */
    fputs(output, stdout);
}

/* 主函数 */
int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "neE", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'n':
                no_newline = 1;
                break;
            case 'e':
                interpret_escapes = 1;
                break;
            case 'E':
                interpret_escapes = 0;
                break;
            case 'h':
                show_help();
                return 0;
            default:
                fprintf(stderr, "无效选项。使用 --help 查看帮助。\n");
                return 1;
        }
    }

    /* 处理剩余参数 */
    for (int i = optind; i < argc; i++) {
        if (i > optind) {
            putchar(' '); /* 参数之间用空格分隔 */
        }
        process_string(argv[i]);
    }

    /* 输出换行符（除非指定-n或使用了\c） */
    if (!no_newline) {
        putchar('\n');
    }

    return 0;
}
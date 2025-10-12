/*
 * M4KK1内核版本信息生成器
 * 生成内核版本头文件
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 0

int main(int argc, char *argv[])
{
    FILE *fp;
    time_t now;
    struct tm *tm_info;
    char timestamp[64];

    /* 获取当前时间戳 */
    time(&now);
    tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    /* 生成版本头文件 */
    fp = fopen("include/version.h", "w");
    if (!fp) {
        perror("无法创建版本头文件");
        return 1;
    }

    fprintf(fp, "/*\n");
    fprintf(fp, " * M4KK1内核版本信息\n");
    fprintf(fp, " * 此文件由mkversion自动生成，请勿手动修改\n");
    fprintf(fp, " */\n\n");

    fprintf(fp, "#ifndef __KERNEL_VERSION_H__\n");
    fprintf(fp, "#define __KERNEL_VERSION_H__\n\n");

    fprintf(fp, "/* 版本号 */\n");
    fprintf(fp, "#define KERNEL_VERSION_MAJOR %d\n", VERSION_MAJOR);
    fprintf(fp, "#define KERNEL_VERSION_MINOR %d\n", VERSION_MINOR);
    fprintf(fp, "#define KERNEL_VERSION_PATCH %d\n", VERSION_PATCH);
    fprintf(fp, "#define KERNEL_VERSION_STRING \"%d.%d.%d\"\n",
            VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    fprintf(fp, "\n/* 内核信息 */\n");
    fprintf(fp, "#define KERNEL_NAME \"M4KK1\"\n");
    fprintf(fp, "#define KERNEL_CODENAME \"M4KK1\"\n");
    fprintf(fp, "#define KERNEL_AUTHOR \"makkichan947\"\n");

    fprintf(fp, "\n/* 构建信息 */\n");
    fprintf(fp, "#define KERNEL_BUILD_DATE \"%s\"\n", timestamp);
    fprintf(fp, "#define KERNEL_BUILD_MACHINE \"%s\"\n", getenv("USER") ?: "unknown");

    fprintf(fp, "\n/* 版本字符串 */\n");
    fprintf(fp, "#define KERNEL_FULL_VERSION KERNEL_NAME \" \" KERNEL_VERSION_STRING\n");

    fprintf(fp, "\n#endif /* __KERNEL_VERSION_H__ */\n");

    fclose(fp);

    printf("版本信息已生成到 include/version.h\n");
    return 0;
}
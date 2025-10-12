/*
 * Copland窗口服务器测试程序
 * 测试客户端-服务器通信和基本功能
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "copland.h"

/* 测试客户端 */
static bool test_client(void)
{
    printf("创建客户端...\n");

    /* 创建客户端 */
    copland_client_t *client = copland_client_create();
    if (!client) {
        printf("创建客户端失败\n");
        return false;
    }

    printf("连接到服务器...\n");

    /* 连接到服务器 */
    if (!copland_client_connect(client, "/tmp/copland.sock")) {
        printf("连接到服务器失败\n");
        copland_client_destroy(client);
        return false;
    }

    printf("创建窗口...\n");

    /* 创建窗口 */
    uint32_t window_id = copland_client_create_window(client, 0,
                                                     COPLAND_WINDOW_TOPLEVEL,
                                                     100, 100, 800, 600,
                                                     "Copland测试窗口");
    if (window_id == 0) {
        printf("创建窗口失败\n");
        copland_client_disconnect(client);
        copland_client_destroy(client);
        return false;
    }

    printf("窗口创建成功，ID: %u\n", window_id);

    /* 等待一段时间 */
    sleep(2);

    printf("销毁窗口...\n");

    /* 销毁窗口 */
    if (!copland_client_destroy_window(client, window_id)) {
        printf("销毁窗口失败\n");
    }

    printf("断开连接...\n");

    /* 断开连接 */
    copland_client_disconnect(client);
    copland_client_destroy(client);

    printf("客户端测试完成\n");
    return true;
}

/* 测试服务器 */
static bool test_server(void)
{
    printf("创建服务器...\n");

    /* 创建服务器 */
    copland_server_t *server = copland_server_create();
    if (!server) {
        printf("创建服务器失败\n");
        return false;
    }

    printf("初始化服务器...\n");

    /* 初始化服务器 */
    if (!copland_server_init(server)) {
        printf("初始化服务器失败\n");
        copland_server_destroy(server);
        return false;
    }

    printf("服务器初始化完成，Socket: %s\n", server->socket_path);

    printf("启动服务器...\n");

    /* 启动服务器（非阻塞模式，用于测试） */
    if (!copland_server_run(server)) {
        printf("启动服务器失败\n");
        copland_server_destroy(server);
        return false;
    }

    /* 等待客户端连接和测试 */
    sleep(5);

    printf("停止服务器...\n");

    /* 停止服务器 */
    copland_server_quit(server);
    copland_server_destroy(server);

    printf("服务器测试完成\n");
    return true;
}

/* 主函数 */
int main(int argc, char *argv[])
{
    pid_t pid;
    int status;

    printf("Copland窗口服务器测试开始\n");

    /* 创建子进程运行服务器 */
    pid = fork();
    if (pid < 0) {
        perror("fork失败");
        return 1;
    }

    if (pid == 0) {
        /* 子进程运行服务器 */
        return test_server() ? 0 : 1;
    } else {
        /* 父进程等待服务器启动 */
        sleep(1);

        /* 运行客户端测试 */
        bool client_ok = test_client();

        /* 等待服务器退出 */
        waitpid(pid, &status, 0);

        printf("Copland窗口服务器测试结束\n");
        return client_ok && WEXITSTATUS(status) == 0 ? 0 : 1;
    }
}
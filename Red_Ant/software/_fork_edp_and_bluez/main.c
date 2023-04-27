#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2, pid3;
    int status1, status2;

    // 创建第一个子进程
    pid1 = fork();
    if (pid1 == 0) {
        // 在子进程中运行第一个可执行文件
        execl("/root/edp", "edp", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    else if (pid1 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // 创建新进程
    pid3 = fork();
    if (pid3 == 0) {
        // 创建第二个子进程
        pid2 = fork();
        if (pid2 == 0) {
            // 在子进程中运行第二个可执行文件
            execl("/root/bt_app", "bt_app", NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        }
        else if (pid2 < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        // 等待第二个子进程结束
        waitpid(pid2, &status2, 0);
        exit(EXIT_SUCCESS);
    }
    else if (pid3 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // 等待第一个子进程和新进程结束
    waitpid(pid1, &status1, 0);
    waitpid(pid3, NULL, 0);

    return 0;
}


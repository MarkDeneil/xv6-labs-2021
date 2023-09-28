#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char * argv[]) {
    int p[2];
    int p1[2];
    pipe(p);
    pipe(p1);
    if (fork() == 0) {
        // 子进程
        close(p[1]);
        close(p1[0]);
        char buf[100];
        read(p[0], buf, 99);
        // printf("%d:receive ping\n", getpid());
        fprintf(2, "%d: received ping\n", getpid());
        write(p1[1], buf, 99);
        close(p[0]);
        close(p1[1]);
        exit(0);
    } else {
        // 父进程
        close(p[0]);
        close(p1[1]);
        write(p[1],"abc", 99);
        char buf[100];
        // wait(NULL);
        read(p1[0], buf, 99);
        // printf("%d:receive pong\n", getpid());
        fprintf(2, "%d: received pong\n", getpid());
        close(p[1]);
        close(p1[0]);
        exit(0);
    }
    return 0;
}



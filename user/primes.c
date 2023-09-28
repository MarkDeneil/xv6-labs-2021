#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sieve(int p[2]) {
    int firstnum;
    read(p[0], &firstnum, sizeof(firstnum)); // 读取第一个数，该数是一个素数
    if (firstnum == -1) {
        exit(0);
    }
    printf("prime %d\n", firstnum);
    int p1[2];
    pipe(p1);
    if (fork() == 0) {
        // 本stage的子进程
        close(p1[1]);
        sieve(p1);
        exit(0);
    } else {
        close(p1[0]);
        int othernum;
        while (read(p[0], &othernum, sizeof(othernum)) > 0) {
            // =0 时说明对端管道 p[1] 已经关闭，说明 p1 已经发送过来了最后一个数 -1
            if (othernum == -1) {
                int tmp = -1;
                write(p1[1], &tmp, sizeof(tmp));
                wait(0); // 等待子进程结束
                exit(0);
            }
            if (othernum % firstnum != 0) {
                write(p1[1], &othernum, sizeof(othernum));
            }
        }
    }

}

int main(int argc, char * argv[]) {
    int p[2];
    pipe(p);
    if (fork() == 0) {
        // 第一个阶段的子进程
        close(p[1]); // 子进程只需要从管道读入数据，再创建一个新的管道像自己的子进程写入数据
        sieve(p);
        exit(0);

        
    } else {
        // 父进程：当前阶段
        close(p[0]); // 父进程只需要写入管道内容，不需要从管道读取
        int i;
        for (i = 2; i <= 35; ++i) {
            write(p[1], &i, sizeof(i));
        }
        i = -1;
        write(p[1], &i, sizeof(i)); // 末尾输入 -1， 用于标识输入完成
        wait(0); // 等待子一个 stage 完成；注意 fork 只能等待直接子进程，不能等待间接子进程
    }
    exit(0);

}
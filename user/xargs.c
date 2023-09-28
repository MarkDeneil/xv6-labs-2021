#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

/*
本实验的 hint 2 
To read individual lines if input, read  character at a tmie until a new line ('\n') apperas.
即当遇到 echo "1\n2" | xargs echo line; 时（其中 '\n' 是换行符）
应当输出
line 1
line 2
即遇到换行符时直接用已经收集的参数执行命令，然后重新收集参数
*/

void run(char * program, char ** args) {
    if (fork() == 0) {
        exec(program, args);
        exit(0);
    }
    return;
}

int main(int argc, char * argv[]) {
    char buf[2048]; 
    char * p = buf, * last_p = buf;
    char * argsbuf[128]; // 全部参数列表，数组中每个元素是一个 char * 指针
    char ** args = argsbuf;
    for (int i = 1; i < argc; ++i) {
        // 将 argv 提供的参数加入到最终的参数列表中
        *args = argv[i];
        ++args;
    }

    // for (int i = 0; i < argc; ++i) 
    //     printf("%s\n", argsbuf[i]);

    char ** pa = args; // 开始读入参数
    // 以管道形式传进来的参数，是作为标准输入进来的
    // echo a | xargs echo b; 中 echo a 命令的标准输出作为 xargs echo b; 的标准输入
    // 因此接下来要从输入中读取参数
    while (read(0, p, 1) != 0) {
        if (*p == ' ' || *p == '\n') {
            *p = '\0';
            *(pa++) = last_p;
            last_p = p + 1;
            if (*p == '\n') {
                *pa = 0;
                // printf("onelinebegin\n");
                run(argv[1], argsbuf); // argv[0] 是 "xargs"
                // printf("onelineend\n");
                pa = args;
            }
        }
        p++;
    }

    // printf("argsbuf\n");
    // for (int i = 0; i < 7; ++i) {
    //     printf("%s\n", argsbuf[i]);
    // }
    // printf("args\n");
    // for (int i = 0; i < 5; ++i)
    //     printf("%s\n", args[i]);

    if (pa != args) {
        // 判断最后一行是否为空行
        *p = '\0';
        *(pa++) = last_p;
        *pa = 0;
        // printf("lastlinebegin\n");
        run(argv[1], argsbuf);
        // printf("lastlineend\n");
    }
    while(wait(0) != -1) {};
    exit(0);
}

// echo hello too | xargs echo bye
// echo bye 作为命令行参数传递给 xargs；hello too 作为标准化输入传给 xargs

// #define MSGSIZE 256
// int main (int argc, char * argv[]) {
//     sleep(10); // 需要先等待一段时间，等待 比如 find 命令的输出
//     // 获取前一个命令的标准输出作为本命令的标准输入
//     char buf[MSGSIZE];
//     read(0, buf, MSGSIZE);
//     // printf("%s\n", buf);

//     // 获取命令行参数
//     char * xargv[MAXARG];
//     int xargc = 0;
//     for (int i = 1; i < argc; ++i) {
//         xargv[xargc] = argv[i];
//         xargc++;
//     }
//     char * p = buf;
//     for (int i = 0; i < MSGSIZE; ++i) {
//         if (buf[i] == '\n') {
//             // 要用当前收集的参数执行命令
//             int pid = fork();
//             if (pid > 0) {
//                 p = &buf[i+1];
//                 wait(0);
//             } else {
//                 buf[i] = 0;
//                 xargv[xargc] = p;
//                 xargc++;
//                 xargv[xargc] = 0; // xargv 中最后一个命令后为 NULL
//                 xargc++;
//                 exec(xargv[0], xargv);
//                 exit(0);
//             }
//         }
//     }

//     wait(0);
//     exit(0);
// }
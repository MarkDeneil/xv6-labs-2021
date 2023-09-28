#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define DIR_LEN 512

void recursionfind(char *buf, char *filename)
{
    char *p = buf + strlen(buf);
    int fd;
    struct stat st;
    struct dirent de;

    if ((fd = open(buf, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", buf);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", buf);
        close(fd);
        return;
    }

    if (strlen(buf) + 1 + DIRSIZ + 1 > DIR_LEN)
    {
        printf("find: dir name too long\n");
        return;
    }

    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        // 每当读取到目录中的一个文件

        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;

        if (de.inum == 0)
            continue;
        int fd1;
        struct stat st1;

        strcpy(p, de.name);
        p[strlen(de.name)] = 0;

        if ((fd1 = open(buf, 0)) < 0)
        {
            fprintf(2, "find: cannot open %s\n", buf);
            return;
        }
        if (fstat(fd1, &st1) < 0)
        {
            fprintf(2, "find: cannot stat %s\n", buf);
            close(fd1);
            return;
        }
        switch (st1.type)
        {
        case T_FILE:
            if (strcmp(filename, de.name) == 0)
            {
                printf("%s\n", buf);
            }
            break;
        case T_DIR:
            p += strlen(de.name);
            *p++ = '/';
            *p = 0;
            recursionfind(buf, filename);
            p -= strlen(de.name);
            *p = 0;
            --p;
            break;
        }
        close(fd1); // 注意，一定要关闭，否则到后面会出现文件描述符不够用导致 open 函数无法打开新文件
    }
}

void find(char *dir, char *filename)
{
    // 在 dir 中查找 filename
    int fd;
    struct stat st;
    char buf[DIR_LEN], *p; // buf 用于存放目录的绝对路径，在递归函数中逐渐增长
    strcpy(buf, dir);
    p = buf + strlen(buf);
    if ((*(p-1) != '/'))
        *p++ = '/';

    if ((fd = open(buf, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", buf);
        return;
    }

    if (fstat(fd, &st) < 0)
    {   
        fprintf(2, "find: cannot stat %s\n", buf);
        close(fd);
        return;
    }
    close(fd);
    switch (st.type)
    {
    case T_FILE:
        printf("The first arg should be a directory.\n");
        break;
    case T_DIR:
        recursionfind(buf, filename);
        break;
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    // find 查找路径 查找文件
    // 如果只给出了一个文件，则默认在当前文件查找
    if (argc < 2)
    {
        printf("find [path] [file] or find [file]\n");
        exit(0);
    }
    if (argc < 3)
    {
        find(".", argv[1]);
        exit(0);
    }
    find(argv[1], argv[2]);
    exit(0);
}

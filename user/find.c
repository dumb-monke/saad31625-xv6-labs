#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char *path, char *target, int exec_mode, char *cmd, char **cmdargs)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type == T_FILE) {
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        close(fd);
        return;
    }

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        fprintf(2, "find: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue;

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        if (strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
            continue;

        if (strcmp(p, target) == 0) {
            if (exec_mode) {
                int pid = fork();

                if (pid == 0) {
                    char *args[MAXARG];
                    int i = 0;

                    while (cmdargs[i] != 0) {
                        args[i] = cmdargs[i];
                        i++;
                    }

                    args[i] = buf;
                    args[i + 1] = 0;

                    exec(cmd, args);

                    fprintf(2, "find: exec %s failed\n", cmd);
                    exit(1);
                }

                if (pid > 0)
                    wait(0);
            } else {
                printf("%s\n", buf);
            }
        }

        if (stat(buf, &st) < 0)
            continue;

        if (st.type == T_DIR)
            find(buf, target, exec_mode, cmd, cmdargs);
    }

    close(fd);
}

int
main(int argc, char *argv[])
{
    int exec_mode = 0;
    char *cmd = 0;
    char **cmdargs = 0;

    if (argc < 3) {
        fprintf(2, "usage: find path name [-exec cmd [args...]]\n");
        exit(1);
    }

    if (argc >= 4) {
        if (strcmp(argv[3], "-exec") != 0 || argc < 5) {
            fprintf(2, "usage: find path name [-exec cmd [args...]]\n");
            exit(1);
        }

        exec_mode = 1;
        cmd = argv[4];
        cmdargs = &argv[4];
    }

    find(argv[1], argv[2], exec_mode, cmd, cmdargs);

    exit(0);
}

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int
is_sep(char c)
{
    return strchr(" -\r\t\n./,", c) != 0;
}

void
process(int fd)
{
    char buf[1];
    char num[32];
    int n = 0;
    int x;

    while (read(fd, buf, 1) == 1) {
        if (is_sep(buf[0])) {
            if (n > 0) {
                num[n] = 0;
                x = atoi(num);

                if (x % 5 == 0 || x % 6 == 0)
                    printf("%d\n", x);

                n = 0;
            }
        } else if (buf[0] >= '0' && buf[0] <= '9') {
            if (n < sizeof(num) - 1)
                num[n++] = buf[0];
        }
    }

    if (n > 0) {
        num[n] = 0;
        x = atoi(num);

        if (x % 5 == 0 || x % 6 == 0)
            printf("%d\n", x);
    }
}

int
main(int argc, char *argv[])
{
    int fd, i;

    if (argc == 1) {
        process(0);
    } else {
        for (i = 1; i < argc; i++) {
            fd = open(argv[i], O_RDONLY);

            if (fd < 0) {
                fprintf(2, "sixfive: cannot open %s\n", argv[i]);
                continue;
            }

            process(fd);
            close(fd);
        }
    }

    exit(0);
}

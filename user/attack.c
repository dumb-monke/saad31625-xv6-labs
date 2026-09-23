#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define PGSIZE 4096

int
main(int argc, char *argv[])
{
  char *p;
  int page, i, j;

  char msg[] = "Here it is: ";

  for (page = 0; page < 100; page++) {
    p = sbrk(PGSIZE);
    if (p == (char *)-1)
      exit(1);

    for (i = 0; i < PGSIZE - 20; i++) {

      // Look for "Here it is: "
      for (j = 0; msg[j] != '\0'; j++) {
        if (p[i + j] != msg[j])
          break;
      }

      if (msg[j] != '\0')
        continue;

      // Character immediately after the message
      char *s = p + i + 12;

      // Must contain at least one alphanumeric character
      if (!((*s >= 'a' && *s <= 'z') ||
            (*s >= 'A' && *s <= 'Z') ||
            (*s >= '0' && *s <= '9')))
        continue;

      // Print the recovered secret
      printf("%s\n", s);
      exit(0);
    }
  }

  exit(1);
}

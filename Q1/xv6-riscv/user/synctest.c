#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  sem_init(0, 1);
  sem_init(1, 0);
  sem_init(2, 0);

  if (fork() == 0) {
    for (int i = 0; i < 5; i++) {
      sem_wait(0);
      printf("A ");
      sem_post(1);
    }
    exit(0);
  }

  if (fork() == 0) {
    for (int i = 0; i < 5; i++) {
      sem_wait(1);
      printf("B ");
      sem_post(2);
    }
    exit(0);
  }

  if (fork() == 0) {
    for (int i = 0; i < 5; i++) {
      sem_wait(2);
      printf("C ");
      sem_post(0);
    }
    exit(0);
  }

  for (int i = 0; i < 3; i++)
    wait(0);

  printf("\nDone\n");
  exit(0);
}

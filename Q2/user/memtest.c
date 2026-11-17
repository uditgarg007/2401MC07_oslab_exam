#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int global_var = 42;

int
main(int argc, char *argv[])
{
  int n1, n2, pa;
  int pid;

  n1 = freepages();
  printf("freepages at start-up: %d\n", n1);

  n2 = freepages();
  printf("freepages after sbrk(10 * 4096): %d\n", n2);

  pa = va2pa((uint64)&global_var);
  printf("global variable: va = %p, pa = 0x%x\n", &global_var, (uint)pa);

  printf("va2pa(0x7FFFFFFF) = %d\n", va2pa(0x7FFFFFFF));

  int before_fork = freepages();
  printf("freepages before fork: %d\n", before_fork);

  pid = fork();

  if (pid == 0) {
    int child_pa = va2pa((uint64)&global_var);
    printf("child: va = %p, pa = 0x%x\n", &global_var, (uint)child_pa);
    exit(0);
  } else {
    pause(2);
    int parent_pa = va2pa((uint64)&global_var);
    printf("parent: va = %p, pa = 0x%x\n", &global_var, (uint)parent_pa);
    printf("freepages after fork: %d\n", freepages());

    wait(0);
    printf("freepages after wait: %d\n", freepages());
  }

  exit(0);
}

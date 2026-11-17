#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include "proc.h"

#define MAX_SEM 8

struct sem {
  int val;
  int used;
};

struct {
  struct spinlock lock;
  struct sem sems[MAX_SEM];
} semtable;

void
seminit(void)
{
  initlock(&semtable.lock, "semtable");
  for (int i = 0; i < MAX_SEM; i++) {
    semtable.sems[i].val = 0;
    semtable.sems[i].used = 0;
  }
}

int
sem_init(int id, int value)
{
  if (id < 0 || id >= MAX_SEM || value < 0)
    return -1;

  acquire(&semtable.lock);
  semtable.sems[id].val = value;
  semtable.sems[id].used = 1;
  release(&semtable.lock);
  return 0;
}

int
sem_wait(int id)
{
  if (id < 0 || id >= MAX_SEM)
    return -1;

  acquire(&semtable.lock);
  if (!semtable.sems[id].used) {
    release(&semtable.lock);
    return -1;
  }

  while (semtable.sems[id].val == 0) {
    if (killed(myproc())) {
      release(&semtable.lock);
      return -1;
    }
    sleep_prepare(&semtable.sems[id]);
    release(&semtable.lock);
    sleep();
    acquire(&semtable.lock);
  }

  semtable.sems[id].val--;
  release(&semtable.lock);
  return 0;
}

int
sem_post(int id)
{
  if (id < 0 || id >= MAX_SEM)
    return -1;

  acquire(&semtable.lock);
  if (!semtable.sems[id].used) {
    release(&semtable.lock);
    return -1;
  }

  semtable.sems[id].val++;
  wakeup(&semtable.sems[id]);
  release(&semtable.lock);
  return 0;
}

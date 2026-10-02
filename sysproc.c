#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// ==========================================
// Lab DA-3: Race Condition Variables
// ==========================================
int shared_counter = 0;
struct spinlock counter_lock;
int lock_initialized = 0;

void ensure_lock_init(void) {
  if (!lock_initialized) {
    initlock(&counter_lock, "counter_lock");
    lock_initialized = 1;
  }
}

// System call: Increments the counter 100,000 times
// System call: Increments the counter 100,000 times
int sys_race_inc(void) {
  ensure_lock_init();
  
  for (int i = 0; i < 100000; i++) {
    // 1. UNLOCKED VERSION (Demonstrates Race Condition)
    // int temp = shared_counter;     // Read
    
    // // Tiny delay to force CPUs to overlap
    // for(volatile int j = 0; j < 10; j++); 
    
    // shared_counter = temp + 1;     // Write

    // 2. LOCKED VERSION (Fixes Race Condition)
    acquire(&counter_lock);
    shared_counter++;
    release(&counter_lock);
  }
  return 0;
}
// System call: Returns the final value of the shared counter
int sys_race_get(void) {
  return shared_counter;
}
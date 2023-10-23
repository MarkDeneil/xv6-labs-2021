#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

// 供 sys_sysinfo 系统调用使用
#include "sysinfo.h"
extern uint64 count_nproc();
extern uint64 count_freemem();

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
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

uint64
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

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 sys_trace(void) {
  // 通过下面这段代码 使用 argint 函数（在 kernel/syscall.c 中定义）从用户空间拿到 mask 变量（即 trace 系统调用的参数）的值
  int mask;
  if(argint(0, &mask) < 0) // 调用 argint 函数从进程的 trapframe 中获得 mask 参数
    return -1;
  

  // printf("call sys_trace in sysproc.c.\n");
  // printf("args from user space:%d\n", mask);
  myproc()->syscall_trace_mask = mask;
  return 0;
}

uint64 sys_sysinfo(void) {
  uint64 sinfo; // user pointer to struct sysinfo

  if(argaddr(0, &sinfo) < 0)
    return -1;
  
  struct proc * p = myproc();
  struct sysinfo info;
  info.nproc = count_nproc();
  info.freemem = count_freemem();
  if(copyout(p->pagetable, sinfo, (char *)&info, sizeof(info)) < 0)
      return -1;
  return 0;
}
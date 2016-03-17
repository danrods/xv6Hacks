#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void pgflthandler(void);

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
  
  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(proc->killed)
      exit();
    proc->tf = tf;
    syscall();
    if(proc->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER: // Timer Interrupt
    if(cpu->id == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE: // DISK interrupt
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD: // Keyboard Interrupt (Ctrl + C)
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1: //Serial Port Interrupt
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS: //hardware interrupt that is unwanted.
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpu->id, tf->cs, tf->eip);
    lapiceoi();
    break;

  #ifndef original
  case T_PGFLT: //Page fault interrupt

    if(proc ==0 || (tf->cs&3) == 0){
        cprintf("Kernel level error\n");
    }
    if(tf->err & FEC_U){
       cprintf("We're in user space! --> EIP : %x\n", tf->eip);

       uint fault_addr = rcr2();
        cprintf("Found fault_addr in user mode: %p\n", fault_addr);

        if((pte = (pte_t *)walkpagedir(proc->pgdir, (void *) fault_addr, 0)) == 0){
            panic("Error fetching PTE from CR2 Register!\n");
        }

        uint flags = PTE_FLAGS(*pte);

       lapiceoi();
    }
    if(tf->err & FEC_WR){
      pgflthandler();
      lapiceoi();
      //proc->killed = 1;
    }

    
    break;

  #endif

  //PAGEBREAK: 13
  default:
    if(proc == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpu->id, tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            proc->pid, proc->name, tf->trapno, tf->err, cpu->id, tf->eip, 
            rcr2());
    proc->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running 
  // until it gets to the regular system call return.)
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(proc && proc->state == RUNNING && tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit();
}

#ifndef original

void pgflthandler(void){

  pte_t * pte;

  uint fault_addr = rcr2();
  cprintf("Found fault_addr: %p\n", fault_addr);

  if((pte = (pte_t *)walkpagedir(proc->pgdir, (void *) fault_addr, 0)) == 0){
      panic("Error fetching PTE from CR2 Register!\n");
  }

  uint flags = PTE_FLAGS(*pte);
  cprintf("Found flags 0x%p\n", flags);

  if(! (*pte & PTE_U)){
    return;
    panic("ERROR ----> Kernel space page fault!\n");
  }


  if(*pte & PTE_COW){
    cprintf("ERROR ----> COWpage fault!\n");
    int ref_count = getRefCount((void*) fault_addr);
    if (ref_count > 1) {
      int pa = PTE_ADDR(*pte);
      char *mem = kalloc();
      memset(mem, 0, PGSIZE);
      memmove(mem, (char*)p2v(pa), PGSIZE);
      //mappages(proc->pgdir, PGSIZE, v2p(mem), pa, PTE_W|PTE_U);
      flags &= ~PTE_COW;
      *pte = v2p(mem) | flags | PTE_W;
      decRefCount((void*) fault_addr);
    } 
    else if (ref_count == 1) {
      *pte &= ~PTE_COW;
      *pte |= PTE_W;
    }
    invlpg(pte);
  }
  else {
    //PANIC
    proc->killed = 1;
  }
  
  
}


#endif

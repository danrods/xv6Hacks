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

int pgflthandler(void);

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

    if(tf->err & FEC_WR){
      //cprintf("We're in user space! --> EIP : %x\n", tf->eip);
      if(pgflthandler()){
          //cprintf("Not a User address, lets not confirm\n");
      }
      else lapiceoi();

    }
    else{
      cprintf("Read Page Fault? --> 0x%x\n", tf->err);
    }

    return;
    break;

  #endif

  //PAGEBREAK: 13
  default:
    if(proc == 0 || (tf->cs&3) == 0){ //CS stores privilege level : 0 for Kernel, 3 for User
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
  if(proc && proc->killed && (tf->cs&3) == DPL_USER){
      cprintf("DIE Process %d",proc->pid);
      exit();
  }
    

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(proc && proc->state == RUNNING && tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(proc && proc->killed && (tf->cs&3) == DPL_USER){
      cprintf("DIE Process %d",proc->pid);
      exit();
  }
    
}

#ifndef original

int pgflthandler(void){
  //cprintf("-----------------Starting Page Fault Handler---------------------\n");
  pte_t * pte;

  uint fault_addr = rcr2();
  //cprintf("Found fault_addr: 0x%p\n", fault_addr);
  void* page = (void*) PGROUNDDOWN(fault_addr);
  void* vpage = (void*) uva2ka(proc->pgdir, (char*)fault_addr);
  //cprintf("Comparing two addresses : Rounding--> %p ; uva2kva-->%p\n", page, vpage);
  //void* page = (void*) PGROUNDDOWN(fault_addr);

  //cprintf("On Page Boundary : 0x%p\n", page);

  if((pte = (pte_t *)walkpagedir(proc->pgdir, page, 0)) == 0){
      panic("Error fetching PTE from CR2 Register!\n");
  }

  uint pa = PTE_ADDR(*pte);
  uint flags = PTE_FLAGS(*pte);
  //cprintf("Found flags 0x%p\n", flags);

  if(! (*pte & PTE_U)){
    cprintf("ERROR ----> Kernel space page fault!\n");
  }


  if(*pte & PTE_COW){
    //cprintf("ERROR ----> COW page fault for process %d!\n", proc->pid);
    int ref_count = getRefCount(vpage);
    //int ref_count = getRefCount(p2v(pa));
    if (ref_count > 1) {
      cprintf("%d References\n", ref_count);
      
      char *mem = kalloc();
      memset(mem, 0, PGSIZE);
      memmove(mem, (char*)p2v(pa), PGSIZE);
      *pte &= ~PTE_P;
      if(mappages(proc->pgdir, page, PGSIZE, v2p(mem), PTE_W|flags)){
        panic("Error mapping pages");
      }

      decRefCount(vpage);
    } 
    else if (ref_count == 1) {
      cprintf("Only One Reference\n");
      *pte &= ~PTE_P;
      flags &= ~(PTE_COW | PTE_P);
      flags |= PTE_W;
      if(mappages(proc->pgdir, page, PGSIZE, pa, flags)){
          panic("Error mapping pages");
      }
    }

    invlpg((void*)fault_addr);
  }
  else {
    //PANIC
    panic("User page not Copy on Write");
    proc->killed = 1;
  }
  
  //cprintf("-----------------Ending Page Fault Handler---------------------\n");
  
  return 0;
}


#endif

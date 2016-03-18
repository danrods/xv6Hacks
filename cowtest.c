#include "types.h"
#include "defs.h"
#include "mmu.h"
#include "param.h"
#include "proc.h"


void cowpatest(void);

int
main(void)
{
 // cowpatest();
  exit();
  return 0;
}
/*
void 
cowpatest() 
{
  int pid; 
  char* va = "Hey";

  pid = fork();

  //Before writing
  if (pid == 0)
  {
    pte_t *pte = (pte_t*) walkpagedir(proc->pgdir, va, 0);
    uint add = PTE_ADDR(*pte);
    cprintf("Child page pysical address before: %x\n", add);

    //After writing
    va = "Hi";

    pte = (pte_t*) walkpagedir(proc->pgdir, va, 0);
    add = PTE_ADDR(*pte);
    cprintf("Child page pysical address after: %x\n", add);
  }
  else if (pid > 0)
  {
    pte_t *pte = (pte_t*) walkpagedir(proc->pgdir, va, 0);
    uint add = PTE_ADDR(*pte);
    cprintf("Parent page pysical address before: %x\n", add);

    wait();

    pte = (pte_t*) walkpagedir(proc->pgdir, va, 0);
    add = PTE_ADDR(*pte);
    cprintf("Parent page pysical address after: %x\n", add);
    
  }
  else 
  {
    cprintf("Error forking");
  }


  exit();
}
*/

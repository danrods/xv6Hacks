#include "types.h"
#include "defs.h"
#include "mmu.h"
#include "proc.h"
#include "param.h"

void cowpatest(void);

int
main(void)
{
  cowpatest();
  exit();
}

void 
cowpatest() 
{
  int pid; 
  char* va = "Hey";

  pid = fork();

  //Before writing
  if (pid == 0)
  {
    pte_t *pte = walkpagedir(proc->pgdir, va, 0);
    uint add = PTE_ADDR(*pte);
    printf(1, "Child page pysical address before: %x\n", add);

    //After writing
    va = "Hi";

    pte_t *pte = walkpagedir(proc->pgdir, va, 0);
    add = PTE_ADDR(*pte);
    printf(1, "Child page pysical address after: %x\n", add);
  }
  else if (pid > 0)
  {
    pte_t *pte = walkpagedir(proc->pgdir, va, 0);
    uint add = PTE_ADDR(*pte);
    printf(1, "Parent page pysical address before: %x\n", add);

    int id = wait();

    pte_t *pte = walkpagedir(proc->pgdir, va, 0);
    add = PTE_ADDR(*pte);
    printf(1, "Parent page pysical address after: %x\n", add);
    
  }
  else 
  {
    printf(1, "Error forking");
  }


  exit();
}


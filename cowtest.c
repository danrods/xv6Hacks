#include "types.h"
#include "defs.h"
#include "mmu.h"
#include "proc.h"

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
    pte_t *pte = walkpgdir(proc->pgdir, va, 0);
    uint add = PTE_ADDR(*pte);
    printf("Child page pysical address before: %d\n", add);
  }
  else if (pid > 0)
  {
    pte_t *pte = walkpgdir(proc->pgdir, va, 0);
    uint add = PTE_ADDR(*pte);
    printf("Parent page pysical address before: %d\n", add);
  }
  else 
  {
    printf(1, "Error forking");
  }

  //After writing
  va = "Hi";

  if (pid == 0)
  {
    pte_t *pte = walkpgdir(proc->pgdir, va, 0);
    uint add = PTE_ADDR(*pte);
    printf("Child page pysical address after: %d\n", add);
  }
  else if (pid > 0)
  {
    pte_t *pte = walkpgdir(proc->pgdir, va, 0);
    uint add = PTE_ADDR(*pte);
    printf("Parent page pysical address after: %d\n", add);
  }
  else 
  {
    printf(1, "Error forking");
  }
}


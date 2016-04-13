#include "types.h"
#include "defs.h"

/*
void cowpatest(void);
void cowrefcountinctest(void);
void cowrefcountdifftest(void);
void cowchildmemfreedtest(void);

int
main(void)
{
 // cowpatest();
  exit();
  return 0;
}

//pysical address is expected to be different 
//after COW
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

//the ref_count should be incremented after fork
void 
cowrefcountinctest()
{
  char* va = "Hey there!";
  void* page = (void*) PGROUNDDOWN (va);

  int ref_count = getRefCount(page);
  cprintf("Page ref count before fork: %d\n", ref_count);

  fork();

  ref_count = getRefCount(page);
  cprintf("Page ref count after fork: %d\n", ref_count);

  exit();
}

//the parent and child pages' ref_counts should
//be different
void
cowrefcountdifftest() {
  int pid;
  char* va = "Good Evening!";
  void* page = (void*) PGROUNDDOWN(va);

  pid = fork();

  if (pid < 0) {
    cprintf("Error forking!");
  }
  else if (pid == 0) {
    int ref_count_child = getRefCount(page);
    cprintf("Child page ref count before: %d\n", ref_count_child);

    *va = "Good Day!";

    ref_count_child = getRefCount(page);
    cprintf("Child page ref count after: %d\n", ref_count_child);
  }
  else
  {
    int ref_count_parent = getRefCount(page);
    cprintf("Parent page ref count before: %d\n", ref_count_parent);

    wait();

    ref_count_parent = getRefCount(page);
    cprintf("Parent page ref count after: %d\n", ref_count_parent);
  }

  exit();
}

//when the child process is killed by the parent,
//its memory should be freed
void
cowchildmemfreedtest() {
  int pid;
  struct kmem *mem;
  struct run *r;
  char* va = "Hey OS!";
  void* page = (void*) PGROUNDDOWN(va);

  pid = fork();

  if (pid < 0) {
    cprintf("Error forking!");
  }
  else if (pid == 0)
  {
    va = "Hey Programmer!";
  }
  else
  {
    wait();
    if (proc->killed == 1)
    {
      if (r->next == mem.freelist)
      {
        cprintf("Child page space freed.");
      }
      else {
        cprintf("Child page space NOT freed!");
      }
    }
    else {
      cprintf("Child process not killed!");
    }
  }

  exit();
}
*/
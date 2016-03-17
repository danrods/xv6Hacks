// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "types.h"
#include "stat.h"
#include "user.h"
#include "defs.h"
#include "mmu.h"



#define N  1000

void
printf(int fd, char *s, ...)
{
  write(fd, s, strlen(s));
}

void
forktest(void)
{
  int n, pid;

  printf(1, "fork test\n");

  for(n=0; n<N; n++){
    pid = fork();
    if(pid < 0)
      break;
    if(pid == 0)
      exit();
  }
  
  if(n == N){
    printf(1, "fork claimed to work N times!\n", N);
    exit();
  }
  
  for(; n > 0; n--){
    if(wait() < 0){
      printf(1, "wait stopped early\n");
      exit();
    }
  }
  
  if(wait() != -1){
    printf(1, "wait got too many\n");
    exit();
  }
  
  printf(1, "fork test OK\n");
}

void 
cowrefcounttest() 
{
  pte_t* pte = (char*) 0x2789;
  int pa = PTE_ADDR(*pte);
  int ref_count = getRefCount(pa);
  cowuvm(*pte, 2);
  int ref_count_new = getRefCount(pa);
  if (ref_count_new > ref_count)
  {
    printf(1, "SUCCESS: reference count incremented after COW");
  }
  else 
  {
    printf(1, "FAILED: reference count not incremebted after COW");
  }
}


int
main(void)
{
  forktest();
  cowrefcounttest();
  exit();
}

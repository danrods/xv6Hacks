// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

#define MAXPAGES (PHYSTOP / PGSIZE)

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file

struct run {
  int ref_count;
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;

  // DEP: For COW fork, we can't store the run in the 
  //      physical page, because we need space for the ref
  //      count.  Move to the kmem struct.
  struct run runs[MAXPAGES];
} kmem;


// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree_sys(p);
}

//PAGEBREAK: 21

static void
_kfree(char* v, int isStartup){


    struct run *r;

  if((uint)v % PGSIZE || v < end || v2p(v) >= PHYSTOP)
    panic("kfree");

    // Fill with junk to catch dangling refs.
    memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);

    r = &kmem.runs[(V2P(v) / PGSIZE)];
#ifndef original
    if(isStartup == 0){ //If we're not starting up
       if(r->ref_count){
        //cprintf("Decrementing Ref count for memory address %p\n", r->ref_count);
         r->ref_count--;
       }
       else{
         panic("ref_count < 1 when freeing");
       }
    }
    if(r-> ref_count == 0){
      //cprintf("Freeing memory address %p\n", v);
      r->next = kmem.freelist;
      kmem.freelist = r;
    }   
  
#else
    r->next = kmem.freelist;
    kmem.freelist = r;

#endif 

  if(kmem.use_lock)
    release(&kmem.lock);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  _kfree(v, 0); 
}

void kfree_sys(char* v){
   _kfree(v, 1);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;
  char *rv;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  #ifndef original
    r->ref_count++;
    //cprintf("Kalloc --> {R:%p\tRuns:%p}\n", r, kmem.runs);
  #endif
  if(kmem.use_lock)
    release(&kmem.lock);
  rv = P2V((r - kmem.runs) * PGSIZE);
  return rv;
}


int getRefCount(void* va){
  if(kmem.use_lock)
    acquire(&kmem.lock);
   
   int refCount = kmem.runs[(V2P(va) / PGSIZE)].ref_count;
   
  if(kmem.use_lock)
    release(&kmem.lock);
   //cprintf("Returning ref count : %d for %p\n", refCount, va);
   return refCount;
}

void
incRefCount(void* va){
  if(kmem.use_lock)
    acquire(&kmem.lock);
  kmem.runs[(V2P(va) / PGSIZE)].ref_count++;
  if(kmem.use_lock)
    release(&kmem.lock);
}

void
decRefCount(void* va){
  if(kmem.use_lock)
    acquire(&kmem.lock);
  kmem.runs[(V2P(va) / PGSIZE)].ref_count--;
  if(kmem.use_lock)
    release(&kmem.lock);
}

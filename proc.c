#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

struct {
  int totalTickets;
  int totalTicketHolders;
  struct spinlock lock;
  struct TicketHolder holders[NPROC];
} tickettable;

int totalTickets = 0;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);


static void wakeup1(void *chan);

int read_pointer = 0;
int write_pointer = 0;
int seeds[10];
int M = 16873;
uint random_number = 8;
int isLottery = 1;
//static void getseeds(uint *val);


TicketHolder* binarySearch(uint random, int start, int end);

#ifndef lottery

void updateTicketHolders(struct TicketHolder* holder); 
static int getTicketAmount(struct proc * proc);

#endif

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");

}


#ifdef lottery //TODO : Switch this over when want to use lottery officially

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;
  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;
  
  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;
  
  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

//  cprintf("Created new Process with pid : %d\n", p->pid);
  return p;
}

#else

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;
  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;
  
  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;
  
  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  p->nice = 120; //Default nice
  p->tickets = getTicketAmount(proc); //Give it the same amount of tickets as it's parent for now.
  totalTickets += p->tickets;  //Lets incrememnt that count son.

  //cprintf("Successfully Added a Holder to process %d with %d tickets\n", p->pid, t->totalTickets);
  
  return p;
}



#endif


#ifndef lottery

/**
* Convenience method to return the amount of tickets based on the nice value of the process.
*/
static
int getTicketAmount(struct proc * proc){

  if(NULL == proc) return 3; //shouldn't be null

  int returnVal = 3;//By Default Return the default value for 120

    if(proc->nice > 139){//Too High
        cprintf("Nice value is too high!");
        proc->nice = 120;
    }
    else if(proc->nice == 139){
        returnVal =  1;
    }
    else if(proc->nice > 129){ // 130 - 138
        returnVal =  2;
    }
    else if(proc->nice > 119){ // 120 - 129
        returnVal =  3;
    } 
    else if(proc->nice > 109){ // 110 - 119
        returnVal =  4;
    }
    else if(proc->nice > 100){ // 101 - 109
        returnVal =  5;
    }
    else if(proc->nice == 100){ 
        returnVal =  6;
    }
    else{ // Too Low
        cprintf("Nice value is too low!");
        proc->nice = 120;
    }

    return returnVal; 
}


/**
* Convenience method to update the runningTotal for the subsequent tickets after i.
* NOTE : Tickettable Lock should be acquired before calling this method.
*/
 
void updateTicketHolders(struct TicketHolder* holder){

    if(NULL == &tickettable.holders[NPROC] || 
        holder < tickettable.holders       ||
        holder > &tickettable.holders[NPROC]) return; // i must be bounded by the tickettable array. 

    int ticketHolders = tickettable.totalTicketHolders;
    int numTickets = tickettable.totalTickets;
    

    struct TicketHolder * last = &tickettable.holders[ticketHolders];
    memmove(holder, last, sizeof(struct TicketHolder)); //Copy the last one to the newly created free spot
    last->status = AVAILABLE;
    last->proc = NULL;

    //Update the runningTotal of the one we copied
    holder->runningTotal = (holder > tickettable.holders) ? (((holder - 1)->runningTotal) + holder->totalTickets) : holder->totalTickets; 
    //holder->proc->stub = holder; //Make sure the process is updated

    struct TicketHolder *t1 = holder, *t2;
    for(t2 = t1 + 1;  //First pointer is at the initialized ticket, Second pointer is at the one after
        t2 < &tickettable.holders[ticketHolders]; //While the 1st & 2nd pointers are less than the second to last and last, continue
        t1++, t2++){ 

        if(t2->status == AVAILABLE){
            panic("No holes in ticket holder array allowed\n");
        }

        t2->runningTotal = t1->runningTotal + t2->totalTickets;
    }

    tickettable.totalTickets -=  numTickets;
    tickettable.totalTicketHolders--;
}

#endif

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];
  
  p = allocproc();
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  p->state = RUNNABLE;
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  
  sz = proc->sz;
  if(n > 0){
    if((sz = allocuvm(proc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(proc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  proc->sz = sz;
  switchuvm(proc);
  return 0;
}





#ifdef original


// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;

  // Allocate process.
  if((np = allocproc()) == 0)
    return -1;

  // Copy process state from p.
  if((np->pgdir = copyuvm(proc->pgdir, proc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = proc->sz;
  np->parent = proc;
  *np->tf = *proc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(proc->ofile[i])
      np->ofile[i] = filedup(proc->ofile[i]);
  np->cwd = idup(proc->cwd);

  safestrcpy(np->name, proc->name, sizeof(proc->name));
 
  pid = np->pid;


  // lock to force the compiler to emit the np->state write last.
  acquire(&ptable.lock);
  np->state = RUNNABLE;
  release(&ptable.lock);

  cprintf("Successfully Forked Process with PID : %d\n", pid);
  
  return pid;
}


#else

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;

  cprintf("FORK --> Started!\n");

  // Allocate process.
  if((np = allocproc()) == 0)
    return -1;
  cprintf("FORK --> Allocated!\n");

  // Copy process state from p.
  if((np->pgdir = cowuvm(proc->pgdir, proc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }

  cprintf("FORK --> COWUVM!\n");

  np->sz = proc->sz;
  np->parent = proc;
  *np->tf = *proc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(proc->ofile[i])
      np->ofile[i] = filedup(proc->ofile[i]);
  np->cwd = idup(proc->cwd);

  cprintf("FORK --> File Copy!\n");

  safestrcpy(np->name, proc->name, sizeof(proc->name));
 
  pid = np->pid;

  // lock to force the compiler to emit the np->state write last.
  acquire(&ptable.lock);
  np->state = RUNNABLE;
  release(&ptable.lock);
  
  cprintf("FORK --> Done!\n");
  return pid;
}


#endif

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  //cprintf("Exiting the process : {PID:%d, Name:%s, Killed:%d, Parent:%d, Size:%d}\n", proc->pid, proc->name, proc->killed, proc->parent->pid, proc->sz);
  
  struct proc *p;
  int fd;

  if(proc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(proc->ofile[fd]){
      fileclose(proc->ofile[fd]);
      proc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(proc->cwd);
  end_op();
  proc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(proc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == proc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  // Jump into the scheduler, never to return.
 
  cprintf("Exiting Process --> [%d:%s].  Proc State : %d\n", proc->pid, proc->name, proc->state);
   proc->state = ZOMBIE;
 
  #ifndef lottery
    totalTickets -= proc->tickets;
  #endif
  
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;

  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for zombie children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != proc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        //cprintf("Found Zombie : {PID:%d, Name:%s, Killed:%d, Parent:%d, Size:%d, Pgdir:0x%x}\n", proc->pid, proc->name, proc->killed, proc->parent->pid, proc->sz, proc->pgdir);
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->state = UNUSED;
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || proc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(proc, &ptable.lock);  //DOC: wait-sleep
  }
}


/**
* Method that will perform binary search to find which process should be running.
* Random is a randomly generated number between 0 and the number of tickets that exist 
* in the system. Searches from start - end, will do work recursively
*/ 
TicketHolder* binarySearch(uint random, int start, int end){
     
     //cprintf("Binary Search --> {Random: %d, Start: %d, End: %d\n", random, start, end);
     if(start > end) return NULL; // While start <= end continue
     
    int mid = (start + end) / 2;

    
    int ticketStart = (&tickettable.holders[mid])->runningTotal - (&tickettable.holders[mid])->totalTickets;
    int ticketEnd = tickettable.holders[mid].runningTotal;

    //cprintf("\tMiddle Ticket --> {TicketStart: %d, lastTicket: %d}\n", ticketStart, ticketEnd);

    //Is the random number bound by the current TicketHolder 
    if((ticketEnd >= random) && (ticketStart <= random) ){ 
        //struct proc* winner = (&tickettable.holders[mid])->proc;
        //cprintf("\tFound Process --> {Name : %s, Nice Val: %d, PID: %d, killed %d}\n", winner->name, winner->nice, winner->pid, winner->killed);
        return &tickettable.holders[mid];
    }
    else if(ticketEnd < random ){ // It's bigger
        //cprintf("\tRecursive Bigger\n");
        return binarySearch(random, mid + 1, end);
    }
    else if(ticketStart > random ) { // It's smaller
        //cprintf("\tRecursive Smaller\n");
        return binarySearch(random, start, mid);
    }
    else{
      //cprintf("It's not bigger than or less than and not bounded by. ERR!\n");
    }

    return NULL;
}


#ifdef lottery

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p;

  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      proc = p;
      switchuvm(p);
      p->state = RUNNING;
      swtch(&cpu->scheduler, proc->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      proc = 0;
    }
    release(&ptable.lock);
  }
}


#else

void scheduler_rr(void);
void scheduler_lottery(void);

void
scheduler(void)
{

  for(;;){
      if(isLottery){
        cprintf("Starting Lottery Scheduler!\n");
        scheduler_lottery();
      }
      else{
        cprintf("Starting Round Robin!\n");
        scheduler_rr();
      }
  }

}


//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler_rr(void)
{
  struct proc *p;

  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      proc = p;
      switchuvm(p);
      p->state = RUNNING;
      swtch(&cpu->scheduler, proc->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      proc = 0;
    }
    release(&ptable.lock);

    if(isLottery){ //If we switched to lottery, break out of the loop.
        break;
    }
  }
}


// NEW ONE
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler_lottery(void)
{
  struct proc *p, *winner = NULL;
  //struct TicketHolder* t;
 // int isFound;
  uint random;
  int runningTotal;

  for(;;){
    // Enable interrupts on this processor.
    sti();

    acquire(&ptable.lock);

      random = prng(totalTickets);                //Step 1. Get a Random number

      winner = NULL;
      runningTotal = 0;
      for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
        if(p->state != RUNNABLE){
          continue;
        }

        runningTotal += getTicketAmount(p);

        if(runningTotal > random){
            winner = p;
            break;
        }

      }

      if(winner){ // If we found a winner

          if(winner->pid > 2){
              //cprintf("Winner on CPU : %d! --> Found Ticket : { Name : %s\t Tickets : %d\t PID: %d\t Parent PID :%p \t Killed : %d \t Nice: %d\t PDIR: %p\t State: %d\n}\n", 
              //cpu->id, winner->name, winner->tickets, winner->pid, winner->parent, winner->killed, winner->nice, winner->pgdir, winner->state);
          }

          // Switch to chosen process.  It is the process's job
          // to release ptable.lock and then reacquire it
          // before jumping back to us.
          proc = winner;
          switchuvm(winner);
          winner->state = RUNNING;
          swtch(&cpu->scheduler, proc->context);
          switchkvm();

          // Process is done running for now.
          // It should have changed its p->state before coming back.
          proc = 0;        
      }

    release(&ptable.lock);


    if(isLottery == 0){ //If we switched to RR, break out of the loop.
        break;
    }

  }
}

#endif


// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state.
void
sched(void)
{
  int intena;

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(cpu->ncli != 1)
    panic("sched locks");
  if(proc->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = cpu->intena;
  swtch(&proc->context, cpu->scheduler);
  cpu->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  proc->state = RUNNABLE;
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot 
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }
  
  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  if(proc == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }

  // Go to sleep.
  proc->chan = chan;
  proc->state = SLEEPING;
  sched();

  // Tidy up.
  proc->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];
  
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

void
ticketdump(void){

  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };

  struct proc *p;
  char *state;

  cprintf("Total Tickets : ---> %d\n", totalTickets);
  
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;

    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";

    cprintf("Running Process on CPU : %d -> {PID: %d, Name: %s, State: %s, Killed: %d, Nice-Value: %d, Tickets: %d}\n", 
                                        cpu->id, p->pid, p->name, state, p->killed, p->nice, p->tickets);
  }
}


#ifndef lottery

int updateNice(int nice, struct proc* p){

    int amount;

    p->nice = 120 + nice;

    if((amount = getTicketAmount(proc)) < 0) return -1;

    totalTickets -= p->tickets;
    totalTickets += (p->tickets = amount);

    return 0;
}

#endif



// Title: Blum Blum Shub Generator
// Availability: https://en.wikipedia.org/wiki/Blum_Blum_Shub
uint 
prng(uint upper) {
  uint x = 0;

  if(upper){
    random_number = random_number * random_number;
    random_number = random_number % M;
    x = random_number % upper ;
    //cprintf("random: %d; x: %d \n", random_number, x);
  }
  
  return x;
}


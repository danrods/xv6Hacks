xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern x86-based multiprocessor using ANSI C.

# ACKNOWLEDGMENTS

xv6 is inspired by John Lions's Commentary on UNIX 6th Edition (Peer
to Peer Communications; ISBN: 1-57398-013-7; 1st edition (June 14,
2000)). See also http://pdos.csail.mit.edu/6.828/2014/xv6.html, which
provides pointers to on-line resources for v6.

xv6 borrows code from the following sources:
    JOS (asm.h, elf.h, mmu.h, bootasm.S, ide.c, console.c, and others)
    Plan 9 (entryother.S, mp.h, mp.c, lapic.c)
    FreeBSD (ioapic.c)
    NetBSD (console.c)

The following people have made contributions:
    * Russ Cox (context switching, locking)
    * Cliff Frey (MP)
    * Xiao Yu (MP)
    * Nickolai Zeldovich
    * Austin Clements
    * **Danny Rodrigues**

In addition, we are grateful for the bug reports and patches contributed by
Silas Boyd-Wickizer, Peter Froehlich, Shivam Handa, Anders Kaseorg, Eddie
Kohler, Yandong Mao, Hitoshi Mitake, Carmi Merimovich, Joel Nider, Greg Price,
Eldar Sehayek, Yongming Shen, Stephen Tu, and Zouchangwei.

The code in the files that constitute xv6 is
Copyright 2006-2014 Frans Kaashoek, Robert Morris, and Russ Cox.

# ERROR REPORTS

If you spot errors or have suggestions for improvement, please send
email to Frans Kaashoek and Robert Morris (kaashoek,rtm@csail.mit.edu). 

# BUILDING AND RUNNING XV6

To build xv6 on an x86 ELF machine (like Linux or FreeBSD), run "make".
On non-x86 or non-ELF machines (like OS X, even on x86), you will
need to install a cross-compiler gcc suite capable of producing x86 ELF
binaries.  See http://pdos.csail.mit.edu/6.828/2014/tools.html.
Then run "make TOOLPREFIX=i386-jos-elf-".

To run xv6, install the QEMU PC simulators.  To run in QEMU, run "make qemu".

To create a typeset version of the code, run "make xv6.pdf".  This
requires the "mpage" utility.  See http://www.mesa.nl/pub/mpage/.

================

# Modifications

### Part 1 :
Added new syscall called wolfietest which displays an image of wolfie

### Part 2 : 
Implement Copy-On-Write Fork to replace xv6's Copy on Fork implemention.
Note: This part is still a Work-in-progress, by default compile with `SPECIAL_FLAG = -Doriginal` to continue using
the original Copy on fork implementation

### Part 3 :
Implement Lottery Scheduler to replace xv6's default Round Robin Scheduler. In addition, a syscall was created to dynamically
switch between the two schedulers called **switchScheduler** which takes no parameters. This effect can be seen by running the program
`schedTest` directly from the shell after OS startup.

### Part 4 : 
Implement BSD's Fast File System to replace the default static location of data blocks and iNodes by doing the following :

1. Seperate the Disk Image into cylinder or block groups by moving the static iNodes, free block bitmap and data block sections into
    the same block group. iNodes are evenly divided between the number of blockgroups, and the number of data blocks in a blockgroup is dependent on how many iNodes are in that group.  NOTE : by default there are 12 direct blocks and 1 indirect block per iNode

2. New File iNodes are put in the same block group as other files in the same folder if possible.
3. New folders are put into their own block groups if possible.
4. Large File exception are implemented to split chunks of a larger file into different block groups to save space in block groups.
    **NOTE : This is still a work in progress**

================




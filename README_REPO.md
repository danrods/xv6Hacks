Repository for Celeste Ma (Qima) and Daniel Rodrigues (drodrigues)

Lab 1 : System calls

Ascii art generated from site http://picascii.com/

Pic : http://2.bp.blogspot.com/_NxDpePvRIus/S_6pEo6bpwI/AAAAAAAAAi8/HJj8rQPDpEA/s1600/StonyBrookUniv.jpg

________________________________________________

Lab 2: Copy-on-Write Fork

Implement copy-on-write fork() method when a user tries to write 
to a child process. A page fault interrupt will signal then copy 
and allocate new space for the child process to write.

TESTS

When fork() takes place, the page reference count is expected to
increment. Check the reference count before and after fork().

When copy-on-write to a child process, the parent and child processes
have the same virtual addresses but different physical addresses. 
Compare the physical addresses before and after copy-on-writel

When copy-on-write to a child process, a new page is allocted to the
child. The reference count of the parent page is expected to decrement.


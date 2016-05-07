struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe;
  struct inode *ip;
  uint off;
};

#ifdef ffs
// in-memory copy of an inode
struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count of # processes using inode
  int flags;          // I_BUSY, I_VALID

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;        // Number of hard links on disk
  uint size;
  uint addrs[NDIRECT+1];
};
#else
// new in-memory copy of an inode
struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  uint bgn;           // Block Group Number
  int ref;            // Reference count of # processes using inode
  int flags;          // I_BUSY, I_VALID

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;        // Number of hard links on disk
  uint size;
  uint addrs[NDIRECT+1];
};
#endif

#define I_BUSY 0x1
#define I_VALID 0x2

// table mapping major device number to
// device functions
struct devsw {
  int (*read)(struct inode*, char*, int);
  int (*write)(struct inode*, char*, int);
};

extern struct devsw devsw[];

#define CONSOLE 1

//PAGEBREAK!
// Blank page.

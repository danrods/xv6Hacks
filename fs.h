// On-disk file system format. 
// Both the kernel and user programs use this header file.


#define ROOTINO 1  // root i-number
#define BSIZE 512  // block size

// Disk layout:
// [ boot block | super block | log | inode blocks | free bit map | data blocks ]
//
// mkfs computes the super block and builds an initial file system. The super describes
// the disk layout:

#ifdef xv6ffs

struct superblock {
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
  uint nlog;         // Number of log blocks
  uint logstart;     // Block number of first log block
  uint inodestart;   // Block number of first inode block
  uint bmapstart;    // Block number of first free map block
};

#else
struct superblock {
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
  uint nlog;         // Number of log blocks
  uint logstart;     // Block number of first log block
  uint bgstart;      // The start of the block groups
  uint nblockgroups;  // Total number of block groups in the fs
  uint bpbg;         // Number of blocks per Block Group
  uint ipbg;         // Number of iNodes per Block Group
};

#endif

#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

// On-disk inode structure
struct dinode {
  short type;           // File type
  short major;          // Major device number (T_DEV only)
  short minor;          // Minor device number (T_DEV only)
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  uint addrs[NDIRECT+1];   // Data block addresses
};


// Inodes per block.
#define IPB           (BSIZE / sizeof(struct dinode))


#ifdef xv6ffs

  // Bitmap bits per block
  #define BPB           (BSIZE*8)

  // size of file system in blocks
  #define FSSIZE       1000  
  
  // Block containing inode i
  #define IBLOCK(i, sb)     ((i) / IPB + sb.inodestart)

  // Block of free map containing bit for block b
  #define BBLOCK(b, sb)     (b/BPB + sb.bmapstart)


  #define DINODEOFFSET(inum, sb) (inum % IPB)
  
#else

  void printFSStats(void);
  void clearFSStats(void);
  void fillFSStats(void);
  ///////////////////// Constants //////////////////////

  //Arbitarily chosen
  #define BLOCKGROUPS 25

  // Bitmap bits per block
  #define BITSPERBLOCK           (BSIZE*8)

   // NOTE : 36 Bits padding  = 4.5 Bytes padding = 1 uint
  // Unaligned : (4096 [Data Blocks] % 140 [Blocks / iNode] = 36 Bits padding)
  // Total Data Blocks = 4096 - 36 = 4060 Blocks: 
  #define NDATABLOCKS (BITSPERBLOCK - (BITSPERBLOCK % MAXFILE)) 


  // The number of Data bits per Block Group Bitmap is defined to be the same as NDATABLOCKS
  #define BPB        NDATABLOCKS

  // Num iNodes per Block Group :
  //    4060 [Data Blocks / Block Group] / 140 [Data Blocks / iNode]
  //    ==> 4060 [Data Blocks / Block Group] * 1/140 [iNodes / Data Block] 
  //    ======> 29 [iNodes / Block Group]
  #define IPBG        (NDATABLOCKS / MAXFILE)

  // Total iNode Blocks : 29 [iNodes / Block Group]  /  8 [iNodes / Block] = 3 [blocks / Block group] + 1 [Round Up]   
  #define NINODEBLOCKS  ( (IPBG / IPB) + 1)

  // Total Blocks per Block group ==> # of iNode Blocks + # of Data Blocks + 1 Free Data Block
  //  ==> 4 iNode Blocks + 4060 Data Blocks + 1 Data BitMap Block  ==> 4065 Blocks
  #define BPBG        (NINODEBLOCKS + NDATABLOCKS + 1)

  // Total FS Size :
  //  ==> X [Block Groups] * 4065 [Blocks / Block Group] = 4065X [Blocks] + 30 [Log Blocks] + 1 [Super Block] + 1 [Boot Block]
  //    ===> 4065X + 32 [Total Blocks] --> Ex :  (4065 * 25) + 32 = 101,657 Data Blocks
  #define FSSIZE      ((BLOCKGROUPS * BPBG) + LOGSIZE + 2)


  // Total Number of Blocks between the start of a BG and The number of Datablocks is the Number of iNode blocks
  #define DATABLOCK_OFF NINODEBLOCKS

  /////////////////////////////////////////////////////////////////////////////////

  struct ff_stats{
      uint usedBlocks;
      uint percentFull;
  };


  //////////////////// Relative Macros /////////////////////////

  #define BGROUP(i, sb) ((i) / sb.ipbg)

  //Offset in a block of dInodes
  #define DINODEOFFSET(inum, sb) ((inum % sb.ipbg) % IPB)

  // Step 1 : Get the Block No. for the start of the block group based on the iNode Number 
  // Step 2 : Calculate the offset into number of iNodes for block group (i % sb.ibpg)
  // Step 3 : Get the block number based on the relative iNode offset ( Divide by IPB)
  // Step 4 : Combine all to get the Actual block number
  // Step 5 : Add 1 Because the First block is the Free Block Data Map
  // Step 6 : Add the start of the block groups
  #define IBLOCK(i, sb)    (  (((i) / sb.ipbg) * sb.bpbg) + (((i) % sb.ipbg) / IPB) + 1 + sb.bgstart)

  // Step 1 : Figure out which Block Group we are based on the block no.
  // Step 2 : Block Group no. * Number of Blocks per Block group yields block no. for start of bg
  // Step 3 : Free Data Bitmap is first block in Block group so no need to offset
  #define BBLOCK(b, sb)    ( (((b)/BPB) * sb.bpbg) + sb.bgstart ) 


  // Step 1 : Figure out which Block group we are in, based on the block no.
  // Step 2 : Add the Number of iNode Blocks and the 1 block for the data bitmap to arrive at data section
  // Step 3 : Add the remainder of the block no, which is the offset to the data blocks in the corresponding block group
  #define DBLOCK(b, sb)    ( ( ((b)/BPB) * sb.bpbg) + ((b) % BPB) + NINODEBLOCKS + 1 + sb.bgstart) 


  // Since we only use 29 iNodes per block to divide evenly with the num data blocks
  // We have 3 * sizeof(dinode) [64 Bytes]  ==> 192 Bytes of padding. Lets use bottom one for stats
  //  [Data Bit Map | iBlock 1 | iBlock 2 | iBlock 3 | {diNode1, diNode2...,diNode 5} {~192 Free Bytes | Stats block }  | Data Blocks] 
  #define STATBLOCK(bg, sb)   (sb.bgstart +  (bg * sb.bpbg) + NINODEBLOCKS )

  #define STATOFF(buf) (buf->data + (BSIZE - sizeof(struct ff_stats)))
  /////////////////////////////////////////////////////////////


  

#endif




// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent {
  ushort inum;
  char name[DIRSIZ];
};

#define func_enter() cprintf("\n=========== File: %s | Function: %s | Line number: %d ===========\n", __FILE__, __func__, __LINE__)

#define func_exit(fmt, ...) cprintf("Returning Args: \n");cprintf(fmt, ##__VA_ARGS__); cprintf("{{{{{{{{{{{{{{{{{{{{{{ END Function: %s }}}}}}}}}}}}}}}}}}}}}}}}\n\n", __func__)

#define fs_debug(fmt, ...) cprintf("[Debug] --->"); cprintf(fmt, ##__VA_ARGS__)
#define fs_error(fmt, ...) cprintf("~~~~~~~[Error] --->"); cprintf(fmt, ##__VA_ARGS__)
#define iNode_info(i) fs_debug("iNode : {Dev: %d, Type: %d, iNum: %d, Ref Count: %d, Flags: %d, Size: %d}\n", i->dev, i->type, i->inum, i->ref, i->flags, i->size)
#define buf_info(b) fs_debug("Buf : {Dev: %d, BlockNo: %d, Flags: %d, Prev: %d, Next: %d}\n", b->dev, b->blockno, b->flags, b->prev->blockno, b->next->blockno)
#define ffStats_info(stat) fs_debug("FF_STAT : {Used Blocks: %d, Percent Full: %d}\n", stat->usedBlocks, stat->percentFull)

#define diNode_info(d) do{                                                                                                                                \
                          cprintf("{Type: %d, Major: %d, Minor: %d, Link: %d, Size: %d, Addrs:[", d->type, d->major, d->minor, d->nlink, d->size);         \
                          int i;                                                                                                                          \
                          for(i=0; i < NDIRECT; i++){                                                                                                     \
                               if(d->addrs[i]) cprintf("%d,",i, d->addrs[i]);                                                          \
                          }                                                                                                                               \
                          if(d->addrs[NDIRECT]) cprintf("Indirect Address => %d", d->addrs[NDIRECT]);                                                   \
                          cprintf("] }\n");                                                                                                               \
                          }while(0)                                                                                                       










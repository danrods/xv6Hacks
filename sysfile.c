//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "file.h"
#include "fcntl.h"


char* wolfie_img = "                     .........................              \n"\
"                     ...........................            \n"\
"                   ..........,.,,,,,,,..............        \n"\
"                   .......~8ZO+,,,:I$$?:,,,...........      \n"\
"                   .......DD8ZZZ7$$$$$~+$=,,,..........     \n"\
"                  ......,,8DDDDD8$888D88Z7$,,,,.........    \n"\
"                  ......,,8DDDNNDDDDDDDDDD88OI,,........    \n"\
"               ........,,,=DNNNNMMONMNDDDD88888OZ?......    \n"\
"              ........,,,:?NI??IN8$$MMMMNM8OOOZZ?,......    \n"\
"             ........,,:O7ZDDDII8D$$NMMMN.OZO8I,,.......    \n"\
"             .......,?$ZOOO$$8DNM$NDDDD8O?7.$,,,,.......    \n"\
"             ......,ZZ$ZOZOOZZOODNDDDDD88O?=$8,,,,......    \n"\
"            .......NOZZZONN88D88?DDDDND??Z8ZDZ$:,,......    \n"\
"            .......8MNND88DN88D?+7NZ$DNNDD=I8?:~,,......    \n"\
"            .......,,8DDDDDDDD$D?IZI+=?DND~:8=~=,,......    \n"\
"            .......,,,,,8N8DD7?NMIII?+~~,,,,.I7?I.......    \n"\
"          ........,,,,,:::NNO7?MMN7II+??I~..~Z7I=.......    \n"\
"         .........,,,ZOOO8DMZ$I7MN8DDNMM?~=:++OO,.......    \n"\
"      ..........,,:ZZZZZZOOO88I?MM.ZDOMN,Z$~:,,,,.......    \n"\
"    ...........,,$$Z$ZZZZZZOO8$??+.,.,:.=Z$7$:,,,.......    \n"\
"    ........,,,$$ZZZ$ZZZZZZZOO8$I?+=====:77II77,,,......    \n"\
"   .......,,,$Z$$$ZO8ZZZZZZZZOOOO$77I=~:,III??II,,.......   \n"\
"  .......,,?$$$$$$ZOOOZZZZZZZOOOZ$$7$Z$$7II????7=,,........ \n"\
" .......,7$$$$$$$$$$OOOOZZZZZZZ$$77:.~$77II?++?$?+,,.......  \n"\
" ......,OMDZ$$$$$$$ZO8OOOZZZZZ$$$7777$IIIIII+7?$I?+,,......  \n"\
" ......,,8DNN$$$$$ZZDO8OOZZZZI::7?.$$+?I7III?$I77+++=,.......\n"\
" .....,,+DNDDNO$$$ZDDD88OOZOI,.:$Z7$$=~.7$,I7IZ77??I?,,......\n"\
" .....,,ODNDDDDM$$8DD88OOO8ZO$$$ZI$I:77II7$=I88Z$IIIZ,,......\n"\
" .....,+DDDD8DD8N::DDD8O8DOOO$$Z$I7..7III????88OZ7N8Z,,......\n"\
" .....,ZDO88OD8O:::?DD888OOOO$$Z$7?7.I7II??I::88ND88OI,,.....\n"\
" .....,$D888888=::::D8DDO8OOO$ZZ$I??.IIII?7~:::8DDD8OZ~,.....\n"\
" .....,=8DD8Z87,,,,,8DD888OOZ$ZZ$II=.II7?I+,,,,,8D88OZ?,.....\n"\
" .....,,8NDD87,,,,,,ID888OOOZ$ZZZI7...?7I?=,,,,,OOZ8Z7,......\n"\
" .....,,ND8DDO,,,,,,,88888OOZZZOZ77..=I?II~,,,,,:ZZOO7,,.....\n"\
" ......,8D8DOOI,,,,,,88888OOZZOOZ7?I??I7?I,,,,,,,O8$O7,......\n"\
" ......,IDD8DD8:,,,,,88OOOOOZZOOZ$I??I?III,,,,,,,ZDD$I,......\n"\
" ......,,D8888DD?,,,,OOOOOOOZZOOZ$7???I?I?,,,,,,,+ND7?,......\n"\
" .......,O8ODDODZ,,,,:OOOOZZZZOOZ$7IIII???,,,,,,,,DZ?=.......\n"\
"  ......,,OO8OOO$I,,,:O$7O8O8888OOZ$O77Z7I$,,,,,II?I~........\n"\
"  ........,?O88OZ?,,,=O7IZZ$$Z7O7ZO7ZIIII7I~,,,$7:~:.........\n"\
"    ............,,,,,~8IOZZ$Z$7$7Z7I7Z$?7II?,,,,...........  \n"\
"    ..............,,,,8IOOZ$Z7$777777II$7?I?,,,............  \n"\
"    ...............,,,ZIOOZ$Z7$777II77II77II~,,..........    \n"\
"      .............,,,77OOZZ$$$7777I$I77III??,,.........     \n"\
"            .......,,:IOOOZZZ$$7777IZ$7IIIII?,,......        \n"\
"             ......,,=?OOOZZZ$77777O$$$77III?,......         \n"\
"             ......,Z878OOZZZ$$$77I8Z$$$777II,,.....         \n"\
"             ......,878OOOOZZ$$$$7IDOZ$$7777,,......         \n"\
"             ......,,+O8OOOOZZ$$77O8OZ$$7777,,......         \n"\
"             .....,,,7OO8OOOZZ$$Z7D88Z$$$77?,,......         \n"\
"            ......,,+88O88OOZZ$$$ND88OOZ$777,.......         \n"\
"            ......,,+8O8Z8OO$OZ7$D8888OZ$$7,,......          \n"\
"            ......,+?OO88OOOZZO7IDN8OZ$$77,,.......          \n"\
"            ......,,8OZO8$87O$IINMMMN88$7~,,,.....           \n"\
"            ......,,DO8NN8ZOZ:::ZNNDOODI~,,,......           \n"\
"            ......,:8OZOZ8$==:::,DO88O?$Z,,,......           \n"\
"            .....,,~DNO88Z++I,,,::NDNDD88,,,,.........       \n"\
"            ......,,OOOZ$+ZN?:,,,~DD8OI??,,,,,........       \n"\
"            .....,,88O8OZ$O=,,,,,ZDD8O$??,,,,,,.........     \n"\
"            .....,,8888ZZOI:,,,,,ODD88OZZ,.I.I,.........     \n"\
"          .......,,OD=ZZ$7I,,,,,,8D8OOZ$$$7+I... ........    \n"\
"          ......,,,8~=OZ$I7:,,,,,8D88OZZ$$:..,. .,.......    \n"\
"          ......,,OZOI..7II~,,,,,DD88OOZZZ,,...  ........    \n"\
"          ......,~O=,.$.7=7$,,,,.,NN8888OI...............    \n"\
"          ......==,,.,.$:..$,,,...,,,,ON8~,.....8~.......    \n"\
"         ......,?+~,.,.....I,,........,,,,,,,,.,........     \n"\
"         .......I+~:,,..  .~,...........................     \n"\
"          ......I?+~:,,...............................       \n"\
"          .......M?+~:,,:8,.........................         \n"\
"          .......,,.,,,,,..........  .............           \n"\
"          .......................                            \n"\
"            ....................                             \n"\
"             .................  \n";


// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
static int
argfd(int n, int *pfd, struct file **pf)
{
  int fd;
  struct file *f;

  if(argint(n, &fd) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE || (f=proc->ofile[fd]) == 0)
    return -1;
  if(pfd)
    *pfd = fd;
  if(pf)
    *pf = f;
  return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int
fdalloc(struct file *f)
{
  int fd;

  for(fd = 0; fd < NOFILE; fd++){
    if(proc->ofile[fd] == 0){
      proc->ofile[fd] = f;
      return fd;
    }
  }
  return -1;
}

int
sys_dup(void)
{
  struct file *f;
  int fd;
  
  if(argfd(0, 0, &f) < 0)
    return -1;
  if((fd=fdalloc(f)) < 0)
    return -1;
  filedup(f);
  return fd;
}

int
sys_read(void)
{
  struct file *f;
  int n;
  char *p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
    return -1;
  return fileread(f, p, n);
}

int
sys_write(void)
{
  struct file *f;
  int n;
  char *p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
    return -1;
  return filewrite(f, p, n);
}

int
sys_close(void)
{
  int fd;
  struct file *f;
  
  if(argfd(0, &fd, &f) < 0)
    return -1;
  proc->ofile[fd] = 0;
  fileclose(f);
  return 0;
}

int
sys_fstat(void)
{
  struct file *f;
  struct stat *st;
  
  if(argfd(0, 0, &f) < 0 || argptr(1, (void*)&st, sizeof(*st)) < 0)
    return -1;
  return filestat(f, st);
}

// Create the path new as a link to the same inode as old.
int
sys_link(void)
{
  char name[DIRSIZ], *new, *old;
  struct inode *dp, *ip;

  if(argstr(0, &old) < 0 || argstr(1, &new) < 0)
    return -1;

  begin_op();
  if((ip = namei(old)) == 0){
    end_op();
    return -1;
  }

  ilock(ip);
  if(ip->type == T_DIR){
    iunlockput(ip);
    end_op();
    return -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  if((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  if(dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0){
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  return 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  return -1;
}

// Is the directory dp empty except for "." and ".." ?
static int
isdirempty(struct inode *dp)
{
  int off;
  struct dirent de;

  for(off=2*sizeof(de); off<dp->size; off+=sizeof(de)){
    if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
      panic("isdirempty: readi");
    if(de.inum != 0)
      return 0;
  }
  return 1;
}

//PAGEBREAK!
int
sys_unlink(void)
{
  struct inode *ip, *dp;
  struct dirent de;
  char name[DIRSIZ], *path;
  uint off;

  if(argstr(0, &path) < 0)
    return -1;

  begin_op();
  if((dp = nameiparent(path, name)) == 0){
    end_op();
    return -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  if(namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  if((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  if(ip->nlink < 1)
    panic("unlink: nlink < 1");
  if(ip->type == T_DIR && !isdirempty(ip)){
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, sizeof(de));
  if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
    panic("unlink: writei");
  if(ip->type == T_DIR){
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  return 0;

bad:
  iunlockput(dp);
  end_op();
  return -1;
}

static struct inode*
create(char *path, short type, short major, short minor)
{
  uint off;
  struct inode *ip, *dp;
  char name[DIRSIZ];

  if((dp = nameiparent(path, name)) == 0)
    return 0;
  ilock(dp);

  if((ip = dirlookup(dp, name, &off)) != 0){
    iunlockput(dp);
    ilock(ip);
    if(type == T_FILE && ip->type == T_FILE)
      return ip;
    iunlockput(ip);
    return 0;
  }

  if((ip = ialloc(dp->dev, type)) == 0)
    panic("create: ialloc");

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  if(type == T_DIR){  // Create . and .. entries.
    dp->nlink++;  // for ".."
    iupdate(dp);
    // No ip->nlink++ for ".": avoid cyclic ref count.
    if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      panic("create dots");
  }

  if(dirlink(dp, name, ip->inum) < 0)
    panic("create: dirlink");

  iunlockput(dp);

  return ip;
}

int
sys_open(void)
{
  char *path;
  int fd, omode;
  struct file *f;
  struct inode *ip;

  if(argstr(0, &path) < 0 || argint(1, &omode) < 0)
    return -1;

  begin_op();

  if(omode & O_CREATE){
    ip = create(path, T_FILE, 0, 0);
    if(ip == 0){
      end_op();
      return -1;
    }
  } else {
    if((ip = namei(path)) == 0){
      end_op();
      return -1;
    }
    ilock(ip);
    if(ip->type == T_DIR && omode != O_RDONLY){
      iunlockput(ip);
      end_op();
      return -1;
    }
  }

  if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
    if(f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    return -1;
  }
  iunlock(ip);
  end_op();

  f->type = FD_INODE;
  f->ip = ip;
  f->off = 0;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
  return fd;
}

int
sys_mkdir(void)
{
  char *path;
  struct inode *ip;

  begin_op();
  if(argstr(0, &path) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0){
    end_op();
    return -1;
  }
  iunlockput(ip);
  end_op();
  return 0;
}

int
sys_mknod(void)
{
  struct inode *ip;
  char *path;
  int len;
  int major, minor;
  
  begin_op();
  if((len=argstr(0, &path)) < 0 ||
     argint(1, &major) < 0 ||
     argint(2, &minor) < 0 ||
     (ip = create(path, T_DEV, major, minor)) == 0){
    end_op();
    return -1;
  }
  iunlockput(ip);
  end_op();
  return 0;
}

int
sys_chdir(void)
{
  char *path;
  struct inode *ip;

  begin_op();
  if(argstr(0, &path) < 0 || (ip = namei(path)) == 0){
    end_op();
    return -1;
  }
  ilock(ip);
  if(ip->type != T_DIR){
    iunlockput(ip);
    end_op();
    return -1;
  }
  iunlock(ip);
  iput(proc->cwd);
  end_op();
  proc->cwd = ip;
  return 0;
}

int
sys_exec(void)
{
  char *path, *argv[MAXARG];
  int i;
  uint uargv, uarg;

  if(argstr(0, &path) < 0 || argint(1, (int*)&uargv) < 0){
    return -1;
  }
  memset(argv, 0, sizeof(argv));
  for(i=0;; i++){
    if(i >= NELEM(argv))
      return -1;
    if(fetchint(uargv+4*i, (int*)&uarg) < 0)
      return -1;
    if(uarg == 0){
      argv[i] = 0;
      break;
    }
    if(fetchstr(uarg, &argv[i]) < 0)
      return -1;
  }
  return exec(path, argv);
}

int
sys_pipe(void)
{
  int *fd;
  struct file *rf, *wf;
  int fd0, fd1;

  if(argptr(0, (void*)&fd, 2*sizeof(fd[0])) < 0)
    return -1;
  if(pipealloc(&rf, &wf) < 0)
    return -1;
  fd0 = -1;
  if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
    if(fd0 >= 0)
      proc->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    return -1;
  }
  fd[0] = fd0;
  fd[1] = fd1;
  return 0;
}

int
sys_wolfie(void){

  char* buf;
  int size;
  
  if(argint(1, &size) < 0 || argptr(0, &buf, size) < 0){
    cprintf("Error getting arguments\n");
    return -1;
  }
  
  //Is the buffer null, is the size > 0, and is the size of wolfie bigger than size
  if(!buf || size <= 0 || size < (strlen(wolfie_img) + 1)){
    cprintf("Buffer size is too small for Wolfie!\n");
    return -1;
  } 
  return strlen(strncpy(buf, wolfie_img, size));//Returns the size of the string in buf
}

int 
sys_nice(void) {
  int value;

  if(argint(0, &value) < 0){
    cprintf("Error getting arguments\n");
    return -1;
  }

  if(value < -20 || value > 19) {
    cprintf("Set nice input between -20 and 19!\n");
    return proc->nice;
  }

  proc->nice += value;

  return proc->nice;
}

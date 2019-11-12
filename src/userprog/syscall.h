#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include <list.h>
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "filesys/directory.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "filesys/fsutil.h"
#include "filesys/inode.h"
#include "filesys/off_t.h"

struct fd_elem {
  int fd;
  struct file* file_ptr;
  struct list_elem elem;
};

void kill_process(void);

struct lock file_lock;

void syscall_init (void);

void sys_halt (struct intr_frame * f);
void sys_exit (struct intr_frame * f);
void sys_exec (struct intr_frame * f);
void sys_wait (struct intr_frame * f);
void sys_create (struct intr_frame * f);
void sys_remove (struct intr_frame * f);
void sys_open (struct intr_frame * f);
void sys_filesize (struct intr_frame * f);
void sys_read (struct intr_frame * f);
void sys_write (struct intr_frame * f);
void sys_seek (struct intr_frame * f);
void sys_tell (struct intr_frame * f);
void sys_close (struct intr_frame * f);

#endif /* userprog/syscall.h */

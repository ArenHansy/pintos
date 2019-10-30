#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <threads/vaddr.h>
#include <threads/synch.h>
#include <lib/user/syscall.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "pagedir.h"


struct lock filesys_lock;

static void syscall_handler (struct intr_frame *);
bool syscall_handler_file (struct intr_frame *, int system_call_num, int *argv);
int args_count(int system_call_num);
void parse_argv(int* esp, int* argv, int count);
void check_valid_address (void* vaddr);
void update_argv_page(int system_call_num, int* argv);

void
syscall_init (void)
{
  lock_init(&filesys_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
  check_valid_address(f->esp);
  int *esp = (int *)(f->esp);
  int system_call_num = *esp;
  int argv_count = args_count(system_call_num);
  int argv[argv_count];
  parse_argv(esp + 1, argv, argv_count);
  update_argv_page(system_call_num, argv);

  switch (system_call_num) {
    case SYS_HALT:
      halt();
      break;
    case SYS_EXIT:
      exit(argv[0]);
      break;
    case SYS_EXEC:
      break;
    case SYS_WAIT:
      break;
    case SYS_CREATE:
    case SYS_REMOVE:
    case SYS_OPEN:
    case SYS_FILESIZE:
    case SYS_READ:
    case SYS_WRITE:
    case SYS_SEEK:
    case SYS_TELL:
    case SYS_CLOSE:
      lock_acquire(&filesys_lock);
      bool success = syscall_handler_file(f, system_call_num, argv);
      lock_release(&filesys_lock);
      if (!success)
        exit(ERROR);
      break;
    default:
      break;
  }
}

int args_count(int system_call_num)
{
  switch (system_call_num) {
    case SYS_HALT:
      return 0;
    case SYS_EXIT:
      return 1;
    case SYS_EXEC:
      return 1;
    case SYS_WAIT:
      return 1;
    case SYS_CREATE:
      return 2;
    case SYS_REMOVE:
      return 1;
    case SYS_OPEN:
      return 1;
    case SYS_FILESIZE:
      return 1;
    case SYS_READ:
      return 3;
    case SYS_WRITE:
      return 3;
    case SYS_SEEK:
      return 2;
    case SYS_TELL:
      return 1;
    case SYS_CLOSE:
      return 1;
  }
  return 0;
}

void parse_argv(int* esp, int* argv, int count)
{
  for (int i = 0; i < count; i++)
  {
    int* ptr = esp + i;
    check_valid_address((void *) ptr);
    argv[i] = *ptr;
  }
}

void update_argv_page(int system_call_num, int* argv)
{
  uint32_t * pagedir = thread_current()->pagedir;
  switch (system_call_num) {
    case SYS_EXEC:
    case SYS_CREATE:
    case SYS_REMOVE:
    case SYS_OPEN:
    {
      const void * vaddr = (const void *) argv[0];
      int * paddr = pagedir_get_page(pagedir, vaddr);
      if (!paddr)
        exit(ERROR);
      argv[0] = (int) paddr;
      break;
    }
    case SYS_READ:
    case SYS_WRITE:
    {
      const void * vaddr = (const void *) argv[1];
      void * paddr = pagedir_get_page(pagedir, vaddr);
      if (!paddr)
        exit(ERROR);
      argv[1] = (int) paddr;
      break;
    }
  }
}

//

void halt (void)
{
  shutdown_power_off();
}

void exit (int status)
{
  struct thread *cur = thread_current();
  printf ("%s: exit(%d)\n", cur->name, status);
  thread_exit();
}

pid_t exec (const char *file);
int wait (pid_t);

//

bool
syscall_handler_file (struct intr_frame *f UNUSED, int system_call_num, int *argv)
{
  switch (system_call_num) {
    case SYS_CREATE:
      break;
    case SYS_REMOVE:
      break;
    case SYS_OPEN:
      break;
    case SYS_FILESIZE:
      break;
    case SYS_READ:
      break;
    case SYS_WRITE:
    {
      int fd = argv[0];
      void *buffer = argv[1];
      int size = argv[2];
      if (fd == STDOUT_FILENO)
      {
        putbuf(buffer, size);
      }
      f->eax = size;
      break;
    }
    case SYS_SEEK:
      break;
    case SYS_TELL:
      break;
    case SYS_CLOSE:
      break;
  }
  return true;
}


bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read (int fd, void *buffer, unsigned length);
int write (int fd, const void *buffer, unsigned length);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);

//

void check_valid_address(void* vaddr)
{
  if (is_user_vaddr(vaddr) && USER_VADDR_START <= vaddr)
    return;
  exit(ERROR);
}

#ifndef VM_FRAME_H
#define VM_FRAME_H

#include "threads/palloc.h"
#include "threads/synch.h"
#include "vm/page.h"
#include <stdbool.h>
#include <stdint.h>
#include <list.h>
#include <stdlib.h>


struct list frame_table;
struct lock ft_lock;

struct frame {
  void *kpage;
  void *upage;
  struct spte *spte;
  struct thread *thread;
  struct list_elem elem;
};


void* frame_alloc(enum palloc_flags flags, struct spte *spte);
void frame_free(void *kpage);
struct frame* frame_evict(enum palloc_flags flags);




#endif


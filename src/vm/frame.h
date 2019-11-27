#ifndef VM_FRAME_H
#define VM_FRAME_H

#include "threads/palloc.h"
#include "threads/synch.h"
#include "vm/page.h"
#include <stdbool.h>
#include <stdint.h>
#include <list.h>


struct list frame_table;

struct frame {
  void *kapge;
  void *upage;
  struct spte *spte;
  struct thread *thread;
  struct list_elem elem;
};

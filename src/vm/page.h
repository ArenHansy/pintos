#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <hash.h>
#include "vm/frame.h"

struct spte {
  void *upage;
  struct frame *frame;
  bool pin;
  bool writable;

  struct file *file;
  size_t offset;
  size_t read_bytes;
  size_t zero_bytes;

  int swap_index;
  
  struct hash_elem hash_elem;
};
